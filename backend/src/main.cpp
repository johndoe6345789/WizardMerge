/**
 * @file main.cpp
 * @brief Command-line interface for WizardMerge
 */

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include "wizardmerge/merge/three_way_merge.h"

using namespace wizardmerge::merge;

/**
 * @brief Read lines from a file
 */
std::vector<std::string> read_file(const std::string& filename) {
    std::vector<std::string> lines;
    std::ifstream file(filename);
    std::string line;
    
    while (std::getline(file, line)) {
        lines.push_back(line);
    }
    
    return lines;
}

/**
 * @brief Write lines to a file
 */
void write_file(const std::string& filename, const std::vector<Line>& lines) {
    std::ofstream file(filename);
    for (const auto& line : lines) {
        file << line.content << '\n';
    }
}

int main(int argc, char* argv[]) {
    if (argc != 5) {
        std::cerr << "Usage: " << argv[0] << " <base> <ours> <theirs> <output>\n";
        std::cerr << "Performs three-way merge on three file versions.\n";
        return 1;
    }
    
    std::string base_file = argv[1];
    std::string ours_file = argv[2];
    std::string theirs_file = argv[3];
    std::string output_file = argv[4];
    
    std::cout << "WizardMerge - Intelligent Merge Conflict Resolution\n";
    std::cout << "===================================================\n";
    std::cout << "Base:   " << base_file << '\n';
    std::cout << "Ours:   " << ours_file << '\n';
    std::cout << "Theirs: " << theirs_file << '\n';
    std::cout << "Output: " << output_file << '\n';
    std::cout << '\n';
    
    // Read input files
    auto base = read_file(base_file);
    auto ours = read_file(ours_file);
    auto theirs = read_file(theirs_file);
    
    // Perform merge
    auto result = three_way_merge(base, ours, theirs);
    
    // Auto-resolve simple conflicts
    result = auto_resolve(result);
    
    // Write output
    write_file(output_file, result.merged_lines);
    
    // Report results
    if (result.has_conflicts()) {
        std::cout << "Merge completed with " << result.conflicts.size() 
                  << " conflict(s).\n";
        std::cout << "Please review and resolve conflicts in: " << output_file << '\n';
        return 1;
    } else {
        std::cout << "Merge completed successfully with no conflicts.\n";
        return 0;
    }
}
