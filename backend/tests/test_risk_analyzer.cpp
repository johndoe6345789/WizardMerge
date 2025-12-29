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
  std::vector<std::string> unsafe_code = {"delete ptr;",
                                          "system(\"rm -rf /\");"};

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

/**
 * Test TypeScript interface change detection
 */
TEST(RiskAnalyzerTest, TypeScriptInterfaceChangesDetected) {
  std::vector<std::string> base = {"interface User {", "    name: string;",
                                   "}"};
  std::vector<std::string> modified = {"interface User {", "    name: string;",
                                       "    age: number;", "}"};

  EXPECT_TRUE(has_typescript_interface_changes(base, modified));
}

/**
 * Test TypeScript type alias change detection
 */
TEST(RiskAnalyzerTest, TypeScriptTypeChangesDetected) {
  std::vector<std::string> base = {"type Status = 'pending' | 'approved';"};
  std::vector<std::string> modified = {
      "type Status = 'pending' | 'approved' | 'rejected';"};

  EXPECT_TRUE(has_typescript_interface_changes(base, modified));
}

/**
 * Test TypeScript enum change detection
 */
TEST(RiskAnalyzerTest, TypeScriptEnumChangesDetected) {
  std::vector<std::string> base = {"enum Color {", "    Red,", "    Green",
                                   "}"};
  std::vector<std::string> modified = {"enum Color {", "    Red,", "    Green,",
                                       "    Blue", "}"};

  EXPECT_TRUE(has_typescript_interface_changes(base, modified));
}

/**
 * Test package-lock.json file detection
 */
TEST(RiskAnalyzerTest, PackageLockFileDetection) {
  EXPECT_TRUE(is_package_lock_file("package-lock.json"));
  EXPECT_TRUE(is_package_lock_file("path/to/package-lock.json"));
  EXPECT_TRUE(is_package_lock_file("yarn.lock"));
  EXPECT_TRUE(is_package_lock_file("pnpm-lock.yaml"));
  EXPECT_TRUE(is_package_lock_file("bun.lockb"));
  EXPECT_FALSE(is_package_lock_file("package.json"));
  EXPECT_FALSE(is_package_lock_file("src/index.ts"));
}

/**
 * Test TypeScript critical patterns detection
 */
TEST(RiskAnalyzerTest, TypeScriptCriticalPatternsDetected) {
  std::vector<std::string> code_with_ts_issues = {
      "const user = data as any;", "// @ts-ignore",
      "element.innerHTML = userInput;",
      "localStorage.setItem('password', pwd);"};

  EXPECT_TRUE(contains_critical_patterns(code_with_ts_issues));
}

/**
 * Test TypeScript safe code doesn't trigger false positives
 */
TEST(RiskAnalyzerTest, TypeScriptSafeCodeNoFalsePositives) {
  std::vector<std::string> safe_code = {
      "const user: User = { name: 'John', age: 30 };",
      "function greet(name: string): string {", "    return `Hello, ${name}`;",
      "}"};

  EXPECT_FALSE(contains_critical_patterns(safe_code));
}

/**
 * Test risk analysis includes TypeScript interface changes
 */
TEST(RiskAnalyzerTest, RiskAnalysisIncludesTypeScriptChanges) {
  std::vector<std::string> base = {"interface User {", "    name: string;",
                                   "}"};
  std::vector<std::string> ours = {"interface User {", "    name: string;",
                                   "    email: string;", "}"};
  std::vector<std::string> theirs = base;

  auto risk = analyze_risk_ours(base, ours, theirs);

  EXPECT_TRUE(risk.has_api_changes);
  EXPECT_TRUE(risk.level >= RiskLevel::MEDIUM);

  // Check if TypeScript-related risk factor is mentioned
  bool has_ts_risk = false;
  for (const auto &factor : risk.risk_factors) {
    if (factor.find("TypeScript") != std::string::npos) {
      has_ts_risk = true;
      break;
    }
  }
  EXPECT_TRUE(has_ts_risk);
}
