# WizardMerge C++ Backend

This is the C++ backend for WizardMerge implementing the core merge algorithms with a Drogon-based HTTP API.

## Build System

- **Build Tool**: Ninja
- **Package Manager**: Conan
- **CMake**: Version 3.15+
- **C++ Standard**: C++17
- **Web Framework**: Drogon

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

# Run the HTTP server
./wizardmerge-cli
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
├── config.json          # Drogon server configuration
├── include/             # Public headers
│   └── wizardmerge/
│       └── merge/
│           └── three_way_merge.h
├── src/                 # Implementation files
│   ├── main.cpp
│   ├── controllers/
│   │   ├── MergeController.h
│   │   └── MergeController.cc
│   └── merge/
│       └── three_way_merge.cpp
└── tests/               # Unit tests
```

## Features

- Three-way merge algorithm (Phase 1.1 from ROADMAP)
- Conflict detection and marking
- Auto-resolution of common patterns
- HTTP API server using Drogon framework
- JSON-based request/response

## API Usage

### Start the server

```sh
./wizardmerge-cli [config.json]
```

The server will start on port 8080 by default (configurable in config.json).

### POST /api/merge

Perform a three-way merge operation.

**Request:**
```json
{
  "base": ["line1", "line2", "line3"],
  "ours": ["line1", "line2_modified", "line3"],
  "theirs": ["line1", "line2", "line3_modified"]
}
```

**Response:**
```json
{
  "merged": ["line1", "line2_modified", "line3_modified"],
  "conflicts": [],
  "has_conflicts": false
}
```

**Example with curl:**
```sh
curl -X POST http://localhost:8080/api/merge \
  -H "Content-Type: application/json" \
  -d '{
    "base": ["line1", "line2", "line3"],
    "ours": ["line1", "line2_ours", "line3"],
    "theirs": ["line1", "line2_theirs", "line3"]
  }'
```
