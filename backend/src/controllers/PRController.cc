/**
 * @file PRController.cc
 * @brief Implementation of HTTP controller for pull request operations
 */

#include "PRController.h"
#include "wizardmerge/git/git_platform_client.h"
#include "wizardmerge/merge/three_way_merge.h"
#include <json/json.h>
#include <iostream>

using namespace wizardmerge::controllers;
using namespace wizardmerge::git;
using namespace wizardmerge::merge;

void PRController::resolvePR(
    const HttpRequestPtr &req,
    std::function<void(const HttpResponsePtr &)> &&callback) {
    
    // Parse request JSON
    auto jsonPtr = req->getJsonObject();
    if (!jsonPtr) {
        Json::Value error;
        error["error"] = "Invalid JSON in request body";
        auto resp = HttpResponse::newHttpJsonResponse(error);
        resp->setStatusCode(k400BadRequest);
        callback(resp);
        return;
    }

    const auto &json = *jsonPtr;
    
    // Validate required fields
    if (!json.isMember("pr_url")) {
        Json::Value error;
        error["error"] = "Missing required field: pr_url";
        auto resp = HttpResponse::newHttpJsonResponse(error);
        resp->setStatusCode(k400BadRequest);
        callback(resp);
        return;
    }

    std::string pr_url = json["pr_url"].asString();
    std::string api_token = json.get("api_token", json.get("github_token", "").asString()).asString();
    bool create_branch = json.get("create_branch", false).asBool();
    std::string branch_name = json.get("branch_name", "").asString();

    // Parse PR/MR URL
    GitPlatform platform;
    std::string owner, repo;
    int pr_number;
    
    if (!parse_pr_url(pr_url, platform, owner, repo, pr_number)) {
        Json::Value error;
        error["error"] = "Invalid pull/merge request URL format";
        error["pr_url"] = pr_url;
        error["note"] = "Supported platforms: GitHub (pull requests) and GitLab (merge requests)";
        auto resp = HttpResponse::newHttpJsonResponse(error);
        resp->setStatusCode(k400BadRequest);
        callback(resp);
        return;
    }

    // Fetch pull/merge request information
    auto pr_opt = fetch_pull_request(platform, owner, repo, pr_number, api_token);
    
    if (!pr_opt) {
        Json::Value error;
        error["error"] = "Failed to fetch pull/merge request information";
        error["platform"] = (platform == GitPlatform::GitHub) ? "GitHub" : "GitLab";
        error["owner"] = owner;
        error["repo"] = repo;
        error["pr_number"] = pr_number;
        auto resp = HttpResponse::newHttpJsonResponse(error);
        resp->setStatusCode(k502BadGateway);
        callback(resp);
        return;
    }

    PullRequest pr = pr_opt.value();

    // Process each file in the PR
    Json::Value resolved_files_array(Json::arrayValue);
    int total_files = 0;
    int resolved_files = 0;
    int failed_files = 0;

    for (const auto& file : pr.files) {
        total_files++;
        
        Json::Value file_result;
        file_result["filename"] = file.filename;
        file_result["status"] = file.status;
        
        // Skip deleted files
        if (file.status == "removed") {
            file_result["skipped"] = true;
            file_result["reason"] = "File was deleted";
            resolved_files_array.append(file_result);
            continue;
        }

        // For modified files, fetch base and head versions
        if (file.status == "modified" || file.status == "added") {
            // Fetch base version (empty for added files)
            std::vector<std::string> base_content;
            if (file.status == "modified") {
                auto base_opt = fetch_file_content(platform, owner, repo, pr.base_sha, file.filename, api_token);
                if (!base_opt) {
                    file_result["error"] = "Failed to fetch base version";
                    file_result["had_conflicts"] = false;
                    failed_files++;
                    resolved_files_array.append(file_result);
                    continue;
                }
                base_content = base_opt.value();
            }

            // Fetch head version
            auto head_opt = fetch_file_content(platform, owner, repo, pr.head_sha, file.filename, api_token);
            if (!head_opt) {
                file_result["error"] = "Failed to fetch head version";
                file_result["had_conflicts"] = false;
                failed_files++;
                resolved_files_array.append(file_result);
                continue;
            }
            std::vector<std::string> head_content = head_opt.value();

            // For added files or when there might be a conflict with existing file
            // Note: This is a simplified merge for PR review purposes.
            // In a real merge scenario with conflicts, you'd need the merge-base commit.
            // Here we're showing what changes if we accept the head version:
            //   - base: common ancestor (PR base)
            //   - ours: current state (PR base)  
            //   - theirs: proposed changes (PR head)
            // This effectively shows all changes from the PR head.
            
            // Perform three-way merge: base, ours (base), theirs (head)
            auto merge_result = three_way_merge(base_content, base_content, head_content);
            merge_result = auto_resolve(merge_result);

            file_result["had_conflicts"] = merge_result.has_conflicts();
            file_result["auto_resolved"] = !merge_result.has_conflicts();

            // Extract merged content
            Json::Value merged_content(Json::arrayValue);
            for (const auto& line : merge_result.merged_lines) {
                merged_content.append(line.content);
            }
            file_result["merged_content"] = merged_content;

            if (!merge_result.has_conflicts()) {
                resolved_files++;
            }
        }

        resolved_files_array.append(file_result);
    }

    // Build response
    Json::Value response;
    response["success"] = true;
    
    Json::Value pr_info;
    pr_info["platform"] = (pr.platform == GitPlatform::GitHub) ? "GitHub" : "GitLab";
    pr_info["number"] = pr.number;
    pr_info["title"] = pr.title;
    pr_info["state"] = pr.state;
    pr_info["base_ref"] = pr.base_ref;
    pr_info["head_ref"] = pr.head_ref;
    pr_info["base_sha"] = pr.base_sha;
    pr_info["head_sha"] = pr.head_sha;
    pr_info["mergeable"] = pr.mergeable;
    pr_info["mergeable_state"] = pr.mergeable_state;
    response["pr_info"] = pr_info;

    response["resolved_files"] = resolved_files_array;
    response["total_files"] = total_files;
    response["resolved_count"] = resolved_files;
    response["failed_count"] = failed_files;
    
    // Branch creation would require Git CLI access
    // For now, just report what would be done
    response["branch_created"] = false;
    if (create_branch) {
        if (branch_name.empty()) {
            branch_name = "wizardmerge-resolved-pr-" + std::to_string(pr_number);
        }
        response["branch_name"] = branch_name;
        response["note"] = "Branch creation requires Git CLI integration (not yet implemented)";
    }

    auto resp = HttpResponse::newHttpJsonResponse(response);
    resp->setStatusCode(k200OK);
    callback(resp);
}
