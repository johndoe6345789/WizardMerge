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
- **Features**: Three-way merge algorithm, conflict detection, auto-resolution, HTTP API

### Frontend (TypeScript/Next.js)
- **Location**: `frontend/`
- **Runtime**: bun
- **Framework**: Next.js 14
- **Features**: Web-based UI for conflict resolution

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
```

See [backend/README.md](backend/README.md) for details.

### TypeScript Frontend

```sh
cd frontend
bun install
bun run dev
```

See [frontend/README.md](frontend/README.md) for details.

## Research Foundation

WizardMerge is based on research from The University of Hong Kong achieving:
- 28.85% reduction in conflict resolution time
- Merge suggestions for over 70% of code blocks affected by conflicts
- Dependency analysis at text and LLVM-IR levels

See [docs/PAPER.md](docs/PAPER.md) for the complete research paper.
