---
title: "Phase 1.3: Core UI Components for Conflict Visualization"
labels: ["enhancement", "phase-1", "ui-ux", "high-priority"]
assignees: []
milestone: "Phase 1 - Foundation"
---

## Overview

Implement core UI components for visualizing and resolving merge conflicts in the Qt6 and Next.js frontends. Provide an intuitive, visual interface for understanding and resolving conflicts.

## Related Roadmap Section

Phase 1.3 - Core UI Components

## Motivation

Current merge tools often present conflicts in a confusing way. WizardMerge aims to make conflicts immediately obvious with:
- Clear visual distinction between base, ours, and theirs
- Syntax highlighting for readability
- Easy navigation between conflicts
- Intuitive action buttons

## Features to Implement

### 1. Three-Panel Diff View

Display base, ours, and theirs side-by-side:

```
┌─────────────┬─────────────┬─────────────┐
│    BASE     │    OURS     │   THEIRS    │
├─────────────┼─────────────┼─────────────┤
│ def foo():  │ def foo():  │ def foo():  │
│   return 1  │   return 2  │   return 3  │
│             │ def bar():  │ def baz():  │
│             │   pass      │   pass      │
└─────────────┴─────────────┴─────────────┘
```

**Components**:
- [ ] `ThreePanelView` - Container for three panels
- [ ] `DiffPanel` - Individual diff panel with syntax highlighting
- [ ] Synchronized scrolling between panels
- [ ] Line number display
- [ ] Change highlighting (added, removed, modified)

### 2. Unified Conflict View

Display conflicts inline with markers:

```
func calculate(x int) int {
    // Non-conflicted code
    y := x * 2
    
<<<<<<< OURS
    return y + 1
=======
    return y + 2
>>>>>>> THEIRS
    
    // More non-conflicted code
}
```

**Components**:
- [ ] `UnifiedConflictView` - Inline conflict display
- [ ] `ConflictMarker` - Visual markers for conflict boundaries
- [ ] `ConflictRegion` - Highlighted conflict sections
- [ ] Color coding: Green (ours), Blue (theirs), Yellow (both)
- [ ] Collapsible conflict regions

### 3. Syntax Highlighting

- [ ] Support for common languages:
  - Python
  - JavaScript/TypeScript
  - Java
  - C/C++
  - Go
  - Rust
  - Ruby
  - PHP
  - Shell scripts
  - JSON/YAML/XML
  - HTML/CSS
  - Markdown

**Qt6 Implementation**: QSyntaxHighlighter
**Next.js Implementation**: Prism.js or Highlight.js

### 4. Line Numbering

- [ ] Display line numbers for each panel
- [ ] Align line numbers with code
- [ ] Handle line insertions/deletions
- [ ] Optional: Show line numbers from original files

### 5. Conflict Navigation

- [ ] Conflict counter: "Conflict 2 of 5"
- [ ] Next/Previous conflict buttons
- [ ] Jump to conflict dropdown
- [ ] Keyboard shortcuts:
  - `n` - Next conflict
  - `p` - Previous conflict
  - `j`/`k` - Scroll down/up
- [ ] Minimap showing conflict locations (optional)

**UI Mockup**:
```
┌────────────────────────────────────────────────┐
│  [< Prev]  Conflict 2 of 5  [Next >]   [v]    │
└────────────────────────────────────────────────┘
```

### 6. Conflict Complexity Indicator

Show how complex each conflict is:

```
┌────────────────────────────────────────────────┐
│  Conflict 1: ●○○○○ Simple                     │
│  Conflict 2: ●●●○○ Medium                     │
│  Conflict 3: ●●●●● Complex                    │
└────────────────────────────────────────────────┘
```

**Complexity Factors**:
- Lines affected
- Number of changes
- Semantic complexity (if SDG analysis available)

### 7. Change Type Highlighting

Color-coded change types:
- 🟢 **Added** (green) - New lines
- 🔴 **Removed** (red) - Deleted lines
- 🟡 **Modified** (yellow) - Changed lines
- 🔵 **Conflicted** (blue) - Merge conflict
- ⚪ **Unchanged** (default) - No change

## Technical Design

### Qt6 (QML) Components

```qml
// ThreePanelView.qml
Item {
    id: threePanelView
    
    Row {
        spacing: 1
        
        DiffPanel {
            id: basePanel
            title: "BASE"
            content: mergeModel.baseContent
            width: parent.width / 3
        }
        
        DiffPanel {
            id: oursPanel
            title: "OURS"
            content: mergeModel.oursContent
            width: parent.width / 3
        }
        
        DiffPanel {
            id: theirsPanel
            title: "THEIRS"
            content: mergeModel.theirsContent
            width: parent.width / 3
        }
    }
    
    // Synchronized scrolling
    Connections {
        target: basePanel.flickable
        onContentYChanged: {
            oursPanel.flickable.contentY = basePanel.flickable.contentY
            theirsPanel.flickable.contentY = basePanel.flickable.contentY
        }
    }
}

// DiffPanel.qml
Rectangle {
    id: diffPanel
    property string title: ""
    property string content: ""
    property alias flickable: scrollView.contentItem
    
    Column {
        Header {
            text: title
        }
        
        ScrollView {
            id: scrollView
            
            TextArea {
                text: content
                readOnly: true
                font.family: "monospace"
                // Syntax highlighting applied here
            }
        }
    }
}
```

### Next.js (React) Components

```tsx
// ThreePanelView.tsx
import { useState } from 'react';
import DiffPanel from './DiffPanel';
import SyntaxHighlighter from 'react-syntax-highlighter';

export default function ThreePanelView({ base, ours, theirs, language }) {
  const [scrollTop, setScrollTop] = useState(0);
  
  const handleScroll = (e) => {
    setScrollTop(e.target.scrollTop);
  };
  
  return (
    <div className="three-panel-view">
      <DiffPanel 
        title="BASE" 
        content={base} 
        language={language}
        scrollTop={scrollTop}
        onScroll={handleScroll}
      />
      <DiffPanel 
        title="OURS" 
        content={ours} 
        language={language}
        scrollTop={scrollTop}
        onScroll={handleScroll}
      />
      <DiffPanel 
        title="THEIRS" 
        content={theirs} 
        language={language}
        scrollTop={scrollTop}
        onScroll={handleScroll}
      />
    </div>
  );
}

// DiffPanel.tsx
export default function DiffPanel({ title, content, language, scrollTop, onScroll }) {
  return (
    <div className="diff-panel">
      <div className="panel-header">{title}</div>
      <div className="panel-content" style={{ scrollTop }} onScroll={onScroll}>
        <SyntaxHighlighter language={language}>
          {content}
        </SyntaxHighlighter>
      </div>
    </div>
  );
}
```

## Implementation Steps

### Phase 1: Basic Layout (1 week)
- [ ] Create three-panel layout (Qt6)
- [ ] Create three-panel layout (Next.js)
- [ ] Implement synchronized scrolling
- [ ] Add line numbers

### Phase 2: Conflict Display (1 week)
- [ ] Parse conflicts from merge result
- [ ] Display conflict markers
- [ ] Highlight conflict regions
- [ ] Color-code changes

### Phase 3: Syntax Highlighting (1 week)
- [ ] Integrate QSyntaxHighlighter (Qt6)
- [ ] Integrate Prism.js (Next.js)
- [ ] Add language detection
- [ ] Configure themes

### Phase 4: Navigation (1 week)
- [ ] Implement conflict counter
- [ ] Add next/previous buttons
- [ ] Keyboard shortcuts
- [ ] Jump to conflict dropdown

### Phase 5: Polish (1 week)
- [ ] Conflict complexity indicator
- [ ] Minimap (optional)
- [ ] Responsive design (Next.js)
- [ ] Accessibility (ARIA labels, keyboard nav)
- [ ] Dark mode support

## Acceptance Criteria

- [ ] Three-panel view displays base, ours, theirs
- [ ] Unified view shows conflicts inline
- [ ] Syntax highlighting works for major languages
- [ ] Line numbers displayed correctly
- [ ] Can navigate between conflicts easily
- [ ] Keyboard shortcuts work
- [ ] Scrolling is synchronized in three-panel view
- [ ] UI is responsive (Next.js)
- [ ] Dark mode supported
- [ ] Accessible (keyboard navigation, screen readers)

## Testing

- [ ] Unit tests for components
- [ ] Visual regression tests
- [ ] Accessibility tests
- [ ] Performance tests (large files)
- [ ] Cross-browser testing (Next.js)

## Dependencies

- Three-way merge algorithm ✅
- Backend API ✅

## Priority

**HIGH** - Essential for user experience

## Estimated Effort

5 weeks (parallel development for Qt6 and Next.js)

## Related Issues

- #TBD (Phase 1 completion)
- #TBD (UI/UX improvements)
- #TBD (Enhanced visualization - Phase 2)

## Design Resources

- [ ] Create mockups for three-panel view
- [ ] Create mockups for unified view
- [ ] Design conflict navigation UI
- [ ] Choose color scheme for changes
- [ ] Select monospace fonts

## Success Metrics

- 90% user satisfaction with UI
- <100ms render time for typical files
- 100% keyboard accessibility
- Positive feedback on conflict clarity
