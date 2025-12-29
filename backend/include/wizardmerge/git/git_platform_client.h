/**
 * @file git_platform_client.h
 * @brief Git platform API client for fetching pull/merge request information
 *
 * Supports GitHub and GitLab platforms
 */

#ifndef WIZARDMERGE_GIT_PLATFORM_CLIENT_H
#define WIZARDMERGE_GIT_PLATFORM_CLIENT_H

#include <map>
#include <optional>
#include <string>
#include <vector>

namespace wizardmerge {
namespace git {

/**
 * @brief Supported git platforms
 */
enum class GitPlatform { GitHub, GitLab, Unknown };

/**
 * @brief Information about a file in a pull/merge request
 */
struct PRFile {
  std::string filename;
  std::string status; // "added", "modified", "removed", "renamed"
  int additions;
  int deletions;
  int changes;
};

/**
 * @brief Pull/merge request information from GitHub or GitLab
 */
struct PullRequest {
  GitPlatform platform;
  int number;
  std::string title;
  std::string state;
  std::string base_ref; // Base branch name
  std::string head_ref; // Head branch name
  std::string base_sha;
  std::string head_sha;
  std::string repo_owner;
  std::string repo_name;
  std::vector<PRFile> files;
  bool mergeable;
  std::string mergeable_state;
};

/**
 * @brief Parse pull/merge request URL
 *
 * Extracts platform, owner, repo, and PR/MR number from URLs like:
 * - https://github.com/owner/repo/pull/123
 * - https://gitlab.com/owner/repo/-/merge_requests/456
 * - github.com/owner/repo/pull/123
 * - gitlab.com/group/subgroup/project/-/merge_requests/789
 *
 * @param url The pull/merge request URL
 * @param platform Output git platform
 * @param owner Output repository owner/group
 * @param repo Output repository name/project
 * @param pr_number Output PR/MR number
 * @return true if successfully parsed, false otherwise
 */
bool parse_pr_url(const std::string &url, GitPlatform &platform,
                  std::string &owner, std::string &repo, int &pr_number);

/**
 * @brief Fetch pull/merge request information from GitHub or GitLab API
 *
 * @param platform Git platform (GitHub or GitLab)
 * @param owner Repository owner/group
 * @param repo Repository name/project
 * @param pr_number Pull/merge request number
 * @param token Optional API token for authentication
 * @return Pull request information, or empty optional on error
 */
std::optional<PullRequest> fetch_pull_request(GitPlatform platform,
                                              const std::string &owner,
                                              const std::string &repo,
                                              int pr_number,
                                              const std::string &token = "");

/**
 * @brief Fetch file content from GitHub or GitLab at a specific commit
 *
 * @param platform Git platform (GitHub or GitLab)
 * @param owner Repository owner/group
 * @param repo Repository name/project
 * @param sha Commit SHA
 * @param path File path
 * @param token Optional API token
 * @return File content as vector of lines, or empty optional on error
 */
std::optional<std::vector<std::string>>
fetch_file_content(GitPlatform platform, const std::string &owner,
                   const std::string &repo, const std::string &sha,
                   const std::string &path, const std::string &token = "");

} // namespace git
} // namespace wizardmerge

#endif // WIZARDMERGE_GIT_PLATFORM_CLIENT_H
