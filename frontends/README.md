# WizardMerge Frontends

This directory contains multiple frontend implementations for WizardMerge, each designed for different use cases and workflows.

## Available Frontends

### 1. Qt6 Desktop Frontend (`qt6/`)

**Type**: Native desktop application  
**Language**: C++ with Qt6 and QML  
**Platforms**: Linux, Windows, macOS

A native desktop application providing the best performance and integration with desktop environments.

**Features**:
- Native window management and desktop integration
- Offline capability with embedded backend option
- High-performance rendering
- Three-panel diff viewer with QML-based UI
- Keyboard shortcuts and native file dialogs

**Best for**: Desktop users who want a fast, native application with full offline support.

See [qt6/README.md](qt6/README.md) for build and usage instructions.

### 2. Next.js Web Frontend (`nextjs/`)

**Type**: Web application  
**Language**: TypeScript with React/Next.js  
**Runtime**: bun

A modern web-based interface accessible from any browser.

**Features**:
- Cross-platform browser access
- No installation required
- Real-time collaboration (planned)
- Responsive design
- Easy deployment and updates

**Best for**: Teams needing shared access, cloud deployments, or users who prefer web interfaces.

See [nextjs/README.md](nextjs/README.md) for development and deployment instructions.

### 3. CLI Frontend (`cli/`)

**Type**: Command-line interface  
**Language**: C++  
**Platforms**: Linux, Windows, macOS

A command-line tool for automation and scripting.

**Features**:
- Non-interactive batch processing
- Scriptable and automatable
- CI/CD pipeline integration
- Git workflow integration
- Minimal dependencies

**Best for**: Automation, scripting, CI/CD pipelines, and terminal-based workflows.

See [cli/README.md](cli/README.md) for usage and examples.

## Architecture

All frontends communicate with the WizardMerge C++ backend through a common HTTP API:

```
┌─────────────────────────────────────────────────┐
│                   Frontends                     │
│  ┌────────────┐  ┌────────────┐  ┌──────────┐ │
│  │ Qt6 Native │  │ Next.js    │  │   CLI    │ │
│  │   (C++)    │  │(TypeScript)│  │  (C++)   │ │
│  └─────┬──────┘  └──────┬─────┘  └────┬─────┘ │
└────────┼─────────────────┼─────────────┼───────┘
         │                 │             │
         └─────────────────┼─────────────┘
                           │
                    HTTP REST API
                           │
         ┌─────────────────▼──────────────────┐
         │    WizardMerge C++ Backend         │
         │    (Drogon HTTP Server)            │
         │                                    │
         │  - Three-way merge algorithm       │
         │  - Conflict detection              │
         │  - Auto-resolution                 │
         │  - Semantic analysis               │
         └────────────────────────────────────┘
```

### Backend API

The backend provides a REST API on port 8080 (configurable):

- `POST /api/merge` - Perform three-way merge

All frontends use this common API, ensuring consistent merge behavior regardless of the interface used.

## Choosing a Frontend

| Feature | Qt6 | Next.js | CLI |
|---------|-----|---------|-----|
| Native Performance | ✓ | - | ✓ |
| Offline Support | ✓ | - | ✓ |
| Web Browser Access | - | ✓ | - |
| Collaboration | - | ✓ (planned) | - |
| Automation/Scripting | - | - | ✓ |
| Visual UI | ✓ | ✓ | - |
| Installation Required | ✓ | - | ✓ |
| Platform Support | All | All | All |

## Building All Frontends

### Prerequisites

Install dependencies for all frontends you want to build:

```bash
# Qt6 (for qt6 frontend)
# Ubuntu/Debian:
sudo apt-get install qt6-base-dev qt6-declarative-dev

# macOS:
brew install qt@6

# Next.js (for nextjs frontend)
curl -fsSL https://bun.sh/install | bash

# CLI (for cli frontend)
# Ubuntu/Debian:
sudo apt-get install libcurl4-openssl-dev

# macOS:
brew install curl
```

### Build All

```bash
# Build Qt6 frontend
cd qt6
mkdir build && cd build
cmake .. -G Ninja
ninja
cd ../..

# Build/setup Next.js frontend
cd nextjs
bun install
bun run build
cd ..

# Build CLI frontend
cd cli
mkdir build && cd build
cmake .. -G Ninja
ninja
cd ../..
```

## Development Guidelines

When developing a frontend:

1. **Consistency**: Maintain consistent UX across all frontends where applicable
2. **API Usage**: Use the common backend API for all merge operations
3. **Error Handling**: Properly handle backend connection errors and API failures
4. **Documentation**: Update frontend-specific README files
5. **Testing**: Add tests for new features

### Adding a New Frontend

To add a new frontend implementation:

1. Create a new directory under `frontends/`
2. Implement the UI using your chosen technology
3. Use the backend HTTP API (`POST /api/merge`)
4. Add a README.md with build and usage instructions
5. Update this file to list the new frontend

## License

See [../LICENSE](../LICENSE) for details.
