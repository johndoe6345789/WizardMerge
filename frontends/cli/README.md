# WizardMerge CLI Frontend

Command-line interface for WizardMerge merge conflict resolution.

## Features

- Simple command-line interface
- Communicates with WizardMerge backend via HTTP API
- Suitable for automation and scripting
- Cross-platform (Linux, Windows, macOS)
- Non-interactive batch processing

## Prerequisites

- C++17 compiler (GCC 7+, Clang 6+, MSVC 2017+)
- CMake 3.15+
- libcurl (for HTTP client)

## Building

### Install Dependencies

**Ubuntu/Debian:**
```bash
sudo apt-get install libcurl4-openssl-dev
```

**macOS (Homebrew):**
```bash
brew install curl
```

**Windows:**
libcurl is typically included with MSVC or can be installed via vcpkg.

### Build the Application

```bash
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make
```

Or using Ninja:
```bash
mkdir build && cd build
cmake .. -G Ninja -DCMAKE_BUILD_TYPE=Release
ninja
```

### Run

```bash
./wizardmerge-cli --help
```

## Usage

### Basic Three-Way Merge

```bash
wizardmerge-cli merge --base base.txt --ours ours.txt --theirs theirs.txt -o result.txt
```

### Merge with Backend Server

```bash
# Use default backend (http://localhost:8080)
wizardmerge-cli merge --base base.txt --ours ours.txt --theirs theirs.txt

# Specify custom backend URL
wizardmerge-cli --backend http://remote-server:8080 merge --base base.txt --ours ours.txt --theirs theirs.txt
```

### Git Integration

```bash
# Resolve conflicts in a Git repository
cd /path/to/git/repo
wizardmerge-cli git-resolve

# Resolve a specific file
wizardmerge-cli git-resolve path/to/conflicted/file.txt
```

### Batch Processing

```bash
# Process all conflicted files in current directory
wizardmerge-cli batch-resolve .
```

## Command Reference

### Global Options

- `--backend <url>` - Backend server URL (default: http://localhost:8080)
- `--verbose, -v` - Enable verbose output
- `--quiet, -q` - Suppress non-error output
- `--help, -h` - Show help message
- `--version` - Show version information

### Commands

#### merge

Perform a three-way merge operation.

```bash
wizardmerge-cli merge [OPTIONS]
```

Options:
- `--base <file>` - Path to base version (required)
- `--ours <file>` - Path to our version (required)
- `--theirs <file>` - Path to their version (required)
- `-o, --output <file>` - Output file path (default: stdout)
- `--format <format>` - Output format: text, json (default: text)

#### git-resolve

Resolve Git merge conflicts.

```bash
wizardmerge-cli git-resolve [FILE]
```

Arguments:
- `FILE` - Specific file to resolve (optional, resolves all if omitted)

#### batch-resolve

Batch process multiple files.

```bash
wizardmerge-cli batch-resolve [DIRECTORY]
```

Arguments:
- `DIRECTORY` - Directory to scan for conflicts (default: current directory)

Options:
- `--recursive, -r` - Process directories recursively
- `--pattern <pattern>` - File pattern to match (default: *)

## Examples

### Example 1: Simple Merge

```bash
# Create test files
echo -e "line1\nline2\nline3" > base.txt
echo -e "line1\nline2-ours\nline3" > ours.txt
echo -e "line1\nline2-theirs\nline3" > theirs.txt

# Perform merge
wizardmerge-cli merge --base base.txt --ours ours.txt --theirs theirs.txt
```

### Example 2: JSON Output

```bash
wizardmerge-cli merge --base base.txt --ours ours.txt --theirs theirs.txt --format json > result.json
```

### Example 3: Git Workflow

```bash
# In a Git repository with conflicts
git merge feature-branch
# Conflicts occur...

# Resolve using WizardMerge
wizardmerge-cli git-resolve

# Review and commit
git commit
```

## Exit Codes

- `0` - Success (no conflicts or all conflicts resolved)
- `1` - General error
- `2` - Invalid arguments
- `3` - Backend connection error
- `4` - File I/O error
- `5` - Merge conflicts detected (when running in strict mode)

## Configuration

Configuration can be provided via:

1. Command-line arguments (highest priority)
2. Environment variables:
   - `WIZARDMERGE_BACKEND` - Backend server URL
   - `WIZARDMERGE_VERBOSE` - Enable verbose output (1/0)
3. Configuration file `~/.wizardmergerc` (lowest priority)

### Configuration File Format

```ini
[backend]
url = http://localhost:8080

[cli]
verbose = false
format = text
```

## Project Structure

```
cli/
├── CMakeLists.txt       # CMake build configuration
├── README.md            # This file
├── src/                 # C++ source files
│   ├── main.cpp         # Application entry point
│   ├── http_client.cpp  # HTTP client implementation
│   └── file_utils.cpp   # File handling utilities
└── include/             # Header files
    ├── http_client.h
    └── file_utils.h
```

## Development

### Architecture

The CLI frontend is a thin client that:
1. Parses command-line arguments
2. Reads input files
3. Sends HTTP requests to backend
4. Formats and displays results

### Current Limitations

**JSON Handling (Prototype Implementation)**:
- The current implementation uses simple string-based JSON serialization/parsing
- Does NOT escape special characters (quotes, backslashes, newlines, etc.)
- Will fail on file content with complex characters
- Suitable for simple text files and prototyping only

**Production Readiness**:
For production use, the JSON handling should be replaced with a proper library:
- Option 1: [nlohmann/json](https://github.com/nlohmann/json) - Header-only, modern C++
- Option 2: [RapidJSON](https://github.com/Tencent/rapidjson) - Fast and lightweight
- Option 3: [jsoncpp](https://github.com/open-source-parsers/jsoncpp) - Mature and stable

See `src/http_client.cpp` for TODO comments marking areas needing improvement.

### Dependencies

- Standard C++ library
- libcurl (for HTTP client)
- POSIX API (for file operations)

### Adding New Commands

1. Add command handler in `src/main.cpp`
2. Implement command logic
3. Update help text and README
4. Add tests

## Troubleshooting

### Backend Connection Failed

```bash
# Check backend is running
curl http://localhost:8080/api/health

# Start backend if needed
cd ../../backend
./build/wizardmerge-cli
```

### File Not Found

Ensure file paths are correct and files are readable:
```bash
ls -la base.txt ours.txt theirs.txt
```

### Permission Denied

Check file permissions:
```bash
chmod +r base.txt ours.txt theirs.txt
```

## License

See [LICENSE](../../LICENSE) for details.
