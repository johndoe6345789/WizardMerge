# WizardMerge

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
  - HTTP API endpoints
  - GitHub Pull Request integration
  - Pull request conflict resolution

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

## Pull Request Conflict Resolution

WizardMerge can automatically resolve conflicts in GitHub pull requests using advanced merge algorithms.

### Using the CLI

```sh
# Resolve conflicts in a pull request
./wizardmerge-cli-frontend pr-resolve --url https://github.com/owner/repo/pull/123

# With GitHub token for private repos
./wizardmerge-cli-frontend pr-resolve --url https://github.com/owner/repo/pull/123 --token ghp_xxx

# Or use environment variable
export GITHUB_TOKEN=ghp_xxx
./wizardmerge-cli-frontend pr-resolve --url https://github.com/owner/repo/pull/123
```

### Using the HTTP API

```sh
# POST /api/pr/resolve
curl -X POST http://localhost:8080/api/pr/resolve \
  -H "Content-Type: application/json" \
  -d '{
    "pr_url": "https://github.com/owner/repo/pull/123",
    "github_token": "ghp_xxx",
    "create_branch": true,
    "branch_name": "wizardmerge-resolved-pr-123"
  }'
```

The API will:
1. Parse the PR URL and fetch PR metadata from GitHub
2. Retrieve base and head versions of all modified files
3. Apply the three-way merge algorithm to each file
4. Auto-resolve conflicts using heuristics
5. Return merged content with conflict status

## Research Foundation

WizardMerge is based on research from The University of Hong Kong achieving:
- 28.85% reduction in conflict resolution time
- Merge suggestions for over 70% of code blocks affected by conflicts
- Dependency analysis at text and LLVM-IR levels

See [docs/PAPER.md](docs/PAPER.md) for the complete research paper.
