---
title: "Phase 1.4: Basic Conflict Resolution Actions and Undo/Redo"
labels: ["enhancement", "phase-1", "ui-ux", "high-priority"]
assignees: []
milestone: "Phase 1 - Foundation"
---

## Overview

Implement user actions for resolving conflicts: accept ours, accept theirs, accept both, manual edit, and a robust undo/redo system for safe conflict resolution.

## Related Roadmap Section

Phase 1.4 - Basic Conflict Resolution Actions

## Motivation

Users need simple, clear actions to resolve conflicts:
- One-click resolution for simple cases
- Safety net (undo) for mistakes
- Keyboard shortcuts for efficiency
- Visual feedback for actions taken

## Features to Implement

### 1. Resolution Actions

#### Accept Ours
- [ ] Keep changes from "ours" branch
- [ ] Discard changes from "theirs" branch
- [ ] Button: "Accept Ours" or "Keep Ours"
- [ ] Keyboard shortcut: `o` or `Ctrl+1`

#### Accept Theirs
- [ ] Keep changes from "theirs" branch
- [ ] Discard changes from "ours" branch
- [ ] Button: "Accept Theirs" or "Keep Theirs"
- [ ] Keyboard shortcut: `t` or `Ctrl+2`

#### Accept Both
- [ ] Keep changes from both branches
- [ ] Concatenate or merge intelligently
- [ ] Button: "Accept Both" or "Keep Both"
- [ ] Keyboard shortcut: `b` or `Ctrl+3`
- [ ] Options for ordering:
  - Ours first, then theirs
  - Theirs first, then ours
  - Smart merge (if possible)

#### Manual Edit
- [ ] Allow direct text editing in conflict region
- [ ] Syntax highlighting preserved
- [ ] Real-time validation
- [ ] Button: "Edit Manually"
- [ ] Keyboard shortcut: `e` or `Ctrl+E`

#### Accept Smart Suggestion
- [ ] If SDG or AI provides suggestion, allow one-click accept
- [ ] Display confidence score
- [ ] Button: "Accept Suggestion"
- [ ] Keyboard shortcut: `s` or `Ctrl+Enter`

### 2. Action Bar UI

```
┌────────────────────────────────────────────────────┐
│  Conflict 2 of 5                                   │
│  ┌──────────┬──────────┬──────────┬──────────┐    │
│  │ Keep Ours│Keep Theirs│Keep Both│Edit (e) │    │
│  │   (o)    │    (t)    │   (b)   │         │    │
│  └──────────┴──────────┴──────────┴──────────┘    │
│                                                     │
│  Smart Suggestion (82% confidence): Keep Ours      │
│  [ Accept Suggestion (s) ]                         │
└────────────────────────────────────────────────────┘
```

### 3. Undo/Redo System

**Requirements**:
- [ ] Unlimited undo/redo history (or configurable limit)
- [ ] Per-conflict undo/redo
- [ ] Global undo/redo across all conflicts
- [ ] Persist undo history during session
- [ ] Clear visual indication of undo state

**UI**:
```
┌────────────────────────────────────────────────────┐
│  File: src/app.py            [Undo (⌘Z)] [Redo (⌘⇧Z)] │
│                                                     │
│  History:                                          │
│  ✓ Conflict 1: Accepted Ours                      │
│  ✓ Conflict 2: Accepted Both                      │
│  → Conflict 3: Manual Edit (current)               │
│  ? Conflict 4: Unresolved                         │
│  ? Conflict 5: Unresolved                         │
└────────────────────────────────────────────────────┘
```

**Keyboard Shortcuts**:
- Undo: `Ctrl+Z` (Win/Linux), `Cmd+Z` (Mac)
- Redo: `Ctrl+Shift+Z` or `Ctrl+Y` (Win/Linux), `Cmd+Shift+Z` (Mac)

**Command Pattern Implementation**:
```cpp
class Command {
public:
  virtual ~Command() = default;
  virtual void execute() = 0;
  virtual void undo() = 0;
  virtual std::string description() const = 0;
};

class AcceptOursCommand : public Command {
  ConflictID conflict_id;
  std::string previous_state;
  std::string new_state;
  
public:
  void execute() override {
    previous_state = get_conflict_state(conflict_id);
    set_conflict_state(conflict_id, resolve_with_ours());
    new_state = get_conflict_state(conflict_id);
  }
  
  void undo() override {
    set_conflict_state(conflict_id, previous_state);
  }
  
  std::string description() const override {
    return "Accept Ours for Conflict " + std::to_string(conflict_id);
  }
};

class UndoRedoManager {
  std::vector<std::unique_ptr<Command>> undo_stack;
  std::vector<std::unique_ptr<Command>> redo_stack;
  
public:
  void execute_command(std::unique_ptr<Command> cmd) {
    cmd->execute();
    undo_stack.push_back(std::move(cmd));
    redo_stack.clear();  // Clear redo stack on new command
  }
  
  void undo() {
    if (!undo_stack.empty()) {
      auto cmd = std::move(undo_stack.back());
      undo_stack.pop_back();
      cmd->undo();
      redo_stack.push_back(std::move(cmd));
    }
  }
  
  void redo() {
    if (!redo_stack.empty()) {
      auto cmd = std::move(redo_stack.back());
      redo_stack.pop_back();
      cmd->execute();
      undo_stack.push_back(std::move(cmd));
    }
  }
  
  bool can_undo() const { return !undo_stack.empty(); }
  bool can_redo() const { return !redo_stack.empty(); }
  
  std::vector<std::string> get_history() const {
    std::vector<std::string> history;
    for (const auto& cmd : undo_stack) {
      history.push_back(cmd->description());
    }
    return history;
  }
};
```

### 4. Visual Feedback

- [ ] **Action confirmation**: Show toast/notification after action
  - "✓ Conflict resolved with 'Ours'"
  - "✓ Manual edit applied"
  
- [ ] **Progress indicator**: Show how many conflicts resolved
  - "3 of 5 conflicts resolved"
  
- [ ] **Color changes**: Mark resolved conflicts differently
  - Unresolved: Red border
  - Resolved: Green border
  
- [ ] **Animation**: Smooth transition when accepting resolution
  - Fade out conflict markers
  - Fade in resolved code

### 5. Batch Actions

For multiple similar conflicts:

- [ ] **Accept All Ours**: Resolve all conflicts with "ours"
- [ ] **Accept All Theirs**: Resolve all conflicts with "theirs"
- [ ] **Accept All Smart Suggestions**: Auto-resolve with AI/SDG suggestions
- [ ] **Confirmation dialog**: "This will resolve 5 conflicts. Continue?"

```
┌────────────────────────────────────────────────────┐
│  Batch Actions                                     │
│  ┌──────────────────┬──────────────────┐          │
│  │ Accept All Ours  │ Accept All Theirs│          │
│  └──────────────────┴──────────────────┘          │
│  [ Accept All Smart Suggestions ]                 │
└────────────────────────────────────────────────────┘
```

### 6. Conflict Status Tracking

Track state of each conflict:

```cpp
enum class ConflictState {
  UNRESOLVED,           // Not yet resolved
  RESOLVED_OURS,        // Resolved with "ours"
  RESOLVED_THEIRS,      // Resolved with "theirs"
  RESOLVED_BOTH,        // Resolved with "both"
  RESOLVED_MANUAL,      // Manually edited
  RESOLVED_SUGGESTION,  // Accepted smart suggestion
};

struct ConflictStatus {
  ConflictID id;
  ConflictState state;
  std::string resolved_content;
  time_t resolved_at;
  std::string resolution_method;  // "ours", "theirs", "both", "manual", "suggestion"
};
```

### 7. Keyboard Shortcuts

Full keyboard navigation:

| Action | Shortcut | Alternative |
|--------|----------|-------------|
| Accept Ours | `o` | `Ctrl+1` |
| Accept Theirs | `t` | `Ctrl+2` |
| Accept Both | `b` | `Ctrl+3` |
| Manual Edit | `e` | `Ctrl+E` |
| Accept Suggestion | `s` | `Ctrl+Enter` |
| Next Conflict | `n` | `Ctrl+Down` |
| Previous Conflict | `p` | `Ctrl+Up` |
| Undo | `Ctrl+Z` | `u` |
| Redo | `Ctrl+Shift+Z` | `r` |
| Save | `Ctrl+S` | - |
| Cancel | `Esc` | `Ctrl+Q` |

## Technical Design

### Action Handler (C++ Backend)

```cpp
class ConflictResolver {
public:
  // Resolution actions
  MergeResult accept_ours(ConflictID conflict_id);
  MergeResult accept_theirs(ConflictID conflict_id);
  MergeResult accept_both(ConflictID conflict_id, MergeOrder order);
  MergeResult manual_edit(ConflictID conflict_id, const std::string& content);
  MergeResult accept_suggestion(ConflictID conflict_id);
  
  // Batch actions
  std::vector<MergeResult> accept_all_ours();
  std::vector<MergeResult> accept_all_theirs();
  std::vector<MergeResult> accept_all_suggestions();
  
  // State management
  ConflictStatus get_status(ConflictID conflict_id);
  std::vector<ConflictStatus> get_all_statuses();
  bool is_all_resolved();
};
```

### UI Integration (Qt6 QML)

```qml
Item {
    id: conflictResolutionPanel
    
    property var undoRedoManager: UndoRedoManager {}
    
    Row {
        spacing: 10
        
        Button {
            text: "Keep Ours (o)"
            onClicked: resolveWithOurs()
        }
        
        Button {
            text: "Keep Theirs (t)"
            onClicked: resolveWithTheirs()
        }
        
        Button {
            text: "Keep Both (b)"
            onClicked: resolveWithBoth()
        }
        
        Button {
            text: "Edit (e)"
            onClicked: enableManualEdit()
        }
    }
    
    Row {
        Button {
            text: "Undo (Ctrl+Z)"
            enabled: undoRedoManager.canUndo
            onClicked: undoRedoManager.undo()
        }
        
        Button {
            text: "Redo (Ctrl+Shift+Z)"
            enabled: undoRedoManager.canRedo
            onClicked: undoRedoManager.redo()
        }
    }
    
    // Keyboard shortcuts
    Shortcut {
        sequence: "o"
        onActivated: resolveWithOurs()
    }
    
    Shortcut {
        sequence: "Ctrl+Z"
        onActivated: undoRedoManager.undo()
    }
    
    // ... other shortcuts
}
```

### UI Integration (Next.js React)

```tsx
import { useState } from 'react';
import { useHotkeys } from 'react-hotkeys-hook';

export default function ConflictResolutionPanel({ conflict, onResolve }) {
  const [history, setHistory] = useState([]);
  const [historyIndex, setHistoryIndex] = useState(-1);
  
  const resolveWithOurs = () => {
    const command = { type: 'OURS', conflictId: conflict.id, previousState: conflict.state };
    executeCommand(command);
  };
  
  const undo = () => {
    if (historyIndex >= 0) {
      const command = history[historyIndex];
      undoCommand(command);
      setHistoryIndex(historyIndex - 1);
    }
  };
  
  const redo = () => {
    if (historyIndex < history.length - 1) {
      const command = history[historyIndex + 1];
      executeCommand(command);
      setHistoryIndex(historyIndex + 1);
    }
  };
  
  // Keyboard shortcuts
  useHotkeys('o', resolveWithOurs);
  useHotkeys('ctrl+z', undo);
  useHotkeys('ctrl+shift+z', redo);
  
  return (
    <div className="conflict-resolution-panel">
      <div className="actions">
        <button onClick={resolveWithOurs}>Keep Ours (o)</button>
        <button onClick={resolveWithTheirs}>Keep Theirs (t)</button>
        <button onClick={resolveWithBoth}>Keep Both (b)</button>
        <button onClick={enableManualEdit}>Edit (e)</button>
      </div>
      <div className="undo-redo">
        <button onClick={undo} disabled={historyIndex < 0}>Undo (⌘Z)</button>
        <button onClick={redo} disabled={historyIndex >= history.length - 1}>Redo (⌘⇧Z)</button>
      </div>
    </div>
  );
}
```

## Implementation Steps

### Phase 1: Basic Actions (1 week)
- [ ] Implement accept ours/theirs/both in backend
- [ ] Create action buttons in UI
- [ ] Add visual feedback (toasts)
- [ ] Test with simple conflicts

### Phase 2: Manual Edit (1 week)
- [ ] Enable manual editing mode
- [ ] Preserve syntax highlighting during edit
- [ ] Real-time validation
- [ ] Save edited content

### Phase 3: Undo/Redo (2 weeks)
- [ ] Implement command pattern
- [ ] Create undo/redo manager
- [ ] Integrate with UI
- [ ] Test complex scenarios

### Phase 4: Keyboard Shortcuts (1 week)
- [ ] Implement all shortcuts
- [ ] Add shortcut hints in UI
- [ ] Handle conflicts between shortcuts
- [ ] Test on different platforms

### Phase 5: Batch Actions (1 week)
- [ ] Implement batch resolution
- [ ] Add confirmation dialogs
- [ ] Progress indicators
- [ ] Test with many conflicts

### Phase 6: Polish (1 week)
- [ ] Animations and transitions
- [ ] Accessibility improvements
- [ ] Error handling
- [ ] User testing

## Acceptance Criteria

- [ ] Can resolve conflicts with one click
- [ ] Undo/redo works correctly
- [ ] Keyboard shortcuts work on all platforms
- [ ] Manual editing preserves syntax
- [ ] Batch actions work for multiple conflicts
- [ ] Visual feedback is clear
- [ ] State persists during session
- [ ] Accessible (keyboard-only navigation)

## Testing

- [ ] Unit tests for each action
- [ ] Undo/redo edge cases
- [ ] Keyboard shortcut conflicts
- [ ] Performance with many conflicts
- [ ] Accessibility testing

## Priority

**HIGH** - Essential for basic usability

## Estimated Effort

7 weeks

## Dependencies

- Core UI components (Issue #TBD)
- Three-way merge algorithm ✅

## Related Issues

- #TBD (Phase 1 completion)
- #TBD (Core UI components)
- #TBD (Keyboard navigation)

## Success Metrics

- 95% user satisfaction with actions
- <50ms action response time
- 100% undo/redo correctness
- 90% of users use keyboard shortcuts
