#!/usr/bin/env python3
"""
Example client for WizardMerge HTTP API
Demonstrates how to use the POST /api/merge endpoint
"""

import requests
import json
import sys

def test_merge(base_lines, ours_lines, theirs_lines, server_url="http://localhost:8080"):
    """
    Test the merge API with given inputs
    """
    endpoint = f"{server_url}/api/merge"
    
    payload = {
        "base": base_lines,
        "ours": ours_lines,
        "theirs": theirs_lines
    }
    
    print(f"Sending merge request to {endpoint}")
    print(f"Base:   {base_lines}")
    print(f"Ours:   {ours_lines}")
    print(f"Theirs: {theirs_lines}")
    print()
    
    try:
        response = requests.post(endpoint, json=payload)
        response.raise_for_status()
        
        result = response.json()
        
        print("=== Merge Result ===")
        print(f"Merged lines: {result['merged']}")
        print(f"Has conflicts: {result['has_conflicts']}")
        
        if result['conflicts']:
            print(f"Number of conflicts: {len(result['conflicts'])}")
            for i, conflict in enumerate(result['conflicts']):
                print(f"\nConflict {i+1}:")
                print(f"  Lines: {conflict['start_line']}-{conflict['end_line']}")
                print(f"  Base: {conflict['base_lines']}")
                print(f"  Ours: {conflict['our_lines']}")
                print(f"  Theirs: {conflict['their_lines']}")
        
        return result
        
    except requests.exceptions.ConnectionError:
        print(f"ERROR: Could not connect to server at {server_url}")
        print("Make sure the server is running with: ./wizardmerge-cli")
        sys.exit(1)
    except requests.exceptions.HTTPError as e:
        print(f"ERROR: HTTP {e.response.status_code}")
        print(e.response.text)
        sys.exit(1)
    except Exception as e:
        print(f"ERROR: {e}")
        sys.exit(1)

def main():
    print("WizardMerge API Client - Example Usage")
    print("=" * 50)
    print()
    
    # Test 1: No conflicts
    print("Test 1: No conflicts (non-overlapping changes)")
    print("-" * 50)
    test_merge(
        base_lines=["line1", "line2", "line3"],
        ours_lines=["line1", "line2_modified", "line3"],
        theirs_lines=["line1", "line2", "line3_modified"]
    )
    print()
    
    # Test 2: With conflicts
    print("\nTest 2: With conflicts (overlapping changes)")
    print("-" * 50)
    test_merge(
        base_lines=["line1", "line2", "line3"],
        ours_lines=["line1", "line2_ours", "line3"],
        theirs_lines=["line1", "line2_theirs", "line3"]
    )
    print()
    
    # Test 3: Identical changes
    print("\nTest 3: Identical changes (auto-resolved)")
    print("-" * 50)
    test_merge(
        base_lines=["line1", "line2", "line3"],
        ours_lines=["line1", "line2_same", "line3"],
        theirs_lines=["line1", "line2_same", "line3"]
    )
    print()

if __name__ == "__main__":
    main()
