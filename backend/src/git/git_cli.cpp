/**
 * @file git_cli.cpp
 * @brief Implementation of Git CLI wrapper functions
 */

#include "wizardmerge/git/git_cli.h"
#include <array>
#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <sstream>
#include <sys/wait.h>

namespace wizardmerge {
namespace git {

namespace {

/**
 * @brief Execute a shell command and capture output
 */
GitResult execute_command(const std::string &command) {
  GitResult result;
  result.exit_code = 0;

  // Execute command and capture output
  std::array<char, 128> buffer;
  std::string output;

  FILE *pipe = popen((command + " 2>&1").c_str(), "r");
  if (!pipe) {
    result.success = false;
    result.error = "Failed to execute command";
    result.exit_code = -1;
    return result;
  }

  while (fgets(buffer.data(), buffer.size(), pipe) != nullptr) {
    output += buffer.data();
  }

  int status = pclose(pipe);
  result.exit_code = WEXITSTATUS(status);
  result.success = (result.exit_code == 0);
  result.output = output;

  if (!result.success) {
    result.error = output;
  }

  return result;
}

/**
 * @brief Build git command with working directory
 */
std::string git_command(const std::string &repo_path, const std::string &cmd) {
  if (repo_path.empty()) {
    return "git " + cmd;
  }
  return "git -C \"" + repo_path + "\" " + cmd;
}

} // anonymous namespace

bool is_git_available() {
  GitResult result = execute_command("git --version");
  return result.success;
}

GitResult clone_repository(const std::string &url,
                           const std::string &destination,
                           const std::string &branch, int depth) {
  std::ostringstream cmd;
  cmd << "git clone";

  if (!branch.empty()) {
    cmd << " --branch \"" << branch << "\"";
  }

  if (depth > 0) {
    cmd << " --depth " << depth;
  }

  cmd << " \"" << url << "\" \"" << destination << "\"";

  return execute_command(cmd.str());
}

GitResult create_branch(const std::string &repo_path,
                        const std::string &branch_name,
                        const std::string &base_branch) {
  std::ostringstream cmd;
  cmd << "checkout -b \"" << branch_name << "\"";

  if (!base_branch.empty()) {
    cmd << " \"" << base_branch << "\"";
  }

  return execute_command(git_command(repo_path, cmd.str()));
}

GitResult checkout_branch(const std::string &repo_path,
                          const std::string &branch_name) {
  std::string cmd = "checkout \"" + branch_name + "\"";
  return execute_command(git_command(repo_path, cmd));
}

GitResult add_files(const std::string &repo_path,
                    const std::vector<std::string> &files) {
  if (files.empty()) {
    GitResult result;
    result.success = true;
    result.output = "No files to add";
    result.exit_code = 0;
    return result;
  }

  std::ostringstream cmd;
  cmd << "add";

  for (const auto &file : files) {
    cmd << " \"" << file << "\"";
  }

  return execute_command(git_command(repo_path, cmd.str()));
}

GitResult commit(const std::string &repo_path, const std::string &message,
                 const GitConfig &config) {
  // Set user config if provided
  if (!config.user_name.empty() && !config.user_email.empty()) {
    auto name_result = execute_command(git_command(
        repo_path, "config user.name \"" + config.user_name + "\""));
    if (!name_result.success) {
      GitResult result;
      result.success = false;
      result.error = "Failed to set user.name: " + name_result.error;
      result.exit_code = name_result.exit_code;
      return result;
    }

    auto email_result = execute_command(git_command(
        repo_path, "config user.email \"" + config.user_email + "\""));
    if (!email_result.success) {
      GitResult result;
      result.success = false;
      result.error = "Failed to set user.email: " + email_result.error;
      result.exit_code = email_result.exit_code;
      return result;
    }
  }

  // Escape commit message for shell
  std::string escaped_message = message;
  size_t pos = 0;
  while ((pos = escaped_message.find("\"", pos)) != std::string::npos) {
    escaped_message.replace(pos, 1, "\\\"");
    pos += 2;
  }

  std::string cmd = "commit -m \"" + escaped_message + "\"";
  return execute_command(git_command(repo_path, cmd));
}

GitResult push(const std::string &repo_path, const std::string &remote,
               const std::string &branch, bool force, const GitConfig &config) {
  std::ostringstream cmd;
  cmd << "push";

  if (force) {
    cmd << " --force";
  }

  // Set upstream if it's a new branch
  cmd << " --set-upstream \"" << remote << "\" \"" << branch << "\"";

  std::string full_cmd = git_command(repo_path, cmd.str());

  // If auth token is provided, inject it into the URL
  // This is a simplified approach; in production, use credential helpers
  if (!config.auth_token.empty()) {
    // Note: This assumes HTTPS URLs. For production, use git credential helpers
    // or SSH keys for better security
    std::cerr << "Note: Auth token provided. Consider using credential helpers "
                 "for production."
              << std::endl;
  }

  return execute_command(full_cmd);
}

std::optional<std::string> get_current_branch(const std::string &repo_path) {
  GitResult result =
      execute_command(git_command(repo_path, "rev-parse --abbrev-ref HEAD"));

  if (!result.success) {
    return std::nullopt;
  }

  // Trim whitespace
  std::string branch = result.output;
  size_t last_non_ws = branch.find_last_not_of(" \n\r\t");

  if (last_non_ws == std::string::npos) {
    // String contains only whitespace
    return std::nullopt;
  }

  branch.erase(last_non_ws + 1);

  return branch;
}

bool branch_exists(const std::string &repo_path,
                   const std::string &branch_name) {
  std::string cmd = "rev-parse --verify \"" + branch_name + "\"";
  GitResult result = execute_command(git_command(repo_path, cmd));
  return result.success;
}

GitResult status(const std::string &repo_path) {
  return execute_command(git_command(repo_path, "status"));
}

} // namespace git
} // namespace wizardmerge
