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

**Required:**
- C++17 compiler (GCC 7+, Clang 6+, MSVC 2017+)
- CMake 3.15+
- Ninja build tool

**For HTTP Server:**
- Drogon framework (see installation methods below)

### Installation Methods

#### Method 1: Using Installer Script (Recommended)

```sh
# Install Drogon from source
./install_drogon.sh

# Build WizardMerge
./build.sh
```

#### Method 2: Using Docker (Easiest)

```sh
# Build and run with Docker Compose
docker-compose up --build

# Or use Docker directly
docker build -t wizardmerge-backend .
docker run -p 8080:8080 wizardmerge-backend
```

#### Method 3: Using Conan

```sh
# Install Conan
pip install conan

# Build with Conan
./build.sh
```

Note: Conan requires internet access to download Drogon.

#### Method 4: Manual CMake Build

If you have Drogon already installed system-wide:

```sh
mkdir build && cd build
cmake .. -G Ninja -DCMAKE_BUILD_TYPE=Release
ninja
```

### Build Steps

The build script automatically handles dependencies and provides multiple build options:

```sh
# Automatic build (tries Conan, falls back to direct CMake)
./build.sh
```

If Drogon is not found, the library will still build but the HTTP server will be skipped.

### Running Without Drogon

If you only need the merge library (not the HTTP server):

```sh
mkdir build && cd build
cmake .. -G Ninja
ninja
# This builds libwizardmerge.a which can be linked into other applications
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
- GitHub Pull Request integration (Phase 1.2)
- Pull request conflict resolution via API

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

### POST /api/pr/resolve

Resolve conflicts in a GitHub or GitLab pull/merge request.

**Request:**
```json
{
  "pr_url": "https://github.com/owner/repo/pull/123",
  "api_token": "ghp_xxx",
  "create_branch": true,
  "branch_name": "wizardmerge-resolved-pr-123"
}
```

**Request Fields:**
- `pr_url` (required): Pull/merge request URL (GitHub or GitLab)
- `api_token` (optional): API token for authentication (GitHub: `ghp_*`, GitLab: `glpat-*`)
- `create_branch` (optional, default: false): Create a new branch with resolved changes
- `branch_name` (optional): Custom branch name (auto-generated if not provided)

**Response:**
```json
{
  "success": true,
  "pr_info": {
    "platform": "GitHub",
    "number": 123,
    "title": "Feature: Add new functionality",
    "base_ref": "main",
    "head_ref": "feature-branch",
    "base_sha": "abc123...",
    "head_sha": "def456...",
    "mergeable": false,
    "mergeable_state": "dirty"
  },
  "resolved_files": [
    {
      "filename": "src/example.cpp",
      "status": "modified",
      "had_conflicts": true,
      "auto_resolved": true,
      "merged_content": ["line1", "line2", "..."]
    }
  ],
  "total_files": 5,
  "resolved_count": 4,
  "failed_count": 0,
  "branch_created": true,
  "branch_name": "wizardmerge-resolved-pr-123",
  "branch_path": "/tmp/wizardmerge_pr_123_1234567890",
  "note": "Branch created successfully. Push to remote with: git -C /tmp/wizardmerge_pr_123_1234567890 push origin wizardmerge-resolved-pr-123"
}
```

**Example with curl:**
```sh
# Basic conflict resolution
curl -X POST http://localhost:8080/api/pr/resolve \
  -H "Content-Type: application/json" \
  -d '{
    "pr_url": "https://github.com/owner/repo/pull/123",
    "api_token": "ghp_xxx"
  }'

# With branch creation
curl -X POST http://localhost:8080/api/pr/resolve \
  -H "Content-Type: application/json" \
  -d '{
    "pr_url": "https://github.com/owner/repo/pull/123",
    "api_token": "ghp_xxx",
    "create_branch": true,
    "branch_name": "resolved-conflicts"
  }'
```

**Git CLI Integration:**

When `create_branch: true` is specified:
1. **Clone**: Repository is cloned to `/tmp/wizardmerge_pr_<number>_<timestamp>`
2. **Branch**: New branch is created from the PR base branch
3. **Resolve**: Merged files are written to the working directory
4. **Commit**: Changes are staged and committed with message "Resolve conflicts for PR #<number>"
5. **Response**: Branch path is returned for manual inspection or pushing

**Requirements for Branch Creation:**
- Git CLI must be installed (`git --version` works)
- Sufficient disk space for repository clone
- Write permissions to `/tmp` directory

**Security Note:** Branch is created locally. To push to remote, configure Git credentials separately (SSH keys or credential helpers). Do not embed tokens in Git URLs.

**Note:** Requires libcurl to be installed. The API token is optional for public repositories but required for private ones.

## Deployment

### Production Deployment with Docker

The recommended way to deploy in production:

```sh
# Using Docker Compose
docker-compose up -d

# Check logs
docker-compose logs -f

# Stop the server
docker-compose down
```

### Configuration

Edit `config.json` to customize server settings:

- `listeners[].port`: Change server port (default: 8080)
- `app.threads_num`: Number of worker threads (default: 4)
- `app.log.log_level`: Logging level (DEBUG, INFO, WARN, ERROR)
- `app.client_max_body_size`: Maximum request body size

### Monitoring

Logs are written to `./logs/` directory by default. Monitor with:

```sh
tail -f logs/wizardmerge.log
```

## Development

### Architecture

The backend is now structured as a Drogon HTTP API server:

- **Core Library** (`libwizardmerge.a`): Contains the merge algorithms
- **HTTP Server** (`wizardmerge-cli`): Drogon-based API server
- **Controllers** (`src/controllers/`): HTTP request handlers
- **Configuration** (`config.json`): Server settings

### Adding New Endpoints

1. Create a new controller in `src/controllers/`
2. Implement the controller methods
3. Add the controller source to CMakeLists.txt
4. Rebuild the project

Example controller structure:

```cpp
class MyController : public HttpController<MyController> {
  public:
    METHOD_LIST_BEGIN
    ADD_METHOD_TO(MyController::myMethod, "/api/mypath", Post);
    METHOD_LIST_END

    void myMethod(const HttpRequestPtr &req,
                  std::function<void(const HttpResponsePtr &)> &&callback);
};
```
