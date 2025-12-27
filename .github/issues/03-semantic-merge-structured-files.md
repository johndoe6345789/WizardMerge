---
title: "Phase 2.1: Semantic Merge for Structured File Types (JSON, YAML, XML, Package Files)"
labels: ["enhancement", "phase-2", "semantic-merge", "high-priority"]
assignees: []
milestone: "Phase 2 - Intelligence & Usability"
---

## Overview

Implement intelligent, structure-aware merging for common structured file types. Instead of treating these files as plain text, understand their structure and merge at the semantic level.

## Related Roadmap Section

Phase 2.1 - Smart Conflict Resolution (Semantic merge for common file types)

## Motivation

Traditional text-based merging fails to understand the structure of JSON, YAML, XML, and package files. This leads to:
- Unnecessary conflicts in well-structured data
- Breaking valid syntax during merge
- Missing semantic relationships between changes
- Poor handling of reordered elements

Semantic merging can reduce conflicts by 30-50% in structured files.

## Features to Implement

### JSON Merging

- [ ] **Key-based merging**: Merge objects by key structure
- [ ] **Preserve nested objects**: Maintain hierarchy during merge
- [ ] **Smart array merging**: 
  - Detect ID fields (`id`, `_id`, `key`, etc.)
  - Match array elements by ID when possible
  - Handle insertions, deletions, and reordering
- [ ] **Structural vs. value changes**: Differentiate between structure modifications and value updates
- [ ] **Conflict detection**: Identify true semantic conflicts (e.g., same key, different values)

**Example**:
```json
// Base
{"users": [{"id": 1, "name": "Alice"}]}

// Ours
{"users": [{"id": 1, "name": "Alice"}, {"id": 2, "name": "Bob"}]}

// Theirs
{"users": [{"id": 1, "name": "Alice", "email": "alice@example.com"}]}

// Merged (semantic)
{"users": [{"id": 1, "name": "Alice", "email": "alice@example.com"}, {"id": 2, "name": "Bob"}]}
```

### YAML Merging

- [ ] **Hierarchy preservation**: Maintain indentation and structure
- [ ] **Comment preservation**: Keep comments associated with keys
- [ ] **Anchor and alias handling**: Preserve YAML anchors (`&`) and aliases (`*`)
- [ ] **Multi-document YAML**: Handle files with multiple YAML documents (`---` separators)
- [ ] **Schema-aware conflicts**: Detect conflicts based on YAML schema (if available)

**Example**:
```yaml
# Base
config:
  host: localhost
  port: 8080

# Ours (add new key)
config:
  host: localhost
  port: 8080
  timeout: 30

# Theirs (change value)
config:
  host: example.com  # Updated host
  port: 8080

# Merged
config:
  host: example.com
  port: 8080
  timeout: 30
```

### Package File Merging

Intelligent dependency merging for various ecosystems:

#### package.json (npm)
- [ ] Merge dependencies by semver ranges
- [ ] Detect version conflicts (incompatible ranges)
- [ ] Preserve script order and structure
- [ ] Handle devDependencies, peerDependencies separately

#### requirements.txt (pip)
- [ ] Detect version conflicts
- [ ] Merge inline comments
- [ ] Handle version specifiers (==, >=, ~=, etc.)

#### go.mod
- [ ] Merge require directives
- [ ] Handle replace directives
- [ ] Resolve version conflicts using go.sum

#### Cargo.toml (Rust)
- [ ] Merge dependencies table
- [ ] Handle feature flags
- [ ] Resolve version conflicts

#### pom.xml (Maven)
- [ ] Merge dependencies
- [ ] Handle dependency management
- [ ] Resolve version conflicts

**Common Features**:
- [ ] Detect breaking version upgrades
- [ ] Warn about incompatible version ranges
- [ ] Suggest conflict resolution based on semver

### XML Merging

- [ ] **Structure-aware**: Understand element hierarchy
- [ ] **DTD/Schema preservation**: Keep document type declarations
- [ ] **Attribute-based matching**: Match elements by `id`, `name`, or other attributes
- [ ] **Namespace handling**: Correctly handle XML namespaces
- [ ] **Comment preservation**: Maintain XML comments

**Example**:
```xml
<!-- Base -->
<config>
  <server id="main" host="localhost" />
</config>

<!-- Ours -->
<config>
  <server id="main" host="localhost" port="8080" />
</config>

<!-- Theirs -->
<config>
  <server id="main" host="example.com" />
  <server id="backup" host="backup.example.com" />
</config>

<!-- Merged -->
<config>
  <server id="main" host="example.com" port="8080" />
  <server id="backup" host="backup.example.com" />
</config>
```

## Technical Design

### Architecture

```cpp
// Abstract base class for semantic mergers
class SemanticMerger {
public:
  virtual MergeResult merge(
    const std::string& base,
    const std::string& ours,
    const std::string& theirs
  ) = 0;
  
  virtual bool can_handle(const std::string& file_path) = 0;
};

// Implementations
class JSONMerger : public SemanticMerger { /* ... */ };
class YAMLMerger : public SemanticMerger { /* ... */ };
class XMLMerger : public SemanticMerger { /* ... */ };
class PackageFileMerger : public SemanticMerger { /* ... */ };
```

### Merger Registry

```cpp
class SemanticMergerRegistry {
  void register_merger(std::unique_ptr<SemanticMerger> merger);
  SemanticMerger* find_merger(const std::string& file_path);
  MergeResult smart_merge(const std::string& file_path, /* ... */);
};
```

### Integration with Three-Way Merge

```cpp
// In three_way_merge.cpp
MergeResult merge_files(const std::string& base, /* ... */) {
  // Try semantic merge first
  auto semantic_merger = merger_registry.find_merger(file_path);
  if (semantic_merger) {
    auto result = semantic_merger->merge(base, ours, theirs);
    if (result.success || result.has_semantic_conflicts) {
      return result;
    }
  }
  
  // Fall back to text-based merge
  return text_based_merge(base, ours, theirs);
}
```

## Implementation Steps

1. **Set up semantic merge framework**
   - Create `backend/src/semantic/` directory
   - Define `SemanticMerger` interface
   - Create merger registry

2. **Implement JSON merger**
   - Use JSON library (jsoncpp, nlohmann/json, or similar)
   - Recursive object merging
   - Array merging with ID detection
   - Unit tests

3. **Implement YAML merger**
   - Use yaml-cpp library
   - Preserve comments and anchors
   - Handle multi-document files
   - Unit tests

4. **Implement XML merger**
   - Use libxml2 or tinyxml2
   - Element matching by attributes
   - Namespace handling
   - Unit tests

5. **Implement package file mergers**
   - Detect file types by name
   - Version comparison logic
   - Conflict reporting
   - Unit tests

6. **Integration**
   - Hook into main merge pipeline
   - Add file type detection
   - Update API to support semantic merging
   - Integration tests

7. **Documentation**
   - User guide for semantic merging
   - Examples for each file type
   - API documentation

## Libraries to Use

- **JSON**: nlohmann/json (header-only, modern C++)
- **YAML**: yaml-cpp (Conan available)
- **XML**: tinyxml2 (Conan available)
- **Semver**: semver.c or custom implementation

## Acceptance Criteria

- [ ] JSON files merge at object/array level
- [ ] YAML files preserve structure and comments
- [ ] XML files merge by element attributes
- [ ] Package files detect version conflicts
- [ ] Falls back to text merge when semantic merge fails
- [ ] All unit tests pass (>90% coverage)
- [ ] Integration tests with real-world examples
- [ ] Performance: <100ms for files up to 10MB
- [ ] Documentation complete

## Test Cases

### JSON
1. Merge objects with non-overlapping keys
2. Merge arrays with ID fields
3. Detect value conflicts for same key
4. Handle nested object merging
5. Preserve number precision

### YAML
1. Merge with comment preservation
2. Handle anchors and aliases
3. Multi-document YAML files
4. Nested structure merging

### XML
1. Match elements by ID attribute
2. Namespace handling
3. DTD preservation
4. Comment preservation

### Package Files
1. Merge dependencies without conflicts
2. Detect version conflicts
3. Handle different package managers
4. Preserve comments and formatting

## Priority

**HIGH** - This is a key differentiator for WizardMerge and significantly improves user experience.

## Estimated Effort

4-6 weeks

## Dependencies

- Three-way merge algorithm ✅
- File I/O module (Issue #TBD)

## Related Issues

- #TBD (Phase 2 tracking)
- #TBD (Language-aware AST merging)
- #TBD (SDG Analysis)

## Success Metrics

- Reduce conflicts in structured files by 40%
- 95% user satisfaction for JSON/YAML merging
- <100ms merge time for typical files
