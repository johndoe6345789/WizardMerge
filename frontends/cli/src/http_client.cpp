#include "http_client.h"
#include <curl/curl.h>
#include <iostream>
#include <sstream>

// Callback for libcurl to write response data
static size_t WriteCallback(void *contents, size_t size, size_t nmemb,
                            void *userp) {
  ((std::string *)userp)->append((char *)contents, size * nmemb);
  return size * nmemb;
}

HttpClient::HttpClient(const std::string &backendUrl)
    : backendUrl_(backendUrl), lastError_("") {}

bool HttpClient::post(const std::string &endpoint, const std::string &jsonBody,
                      std::string &response) {
  CURL *curl = curl_easy_init();
  if (!curl) {
    lastError_ = "Failed to initialize CURL";
    return false;
  }

  std::string url = backendUrl_ + endpoint;
  response.clear();

  curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
  curl_easy_setopt(curl, CURLOPT_POSTFIELDS, jsonBody.c_str());
  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

  struct curl_slist *headers = nullptr;
  headers = curl_slist_append(headers, "Content-Type: application/json");
  curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

  CURLcode res = curl_easy_perform(curl);

  bool success = (res == CURLE_OK);
  if (!success) {
    lastError_ = std::string("CURL error: ") + curl_easy_strerror(res);
  }

  curl_slist_free_all(headers);
  curl_easy_cleanup(curl);

  return success;
}

bool HttpClient::performMerge(const std::vector<std::string> &base,
                              const std::vector<std::string> &ours,
                              const std::vector<std::string> &theirs,
                              std::vector<std::string> &merged,
                              bool &hasConflicts) {
  // Build JSON request
  // NOTE: This is a simplified JSON builder for prototype purposes.
  // LIMITATION: Does not escape special characters in strings (quotes,
  // backslashes, etc.)
  // TODO: For production, use a proper JSON library like nlohmann/json or
  // rapidjson This implementation works for simple test cases but will fail
  // with complex content.
  std::ostringstream json;
  json << "{";
  json << "\"base\":[";
  for (size_t i = 0; i < base.size(); ++i) {
    json << "\"" << base[i] << "\""; // WARNING: No escaping!
    if (i < base.size() - 1)
      json << ",";
  }
  json << "],";
  json << "\"ours\":[";
  for (size_t i = 0; i < ours.size(); ++i) {
    json << "\"" << ours[i] << "\""; // WARNING: No escaping!
    if (i < ours.size() - 1)
      json << ",";
  }
  json << "],";
  json << "\"theirs\":[";
  for (size_t i = 0; i < theirs.size(); ++i) {
    json << "\"" << theirs[i] << "\""; // WARNING: No escaping!
    if (i < theirs.size() - 1)
      json << ",";
  }
  json << "]";
  json << "}";

  std::string response;
  if (!post("/api/merge", json.str(), response)) {
    return false;
  }

  // Parse JSON response (simple parsing for now)
  // NOTE: This is a fragile string-based parser for prototype purposes.
  // LIMITATION: Will break on complex JSON or unexpected formatting.
  // TODO: For production, use a proper JSON library like nlohmann/json or
  // rapidjson
  merged.clear();
  hasConflicts = (response.find("\"has_conflicts\":true") != std::string::npos);

  // Extract merged lines from response
  // This is a simplified parser - production code MUST use a JSON library
  size_t mergedPos = response.find("\"merged\":");
  if (mergedPos != std::string::npos) {
    size_t startBracket = response.find("[", mergedPos);
    size_t endBracket = response.find("]", startBracket);
    if (startBracket != std::string::npos && endBracket != std::string::npos) {
      std::string mergedArray =
          response.substr(startBracket + 1, endBracket - startBracket - 1);

      // Parse lines (simplified)
      size_t pos = 0;
      while (pos < mergedArray.size()) {
        size_t quoteStart = mergedArray.find("\"", pos);
        if (quoteStart == std::string::npos)
          break;
        size_t quoteEnd = mergedArray.find("\"", quoteStart + 1);
        if (quoteEnd == std::string::npos)
          break;

        std::string line =
            mergedArray.substr(quoteStart + 1, quoteEnd - quoteStart - 1);
        merged.push_back(line);
        pos = quoteEnd + 1;
      }
    }
  }

  return true;
}

bool HttpClient::checkBackend() {
  CURL *curl = curl_easy_init();
  if (!curl) {
    lastError_ = "Failed to initialize CURL";
    return false;
  }

  std::string url = backendUrl_ + "/";
  curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
  curl_easy_setopt(curl, CURLOPT_NOBODY, 1L);
  curl_easy_setopt(curl, CURLOPT_TIMEOUT, 5L);

  CURLcode res = curl_easy_perform(curl);
  bool success = (res == CURLE_OK);

  if (!success) {
    lastError_ =
        std::string("Cannot reach backend: ") + curl_easy_strerror(res);
  }

  curl_easy_cleanup(curl);
  return success;
}
