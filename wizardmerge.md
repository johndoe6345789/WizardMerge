# WizardMerge: Save Us From Merging Without Any Clues

**Authors:** Qingyu Zhang, Junzhe Li, Jiayi Lin, Jie Ding, Lanteng Lin, Chenxiong Qian

> This markdown version condenses the contents of `wizardmerge.pdf` into a clean, accessible summary. It preserves the
> paper's structure, key ideas, and reported results while omitting PDF-specific artifacts that appeared in the original
> automated extraction.

## Abstract

Modern software development relies on efficient, version-oriented collaboration, yet Git's textual three-way merge can
produce unsatisfactory results that leave developers with little guidance on how to resolve conflicts or detect incorrectly
applied, conflict-free changes. WizardMerge is an auxiliary tool that augments Git's merge output by retrieving code-block
dependencies at both the text and LLVM-IR levels and surfacing developer-facing suggestions. In evaluations across 227
conflicts drawn from five large-scale projects, WizardMerge reduced conflict-handling time by 23.85% and provided
suggestions for over 70% of code blocks potentially affected by the conflicts, including conflict-unrelated blocks that
Git mistakenly applied.

## 1. Introduction

Git's default line-oriented three-way merge is fast and general, but it ignores syntax and semantics. Developers therefore
frequently encounter merge conflicts or incorrect, conflict-free merges that still alter behavior. Prior structured and
semi-structured merge tools reframe the problem around AST manipulation but still leave developers without guidance when
conflicts arise. Machine-learning approaches can suggest resolutions but depend on specialized training data, introduce
length constraints, and may not match developer intent. WizardMerge addresses these gaps by guiding developers toward
conflict resolution rather than automatically rewriting code, highlighting both conflicting and potentially affected
non-conflicting regions.

## 2. Background: Git Merging

Git identifies a merge base, aligns modified code blocks from each side, and treats each modified segment as a Differing
Code Block (DCB). Conflicts occur when both sides touch overlapping regions; non-conflicting DCBs are applied directly but
may still change behavior in subtle ways. Developers therefore need insight into how DCBs depend on one another and which
blocks merit closer inspection during reconciliation.

## 3. Design of WizardMerge

WizardMerge combines Git's merge output with LLVM-based static analysis to illuminate dependencies among code blocks. The
high-level workflow is:

1. **Metadata collection:** Compile each merge input with LLVM to gather intermediate representation (IR) and debug
   information without adding custom build steps for large projects.
2. **Dependency graph generation:** Build overall dependency graphs from LLVM IR, aligning Git's DCBs with graph nodes to
   capture relationships across both text and IR levels.
3. **Group-wise analysis:** Partition DCBs into relevance groups so that developers can triage related changes together
   rather than in isolation.
4. **Priority-oriented classification:** Score and order DCBs based on dependency violations or potential risk, helping
   developers focus on code most likely to be affected by the merge.
5. **Resolution suggestions:** Surface actionable hints for resolving conflicts and flag conflict-unrelated blocks that Git
   applied but still require human attention.

## 4. Evaluation

WizardMerge was evaluated on 227 conflicts from five large-scale projects. Key findings include:

- **Efficiency:** Average conflict-handling time decreased by 23.85% compared to baseline Git workflows.
- **Coverage:** WizardMerge produced suggestions for more than 70% of code blocks potentially impacted by conflicts.
- **False-safety detection:** The tool identified conflict-unrelated blocks that Git applied automatically but that still
  demanded manual review.
- **Comparison to ML approaches:** Machine-learning-based merge generators struggle with large codebases due to sequence
  length limits and generalization challenges; WizardMerge avoids these constraints by relying on static analysis rather
  than learned models.

## 5. Limitations and Threats to Validity

- WizardMerge depends on successful LLVM compilation of both merge inputs; projects that cannot be built or require
  non-standard toolchains may limit applicability.
- Static analysis provides conservative approximations and may miss dynamic dependencies, so developer judgment remains
  essential.
- The evaluation focuses on a curated set of conflicts; broader studies could further validate effectiveness across diverse
  languages and project types.

## 6. Conclusion

WizardMerge augments Git's textual merging by revealing dependency-aware relationships among differing code blocks and
prioritizing developer effort. By coupling Git merge results with LLVM-based analysis, it shortens conflict resolution time
and highlights risky, conflict-unrelated changes that would otherwise slip through. Future work includes expanding language
coverage, refining prioritization heuristics, and integrating the tool more deeply into developer workflows.
