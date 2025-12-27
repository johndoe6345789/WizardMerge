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
    std::vector<std::string> lines = {
        "#include <iostream>",
        "",
        "class MyClass {",
        "public:",
        "    void myMethod() {",
        "        int x = 42;",
        "        int y = 100;",
        "        return;",
        "    }",
        "};"
    };
    
    auto context = analyze_context(lines, 5, 7);
    
    EXPECT_EQ(context.start_line, 5);
    EXPECT_EQ(context.end_line, 7);
    EXPECT_FALSE(context.surrounding_lines.empty());
}

/**
 * Test function name extraction
 */
TEST(ContextAnalyzerTest, ExtractFunctionName) {
    std::vector<std::string> lines = {
        "void testFunction() {",
        "    int x = 10;",
        "    return;",
        "}"
    };
    
    std::string func_name = extract_function_name(lines, 1);
    EXPECT_EQ(func_name, "testFunction");
}

/**
 * Test Python function name extraction
 */
TEST(ContextAnalyzerTest, ExtractPythonFunctionName) {
    std::vector<std::string> lines = {
        "def my_python_function():",
        "    x = 10",
        "    return x"
    };
    
    std::string func_name = extract_function_name(lines, 1);
    EXPECT_EQ(func_name, "my_python_function");
}

/**
 * Test class name extraction
 */
TEST(ContextAnalyzerTest, ExtractClassName) {
    std::vector<std::string> lines = {
        "class TestClass {",
        "    int member;",
        "};"
    };
    
    std::string class_name = extract_class_name(lines, 1);
    EXPECT_EQ(class_name, "TestClass");
}

/**
 * Test import extraction
 */
TEST(ContextAnalyzerTest, ExtractImports) {
    std::vector<std::string> lines = {
        "#include <iostream>",
        "#include <vector>",
        "",
        "int main() {",
        "    return 0;",
        "}"
    };
    
    auto imports = extract_imports(lines);
    EXPECT_EQ(imports.size(), 2);
    EXPECT_EQ(imports[0], "#include <iostream>");
    EXPECT_EQ(imports[1], "#include <vector>");
}

/**
 * Test context with no function
 */
TEST(ContextAnalyzerTest, NoFunctionContext) {
    std::vector<std::string> lines = {
        "int x = 10;",
        "int y = 20;"
    };
    
    std::string func_name = extract_function_name(lines, 0);
    EXPECT_EQ(func_name, "");
}

/**
 * Test context window boundaries
 */
TEST(ContextAnalyzerTest, ContextWindowBoundaries) {
    std::vector<std::string> lines = {
        "line1",
        "line2",
        "line3",
        "line4",
        "line5"
    };
    
    // Test with small context window at beginning of file
    auto context = analyze_context(lines, 0, 0, 2);
    EXPECT_GE(context.surrounding_lines.size(), 1);
    
    // Test with context window at end of file
    context = analyze_context(lines, 4, 4, 2);
    EXPECT_GE(context.surrounding_lines.size(), 1);
}
