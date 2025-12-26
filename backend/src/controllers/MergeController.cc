/**
 * @file MergeController.cc
 * @brief Implementation of HTTP controller for merge operations
 */

#include "MergeController.h"
#include "wizardmerge/merge/three_way_merge.h"
#include <json/json.h>

using namespace wizardmerge::controllers;
using namespace wizardmerge::merge;

void MergeController::merge(
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
    if (!json.isMember("base") || !json.isMember("ours") || !json.isMember("theirs")) {
        Json::Value error;
        error["error"] = "Missing required fields: base, ours, theirs";
        auto resp = HttpResponse::newHttpJsonResponse(error);
        resp->setStatusCode(k400BadRequest);
        callback(resp);
        return;
    }

    // Convert JSON arrays to std::vector<std::string>
    std::vector<std::string> base;
    std::vector<std::string> ours;
    std::vector<std::string> theirs;

    try {
        for (const auto &line : json["base"]) {
            base.push_back(line.asString());
        }
        for (const auto &line : json["ours"]) {
            ours.push_back(line.asString());
        }
        for (const auto &line : json["theirs"]) {
            theirs.push_back(line.asString());
        }
    } catch (const std::exception &e) {
        Json::Value error;
        error["error"] = "Invalid array format in request";
        auto resp = HttpResponse::newHttpJsonResponse(error);
        resp->setStatusCode(k400BadRequest);
        callback(resp);
        return;
    }

    // Perform merge
    auto result = three_way_merge(base, ours, theirs);
    
    // Auto-resolve simple conflicts
    result = auto_resolve(result);

    // Build response JSON
    Json::Value response;
    Json::Value mergedArray(Json::arrayValue);
    for (const auto &line : result.merged_lines) {
        mergedArray.append(line.content);
    }
    response["merged"] = mergedArray;

    // Add conflicts information
    Json::Value conflictsArray(Json::arrayValue);
    for (const auto &conflict : result.conflicts) {
        Json::Value conflictObj;
        // Note: start_line and end_line are size_t (always non-negative)
        conflictObj["start_line"] = static_cast<Json::UInt64>(conflict.start_line);
        conflictObj["end_line"] = static_cast<Json::UInt64>(conflict.end_line);
        
        Json::Value baseLines(Json::arrayValue);
        for (const auto &line : conflict.base_lines) {
            baseLines.append(line.content);
        }
        conflictObj["base_lines"] = baseLines;
        
        Json::Value ourLines(Json::arrayValue);
        for (const auto &line : conflict.our_lines) {
            ourLines.append(line.content);
        }
        conflictObj["our_lines"] = ourLines;
        
        Json::Value theirLines(Json::arrayValue);
        for (const auto &line : conflict.their_lines) {
            theirLines.append(line.content);
        }
        conflictObj["their_lines"] = theirLines;
        
        conflictsArray.append(conflictObj);
    }
    response["conflicts"] = conflictsArray;
    response["has_conflicts"] = result.has_conflicts();

    // Return successful response
    auto resp = HttpResponse::newHttpJsonResponse(response);
    resp->setStatusCode(k200OK);
    callback(resp);
}
