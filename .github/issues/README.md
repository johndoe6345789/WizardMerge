# WizardMerge GitHub Issues

This directory contains detailed GitHub issues for the WizardMerge project, covering project specifications and future feature implementations.

## Overview

These issue templates provide comprehensive documentation for:
- Project architecture and specifications
- Phase 1 (Foundation) features
- Phase 2 (Intelligence & Usability) features  
- Phase 3 (Advanced Features)
- Testing and quality assurance

## Issue Templates

Located in `.github/ISSUE_TEMPLATE/`:

1. **bug_report.yml** - Report bugs and issues
2. **feature_request.yml** - Suggest new features
3. **documentation.yml** - Improve documentation
4. **config.yml** - Configuration for issue templates

## Feature Issues

Located in `.github/issues/`:

### Project Specification
- **01-project-specification.md** - Core architecture, components, and current status

### Phase 1: Foundation
- **02-file-io-git-integration.md** - File I/O, Git repository integration, conflict parsing
- **07-core-ui-components.md** - Three-panel view, syntax highlighting, conflict navigation
- **09-conflict-resolution-actions.md** - Resolution actions, undo/redo, keyboard shortcuts

### Phase 2: Intelligence & Usability
- **03-semantic-merge-structured-files.md** - JSON, YAML, XML, package file merging
- **04-ast-based-merging.md** - Language-aware merging (Python, JS, Java, C++)
- **05-sdg-analysis.md** - System Dependence Graph analysis (core research contribution)
- **06-multi-platform-support.md** - Bitbucket, Azure DevOps, Gitea/Forgejo support
- **10-testing-quality.md** - Comprehensive testing, benchmarks, fuzzing

### Phase 3: Advanced Features
- **08-ai-assisted-merging.md** - ML models, natural language explanations, risk assessment

## How to Use These Issues

### For Project Planning

1. **Review the project specification** (issue 01) to understand the overall architecture
2. **Prioritize issues** based on roadmap phases and dependencies
3. **Create GitHub issues** from these templates by copying content
4. **Track progress** using GitHub Projects or milestones

### For Contributors

1. **Choose an issue** that matches your skills and interests
2. **Read the full issue description** including implementation steps
3. **Check dependencies** - some issues require others to be completed first
4. **Ask questions** by commenting on the issue
5. **Submit PRs** that reference the issue number

### For Creating GitHub Issues

You can create issues directly from these templates:

```bash
# Using GitHub CLI
gh issue create --title "Phase 2.1: Semantic Merge for Structured Files" \
  --body-file .github/issues/03-semantic-merge-structured-files.md \
  --label "enhancement,phase-2,semantic-merge,high-priority" \
  --milestone "Phase 2 - Intelligence & Usability"
```

Or copy-paste the content into GitHub's web interface.

## Issue Metadata

Each issue includes:
- **Title** - Clear, descriptive title
- **Labels** - For categorization (phase, priority, component)
- **Milestone** - Which roadmap phase it belongs to
- **Overview** - High-level description
- **Motivation** - Why this feature is important
- **Features** - Detailed list of sub-features
- **Technical Design** - Architecture and implementation approach
- **Implementation Steps** - Phased development plan
- **Acceptance Criteria** - Definition of done
- **Dependencies** - What must be completed first
- **Estimated Effort** - Time estimate
- **Priority** - HIGH/MEDIUM/LOW

## Priority Levels

- **HIGH**: Essential for the current phase, blocks other work
- **MEDIUM**: Important but can be deferred
- **LOW**: Nice to have, future enhancement

## Dependencies

Issues are organized with dependencies in mind:

```
Phase 1 (Foundation)
├─ Three-way merge algorithm ✅ (completed)
├─ Git CLI integration ✅ (completed)
├─ 02: File I/O & Git integration
├─ 07: Core UI components
└─ 09: Conflict resolution actions

Phase 2 (Intelligence)
├─ 03: Semantic merge (depends on: Phase 1)
├─ 04: AST-based merging (depends on: 03)
├─ 05: SDG analysis (depends on: 04)
├─ 06: Multi-platform support (depends on: Phase 1)
└─ 10: Testing & quality (depends on: all Phase 2)

Phase 3 (Advanced)
└─ 08: AI-assisted merging (depends on: 05)
```

## Roadmap Alignment

These issues align with the project roadmap in `ROADMAP.md`:

- **Phase 1 (0-3 months)**: Foundation - Issues 02, 07, 09
- **Phase 2 (3-6 months)**: Intelligence - Issues 03, 04, 05, 06, 10
- **Phase 3 (6-12 months)**: Advanced - Issue 08

## Contributing

See each issue for:
- **Implementation steps** - Detailed development plan
- **Technical design** - Architecture and code examples
- **Acceptance criteria** - How to know when it's done
- **Test cases** - What to test

## Issue Labels

Common labels used:
- `enhancement` - New feature
- `bug` - Bug report
- `documentation` - Documentation improvement
- `phase-1`, `phase-2`, `phase-3` - Roadmap phase
- `high-priority`, `medium-priority`, `low-priority` - Priority level
- Component labels: `semantic-merge`, `ast-merge`, `sdg-analysis`, `ui-ux`, `git-integration`, `ai-ml`, `testing`

## Creating Issues from Templates

### Option 1: GitHub Web Interface
1. Go to Issues → New Issue
2. Select template (bug report, feature request, or documentation)
3. Fill in the form
4. Submit

### Option 2: Copy from Issue Files
1. Navigate to `.github/issues/`
2. Open the issue markdown file
3. Copy content to new GitHub issue
4. Set labels and milestone

### Option 3: GitHub CLI
```bash
# Create issue from file
gh issue create \
  --title "Issue Title" \
  --body-file .github/issues/XX-issue-name.md \
  --label "label1,label2" \
  --milestone "Milestone Name"
```

## Questions?

- Open a discussion in GitHub Discussions
- Comment on related issues
- Reach out to maintainers

## License

These issue templates are part of the WizardMerge project and follow the same license.
