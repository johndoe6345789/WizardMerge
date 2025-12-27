/**
 * @file github_client.h
 * @brief GitHub API client for fetching pull request information
 */

#ifndef WIZARDMERGE_GIT_GITHUB_CLIENT_H
#define WIZARDMERGE_GIT_GITHUB_CLIENT_H

#include <string>
#include <vector>
#include <map>
#include <optional>

namespace wizardmerge {
namespace git {

/**
 * @brief Information about a file in a pull request
 */
struct PRFile {
    std::string filename;
    std::string status;  // "added", "modified", "removed", "renamed"
    std::string base_content;
    std::string head_content;
    int additions;
    int deletions;
    int changes;
};

/**
 * @brief Pull request information from GitHub
 */
struct PullRequest {
    int number;
    std::string title;
    std::string state;
    std::string base_ref;  // Base branch name
    std::string head_ref;  // Head branch name
    std::string base_sha;
    std::string head_sha;
    std::string repo_owner;
    std::string repo_name;
    std::vector<PRFile> files;
    bool mergeable;
    std::string mergeable_state;
};

/**
 * @brief Parse GitHub pull request URL
 * 
 * Extracts owner, repo, and PR number from URLs like:
 * - https://github.com/owner/repo/pull/123
 * - github.com/owner/repo/pull/123
 * 
 * @param url The pull request URL
 * @param owner Output repository owner
 * @param repo Output repository name
 * @param pr_number Output PR number
 * @return true if successfully parsed, false otherwise
 */
bool parse_pr_url(const std::string& url, std::string& owner, 
                  std::string& repo, int& pr_number);

/**
 * @brief Fetch pull request information from GitHub API
 * 
 * @param owner Repository owner
 * @param repo Repository name
 * @param pr_number Pull request number
 * @param token Optional GitHub API token for authentication
 * @return Pull request information, or empty optional on error
 */
std::optional<PullRequest> fetch_pull_request(
    const std::string& owner,
    const std::string& repo,
    int pr_number,
    const std::string& token = ""
);

/**
 * @brief Fetch file content from GitHub at a specific commit
 * 
 * @param owner Repository owner
 * @param repo Repository name
 * @param sha Commit SHA
 * @param path File path
 * @param token Optional GitHub API token
 * @return File content as vector of lines, or empty optional on error
 */
std::optional<std::vector<std::string>> fetch_file_content(
    const std::string& owner,
    const std::string& repo,
    const std::string& sha,
    const std::string& path,
    const std::string& token = ""
);

}  // namespace git
}  // namespace wizardmerge

#endif  // WIZARDMERGE_GIT_GITHUB_CLIENT_H
