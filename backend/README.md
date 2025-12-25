# WizardMerge C++ Backend

This is the C++ backend for WizardMerge implementing the core merge algorithms.

## Build System

- **Build Tool**: Ninja
- **Package Manager**: Conan
- **CMake**: Version 3.15+
- **C++ Standard**: C++17

## Building

### Prerequisites

```sh
# Install Conan
pip install conan

# Install CMake and Ninja
# On Ubuntu/Debian:
sudo apt-get install cmake ninja-build

# On macOS:
brew install cmake ninja
```

### Build Steps

```sh
# Configure with Conan
conan install . --output-folder=build --build=missing

# Build with CMake and Ninja
cd build
cmake .. -G Ninja -DCMAKE_TOOLCHAIN_FILE=conan_toolchain.cmake -DCMAKE_BUILD_TYPE=Release
ninja

# Run the executable
./wizardmerge-cli base.txt ours.txt theirs.txt output.txt
```

## Testing

```sh
# Run tests (if GTest is available)
ninja test
```

## Project Structure

```
backend/
├── CMakeLists.txt       # CMake build configuration
├── conanfile.py         # Conan package definition
├── include/             # Public headers
│   └── wizardmerge/
│       └── merge/
│           └── three_way_merge.h
├── src/                 # Implementation files
│   ├── main.cpp
│   └── merge/
│       └── three_way_merge.cpp
└── tests/               # Unit tests
```

## Features

- Three-way merge algorithm (Phase 1.1 from ROADMAP)
- Conflict detection and marking
- Auto-resolution of common patterns
- Command-line interface

## Usage

```sh
wizardmerge-cli <base> <ours> <theirs> <output>
```

Arguments:
- `base`: Common ancestor version
- `ours`: Current branch version
- `theirs`: Branch being merged
- `output`: Output file for merged result
