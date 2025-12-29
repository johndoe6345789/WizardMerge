/**
 * @file typescript_example.cpp
 * @brief Example demonstrating TypeScript support in WizardMerge
 */

#include "wizardmerge/analysis/context_analyzer.h"
#include "wizardmerge/analysis/risk_analyzer.h"
#include <iostream>
#include <string>
#include <vector>

using namespace wizardmerge::analysis;

void print_separator() {
  std::cout << "\n" << std::string(60, '=') << "\n" << std::endl;
}

int main() {
  std::cout << "WizardMerge TypeScript Support Demo" << std::endl;
  print_separator();

  // Example 1: TypeScript Function Detection
  std::cout << "Example 1: TypeScript Function Detection" << std::endl;
  std::cout << std::string(40, '-') << std::endl;

  std::vector<std::string> ts_functions = {
      "export async function fetchUser(id: number): Promise<User> {",
      "    const response = await api.get(`/users/${id}`);",
      "    return response.data;", "}"};

  std::string func_name = extract_function_name(ts_functions, 1);
  std::cout << "Detected function: " << func_name << std::endl;
  print_separator();

  // Example 2: TypeScript Interface Detection
  std::cout << "Example 2: TypeScript Interface Detection" << std::endl;
  std::cout << std::string(40, '-') << std::endl;

  std::vector<std::string> ts_interface = {
      "export interface User {", "    id: number;", "    name: string;",
      "    email: string;", "}"};

  std::string type_name = extract_class_name(ts_interface, 2);
  std::cout << "Detected type: " << type_name << std::endl;
  print_separator();

  // Example 3: TypeScript Import Extraction
  std::cout << "Example 3: TypeScript Import Extraction" << std::endl;
  std::cout << std::string(40, '-') << std::endl;

  std::vector<std::string> ts_imports = {
      "import { Component, useState } from 'react';",
      "import type { User } from './types';",
      "import * as utils from './utils';", "",
      "export const MyComponent = () => {"};

  auto imports = extract_imports(ts_imports);
  std::cout << "Detected " << imports.size() << " imports:" << std::endl;
  for (const auto &import : imports) {
    std::cout << "  - " << import << std::endl;
  }
  print_separator();

  // Example 4: TypeScript Interface Change Detection
  std::cout << "Example 4: TypeScript Interface Change Detection" << std::endl;
  std::cout << std::string(40, '-') << std::endl;

  std::vector<std::string> base_interface = {
      "interface User {", "    id: number;", "    name: string;", "}"};

  std::vector<std::string> modified_interface = {
      "interface User {",
      "    id: number;",
      "    name: string;",
      "    email: string;  // Added",
      "    age?: number;    // Added optional",
      "}"};

  bool has_ts_changes =
      has_typescript_interface_changes(base_interface, modified_interface);
  std::cout << "Interface changed: " << (has_ts_changes ? "YES" : "NO")
            << std::endl;
  std::cout << "Risk: Breaking change - affects all usages of User"
            << std::endl;
  print_separator();

  // Example 5: TypeScript Critical Pattern Detection
  std::cout << "Example 5: TypeScript Critical Pattern Detection" << std::endl;
  std::cout << std::string(40, '-') << std::endl;

  std::vector<std::string> risky_code = {
      "// Type safety bypass",
      "const user = response.data as any;",
      "",
      "// Error suppression",
      "// @ts-ignore",
      "element.innerHTML = userInput;",
      "",
      "// Insecure storage",
      "localStorage.setItem('password', pwd);"};

  bool has_critical = contains_critical_patterns(risky_code);
  std::cout << "Contains critical patterns: " << (has_critical ? "YES" : "NO")
            << std::endl;
  if (has_critical) {
    std::cout << "Critical issues detected:" << std::endl;
    std::cout << "  - Type safety bypass (as any)" << std::endl;
    std::cout << "  - Error suppression (@ts-ignore)" << std::endl;
    std::cout << "  - XSS vulnerability (innerHTML)" << std::endl;
    std::cout << "  - Insecure password storage (localStorage)" << std::endl;
  }
  print_separator();

  // Example 6: Package Lock File Detection
  std::cout << "Example 6: Package Lock File Detection" << std::endl;
  std::cout << std::string(40, '-') << std::endl;

  std::vector<std::string> lock_files = {"package-lock.json", "yarn.lock",
                                         "pnpm-lock.yaml", "bun.lockb",
                                         "package.json"};

  for (const auto &file : lock_files) {
    bool is_lock = is_package_lock_file(file);
    std::cout << file << ": " << (is_lock ? "LOCK FILE" : "regular file")
              << std::endl;
  }

  std::cout << "\nRecommendation for lock file conflicts:" << std::endl;
  std::cout << "  1. Merge package.json manually" << std::endl;
  std::cout << "  2. Delete lock file" << std::endl;
  std::cout << "  3. Run package manager to regenerate" << std::endl;
  print_separator();

  // Example 7: Complete Risk Analysis
  std::cout << "Example 7: Complete Risk Analysis for TypeScript Changes"
            << std::endl;
  std::cout << std::string(40, '-') << std::endl;

  std::vector<std::string> base = {"interface Config {", "    timeout: number;",
                                   "}"};

  std::vector<std::string> ours = {"interface Config {", "    timeout: number;",
                                   "    retries: number;", "}"};

  std::vector<std::string> theirs = {"interface Config {",
                                     "    timeout: number;", "}"};

  auto risk = analyze_risk_ours(base, ours, theirs);

  std::cout << "Risk Level: " << risk_level_to_string(risk.level) << std::endl;
  std::cout << "Confidence: " << risk.confidence_score << std::endl;
  std::cout << "Has API Changes: " << (risk.has_api_changes ? "YES" : "NO")
            << std::endl;

  std::cout << "\nRisk Factors:" << std::endl;
  for (const auto &factor : risk.risk_factors) {
    std::cout << "  - " << factor << std::endl;
  }

  std::cout << "\nRecommendations:" << std::endl;
  for (const auto &rec : risk.recommendations) {
    std::cout << "  - " << rec << std::endl;
  }
  print_separator();

  std::cout << "Demo completed successfully!" << std::endl;
  std::cout << "See docs/TYPESCRIPT_SUPPORT.md for more details." << std::endl;

  return 0;
}
