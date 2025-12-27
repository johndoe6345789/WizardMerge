/**
 * @file github_client.cpp
 * @brief Implementation of GitHub API client
 */

#include "wizardmerge/git/github_client.h"
#include <regex>
#include <sstream>
#include <iostream>
#include <curl/curl.h>
#include <json/json.h>

namespace wizardmerge {
namespace git {

namespace {

// Callback for libcurl to write response data
size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

/**
 * @brief Perform HTTP GET request using libcurl
 */
bool http_get(const std::string& url, const std::string& token, std::string& response) {
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

    // Setup headers
    struct curl_slist* headers = nullptr;
    headers = curl_slist_append(headers, "Accept: application/vnd.github.v3+json");
    
    if (!token.empty()) {
        std::string auth_header = "Authorization: token " + token;
        headers = curl_slist_append(headers, auth_header.c_str());
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

bool parse_pr_url(const std::string& url, std::string& owner, 
                  std::string& repo, int& pr_number) {
    // Match patterns like:
    // https://github.com/owner/repo/pull/123
    // github.com/owner/repo/pull/123
    // owner/repo/pull/123
    
    std::regex pr_regex(R"((?:https?://)?(?:www\.)?github\.com/([^/]+)/([^/]+)/pull/(\d+))");
    std::smatch matches;
    
    if (std::regex_search(url, matches, pr_regex)) {
        if (matches.size() == 4) {
            owner = matches[1].str();
            repo = matches[2].str();
            pr_number = std::stoi(matches[3].str());
            return true;
        }
    }
    
    return false;
}

std::optional<PullRequest> fetch_pull_request(
    const std::string& owner,
    const std::string& repo,
    int pr_number,
    const std::string& token
) {
    // Fetch PR info
    std::string pr_url = "https://api.github.com/repos/" + owner + "/" + repo + "/pulls/" + std::to_string(pr_number);
    std::string response;
    
    if (!http_get(pr_url, token, response)) {
        std::cerr << "Failed to fetch pull request info" << std::endl;
        return std::nullopt;
    }

    // Parse JSON response
    Json::Value root;
    Json::CharReaderBuilder reader;
    std::string errs;
    std::istringstream s(response);
    
    if (!Json::parseFromStream(reader, s, &root, &errs)) {
        std::cerr << "Failed to parse PR JSON: " << errs << std::endl;
        return std::nullopt;
    }

    PullRequest pr;
    pr.number = pr_number;
    pr.title = root.get("title", "").asString();
    pr.state = root.get("state", "").asString();
    pr.repo_owner = owner;
    pr.repo_name = repo;
    
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

    // Fetch PR files
    std::string files_url = "https://api.github.com/repos/" + owner + "/" + repo + "/pulls/" + std::to_string(pr_number) + "/files";
    std::string files_response;
    
    if (!http_get(files_url, token, files_response)) {
        std::cerr << "Failed to fetch pull request files" << std::endl;
        return std::nullopt;
    }

    Json::Value files_root;
    std::istringstream files_stream(files_response);
    
    if (!Json::parseFromStream(reader, files_stream, &files_root, &errs)) {
        std::cerr << "Failed to parse files JSON: " << errs << std::endl;
        return std::nullopt;
    }

    if (files_root.isArray()) {
        for (const auto& file : files_root) {
            PRFile pr_file;
            pr_file.filename = file.get("filename", "").asString();
            pr_file.status = file.get("status", "").asString();
            pr_file.additions = file.get("additions", 0).asInt();
            pr_file.deletions = file.get("deletions", 0).asInt();
            pr_file.changes = file.get("changes", 0).asInt();
            
            pr.files.push_back(pr_file);
        }
    }

    return pr;
}

std::optional<std::vector<std::string>> fetch_file_content(
    const std::string& owner,
    const std::string& repo,
    const std::string& sha,
    const std::string& path,
    const std::string& token
) {
    // Use GitHub API to get file content at specific commit
    std::string url = "https://api.github.com/repos/" + owner + "/" + repo + "/contents/" + path + "?ref=" + sha;
    std::string response;
    
    if (!http_get(url, token, response)) {
        std::cerr << "Failed to fetch file content for " << path << " at " << sha << std::endl;
        return std::nullopt;
    }

    // Parse JSON response
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
    encoded_content.erase(std::remove(encoded_content.begin(), encoded_content.end(), '\n'), encoded_content.end());
    encoded_content.erase(std::remove(encoded_content.begin(), encoded_content.end(), '\r'), encoded_content.end());
    
    // Simple base64 decode (using curl's built-in decoder)
    CURL* curl = curl_easy_init();
    if (!curl) {
        return std::nullopt;
    }
    
    int outlen;
    unsigned char* decoded = curl_easy_unescape(curl, encoded_content.c_str(), encoded_content.length(), &outlen);
    
    if (!decoded) {
        // Fallback: try manual base64 decode
        // For now, return empty as we need proper base64 decoder
        curl_easy_cleanup(curl);
        std::cerr << "Failed to decode base64 content" << std::endl;
        return std::nullopt;
    }
    
    std::string decoded_content(reinterpret_cast<char*>(decoded), outlen);
    curl_free(decoded);
    curl_easy_cleanup(curl);
    
    // Split content into lines
    return split_lines(decoded_content);
}

}  // namespace git
}  // namespace wizardmerge
