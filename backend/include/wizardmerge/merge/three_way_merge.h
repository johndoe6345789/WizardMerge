/**
 * @file three_way_merge.h
 * @brief Three-way merge algorithm for WizardMerge
 *
 * Implements the core three-way merge algorithm based on the paper from
 * The University of Hong Kong. This algorithm uses dependency analysis
 * at both text and LLVM-IR levels to provide intelligent merge suggestions.
 */

#ifndef WIZARDMERGE_MERGE_THREE_WAY_MERGE_H
#define WIZARDMERGE_MERGE_THREE_WAY_MERGE_H

#include <string>
#include <vector>

namespace wizardmerge {
namespace merge {

/**
 * @brief Represents a single line in a file with its origin.
 */
struct Line {
    std::string content;
    enum Origin { BASE, OURS, THEIRS, MERGED } origin;
};

/**
 * @brief Represents a conflict region in the merge result.
 */
struct Conflict {
    size_t start_line;
    size_t end_line;
    std::vector<Line> base_lines;
    std::vector<Line> our_lines;
    std::vector<Line> their_lines;
};

/**
 * @brief Result of a three-way merge operation.
 */
struct MergeResult {
    std::vector<Line> merged_lines;
    std::vector<Conflict> conflicts;
    bool has_conflicts() const { return !conflicts.empty(); }
};

/**
 * @brief Performs a three-way merge on three versions of content.
 *
 * This function implements the three-way merge algorithm that compares
 * the base version with two variants (ours and theirs) to produce a
 * merged result with conflict markers where automatic resolution is
 * not possible.
 *
 * @param base The common ancestor version
 * @param ours Our version (current branch)
 * @param theirs Their version (branch being merged)
 * @return MergeResult containing the merged content and any conflicts
 */
MergeResult three_way_merge(
    const std::vector<std::string>& base,
    const std::vector<std::string>& ours,
    const std::vector<std::string>& theirs
);

/**
 * @brief Auto-resolves simple non-conflicting patterns.
 *
 * Handles common auto-resolvable patterns:
 * - Non-overlapping changes
 * - Identical changes from both sides
 * - Whitespace-only differences
 *
 * @param result The merge result to auto-resolve
 * @return Updated merge result with resolved conflicts
 */
MergeResult auto_resolve(const MergeResult& result);

}  // namespace merge
}  // namespace wizardmerge

#endif  // WIZARDMERGE_MERGE_THREE_WAY_MERGE_H
