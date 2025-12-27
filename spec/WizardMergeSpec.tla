------------------------------- MODULE WizardMergeSpec -------------------------------
EXTENDS Naturals, FiniteSets

(*
  Implementation Status (as of December 2024):
  
  This formal specification describes the dependency-aware merge algorithm that
  WizardMerge aims to implement. The current implementation status is:
  
  IMPLEMENTED (Phase 1.1):
  - Basic three-way merge algorithm (C++ backend)
  - Line-level conflict detection
  - Auto-resolution for common patterns:
    * Non-overlapping changes
    * Identical changes from both sides
    * Whitespace-only differences
  - Command-line interface (wizardmerge-cli)
  - Pull request URL processing and conflict resolution:
    * Parse GitHub PR URLs
    * Fetch PR data via GitHub API
    * Apply merge algorithm to PR files
    * HTTP API endpoint for PR resolution
  
  NOT YET IMPLEMENTED (Future phases):
  - Dependency graph construction (SDG analysis)
  - LLVM-IR level analysis
  - Edge classification (safe vs. violated)
  - Fine-grained DCB (Definition-Code Block) tracking
  - Mirror mapping and matching
  - Git branch creation for resolved PRs
  
  The current implementation in backend/src/merge/three_way_merge.cpp provides
  a foundation for the full dependency-aware algorithm specified here. Future
  phases will enhance it with the SDG analysis, edge classification, and
  dependency-aware conflict resolution described in this specification.
  
  PR Resolution Workflow (Phase 1.2):
  The PR resolution feature extends the core merge algorithm to work with
  GitHub pull requests. The workflow is:
    1. Accept PR URL: Parse URL to extract owner, repo, and PR number
    2. Fetch PR metadata: Use GitHub API to retrieve PR information
    3. Fetch file versions: Retrieve base and head versions of modified files
    4. Apply merge algorithm: For each file, perform three-way merge
    5. Auto-resolve conflicts: Apply heuristic resolution where possible
    6. Return results: Provide merged content and conflict status
  
  This workflow enables batch processing of PR conflicts using the same
  dependency-aware merge principles, with future integration planned for
  automatic branch creation and PR updates.
*)

(*
  High-level intent

  This module formalizes the core of WizardMerge's merge-handling logic at the
  dependency-graph level. It abstracts away concrete source code and LLVM IR and
  instead reasons about:

    - Vertices: fine-grained DCB-bearing definition nodes in the Shrunk
      Dependency Graphs (SDGs) for VA and VB.

    - Edges: directed dependency edges v -> u in those SDGs.

    - Per-vertex status in the merged result:
        * applied        (vertex chosen by Git's textual merge)
        * conflict       (vertex is part of a conflict in Git's textual merge)
        * not-applied    (vertex is discarded in the merge)

    - Mirror mapping Mi(v): the "mirror" vertex of v in the other SDG.

    - Matching information: whether a mirror node Mi(v) is a "matching" node
      (same definition name and compatible DCB metadata).

  The key objective is to classify edges as safe vs. violated according to the
  edge-sensitive rules (1)-(6) in the paper, and to derive the set of violated
  vertices/DCBs. A "properly handled" merge is one where there are no violated
  edges (GoodMerge = TRUE).
*)

CONSTANTS
  (*
    Three versions used by the textual three-way merge:
      - Base : common ancestor commit
      - VA   : Variant A
      - VB   : Variant B

    These are only used as tags; they are assumed to be distinct.
  *)
  Base, VA, VB,

  (*
    VERTICES: the set of all vertices (fine-grained definition/DCB nodes) that
    appear in either SDG (for VA or VB). Each vertex belongs to exactly one
    SDG (either VA's or VB's).
  *)
  VERTICES,

  (*
    EDGES: the set of dependency edges in both SDGs. Each edge is a record
    [from |-> v, to |-> u], which corresponds to v -> u (v depends on u).
  *)
  EDGES,

  (*
    VersionTag: a function that maps each vertex to the SDG it belongs to,
    i.e. either VA or VB. Base is never used as a VersionTag value.
  *)
  VersionTag,

  (*
    Mirror: Mi(v) in the paper. For a vertex v in one SDG, Mirror[v] is the
    unique corresponding vertex in the other SDG. Mirror is a bijection and
    an involution (Mirror[Mirror[v]] = v).
  *)
  Mirror,

  (*
    MatchSet: the set of vertices whose mirror node is a "matching" node
    according to the graph-text alignment phase (same definition and aligned
    DCB metadata). In the paper this is match(Mi(v)) = True/False.
    Here we encode it as a unary predicate: v ∈ MatchSet.
  *)
  MatchSet,

  (*
    AppliedSet: the set V_A of vertices whose DCBs are applied in the merged
    textual result from Git (non-conflicting choice).
  *)
  AppliedSet,

  (*
    ConflictSet: the set V_C of vertices that are part of a textual conflict
    produced by Git.
  *)
  ConflictSet

(***************************************************************************)
(* Basic derived sets and assumptions                                      *)
(***************************************************************************)

(*
  NotAppliedSet: the set V_N of vertices that are not applied in the textual
  merge result (they are discarded or overshadowed).
*)
NotAppliedSet ==
  VERTICES \ (AppliedSet \cup ConflictSet)

(*
  Sanity assumptions about the configuration. These are required to model the
  SDG properties stated in the paper.
*)
ASSUME
  /\ AppliedSet \subseteq VERTICES
  /\ ConflictSet \subseteq VERTICES
  /\ AppliedSet \cap ConflictSet = {}               \* no vertex is both applied and conflicted
  /\ EDGES \subseteq [from : VERTICES, to : VERTICES]
  /\ MatchSet \subseteq VERTICES
  /\ VersionTag \in [VERTICES -> {VA, VB}]
  /\ Mirror \in [VERTICES -> VERTICES]
  /\ \A v \in VERTICES : Mirror[Mirror[v]] = v      \* Mirror is an involution
  /\ \A v \in VERTICES : VersionTag[Mirror[v]] # VersionTag[v]
       \* Mirror always points into the "other" SDG
  /\ \A v \in VERTICES :
       (*
         SDG facts from the paper:

           - If v ∈ V_A (applied) then Mi(v) ∈ V_N (not applied), and vice versa.
           - If v ∈ V_C (conflict) then Mi(v) ∈ V_C as well.
       *)
       (v \in AppliedSet)  <=>  (Mirror[v] \in NotAppliedSet)
       /\ (v \in ConflictSet) <=> (Mirror[v] \in ConflictSet)

(***************************************************************************)
(* Status predicates                                                        *)
(***************************************************************************)

IsApplied(v)  == v \in AppliedSet
IsConflict(v) == v \in ConflictSet
IsNotApplied(v) == v \in NotAppliedSet

(*
  MirrorMatch(v) corresponds to match(Mi(v)) = True for the mirror node of v.
  Since MatchSet is defined over the original vertex set, we test membership
  for Mirror[v].
*)
MirrorMatch(v) == Mirror[v] \in MatchSet

(***************************************************************************)
(* Edge classification: Safe vs Violated                                   *)
(***************************************************************************)

(*
  The six scenarios in the paper are phrased over edges e = (v, u) with v
  depending on u. We encode them as predicates over (v, u).

  Safe scenarios (Equations (1)-(4) in the paper):

    (1) v ∈ V_A, u ∈ V_A
    (2) v ∈ V_A, u ∉ V_A, match(Mi(u)) = True
    (3) v ∈ V_C, u ∈ V_A
    (4) v ∈ V_C, u ∉ V_A, match(Mi(u)) = True

  Violated scenarios (Equations (5)-(6)):

    (5) v ∈ V_A, u ∉ V_A, match(Mi(u)) = False
    (6) v ∈ V_C, u ∉ V_A, match(Mi(u)) = False

  All scenarios not detected as safe are treated as violated in the algorithm.
*)

SafeEdge(v, u) ==
  \/ (IsApplied(v) /\ IsApplied(u))                         \* (1)
  \/ (IsApplied(v) /\ ~IsApplied(u) /\ MirrorMatch(u))      \* (2)
  \/ (IsConflict(v) /\ IsApplied(u))                        \* (3)
  \/ (IsConflict(v) /\ ~IsApplied(u) /\ MirrorMatch(u))     \* (4)

ViolatedEdge(v, u) ==
  \/ (IsApplied(v) /\ ~IsApplied(u) /\ ~MirrorMatch(u))     \* (5)
  \/ (IsConflict(v) /\ ~IsApplied(u) /\ ~MirrorMatch(u))    \* (6)

(*
  Edge-level classification, as sets of EDGES.
*)

SafeEdges ==
  { e \in EDGES :
       LET v == e.from
           u == e.to
       IN  SafeEdge(v, u)
  }

ViolatedEdges ==
  { e \in EDGES :
       LET v == e.from
           u == e.to
       IN  ViolatedEdge(v, u)
  }

(*
  In the intended algorithm, every edge encountered during SDG traversal is
  either safe or violated (never both).
*)
EdgePartitionInvariant ==
  /\ SafeEdges \cap ViolatedEdges = {}
  /\ \A e \in EDGES :
       LET v == e.from
           u == e.to
       IN  SafeEdge(v, u) \/ ViolatedEdge(v, u)

(***************************************************************************)
(* Violated vertices and DCBs                                              *)
(***************************************************************************)

(*
  ViolatedVertices: the set of vertices whose DCBs are considered violated by
  WizardMerge. The paper's algorithm also marks:

    - v, u, Mi(v), Mi(u) whenever an edge (v, u) is in a violated scenario, and
    - all conflict vertices as violated, regardless of edge classification.

  We encode that directly.
*)

ViolatedVertices ==
  (*
    Vertices that participate in at least one violated edge, either directly
    or through their mirror. This matches "mark v, u, Mi(v), Mi(u) as violated".
  *)
  { w \in VERTICES :
      \E e \in ViolatedEdges :
        LET v == e.from
            u == e.to
        IN  (w = v) \/ (w = u) \/ (w = Mirror[v]) \/ (w = Mirror[u])
  }
  \cup ConflictSet

(*
  SafeVertices: vertices that are not violated. This is a useful derived set
  for reasoning about which DCBs can be safely auto-applied.
*)
SafeVertices ==
  VERTICES \ ViolatedVertices

(***************************************************************************)
(* "Properly handled" merges                                               *)
(***************************************************************************)

(*
  GoodMerge: a merge whose dependency structure is fully consistent and does
  not contain any violated edges. In such a configuration, WizardMerge would
  detect no violated DCBs beyond conflicts (which we treat as violations to
  be resolved manually).
*)
GoodMerge ==
  ViolatedEdges = {}

(*
  Stronger variant: in a good merge, no applied vertex depends (transitively)
  on a violated or non-applied unmatched definition. This is not enforced as
  an invariant here, but can be used as a property to check on concrete
  instances.
*)

(***************************************************************************)
(* Trivial temporal structure for TLC                                      *)
(***************************************************************************)

(*
  We model a single-shot computation: there is no dynamic evolution of the
  graph in this abstraction. All relevant structure is encoded in CONSTANTS,
  and all operators above are purely definitional.

  To allow TLC to run, we introduce a dummy state variable and a trivial
  transition relation. In practice, you instantiate the CONSTANTS via a
  config file, and then check invariants like EdgePartitionInvariant,
  GoodMerge, and properties over ViolatedVertices/SafeVertices.
*)

VARIABLE dummy

Init ==
  TRUE

Next ==
  UNCHANGED dummy

Spec ==
  Init /\ [][Next]_<<dummy>>

(***************************************************************************)
(* Example invariants to be checked by TLC                                 *)
(***************************************************************************)

Inv ==
  EdgePartitionInvariant

THEOREM Spec => []Inv

(***************************************************************************)
(* Pull Request Resolution Specification (Phase 1.2)                      *)
(***************************************************************************)

(*
  This section extends the core merge specification to model the PR resolution
  workflow. It describes how WizardMerge processes GitHub pull requests to
  identify and resolve conflicts across multiple files.
*)

CONSTANTS
  (*
    PR_FILES: the set of all files in the pull request
  *)
  PR_FILES,
  
  (*
    FileStatus: maps each file to its modification status in the PR
    Possible values: "modified", "added", "removed", "renamed"
  *)
  FileStatus,
  
  (*
    BaseSHA, HeadSHA: commit identifiers for base and head of the PR
  *)
  BaseSHA, HeadSHA

(*
  A file is resolvable if it was modified (not removed) and we can fetch
  both its base and head versions.
*)
Resolvable(f) ==
  FileStatus[f] \in {"modified", "added"}

(*
  PR_MergeResult: for each file f in PR_FILES, we compute a merge result
  using the three-way merge algorithm. This is a function from PR_FILES
  to merge outcomes.
  
  Possible outcomes:
    - "success": file merged without conflicts
    - "conflict": file has unresolved conflicts
    - "error": failed to fetch or process file
    - "skipped": file was removed or not applicable
*)

VARIABLE PR_MergeResults

PR_Init ==
  PR_MergeResults = [f \in PR_FILES |-> "pending"]

(*
  Process a single file by applying the three-way merge algorithm.
  This abstracts the actual merge computation but captures the key decision:
  whether the file can be auto-resolved or requires manual intervention.
*)
ProcessFile(f) ==
  /\ PR_MergeResults[f] = "pending"
  /\ IF ~Resolvable(f)
     THEN PR_MergeResults' = [PR_MergeResults EXCEPT ![f] = "skipped"]
     ELSE \/ PR_MergeResults' = [PR_MergeResults EXCEPT ![f] = "success"]
          \/ PR_MergeResults' = [PR_MergeResults EXCEPT ![f] = "conflict"]
          \/ PR_MergeResults' = [PR_MergeResults EXCEPT ![f] = "error"]

(*
  PR completion: all files have been processed
*)
PR_Complete ==
  \A f \in PR_FILES : PR_MergeResults[f] # "pending"

(*
  PR success metric: percentage of files successfully merged
*)
PR_SuccessRate ==
  LET successful == {f \in PR_FILES : PR_MergeResults[f] = "success"}
  IN  Cardinality(successful) * 100 \div Cardinality(PR_FILES)

(*
  PR resolution quality property: a "good" PR resolution is one where
  all resolvable files are either successfully merged or marked as conflicts
  (no errors in fetching or processing).
*)
GoodPRResolution ==
  \A f \in PR_FILES :
    Resolvable(f) => PR_MergeResults[f] \in {"success", "conflict"}

PR_Spec ==
  PR_Init /\ [][(\E f \in PR_FILES : ProcessFile(f))]_<<PR_MergeResults>>

PR_Invariant ==
  PR_Complete => GoodPRResolution

THEOREM PR_Spec => []PR_Invariant

=============================================================================
