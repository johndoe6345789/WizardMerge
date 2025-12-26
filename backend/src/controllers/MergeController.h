/**
 * @file MergeController.h
 * @brief HTTP controller for merge operations
 */

#ifndef WIZARDMERGE_CONTROLLERS_MERGE_CONTROLLER_H
#define WIZARDMERGE_CONTROLLERS_MERGE_CONTROLLER_H

#include <drogon/HttpController.h>

using namespace drogon;

namespace wizardmerge {
namespace controllers {

/**
 * @brief HTTP controller for three-way merge API
 */
class MergeController : public HttpController<MergeController> {
  public:
    METHOD_LIST_BEGIN
    // POST /api/merge - Perform three-way merge
    ADD_METHOD_TO(MergeController::merge, "/api/merge", Post);
    METHOD_LIST_END

    /**
     * @brief Perform three-way merge operation
     * 
     * Request body should be JSON:
     * {
     *   "base": ["line1", "line2", ...],
     *   "ours": ["line1", "line2", ...],
     *   "theirs": ["line1", "line2", ...]
     * }
     * 
     * Response:
     * {
     *   "merged": ["line1", "line2", ...],
     *   "conflicts": [...]
     * }
     */
    void merge(const HttpRequestPtr &req,
              std::function<void(const HttpResponsePtr &)> &&callback);
};

}  // namespace controllers
}  // namespace wizardmerge

#endif  // WIZARDMERGE_CONTROLLERS_MERGE_CONTROLLER_H
