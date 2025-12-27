---
title: "Phase 2.5: Multi-Platform Support (Bitbucket, Azure DevOps, Gitea/Forgejo)"
labels: ["enhancement", "phase-2", "git-platforms", "medium-priority"]
assignees: []
milestone: "Phase 2 - Intelligence & Usability"
---

## Overview

Extend PR/MR conflict resolution to support additional Git platforms beyond GitHub and GitLab. Implement an extensible platform pattern that makes it easy to add new platforms in the future.

## Related Roadmap Section

Phase 2.5 - Additional Platform Support

## Current Status

✅ **Implemented**:
- GitHub (GitHub API v3)
- GitLab (GitLab API)

🔜 **To Implement**:
- Bitbucket (Cloud and Server)
- Azure DevOps (Cloud and Server)
- Gitea/Forgejo

## Platforms to Support

### 1. Bitbucket

#### Bitbucket Cloud
- **API**: Bitbucket Cloud REST API 2.0
- **URL Pattern**: `https://bitbucket.org/{workspace}/{repo}/pull-requests/{number}`
- **Authentication**: 
  - App passwords
  - OAuth 2.0
- **API Endpoints**:
  - GET `/2.0/repositories/{workspace}/{repo}/pullrequests/{number}`
  - GET `/2.0/repositories/{workspace}/{repo}/src/{commit}/{path}`
  - GET `/2.0/repositories/{workspace}/{repo}/diff/{spec}`

#### Bitbucket Server (Self-Hosted)
- **API**: Bitbucket Server REST API 1.0
- **URL Pattern**: `https://{server}/projects/{project}/repos/{repo}/pull-requests/{number}`
- **Authentication**: Personal Access Tokens, HTTP Basic Auth
- **API Endpoints**:
  - GET `/rest/api/1.0/projects/{project}/repos/{repo}/pull-requests/{number}`
  - GET `/rest/api/1.0/projects/{project}/repos/{repo}/browse/{path}?at={ref}`

**Implementation Priority**: HIGH

### 2. Azure DevOps

#### Azure DevOps Cloud
- **API**: Azure DevOps REST API 6.0
- **URL Pattern**: `https://dev.azure.com/{organization}/{project}/_git/{repo}/pullrequest/{number}`
- **Authentication**: Personal Access Tokens (PAT)
- **API Endpoints**:
  - GET `/{organization}/{project}/_apis/git/repositories/{repo}/pullrequests/{number}`
  - GET `/{organization}/{project}/_apis/git/repositories/{repo}/items?path={path}&version={ref}`

#### Azure DevOps Server (On-Premises)
- **API**: Same as cloud
- **URL Pattern**: `https://{server}/{organization}/{project}/_git/{repo}/pullrequest/{number}`
- **Authentication**: PAT, NTLM, Kerberos

**Implementation Priority**: HIGH (widely used in enterprise)

### 3. Gitea / Forgejo

- **API**: Gitea/Forgejo API (OpenAPI compatible)
- **URL Pattern**: `https://{server}/{owner}/{repo}/pulls/{number}`
- **Authentication**: Access tokens
- **API Endpoints**:
  - GET `/api/v1/repos/{owner}/{repo}/pulls/{number}`
  - GET `/api/v1/repos/{owner}/{repo}/contents/{path}?ref={ref}`
- **Note**: Forgejo is a fork of Gitea with compatible API

**Implementation Priority**: MEDIUM (growing community adoption)

## Extensible Platform Pattern

### Design Goals

1. **Easy to add new platforms** - Minimal code changes
2. **Abstract common operations** - Unified interface
3. **Platform-specific customization** - Handle API differences
4. **Configuration-driven** - Define platforms via config
5. **Plugin system** - External platform implementations

### Abstract Interface

```cpp
// Abstract Git platform API
class GitPlatformAPI {
public:
  virtual ~GitPlatformAPI() = default;
  
  // Core operations
  virtual PullRequest fetch_pr_info(
    const std::string& owner,
    const std::string& repo,
    const std::string& pr_number,
    const std::string& token
  ) = 0;
  
  virtual std::string fetch_file_content(
    const std::string& owner,
    const std::string& repo,
    const std::string& file_path,
    const std::string& ref,
    const std::string& token
  ) = 0;
  
  // Optional operations
  virtual bool create_comment(
    const std::string& owner,
    const std::string& repo,
    const std::string& pr_number,
    const std::string& comment,
    const std::string& token
  ) { return false; }
  
  virtual bool update_pr_status(
    const std::string& owner,
    const std::string& repo,
    const std::string& pr_number,
    const std::string& status,
    const std::string& token
  ) { return false; }
  
  // Metadata
  virtual std::string platform_name() const = 0;
  virtual std::string api_base_url() const = 0;
};
```

### Platform Registry

```cpp
class GitPlatformRegistry {
public:
  // Register a platform implementation
  void register_platform(
    const std::string& name,
    std::unique_ptr<GitPlatformAPI> api
  );
  
  // Detect platform from URL
  GitPlatform detect_platform(const std::string& url);
  
  // Get platform implementation
  GitPlatformAPI* get_platform(GitPlatform platform);
  
  // Parse PR URL (delegates to platform)
  PRInfo parse_pr_url(const std::string& url);
};
```

### Platform Detection

```cpp
struct URLPattern {
  std::string pattern;      // Regex pattern
  GitPlatform platform;     // Platform enum
  std::vector<std::string> capture_groups;  // owner, repo, pr_number, etc.
};

class PlatformDetector {
  std::vector<URLPattern> patterns;
  
  GitPlatform detect(const std::string& url) {
    for (const auto& pattern : patterns) {
      if (std::regex_match(url, pattern.pattern)) {
        return pattern.platform;
      }
    }
    return GitPlatform::Unknown;
  }
};
```

### Configuration-Based Platform Definitions

```yaml
# platforms.yml
platforms:
  - name: bitbucket_cloud
    url_pattern: "(?:https?://)?bitbucket\\.org/([^/]+)/([^/]+)/pull-requests/(\\d+)"
    api_base_url: "https://api.bitbucket.org/2.0"
    auth_type: bearer_token
    endpoints:
      pr_info: "/repositories/{owner}/{repo}/pullrequests/{number}"
      file_content: "/repositories/{owner}/{repo}/src/{ref}/{path}"
    field_mappings:
      base_ref: "destination.branch.name"
      head_ref: "source.branch.name"
  
  - name: azure_devops
    url_pattern: "https://dev\\.azure\\.com/([^/]+)/([^/]+)/_git/([^/]+)/pullrequest/(\\d+)"
    api_base_url: "https://dev.azure.com/{organization}/{project}/_apis"
    auth_type: basic_auth
    endpoints:
      pr_info: "/git/repositories/{repo}/pullrequests/{number}?api-version=6.0"
      file_content: "/git/repositories/{repo}/items?path={path}&version={ref}&api-version=6.0"
```

## Implementation Steps

### 1. Refactor Existing Code (1 week)

- [ ] Extract GitHub implementation to `GitHubAPI` class
- [ ] Extract GitLab implementation to `GitLabAPI` class
- [ ] Create `GitPlatformAPI` interface
- [ ] Create `GitPlatformRegistry`
- [ ] Update `PRController` to use registry

### 2. Implement Bitbucket Support (1.5 weeks)

- [ ] Create `BitbucketCloudAPI` class
- [ ] Implement PR info fetching
- [ ] Implement file content fetching
- [ ] Handle authentication (App passwords)
- [ ] Add URL pattern to detector
- [ ] Unit tests
- [ ] Integration tests with Bitbucket API

- [ ] Create `BitbucketServerAPI` class (separate from Cloud)
- [ ] Handle different API structure
- [ ] Test with Bitbucket Server instance

### 3. Implement Azure DevOps Support (1.5 weeks)

- [ ] Create `AzureDevOpsAPI` class
- [ ] Implement PR info fetching
- [ ] Implement file content fetching
- [ ] Handle PAT authentication
- [ ] Add URL pattern to detector
- [ ] Handle both Cloud and Server versions
- [ ] Unit tests
- [ ] Integration tests

### 4. Implement Gitea/Forgejo Support (1 week)

- [ ] Create `GiteaAPI` class
- [ ] Implement PR info fetching
- [ ] Implement file content fetching
- [ ] Handle token authentication
- [ ] Add URL pattern to detector
- [ ] Unit tests
- [ ] Integration tests

### 5. Configuration System (1 week)

- [ ] Design YAML schema for platform definitions
- [ ] Implement YAML parser
- [ ] Create `ConfigBasedPlatform` class
- [ ] Support dynamic platform loading
- [ ] Documentation for adding custom platforms

### 6. Testing & Documentation (1 week)

- [ ] Comprehensive unit tests for all platforms
- [ ] Integration tests with real APIs (use test repos)
- [ ] Update README.md with examples for all platforms
- [ ] Create platform implementation guide
- [ ] API documentation

## Example Usage

### Bitbucket Cloud

```bash
# CLI
./wizardmerge-cli-frontend pr-resolve \
  --url https://bitbucket.org/myworkspace/myrepo/pull-requests/42 \
  --token <app_password>

# Or with environment variable
export BITBUCKET_TOKEN=<app_password>
./wizardmerge-cli-frontend pr-resolve \
  --url https://bitbucket.org/myworkspace/myrepo/pull-requests/42
```

```bash
# HTTP API
curl -X POST http://localhost:8080/api/pr/resolve \
  -H "Content-Type: application/json" \
  -d '{
    "pr_url": "https://bitbucket.org/myworkspace/myrepo/pull-requests/42",
    "api_token": "<app_password>"
  }'
```

### Azure DevOps

```bash
# CLI
./wizardmerge-cli-frontend pr-resolve \
  --url https://dev.azure.com/myorg/myproject/_git/myrepo/pullrequest/123 \
  --token <pat>

# HTTP API
curl -X POST http://localhost:8080/api/pr/resolve \
  -H "Content-Type: application/json" \
  -d '{
    "pr_url": "https://dev.azure.com/myorg/myproject/_git/myrepo/pullrequest/123",
    "api_token": "<pat>"
  }'
```

### Gitea

```bash
# CLI
./wizardmerge-cli-frontend pr-resolve \
  --url https://gitea.mycompany.com/owner/repo/pulls/5 \
  --token <access_token>
```

## Platform Implementation Guide

For future contributors who want to add a new platform:

### Step 1: Create Platform API Class

```cpp
class MyPlatformAPI : public GitPlatformAPI {
public:
  PullRequest fetch_pr_info(...) override {
    // 1. Build API URL
    std::string url = api_base_url + "/pr/endpoint";
    
    // 2. Make HTTP request with auth
    auto response = http_client.get(url, headers);
    
    // 3. Parse JSON response
    Json::Value root = parse_json(response.body);
    
    // 4. Map to PullRequest structure
    PullRequest pr;
    pr.base_ref = root["target_branch"].asString();
    pr.head_ref = root["source_branch"].asString();
    // ... map other fields
    
    return pr;
  }
  
  std::string fetch_file_content(...) override {
    // Similar pattern
  }
  
  std::string platform_name() const override { return "MyPlatform"; }
};
```

### Step 2: Add URL Pattern

```cpp
// In git_platform_client.cpp
std::regex myplatform_regex(
  R"((?:https?://)?myplatform\.com/([^/]+)/([^/]+)/pr/(\\d+))"
);

if (std::regex_match(pr_url, match, myplatform_regex)) {
  platform = GitPlatform::MyPlatform;
  // Extract owner, repo, pr_number from match groups
}
```

### Step 3: Register in Registry

```cpp
// In main() or init()
platform_registry.register_platform(
  "myplatform",
  std::make_unique<MyPlatformAPI>()
);
```

### Step 4: Add Tests

```cpp
TEST(MyPlatformAPITest, FetchPRInfo) {
  MyPlatformAPI api;
  auto pr = api.fetch_pr_info("owner", "repo", "123", "token");
  ASSERT_EQ(pr.number, "123");
  ASSERT_FALSE(pr.base_ref.empty());
}
```

## Acceptance Criteria

- [ ] Bitbucket Cloud support working
- [ ] Bitbucket Server support working
- [ ] Azure DevOps Cloud support working
- [ ] Azure DevOps Server support working
- [ ] Gitea/Forgejo support working
- [ ] Platform registry implemented
- [ ] URL detection automatic
- [ ] Configuration-based platforms supported
- [ ] All platforms tested with real APIs
- [ ] Documentation complete
- [ ] Implementation guide available

## Priority

**MEDIUM-HIGH** - Important for enterprise adoption and wider user base.

## Estimated Effort

6-8 weeks total

## Dependencies

- PR resolution feature ✅
- Git CLI integration ✅

## Related Issues

- #TBD (Phase 2 tracking)
- #TBD (API improvements)

## Success Metrics

- Support 5+ major Git platforms
- <5% error rate for each platform
- Easy to add new platforms (<1 day of work)
- 90% user satisfaction across platforms
