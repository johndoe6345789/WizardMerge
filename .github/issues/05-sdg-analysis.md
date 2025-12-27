---
title: "Phase 2.1: System Dependence Graph (SDG) Analysis for Intelligent Conflict Resolution"
labels: ["enhancement", "phase-2", "sdg-analysis", "research", "high-priority"]
assignees: []
milestone: "Phase 2 - Intelligence & Usability"
---

## Overview

Implement System Dependence Graph (SDG) analysis based on the research paper from The University of Hong Kong. This is the core innovation of WizardMerge that achieves 28.85% reduction in conflict resolution time and provides merge suggestions for >70% of conflicted blocks.

## Related Roadmap Section

Phase 2.1 - Smart Conflict Resolution (SDG Analysis)

## Research Foundation

Based on the paper in `docs/PAPER.md`, which demonstrates:
- **28.85% reduction** in conflict resolution time
- **Merge suggestions for >70%** of code blocks affected by conflicts
- Tested on **227 conflicts** across five large-scale projects
- Uses **dependency analysis** at text and LLVM-IR levels

## What is SDG Analysis?

A System Dependence Graph (SDG) captures dependencies between code blocks:
- **Nodes**: Code blocks (statements, expressions, definitions)
- **Edges**: Dependencies (data flow, control flow)
- **Types**: 
  - Data dependencies (def-use relationships)
  - Control dependencies (branching, loops)
  - Call dependencies (function calls)

### Why SDG for Merging?

Traditional text-based merging ignores code semantics:
```python
# Base
x = 1
y = x + 1

# Ours: Change x
x = 2
y = x + 1

# Theirs: Use y
x = 1
y = x + 1
print(y)  # Depends on x!

# Text merge: May miss that x change affects print(y)
# SDG merge: Detects dependency and suggests keeping x = 2
```

## Architecture

### Multi-Level Dependency Analysis

```
┌─────────────────────────────────────────┐
│     Text-Level Dependencies             │
│  - Line-to-line dependencies            │
│  - Block-to-block dependencies          │
│  - Variable def-use (regex-based)       │
└─────────────────────────────────────────┘
                ↓
┌─────────────────────────────────────────┐
│     AST-Level Dependencies              │
│  - Function calls                       │
│  - Variable references                  │
│  - Class/method relationships           │
│  - Import dependencies                  │
└─────────────────────────────────────────┘
                ↓
┌─────────────────────────────────────────┐
│   LLVM-IR Level Dependencies (C/C++)    │
│  - Precise data flow                    │
│  - Control flow                         │
│  - Pointer aliasing                     │
│  - Memory dependencies                  │
└─────────────────────────────────────────┘
```

### SDG Construction

```cpp
class SystemDependenceGraph {
public:
  // Build SDG for a code file
  void build_graph(const std::string& code, const std::string& language);
  
  // Add nodes (code blocks)
  NodeID add_node(const CodeBlock& block);
  
  // Add edges (dependencies)
  void add_edge(NodeID from, NodeID to, DependencyType type);
  
  // Query dependencies
  std::vector<NodeID> get_dependencies(NodeID node);
  std::vector<NodeID> get_dependents(NodeID node);
  
  // Transitive closure
  std::set<NodeID> get_transitive_dependencies(NodeID node);
  
  // Conflict analysis
  ConflictAnalysis analyze_conflict(
    const SDG& base_sdg,
    const SDG& ours_sdg,
    const SDG& theirs_sdg
  );
};
```

### Dependency Types

```cpp
enum class DependencyType {
  // Data dependencies
  DATA_FLOW,         // x = 1; y = x;
  DEF_USE,           // Definition-use chain
  
  // Control dependencies
  CONTROL_FLOW,      // if (x) { y = 1; }
  CALL,              // foo(); (inside foo's body)
  
  // Structural dependencies
  IMPORT,            // import X; use X.method()
  INHERITANCE,       // class B extends A
  FIELD_ACCESS,      // obj.field
  
  // LLVM-IR dependencies (C/C++)
  MEMORY_ALIAS,      // Pointer aliasing
  LOAD_STORE,        // Memory load/store dependencies
};
```

## Features to Implement

### 1. Text-Level Dependency Analysis

- [ ] **Line-to-line dependencies**
  - Variable definition tracking (regex-based)
  - Variable use tracking
  - Build def-use chains
  
- [ ] **Block-level dependencies**
  - Identify code blocks (functions, loops, conditionals)
  - Track block boundaries
  - Build block dependency graph

**Algorithm**:
```
For each line:
  1. Extract variable definitions (x = ..., def foo():)
  2. Extract variable uses (y = x + 1)
  3. Create dependency edge: definition → use
```

### 2. AST-Level Dependency Analysis

- [ ] **Parse code into AST** (using tree-sitter)
- [ ] **Extract semantic elements**:
  - Function definitions and calls
  - Variable declarations and references
  - Class definitions and instantiations
  - Import/include statements
- [ ] **Build dependency graph**:
  - Function call graph
  - Variable reference graph
  - Import dependency graph
- [ ] **Detect conflicts**:
  - Modified functions with dependent code
  - Changed variables still in use
  - Removed imports still referenced

**Example**:
```python
# Base
def compute(x):
    return x * 2

result = compute(5)

# Ours: Change compute
def compute(x):
    return x * 3  # Changed!

result = compute(5)

# Theirs: Use result
def compute(x):
    return x * 2

result = compute(5)
print(result)  # Depends on compute!

# SDG Analysis: Detects that print(result) depends on compute()
# Suggests: Keep changed compute, preserve print
```

### 3. LLVM-IR Level Analysis (C/C++)

- [ ] **Compile to LLVM IR**
  - Use Clang to generate LLVM IR
  - Parse IR and build control flow graph (CFG)
  - Build data flow graph (DFG)
  
- [ ] **Analyze dependencies**:
  - Data flow (load/store, SSA form)
  - Control flow (branches, loops)
  - Memory dependencies (aliasing)
  - Function calls
  
- [ ] **Conflict detection**:
  - Detect violated dependencies
  - Find affected code blocks
  - Compute conflict impact radius

**Tools**: LLVM libraries (libclang, LLVM IR parser)

### 4. Conflict Analysis with SDG

```cpp
struct ConflictAnalysis {
  // Classification
  bool is_true_conflict;        // Semantic conflict
  bool is_false_conflict;       // Text conflict only
  
  // Impact
  std::set<NodeID> affected_blocks;  // Blocks affected by conflict
  int impact_radius;            // Distance in dependency graph
  
  // Dependencies
  std::vector<Dependency> violated_edges;  // Dependencies broken by merge
  std::vector<Dependency> safe_edges;      // Dependencies preserved
  
  // Suggestions
  std::vector<Resolution> suggestions;     // Merge suggestions
  double confidence;            // Confidence score (0-1)
};

class ConflictAnalyzer {
  ConflictAnalysis analyze(
    const SDG& base_sdg,
    const SDG& ours_sdg,
    const SDG& theirs_sdg,
    const ConflictRegion& conflict
  );
  
  // Classify edges
  void classify_edges();
  
  // Compute impact
  void compute_impact();
  
  // Generate suggestions
  void generate_suggestions();
};
```

### 5. Visualization

- [ ] **Dependency graph viewer**:
  - Interactive graph visualization
  - Show nodes (code blocks)
  - Show edges (dependencies)
  - Highlight conflicts and affected blocks
  
- [ ] **Impact visualization**:
  - Color-coded nodes (safe, conflicted, affected)
  - Show dependency paths
  - Display conflict impact radius
  
- [ ] **Suggestion UI**:
  - Show suggested resolutions
  - Display confidence scores
  - Explain reasoning (why this suggestion?)

**Library**: D3.js (web) or Qt Graphics (desktop)

## Implementation Steps

1. **Phase 1: Text-Level Analysis (2 weeks)**
   - Implement variable tracking
   - Build line-level dependency graph
   - Test on simple examples

2. **Phase 2: AST-Level Analysis (3 weeks)**
   - Integrate tree-sitter
   - Parse AST for Python, JS, Java, C++
   - Build semantic dependency graph
   - Test on real-world code

3. **Phase 3: LLVM-IR Analysis (3 weeks)**
   - Integrate LLVM/Clang
   - Generate and parse LLVM IR
   - Build precise dependency graph
   - Test on C/C++ projects

4. **Phase 4: Conflict Analysis (2 weeks)**
   - Implement edge classification
   - Compute conflict impact
   - Generate merge suggestions
   - Test on conflict datasets

5. **Phase 5: Visualization (2 weeks)**
   - Build dependency graph viewer
   - Integrate into UI
   - User testing

6. **Phase 6: Integration & Optimization (2 weeks)**
   - Integrate with merge pipeline
   - Optimize performance
   - Cache dependency graphs
   - Final testing

## Libraries and Tools

- **tree-sitter**: AST parsing
- **LLVM/Clang**: IR generation and analysis
- **Boost Graph Library**: Graph algorithms
- **D3.js**: Visualization (web)
- **Qt Graphics**: Visualization (desktop)

## Acceptance Criteria

- [ ] Text-level dependency analysis works
- [ ] AST-level dependency analysis works for Python, JS, Java, C++
- [ ] LLVM-IR analysis works for C/C++
- [ ] Conflict analyzer detects true vs. false conflicts
- [ ] Generates merge suggestions with confidence scores
- [ ] Achieves >70% suggestion rate on test dataset
- [ ] Reduces resolution time by >25% (user study)
- [ ] Visualization is clear and helpful
- [ ] Performance: <500ms for files up to 2000 lines
- [ ] Documentation complete

## Test Cases

### Text-Level
1. Simple def-use chain
2. Multiple definitions
3. Variable shadowing
4. Cross-block dependencies

### AST-Level
1. Function call dependencies
2. Import dependencies
3. Class inheritance
4. Variable references across scopes

### LLVM-IR (C/C++)
1. Pointer aliasing
2. Memory dependencies
3. Control flow dependencies
4. Function call dependencies

### Conflict Analysis
1. True conflict (semantic)
2. False conflict (text-only)
3. Transitive dependencies
4. Conflict impact radius
5. Suggestion generation

## Priority

**HIGH** - This is the core innovation of WizardMerge and the main research contribution.

## Estimated Effort

10-14 weeks (full implementation)

Incremental approach:
- **Milestone 1** (4 weeks): Text and AST analysis
- **Milestone 2** (4 weeks): LLVM-IR analysis
- **Milestone 3** (4 weeks): Conflict analysis and visualization

## Dependencies

- AST-based merging (Issue #TBD)
- Three-way merge algorithm ✅

## Related Issues

- #TBD (Phase 2 tracking)
- #TBD (AST-based merging)
- #TBD (Visualization enhancements)

## Success Metrics

- **28.85% reduction** in conflict resolution time (match research)
- **>70% suggestion rate** for conflicted blocks
- **90% user satisfaction** with SDG-based suggestions
- **High precision** (>80%) for conflict detection

## References

- Research paper: `docs/PAPER.md`
- Formal specification: `spec/WizardMergeSpec.tla`
- ROADMAP.md Phase 2.1

## Future Enhancements

- Machine learning for confidence scoring
- User feedback loop (learn from resolutions)
- Cross-file dependency analysis
- Language-specific semantic analysis
- Integration with LSP for real-time analysis
