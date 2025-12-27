# Pull Request URL Support - Implementation Summary

## Overview

This implementation adds the ability for WizardMerge to accept GitHub Pull Request URLs and automatically resolve merge conflicts using the existing three-way merge algorithm with advanced heuristics.

## Key Features

### 1. GitHub API Integration
- **PR URL Parsing**: Extracts owner, repository, and PR number from GitHub URLs
- **Metadata Fetching**: Retrieves PR information including base/head refs and commit SHAs
- **File Content Retrieval**: Fetches file versions at specific commits with base64 decoding

### 2. HTTP API Endpoint
- **Endpoint**: `POST /api/pr/resolve`
- **Input**: PR URL, optional GitHub token, branch creation flags
- **Output**: Detailed resolution results with file-by-file conflict status

### 3. CLI Integration
- **Command**: `pr-resolve --url <pr_url>`
- **Environment**: Supports `GITHUB_TOKEN` environment variable
- **Options**: `--token`, `--branch`, `--output`

### 4. Resolution Algorithm
For each modified file in the PR:
1. Fetch base version (from PR base SHA)
2. Fetch head version (from PR head SHA)  
3. Apply three-way merge algorithm
4. Use auto-resolution heuristics:
   - Non-overlapping changes
   - Identical changes from both sides
   - Whitespace-only differences
5. Return merged content or conflict markers

## Architecture

```
GitHub API
    ↓
github_client.cpp (C++)
    ↓
PRController.cc (Drogon HTTP handler)
    ↓
three_way_merge.cpp (Core algorithm)
    ↓
JSON Response
```

## Files Changed

### Backend Core
- `backend/include/wizardmerge/git/github_client.h` - GitHub API client interface
- `backend/src/git/github_client.cpp` - GitHub API implementation with libcurl
- `backend/src/controllers/PRController.h` - PR resolution HTTP controller header
- `backend/src/controllers/PRController.cc` - PR resolution controller implementation

### Build System
- `backend/CMakeLists.txt` - Added libcurl dependency, conditional compilation
- `backend/conanfile.py` - Added libcurl/8.4.0 to Conan requirements
- `backend/build.sh` - Added non-interactive mode support

### Frontend
- `frontends/cli/src/main.cpp` - Added `pr-resolve` command with argument parsing

### Testing
- `backend/tests/test_github_client.cpp` - Unit tests for PR URL parsing

### Documentation
- `README.md` - Added PR resolution examples and API documentation
- `backend/README.md` - Detailed API endpoint documentation with curl examples
- `backend/examples/pr_resolve_example.py` - Python example script
- `spec/WizardMergeSpec.tla` - Updated formal specification with PR workflow

## Dependencies

### Required for PR Features
- **libcurl**: HTTP client for GitHub API communication
- **jsoncpp**: JSON parsing (transitive dependency from Drogon)

### Optional
- **Drogon**: Web framework for HTTP server (required for API endpoint)
- **GTest**: Testing framework (required for unit tests)

All dependencies can be installed via Conan package manager.

## Build Instructions

### With Conan (Recommended)
```bash
cd backend
conan install . --output-folder=build --build=missing
cd build
cmake .. -G Ninja -DCMAKE_TOOLCHAIN_FILE=conan_toolchain.cmake
ninja
```

### Without Full Dependencies
```bash
cd backend
WIZARDMERGE_AUTO_BUILD=1 ./build.sh
```

This builds the core library without GitHub API features.

## Usage Examples

### CLI Usage
```bash
# Basic PR resolution
./wizardmerge-cli-frontend pr-resolve --url https://github.com/owner/repo/pull/123

# With GitHub token
./wizardmerge-cli-frontend pr-resolve \
  --url https://github.com/owner/repo/pull/123 \
  --token ghp_xxx

# Save to file
./wizardmerge-cli-frontend pr-resolve \
  --url https://github.com/owner/repo/pull/123 \
  -o result.json
```

### HTTP API Usage
```bash
curl -X POST http://localhost:8080/api/pr/resolve \
  -H "Content-Type: application/json" \
  -d '{
    "pr_url": "https://github.com/owner/repo/pull/123",
    "github_token": "optional_token"
  }'
```

### Python Script
```bash
python backend/examples/pr_resolve_example.py https://github.com/owner/repo/pull/123
```

## Response Format

```json
{
  "success": true,
  "pr_info": {
    "number": 123,
    "title": "Feature: Add new functionality",
    "base_ref": "main",
    "head_ref": "feature-branch",
    "base_sha": "abc1234",
    "head_sha": "def5678",
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
  "branch_created": false,
  "note": "Branch creation requires Git CLI integration (not yet implemented)"
}
```

## Limitations and Future Work

### Current Limitations
1. **Branch Creation**: Not yet implemented; requires Git CLI integration
2. **Merge Base**: Uses simplified merge logic (base vs head) instead of true merge-base commit
3. **Large Files**: GitHub API has file size limits (~100MB)
4. **Rate Limiting**: GitHub API has rate limits (60/hour unauthenticated, 5000/hour authenticated)

### Future Enhancements
1. **Git Integration**: Clone repo, create branches, push resolved changes
2. **Merge Base Detection**: Use `git merge-base` to find true common ancestor
3. **Semantic Merging**: Language-aware conflict resolution (JSON, YAML, etc.)
4. **Dependency Analysis**: SDG-based conflict detection (from research paper)
5. **Interactive Mode**: Present conflicts to user for manual resolution
6. **Batch Processing**: Resolve multiple PRs in parallel

## Testing

### Unit Tests
```bash
cd backend/build
./wizardmerge-tests --gtest_filter=GitHubClientTest.*
```

Tests cover:
- PR URL parsing (valid and invalid formats)
- Special characters in owner/repo names
- Different URL formats (with/without https, www)

### Integration Testing
Requires:
- Running backend server with Drogon + CURL
- GitHub API access (public or with token)
- Real or mock GitHub repository with PRs

## Performance Considerations

- **API Calls**: One call for PR metadata + N calls for file contents (where N = modified files)
- **Rate Limits**: Use GitHub tokens to increase limits
- **Caching**: File contents could be cached by SHA for repeated resolutions
- **Concurrency**: File fetching could be parallelized

## Security

- **Token Handling**: Tokens passed via headers, not logged
- **Input Validation**: URL parsing validates format before API calls
- **Base64 Decoding**: Custom decoder avoids potential vulnerabilities in external libs
- **Rate Limiting**: Respects GitHub API limits to avoid abuse

## Formal Specification

The TLA+ specification (`spec/WizardMergeSpec.tla`) has been updated to include:
- PR resolution workflow model
- File processing state machine
- Success criteria and invariants
- Proof of correctness properties

## Compliance with Roadmap

This implementation completes **Phase 1.2** of the roadmap:
- ✅ Parse pull request URLs
- ✅ Fetch PR data via GitHub API
- ✅ Apply merge algorithm to PR files
- ✅ HTTP API endpoint
- ✅ CLI command
- ✅ Documentation
- ⏳ Git branch creation (future)

## Contributing

To extend this feature:
1. Add new merge strategies in `three_way_merge.cpp`
2. Enhance GitHub client for additional API endpoints
3. Implement Git CLI integration for branch creation
4. Add language-specific semantic merging
5. Improve error handling and retry logic
