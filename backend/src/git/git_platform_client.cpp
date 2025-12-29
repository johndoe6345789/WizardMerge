/**
 * @file git_platform_client.cpp
 * @brief Implementation of git platform API client for GitHub and GitLab
 */

#include "wizardmerge/git/git_platform_client.h"
#include <algorithm>
#include <curl/curl.h>
#include <iostream>
#include <json/json.h>
#include <regex>
#include <sstream>

namespace wizardmerge {
namespace git {

namespace {

/**
 * @brief Simple base64 decoder
 */
std::string base64_decode(const std::string &encoded) {
  static const std::string base64_chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                                          "abcdefghijklmnopqrstuvwxyz"
                                          "0123456789+/";

  std::string decoded;
  std::vector<int> T(256, -1);
  for (int i = 0; i < 64; i++)
    T[base64_chars[i]] = i;

  int val = 0, valb = -8;
  for (unsigned char c : encoded) {
    if (T[c] == -1)
      break;
    val = (val << 6) + T[c];
    valb += 6;
    if (valb >= 0) {
      decoded.push_back(char((val >> valb) & 0xFF));
      valb -= 8;
    }
  }
  return decoded;
}

// Callback for libcurl to write response data
size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp) {
  ((std::string *)userp)->append((char *)contents, size * nmemb);
  return size * nmemb;
}

/**
 * @brief Perform HTTP GET request using libcurl
 */
bool http_get(const std::string &url, const std::string &token,
              std::string &response,
              GitPlatform platform = GitPlatform::GitHub) {
  CURL *curl = curl_easy_init();
  if (!curl) {
    std::cerr << "Failed to initialize CURL" << std::endl;
    return false;
  }

  response.clear();

  curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
  curl_easy_setopt(curl, CURLOPT_USERAGENT, "WizardMerge/1.0");
  curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
  curl_easy_setopt(curl, CURLOPT_TIMEOUT, 30L);

  // Setup headers based on platform
  struct curl_slist *headers = nullptr;

  if (platform == GitPlatform::GitHub) {
    headers =
        curl_slist_append(headers, "Accept: application/vnd.github.v3+json");
    if (!token.empty()) {
      std::string auth_header = "Authorization: token " + token;
      headers = curl_slist_append(headers, auth_header.c_str());
    }
  } else if (platform == GitPlatform::GitLab) {
    headers = curl_slist_append(headers, "Accept: application/json");
    if (!token.empty()) {
      std::string auth_header = "PRIVATE-TOKEN: " + token;
      headers = curl_slist_append(headers, auth_header.c_str());
    }
  }

  curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

  CURLcode res = curl_easy_perform(curl);

  bool success = (res == CURLE_OK);
  if (!success) {
    std::cerr << "CURL error: " << curl_easy_strerror(res) << std::endl;
  }

  curl_slist_free_all(headers);
  curl_easy_cleanup(curl);

  return success;
}

/**
 * @brief Split string by newlines
 */
std::vector<std::string> split_lines(const std::string &content) {
  std::vector<std::string> lines;
  std::istringstream stream(content);
  std::string line;

  while (std::getline(stream, line)) {
    lines.push_back(line);
  }

  return lines;
}

} // anonymous namespace

bool parse_pr_url(const std::string &url, GitPlatform &platform,
                  std::string &owner, std::string &repo, int &pr_number) {
  // Try GitHub pattern first:
  // https://github.com/owner/repo/pull/123
  // github.com/owner/repo/pull/123

  std::regex github_regex(
      R"((?:https?://)?(?:www\.)?github\.com/([^/]+)/([^/]+)/pull/(\d+))");
  std::smatch matches;

  if (std::regex_search(url, matches, github_regex)) {
    if (matches.size() == 4) {
      platform = GitPlatform::GitHub;
      owner = matches[1].str();
      repo = matches[2].str();
      pr_number = std::stoi(matches[3].str());
      return true;
    }
  }

  // Try GitLab pattern:
  // https://gitlab.com/owner/repo/-/merge_requests/456
  // gitlab.com/group/subgroup/project/-/merge_requests/789

  std::regex gitlab_regex(
      R"((?:https?://)?(?:www\.)?gitlab\.com/([^/-]+(?:/[^/-]+)*?)/-/merge_requests/(\d+))");

  if (std::regex_search(url, matches, gitlab_regex)) {
    if (matches.size() == 3) {
      platform = GitPlatform::GitLab;
      std::string full_path = matches[1].str();

      // For GitLab, store the full project path
      // The path can be: owner/repo or group/subgroup/project
      // We split at the last slash to separate for potential use
      size_t last_slash = full_path.find_last_of('/');
      if (last_slash != std::string::npos) {
        owner = full_path.substr(0, last_slash);
        repo = full_path.substr(last_slash + 1);
      } else {
        // Single level project (rare but possible)
        // Store entire path as owner, repo empty
        // API calls will use full path by concatenating
        owner = full_path;
        repo = "";
      }

      pr_number = std::stoi(matches[2].str());
      return true;
    }
  }

  platform = GitPlatform::Unknown;
  return false;
}

std::optional<PullRequest> fetch_pull_request(GitPlatform platform,
                                              const std::string &owner,
                                              const std::string &repo,
                                              int pr_number,
                                              const std::string &token) {
  PullRequest pr;
  pr.platform = platform;
  pr.number = pr_number;
  pr.repo_owner = owner;
  pr.repo_name = repo;

  std::string pr_url, files_url;

  if (platform == GitPlatform::GitHub) {
    // GitHub API URLs
    pr_url = "https://api.github.com/repos/" + owner + "/" + repo + "/pulls/" +
             std::to_string(pr_number);
    files_url = "https://api.github.com/repos/" + owner + "/" + repo +
                "/pulls/" + std::to_string(pr_number) + "/files";
  } else if (platform == GitPlatform::GitLab) {
    // GitLab API URLs - encode project path
    std::string project_path = owner;
    if (!repo.empty()) {
      project_path += "/" + repo;
    }
    // URL encode the project path
    CURL *curl = curl_easy_init();
    char *encoded =
        curl_easy_escape(curl, project_path.c_str(), project_path.length());
    std::string encoded_project = encoded;
    curl_free(encoded);
    curl_easy_cleanup(curl);

    pr_url = "https://gitlab.com/api/v4/projects/" + encoded_project +
             "/merge_requests/" + std::to_string(pr_number);
    files_url = "https://gitlab.com/api/v4/projects/" + encoded_project +
                "/merge_requests/" + std::to_string(pr_number) + "/changes";
  } else {
    std::cerr << "Unknown platform" << std::endl;
    return std::nullopt;
  }

  // Fetch PR/MR info
  std::string response;
  if (!http_get(pr_url, token, response, platform)) {
    std::cerr << "Failed to fetch pull/merge request info" << std::endl;
    return std::nullopt;
  }

  // Parse JSON response
  Json::Value root;
  Json::CharReaderBuilder reader;
  std::string errs;
  std::istringstream s(response);

  if (!Json::parseFromStream(reader, s, &root, &errs)) {
    std::cerr << "Failed to parse PR/MR JSON: " << errs << std::endl;
    return std::nullopt;
  }

  pr.title = root.get("title", "").asString();
  pr.state = root.get("state", "").asString();

  if (platform == GitPlatform::GitHub) {
    if (root.isMember("base") && root["base"].isObject()) {
      pr.base_ref = root["base"].get("ref", "").asString();
      pr.base_sha = root["base"].get("sha", "").asString();
    }

    if (root.isMember("head") && root["head"].isObject()) {
      pr.head_ref = root["head"].get("ref", "").asString();
      pr.head_sha = root["head"].get("sha", "").asString();
    }

    pr.mergeable = root.get("mergeable", false).asBool();
    pr.mergeable_state = root.get("mergeable_state", "unknown").asString();
  } else if (platform == GitPlatform::GitLab) {
    pr.base_ref = root.get("target_branch", "").asString();
    pr.head_ref = root.get("source_branch", "").asString();
    pr.base_sha =
        root.get("diff_refs", Json::Value::null).get("base_sha", "").asString();
    pr.head_sha =
        root.get("diff_refs", Json::Value::null).get("head_sha", "").asString();

    // GitLab uses different merge status
    std::string merge_status = root.get("merge_status", "").asString();
    pr.mergeable = (merge_status == "can_be_merged");
    pr.mergeable_state = merge_status;
  }

  // Fetch PR/MR files
  std::string files_response;

  if (!http_get(files_url, token, files_response, platform)) {
    std::cerr << "Failed to fetch pull/merge request files" << std::endl;
    return std::nullopt;
  }

  Json::Value files_root;
  std::istringstream files_stream(files_response);

  if (!Json::parseFromStream(reader, files_stream, &files_root, &errs)) {
    std::cerr << "Failed to parse files JSON: " << errs << std::endl;
    return std::nullopt;
  }

  // Process files based on platform
  if (platform == GitPlatform::GitHub && files_root.isArray()) {
    // GitHub format: array of file objects
    for (const auto &file : files_root) {
      PRFile pr_file;
      pr_file.filename = file.get("filename", "").asString();
      pr_file.status = file.get("status", "").asString();
      pr_file.additions = file.get("additions", 0).asInt();
      pr_file.deletions = file.get("deletions", 0).asInt();
      pr_file.changes = file.get("changes", 0).asInt();

      pr.files.push_back(pr_file);
    }
  } else if (platform == GitPlatform::GitLab &&
             files_root.isMember("changes")) {
    // GitLab format: object with "changes" array
    const Json::Value &changes = files_root["changes"];
    if (changes.isArray()) {
      for (const auto &file : changes) {
        PRFile pr_file;
        pr_file.filename =
            file.get("new_path", file.get("old_path", "").asString())
                .asString();

        // Determine status from new_file, deleted_file, renamed_file flags
        bool new_file = file.get("new_file", false).asBool();
        bool deleted_file = file.get("deleted_file", false).asBool();
        bool renamed_file = file.get("renamed_file", false).asBool();

        if (new_file) {
          pr_file.status = "added";
        } else if (deleted_file) {
          pr_file.status = "removed";
        } else if (renamed_file) {
          pr_file.status = "renamed";
        } else {
          pr_file.status = "modified";
        }

        // GitLab doesn't provide addition/deletion counts in the changes
        // endpoint
        pr_file.additions = 0;
        pr_file.deletions = 0;
        pr_file.changes = 0;

        pr.files.push_back(pr_file);
      }
    }
  }
}

return pr;
}

std::optional<std::vector<std::string>>
fetch_file_content(GitPlatform platform, const std::string &owner,
                   const std::string &repo, const std::string &sha,
                   const std::string &path, const std::string &token) {
  std::string url;

  if (platform == GitPlatform::GitHub) {
    // GitHub API URL
    url = "https://api.github.com/repos/" + owner + "/" + repo + "/contents/" +
          path + "?ref=" + sha;
  } else if (platform == GitPlatform::GitLab) {
    // GitLab API URL - encode project path and file path
    std::string project_path = owner;
    if (!repo.empty()) {
      project_path += "/" + repo;
    }

    CURL *curl = curl_easy_init();
    char *encoded_project =
        curl_easy_escape(curl, project_path.c_str(), project_path.length());
    char *encoded_path = curl_easy_escape(curl, path.c_str(), path.length());

    url = "https://gitlab.com/api/v4/projects/" + std::string(encoded_project) +
          "/repository/files/" + std::string(encoded_path) + "/raw?ref=" + sha;

    curl_free(encoded_project);
    curl_free(encoded_path);
    curl_easy_cleanup(curl);
  } else {
    std::cerr << "Unknown platform" << std::endl;
    return std::nullopt;
  }

  std::string response;

  if (!http_get(url, token, response, platform)) {
    std::cerr << "Failed to fetch file content for " << path << " at " << sha
              << std::endl;
    return std::nullopt;
  }

  // Handle response based on platform
  if (platform == GitPlatform::GitHub) {
    // GitHub returns JSON with base64-encoded content
    Json::Value root;
    Json::CharReaderBuilder reader;
    std::string errs;
    std::istringstream s(response);

    if (!Json::parseFromStream(reader, s, &root, &errs)) {
      std::cerr << "Failed to parse content JSON: " << errs << std::endl;
      return std::nullopt;
    }

    // GitHub API returns content as base64 encoded
    if (!root.isMember("content") || !root.isMember("encoding")) {
      std::cerr << "Invalid response format for file content" << std::endl;
      return std::nullopt;
    }

    std::string encoding = root["encoding"].asString();
    if (encoding != "base64") {
      std::cerr << "Unsupported encoding: " << encoding << std::endl;
      return std::nullopt;
    }

    // Decode base64 content
    std::string encoded_content = root["content"].asString();

    // Remove newlines from base64 string
    encoded_content.erase(
        std::remove(encoded_content.begin(), encoded_content.end(), '\n'),
        encoded_content.end());
    encoded_content.erase(
        std::remove(encoded_content.begin(), encoded_content.end(), '\r'),
        encoded_content.end());

    // Decode base64
    std::string decoded_content = base64_decode(encoded_content);

    if (decoded_content.empty()) {
      std::cerr << "Failed to decode base64 content" << std::endl;
      return std::nullopt;
    }

    // Split content into lines
    return split_lines(decoded_content);
  } else if (platform == GitPlatform::GitLab) {
    // GitLab returns raw file content directly
    return split_lines(response);
  }

  return std::nullopt;
}

} // namespace git
} // namespace wizardmerge
