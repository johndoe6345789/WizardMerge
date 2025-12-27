# TypeScript Support in WizardMerge

## Overview

WizardMerge includes comprehensive TypeScript support with context-aware analysis and intelligent merge risk assessment specifically designed for TypeScript codebases.

## Features

### 1. TypeScript Context Awareness

The context analyzer recognizes TypeScript-specific code patterns:

**Function Detection:**
- Regular functions: `function myFunc()`, `export function myFunc()`
- Async functions: `async function myFunc()`
- Arrow functions: `const myFunc = () => {}`
- Typed arrow functions: `const myFunc = (x: number) => {}`
- Method signatures: `myMethod(param: string): ReturnType`

**Type Structures:**
- Interfaces: `interface User { ... }`
- Type aliases: `type Status = 'pending' | 'approved'`
- Enums: `enum Color { Red, Green, Blue }`
- Export declarations: `export interface`, `export type`, `export enum`

**Import Patterns:**
- Named imports: `import { Component } from 'react'`
- Type imports: `import type { User } from './types'`
- Namespace imports: `import * as utils from './utils'`
- Re-exports: `export { User } from './types'`
- Export all: `export * from './types'`

### 2. Package Lock Conflict Handling

WizardMerge intelligently detects package lock files and can apply special merge strategies:

**Supported Lock Files:**
- `package-lock.json` (npm)
- `yarn.lock` (Yarn)
- `pnpm-lock.yaml` (pnpm)
- `bun.lockb` (Bun)

**Why Package Locks Are Special:**
Package lock files are notoriously conflict-prone because:
- They're automatically generated
- They change with every dependency update
- Conflicts are extremely common in team environments
- Manual resolution is error-prone

**Detection API:**
```cpp
#include "wizardmerge/analysis/risk_analyzer.h"

std::string filename = "package-lock.json";
if (is_package_lock_file(filename)) {
    // Apply special merge strategy
    // Suggestion: regenerate lock file instead of manual merge
}
```

### 3. TypeScript Merge Risk Analysis

The risk analyzer includes TypeScript-specific risk factors:

**Type Safety Risks:**
- **`as any` casts**: Bypasses TypeScript's type system
- **`@ts-ignore`**: Suppresses type errors on next line
- **`@ts-nocheck`**: Disables type checking for entire file

**Security Risks:**
- **`dangerouslySetInnerHTML`**: React XSS vulnerability vector
- **`innerHTML =`**: Direct DOM manipulation, XSS risk
- **`localStorage.setItem(...password...)`**: Insecure password storage

**Breaking Changes:**
- Interface modifications (adding/removing/changing properties)
- Type alias changes
- Enum modifications
- Function signature changes with type annotations

## API Usage Examples

### Detecting TypeScript Interface Changes

```cpp
#include "wizardmerge/analysis/risk_analyzer.h"

std::vector<std::string> base = {
    "interface User {",
    "    id: number;",
    "    name: string;",
    "}"
};

std::vector<std::string> modified = {
    "interface User {",
    "    id: number;",
    "    name: string;",
    "    email: string;  // Added field",
    "}"
};

if (has_typescript_interface_changes(base, modified)) {
    std::cout << "Breaking change: Interface modified" << std::endl;
    std::cout << "Recommendation: Review all usages of User interface" << std::endl;
}
```

### Analyzing TypeScript Code Risk

```cpp
#include "wizardmerge/analysis/risk_analyzer.h"

std::vector<std::string> base = {"const user: User = data;"};
std::vector<std::string> ours = {"const user = data as any;"};
std::vector<std::string> theirs = {"const user: User = data;"};

auto risk = analyze_risk_ours(base, ours, theirs);

if (risk.affects_critical_section) {
    std::cout << "Warning: TypeScript type safety bypassed!" << std::endl;
}

for (const auto& factor : risk.risk_factors) {
    std::cout << "Risk: " << factor << std::endl;
}
// Output: "Risk: Contains critical code patterns (security/data operations)"
```

### Full Context Analysis for TypeScript

```cpp
#include "wizardmerge/analysis/context_analyzer.h"

std::vector<std::string> typescript_code = {
    "import { useState } from 'react';",
    "import type { User } from './types';",
    "",
    "interface Props {",
    "    user: User;",
    "}",
    "",
    "export const UserCard = ({ user }: Props) => {",
    "    const [expanded, setExpanded] = useState(false);",
    "    return <div>{user.name}</div>;",
    "};"
};

auto context = analyze_context(typescript_code, 7, 9);

std::cout << "Function: " << context.function_name << std::endl;
// Output: "Function: UserCard"

std::cout << "Type: " << context.class_name << std::endl;
// Output: "Type: Props"

std::cout << "Imports:" << std::endl;
for (const auto& import : context.imports) {
    std::cout << "  - " << import << std::endl;
}
// Output:
//   - import { useState } from 'react';
//   - import type { User } from './types';
```

## Integration with Merge Workflow

### Example: Smart TypeScript Merge

```cpp
#include "wizardmerge/merge/three_way_merge.h"
#include "wizardmerge/analysis/context_analyzer.h"
#include "wizardmerge/analysis/risk_analyzer.h"

// Perform three-way merge
auto result = three_way_merge(base_lines, our_lines, their_lines);

// Analyze each conflict
for (const auto& conflict : result.conflicts) {
    // Get context
    auto context = analyze_context(base_lines, 
                                   conflict.start_line, 
                                   conflict.end_line);
    
    // Check if we're in a TypeScript interface
    if (context.class_name.find("interface") != std::string::npos ||
        context.class_name.find("type") != std::string::npos) {
        std::cout << "Conflict in TypeScript type definition: " 
                  << context.class_name << std::endl;
    }
    
    // Assess risks
    auto risk_ours = analyze_risk_ours(conflict.base_lines,
                                       conflict.our_lines,
                                       conflict.their_lines);
    
    if (has_typescript_interface_changes(conflict.base_lines, 
                                        conflict.our_lines)) {
        std::cout << "Warning: Accepting OURS will change type definitions" 
                  << std::endl;
    }
    
    // Check for type safety violations
    if (contains_critical_patterns(conflict.our_lines)) {
        std::cout << "Critical: Code contains type safety bypasses!" 
                  << std::endl;
    }
}
```

### Example: Package Lock Conflict Resolution

```cpp
#include "wizardmerge/analysis/risk_analyzer.h"

std::string filename = "package-lock.json";

if (is_package_lock_file(filename)) {
    std::cout << "Package lock file detected!" << std::endl;
    std::cout << "Recommendation: Regenerate instead of merging" << std::endl;
    std::cout << "Steps:" << std::endl;
    std::cout << "  1. Delete package-lock.json" << std::endl;
    std::cout << "  2. Merge package.json manually" << std::endl;
    std::cout << "  3. Run 'npm install' to regenerate lock file" << std::endl;
    std::cout << "  4. Commit the new lock file" << std::endl;
    
    // Skip manual merge and suggest regeneration
    return;
}
```

## Testing

WizardMerge includes comprehensive tests for TypeScript support:

### Context Analyzer Tests
- `TypeScriptFunctionDetection`: Verifies async function detection
- `TypeScriptArrowFunctionDetection`: Tests arrow function parsing
- `TypeScriptInterfaceDetection`: Validates interface extraction
- `TypeScriptTypeAliasDetection`: Tests type alias recognition
- `TypeScriptEnumDetection`: Verifies enum parsing
- `TypeScriptImportExtraction`: Tests import statement detection

### Risk Analyzer Tests
- `TypeScriptInterfaceChangesDetected`: Validates interface change detection
- `TypeScriptTypeChangesDetected`: Tests type alias modifications
- `TypeScriptEnumChangesDetected`: Verifies enum change detection
- `PackageLockFileDetection`: Tests lock file identification
- `TypeScriptCriticalPatternsDetected`: Validates detection of type safety bypasses
- `TypeScriptSafeCodeNoFalsePositives`: Ensures safe code doesn't trigger warnings
- `RiskAnalysisIncludesTypeScriptChanges`: Integration test for risk assessment

### Running Tests

```bash
cd backend/build
./wizardmerge-tests --gtest_filter="*TypeScript*"
```

All TypeScript tests pass with 100% success rate.

## Best Practices

### When Merging TypeScript Code

1. **Always review interface/type changes**: Breaking changes can affect many files
2. **Watch for type safety bypasses**: `as any`, `@ts-ignore` should be rare
3. **Be cautious with package lock conflicts**: Consider regenerating instead of manual merge
4. **Check import changes**: Missing or duplicate imports can break builds
5. **Validate after merge**: Run TypeScript compiler to catch type errors

### Package Lock Files

**Recommended Strategy:**
1. Don't manually merge package lock files
2. Merge `package.json` first
3. Delete the lock file
4. Run package manager to regenerate it
5. This ensures consistency and avoids corruption

**Why This Works:**
- Lock files are deterministic - given the same `package.json`, you get the same lock
- Manual merging can create invalid dependency trees
- Regeneration is faster and safer than manual resolution

## Language Support Summary

| Feature | Support Level |
|---------|--------------|
| Function detection | ✅ Full |
| Arrow functions | ✅ Full |
| Async/await | ✅ Full |
| Interfaces | ✅ Full |
| Type aliases | ✅ Full |
| Enums | ✅ Full |
| Generics | ⚠️ Partial (detected as part of function signatures) |
| Decorators | ⚠️ Partial (detected in context) |
| TSX/JSX | ✅ Full (treated as TypeScript) |
| Import patterns | ✅ Full |
| Type safety validation | ✅ Full |
| Package lock detection | ✅ Full |

## Future Enhancements

Potential improvements for TypeScript support:

1. **Semantic merging**: Parse AST to merge at type level instead of line level
2. **Dependency tree analysis**: Detect impact of type changes across files
3. **Auto-fix suggestions**: Propose specific merge resolutions based on type information
4. **Integration with TypeScript compiler**: Use `tsc` for validation
5. **Package version conflict resolution**: Smart handling of semver ranges in lock files

## See Also

- [Context and Risk Analysis Documentation](CONTEXT_RISK_ANALYSIS.md)
- [ROADMAP](../ROADMAP.md) - Phase 2.1: Smart Conflict Resolution
- [Backend API Documentation](../backend/README.md)
