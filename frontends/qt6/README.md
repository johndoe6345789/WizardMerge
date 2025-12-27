# WizardMerge Qt6 Frontend

Native desktop frontend for WizardMerge built with Qt6 and C++.

## Features

- Native desktop application for Linux, Windows, and macOS
- Qt6 Widgets/QML-based UI
- Direct integration with C++ backend
- Offline capability
- High performance

## Prerequisites

- Qt6 (6.2+)
- CMake 3.16+
- C++17 compiler (GCC 7+, Clang 6+, MSVC 2017+)
- Ninja (recommended)

## Building

### Install Qt6

**Ubuntu/Debian:**
```bash
sudo apt-get install qt6-base-dev qt6-declarative-dev
```

**macOS (Homebrew):**
```bash
brew install qt@6
```

**Windows:**
Download and install Qt6 from https://www.qt.io/download

### Build the Application

```bash
mkdir build && cd build
cmake .. -G Ninja -DCMAKE_BUILD_TYPE=Release
ninja
```

### Run

```bash
./wizardmerge-qt6
```

## Project Structure

```
qt6/
├── CMakeLists.txt       # CMake build configuration
├── README.md            # This file
├── src/                 # C++ source files
│   └── main.cpp         # Application entry point
├── qml/                 # QML UI files
│   └── main.qml         # Main window UI
└── include/             # Header files
```

## Development

### Architecture

The Qt6 frontend communicates with the WizardMerge C++ backend via:
- Direct library linking (for standalone mode)
- HTTP API calls (for client-server mode)

### UI Components

The UI is built using QML for declarative UI design:
- Three-panel diff viewer
- Conflict resolution controls
- Syntax highlighting
- File navigation

## Configuration

The application can be configured via command-line arguments:

```bash
# Open a specific file
./wizardmerge-qt6 /path/to/conflicted/file

# Connect to remote backend
./wizardmerge-qt6 --backend-url http://localhost:8080

# Use standalone mode (embedded backend)
./wizardmerge-qt6 --standalone
```

## Dependencies

- Qt6 Core
- Qt6 Widgets
- Qt6 Quick (QML)
- Qt6 Network (for HTTP client)

## License

See [LICENSE](../../LICENSE) for details.
