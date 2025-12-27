/**
 * @file context_analyzer.h
 * @brief Context analysis for merge conflicts
 *
 * Analyzes the code context around merge conflicts to provide better
 * understanding and intelligent suggestions for resolution.
 */

#ifndef WIZARDMERGE_ANALYSIS_CONTEXT_ANALYZER_H
#define WIZARDMERGE_ANALYSIS_CONTEXT_ANALYZER_H

#include <string>
#include <vector>
#include <map>

namespace wizardmerge {
namespace analysis {

/**
 * @brief Represents code context information for a specific line or region.
 */
struct CodeContext {
    size_t start_line;
    size_t end_line;
    std::vector<std::string> surrounding_lines;
    std::string function_name;
    std::string class_name;
    std::vector<std::string> imports;
    std::map<std::string, std::string> metadata;
};

/**
 * @brief Analyzes code context around a specific region.
 *
 * This function examines the code surrounding a conflict or change
 * to provide contextual information that can help in understanding
 * the change and making better merge decisions.
 *
 * @param lines The full file content as lines
 * @param start_line Starting line of the region of interest
 * @param end_line Ending line of the region of interest
 * @param context_window Number of lines before/after to include (default: 5)
 * @return CodeContext containing analyzed context information
 */
CodeContext analyze_context(
    const std::vector<std::string>& lines,
    size_t start_line,
    size_t end_line,
    size_t context_window = 5
);

/**
 * @brief Extracts function or method name from context.
 *
 * Analyzes surrounding code to determine if the region is within
 * a function or method, and extracts its name.
 *
 * @param lines Lines of code to analyze
 * @param line_number Line number to check
 * @return Function name if found, empty string otherwise
 */
std::string extract_function_name(
    const std::vector<std::string>& lines,
    size_t line_number
);

/**
 * @brief Extracts class name from context.
 *
 * Analyzes surrounding code to determine if the region is within
 * a class definition, and extracts its name.
 *
 * @param lines Lines of code to analyze
 * @param line_number Line number to check
 * @return Class name if found, empty string otherwise
 */
std::string extract_class_name(
    const std::vector<std::string>& lines,
    size_t line_number
);

/**
 * @brief Extracts import/include statements from the file.
 *
 * Scans the file for import, include, or require statements
 * to understand dependencies.
 *
 * @param lines Lines of code to analyze
 * @return Vector of import statements
 */
std::vector<std::string> extract_imports(
    const std::vector<std::string>& lines
);

}  // namespace analysis
}  // namespace wizardmerge

#endif  // WIZARDMERGE_ANALYSIS_CONTEXT_ANALYZER_H
