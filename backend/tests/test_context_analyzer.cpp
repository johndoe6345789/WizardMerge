/**
 * @file test_context_analyzer.cpp
 * @brief Unit tests for context analysis module
 */

#include "wizardmerge/analysis/context_analyzer.h"
#include <gtest/gtest.h>

using namespace wizardmerge::analysis;

/**
 * Test basic context analysis
 */
TEST(ContextAnalyzerTest, BasicContextAnalysis) {
  std::vector<std::string> lines = {"#include <iostream>",
                                    "",
                                    "class MyClass {",
                                    "public:",
                                    "    void myMethod() {",
                                    "        int x = 42;",
                                    "        int y = 100;",
                                    "        return;",
                                    "    }",
                                    "};"};

  auto context = analyze_context(lines, 5, 7);

  EXPECT_EQ(context.start_line, 5);
  EXPECT_EQ(context.end_line, 7);
  EXPECT_FALSE(context.surrounding_lines.empty());
}

/**
 * Test function name extraction
 */
TEST(ContextAnalyzerTest, ExtractFunctionName) {
  std::vector<std::string> lines = {"void testFunction() {", "    int x = 10;",
                                    "    return;", "}"};

  std::string func_name = extract_function_name(lines, 1);
  EXPECT_EQ(func_name, "testFunction");
}

/**
 * Test Python function name extraction
 */
TEST(ContextAnalyzerTest, ExtractPythonFunctionName) {
  std::vector<std::string> lines = {"def my_python_function():", "    x = 10",
                                    "    return x"};

  std::string func_name = extract_function_name(lines, 1);
  EXPECT_EQ(func_name, "my_python_function");
}

/**
 * Test class name extraction
 */
TEST(ContextAnalyzerTest, ExtractClassName) {
  std::vector<std::string> lines = {"class TestClass {", "    int member;",
                                    "};"};

  std::string class_name = extract_class_name(lines, 1);
  EXPECT_EQ(class_name, "TestClass");
}

/**
 * Test import extraction
 */
TEST(ContextAnalyzerTest, ExtractImports) {
  std::vector<std::string> lines = {
      "#include <iostream>", "#include <vector>", "",
      "int main() {",        "    return 0;",     "}"};

  auto imports = extract_imports(lines);
  EXPECT_EQ(imports.size(), 2);
  EXPECT_EQ(imports[0], "#include <iostream>");
  EXPECT_EQ(imports[1], "#include <vector>");
}

/**
 * Test context with no function
 */
TEST(ContextAnalyzerTest, NoFunctionContext) {
  std::vector<std::string> lines = {"int x = 10;", "int y = 20;"};

  std::string func_name = extract_function_name(lines, 0);
  EXPECT_EQ(func_name, "");
}

/**
 * Test context window boundaries
 */
TEST(ContextAnalyzerTest, ContextWindowBoundaries) {
  std::vector<std::string> lines = {"line1", "line2", "line3", "line4",
                                    "line5"};

  // Test with small context window at beginning of file
  auto context = analyze_context(lines, 0, 0, 2);
  EXPECT_GE(context.surrounding_lines.size(), 1);

  // Test with context window at end of file
  context = analyze_context(lines, 4, 4, 2);
  EXPECT_GE(context.surrounding_lines.size(), 1);
}

/**
 * Test TypeScript function detection
 */
TEST(ContextAnalyzerTest, TypeScriptFunctionDetection) {
  std::vector<std::string> lines = {"export async function fetchData() {",
                                    "    const data = await api.get();",
                                    "    return data;", "}"};

  std::string func_name = extract_function_name(lines, 1);
  EXPECT_EQ(func_name, "fetchData");
}

/**
 * Test TypeScript arrow function detection
 */
TEST(ContextAnalyzerTest, TypeScriptArrowFunctionDetection) {
  std::vector<std::string> lines = {
      "const handleClick = (event: MouseEvent) => {", "    console.log(event);",
      "};"};

  std::string func_name = extract_function_name(lines, 0);
  EXPECT_EQ(func_name, "handleClick");
}

/**
 * Test TypeScript interface detection
 */
TEST(ContextAnalyzerTest, TypeScriptInterfaceDetection) {
  std::vector<std::string> lines = {
      "export interface User {", "    id: number;", "    name: string;", "}"};

  std::string class_name = extract_class_name(lines, 1);
  EXPECT_EQ(class_name, "User");
}

/**
 * Test TypeScript type alias detection
 */
TEST(ContextAnalyzerTest, TypeScriptTypeAliasDetection) {
  std::vector<std::string> lines = {
      "export type Status = 'pending' | 'approved' | 'rejected';",
      "const status: Status = 'pending';"};

  std::string type_name = extract_class_name(lines, 0);
  EXPECT_EQ(type_name, "Status");
}

/**
 * Test TypeScript enum detection
 */
TEST(ContextAnalyzerTest, TypeScriptEnumDetection) {
  std::vector<std::string> lines = {"enum Color {", "    Red,", "    Green,",
                                    "    Blue", "}"};

  std::string enum_name = extract_class_name(lines, 1);
  EXPECT_EQ(enum_name, "Color");
}

/**
 * Test TypeScript import extraction
 */
TEST(ContextAnalyzerTest, TypeScriptImportExtraction) {
  std::vector<std::string> lines = {"import { Component } from 'react';",
                                    "import type { User } from './types';",
                                    "import * as utils from './utils';",
                                    "",
                                    "function MyComponent() {",
                                    "    return null;",
                                    "}"};

  auto imports = extract_imports(lines);
  EXPECT_GE(imports.size(), 3);
}
