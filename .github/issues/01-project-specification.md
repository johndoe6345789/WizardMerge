---
title: "Project Specification: WizardMerge Core Architecture and Features"
labels: ["documentation", "project-spec", "high-priority"]
assignees: []
---

## Overview

This issue tracks the comprehensive project specification for WizardMerge - an intelligent merge conflict resolution tool based on research from The University of Hong Kong.

## Core Mission

WizardMerge aims to reduce merge conflict resolution time by 28.85% through intelligent algorithms that provide merge suggestions for over 70% of code blocks affected by conflicts, using dependency analysis at text and LLVM-IR levels.

## Architecture Components

### 1. Backend (C++)
- **Build System**: CMake + Ninja + Conan
- **Web Framework**: Drogon HTTP server
- **Core Features**:
  - Three-way merge algorithm ✅
  - Conflict detection and auto-resolution ✅
  - HTTP API endpoints ✅
  - GitHub Pull Request integration ✅
  - GitLab Merge Request integration ✅
  - Git CLI integration for branch creation ✅

### 2. Frontend Options

#### Qt6 Native Desktop (C++)
- **Framework**: Qt6 with QML
- **Platforms**: Linux, Windows, macOS
- **Features**: Native desktop UI, offline capability, high performance

#### Next.js Web UI (TypeScript)
- **Runtime**: bun
- **Framework**: Next.js 14
- **Features**: Web-based UI, real-time collaboration, cross-platform access

#### CLI (C++)
- **Features**: Command-line interface, automation support, CI/CD integration
- **Use Cases**: Batch processing, scripting, terminal workflows

### 3. Formal Verification
- **Specification**: TLA+ formal specification (spec/WizardMergeSpec.tla)
- **CI Integration**: Automated verification on every push
- **Coverage**: Syntax, module structure, invariants, temporal properties

## Research Foundation

Based on research achieving:
- 28.85% reduction in conflict resolution time
- Merge suggestions for >70% of conflicted code blocks
- Dependency analysis at text and LLVM-IR levels
- Tested on 227 conflicts across five large-scale projects

See: `docs/PAPER.md`

## Core Principles

1. **Visual Clarity**: Show conflicts in a way that makes the problem immediately obvious
2. **Smart Assistance**: Provide intelligent suggestions while keeping humans in control
3. **Context Awareness**: Understand code structure and semantics, not just text diffs
4. **Workflow Integration**: Seamlessly fit into developers' existing Git workflows
5. **Safety First**: Make it hard to accidentally lose changes or break code

## Current Implementation Status

### Phase 1 (Foundation) - Partially Complete
- ✅ Three-way merge algorithm (base, ours, theirs)
- ✅ Conflict detection and marking
- ✅ Common auto-resolvable patterns (non-overlapping, identical, whitespace)
- ✅ Git CLI wrapper module for branch operations
- ✅ GitHub and GitLab PR/MR resolution via API
- ⏳ Support for different conflict markers
- ⏳ Line-level granularity with word-level highlighting
- ⏳ Git repository detection and conflict listing
- ⏳ File input/output with backup mechanism

### Future Phases (See Roadmap)
- Phase 2: Intelligence & Usability (3-6 months)
- Phase 3: Advanced Features (6-12 months)

## API Endpoints

### POST /api/merge
Three-way merge for direct file content

### POST /api/pr/resolve
Pull Request/Merge Request conflict resolution with branch creation support

### Platform Support
- ✅ GitHub (via GitHub API v3)
- ✅ GitLab (via GitLab API)
- 🔜 Bitbucket (planned - Phase 2)
- 🔜 Azure DevOps (planned - Phase 2)
- 🔜 Gitea/Forgejo (planned - Phase 2)

## Related Documentation

- Main README: `README.md`
- Roadmap: `ROADMAP.md`
- Build Guide: `BUILD.md`
- Research Paper: `docs/PAPER.md`
- Git CLI Implementation: `GIT_CLI_IMPLEMENTATION.md`
- Backend README: `backend/README.md`

## Success Metrics

### Phase 1 (Current)
- ✅ Successfully resolve basic three-way merges
- ✅ Handle 90% of common conflict patterns
- ✅ Command-line integration working
- ⏳ 5 active users providing feedback

## Tasks

- [x] Define core architecture
- [x] Implement three-way merge algorithm
- [x] Add GitHub/GitLab PR integration
- [x] Add Git CLI wrapper
- [ ] Document API specification
- [ ] Create comprehensive user guide
- [ ] Define plugin API specification
- [ ] Document semantic merge algorithm design
- [ ] Document SDG analysis architecture

## Related Issues

- Phase 1 Features: #TBD
- Phase 2 Features: #TBD
- Phase 3 Features: #TBD

---

**Note**: This is a living specification that will be updated as the project evolves. Please refer to the latest version in the repository.
