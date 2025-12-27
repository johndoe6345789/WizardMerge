/**
 * @file risk_analyzer.cpp
 * @brief Implementation of risk analysis for merge conflict resolutions
 */

#include "wizardmerge/analysis/risk_analyzer.h"
#include <algorithm>
#include <regex>
#include <cmath>

namespace wizardmerge {
namespace analysis {

namespace {

// Confidence score weights for risk assessment
constexpr double BASE_CONFIDENCE = 0.5;        // Base confidence level
constexpr double SIMILARITY_WEIGHT = 0.3;      // Weight for code similarity
constexpr double CHANGE_RATIO_WEIGHT = 0.2;    // Weight for change ratio

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
 * @brief Calculate similarity score between two sets of lines (0.0 to 1.0).
 */
double calculate_similarity(
    const std::vector<std::string>& lines1,
    const std::vector<std::string>& lines2
) {
    if (lines1.empty() && lines2.empty()) return 1.0;
    if (lines1.empty() || lines2.empty()) return 0.0;
    
    // Simple Jaccard similarity on lines
    size_t common_lines = 0;
    for (const auto& line1 : lines1) {
        if (std::find(lines2.begin(), lines2.end(), line1) != lines2.end()) {
            common_lines++;
        }
    }
    
    size_t total_unique = lines1.size() + lines2.size() - common_lines;
    return total_unique > 0 ? static_cast<double>(common_lines) / total_unique : 0.0;
}

/**
 * @brief Count number of changed lines between two versions.
 */
size_t count_changes(
    const std::vector<std::string>& base,
    const std::vector<std::string>& modified
) {
    size_t changes = 0;
    size_t max_len = std::max(base.size(), modified.size());
    
    for (size_t i = 0; i < max_len; ++i) {
        std::string base_line = (i < base.size()) ? base[i] : "";
        std::string mod_line = (i < modified.size()) ? modified[i] : "";
        
        if (base_line != mod_line) {
            changes++;
        }
    }
    
    return changes;
}

/**
 * @brief Check if line contains function or method definition.
 */
bool is_function_signature(const std::string& line) {
    std::string trimmed = trim(line);
    
    std::vector<std::regex> patterns = {
        std::regex(R"(^\w+\s+\w+\s*\([^)]*\))"),      // C/C++/Java
        std::regex(R"(^def\s+\w+\s*\([^)]*\):)"),     // Python
        std::regex(R"(^function\s+\w+\s*\([^)]*\))"), // JavaScript
    };
    
    for (const auto& pattern : patterns) {
        if (std::regex_search(trimmed, pattern)) {
            return true;
        }
    }
    
    return false;
}

}  // anonymous namespace

std::string risk_level_to_string(RiskLevel level) {
    switch (level) {
        case RiskLevel::LOW: return "low";
        case RiskLevel::MEDIUM: return "medium";
        case RiskLevel::HIGH: return "high";
        case RiskLevel::CRITICAL: return "critical";
        default: return "unknown";
    }
}

bool contains_critical_patterns(const std::vector<std::string>& lines) {
    std::vector<std::regex> critical_patterns = {
        std::regex(R"(delete\s+\w+)"),          // Delete operations
        std::regex(R"(drop\s+(table|database))"), // Database drops
        std::regex(R"(rm\s+-rf)"),              // Destructive file operations
        std::regex(R"(eval\s*\()"),             // Eval (security risk)
        std::regex(R"(exec\s*\()"),             // Exec (security risk)
        std::regex(R"(system\s*\()"),           // System calls
        std::regex(R"(\.password\s*=)"),        // Password assignments
        std::regex(R"(\.secret\s*=)"),          // Secret assignments
        std::regex(R"(sudo\s+)"),               // Sudo usage
        std::regex(R"(chmod\s+777)"),           // Overly permissive permissions
    };
    
    for (const auto& line : lines) {
        std::string trimmed = trim(line);
        for (const auto& pattern : critical_patterns) {
            if (std::regex_search(trimmed, pattern)) {
                return true;
            }
        }
    }
    
    return false;
}

bool has_api_signature_changes(
    const std::vector<std::string>& base,
    const std::vector<std::string>& modified
) {
    // Check if function signatures changed
    for (size_t i = 0; i < base.size() && i < modified.size(); ++i) {
        bool base_is_sig = is_function_signature(base[i]);
        bool mod_is_sig = is_function_signature(modified[i]);
        
        if (base_is_sig && mod_is_sig && base[i] != modified[i]) {
            return true;
        }
    }
    
    return false;
}

RiskAssessment analyze_risk_ours(
    const std::vector<std::string>& base,
    const std::vector<std::string>& ours,
    const std::vector<std::string>& theirs
) {
    RiskAssessment assessment;
    assessment.level = RiskLevel::LOW;
    assessment.confidence_score = 0.5;
    assessment.has_syntax_changes = false;
    assessment.has_logic_changes = false;
    assessment.has_api_changes = false;
    assessment.affects_multiple_functions = false;
    assessment.affects_critical_section = false;
    
    // Calculate changes
    size_t our_changes = count_changes(base, ours);
    size_t their_changes = count_changes(base, theirs);
    double similarity_to_theirs = calculate_similarity(ours, theirs);
    
    // Check for critical patterns
    if (contains_critical_patterns(ours)) {
        assessment.affects_critical_section = true;
        assessment.risk_factors.push_back("Contains critical code patterns (security/data operations)");
        assessment.level = RiskLevel::HIGH;
    }
    
    // Check for API changes
    if (has_api_signature_changes(base, ours)) {
        assessment.has_api_changes = true;
        assessment.risk_factors.push_back("Function/method signatures changed");
        if (assessment.level < RiskLevel::MEDIUM) {
            assessment.level = RiskLevel::MEDIUM;
        }
    }
    
    // Assess based on amount of change
    if (our_changes > 10) {
        assessment.has_logic_changes = true;
        assessment.risk_factors.push_back("Large number of changes (" + std::to_string(our_changes) + " lines)");
        if (assessment.level < RiskLevel::MEDIUM) {
            assessment.level = RiskLevel::MEDIUM;
        }
    }
    
    // Check if we're discarding significant changes from theirs
    if (their_changes > 5 && similarity_to_theirs < 0.3) {
        assessment.risk_factors.push_back("Discarding significant changes from other branch (" + 
                                         std::to_string(their_changes) + " lines)");
        if (assessment.level < RiskLevel::MEDIUM) {
            assessment.level = RiskLevel::MEDIUM;
        }
    }
    
    // Calculate confidence score based on various factors
    double change_ratio = (our_changes + their_changes) > 0 ? 
        static_cast<double>(our_changes) / (our_changes + their_changes) : BASE_CONFIDENCE;
    assessment.confidence_score = BASE_CONFIDENCE + 
                                 (SIMILARITY_WEIGHT * similarity_to_theirs) + 
                                 (CHANGE_RATIO_WEIGHT * change_ratio);
    
    // Add recommendations
    if (assessment.level >= RiskLevel::MEDIUM) {
        assessment.recommendations.push_back("Review changes carefully before accepting");
    }
    if (assessment.has_api_changes) {
        assessment.recommendations.push_back("Verify API compatibility with dependent code");
    }
    if (assessment.affects_critical_section) {
        assessment.recommendations.push_back("Test thoroughly, especially security and data operations");
    }
    if (assessment.risk_factors.empty()) {
        assessment.recommendations.push_back("Changes appear safe to accept");
    }
    
    return assessment;
}

RiskAssessment analyze_risk_theirs(
    const std::vector<std::string>& base,
    const std::vector<std::string>& ours,
    const std::vector<std::string>& theirs
) {
    RiskAssessment assessment;
    assessment.level = RiskLevel::LOW;
    assessment.confidence_score = 0.5;
    assessment.has_syntax_changes = false;
    assessment.has_logic_changes = false;
    assessment.has_api_changes = false;
    assessment.affects_multiple_functions = false;
    assessment.affects_critical_section = false;
    
    // Calculate changes
    size_t our_changes = count_changes(base, ours);
    size_t their_changes = count_changes(base, theirs);
    double similarity_to_ours = calculate_similarity(theirs, ours);
    
    // Check for critical patterns
    if (contains_critical_patterns(theirs)) {
        assessment.affects_critical_section = true;
        assessment.risk_factors.push_back("Contains critical code patterns (security/data operations)");
        assessment.level = RiskLevel::HIGH;
    }
    
    // Check for API changes
    if (has_api_signature_changes(base, theirs)) {
        assessment.has_api_changes = true;
        assessment.risk_factors.push_back("Function/method signatures changed");
        if (assessment.level < RiskLevel::MEDIUM) {
            assessment.level = RiskLevel::MEDIUM;
        }
    }
    
    // Assess based on amount of change
    if (their_changes > 10) {
        assessment.has_logic_changes = true;
        assessment.risk_factors.push_back("Large number of changes (" + std::to_string(their_changes) + " lines)");
        if (assessment.level < RiskLevel::MEDIUM) {
            assessment.level = RiskLevel::MEDIUM;
        }
    }
    
    // Check if we're discarding our changes
    if (our_changes > 5 && similarity_to_ours < 0.3) {
        assessment.risk_factors.push_back("Discarding our local changes (" + 
                                         std::to_string(our_changes) + " lines)");
        if (assessment.level < RiskLevel::MEDIUM) {
            assessment.level = RiskLevel::MEDIUM;
        }
    }
    
    // Calculate confidence score
    double change_ratio = (our_changes + their_changes) > 0 ? 
        static_cast<double>(their_changes) / (our_changes + their_changes) : BASE_CONFIDENCE;
    assessment.confidence_score = BASE_CONFIDENCE + 
                                 (SIMILARITY_WEIGHT * similarity_to_ours) + 
                                 (CHANGE_RATIO_WEIGHT * change_ratio);
    
    // Add recommendations
    if (assessment.level >= RiskLevel::MEDIUM) {
        assessment.recommendations.push_back("Review changes carefully before accepting");
    }
    if (assessment.has_api_changes) {
        assessment.recommendations.push_back("Verify API compatibility with dependent code");
    }
    if (assessment.affects_critical_section) {
        assessment.recommendations.push_back("Test thoroughly, especially security and data operations");
    }
    if (assessment.risk_factors.empty()) {
        assessment.recommendations.push_back("Changes appear safe to accept");
    }
    
    return assessment;
}

RiskAssessment analyze_risk_both(
    const std::vector<std::string>& base,
    const std::vector<std::string>& ours,
    const std::vector<std::string>& theirs
) {
    RiskAssessment assessment;
    assessment.level = RiskLevel::MEDIUM;  // Default to medium for concatenation
    assessment.confidence_score = 0.3;      // Lower confidence for concatenation
    assessment.has_syntax_changes = true;
    assessment.has_logic_changes = true;
    assessment.has_api_changes = false;
    assessment.affects_multiple_functions = false;
    assessment.affects_critical_section = false;
    
    // Concatenating both versions is generally risky
    assessment.risk_factors.push_back("Concatenating both versions may cause duplicates or conflicts");
    
    // Check if either contains critical patterns
    if (contains_critical_patterns(ours) || contains_critical_patterns(theirs)) {
        assessment.affects_critical_section = true;
        assessment.risk_factors.push_back("Contains critical code patterns that may conflict");
        assessment.level = RiskLevel::HIGH;
    }
    
    // Check for duplicate logic
    double similarity = calculate_similarity(ours, theirs);
    if (similarity > 0.5) {
        assessment.risk_factors.push_back("High similarity may result in duplicate code");
        assessment.level = RiskLevel::HIGH;
    }
    
    // API changes from either side
    if (has_api_signature_changes(base, ours) || has_api_signature_changes(base, theirs)) {
        assessment.has_api_changes = true;
        assessment.risk_factors.push_back("Multiple API changes may cause conflicts");
        assessment.level = RiskLevel::HIGH;
    }
    
    // Recommendations for concatenation
    assessment.recommendations.push_back("Manual review required - automatic concatenation is risky");
    assessment.recommendations.push_back("Consider merging logic manually instead of concatenating");
    assessment.recommendations.push_back("Test thoroughly for duplicate or conflicting code");
    
    return assessment;
}

}  // namespace analysis
}  // namespace wizardmerge
