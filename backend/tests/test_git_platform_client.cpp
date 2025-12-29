/**
 * @file test_git_platform_client.cpp
 * @brief Unit tests for git platform client functionality (GitHub and GitLab)
 */

#include "wizardmerge/git/git_platform_client.h"
#include <gtest/gtest.h>

using namespace wizardmerge::git;

/**
 * Test PR URL parsing with various GitHub formats
 */
TEST(GitPlatformClientTest, ParseGitHubPRUrl_ValidUrls) {
  GitPlatform platform;
  std::string owner, repo;
  int pr_number;

  // Test full HTTPS URL
  ASSERT_TRUE(parse_pr_url("https://github.com/owner/repo/pull/123", platform,
                           owner, repo, pr_number));
  EXPECT_EQ(platform, GitPlatform::GitHub);
  EXPECT_EQ(owner, "owner");
  EXPECT_EQ(repo, "repo");
  EXPECT_EQ(pr_number, 123);

  // Test without https://
  ASSERT_TRUE(parse_pr_url("github.com/user/project/pull/456", platform, owner,
                           repo, pr_number));
  EXPECT_EQ(platform, GitPlatform::GitHub);
  EXPECT_EQ(owner, "user");
  EXPECT_EQ(repo, "project");
  EXPECT_EQ(pr_number, 456);

  // Test with www
  ASSERT_TRUE(parse_pr_url("https://www.github.com/testuser/testrepo/pull/789",
                           platform, owner, repo, pr_number));
  EXPECT_EQ(platform, GitPlatform::GitHub);
  EXPECT_EQ(owner, "testuser");
  EXPECT_EQ(repo, "testrepo");
  EXPECT_EQ(pr_number, 789);
}

/**
 * Test GitLab MR URL parsing with various formats
 */
TEST(GitPlatformClientTest, ParseGitLabMRUrl_ValidUrls) {
  GitPlatform platform;
  std::string owner, repo;
  int pr_number;

  // Test full HTTPS URL
  ASSERT_TRUE(parse_pr_url("https://gitlab.com/owner/repo/-/merge_requests/123",
                           platform, owner, repo, pr_number));
  EXPECT_EQ(platform, GitPlatform::GitLab);
  EXPECT_EQ(owner, "owner");
  EXPECT_EQ(repo, "repo");
  EXPECT_EQ(pr_number, 123);

  // Test with group/subgroup/project
  ASSERT_TRUE(parse_pr_url(
      "https://gitlab.com/group/subgroup/project/-/merge_requests/456",
      platform, owner, repo, pr_number));
  EXPECT_EQ(platform, GitPlatform::GitLab);
  EXPECT_EQ(owner, "group/subgroup");
  EXPECT_EQ(repo, "project");
  EXPECT_EQ(pr_number, 456);

  // Test without https://
  ASSERT_TRUE(parse_pr_url("gitlab.com/mygroup/myproject/-/merge_requests/789",
                           platform, owner, repo, pr_number));
  EXPECT_EQ(platform, GitPlatform::GitLab);
  EXPECT_EQ(owner, "mygroup");
  EXPECT_EQ(repo, "myproject");
  EXPECT_EQ(pr_number, 789);
}

/**
 * Test PR/MR URL parsing with invalid formats
 */
TEST(GitPlatformClientTest, ParsePRUrl_InvalidUrls) {
  GitPlatform platform;
  std::string owner, repo;
  int pr_number;

  // Missing PR number
  EXPECT_FALSE(parse_pr_url("https://github.com/owner/repo/pull/", platform,
                            owner, repo, pr_number));

  // Invalid format
  EXPECT_FALSE(parse_pr_url("https://github.com/owner/repo", platform, owner,
                            repo, pr_number));

  // Not a GitHub or GitLab URL
  EXPECT_FALSE(
      parse_pr_url("https://bitbucket.org/owner/repo/pull-requests/123",
                   platform, owner, repo, pr_number));

  // Empty string
  EXPECT_FALSE(parse_pr_url("", platform, owner, repo, pr_number));

  // Wrong path for GitLab
  EXPECT_FALSE(parse_pr_url("https://gitlab.com/owner/repo/pull/123", platform,
                            owner, repo, pr_number));
}

/**
 * Test PR/MR URL with special characters in owner/repo names
 */
TEST(GitPlatformClientTest, ParsePRUrl_SpecialCharacters) {
  GitPlatform platform;
  std::string owner, repo;
  int pr_number;

  // GitHub: Underscores and hyphens
  ASSERT_TRUE(
      parse_pr_url("https://github.com/my-owner_123/my-repo_456/pull/999",
                   platform, owner, repo, pr_number));
  EXPECT_EQ(platform, GitPlatform::GitHub);
  EXPECT_EQ(owner, "my-owner_123");
  EXPECT_EQ(repo, "my-repo_456");
  EXPECT_EQ(pr_number, 999);

  // GitLab: Complex group paths
  ASSERT_TRUE(parse_pr_url(
      "https://gitlab.com/org-name/team-1/my_project/-/merge_requests/100",
      platform, owner, repo, pr_number));
  EXPECT_EQ(platform, GitPlatform::GitLab);
  EXPECT_EQ(owner, "org-name/team-1");
  EXPECT_EQ(repo, "my_project");
  EXPECT_EQ(pr_number, 100);
}
