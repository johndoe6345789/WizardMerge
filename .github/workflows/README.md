# WizardMerge CI/CD Workflows

This directory contains GitHub Actions workflows for continuous integration and deployment.

## Workflows

### 1. Gated Tree CI/CD (`ci.yml`)

A comprehensive multi-stage pipeline with quality gates at each level.

#### Workflow Structure

```
Gate 1: Code Quality & Linting
├── lint-cpp (C++ formatting)
├── lint-typescript (TypeScript linting)
└── lint-python (Python linting)
        ↓
Gate 2: Build Components
├── build-backend (C++ backend) ← depends on lint-cpp, lint-python
├── build-cli (CLI frontend) ← depends on lint-cpp
├── build-qt6 (Qt6 frontend) ← depends on lint-cpp
└── build-nextjs (Next.js frontend) ← depends on lint-typescript
        ↓
Gate 3: Testing
├── test-backend ← depends on build-backend
└── test-tlaplus ← depends on lint-python
        ↓
Gate 4: Security Scanning
└── security-codeql ← depends on test-backend, test-tlaplus
        ↓
Gate 5: Integration Tests
└── integration-tests ← depends on security-codeql
        ↓
Gate 6: Deployment Gate (main branch only)
├── deployment-ready ← depends on integration-tests
└── publish-results ← depends on integration-tests
```

#### Gates Explained

**Gate 1: Code Quality**
- Ensures code follows formatting and style guidelines
- Runs linters for C++, TypeScript, and Python
- Fast feedback on code quality issues

**Gate 2: Build Components**
- Builds all project components (backend, frontends)
- Verifies that code compiles successfully
- Only runs if linting passes
- Produces build artifacts for testing

**Gate 3: Testing**
- Runs unit tests for backend
- Verifies TLA+ formal specification
- Only runs if builds succeed

**Gate 4: Security Scanning**
- CodeQL analysis for security vulnerabilities
- Scans C++, Python, and JavaScript code
- Only runs if tests pass

**Gate 5: Integration Tests**
- Tests API endpoints
- Verifies component interaction
- Only runs if security scan completes

**Gate 6: Deployment Gate**
- Final gate before deployment
- Only runs on main branch
- Publishes test results to ci/test-results branch

#### Triggering the Workflow

The workflow runs on:
- Push to `main` or `develop` branches
- Pull requests targeting `main` or `develop`

#### Artifacts

The workflow produces the following artifacts:
- `backend-build`: Compiled backend binary
- `cli-build`: Compiled CLI frontend binary
- `qt6-build`: Compiled Qt6 frontend binary
- `nextjs-build`: Built Next.js application
- `tlc-results`: TLA+ verification results

Artifacts are retained for 1 day (except TLC results: 7 days).

#### Branch Protection

For a complete gated workflow, configure branch protection on `main`:

1. Go to Settings → Branches → Add rule
2. Branch name pattern: `main`
3. Enable "Require status checks to pass before merging"
4. Select required checks:
   - `lint-cpp`
   - `lint-typescript`
   - `lint-python`
   - `build-backend`
   - `build-cli`
   - `build-qt6`
   - `build-nextjs`
   - `test-backend`
   - `test-tlaplus`
   - `security-codeql`
   - `integration-tests`

#### Local Testing

Before pushing, you can run checks locally:

**C++ Formatting:**
```bash
find backend frontends/qt6 frontends/cli -name "*.cpp" -o -name "*.h" -o -name "*.hpp" | \
  xargs clang-format -i
```

**TypeScript Linting:**
```bash
cd frontends/nextjs
bun run tsc --noEmit
```

**Python Linting:**
```bash
pip install ruff
ruff check scripts/
```

**Backend Build:**
```bash
cd backend
./build.sh
```

**Run Tests:**
```bash
cd backend/build
./wizardmerge_tests
```

**TLA+ Verification:**
```bash
python3 scripts/tlaplus.py run
```

### 2. TLA+ Verification (`tlc.yml`)

Legacy workflow for TLA+ specification verification. This is now integrated into the main gated workflow but kept for compatibility.

## Best Practices

1. **Small, focused commits**: Easier to pass gates
2. **Run linters locally**: Catch issues before pushing
3. **Fix one gate at a time**: Don't move to next gate if current fails
4. **Monitor workflow runs**: Check Actions tab for failures
5. **Read security reports**: Address CodeQL findings promptly

## Workflow Philosophy

The gated tree approach ensures:
- **Quality**: Code is checked at multiple levels
- **Security**: Security scanning is mandatory
- **Reliability**: Only tested code reaches production
- **Fast feedback**: Early gates fail fast
- **Confidence**: All gates pass = deployment ready

## Troubleshooting

**Linting fails:**
- Run formatters locally and commit changes

**Build fails:**
- Check dependency installation
- Verify CMake configuration
- Review compiler errors

**Tests fail:**
- Run tests locally to reproduce
- Check test logs in Actions tab
- Fix failing tests before proceeding

**Security scan finds issues:**
- Review CodeQL findings
- Address high-severity issues first
- Update dependencies if needed

**Integration tests fail:**
- Check if backend starts correctly
- Verify API endpoints
- Review server logs

## Future Enhancements

Potential additions to the workflow:
- [ ] Performance benchmarking gate
- [ ] Docker image building and publishing
- [ ] Staging environment deployment
- [ ] Production deployment with manual approval
- [ ] Notification on gate failures
- [ ] Automatic dependency updates
- [ ] Code coverage reporting
- [ ] Documentation generation and deployment
