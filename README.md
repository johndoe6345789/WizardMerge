# WizardMerge

[![Gated Tree CI/CD](https://github.com/johndoe6345789/WizardMerge/actions/workflows/ci.yml/badge.svg)](https://github.com/johndoe6345789/WizardMerge/actions/workflows/ci.yml)
[![TLC Verification](https://github.com/johndoe6345789/WizardMerge/actions/workflows/tlc.yml/badge.svg)](https://github.com/johndoe6345789/WizardMerge/actions/workflows/tlc.yml)

**Intelligent Merge Conflict Resolution**

SEE ALSO: https://github.com/JohnDoe6345789/mergebot

WizardMerge is a powerful tool for resolving merge conflicts using intelligent algorithms based on research from The University of Hong Kong. It combines dependency analysis at both text and LLVM-IR levels to provide smart merge suggestions.

## Architecture

WizardMerge uses a multi-frontend architecture with a high-performance C++ backend and multiple frontend options:

### Backend (C++)
- **Location**: `backend/`
- **Build System**: CMake + Ninja
- **Package Manager**: Conan
- **Web Framework**: Drogon
- **Features**: 
  - Three-way merge algorithm
  - Conflict detection and auto-resolution
  - Context-aware analysis (TypeScript, Python, C++, Java)
  - Intelligent risk assessment
  - HTTP API endpoints
  - GitHub Pull Request integration
  - Pull request conflict resolution
  - TypeScript-specific merge support

### Frontends

WizardMerge provides three frontend options to suit different workflows:

#### Qt6 Native Desktop (C++)
- **Location**: `frontends/qt6/`
- **Framework**: Qt6 with QML
- **Features**: Native desktop application, offline capability, high performance
- **Platforms**: Linux, Windows, macOS

#### Next.js Web UI (TypeScript)
- **Location**: `frontends/nextjs/`
- **Runtime**: bun
- **Framework**: Next.js 14
- **Features**: Web-based UI, real-time collaboration, cross-platform access

#### CLI (C++)
- **Location**: `frontends/cli/`
- **Features**: Command-line interface, automation support, scripting integration
- **Use Cases**: Batch processing, CI/CD pipelines, terminal workflows

## Roadmap
See [ROADMAP.md](ROADMAP.md) for our vision and development plan. The roadmap covers:
- Enhanced merge algorithms (three-way merge, conflict detection) ✓
- Smart semantic merging for different file types
- Advanced visualization and UI improvements
- Git workflow integration
- AI-assisted conflict resolution

## Getting Started

### C++ Backend

```sh
cd backend
./build.sh
./build/wizardmerge-cli
```

The backend server will start on port 8080. See [backend/README.md](backend/README.md) for details.

### Frontends

Choose the frontend that best fits your workflow:

#### Qt6 Desktop Application

```sh
cd frontends/qt6
mkdir build && cd build
cmake .. -G Ninja
ninja
./wizardmerge-qt6
```

See [frontends/qt6/README.md](frontends/qt6/README.md) for details.

#### Next.js Web UI

```sh
cd frontends/nextjs
bun install
bun run dev
```

Visit http://localhost:3000. See [frontends/nextjs/README.md](frontends/nextjs/README.md) for details.

#### CLI

```sh
cd frontends/cli
mkdir build && cd build
cmake .. -G Ninja
ninja
./wizardmerge-cli-frontend --help
```

See [frontends/cli/README.md](frontends/cli/README.md) for details.

## Pull Request / Merge Request Conflict Resolution

WizardMerge can automatically resolve conflicts in GitHub pull requests and GitLab merge requests using advanced merge algorithms.

### Supported Platforms

- **GitHub**: Pull requests via GitHub API
- **GitLab**: Merge requests via GitLab API

### Using the CLI

```sh
# Resolve conflicts in a GitHub pull request
./wizardmerge-cli-frontend pr-resolve --url https://github.com/owner/repo/pull/123

# Resolve conflicts in a GitLab merge request
./wizardmerge-cli-frontend pr-resolve --url https://gitlab.com/owner/repo/-/merge_requests/456

# With API token for private repos
./wizardmerge-cli-frontend pr-resolve --url https://github.com/owner/repo/pull/123 --token ghp_xxx
./wizardmerge-cli-frontend pr-resolve --url https://gitlab.com/owner/repo/-/merge_requests/456 --token glpat-xxx

# Or use environment variable
export GITHUB_TOKEN=ghp_xxx  # For GitHub
export GITLAB_TOKEN=glpat-xxx  # For GitLab
./wizardmerge-cli-frontend pr-resolve --url <pr_or_mr_url>
```

### Using the HTTP API

```sh
# POST /api/pr/resolve - GitHub
curl -X POST http://localhost:8080/api/pr/resolve \
  -H "Content-Type: application/json" \
  -d '{
    "pr_url": "https://github.com/owner/repo/pull/123",
    "api_token": "ghp_xxx"
  }'

# POST /api/pr/resolve - GitLab
curl -X POST http://localhost:8080/api/pr/resolve \
  -H "Content-Type: application/json" \
  -d '{
    "pr_url": "https://gitlab.com/owner/repo/-/merge_requests/456",
    "api_token": "glpat-xxx"
  }'

# With branch creation (requires Git CLI)
curl -X POST http://localhost:8080/api/pr/resolve \
  -H "Content-Type: application/json" \
  -d '{
    "pr_url": "https://github.com/owner/repo/pull/123",
    "api_token": "ghp_xxx",
    "create_branch": true,
    "branch_name": "wizardmerge-resolved-pr-123"
  }'
```

The API will:
1. Parse the PR/MR URL and detect the platform (GitHub or GitLab)
2. Fetch PR/MR metadata using the platform-specific API
3. Retrieve base and head versions of all modified files
4. Apply the three-way merge algorithm to each file
5. Auto-resolve conflicts using heuristics
6. Optionally create a new branch with resolved changes (if `create_branch: true` and Git CLI available)
7. Return merged content with conflict status

### Git CLI Integration

WizardMerge includes Git CLI integration for advanced workflows:

**Features:**
- Clone repositories locally
- Create and checkout branches
- Stage and commit resolved changes
- Push branches to remote repositories

**Branch Creation Workflow:**

When `create_branch: true` is set in the API request:
1. Repository is cloned to a temporary directory
2. New branch is created from the PR base branch
3. Resolved files are written to the working directory
4. Changes are staged and committed
5. Branch path is returned in the response

**Requirements:**
- Git CLI must be installed and available in system PATH
- For pushing to remote, Git credentials must be configured (SSH keys or credential helpers)

**Example Response with Branch Creation:**
```json
{
  "success": true,
  "branch_created": true,
  "branch_name": "wizardmerge-resolved-pr-123",
  "branch_path": "/tmp/wizardmerge_pr_123_1234567890",
  "note": "Branch created successfully. Push to remote with: git -C /tmp/wizardmerge_pr_123_1234567890 push origin wizardmerge-resolved-pr-123",
  ...
}
```

### Authentication

- **GitHub**: Use personal access tokens with `repo` scope
- **GitLab**: Use personal access tokens with `read_api` and `read_repository` scopes
- Tokens can be passed via `--token` flag or environment variables (`GITHUB_TOKEN`, `GITLAB_TOKEN`)

## TypeScript Support

WizardMerge includes comprehensive TypeScript support for intelligent merge conflict resolution:

### Features

- **Context-Aware Analysis**: Recognizes TypeScript interfaces, types, enums, and arrow functions
- **Risk Assessment**: Detects breaking changes in type definitions and type safety bypasses
- **Package Lock Handling**: Smart detection of package-lock.json, yarn.lock, pnpm-lock.yaml, and bun.lockb files
- **Security Patterns**: Identifies XSS risks (dangerouslySetInnerHTML, innerHTML) and type safety issues (as any, @ts-ignore)

### Supported TypeScript Patterns

- Interfaces, type aliases, and enums
- Arrow functions and async functions  
- TypeScript imports (import type, namespace imports)
- Function signatures with type annotations
- Export declarations

### Package Lock Conflicts

Package lock files are extremely common sources of merge conflicts. WizardMerge:
- Automatically detects package lock files
- Suggests regenerating lock files instead of manual merging
- Supports npm, Yarn, pnpm, and Bun lock files

**Recommended workflow for lock file conflicts:**
1. Merge `package.json` manually
2. Delete the lock file
3. Run your package manager to regenerate
4. This ensures consistency and avoids corruption

See [docs/TYPESCRIPT_SUPPORT.md](docs/TYPESCRIPT_SUPPORT.md) for detailed documentation and API examples.

## Formal Verification

WizardMerge includes a formal TLA+ specification that is verified in CI:
- **Specification**: [spec/WizardMergeSpec.tla](spec/WizardMergeSpec.tla)
- **CI Workflow**: `.github/workflows/tlc.yml`
- **Verification Script**: `scripts/tlaplus.py`

The specification is automatically checked on every push to ensure:
- Syntax correctness
- Module structure validity  
- Type checking of invariants and temporal properties

See [scripts/README.md](scripts/README.md) for details on running the verification locally.

## Research Foundation

WizardMerge is based on research from The University of Hong Kong achieving:
- 28.85% reduction in conflict resolution time
- Merge suggestions for over 70% of code blocks affected by conflicts
- Dependency analysis at text and LLVM-IR levels

See [docs/PAPER.md](docs/PAPER.md) for the complete research paper.
