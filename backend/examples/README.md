# WizardMerge API Examples

This directory contains example clients for the WizardMerge HTTP API.

## Prerequisites

Make sure the WizardMerge HTTP server is running:

```sh
cd ..
./build.sh
cd build
./wizardmerge-cli
```

The server should be running on http://localhost:8080

## Examples

### Python Client (`api_client.py`)

Demonstrates API usage with the requests library.

**Requirements:**
```sh
pip install requests
```

**Run:**
```sh
./api_client.py
```

### Curl Examples (`test_api.sh`)

Shell script with curl commands for testing the API.

**Requirements:**
- curl
- jq (optional, for pretty JSON output)

**Run:**
```sh
./test_api.sh
```

## API Endpoint

### POST /api/merge

Performs a three-way merge operation.

**Request Body:**
```json
{
  "base": ["line1", "line2", "line3"],
  "ours": ["line1", "line2_modified", "line3"],
  "theirs": ["line1", "line2", "line3_modified"]
}
```

**Response (Success):**
```json
{
  "merged": ["line1", "line2_modified", "line3_modified"],
  "conflicts": [],
  "has_conflicts": false
}
```

**Response (With Conflicts):**
```json
{
  "merged": [
    "line1",
    "<<<<<<< OURS",
    "line2_ours",
    "=======",
    "line2_theirs",
    ">>>>>>> THEIRS",
    "line3"
  ],
  "conflicts": [
    {
      "start_line": 1,
      "end_line": 1,
      "base_lines": ["line2"],
      "our_lines": ["line2_ours"],
      "their_lines": ["line2_theirs"]
    }
  ],
  "has_conflicts": true
}
```

**Error Response (400 Bad Request):**
```json
{
  "error": "Missing required fields: base, ours, theirs"
}
```

## Manual Testing with curl

Basic example:
```sh
curl -X POST http://localhost:8080/api/merge \
  -H "Content-Type: application/json" \
  -d '{
    "base": ["hello", "world"],
    "ours": ["hello", "beautiful world"],
    "theirs": ["goodbye", "world"]
  }'
```

## Integration Examples

### JavaScript/Node.js

```javascript
const axios = require('axios');

async function merge(base, ours, theirs) {
  const response = await axios.post('http://localhost:8080/api/merge', {
    base, ours, theirs
  });
  return response.data;
}

// Usage
merge(
  ['line1', 'line2'],
  ['line1', 'line2_modified'],
  ['line1', 'line2']
).then(result => {
  console.log('Merged:', result.merged);
  console.log('Has conflicts:', result.has_conflicts);
});
```

### Python

```python
import requests

def merge(base, ours, theirs, server_url="http://localhost:8080"):
    response = requests.post(
        f"{server_url}/api/merge",
        json={"base": base, "ours": ours, "theirs": theirs}
    )
    return response.json()

# Usage
result = merge(
    base=["line1", "line2"],
    ours=["line1", "line2_modified"],
    theirs=["line1", "line2"]
)
print(f"Merged: {result['merged']}")
print(f"Has conflicts: {result['has_conflicts']}")
```

### Go

```go
package main

import (
    "bytes"
    "encoding/json"
    "net/http"
)

type MergeRequest struct {
    Base   []string `json:"base"`
    Ours   []string `json:"ours"`
    Theirs []string `json:"theirs"`
}

type MergeResponse struct {
    Merged       []string    `json:"merged"`
    Conflicts    []Conflict  `json:"conflicts"`
    HasConflicts bool        `json:"has_conflicts"`
}

func merge(base, ours, theirs []string) (*MergeResponse, error) {
    req := MergeRequest{Base: base, Ours: ours, Theirs: theirs}
    jsonData, _ := json.Marshal(req)
    
    resp, err := http.Post(
        "http://localhost:8080/api/merge",
        "application/json",
        bytes.NewBuffer(jsonData),
    )
    if err != nil {
        return nil, err
    }
    defer resp.Body.Close()
    
    var result MergeResponse
    json.NewDecoder(resp.Body).Decode(&result)
    return &result, nil
}
```

## Docker Usage

If running the server in Docker:

```sh
# Start server
docker-compose up -d

# Test API (from host)
curl -X POST http://localhost:8080/api/merge \
  -H "Content-Type: application/json" \
  -d '{"base":["a"],"ours":["b"],"theirs":["c"]}'

# Check logs
docker-compose logs -f
```
