---
title: "Phase 2.1: Language-Aware AST-Based Merging (Python, JavaScript, Java, C/C++)"
labels: ["enhancement", "phase-2", "ast-merge", "high-priority"]
assignees: []
milestone: "Phase 2 - Intelligence & Usability"
---

## Overview

Implement Abstract Syntax Tree (AST) based merging for programming languages. Parse code into AST, merge at the semantic level, and regenerate code. This enables intelligent merging that understands language structure and semantics.

## Related Roadmap Section

Phase 2.1 - Smart Conflict Resolution (Language-aware merging)

## Motivation

Text-based merging treats code as plain text, leading to:
- Conflicts in imports even when they don't overlap
- Breaking syntax during merge
- Missing semantic relationships (e.g., a function using an imported module)
- False conflicts from formatting/whitespace changes

AST-based merging can:
- Merge import statements intelligently (deduplicate, sort)
- Detect real semantic conflicts vs. formatting conflicts
- Preserve code structure and validity
- Understand language-specific constructs (decorators, annotations, etc.)

## Languages to Support

### Priority 1 (High Usage)
- Python
- JavaScript/TypeScript
- Java
- C/C++

### Priority 2 (Future)
- Go
- Rust
- C#
- Ruby
- PHP

## Features by Language

### Python

- [ ] **Import merging**
  - Deduplicate imports
  - Merge `from X import Y, Z` statements
  - Preserve import aliases
  - Detect conflicting aliases
- [ ] **Function definitions**
  - Merge non-overlapping functions
  - Detect signature conflicts
  - Handle decorators intelligently
- [ ] **Class hierarchies**
  - Merge methods in classes
  - Handle inheritance changes
  - Merge class attributes
- [ ] **Type hints**
  - Preserve type annotations
  - Merge type imports from `typing`
- [ ] **Docstrings**
  - Preserve and merge docstrings

**Example**:
```python
# Base
import os
def hello():
    pass

# Ours
import os
import sys
def hello():
    pass
def world():
    pass

# Theirs
import os
from pathlib import Path
def hello():
    """Say hello"""
    pass

# Merged (AST-based)
import os
import sys
from pathlib import Path

def hello():
    """Say hello"""
    pass

def world():
    pass
```

### JavaScript/TypeScript

- [ ] **Import/Export merging**
  - Merge ES6 imports (`import X from 'Y'`)
  - Handle named vs default exports
  - Deduplicate imports
- [ ] **Module analysis**
  - Detect exported functions/classes
  - Handle re-exports
- [ ] **React components**
  - Merge component props
  - Handle JSX conflicts
  - Detect hook usage conflicts
- [ ] **Type definitions (TypeScript)**
  - Merge interfaces
  - Handle type aliases
  - Resolve type conflicts

**Example**:
```typescript
// Base
import { useState } from 'react';

// Ours
import { useState, useEffect } from 'react';
function MyComponent() { /* ... */ }

// Theirs
import { useState } from 'react';
import axios from 'axios';

// Merged
import { useState, useEffect } from 'react';
import axios from 'axios';

function MyComponent() { /* ... */ }
```

### Java

- [ ] **Package declarations**
  - Ensure consistent package
  - Detect package conflicts
- [ ] **Import statements**
  - Merge imports
  - Remove unused imports
  - Sort imports
- [ ] **Class structure**
  - Merge methods
  - Handle overloaded methods
  - Merge fields
- [ ] **Annotations**
  - Preserve annotations
  - Detect annotation conflicts
- [ ] **Method signatures**
  - Detect incompatible changes to method signatures
  - Handle generics

**Example**:
```java
// Base
import java.util.List;
public class MyClass {
    public void doSomething() { }
}

// Ours
import java.util.List;
import java.util.Map;
public class MyClass {
    public void doSomething() { }
    public void doMore() { }
}

// Theirs
import java.util.List;
import java.io.File;
public class MyClass {
    @Override
    public void doSomething() { }
}

// Merged
import java.io.File;
import java.util.List;
import java.util.Map;

public class MyClass {
    @Override
    public void doSomething() { }
    
    public void doMore() { }
}
```

### C/C++

- [ ] **Include directives**
  - Merge `#include` statements
  - Preserve include guards
  - Deduplicate includes
- [ ] **Macro definitions**
  - Detect conflicting macros
  - Merge non-overlapping macros
- [ ] **Function declarations**
  - Merge forward declarations
  - Handle function overloading (C++)
- [ ] **Namespace handling (C++)**
  - Merge namespace contents
  - Handle using directives
- [ ] **Class definitions (C++)**
  - Merge member functions
  - Handle access specifiers
  - Merge nested classes

**Example**:
```cpp
// Base
#include <iostream>

void foo();

// Ours
#include <iostream>
#include <vector>

void foo();
void bar();

// Theirs
#include <iostream>
#include <string>

void foo();
namespace utils {
    void helper();
}

// Merged
#include <iostream>
#include <string>
#include <vector>

void foo();
void bar();

namespace utils {
    void helper();
}
```

## Technical Design

### Architecture

```cpp
// Abstract AST merger interface
class ASTMerger {
public:
  virtual MergeResult merge(
    const std::string& base,
    const std::string& ours,
    const std::string& theirs
  ) = 0;
  
  virtual bool can_handle(const std::string& file_path) = 0;
  
protected:
  virtual ParseTree parse(const std::string& code) = 0;
  virtual ParseTree merge_ast(ParseTree base, ParseTree ours, ParseTree theirs) = 0;
  virtual std::string generate_code(ParseTree merged) = 0;
};

// Language-specific implementations
class PythonASTMerger : public ASTMerger { /* ... */ };
class JavaScriptASTMerger : public ASTMerger { /* ... */ };
class JavaASTMerger : public ASTMerger { /* ... */ };
class CPPASTMerger : public ASTMerger { /* ... */ };
```

### Using Tree-sitter

Tree-sitter provides fast, incremental parsing for many languages:

```cpp
#include <tree_sitter/api.h>

class TreeSitterMerger : public ASTMerger {
  TSParser* parser;
  const TSLanguage* language;
  
  ParseTree parse(const std::string& code) override {
    TSTree* tree = ts_parser_parse_string(parser, nullptr, code.c_str(), code.length());
    return ParseTree(tree);
  }
  
  // Traverse AST and merge nodes
  TSNode merge_nodes(TSNode base, TSNode ours, TSNode theirs);
};
```

### Merge Strategy

1. **Parse all three versions** into AST
2. **Identify top-level constructs** (imports, functions, classes)
3. **Match constructs by name/signature**
4. **Merge non-overlapping constructs**
5. **Detect conflicts** (same construct, different body)
6. **Generate merged code** from merged AST
7. **Format output** (use language-specific formatter)

## Implementation Steps

1. **Set up tree-sitter integration**
   - Add tree-sitter dependency to Conan
   - Add language grammars (Python, JS, Java, C++)
   - Create wrapper classes

2. **Implement Python merger**
   - Parse Python code with tree-sitter
   - Identify imports, functions, classes
   - Implement merge logic
   - Code generation
   - Unit tests

3. **Implement JavaScript/TypeScript merger**
   - Parse JS/TS code
   - Handle ES6 modules
   - Merge imports and exports
   - Unit tests

4. **Implement Java merger**
   - Parse Java code
   - Handle packages and imports
   - Merge class members
   - Unit tests

5. **Implement C/C++ merger**
   - Parse C/C++ code
   - Handle includes and macros
   - Merge declarations and definitions
   - Unit tests

6. **Integration**
   - Register AST mergers in merger registry
   - Add file extension detection
   - Integration tests

7. **Formatting**
   - Integrate with clang-format (C/C++)
   - Use prettier (JavaScript/TypeScript)
   - Use black or autopep8 (Python)
   - Use google-java-format (Java)

8. **Documentation**
   - User guide for AST merging
   - Examples for each language
   - Limitations and edge cases

## Libraries and Tools

- **tree-sitter**: Fast, incremental parsing
  - tree-sitter-python
  - tree-sitter-javascript
  - tree-sitter-java
  - tree-sitter-cpp
- **Formatters**:
  - clang-format (C/C++)
  - prettier (JS/TS)
  - black/autopep8 (Python)
  - google-java-format (Java)

## Acceptance Criteria

- [ ] Can parse and merge Python files
- [ ] Can parse and merge JavaScript/TypeScript files
- [ ] Can parse and merge Java files
- [ ] Can parse and merge C/C++ files
- [ ] Imports/includes are merged intelligently
- [ ] Syntax validity is preserved
- [ ] Falls back to text merge on parse errors
- [ ] Unit tests for each language (>90% coverage)
- [ ] Integration tests with real-world code
- [ ] Performance: <200ms for files up to 5000 lines
- [ ] Documentation complete

## Test Cases

### Python
1. Merge imports (deduplicate)
2. Merge from-imports
3. Merge functions with decorators
4. Merge class methods
5. Handle type hints
6. Preserve docstrings

### JavaScript/TypeScript
1. Merge ES6 imports
2. Handle default and named exports
3. Merge React components
4. Handle TypeScript interfaces
5. Preserve JSX

### Java
1. Merge imports
2. Merge class methods
3. Handle method overloads
4. Preserve annotations
5. Merge nested classes

### C/C++
1. Merge include directives
2. Handle header guards
3. Merge function declarations
4. Handle namespaces (C++)
5. Merge macro definitions

## Priority

**HIGH** - Language-aware merging is a key differentiator and complements semantic merging.

## Estimated Effort

6-8 weeks

## Dependencies

- Semantic merge framework (Issue #TBD)
- Three-way merge algorithm ✅

## Related Issues

- #TBD (Phase 2 tracking)
- #TBD (Semantic merge for structured files)
- #TBD (SDG Analysis)

## Success Metrics

- Reduce conflicts in code files by 50%
- Preserve syntax validity in 99% of merges
- 90% user satisfaction for AST merging
- <200ms merge time for typical files

## Future Enhancements

- Support for more languages (Go, Rust, C#)
- Semantic conflict detection (e.g., variable renamed but still used)
- Integration with LSP for real-time validation
- AI-assisted resolution for complex conflicts
