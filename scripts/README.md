# Scripts

This directory contains utility scripts for the WizardMerge project.

## tlaplus.py

TLA+ Model Checker runner for continuous integration.

### Usage

```bash
python3 scripts/tlaplus.py run
```

### What it does

1. **Downloads TLA+ Tools**: Automatically downloads the TLA+ tools JAR file (containing TLC model checker and SANY parser) to `.tlaplus/` directory if not already present.

2. **Parses Specification**: Runs the SANY parser on `spec/WizardMergeSpec.tla` to verify:
   - Syntax correctness
   - Module structure validity
   - Type checking

3. **Generates Output**: Saves parsing results to `ci-results/WizardMergeSpec_parse.log`

### CI Integration

This script is used in the `.github/workflows/tlc.yml` GitHub Actions workflow to:
- Verify the TLA+ specification on every push and pull request
- Catch syntax errors and structural issues early
- Provide formal verification that the merge algorithm specification is well-formed

### Note on Model Checking

The WizardMergeSpec is a parametric formal specification that defines constants requiring concrete values for full model checking. This script performs syntax validation and type checking, which is appropriate for CI purposes. Full TLC model checking would require creating test harness modules with specific constant instantiations.

### Requirements

- Python 3.6+
- Java 11+ (for running TLA+ tools)
- Internet connection (for initial download of TLA+ tools)
