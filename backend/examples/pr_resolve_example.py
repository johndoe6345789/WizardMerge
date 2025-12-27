#!/usr/bin/env python3
"""
Example: Resolve GitHub Pull Request conflicts using WizardMerge API

This script demonstrates how to use the WizardMerge API to automatically
resolve merge conflicts in a GitHub pull request.

Usage:
    python pr_resolve_example.py https://github.com/owner/repo/pull/123
    
Environment Variables:
    GITHUB_TOKEN: Optional GitHub API token for private repos
    WIZARDMERGE_BACKEND: Backend URL (default: http://localhost:8080)
"""

import sys
import os
import requests
import json
from typing import Optional


def resolve_pr(
    pr_url: str,
    backend_url: str = "http://localhost:8080",
    github_token: Optional[str] = None,
    create_branch: bool = False,
    branch_name: Optional[str] = None
) -> dict:
    """
    Resolve conflicts in a GitHub pull request.
    
    Args:
        pr_url: URL of the pull request (e.g., https://github.com/owner/repo/pull/123)
        backend_url: URL of WizardMerge backend server
        github_token: Optional GitHub API token
        create_branch: Whether to create a new branch with resolved conflicts
        branch_name: Name of the branch to create (optional)
    
    Returns:
        dict: API response with resolution results
    """
    endpoint = f"{backend_url}/api/pr/resolve"
    
    payload = {
        "pr_url": pr_url,
    }
    
    if github_token:
        payload["github_token"] = github_token
    
    if create_branch:
        payload["create_branch"] = True
        if branch_name:
            payload["branch_name"] = branch_name
    
    print(f"Resolving PR: {pr_url}")
    print(f"Backend: {endpoint}")
    print()
    
    try:
        response = requests.post(endpoint, json=payload, timeout=60)
        response.raise_for_status()
        
        result = response.json()
        return result
        
    except requests.exceptions.ConnectionError:
        print(f"ERROR: Could not connect to backend at {backend_url}")
        print("Make sure the backend server is running:")
        print("  cd backend && ./wizardmerge-cli")
        sys.exit(1)
    except requests.exceptions.HTTPError as e:
        print(f"ERROR: HTTP {e.response.status_code}")
        print(e.response.text)
        sys.exit(1)
    except requests.exceptions.Timeout:
        print(f"ERROR: Request timed out after 60 seconds")
        sys.exit(1)
    except Exception as e:
        print(f"ERROR: {e}")
        sys.exit(1)


def print_results(result: dict):
    """Pretty print the resolution results."""
    print("=" * 70)
    print("PULL REQUEST RESOLUTION RESULTS")
    print("=" * 70)
    print()
    
    if not result.get("success"):
        print("❌ Resolution failed")
        if "error" in result:
            print(f"Error: {result['error']}")
        return
    
    # PR Info
    pr_info = result.get("pr_info", {})
    print(f"📋 PR #{pr_info.get('number')}: {pr_info.get('title')}")
    print(f"   Base: {pr_info.get('base_ref')} ({pr_info.get('base_sha', '')[:7]})")
    print(f"   Head: {pr_info.get('head_ref')} ({pr_info.get('head_sha', '')[:7]})")
    print(f"   Mergeable: {pr_info.get('mergeable')}")
    print()
    
    # Statistics
    total = result.get("total_files", 0)
    resolved = result.get("resolved_count", 0)
    failed = result.get("failed_count", 0)
    
    print(f"📊 Statistics:")
    print(f"   Total files: {total}")
    print(f"   ✅ Resolved: {resolved}")
    print(f"   ❌ Failed: {failed}")
    print(f"   Success rate: {(resolved/total*100) if total > 0 else 0:.1f}%")
    print()
    
    # File details
    print("📁 File Resolution Details:")
    print()
    
    resolved_files = result.get("resolved_files", [])
    for file_info in resolved_files:
        filename = file_info.get("filename", "unknown")
        status = file_info.get("status", "unknown")
        
        if file_info.get("skipped"):
            print(f"   ⊘ {filename} (skipped: {file_info.get('reason', 'N/A')})")
            continue
        
        if file_info.get("error"):
            print(f"   ❌ {filename} - Error: {file_info.get('error')}")
            continue
        
        had_conflicts = file_info.get("had_conflicts", False)
        auto_resolved = file_info.get("auto_resolved", False)
        
        if auto_resolved:
            icon = "✅"
            msg = "auto-resolved"
        elif had_conflicts:
            icon = "⚠️"
            msg = "has unresolved conflicts"
        else:
            icon = "✓"
            msg = "no conflicts"
        
        print(f"   {icon} {filename} - {msg}")
    
    print()
    
    # Branch creation
    if result.get("branch_created"):
        branch = result.get("branch_name", "N/A")
        print(f"🌿 Created branch: {branch}")
    elif "branch_name" in result:
        print(f"📝 Note: {result.get('note', 'Branch creation pending')}")
    
    print()
    print("=" * 70)


def main():
    """Main entry point."""
    if len(sys.argv) < 2 or sys.argv[1] in ["-h", "--help"]:
        print(__doc__)
        sys.exit(0)
    
    pr_url = sys.argv[1]
    
    # Get configuration from environment
    backend_url = os.getenv("WIZARDMERGE_BACKEND", "http://localhost:8080")
    github_token = os.getenv("GITHUB_TOKEN")
    
    # Resolve the PR
    result = resolve_pr(
        pr_url=pr_url,
        backend_url=backend_url,
        github_token=github_token
    )
    
    # Print results
    print_results(result)
    
    # Exit with appropriate code
    if result.get("success") and result.get("resolved_count", 0) > 0:
        sys.exit(0)
    else:
        sys.exit(1)


if __name__ == "__main__":
    main()
