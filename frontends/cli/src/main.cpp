#include "http_client.h"
#include "file_utils.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cstring>
#include <cstdlib>
#include <curl/curl.h>

/**
 * @brief Print usage information
 */
void printUsage(const char* programName) {
    std::cout << "WizardMerge CLI Frontend - Intelligent Merge Conflict Resolution\n\n";
    std::cout << "Usage:\n";
    std::cout << "  " << programName << " [OPTIONS] merge --base <file> --ours <file> --theirs <file>\n";
    std::cout << "  " << programName << " [OPTIONS] pr-resolve --url <pr_url> [--token <token>]\n";
    std::cout << "  " << programName << " [OPTIONS] git-resolve [FILE]\n";
    std::cout << "  " << programName << " --help\n";
    std::cout << "  " << programName << " --version\n\n";
    std::cout << "Global Options:\n";
    std::cout << "  --backend <url>     Backend server URL (default: http://localhost:8080)\n";
    std::cout << "  -v, --verbose       Enable verbose output\n";
    std::cout << "  -q, --quiet         Suppress non-error output\n";
    std::cout << "  -h, --help          Show this help message\n";
    std::cout << "  --version           Show version information\n\n";
    std::cout << "Commands:\n";
    std::cout << "  merge               Perform three-way merge\n";
    std::cout << "    --base <file>     Base version file (required)\n";
    std::cout << "    --ours <file>     Our version file (required)\n";
    std::cout << "    --theirs <file>   Their version file (required)\n";
    std::cout << "    -o, --output <file>  Output file (default: stdout)\n";
    std::cout << "    --format <format>    Output format: text, json (default: text)\n\n";
    std::cout << "  pr-resolve          Resolve pull request conflicts\n";
    std::cout << "    --url <url>       Pull request URL (required)\n";
    std::cout << "    --token <token>   GitHub API token (optional, can use GITHUB_TOKEN env)\n";
    std::cout << "    --branch <name>   Create branch with resolved conflicts (optional)\n";
    std::cout << "    -o, --output <dir>  Output directory for resolved files (default: stdout)\n\n";
    std::cout << "  git-resolve         Resolve Git merge conflicts (not yet implemented)\n";
    std::cout << "    [FILE]            Specific file to resolve (optional)\n\n";
    std::cout << "Examples:\n";
    std::cout << "  " << programName << " merge --base base.txt --ours ours.txt --theirs theirs.txt\n";
    std::cout << "  " << programName << " merge --base base.txt --ours ours.txt --theirs theirs.txt -o result.txt\n";
    std::cout << "  " << programName << " pr-resolve --url https://github.com/owner/repo/pull/123\n";
    std::cout << "  " << programName << " pr-resolve --url https://github.com/owner/repo/pull/123 --token ghp_xxx\n";
    std::cout << "  " << programName << " --backend http://remote:8080 merge --base b.txt --ours o.txt --theirs t.txt\n\n";
}

/**
 * @brief Print version information
 */
void printVersion() {
    std::cout << "WizardMerge CLI Frontend v1.0.0\n";
    std::cout << "Part of the WizardMerge Intelligent Merge Conflict Resolution system\n";
}

/**
 * @brief Parse command-line arguments and execute merge
 */
int main(int argc, char* argv[]) {
    // Default configuration
    std::string backendUrl = "http://localhost:8080";
    bool verbose = false;
    bool quiet = false;
    std::string command;
    std::string baseFile, oursFile, theirsFile, outputFile;
    std::string format = "text";
    std::string prUrl, githubToken, branchName;

    // Check environment variable
    const char* envBackend = std::getenv("WIZARDMERGE_BACKEND");
    if (envBackend) {
        backendUrl = envBackend;
    }
    
    // Check for GitHub token in environment
    const char* envToken = std::getenv("GITHUB_TOKEN");
    if (envToken) {
        githubToken = envToken;
    }

    // Parse arguments
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];

        if (arg == "--help" || arg == "-h") {
            printUsage(argv[0]);
            return 0;
        } else if (arg == "--version") {
            printVersion();
            return 0;
        } else if (arg == "--backend") {
            if (i + 1 < argc) {
                backendUrl = argv[++i];
            } else {
                std::cerr << "Error: --backend requires an argument\n";
                return 2;
            }
        } else if (arg == "--verbose" || arg == "-v") {
            verbose = true;
        } else if (arg == "--quiet" || arg == "-q") {
            quiet = true;
        } else if (arg == "merge") {
            command = "merge";
        } else if (arg == "pr-resolve") {
            command = "pr-resolve";
        } else if (arg == "git-resolve") {
            command = "git-resolve";
        } else if (arg == "--url") {
            if (i + 1 < argc) {
                prUrl = argv[++i];
            } else {
                std::cerr << "Error: --url requires an argument\n";
                return 2;
            }
        } else if (arg == "--token") {
            if (i + 1 < argc) {
                githubToken = argv[++i];
            } else {
                std::cerr << "Error: --token requires an argument\n";
                return 2;
            }
        } else if (arg == "--branch") {
            if (i + 1 < argc) {
                branchName = argv[++i];
            } else {
                std::cerr << "Error: --branch requires an argument\n";
                return 2;
            }
        } else if (arg == "--base") {
            if (i + 1 < argc) {
                baseFile = argv[++i];
            } else {
                std::cerr << "Error: --base requires an argument\n";
                return 2;
            }
        } else if (arg == "--ours") {
            if (i + 1 < argc) {
                oursFile = argv[++i];
            } else {
                std::cerr << "Error: --ours requires an argument\n";
                return 2;
            }
        } else if (arg == "--theirs") {
            if (i + 1 < argc) {
                theirsFile = argv[++i];
            } else {
                std::cerr << "Error: --theirs requires an argument\n";
                return 2;
            }
        } else if (arg == "--output" || arg == "-o") {
            if (i + 1 < argc) {
                outputFile = argv[++i];
            } else {
                std::cerr << "Error: --output requires an argument\n";
                return 2;
            }
        } else if (arg == "--format") {
            if (i + 1 < argc) {
                format = argv[++i];
            } else {
                std::cerr << "Error: --format requires an argument\n";
                return 2;
            }
        }
    }

    // Check if command was provided
    if (command.empty()) {
        std::cerr << "Error: No command specified\n\n";
        printUsage(argv[0]);
        return 2;
    }

    // Execute command
    if (command == "merge") {
        // Validate required arguments
        if (baseFile.empty() || oursFile.empty() || theirsFile.empty()) {
            std::cerr << "Error: merge command requires --base, --ours, and --theirs arguments\n";
            return 2;
        }

        // Check files exist
        if (!FileUtils::fileExists(baseFile)) {
            std::cerr << "Error: Base file not found: " << baseFile << "\n";
            return 4;
        }
        if (!FileUtils::fileExists(oursFile)) {
            std::cerr << "Error: Ours file not found: " << oursFile << "\n";
            return 4;
        }
        if (!FileUtils::fileExists(theirsFile)) {
            std::cerr << "Error: Theirs file not found: " << theirsFile << "\n";
            return 4;
        }

        if (verbose) {
            std::cout << "Backend URL: " << backendUrl << "\n";
            std::cout << "Base file: " << baseFile << "\n";
            std::cout << "Ours file: " << oursFile << "\n";
            std::cout << "Theirs file: " << theirsFile << "\n";
        }

        // Read input files
        std::vector<std::string> baseLines, oursLines, theirsLines;
        if (!FileUtils::readLines(baseFile, baseLines)) {
            std::cerr << "Error: Failed to read base file\n";
            return 4;
        }
        if (!FileUtils::readLines(oursFile, oursLines)) {
            std::cerr << "Error: Failed to read ours file\n";
            return 4;
        }
        if (!FileUtils::readLines(theirsFile, theirsLines)) {
            std::cerr << "Error: Failed to read theirs file\n";
            return 4;
        }

        if (verbose) {
            std::cout << "Read " << baseLines.size() << " lines from base\n";
            std::cout << "Read " << oursLines.size() << " lines from ours\n";
            std::cout << "Read " << theirsLines.size() << " lines from theirs\n";
        }

        // Connect to backend and perform merge
        HttpClient client(backendUrl);
        
        if (!quiet) {
            std::cout << "Connecting to backend: " << backendUrl << "\n";
        }

        if (!client.checkBackend()) {
            std::cerr << "Error: Cannot connect to backend: " << client.getLastError() << "\n";
            std::cerr << "Make sure the backend server is running on " << backendUrl << "\n";
            return 3;
        }

        if (!quiet) {
            std::cout << "Performing three-way merge...\n";
        }

        std::vector<std::string> mergedLines;
        bool hasConflicts = false;

        if (!client.performMerge(baseLines, oursLines, theirsLines, mergedLines, hasConflicts)) {
            std::cerr << "Error: Merge failed: " << client.getLastError() << "\n";
            return 1;
        }

        // Output results
        if (!quiet) {
            std::cout << "Merge completed. Has conflicts: " << (hasConflicts ? "Yes" : "No") << "\n";
            std::cout << "Result has " << mergedLines.size() << " lines\n";
        }

        // Write output
        if (outputFile.empty()) {
            // Write to stdout
            for (const auto& line : mergedLines) {
                std::cout << line << "\n";
            }
        } else {
            if (!FileUtils::writeLines(outputFile, mergedLines)) {
                std::cerr << "Error: Failed to write output file\n";
                return 4;
            }
            if (!quiet) {
                std::cout << "Output written to: " << outputFile << "\n";
            }
        }

        return hasConflicts ? 5 : 0;

    } else if (command == "pr-resolve") {
        // Validate required arguments
        if (prUrl.empty()) {
            std::cerr << "Error: pr-resolve command requires --url argument\n";
            return 2;
        }

        if (verbose) {
            std::cout << "Backend URL: " << backendUrl << "\n";
            std::cout << "Pull Request URL: " << prUrl << "\n";
            if (!githubToken.empty()) {
                std::cout << "Using GitHub token: " << githubToken.substr(0, 4) << "...\n";
            }
        }

        // Connect to backend
        HttpClient client(backendUrl);
        
        if (!quiet) {
            std::cout << "Connecting to backend: " << backendUrl << "\n";
        }

        if (!client.checkBackend()) {
            std::cerr << "Error: Cannot connect to backend: " << client.getLastError() << "\n";
            std::cerr << "Make sure the backend server is running on " << backendUrl << "\n";
            return 3;
        }

        if (!quiet) {
            std::cout << "Resolving pull request conflicts...\n";
        }

        // Build JSON request for PR resolution
        std::ostringstream json;
        json << "{";
        json << "\"pr_url\":\"" << prUrl << "\"";
        if (!githubToken.empty()) {
            json << ",\"github_token\":\"" << githubToken << "\"";
        }
        if (!branchName.empty()) {
            json << ",\"create_branch\":true";
            json << ",\"branch_name\":\"" << branchName << "\"";
        }
        json << "}";

        // Perform HTTP POST to /api/pr/resolve
        std::string response;
        CURL* curl = curl_easy_init();
        if (!curl) {
            std::cerr << "Error: Failed to initialize CURL\n";
            return 3;
        }

        std::string url = backendUrl + "/api/pr/resolve";
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, json.str().c_str());
        
        auto WriteCallback = [](void* contents, size_t size, size_t nmemb, void* userp) -> size_t {
            ((std::string*)userp)->append((char*)contents, size * nmemb);
            return size * nmemb;
        };
        
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, +WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

        struct curl_slist* headers = nullptr;
        headers = curl_slist_append(headers, "Content-Type: application/json");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

        CURLcode res = curl_easy_perform(curl);
        
        if (res != CURLE_OK) {
            std::cerr << "Error: Request failed: " << curl_easy_strerror(res) << "\n";
            curl_slist_free_all(headers);
            curl_easy_cleanup(curl);
            return 3;
        }

        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);

        // Output response
        if (outputFile.empty()) {
            std::cout << "\n=== Pull Request Resolution Result ===\n";
            std::cout << response << "\n";
        } else {
            std::ofstream out(outputFile);
            if (!out) {
                std::cerr << "Error: Failed to write output file\n";
                return 4;
            }
            out << response;
            out.close();
            if (!quiet) {
                std::cout << "Result written to: " << outputFile << "\n";
            }
        }

        // Check if resolution was successful (simple check)
        if (response.find("\"success\":true") != std::string::npos) {
            if (!quiet) {
                std::cout << "\nPull request conflicts resolved successfully!\n";
            }
            return 0;
        } else {
            if (!quiet) {
                std::cerr << "\nFailed to resolve some conflicts. See output for details.\n";
            }
            return 1;
        }

    } else if (command == "git-resolve") {
        std::cerr << "Error: git-resolve command not yet implemented\n";
        return 1;
    } else {
        std::cerr << "Error: Unknown command: " << command << "\n";
        return 2;
    }

    return 0;
}
