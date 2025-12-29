/**
 * @file PRController.h
 * @brief HTTP controller for pull request merge operations
 */

#ifndef WIZARDMERGE_CONTROLLERS_PR_CONTROLLER_H
#define WIZARDMERGE_CONTROLLERS_PR_CONTROLLER_H

#include <drogon/HttpController.h>

using namespace drogon;

namespace wizardmerge {
namespace controllers {

/**
 * @brief HTTP controller for pull request merge API
 */
class PRController : public HttpController<PRController> {
public:
  METHOD_LIST_BEGIN
  // POST /api/pr/resolve - Resolve conflicts in a pull request
  ADD_METHOD_TO(PRController::resolvePR, "/api/pr/resolve", Post);
  METHOD_LIST_END

  /**
   * @brief Resolve merge conflicts in a pull request
   *
   * Request body should be JSON:
   * {
   *   "pr_url": "https://github.com/owner/repo/pull/123",
   *   "github_token": "optional_github_token",
   *   "create_branch": true,
   *   "branch_name": "wizardmerge-resolved-pr-123"
   * }
   *
   * Response:
   * {
   *   "success": true,
   *   "pr_info": {
   *     "number": 123,
   *     "title": "...",
   *     "base_ref": "main",
   *     "head_ref": "feature-branch"
   *   },
   *   "resolved_files": [
   *     {
   *       "filename": "...",
   *       "had_conflicts": true,
   *       "auto_resolved": true,
   *       "merged_content": ["line1", "line2", ...]
   *     }
   *   ],
   *   "branch_created": true,
   *   "branch_name": "wizardmerge-resolved-pr-123"
   * }
   */
  void resolvePR(const HttpRequestPtr &req,
                 std::function<void(const HttpResponsePtr &)> &&callback);
};

} // namespace controllers
} // namespace wizardmerge

#endif // WIZARDMERGE_CONTROLLERS_PR_CONTROLLER_H
