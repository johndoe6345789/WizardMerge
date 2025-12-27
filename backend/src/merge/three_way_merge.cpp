/**
 * @file three_way_merge.cpp
 * @brief Implementation of three-way merge algorithm
 */

#include "wizardmerge/merge/three_way_merge.h"
#include "wizardmerge/analysis/context_analyzer.h"
#include "wizardmerge/analysis/risk_analyzer.h"
#include <algorithm>

namespace wizardmerge {
namespace merge {

namespace {

/**
 * @brief Check if two lines are effectively equal (ignoring whitespace).
 */
bool lines_equal_ignore_whitespace(const std::string& a, const std::string& b) {
    auto trim = [](const std::string& s) {
        size_t start = s.find_first_not_of(" \t\n\r");
        size_t end = s.find_last_not_of(" \t\n\r");
        if (start == std::string::npos) return std::string();
        return s.substr(start, end - start + 1);
    };
    return trim(a) == trim(b);
}

}  // namespace

MergeResult three_way_merge(
    const std::vector<std::string>& base,
    const std::vector<std::string>& ours,
    const std::vector<std::string>& theirs
) {
    MergeResult result;
    
    // Simple line-by-line comparison for initial implementation
    // This is a placeholder - full algorithm will use dependency analysis
    
    size_t max_len = std::max({base.size(), ours.size(), theirs.size()});
    
    for (size_t i = 0; i < max_len; ++i) {
        std::string base_line = (i < base.size()) ? base[i] : "";
        std::string our_line = (i < ours.size()) ? ours[i] : "";
        std::string their_line = (i < theirs.size()) ? theirs[i] : "";
        
        // Case 1: All three are the same - use as-is
        if (base_line == our_line && base_line == their_line) {
            result.merged_lines.push_back({base_line, Line::BASE});
        }
        // Case 2: Base == Ours, but Theirs changed - use theirs
        else if (base_line == our_line && base_line != their_line) {
            result.merged_lines.push_back({their_line, Line::THEIRS});
        }
        // Case 3: Base == Theirs, but Ours changed - use ours
        else if (base_line == their_line && base_line != our_line) {
            result.merged_lines.push_back({our_line, Line::OURS});
        }
        // Case 4: Ours == Theirs, but different from Base - use the common change
        else if (our_line == their_line && our_line != base_line) {
            result.merged_lines.push_back({our_line, Line::MERGED});
        }
        // Case 5: All different - conflict
        else {
            Conflict conflict;
            conflict.start_line = result.merged_lines.size();
            conflict.base_lines.push_back({base_line, Line::BASE});
            conflict.our_lines.push_back({our_line, Line::OURS});
            conflict.their_lines.push_back({their_line, Line::THEIRS});
            conflict.end_line = result.merged_lines.size();
            
            // Perform context analysis
            // Use the merged lines we have so far as context
            std::vector<std::string> context_lines;
            for (const auto& line : result.merged_lines) {
                context_lines.push_back(line.content);
            }
            conflict.context = analysis::analyze_context(context_lines, i, i);
            
            // Perform risk analysis for different resolution strategies
            std::vector<std::string> base_vec = {base_line};
            std::vector<std::string> ours_vec = {our_line};
            std::vector<std::string> theirs_vec = {their_line};
            
            conflict.risk_ours = analysis::analyze_risk_ours(base_vec, ours_vec, theirs_vec);
            conflict.risk_theirs = analysis::analyze_risk_theirs(base_vec, ours_vec, theirs_vec);
            conflict.risk_both = analysis::analyze_risk_both(base_vec, ours_vec, theirs_vec);
            
            result.conflicts.push_back(conflict);
            
            // Add conflict markers
            result.merged_lines.push_back({"<<<<<<< OURS", Line::MERGED});
            result.merged_lines.push_back({our_line, Line::OURS});
            result.merged_lines.push_back({"=======", Line::MERGED});
            result.merged_lines.push_back({their_line, Line::THEIRS});
            result.merged_lines.push_back({">>>>>>> THEIRS", Line::MERGED});
        }
    }
    
    return result;
}

MergeResult auto_resolve(const MergeResult& result) {
    MergeResult resolved = result;
    
    // Auto-resolve whitespace-only differences
    std::vector<Conflict> remaining_conflicts;
    
    for (const auto& conflict : result.conflicts) {
        bool can_resolve = false;
        
        // Check if differences are whitespace-only
        if (conflict.our_lines.size() == conflict.their_lines.size()) {
            can_resolve = true;
            for (size_t i = 0; i < conflict.our_lines.size(); ++i) {
                if (!lines_equal_ignore_whitespace(
                    conflict.our_lines[i].content,
                    conflict.their_lines[i].content)) {
                    can_resolve = false;
                    break;
                }
            }
        }
        
        if (!can_resolve) {
            remaining_conflicts.push_back(conflict);
        }
    }
    
    resolved.conflicts = remaining_conflicts;
    return resolved;
}

}  // namespace merge
}  // namespace wizardmerge
