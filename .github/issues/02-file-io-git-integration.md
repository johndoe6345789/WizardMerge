---
title: "Phase 1.2: File Input/Output and Git Integration"
labels: ["enhancement", "phase-1", "git-integration", "high-priority"]
assignees: []
milestone: "Phase 1 - Foundation"
---

## Overview

Implement comprehensive file I/O and Git integration features to enable WizardMerge to work directly with Git repositories and conflicted files.

## Related Roadmap Section

Phase 1.2 and 1.5 from ROADMAP.md

## Features to Implement

### File Input/Output Module

- [ ] Parse Git conflict markers from files (`<<<<<<<`, `=======`, `>>>>>>>`)
- [ ] Support for Mercurial conflict markers
- [ ] Load base, ours, and theirs versions from Git
- [ ] Save resolved merge results to files
- [ ] Support for directory-level conflict resolution
- [ ] Backup mechanism for safety (create `.backup` files before resolution)
- [ ] Handle file encodings (UTF-8, UTF-16, etc.)
- [ ] Validate file write permissions before attempting resolution

**Deliverable**: `backend/src/io/` module with file handlers

### Git Repository Integration

- [ ] Detect when running in Git repository (check for `.git` directory)
- [ ] Read `.git/MERGE_HEAD` to identify active merge conflicts
- [ ] List all conflicted files in repository
- [ ] Get base, ours, theirs versions using Git commands:
  - `git show :1:file` (base)
  - `git show :2:file` (ours)
  - `git show :3:file` (theirs)
- [ ] Mark files as resolved in Git index (`git add`)
- [ ] Support launching from command line: `wizardmerge [file]`
- [ ] Support launching with no arguments to resolve all conflicts in repo

**Deliverable**: Enhanced `backend/src/git/` module and CLI enhancements

## Technical Design

### File Parser Architecture

```cpp
class ConflictFileParser {
  // Parse conflict markers and extract sections
  ConflictSections parse(const std::string& file_content);
  
  // Detect conflict marker style (Git, Mercurial, etc.)
  ConflictMarkerStyle detect_marker_style(const std::string& content);
  
  // Extract base/ours/theirs sections
  std::vector<ConflictBlock> extract_conflict_blocks(const std::string& content);
};
```

### Git Integration Architecture

```cpp
class GitRepository {
  // Check if we're in a Git repository
  bool is_git_repo(const std::string& path);
  
  // List conflicted files
  std::vector<std::string> list_conflicted_files();
  
  // Get file version from Git index
  std::string get_file_version(const std::string& file, GitStage stage);
  
  // Mark file as resolved
  bool mark_resolved(const std::string& file);
};
```

## Implementation Steps

1. **Create file I/O module structure**
   - Set up `backend/src/io/` directory
   - Add CMake configuration
   - Create header files

2. **Implement conflict marker parser**
   - Parse standard Git markers
   - Support custom conflict marker labels
   - Handle nested conflicts (edge case)

3. **Implement Git integration**
   - Repository detection
   - Conflict file listing
   - Index stage reading (`:1:`, `:2:`, `:3:`)

4. **Add CLI enhancements**
   - File path argument handling
   - Directory scanning for conflicts
   - Progress reporting for multiple files

5. **Add safety features**
   - Automatic backups before resolution
   - Dry-run mode for testing
   - Verification of resolved content

6. **Testing**
   - Unit tests for parser
   - Integration tests with real Git repos
   - Test various conflict scenarios

## Acceptance Criteria

- [ ] Can parse Git conflict markers from files
- [ ] Can load base/ours/theirs from Git index
- [ ] Can save resolved files
- [ ] Creates backups before modifying files
- [ ] CLI accepts file paths and resolves conflicts
- [ ] Works with conflicted directories
- [ ] All tests pass
- [ ] Documentation updated

## Dependencies

- Git CLI integration (already implemented) ✅
- Three-way merge algorithm (already implemented) ✅

## Test Cases

1. Parse simple conflict with standard markers
2. Parse multiple conflicts in same file
3. Parse conflict with custom labels
4. Read file versions from Git index
5. Save resolved file without corruption
6. Create and restore from backups
7. Handle binary file conflicts gracefully
8. Handle missing base version (add/add conflict)

## Documentation Updates

- [ ] Update README.md with file I/O usage
- [ ] Update CLI documentation
- [ ] Add examples to backend/README.md
- [ ] Document conflict marker formats supported

## Priority

**HIGH** - This is essential for Phase 1 completion and enables standalone operation without external tools.

## Estimated Effort

2-3 weeks

## Related Issues

- #TBD (Phase 1 completion tracking)
- #TBD (CLI enhancements)
