/**
 * @file test_risk_analyzer.cpp
 * @brief Unit tests for risk analysis module
 */

#include "wizardmerge/analysis/risk_analyzer.h"
#include <gtest/gtest.h>

using namespace wizardmerge::analysis;

/**
 * Test risk level to string conversion
 */
TEST(RiskAnalyzerTest, RiskLevelToString) {
    EXPECT_EQ(risk_level_to_string(RiskLevel::LOW), "low");
    EXPECT_EQ(risk_level_to_string(RiskLevel::MEDIUM), "medium");
    EXPECT_EQ(risk_level_to_string(RiskLevel::HIGH), "high");
    EXPECT_EQ(risk_level_to_string(RiskLevel::CRITICAL), "critical");
}

/**
 * Test basic risk analysis for "ours"
 */
TEST(RiskAnalyzerTest, BasicRiskAnalysisOurs) {
    std::vector<std::string> base = {"int x = 10;"};
    std::vector<std::string> ours = {"int x = 20;"};
    std::vector<std::string> theirs = {"int x = 30;"};
    
    auto risk = analyze_risk_ours(base, ours, theirs);
    
    EXPECT_TRUE(risk.level == RiskLevel::LOW || risk.level == RiskLevel::MEDIUM);
    EXPECT_GE(risk.confidence_score, 0.0);
    EXPECT_LE(risk.confidence_score, 1.0);
    EXPECT_FALSE(risk.recommendations.empty());
}

/**
 * Test basic risk analysis for "theirs"
 */
TEST(RiskAnalyzerTest, BasicRiskAnalysisTheirs) {
    std::vector<std::string> base = {"int x = 10;"};
    std::vector<std::string> ours = {"int x = 20;"};
    std::vector<std::string> theirs = {"int x = 30;"};
    
    auto risk = analyze_risk_theirs(base, ours, theirs);
    
    EXPECT_TRUE(risk.level == RiskLevel::LOW || risk.level == RiskLevel::MEDIUM);
    EXPECT_GE(risk.confidence_score, 0.0);
    EXPECT_LE(risk.confidence_score, 1.0);
    EXPECT_FALSE(risk.recommendations.empty());
}

/**
 * Test risk analysis for "both" (concatenation)
 */
TEST(RiskAnalyzerTest, RiskAnalysisBoth) {
    std::vector<std::string> base = {"int x = 10;"};
    std::vector<std::string> ours = {"int x = 20;"};
    std::vector<std::string> theirs = {"int x = 30;"};
    
    auto risk = analyze_risk_both(base, ours, theirs);
    
    // "Both" strategy should typically have medium or higher risk
    EXPECT_TRUE(risk.level >= RiskLevel::MEDIUM);
    EXPECT_GE(risk.confidence_score, 0.0);
    EXPECT_LE(risk.confidence_score, 1.0);
    EXPECT_FALSE(risk.recommendations.empty());
}

/**
 * Test critical pattern detection
 */
TEST(RiskAnalyzerTest, DetectCriticalPatterns) {
    std::vector<std::string> safe_code = {"int x = 10;", "return x;"};
    std::vector<std::string> unsafe_code = {"delete ptr;", "system(\"rm -rf /\");"};
    
    EXPECT_FALSE(contains_critical_patterns(safe_code));
    EXPECT_TRUE(contains_critical_patterns(unsafe_code));
}

/**
 * Test API signature change detection
 */
TEST(RiskAnalyzerTest, DetectAPISignatureChanges) {
    std::vector<std::string> base_sig = {"void myFunction(int x) {"};
    std::vector<std::string> modified_sig = {"void myFunction(int x, int y) {"};
    std::vector<std::string> same_sig = {"void myFunction(int x) {"};
    
    EXPECT_TRUE(has_api_signature_changes(base_sig, modified_sig));
    EXPECT_FALSE(has_api_signature_changes(base_sig, same_sig));
}

/**
 * Test high risk for large changes
 */
TEST(RiskAnalyzerTest, HighRiskForLargeChanges) {
    std::vector<std::string> base = {"line1"};
    std::vector<std::string> ours;
    std::vector<std::string> theirs = {"line1"};
    
    // Create large change in ours
    for (int i = 0; i < 15; ++i) {
        ours.push_back("changed_line_" + std::to_string(i));
    }
    
    auto risk = analyze_risk_ours(base, ours, theirs);
    
    // Should detect significant changes
    EXPECT_TRUE(risk.level >= RiskLevel::MEDIUM);
    EXPECT_FALSE(risk.risk_factors.empty());
}

/**
 * Test risk with critical patterns
 */
TEST(RiskAnalyzerTest, CriticalPatternsIncreaseRisk) {
    std::vector<std::string> base = {"int x = 10;"};
    std::vector<std::string> ours = {"delete database;", "eval(user_input);"};
    std::vector<std::string> theirs = {"int x = 10;"};
    
    auto risk = analyze_risk_ours(base, ours, theirs);
    
    EXPECT_TRUE(risk.level >= RiskLevel::HIGH);
    EXPECT_TRUE(risk.affects_critical_section);
    EXPECT_FALSE(risk.risk_factors.empty());
}

/**
 * Test risk factors are populated
 */
TEST(RiskAnalyzerTest, RiskFactorsPopulated) {
    std::vector<std::string> base = {"line1", "line2", "line3"};
    std::vector<std::string> ours = {"changed1", "changed2", "changed3"};
    std::vector<std::string> theirs = {"line1", "line2", "line3"};
    
    auto risk = analyze_risk_ours(base, ours, theirs);
    
    // Should have some analysis results
    EXPECT_TRUE(!risk.recommendations.empty() || !risk.risk_factors.empty());
}
