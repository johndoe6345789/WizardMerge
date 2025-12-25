# WizardMerge Roadmap

## Research Foundation

WizardMerge is based on research from The University of Hong Kong. The complete research paper has been extracted via OCR and is available in [`docs/PAPER.md`](docs/PAPER.md).

**Key Research Insights:**
- Traditional Git merging uses textual-based strategies that ignore syntax and semantics
- WizardMerge achieves 28.85% reduction in conflict resolution time
- Provides merge suggestions for over 70% of code blocks affected by conflicts
- Uses code block dependency analysis at text and LLVM-IR levels
- Tested on 227 conflicts across five large-scale projects

## Vision

WizardMerge aims to become the most intuitive and powerful tool for resolving merge conflicts in software development. By combining intelligent algorithms with a clean, accessible UI, we want to make merge conflict resolution from a dreaded task into a smooth, understandable process.

## Core Principles

1. **Visual Clarity**: Show conflicts in a way that makes the problem immediately obvious
2. **Smart Assistance**: Provide intelligent suggestions while keeping humans in control
3. **Context Awareness**: Understand code structure and semantics, not just text diffs
4. **Workflow Integration**: Seamlessly fit into developers' existing Git workflows
5. **Safety First**: Make it hard to accidentally lose changes or break code

---

## Phase 1: Foundation (0-3 months)

### 1.1 Enhanced Merge Algorithm
**Priority: HIGH**

- [ ] Implement three-way merge algorithm (base, ours, theirs)
- [ ] Add conflict detection and marking
- [ ] Support for different conflict markers (Git, Mercurial, etc.)
- [ ] Line-level granularity with word-level highlighting
- [ ] Handle common auto-resolvable patterns:
  - Non-overlapping changes
  - Identical changes from both sides
  - Whitespace-only differences

**Deliverable**: `wizardmerge/algo/three_way_merge.py` module

### 1.2 File Input/Output
**Priority: HIGH**

- [ ] Parse Git conflict markers from files
- [ ] Load base, ours, and theirs versions from Git
- [ ] Save resolved merge results
- [ ] Support for directory-level conflict resolution
- [ ] Backup mechanism for safety

**Deliverable**: `wizardmerge/io/` module with file handlers

### 1.3 Core UI Components
**Priority: HIGH**

- [ ] Three-panel diff view (base, ours, theirs)
- [ ] Unified conflict view with inline markers
- [ ] Syntax highlighting for common languages
- [ ] Line numbering and navigation
- [ ] Conflict counter and navigation (next/previous conflict)

**Deliverable**: Enhanced `main.qml` with conflict viewer components

### 1.4 Basic Conflict Resolution Actions
**Priority: MEDIUM**

- [ ] Accept ours / Accept theirs buttons
- [ ] Accept both (concatenate) option
- [ ] Manual edit capability
- [ ] Undo/redo stack
- [ ] Keyboard shortcuts for common actions

**Deliverable**: Action handlers in QML and Python backend

### 1.5 Git Integration
**Priority: MEDIUM**

- [ ] Detect when running in Git repository
- [ ] Read `.git/MERGE_HEAD` to identify conflicts
- [ ] List all conflicted files
- [ ] Mark files as resolved in Git
- [ ] Launch from command line: `wizardmerge [file]`

**Deliverable**: `wizardmerge/git/` module and CLI enhancements

---

## Phase 2: Intelligence & Usability (3-6 months)

### 2.1 Smart Conflict Resolution
**Priority: HIGH**

- [ ] Semantic merge for common file types:
  - JSON: merge by key structure
  - YAML: preserve hierarchy
  - Package files: intelligent dependency merging
  - XML: structure-aware merging
- [ ] Language-aware merging (AST-based):
  - Python imports and functions
  - JavaScript/TypeScript modules
  - Java classes and methods
- [ ] Auto-resolution suggestions with confidence scores
- [ ] Learn from user's resolution patterns

**Deliverable**: `wizardmerge/algo/semantic/` module

### 2.2 Enhanced Visualization
**Priority: MEDIUM**

- [ ] Side-by-side diff view option
- [ ] Minimap for large files
- [ ] Color-coded change types (added, removed, modified, conflicted)
- [ ] Collapsible unchanged regions
- [ ] Blame/history annotations
- [ ] Conflict complexity indicator

**Deliverable**: Advanced QML components and visualization modes

### 2.3 Code Intelligence
**Priority: MEDIUM**

- [ ] Integration with Language Server Protocol (LSP)
- [ ] Syntax validation during merge
- [ ] Show syntax errors in real-time
- [ ] Auto-formatting after resolution
- [ ] Import/dependency conflict detection

**Deliverable**: `wizardmerge/lsp/` integration module

### 2.4 Multi-Frontend Architecture
**Priority: HIGH**

- [ ] Abstract core merge engine from UI layer
- [ ] Define clean API between frontend and backend
- [ ] C++ backend implementation for performance-critical operations
- [ ] C++/Qt6 native desktop frontend
- [ ] Next.js WebUI frontend for browser-based access
- [ ] Shared state management across frontends
- [ ] RESTful or gRPC API for frontend-backend communication
- [ ] WebSocket support for real-time updates

**Deliverable**: `wizardmerge/core/` (backend abstraction), `frontends/qt6/` (C++/Qt6), `frontends/web/` (Next.js)

### 2.5 Collaboration Features
**Priority: LOW**

- [ ] Add comments to conflicts
- [ ] Mark conflicts for review
- [ ] Export resolution report
- [ ] Share conflict context via link
- [ ] Team resolution patterns library

**Deliverable**: Collaboration UI and sharing infrastructure

### 2.6 Testing & Quality
**Priority: HIGH**

- [ ] Comprehensive test suite for merge algorithms
- [ ] UI automation tests for all frontends
- [ ] Performance benchmarks for large files
- [ ] Fuzzing for edge cases
- [ ] Documentation and examples

**Deliverable**: `tests/` directory with full coverage

---

## Phase 3: Advanced Features (6-12 months)

### 3.1 AI-Assisted Merging
**Priority: MEDIUM**

- [ ] ML model for conflict resolution suggestions
- [ ] Pattern recognition from repository history
- [ ] Natural language explanations of conflicts
- [ ] Context-aware code completion during merge
- [ ] Risk assessment for resolution choices

**Deliverable**: `wizardmerge/ai/` module with ML models

### 3.2 Multi-Repository Support
**Priority: LOW**

- [ ] Support for monorepos
- [ ] Cross-repository dependency tracking
- [ ] Batch conflict resolution
- [ ] Conflict prevention suggestions during PR review

**Deliverable**: Enhanced Git integration with multi-repo awareness

### 3.3 Advanced Git Workflows
**Priority: MEDIUM**

- [ ] Rebase conflict resolution mode
- [ ] Cherry-pick conflict handling
- [ ] Merge strategy selection (recursive, ours, theirs, octopus)
- [ ] Submodule conflict resolution
- [ ] Partial staging of resolved conflicts

**Deliverable**: Comprehensive Git workflow support

### 3.4 Plugin Ecosystem
**Priority: LOW**

- [ ] Plugin API for custom merge strategies
- [ ] Community plugin repository
- [ ] Language-specific plugins (Go, Rust, C++, etc.)
- [ ] IDE integrations (VSCode, IntelliJ, etc.)
- [ ] Custom visualization plugins

**Deliverable**: Plugin system architecture and marketplace

### 3.5 Performance & Scale
**Priority: MEDIUM**

- [ ] Handle files with 100k+ lines
- [ ] Streaming diff for large files
- [ ] Incremental parsing and rendering
- [ ] Background processing for analysis
- [ ] Memory-efficient data structures

**Deliverable**: Performance optimizations throughout codebase

---

## Technical Architecture

### Current Stack
- **UI**: PyQt6 + QML (declarative UI)
- **Backend**: Python 3.8+
- **Themes**: Plugin-based theming system
- **Algorithms**: Custom merge utilities

### Multi-Frontend Architecture (Proposed)

**Core Philosophy**: Separate merge logic from presentation layer to support multiple frontend options while maintaining a single, robust backend.

#### Backend (C++)
- **Core Engine**: High-performance merge algorithms in C++
- **Rationale**: Performance-critical operations (large file parsing, AST analysis, diff computation)
- **API Layer**: RESTful/gRPC interface for frontend communication
- **Components**:
  - Three-way merge engine
  - Conflict detection and resolution
  - Git integration layer
  - File I/O and parsing
  - Semantic analysis engine

#### Frontend Options

1. **Qt6 Native (C++)**
   - **Target**: Desktop users (Linux, Windows, macOS)
   - **Advantages**: Native performance, full desktop integration, offline capability
   - **Components**: Qt6 Widgets/QML UI, direct C++ backend integration
   - **Distribution**: Standalone binaries

2. **Next.js WebUI (TypeScript/React)**
   - **Target**: Browser-based access, cross-platform, team collaboration
   - **Advantages**: No installation, universal access, easy updates, collaborative features
   - **Components**: React UI components, REST/WebSocket API client
   - **Distribution**: Self-hosted or cloud service

3. **PyQt6 (Legacy/Reference)**
   - **Status**: Current implementation, to be maintained as reference
   - **Purpose**: Rapid prototyping, Python-centric workflows
   - **Future**: May be deprecated in favor of Qt6 C++ version

### Proposed Additions
- **Diff Library**: `diff-match-patch` or `difflib` enhancements
- **Git Integration**: `libgit2` (C++) or `GitPython` (Python fallback)
- **Syntax Highlighting**: `Pygments` (Python), `highlight.js` (Web), Qt SyntaxHighlighter (Qt6)
- **AST Parsing**: `tree-sitter` (C++ bindings), Language-specific parsers
- **LSP**: Language Server Protocol integration for all frontends
- **Testing**: `pytest` (Python), `gtest` (C++), `Jest` (TypeScript)
- **ML (future)**: `scikit-learn` or lightweight transformers
- **API Framework**: `FastAPI` (Python) or `Crow` (C++) for backend API
- **WebSockets**: `socket.io` for real-time updates in WebUI

### Architecture Decisions

1. **Multi-Frontend Abstraction**
   - **Backend Core**: C++ for performance-critical merge operations
   - **API Layer**: Clean RESTful/gRPC interface between frontend and backend
   - **Frontend Choice**: Qt6 C++ for native desktop, Next.js for web/collaboration
   - **Rationale**: Users choose their preferred interface while sharing the same robust engine

2. **Separation of Concerns**
   - Keep merge algorithms pure and testable
   - UI communicates via well-defined API
   - Git operations isolated in dedicated module
   - Each frontend can evolve independently

3. **Performance First**
   - C++ backend for computationally expensive operations
   - Lazy loading for large files
   - Background threads for expensive operations
   - Incremental updates to UI
   - WebSocket for real-time web updates

4. **Extensibility**
   - Plugin system for merge strategies
   - Theme system for all frontends
   - Configuration file support
   - API versioning for backward compatibility

5. **Safety**
   - Never modify original files until confirmed
   - Auto-save drafts
   - Full undo history
   - Backup before resolve

---

## Success Metrics

### Phase 1
- [ ] Successfully resolve basic three-way merges
- [ ] Handle 90% of common conflict patterns
- [ ] Command-line integration working
- [ ] 5 active users providing feedback

### Phase 2
- [ ] Auto-resolve 50% of simple conflicts
- [ ] Support 10+ programming languages
- [ ] UI response time < 100ms for typical files
- [ ] 100+ active users

### Phase 3
- [ ] 80% user satisfaction rating
- [ ] Handle repositories with 1M+ lines
- [ ] 10+ community plugins
- [ ] 1000+ active users

---

## Community & Contribution

### Getting Involved
1. **Try it out**: Use WizardMerge on real conflicts
2. **Report issues**: Help us identify edge cases
3. **Suggest features**: What would make your life easier?
4. **Contribute code**: Pick an item from the roadmap
5. **Write plugins**: Extend for your use case

### Development Priorities
We'll prioritize based on:
- **User feedback**: What real developers need most
- **Impact**: Features that help the most people
- **Feasibility**: What we can build well with available resources
- **Foundation first**: Core functionality before advanced features

---

## Related Projects

- **mergebot**: https://github.com/JohnDoe6345789/mergebot - Companion automation tool
- **Git**: Native three-way merge
- **KDiff3**: Mature merge tool
- **Meld**: Visual diff and merge
- **Beyond Compare**: Commercial option

Our niche: **Modern UI + intelligent algorithms + seamless workflow integration**

---

## Timeline Summary

**Quarter 1**: Foundation - Core merge algorithm, file I/O, basic UI
**Quarter 2**: Intelligence - Semantic merging, enhanced visualization
**Quarter 3**: Polish - Testing, performance, collaboration features
**Quarter 4**: Advanced - AI assistance, plugins, scale

---

## Next Steps

1. **Immediate**: Implement three-way merge algorithm
2. **This Sprint**: Add file input/output and Git integration
3. **This Quarter**: Complete Phase 1 foundation items
4. **Feedback Loop**: Release early, get user feedback, iterate

---

*This roadmap is a living document. Priorities may shift based on user feedback and technical discoveries. Last updated: December 2024*
