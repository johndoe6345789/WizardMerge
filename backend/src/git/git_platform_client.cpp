/**
 * @file git_platform_client.cpp
 * @brief Implementation of git platform API client for GitHub and GitLab
 */

#include "wizardmerge/git/git_platform_client.h"
#include <regex>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <optional>
#include <vector>
#include <curl/curl.h>

namespace wizardmerge {
namespace git {

namespace {

std::optional<std::string> extract_object_segment(const std::string& json,
                                                  const std::string& key) {
    auto key_pos = json.find("\"" + key + "\"");
    if (key_pos == std::string::npos) {
        return std::nullopt;
    }

    auto brace_pos = json.find('{', key_pos);
    if (brace_pos == std::string::npos) {
        return std::nullopt;
    }

    int depth = 0;
    for (size_t i = brace_pos; i < json.size(); ++i) {
        if (json[i] == '{') {
            depth++;
        } else if (json[i] == '}') {
            depth--;
            if (depth == 0) {
                return json.substr(brace_pos, i - brace_pos + 1);
            }
        }
    }

    return std::nullopt;
}

std::optional<std::string> extract_array_segment(const std::string& json,
                                                 const std::string& key) {
    auto key_pos = json.find("\"" + key + "\"");
    if (key_pos == std::string::npos) {
        return std::nullopt;
    }

    auto bracket_pos = json.find('[', key_pos);
    if (bracket_pos == std::string::npos) {
        return std::nullopt;
    }

    int depth = 0;
    for (size_t i = bracket_pos; i < json.size(); ++i) {
        if (json[i] == '[') {
            depth++;
        } else if (json[i] == ']') {
            depth--;
            if (depth == 0) {
                return json.substr(bracket_pos, i - bracket_pos + 1);
            }
        }
    }

    return std::nullopt;
}

std::vector<std::string> extract_objects_from_array(const std::string& array) {
    std::vector<std::string> objects;

    int depth = 0;
    std::optional<size_t> start;

    for (size_t i = 0; i < array.size(); ++i) {
        if (array[i] == '{') {
            if (!start) {
                start = i;
            }
            depth++;
        } else if (array[i] == '}') {
            depth--;
            if (depth == 0 && start) {
                objects.push_back(array.substr(*start, i - *start + 1));
                start.reset();
            }
        }
    }

    return objects;
}

std::string extract_string_field(const std::string& json,
                                 const std::string& key,
                                 const std::string& default_value = "") {
    std::regex pattern("\\\\\"" + key + "\\\\\"\\s*:\\s*\\\\\"([^\\\\\"]*)\\\\\"");
    std::smatch match;

    if (std::regex_search(json, match, pattern) && match.size() >= 2) {
        return match[1];
    }

    return default_value;
}

bool extract_bool_field(const std::string& json,
                        const std::string& key,
                        bool default_value = false) {
    std::regex pattern("\\\\\"" + key + "\\\\\"\\s*:\\s*(true|false)");
    std::smatch match;

    if (std::regex_search(json, match, pattern) && match.size() >= 2) {
        return match[1] == "true";
    }

    return default_value;
}

int extract_int_field(const std::string& json,
                      const std::string& key,
                      int default_value = 0) {
    std::regex pattern("\\\\\"" + key + "\\\\\"\\s*:\\s*(-?\\d+)");
    std::smatch match;

    if (std::regex_search(json, match, pattern) && match.size() >= 2) {
        try {
            return std::stoi(match[1]);
        } catch (const std::exception&) {
        }
    }

    return default_value;
}

/**
 * @brief Simple base64 decoder
 */
std::string base64_decode(const std::string& encoded) {
    static const std::string base64_chars = 
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz"
        "0123456789+/";
    
    std::string decoded;
    std::vector<int> T(256, -1);
    for (int i = 0; i < 64; i++) T[base64_chars[i]] = i;
    
    int val = 0, valb = -8;
    for (unsigned char c : encoded) {
        if (T[c] == -1) break;
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
size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

/**
 * @brief Perform HTTP GET request using libcurl
 */
bool http_get(const std::string& url, const std::string& token, std::string& response, GitPlatform platform = GitPlatform::GitHub) {
    CURL* curl = curl_easy_init();
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
    struct curl_slist* headers = nullptr;
    
    if (platform == GitPlatform::GitHub) {
        headers = curl_slist_append(headers, "Accept: application/vnd.github.v3+json");
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
std::vector<std::string> split_lines(const std::string& content) {
    std::vector<std::string> lines;
    std::istringstream stream(content);
    std::string line;
    
    while (std::getline(stream, line)) {
        lines.push_back(line);
    }
    
    return lines;
}

}  // anonymous namespace

bool parse_pr_url(const std::string& url, GitPlatform& platform,
                  std::string& owner, std::string& repo, int& pr_number) {
    // Try GitHub pattern first:
    // https://github.com/owner/repo/pull/123
    // github.com/owner/repo/pull/123
    
    std::regex github_regex(R"((?:https?://)?(?:www\.)?github\.com/([^/]+)/([^/]+)/pull/(\d+))");
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
    
    std::regex gitlab_regex(R"((?:https?://)?(?:www\.)?gitlab\.com/([^/-]+(?:/[^/-]+)*?)/-/merge_requests/(\d+))");
    
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

std::optional<PullRequest> fetch_pull_request(
    GitPlatform platform,
    const std::string& owner,
    const std::string& repo,
    int pr_number,
    const std::string& token
) {
    PullRequest pr;
    pr.platform = platform;
    pr.number = pr_number;
    pr.repo_owner = owner;
    pr.repo_name = repo;
    
    std::string pr_url, files_url;
    
    if (platform == GitPlatform::GitHub) {
        // GitHub API URLs
        pr_url = "https://api.github.com/repos/" + owner + "/" + repo + "/pulls/" + std::to_string(pr_number);
        files_url = "https://api.github.com/repos/" + owner + "/" + repo + "/pulls/" + std::to_string(pr_number) + "/files";
    } else if (platform == GitPlatform::GitLab) {
        // GitLab API URLs - encode project path
        std::string project_path = owner;
        if (!repo.empty()) {
            project_path += "/" + repo;
        }
        // URL encode the project path
        CURL* curl = curl_easy_init();
        char* encoded = curl_easy_escape(curl, project_path.c_str(), project_path.length());
        std::string encoded_project = encoded;
        curl_free(encoded);
        curl_easy_cleanup(curl);
        
        pr_url = "https://gitlab.com/api/v4/projects/" + encoded_project + "/merge_requests/" + std::to_string(pr_number);
        files_url = "https://gitlab.com/api/v4/projects/" + encoded_project + "/merge_requests/" + std::to_string(pr_number) + "/changes";
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

    pr.title = extract_string_field(response, "title");
    pr.state = extract_string_field(response, "state");

    if (platform == GitPlatform::GitHub) {
        if (auto base_object = extract_object_segment(response, "base")) {
            pr.base_ref = extract_string_field(*base_object, "ref");
            pr.base_sha = extract_string_field(*base_object, "sha");
        }

        if (auto head_object = extract_object_segment(response, "head")) {
            pr.head_ref = extract_string_field(*head_object, "ref");
            pr.head_sha = extract_string_field(*head_object, "sha");
        }

        pr.mergeable = extract_bool_field(response, "mergeable", false);
        pr.mergeable_state = extract_string_field(response, "mergeable_state", "unknown");
    } else if (platform == GitPlatform::GitLab) {
        pr.base_ref = extract_string_field(response, "target_branch");
        pr.head_ref = extract_string_field(response, "source_branch");

        if (auto diff_refs = extract_object_segment(response, "diff_refs")) {
            pr.base_sha = extract_string_field(*diff_refs, "base_sha");
            pr.head_sha = extract_string_field(*diff_refs, "head_sha");
        }

        std::string merge_status = extract_string_field(response, "merge_status");
        pr.mergeable = (merge_status == "can_be_merged");
        pr.mergeable_state = merge_status;
    }

    // Fetch PR/MR files
    std::string files_response;
    
    if (!http_get(files_url, token, files_response, platform)) {
        std::cerr << "Failed to fetch pull/merge request files" << std::endl;
        return std::nullopt;
    }

    // Process files based on platform
    if (platform == GitPlatform::GitHub) {
        for (const auto& file_object : extract_objects_from_array(files_response)) {
            PRFile pr_file;
            pr_file.filename = extract_string_field(file_object, "filename");
            pr_file.status = extract_string_field(file_object, "status");
            pr_file.additions = extract_int_field(file_object, "additions");
            pr_file.deletions = extract_int_field(file_object, "deletions");
            pr_file.changes = extract_int_field(file_object, "changes");

            pr.files.push_back(pr_file);
        }
    } else if (platform == GitPlatform::GitLab) {
        if (auto changes_array = extract_array_segment(files_response, "changes")) {
            for (const auto& file_object : extract_objects_from_array(*changes_array)) {
                PRFile pr_file;
                std::string new_path = extract_string_field(file_object, "new_path");
                if (!new_path.empty()) {
                    pr_file.filename = new_path;
                } else {
                    pr_file.filename = extract_string_field(file_object, "old_path");
                }

                bool new_file = extract_bool_field(file_object, "new_file", false);
                bool deleted_file = extract_bool_field(file_object, "deleted_file", false);
                bool renamed_file = extract_bool_field(file_object, "renamed_file", false);

                if (new_file) {
                    pr_file.status = "added";
                } else if (deleted_file) {
                    pr_file.status = "removed";
                } else if (renamed_file) {
                    pr_file.status = "renamed";
                } else {
                    pr_file.status = "modified";
                }

                pr_file.additions = 0;
                pr_file.deletions = 0;
                pr_file.changes = 0;

                pr.files.push_back(pr_file);
            }
        }
    }

    if (platform == GitPlatform::GitHub || platform == GitPlatform::GitLab) {
        // If parsing failed and we have no files, signal an error to callers.
        if (pr.files.empty() && !files_response.empty()) {
            std::cerr << "No files parsed from API response" << std::endl;
        }
    }

    return pr;
}

std::optional<std::vector<std::string>> fetch_file_content(
    GitPlatform platform,
    const std::string& owner,
    const std::string& repo,
    const std::string& sha,
    const std::string& path,
    const std::string& token
) {
    std::string url;
    
    if (platform == GitPlatform::GitHub) {
        // GitHub API URL
        url = "https://api.github.com/repos/" + owner + "/" + repo + "/contents/" + path + "?ref=" + sha;
    } else if (platform == GitPlatform::GitLab) {
        // GitLab API URL - encode project path and file path
        std::string project_path = owner;
        if (!repo.empty()) {
            project_path += "/" + repo;
        }
        
        CURL* curl = curl_easy_init();
        char* encoded_project = curl_easy_escape(curl, project_path.c_str(), project_path.length());
        char* encoded_path = curl_easy_escape(curl, path.c_str(), path.length());
        
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
        std::cerr << "Failed to fetch file content for " << path << " at " << sha << std::endl;
        return std::nullopt;
    }

    // Handle response based on platform
    if (platform == GitPlatform::GitHub) {
        // GitHub returns JSON with base64-encoded content
        std::string encoding = extract_string_field(response, "encoding");
        std::string encoded_content = extract_string_field(response, "content");

        if (encoding.empty() || encoded_content.empty()) {
            std::cerr << "Invalid response format for file content" << std::endl;
            return std::nullopt;
        }

        if (encoding != "base64") {
            std::cerr << "Unsupported encoding: " << encoding << std::endl;
            return std::nullopt;
        }

        // Remove newlines from base64 string
        encoded_content.erase(std::remove(encoded_content.begin(), encoded_content.end(), '\n'), encoded_content.end());
        encoded_content.erase(std::remove(encoded_content.begin(), encoded_content.end(), '\r'), encoded_content.end());

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

}  // namespace git
}  // namespace wizardmerge
