/**
 * @file test_three_way_merge.cpp
 * @brief Unit tests for three-way merge algorithm
 */

#include "wizardmerge/merge/three_way_merge.h"
#include <gtest/gtest.h>

using namespace wizardmerge::merge;

/**
 * Test basic three-way merge with no conflicts
 */
TEST(ThreeWayMergeTest, NoConflicts) {
    std::vector<std::string> base = {"line1", "line2", "line3"};
    std::vector<std::string> ours = {"line1", "line2_modified", "line3"};
    std::vector<std::string> theirs = {"line1", "line2", "line3_modified"};
    
    auto result = three_way_merge(base, ours, theirs);
    
    EXPECT_FALSE(result.has_conflicts());
    ASSERT_EQ(result.merged_lines.size(), 3);
    EXPECT_EQ(result.merged_lines[0].content, "line1");
    EXPECT_EQ(result.merged_lines[1].content, "line2_modified");
    EXPECT_EQ(result.merged_lines[2].content, "line3_modified");
}

/**
 * Test three-way merge with conflicts
 */
TEST(ThreeWayMergeTest, WithConflicts) {
    std::vector<std::string> base = {"line1", "line2", "line3"};
    std::vector<std::string> ours = {"line1", "line2_ours", "line3"};
    std::vector<std::string> theirs = {"line1", "line2_theirs", "line3"};
    
    auto result = three_way_merge(base, ours, theirs);
    
    EXPECT_TRUE(result.has_conflicts());
    EXPECT_EQ(result.conflicts.size(), 1);
}

/**
 * Test identical changes from both sides
 */
TEST(ThreeWayMergeTest, IdenticalChanges) {
    std::vector<std::string> base = {"line1", "line2", "line3"};
    std::vector<std::string> ours = {"line1", "line2_same", "line3"};
    std::vector<std::string> theirs = {"line1", "line2_same", "line3"};
    
    auto result = three_way_merge(base, ours, theirs);
    
    EXPECT_FALSE(result.has_conflicts());
    EXPECT_EQ(result.merged_lines[1].content, "line2_same");
}

/**
 * Test base equals ours, theirs changed
 */
TEST(ThreeWayMergeTest, BaseEqualsOurs) {
    std::vector<std::string> base = {"line1", "line2", "line3"};
    std::vector<std::string> ours = {"line1", "line2", "line3"};
    std::vector<std::string> theirs = {"line1", "line2_changed", "line3"};
    
    auto result = three_way_merge(base, ours, theirs);
    
    EXPECT_FALSE(result.has_conflicts());
    EXPECT_EQ(result.merged_lines[1].content, "line2_changed");
}

/**
 * Test base equals theirs, ours changed
 */
TEST(ThreeWayMergeTest, BaseEqualsTheirs) {
    std::vector<std::string> base = {"line1", "line2", "line3"};
    std::vector<std::string> ours = {"line1", "line2_changed", "line3"};
    std::vector<std::string> theirs = {"line1", "line2", "line3"};
    
    auto result = three_way_merge(base, ours, theirs);
    
    EXPECT_FALSE(result.has_conflicts());
    EXPECT_EQ(result.merged_lines[1].content, "line2_changed");
}

/**
 * Test auto-resolve whitespace differences
 */
TEST(AutoResolveTest, WhitespaceOnly) {
    std::vector<std::string> base = {"line1", "line2", "line3"};
    std::vector<std::string> ours = {"line1", "  line2_changed  ", "line3"};
    std::vector<std::string> theirs = {"line1", "line2_changed", "line3"};
    
    auto result = three_way_merge(base, ours, theirs);
    auto resolved = auto_resolve(result);
    
    // Whitespace-only differences should be auto-resolved
    EXPECT_LT(resolved.conflicts.size(), result.conflicts.size());
}

/**
 * Test empty files
 */
TEST(ThreeWayMergeTest, EmptyFiles) {
    std::vector<std::string> base = {};
    std::vector<std::string> ours = {};
    std::vector<std::string> theirs = {};
    
    auto result = three_way_merge(base, ours, theirs);
    
    EXPECT_FALSE(result.has_conflicts());
    EXPECT_EQ(result.merged_lines.size(), 0);
}

/**
 * Test one side adds lines
 */
TEST(ThreeWayMergeTest, OneSideAddsLines) {
    std::vector<std::string> base = {"line1"};
    std::vector<std::string> ours = {"line1", "line2"};
    std::vector<std::string> theirs = {"line1"};
    
    auto result = three_way_merge(base, ours, theirs);
    
    EXPECT_FALSE(result.has_conflicts());
    ASSERT_EQ(result.merged_lines.size(), 2);
}
