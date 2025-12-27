/**
 * @file git_cli.h
 * @brief Git CLI wrapper for repository operations
 * 
 * Provides C++ wrapper functions for Git command-line operations including
 * cloning, branching, committing, and pushing changes.
 */

#ifndef WIZARDMERGE_GIT_CLI_H
#define WIZARDMERGE_GIT_CLI_H

#include <string>
#include <vector>
#include <optional>

namespace wizardmerge {
namespace git {

/**
 * @brief Result of a Git operation
 */
struct GitResult {
    bool success;
    std::string output;
    std::string error;
    int exit_code;
};

/**
 * @brief Configuration for Git operations
 */
struct GitConfig {
    std::string user_name;
    std::string user_email;
    std::string auth_token;  // For HTTPS authentication
};

/**
 * @brief Clone a Git repository
 * 
 * @param url Repository URL (HTTPS or SSH)
 * @param destination Local directory path
 * @param branch Optional specific branch to clone
 * @param depth Optional shallow clone depth (0 for full clone)
 * @return GitResult with operation status
 */
GitResult clone_repository(
    const std::string& url,
    const std::string& destination,
    const std::string& branch = "",
    int depth = 0
);

/**
 * @brief Create and checkout a new branch
 * 
 * @param repo_path Path to the Git repository
 * @param branch_name Name of the new branch
 * @param base_branch Optional base branch (defaults to current branch)
 * @return GitResult with operation status
 */
GitResult create_branch(
    const std::string& repo_path,
    const std::string& branch_name,
    const std::string& base_branch = ""
);

/**
 * @brief Checkout an existing branch
 * 
 * @param repo_path Path to the Git repository
 * @param branch_name Name of the branch to checkout
 * @return GitResult with operation status
 */
GitResult checkout_branch(
    const std::string& repo_path,
    const std::string& branch_name
);

/**
 * @brief Stage files for commit
 * 
 * @param repo_path Path to the Git repository
 * @param files Vector of file paths (relative to repo root)
 * @return GitResult with operation status
 */
GitResult add_files(
    const std::string& repo_path,
    const std::vector<std::string>& files
);

/**
 * @brief Commit staged changes
 * 
 * @param repo_path Path to the Git repository
 * @param message Commit message
 * @param config Optional Git configuration
 * @return GitResult with operation status
 */
GitResult commit(
    const std::string& repo_path,
    const std::string& message,
    const GitConfig& config = GitConfig()
);

/**
 * @brief Push commits to remote repository
 * 
 * @param repo_path Path to the Git repository
 * @param remote Remote name (default: "origin")
 * @param branch Branch name to push
 * @param force Force push if needed
 * @param config Optional Git configuration with auth token
 * @return GitResult with operation status
 */
GitResult push(
    const std::string& repo_path,
    const std::string& remote,
    const std::string& branch,
    bool force = false,
    const GitConfig& config = GitConfig()
);

/**
 * @brief Get current branch name
 * 
 * @param repo_path Path to the Git repository
 * @return Current branch name, or empty optional on error
 */
std::optional<std::string> get_current_branch(const std::string& repo_path);

/**
 * @brief Check if a branch exists
 * 
 * @param repo_path Path to the Git repository
 * @param branch_name Name of the branch to check
 * @return true if branch exists, false otherwise
 */
bool branch_exists(const std::string& repo_path, const std::string& branch_name);

/**
 * @brief Get repository status
 * 
 * @param repo_path Path to the Git repository
 * @return GitResult with status output
 */
GitResult status(const std::string& repo_path);

/**
 * @brief Check if Git is available in system PATH
 * 
 * @return true if git command is available, false otherwise
 */
bool is_git_available();

}  // namespace git
}  // namespace wizardmerge

#endif  // WIZARDMERGE_GIT_CLI_H
