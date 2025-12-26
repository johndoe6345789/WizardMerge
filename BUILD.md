# WizardMerge Build and Development Guide

This guide provides instructions for building and developing WizardMerge across all components.

## Architecture Overview

WizardMerge uses a multi-component architecture:

```
WizardMerge/
├── backend/          # C++ core merge engine (Conan + Ninja)
├── frontends/        # Multiple frontend options
│   ├── qt6/          # Qt6 native desktop (C++)
│   ├── nextjs/       # Next.js web UI (TypeScript/bun)
│   └── cli/          # Command-line interface (C++)
├── spec/             # TLA+ formal specification
├── docs/             # Research paper and documentation
└── ROADMAP.md        # Development roadmap
```

## Quick Start

### C++ Backend

The backend implements the core three-way merge algorithm with an HTTP API server using Drogon.

**Prerequisites:**
- C++17 compiler (GCC 7+, Clang 6+, MSVC 2017+)
- CMake 3.15+
- Ninja build tool
- Conan package manager

**Build:**
```bash
cd backend
./build.sh
```

**Run the server:**
```bash
./build/wizardmerge-cli
```

The HTTP server will start on port 8080. Use the POST /api/merge endpoint to perform merges.

See [backend/README.md](backend/README.md) for details.

### TypeScript Frontend

The frontend provides a web-based UI for conflict resolution.

**Prerequisites:**
- bun (JavaScript runtime and package manager)

**Setup:**
```bash
cd frontends/nextjs
bun install
```

**Development:**
```bash
bun run dev
```

Visit http://localhost:3000

See [frontends/nextjs/README.md](frontends/nextjs/README.md) for details.

### Qt6 Desktop Frontend

The Qt6 frontend provides a native desktop application.

**Prerequisites:**
- Qt6 (6.2+)
- CMake 3.16+
- C++17 compiler

**Setup:**
```bash
cd frontends/qt6
mkdir build && cd build
cmake .. -G Ninja
ninja
```

**Run:**
```bash
./wizardmerge-qt6
```

See [frontends/qt6/README.md](frontends/qt6/README.md) for details.

### CLI Frontend

The CLI frontend provides a command-line interface.

**Prerequisites:**
- C++17 compiler
- CMake 3.15+
- libcurl

**Setup:**
```bash
cd frontends/cli
mkdir build && cd build
cmake .. -G Ninja
ninja
```

**Run:**
```bash
./wizardmerge-cli-frontend --help
```

See [frontends/cli/README.md](frontends/cli/README.md) for details.

## Development Workflow

### Making Changes

1. **Backend (C++)**: Edit files in `backend/src/` and `backend/include/`
2. **Qt6 Frontend (C++)**: Edit files in `frontends/qt6/src/` and `frontends/qt6/qml/`
3. **Next.js Frontend (TypeScript)**: Edit files in `frontends/nextjs/app/`
4. **CLI Frontend (C++)**: Edit files in `frontends/cli/src/`
5. **Tests**: Add tests in `backend/tests/` for C++ changes
6. **Documentation**: Update relevant README files

### Building

```bash
# C++ backend
cd backend && ./build.sh

# Qt6 desktop frontend
cd frontends/qt6 && mkdir build && cd build && cmake .. -G Ninja && ninja

# Next.js web frontend
cd frontends/nextjs && bun run build

# CLI frontend
cd frontends/cli && mkdir build && cd build && cmake .. -G Ninja && ninja
```

### Testing

```bash
# C++ backend tests (requires GTest)
cd backend/build && ninja test

# Next.js frontend tests
cd frontends/nextjs && bun test
```

## Project Standards

### C++ (Backend)
- **Standard**: C++17
- **Build System**: CMake + Ninja
- **Package Manager**: Conan
- **Coding Style**: Follow backend code conventions
- **Documentation**: Doxygen-style comments

### TypeScript (Frontend)
- **Runtime**: bun
- **Framework**: Next.js 14
- **Language**: TypeScript with strict mode
- **Coding Style**: Follow frontend code conventions
- **Package Manager**: bun

## Roadmap Progress

Track development progress in [ROADMAP.md](ROADMAP.md).

**Phase 1 (Foundation):** ✓ In Progress
- [x] Three-way merge algorithm
- [x] Conflict detection
- [x] Auto-resolution patterns
- [ ] Git integration
- [ ] File I/O module

**Phase 2 (Intelligence):** Planned
- Semantic merge
- Enhanced visualization
- Code intelligence

**Phase 3 (Advanced):** Future
- AI-assisted merging
- Plugin ecosystem
- Performance optimizations

## Contributing

1. Check [ROADMAP.md](ROADMAP.md) for planned features
2. Create a branch for your changes
3. Write tests for new functionality
4. Update documentation
5. Submit a pull request

## Research Foundation

WizardMerge is based on research from The University of Hong Kong:
- 28.85% reduction in conflict resolution time
- Merge suggestions for 70%+ of conflict blocks
- Dependency analysis at text and LLVM-IR levels

See [docs/PAPER.md](docs/PAPER.md) for the complete research paper.

## Formal Specification

The merge algorithm is formally specified in TLA+:
- [spec/WizardMergeSpec.tla](spec/WizardMergeSpec.tla)

The specification defines the dependency-aware merge logic including:
- Vertex and edge classification
- Safe vs. violated edges
- Conflict detection rules

## License

See [LICENSE](LICENSE) for details.

## Related Projects

- [mergebot](https://github.com/JohnDoe6345789/mergebot) - Companion automation tool
