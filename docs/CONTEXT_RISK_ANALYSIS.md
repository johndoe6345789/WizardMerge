# Context Analysis and Risk Analysis Features

## Overview

WizardMerge now includes intelligent context analysis and risk assessment features for merge conflicts, as outlined in ROADMAP.md Phase 3 (AI-Assisted Merging).

## Features

### Context Analysis

Context analysis examines the code surrounding merge conflicts to provide better understanding of the changes.

**Extracted Information:**
- **Function/Method Name**: Identifies which function contains the conflict
- **Class/Struct Name**: Identifies which class contains the conflict
- **Import/Include Statements**: Lists dependencies at the top of the file
- **Surrounding Lines**: Provides configurable context window (default: 5 lines)

**Supported Languages:**
- C/C++
- Python
- JavaScript/TypeScript
- Java

### Risk Analysis

Risk analysis assesses different resolution strategies and provides recommendations.

**Risk Levels:**
- **LOW**: Safe to merge, minimal risk
- **MEDIUM**: Some risk, review recommended
- **HIGH**: High risk, careful review required
- **CRITICAL**: Critical risk, requires expert review

**Resolution Strategies Analyzed:**
1. **Accept OURS**: Use our version
2. **Accept THEIRS**: Use their version
3. **Accept BOTH**: Concatenate both versions

**Risk Factors Detected:**
- Large number of changes (>10 lines)
- Critical code patterns (delete, eval, system calls, security operations)
- API signature changes
- Discarding significant changes from other branch

**Provided Information:**
- Risk level (low/medium/high/critical)
- Confidence score (0.0 to 1.0)
- List of risk factors
- Actionable recommendations

## API Usage

### HTTP API

When calling the `/api/merge` endpoint, conflict responses now include `context` and risk assessment fields:

```json
{
  "merged": [...],
  "has_conflicts": true,
  "conflicts": [
    {
      "start_line": 5,
      "end_line": 5,
      "base_lines": ["..."],
      "our_lines": ["..."],
      "their_lines": ["..."],
      "context": {
        "function_name": "myFunction",
        "class_name": "MyClass",
        "imports": ["#include <iostream>", "import sys"]
      },
      "risk_ours": {
        "level": "low",
        "confidence_score": 0.65,
        "risk_factors": [],
        "recommendations": ["Changes appear safe to accept"]
      },
      "risk_theirs": {
        "level": "low",
        "confidence_score": 0.60,
        "risk_factors": [],
        "recommendations": ["Changes appear safe to accept"]
      },
      "risk_both": {
        "level": "medium",
        "confidence_score": 0.30,
        "risk_factors": [
          "Concatenating both versions may cause duplicates or conflicts"
        ],
        "recommendations": [
          "Manual review required - automatic concatenation is risky",
          "Consider merging logic manually instead of concatenating",
          "Test thoroughly for duplicate or conflicting code"
        ]
      }
    }
  ]
}
```

### C++ API

```cpp
#include "wizardmerge/merge/three_way_merge.h"
#include "wizardmerge/analysis/context_analyzer.h"
#include "wizardmerge/analysis/risk_analyzer.h"

using namespace wizardmerge::merge;
using namespace wizardmerge::analysis;

// Perform merge
auto result = three_way_merge(base, ours, theirs);

// Access analysis for each conflict
for (const auto& conflict : result.conflicts) {
    // Context information
    std::cout << "Function: " << conflict.context.function_name << std::endl;
    std::cout << "Class: " << conflict.context.class_name << std::endl;
    
    // Risk assessment for "ours"
    std::cout << "Risk (ours): " 
              << risk_level_to_string(conflict.risk_ours.level) 
              << std::endl;
    std::cout << "Confidence: " 
              << conflict.risk_ours.confidence_score 
              << std::endl;
    
    // Recommendations
    for (const auto& rec : conflict.risk_ours.recommendations) {
        std::cout << "  - " << rec << std::endl;
    }
}
```

## Implementation Details

### Context Analyzer

**Header:** `backend/include/wizardmerge/analysis/context_analyzer.h`  
**Implementation:** `backend/src/analysis/context_analyzer.cpp`

Key functions:
- `analyze_context()`: Main analysis function
- `extract_function_name()`: Extract function/method name
- `extract_class_name()`: Extract class/struct name
- `extract_imports()`: Extract import statements

### Risk Analyzer

**Header:** `backend/include/wizardmerge/analysis/risk_analyzer.h`  
**Implementation:** `backend/src/analysis/risk_analyzer.cpp`

Key functions:
- `analyze_risk_ours()`: Assess risk of accepting ours
- `analyze_risk_theirs()`: Assess risk of accepting theirs
- `analyze_risk_both()`: Assess risk of concatenation
- `contains_critical_patterns()`: Detect security-critical code
- `has_api_signature_changes()`: Detect API changes

## Testing

Comprehensive test coverage with 24 unit tests:
- 7 tests for context analyzer
- 9 tests for risk analyzer
- 8 existing merge tests

Run tests:
```bash
cd backend/build
./wizardmerge-tests
```

## Security

All code has been scanned with CodeQL:
- **0 vulnerabilities found**
- Safe for production use

## Configuration

Risk analysis weights are configurable via constants in `risk_analyzer.cpp`:
- `BASE_CONFIDENCE`: Base confidence level (default: 0.5)
- `SIMILARITY_WEIGHT`: Weight for code similarity (default: 0.3)
- `CHANGE_RATIO_WEIGHT`: Weight for change ratio (default: 0.2)

Context analysis configuration:
- `IMPORT_SCAN_LIMIT`: Lines to scan for imports (default: 50)

## Future Enhancements

Potential improvements outlined in ROADMAP.md:
- ML-based confidence scoring
- Language-specific pattern detection
- Integration with LSP for deeper semantic analysis
- Historical conflict resolution learning
- Custom risk factor rules

## References

- ROADMAP.md: Phase 3, Section 3.1 (AI-Assisted Merging)
- Research Paper: docs/PAPER.md (dependency analysis methodology)
