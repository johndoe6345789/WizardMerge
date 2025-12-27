/**
 * @file test_github_client.cpp
 * @brief Unit tests for GitHub client functionality
 */

#include "wizardmerge/git/github_client.h"
#include <gtest/gtest.h>

using namespace wizardmerge::git;

/**
 * Test PR URL parsing with various formats
 */
TEST(GitHubClientTest, ParsePRUrl_ValidUrls) {
    std::string owner, repo;
    int pr_number;
    
    // Test full HTTPS URL
    ASSERT_TRUE(parse_pr_url("https://github.com/owner/repo/pull/123", owner, repo, pr_number));
    EXPECT_EQ(owner, "owner");
    EXPECT_EQ(repo, "repo");
    EXPECT_EQ(pr_number, 123);
    
    // Test without https://
    ASSERT_TRUE(parse_pr_url("github.com/user/project/pull/456", owner, repo, pr_number));
    EXPECT_EQ(owner, "user");
    EXPECT_EQ(repo, "project");
    EXPECT_EQ(pr_number, 456);
    
    // Test with www
    ASSERT_TRUE(parse_pr_url("https://www.github.com/testuser/testrepo/pull/789", owner, repo, pr_number));
    EXPECT_EQ(owner, "testuser");
    EXPECT_EQ(repo, "testrepo");
    EXPECT_EQ(pr_number, 789);
}

/**
 * Test PR URL parsing with invalid formats
 */
TEST(GitHubClientTest, ParsePRUrl_InvalidUrls) {
    std::string owner, repo;
    int pr_number;
    
    // Missing PR number
    EXPECT_FALSE(parse_pr_url("https://github.com/owner/repo/pull/", owner, repo, pr_number));
    
    // Invalid format
    EXPECT_FALSE(parse_pr_url("https://github.com/owner/repo", owner, repo, pr_number));
    
    // Not a GitHub URL
    EXPECT_FALSE(parse_pr_url("https://gitlab.com/owner/repo/pull/123", owner, repo, pr_number));
    
    // Empty string
    EXPECT_FALSE(parse_pr_url("", owner, repo, pr_number));
}

/**
 * Test PR URL with special characters in owner/repo names
 */
TEST(GitHubClientTest, ParsePRUrl_SpecialCharacters) {
    std::string owner, repo;
    int pr_number;
    
    // Underscores and hyphens
    ASSERT_TRUE(parse_pr_url("https://github.com/my-owner_123/my-repo_456/pull/999", owner, repo, pr_number));
    EXPECT_EQ(owner, "my-owner_123");
    EXPECT_EQ(repo, "my-repo_456");
    EXPECT_EQ(pr_number, 999);
}
