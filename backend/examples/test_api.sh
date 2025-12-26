#!/bin/bash
# Example API calls using curl

SERVER_URL="http://localhost:8080"

echo "WizardMerge API - Example curl Commands"
echo "========================================"
echo

# Test 1: No conflicts
echo "Test 1: No conflicts (non-overlapping changes)"
echo "-----------------------------------------------"
curl -X POST "${SERVER_URL}/api/merge" \
  -H "Content-Type: application/json" \
  -d '{
    "base": ["line1", "line2", "line3"],
    "ours": ["line1", "line2_modified", "line3"],
    "theirs": ["line1", "line2", "line3_modified"]
  }' | jq '.'
echo
echo

# Test 2: With conflicts
echo "Test 2: With conflicts (overlapping changes)"
echo "---------------------------------------------"
curl -X POST "${SERVER_URL}/api/merge" \
  -H "Content-Type: application/json" \
  -d '{
    "base": ["line1", "line2", "line3"],
    "ours": ["line1", "line2_ours", "line3"],
    "theirs": ["line1", "line2_theirs", "line3"]
  }' | jq '.'
echo
echo

# Test 3: Identical changes
echo "Test 3: Identical changes (auto-resolved)"
echo "------------------------------------------"
curl -X POST "${SERVER_URL}/api/merge" \
  -H "Content-Type: application/json" \
  -d '{
    "base": ["line1", "line2", "line3"],
    "ours": ["line1", "line2_same", "line3"],
    "theirs": ["line1", "line2_same", "line3"]
  }' | jq '.'
echo
echo

# Test 4: Error handling - Missing field
echo "Test 4: Error handling - Missing required field"
echo "------------------------------------------------"
curl -X POST "${SERVER_URL}/api/merge" \
  -H "Content-Type: application/json" \
  -d '{
    "base": ["line1", "line2"],
    "ours": ["line1", "line2_modified"]
  }' | jq '.'
echo
echo

# Test 5: Error handling - Invalid JSON
echo "Test 5: Error handling - Invalid JSON"
echo "--------------------------------------"
curl -X POST "${SERVER_URL}/api/merge" \
  -H "Content-Type: application/json" \
  -d 'not json'
echo
echo

echo "Done!"
