/**
 * @file risk_analyzer.h
 * @brief Risk analysis for merge conflict resolutions
 *
 * Assesses the risk level of different resolution choices to help
 * developers make safer merge decisions.
 */

#ifndef WIZARDMERGE_ANALYSIS_RISK_ANALYZER_H
#define WIZARDMERGE_ANALYSIS_RISK_ANALYZER_H

#include <string>
#include <vector>

namespace wizardmerge {
namespace analysis {

/**
 * @brief Risk level enumeration for merge resolutions.
 */
enum class RiskLevel {
    LOW,      // Safe to merge, minimal risk
    MEDIUM,   // Some risk, review recommended
    HIGH,     // High risk, careful review required
    CRITICAL  // Critical risk, requires expert review
};

/**
 * @brief Detailed risk assessment for a merge resolution.
 */
struct RiskAssessment {
    RiskLevel level;
    double confidence_score;  // 0.0 to 1.0
    std::vector<std::string> risk_factors;
    std::vector<std::string> recommendations;
    
    // Specific risk indicators
    bool has_syntax_changes;
    bool has_logic_changes;
    bool has_api_changes;
    bool affects_multiple_functions;
    bool affects_critical_section;
};

/**
 * @brief Analyzes risk of accepting "ours" version.
 *
 * @param base Base version lines
 * @param ours Our version lines
 * @param theirs Their version lines
 * @return RiskAssessment for accepting ours
 */
RiskAssessment analyze_risk_ours(
    const std::vector<std::string>& base,
    const std::vector<std::string>& ours,
    const std::vector<std::string>& theirs
);

/**
 * @brief Analyzes risk of accepting "theirs" version.
 *
 * @param base Base version lines
 * @param ours Our version lines
 * @param theirs Their version lines
 * @return RiskAssessment for accepting theirs
 */
RiskAssessment analyze_risk_theirs(
    const std::vector<std::string>& base,
    const std::vector<std::string>& ours,
    const std::vector<std::string>& theirs
);

/**
 * @brief Analyzes risk of accepting both versions (concatenation).
 *
 * @param base Base version lines
 * @param ours Our version lines
 * @param theirs Their version lines
 * @return RiskAssessment for accepting both
 */
RiskAssessment analyze_risk_both(
    const std::vector<std::string>& base,
    const std::vector<std::string>& ours,
    const std::vector<std::string>& theirs
);

/**
 * @brief Converts RiskLevel to string representation.
 *
 * @param level Risk level to convert
 * @return String representation ("low", "medium", "high", "critical")
 */
std::string risk_level_to_string(RiskLevel level);

/**
 * @brief Checks if code contains critical patterns (security, data loss, etc.).
 *
 * @param lines Lines of code to check
 * @return true if critical patterns detected
 */
bool contains_critical_patterns(const std::vector<std::string>& lines);

/**
 * @brief Detects if changes affect API signatures.
 *
 * @param base Base version lines
 * @param modified Modified version lines
 * @return true if API changes detected
 */
bool has_api_signature_changes(
    const std::vector<std::string>& base,
    const std::vector<std::string>& modified
);

/**
 * @brief Detects if TypeScript interface or type definitions changed.
 *
 * @param base Base version lines
 * @param modified Modified version lines
 * @return true if interface/type changes detected
 */
bool has_typescript_interface_changes(
    const std::vector<std::string>& base,
    const std::vector<std::string>& modified
);

/**
 * @brief Checks if file is a package-lock.json file.
 *
 * @param filename Name of the file
 * @return true if file is package-lock.json
 */
bool is_package_lock_file(const std::string& filename);

}  // namespace analysis
}  // namespace wizardmerge

#endif  // WIZARDMERGE_ANALYSIS_RISK_ANALYZER_H
