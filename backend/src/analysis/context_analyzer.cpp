/**
 * @file context_analyzer.cpp
 * @brief Implementation of context analysis for merge conflicts
 */

#include "wizardmerge/analysis/context_analyzer.h"
#include <algorithm>
#include <regex>

namespace wizardmerge {
namespace analysis {

namespace {

// Maximum number of lines to scan for imports (imports typically at file top)
constexpr size_t IMPORT_SCAN_LIMIT = 50;

/**
 * @brief Trim whitespace from string.
 */
std::string trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\n\r");
    size_t end = str.find_last_not_of(" \t\n\r");
    if (start == std::string::npos) return "";
    return str.substr(start, end - start + 1);
}

/**
 * @brief Check if a line is a function definition.
 */
bool is_function_definition(const std::string& line) {
    std::string trimmed = trim(line);
    
    // Common function patterns across languages
    std::vector<std::regex> patterns = {
        std::regex(R"(^\w+\s+\w+\s*\([^)]*\)\s*\{?)"),  // C/C++/Java: type name(params)
        std::regex(R"(^def\s+\w+\s*\([^)]*\):)"),       // Python: def name(params):
        std::regex(R"(^function\s+\w+\s*\([^)]*\))"),   // JavaScript: function name(params)
        std::regex(R"(^\w+\s*:\s*function\s*\([^)]*\))"), // JS object method
        std::regex(R"(^(public|private|protected)?\s*\w+\s+\w+\s*\([^)]*\))"), // Java/C# methods
        // TypeScript patterns
        std::regex(R"(^(export\s+)?(async\s+)?function\s+\w+)"), // TS: export/async function
        std::regex(R"(^(export\s+)?(const|let|var)\s+\w+\s*=\s*(async\s+)?\([^)]*\)\s*=>)"), // TS: arrow functions
        std::regex(R"(^(public|private|protected|readonly)?\s*\w+\s*\([^)]*\)\s*:\s*\w+)") // TS: typed methods
    };
    
    for (const auto& pattern : patterns) {
        if (std::regex_search(trimmed, pattern)) {
            return true;
        }
    }
    
    return false;
}

/**
 * @brief Extract function name from a function definition line.
 */
std::string get_function_name_from_line(const std::string& line) {
    std::string trimmed = trim(line);
    
    // Try to extract function name using regex
    std::smatch match;
    
    // Python: def function_name(
    std::regex py_pattern(R"(def\s+(\w+)\s*\()");
    if (std::regex_search(trimmed, match, py_pattern)) {
        return match[1].str();
    }
    
    // JavaScript/TypeScript: function function_name( or export function function_name(
    std::regex js_pattern(R"((?:export\s+)?(?:async\s+)?function\s+(\w+)\s*\()");
    if (std::regex_search(trimmed, match, js_pattern)) {
        return match[1].str();
    }
    
    // TypeScript: const/let/var function_name = (params) =>
    std::regex arrow_pattern(R"((?:const|let|var)\s+(\w+)\s*=\s*(?:async\s+)?\([^)]*\)\s*=>)");
    if (std::regex_search(trimmed, match, arrow_pattern)) {
        return match[1].str();
    }
    
    // C/C++/Java: type function_name(
    std::regex cpp_pattern(R"(\w+\s+(\w+)\s*\()");
    if (std::regex_search(trimmed, match, cpp_pattern)) {
        return match[1].str();
    }
    
    return "";
}

/**
 * @brief Check if a line is a class definition.
 */
bool is_class_definition(const std::string& line) {
    std::string trimmed = trim(line);
    
    std::vector<std::regex> patterns = {
        std::regex(R"(^class\s+\w+)"),                    // Python/C++/Java: class Name
        std::regex(R"(^(public|private)?\s*class\s+\w+)"), // Java/C#: visibility class Name
        std::regex(R"(^struct\s+\w+)"),                   // C/C++: struct Name
        // TypeScript patterns
        std::regex(R"(^(export\s+)?(abstract\s+)?class\s+\w+)"), // TS: export class Name
        std::regex(R"(^(export\s+)?interface\s+\w+)"),    // TS: interface Name
        std::regex(R"(^(export\s+)?type\s+\w+\s*=)"),     // TS: type Name =
        std::regex(R"(^(export\s+)?enum\s+\w+)")          // TS: enum Name
    };
    
    for (const auto& pattern : patterns) {
        if (std::regex_search(trimmed, pattern)) {
            return true;
        }
    }
    
    return false;
}

/**
 * @brief Extract class name from a class definition line.
 */
std::string get_class_name_from_line(const std::string& line) {
    std::string trimmed = trim(line);
    
    std::smatch match;
    
    // Match class, struct, interface, type, or enum
    std::regex pattern(R"((?:export\s+)?(?:abstract\s+)?(class|struct|interface|type|enum)\s+(\w+))");
    
    if (std::regex_search(trimmed, match, pattern)) {
        return match[2].str();
    }
    
    return "";
}

}  // anonymous namespace

CodeContext analyze_context(
    const std::vector<std::string>& lines,
    size_t start_line,
    size_t end_line,
    size_t context_window
) {
    CodeContext context;
    context.start_line = start_line;
    context.end_line = end_line;
    
    // Extract surrounding lines
    size_t window_start = (start_line >= context_window) ? (start_line - context_window) : 0;
    size_t window_end = std::min(end_line + context_window, lines.size());
    
    for (size_t i = window_start; i < window_end; ++i) {
        context.surrounding_lines.push_back(lines[i]);
    }
    
    // Extract function name
    context.function_name = extract_function_name(lines, start_line);
    
    // Extract class name
    context.class_name = extract_class_name(lines, start_line);
    
    // Extract imports
    context.imports = extract_imports(lines);
    
    // Add metadata
    context.metadata["context_window_start"] = std::to_string(window_start);
    context.metadata["context_window_end"] = std::to_string(window_end);
    context.metadata["total_lines"] = std::to_string(lines.size());
    
    return context;
}

std::string extract_function_name(
    const std::vector<std::string>& lines,
    size_t line_number
) {
    if (line_number >= lines.size()) {
        return "";
    }
    
    // Check the line itself first
    if (is_function_definition(lines[line_number])) {
        return get_function_name_from_line(lines[line_number]);
    }
    
    // Search backwards for function definition
    for (int i = static_cast<int>(line_number) - 1; i >= 0; --i) {
        if (is_function_definition(lines[i])) {
            return get_function_name_from_line(lines[i]);
        }
        
        // Stop searching if we hit a class definition or another function
        std::string trimmed = trim(lines[i]);
        if (trimmed.find("class ") == 0 || trimmed.find("struct ") == 0) {
            break;
        }
    }
    
    return "";
}

std::string extract_class_name(
    const std::vector<std::string>& lines,
    size_t line_number
) {
    if (line_number >= lines.size()) {
        return "";
    }
    
    // Search backwards for class definition
    int brace_count = 0;
    for (int i = static_cast<int>(line_number); i >= 0; --i) {
        std::string line = lines[i];
        
        // Count braces to track scope
        brace_count += std::count(line.begin(), line.end(), '}');
        brace_count -= std::count(line.begin(), line.end(), '{');
        
        if (is_class_definition(line) && brace_count <= 0) {
            return get_class_name_from_line(line);
        }
    }
    
    return "";
}

std::vector<std::string> extract_imports(
    const std::vector<std::string>& lines
) {
    std::vector<std::string> imports;
    
    // Scan first lines for imports (imports are typically at the top)
    size_t scan_limit = std::min(lines.size(), IMPORT_SCAN_LIMIT);
    
    for (size_t i = 0; i < scan_limit; ++i) {
        std::string line = trim(lines[i]);
        
        // Check for various import patterns
        if (line.find("#include") == 0 ||
            line.find("import ") == 0 ||
            line.find("from ") == 0 ||
            line.find("require(") != std::string::npos ||
            line.find("using ") == 0 ||
            // TypeScript/ES6 specific patterns
            line.find("import{") == 0 ||
            line.find("import *") == 0 ||
            line.find("import type") == 0 ||
            line.find("export {") == 0 ||
            line.find("export *") == 0) {
            imports.push_back(line);
        }
    }
    
    return imports;
}

}  // namespace analysis
}  // namespace wizardmerge
