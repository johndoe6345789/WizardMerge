# WizardMerge Research Paper

*Extracted via OCR from paper pages in `docs/pages/`*

**Note**: This document contains OCR-extracted text which may have some errors. The original page images are preserved in `docs/pages/` for reference. For critical information, please refer to the original images or the published paper.

---

## Page 1

Jul 2024

3

SE]

rXiv:2407.02818v1 [c

WizardMerge - Save Us From Merging Without Any Clues

QINGYU ZHANG, The University of Hong Kong, China
JUNZHE LI, The University of Hong Kong, China

JIAYI LIN, The University of Hong Kong, China

JIE DING, The University of Hong Kong, China

LANTENG LIN, Beijing University of Posts and Telecommunications, China
CHENXIONG QIAN’, The University of Hong Kong, China

Modern software development necessitates efficient verson-orente collaboration among developers, While Gite the most popular
version control system, it generates unsatisfactory version merging results due to textual-based workflow, leading to potentially
‘unexpected results in the merged version of the project. Although mumercus merging tals have ben proposed for improving merge
results, developers remain struggling to reslve the conflicts and fix incorrectly modified code without cues, We present WizaxnMaxc,
an auxiary tol tha leverages merging results from Git to retrieve code black dependency on text and LLVMAR level and provide
suggestions for developers to reeolre errors introduced by textual merging, Through the evaluation, we subjected WizannMenoe to
testing on 227 conflicts within five large-scale projects. The outcomes demonstrat that WazannMence diminishes confict merging
time costs, achieving a 28.85% reduction. Beyond addressing conflicts, WizanpMancx provides merging suggestions for over 70% of
the code blocks potentially affected bythe conflicts Notably, WizarnMancr exhibits the capability to entify confic-umelted cade
‘blocks that require manual intervention yet are hamnfally aplie by Git during the merging.

(CCS Concepts: «Software and its engineering + Software configuration management and version control systems; Auto-
‘mated static analysis; Theory of computation —+ Program analysis.

Additional Key Words and Phrases: Version contrl, Code merging Static analysis

ACM Reference Format:
(Qingyu Zhang, Joke Li, Jai Lin, le Ding, Lanteng Lin and Chenxiong Qian, 2024, WinardMerge- Save Us From Merging Without
Ay Ches. 1, 1 Quly 2024), 29 pages. htps/dol-org/10.145/nnnnoan.nnnnmn

1 Introdu

[As the most globally renowned version contro system, Git [17] has been helping withthe development and maintenance
cof millions of project. To automatically merge the codes from two different version, Git utilizes a Three-way Merge
Algorithm [12] a the default strategy on the text level. The textual-based merging is general and suitable for various

*Cormapending author

‘Atha Contac Ifomation Qingy Zhang, The Univers of Hong Kong, HongKong, China stanqy@eomnecthku bk; unbe Li, Tthe University
HongKong, Hong Kong, hin, icant say Lin, The Univers of Hong Kong Heng Kan, chi inyOl@cennet bleh e Ding,
“The Unbrerety of Hang Keg, Hong Kong, Chin, ing 9455 ilo com: Lanteng Lin, eng Unters of Pst and Teecommunitions, Being
Chins ntern@bupt dc; Chemcong Qin, The Unitersty af Hang Kang, Hang Kong Chia cance

‘Permission tombe dgtlorkar copes flr pr this wor for personal or hasrom te i granted without fe provided hat copies are not
made or dt far profit commercial advntge and tht cope er thine andthe full tation on the it page Copyright fer component
cfthiewodk avnedby others than the author) mart be honored. Abtacting with cei ie permite. To copy other, or epblih 0 pst
server orto redtubate oli, egies Prior spec pemisinanl/or ee. aque permission fom peminionn@acm or

(0202 Copyright ld by the owner euthor)- Publi ight een ACM.

Manusrpt bite ACM

Manwsrpt abit ACM 1

---

## Page 2

2 Qingyu Zhang, Junzhe Li, Jiayi Lin, Jie Ding, Lanteng Lin, and Cheniong Qian

types of textiles and the merging tempois proved tobe the fastest. However it simply considealine of code neglecting
all yntax and semantic information, which leads to potential bug in the menged version ofthe project [2,26]

“Tomitigste the shor comingsof traditional textual based merging, multiple merging approsches[4, 523,24, 25,4445]
ae proposed for enhancing the correctness of merging result. These tocs can be clasifed as Structured Merging
and Semi-structured Merging. Structured merging tools [23 25, 4,45] transform the code intended for merging
into an Abstract Syntax Tre (AST), thereby converting the code merging into the merging of edges and nodes onthe
AST [1] Semi-structured merging [4, 5,24], akin to structured menging, designates certain semantically insensitive
AST nodes (eg stings) as text allowing them tobe directly processed bythe text-based merging trtegy during the
merging process}

Despite structured and semi-structured merging tools facilitating code merging, challenges arte when it is unclear
‘which version ofthe code should take precedence. This uncertainty leads to merging confit. In the context of large-
scale software projects, conflicts can become a source of frustration for developers, as they must dedicate significant
effort to resolve the inns that arise from merging before they can releat the inal merged version [2,22]. Addressing
these conflict reslutionshas given rise to merging assistance systems, These systems are designed with objectives ach
ss sclecting the most auitable developers to resolve conflicts (7, prioritizing the resohiton order for conflicts [23nd
ven automatically generating alterative conflict reachtions [44]. With the rapid advancements in machine learning
technologie in recent year, researchers have turned to machine leaning approaches [3, 11-13, 15,342]. These
systems ae trained using extensive datasets containing merge conflict aenatios and can provide predictions onthe

most suitable resolution strategy for each conflicting portion of code.

While these researches have made noteworthy contributions to code merging, they still exhibit the following
limitations. Firstly, structured [4, 23, 35, 44, 45] and semi-structured [5, 34] still generate conflicts when the direct
merging between two AST noes filed, and cannot assist developers in resolving them. Secondly, conflict resolution
assistance systems [9, 38, 44] focus solely on resolving conflicts, overlooking potential errors introduced by incorrectly
applied but confit-fice code. Though machine learning approaches [3, 11, 12, 15,38, 43] can automate conflict merging,
the functionality of the resulting code may not ensure alignment with developers’ expectations. Additionally, machine
learning methods necessitate specific code datasets for pre-training, which raises universality issues when applied to
more diverse code projects. Moreover, the developers often have unpredictable operations on resolving conflicts, where

the auto-generated conic resolution may fil to meet the expectations of them.
‘To address these Imitations, we introduce novel approach aiming to aid developers in handling complex code-
merging scenarios. Recognizing that developers have unpredictable ultimate needs, our approach focuses on
rouping conflicts by elevance and providing prioritized suggestion for resolving conflict rather than resolving them
directly. Our approach also detect the incorrectly applied codes as they can he easily neglected but cumbersome to
Dhan for developers To realize these design goals, cur approach requires a thorough understanding ofthe dependency
relations in the two merging candidate versions. Firstly, we perform definition dependency graph constuction on
LIVM Intermediate Representation (IR) [25] and ullize definition indicators extracted from Clang [25] to indicate the
definition name and ranges in the source file. Following graph construction, our approach analyzes the Git merge
outcomes that present all the conflicts and modifed code blocks. Then, we align the definitions in the source Se with
code snippets in Git's outcomes to map from LLVMIR to the Git merge results. Given the dependency graph analysis
nd aligned Git merge results, we further conduct an edge-tenstive algorithm to detect incomecty applied codes, which
wwe deem a conflicts since they require developer atenton as wel. nally, we assign priorities to these conilcted
nodes and produce merging suggestions
Manu beiteo ACM.

---

## Page 3

WizardMerge - Save Us From Merging Without Any Clues 3

We implement a prototype WiZARDMERGE for C/C++ projects ae most large-scale projects are written in C/C++ [20].
We evaluate WizanDMERGE on 227 conflicts from five popular large projects. The outcomes demonstrate that Wiz-
ARDMENGE diminishes conflict merging time costs. Beyond addressing conflicts, WIZARDMERGE provides merging
suggestions for most of the code blocks affected by the conflicts, Moreaver, WiZARDMERGE exhibits the capability to
identify conflct-unrelated code blocks which should require manual intervention yet automatically applied by Git.
In summary, we make the following contributions:
‘© We proposed a novel approach to infer the dependency between text-level code blocks with modification,
‘¢ We proposed a remarkable methodology, which aims to pinpoint code blocks that have been automatically applied
by Git but introduce unexpected results,
‘© We implemented a code merging auxiliary prototype named W1zannMencx based on the approaches and evaluated
‘the system on projects with large-scale code bases. We will open source WiZARDMERGE and evahuation datascts at
Ittpes//github.com/aa/bb,

2. Background
2. Git Merging

Variant A_(VA) Base Variant B_(VB)
Tine | we = Stmea0p Vi = Semis Vi = Stati()3
Line 2| Wi-= Semt2(07 V5 Va = Semt2(VA)y VE = Stat 2(VATy
Line | “Wa= seme (v2) V3 = Semt3(Vi, V2)p VS Stat9(Vi, V2)p
“Line 4| V3-= Stme3(Vi, V2)p Var = Semt2(V2)3 V5-= Stat a(V2, V3)5
Line | -W5-= Stmea(v2, V3); Vs = Stata(v2, V3)z Ve = Stat5 (Vi)
Line 6| We= Seat5(Vi)3 Ve = Stmt5(VI)y We si 2(5)y
(Zine | WWrs See, Wey] 0e =o [V7 = Sem (V6);
‘Merged Commit
Va = Semi;
a & [rs seme2(2y5 |
— Vas Sem 2(V2)5 !
fo Fis sea |
[v5 semea(v2, va); —| |
Ve = Sem5(VI); Developer
<Confict>

Fig. 1. Three-way merging workflow of Git

Git is a free and open-source distributed version control system designed for eficent handling of projects ofall
sizes [17] Git merge, as a key operation, facilitates the integration of changes between versions and is often ured to
consolidate code changes, bug fixes, or new features [18]. Git supports multiple textual-based merging algorithms [7].
‘Take the default and most commonly employed algorithm, Three-way Merging Algorithm [42] as an example, the
‘worlllow is shown in Figure 1. When the developer tres to merge two commits (ie, Cs Variant A (VA) and Cs Vai
B (VB) inFigure 1), Gt first travertes the commit-graph back until it reaches the nearest common ancestor commit (i.e,
Cz,mamed Base). Then, all source code blocks of each newer version are mapped to the corresponding code blocks in
base versions, and the modified code blocks of either VA or VB are regarded as Diff Code Blocks (DCBs). A DCB in a

Manat submited to ACM.

---

## Page 4

4 Qingyu Zhang, Junzhe Li, Jiayi Lin, Jie Ding, Lanteng Lin, and Cheniong Qian

variant may contain zero or multiple lines of code, indicating the removal ofan existing line or modification/addition of
‘multiple continuous lines. ARer the code matching stage, Git will iterate each code block tuple, and determine which
commit should be applied according to the following rules:

« Ifcode blocks of VA and VB remain the same as the code block of Base, or both of them are changed in the same
‘way, inherit the code block from VA. In Figure 1, both VA and VB remove the definition of w (ie, deleting line 4 of
Base). Due to identical modifications atthe same locations, Gt applies these changes. Ibis noteworthy that even for
code removal, VA and VB maintain a pair of empty DCBs, symbolizing the deletion of lines.

‘* Ifonly one code block of ether VA or VBis changed, Git applies this code block, and mars this code block and the
corresponding code block in the other variant as DCB. In Figure 1, VA alters lines 1-2 and introduces a new line at
3. Git's code-matching algorithm identifies these modifications, designating lines 1-3 of VA as DCB Zand lines 1-2 of
‘Bas DCB2. Since only DCB 1 changes, Git applies it to generate the code forthe Merged Commit at lines 1-3,

‘* Ifboth code blocks from VA and VBare changed, and the modifications are different, Git marks both code blocks as
DCBs.As Gt cannot decide which one should be applied it emis a conflict containing the two DCBs. In Figure 1, VA
and VB independently modify the definition of v7. Git detects these alterations, marking line 7 of VA as DCB Sand
line 6 of VB as DCB 4. Since these two modifications der, Git cannot determine which one to apply. Consequently,
Git signals the inconsistency between DCB 3 and DCB 4, prompting developers to resolve this conflict.

// int a= 1; int a = 1;
ya [ant b= 25 int b = 25 Base
int c = 1+ b; int c = 1+ b;
printf(“%d", c); || printf (“%d”, c)5
int a = 1; // int a
int b = 25 int b = 25
VB | int c= a+b: intc=a+b; |Mersed
printf("%d", ¢); || print#(“%d”, c)5

Fig. 2. An example ofa clean but incorect Git mergewith the three-way merging algorithm

2.2 Limitations of Existing Approaches
Despite being the most widely used code merging tool currently, Git merge faces the following two challenges: (Challenge
1) cannot ascertain developers’ expectations fer code functionality: Consequently, when both versions modify the same
code, Git treats this discrepancy as a conflict, necessitating manual intervention for resolution. We discussed how the
conflicts are generated in §2.1 (Challenge 2)t cannct ensure the correctness of successfully applied and conflict free code
blocks. Even when Git successfully merges two versions without conflicts, the merged code may be generated with
new bugs, Figure 2 shows an incorrect merging scenario. Only the frst line of VA and the third lin of VB are changed,
Manat abit to ACM.

---

## Page 5

WizardMerge - Save Us Prom Merging Without Any Clues 5

Using Git merge, the two versions of code can be easily merged without any conflict (ie. the first line applied from.
VA and the second line applied from VB). Unfortunately, in the merged version, the definition of variable ahhas been
removed, which causes a compilation error atthe third lin. These DCBs enforced by Git without conflict, yet resulting,
in unexpected results, are termed Violated DCBs. Given that these violated DCBs are not flagged as conflicts and
are automatically incorporated by Git merge, they tend to introduce concealed programming errors into the merged
code. In the most severe scenarios, these error-prone code segments might seemingly execute successfully but could
potentially give rise to security issues post-deployment (22, 6]

(Over the years, researchers have concentrated on enhancing the quality of merged code and conflict resolution.
However, these two basic limitations have not yet been filly resolved. On the one hand, structured (23, 35,44, 45] and
sractured (4, 5,34] merging still report conflicts when the sume AST node is modified simultaneously by two
‘merging candidate branches. On the other hand, the conflict resolution assistance system [33, 44] and machine learning,
approaches [3, 1-13, 15, 38, 43] only consider conflicts reported by the merge tool, ignoring all code snippets that
have been applied, Additionally, automatic software merging tools [3-5, 11-13, 15, 23, 4, 35,38, 43-45] neglect an.
soential observation: developers have unpredictable ultimate needs, therefore cannot filly overcome Challenge 1
‘To better illustrate thie, we provide an example of implementing the Fibonacci sequence in Figure 3. Two versions, A
and B, calculate the Fibonacci sequence using recursion and iteration, respectively. When attempting to merge these
‘versions, a conilict arises, and two developers resolve it. Although both developers decide to use the iterative approach
from version B to avoid stack overflow for large values of n, they implement different strategies for handling negative
values of. Developer A, inspired by version A, uses an unsigned definition for nto eliminate negative values (line 1
in resolution A), In contrast, Developer B opts to return zero when mis less than zero (line 3 in resolution B). Since
developers’ decisions on conflict resolution are unpredictable, automatic software merging tools often fail ta produce

an acceptable resolution in such cases.

‘esionA Resolaion A
Fine Fab(unsigned n) vetoes fine Fib(unsigned n) {
if (ned) { Uataptenoa sch oven] Ant @ = 8, B= 1, Cb
otety Unipefa ncaa ee a ree
} else ( for (int {= 25 1 cen 44)
Fetuon fib(n-1) + fib(n-2); cearbasd, bac
} return b;
a x oe y
Nersion B Coniiet Resolution B
jint fib(int n) { Jint fib(int n) {
int a= 0, b= 1, int a= 0, b= 1,
AF (n == 0) retutn 95 4F (nce 6) return
for (Ant d= 25 4 <= 5 444) for (int 1 = 2; <= n; S44)
edict aot) boc nf egdcty ao boc
return bs atid ck oon] retUPn 5
b Koumtoo asec Ly

Fig.3. An example of different resolutions tothe same conflict

Furthermore, machine learning systems [3, 1-13, 15,38, 43] are limited in their ability to resolve conflicts in large
codebases due to input and output sequence constraints. However, these conflicts typically do not significantly impact
Manat abit to ACM.

---

## Page 6

6 Qingyu Zhang, Junzhe Li, Jiayi Lin, Jie Ding, Lanteng Lin, and Cheniong Qian

developers’ time. For example, even MergeGen [13], the most advanced system available, filed to achieve satisfactory
results when applied to the large-rcale targets evaluated in 4. Upon completing MergeGen's pre-training, we discovered
that it produced no correct outputs, achieving an accuracy of 0%. Ths failure can be attributed to MergeGen’s default
input and output sequence length limitations of 500 and 200 BPE, respectively [13]. These limitations are excessively
stringent for large-scale project source code, leading to truncation of code beyond the prescribed lengths. However, the
end ofthe truncated code is usualy far sway from where conflict oceurs and MergeGen cannot master the ability to
resolve the conflicts given the trimmed training and testing cts, Attemptstorelaxthere length limitations areimpractical
due to increased GPU memory consumption during training. Moreover, while machine learning techniques excel in
generating merge results they may not abvaye align perfectly with developers’ expectations. Although MergeGen's
cvalustion results demonstrate superior conllict cde matching precision and accuracy om a line of code basis compared
to previous works [13] it stil falls short of espectations in some cases, with average precision and accuracy rates of|
71.83% and 69.936, respectively.

3. Design
Given the challenges outlined in the preceding section, we have identified two key insights. Addressing Challenge
{we recognize that the expected outcomes of memges are subjective and open-ended. Consequently, our approach
‘emphasizes guiding developers in resolving conflicts rather than prescribing resolutions. To tackle Challenge 2, itis
imperative to analyze all modified code. To this end, we present Wiza RDMERGE, a novel conflict resolution assistance
system, which provides developers with clues and aids in conflict resolution. Moreover, WizanpMence emphasizes all
‘modified code snippets, not just conflicts, enabling it to uncover hidden discrepancies and preempt potential errors. A
detailed design of WizanpMance will be presented in the subsequent subsections.

31 Overview

eeeers rc
vee LJ omnis
ee
OF
con

aes ont npn
sort :
sountenon) Las

Manat sated Fig. 4. system overview of WizanoMence

---

## Page 7

WizardMerge - Save Us From Merging Without Any Clues 7

Figure 4 depicts the eystem overview of WizaRDMERGE. WizaRDMERGE first consumes compilation metadata from
the two merge candidates through LLVM[Zs], WizaRbMEncr employs LLVM based on two insights: i) For large
projects, code is compiled before a new version release to ensure compilation success, allowing WIZARDMERGE to
collect metadata without introducing additional merging steps; ii) LLVM Intermediate Representation (I) provides
1 robust static analysis framework and comprehensive debug information, facilitating W1zaRDMERGE in performing
definition dependency reasoning. To bridge LVM IR analysis with merge results from Git, the compilation metadata,
also encompasses definition indicators. A definition indicator serves asthe representation of named definition section
at the source code level, indicating the range and name of a definition. For example, a definition indicator of afunction
records its Function name, along with its stat and end line numbers in the source code file. Subsequently, To identify
‘relationships between all modified codes, WizaRDMunGE advances to the Dependency Graph Generation stage.
Instead of focusing only on conflicts, dependencies forall definitions are analyzed. WizaRDMERGE creates vertices for
the definitions and establishes edges between vertices according to the dependency relationships of definitions analyzed.
from LVM IRs. Following this, the Overall Dependency Graph (ODG) is generated to represent the dependencies
‘among all the definitions in both candidate versions of the target project. To attach its textual results with I-level
dependency information, WizaRDMERGE aligns Git merge’s results and ODG to match DCBs with vertices in ODG.
through Graph-Text Alignmont. This stage is capable of mapping DCBs to the corresponding definition nodes in each,
ODG, refining definition nodes into DCB nodes, and transforming definition dependencies into DCB dependencies.
‘To rectify errors introduced by Git merge, WizanDMuncE employs an edge-sensitive algorithm in the Violated DCB
Detection stage, detecting both conflict-related and conflict-unrclated violated DCBs. These violated DCBs and conflicts
are categorized, prioritized based on the dependency graph, and presented as suggestions to developers to assist in

resolution,
42 Dependency Graph Generation

‘WizarpMunen first constructs ODG based on the LLVM IR and definition indicator, WizanpMunce currently supports
the following types of nodes and edges

Graph node Subsequently, WizaRDMERGE generates nodes for each named definition with the corresponding indicator.
Considering the types of definitions, WizaRDMERGE creates three types of nodes based on the indicator information:
‘Type Node (TN) represents composite types, eg, structure, class, type alias, and enumeration; Global Variable Node
(GN) represents global variable; Function Node (FN) represents function.

Graph edge. Bach named definition may depend on other definitions within the project. For instance, a function,
‘might utilize a global variable or return a pointer with a specific structure type. Inthe ODG, these usage dependencies
‘among definitions are deduced from LLVM IRe and represented by the edges linking one node to another. The types of
edges are defined by the types of their two connected nodes, An edge of type A‘B represents a dependency from node
Ato node B. In WizannMuncr, five classifications of edges are supported: TN-TN, GN-IN, FN-TN, FN-GN and FN-EN,

[Note that WizaRnM nce will never generate an edge from TN to FN, even though member functions may be defined
‘within a composite type. Instead, WIZARDMERGE creates TN for all functions irespective of whether the function is
standalone or nested within another scope. Fan FN represents a nested function defined inside a composite type, an
N-IN edge is built to represent the dependency from the function tothe specific typ.

1m Generation

43 Resolving Suge:

In this stage, WizaRDMEnGE extracts DCB information from Git merge to aid in the analysis. Then, merging suggestions
‘can be generated based on the analysis results
Manat abit to ACM.

---

## Page 8

5 Qingyu Zhang, Junzhe Li, Jiayi Lin, Jie Ding, Lanteng Lin, and Cheniong Qian

43.1 Graph-Text Alignment. The merge results cannot be directly linked to the corresponding node in ODG. This is
primarily because these results are raw texts that provide no additional clues for program analysis. In the raw reauls, a
single large DCB may cover multiple nodes and a single node may encompass several DCBs. This N-to-N relationship
complicates the analysis of the graph.

Before GaphTetAigament Afr Giaph Te Aignnent
amare oT =a
ana >| Start Lime: L1 + Start Lime: LS
Sty eam) |e
# Eade Se | | ERS
i mat ee
Sino ‘Name: Named Def 2 Dep: (12.15)
Bis =
> SaetineLs Smee
Code File with DCBs End Line: L7 ‘End Line: L7
Ein

Fig.5. Example alignment between node and OCB tet.

‘WrzarnMencr initially associates cach DCB with its corresponding definition indicators. To identify all nodes
contained in the file where the DCB is situated, WizaRDMEnGE extracts the nodes whose range intersects with the
DCB range. Ino matched node is found, it indicates that the DCB is notin a definition (eg, newlines or comments)
and will not be further analyzed by WizanDMEncr. Treating each DCB as a ‘line segment’ its start and end line
smumbers represent the coordinates ofthe line segment’s two endpoints on the coordinate axis. Similarly, each definition
indicator of a node is considered as a unique "coloy’ The range ofeach node signifies a specific coordinate interval that
is colored with the definition represented by the node. Thus, etreving the node during each DCB traversal can be seen
as. color query forthe interval [start_Iine, end_Line], and interval modification and information maintenance can be
cflicently managed using line segment trees. Fora target project containing N lines of code, each ‘coloring” and ‘eolor
query" operation can be achieved with a ime complexity of O(logaN) [10]. WizanpMance subsequently logs all DCBs
corresponding to each node, organizing them in ascending order of line numbers. The node is then subdivided into
finer-grained nodes. The ranges of these finer-grained nodes are updated concurrently bared on the original node and
‘their corresponding unique DCBs. Fine-grained nodes belonging to the same definition will be connected sequential
in range order, which implicitly expresses the dependency between code blacks. This mechanism ensures thateach
node in ODG aligns with at most one DCB, effectively preventing any matching confusion within the ODG.

‘Taking Figure 5 as an instance, there are two definitions: Named Def 1, Named Def2. Before alignment, the nodes
are created with their range information, Let's consider two DCBs associated with these definitions ([L2,L5] and
[L6,L7)). To establish a one-to-one mapping between DCBs and nodes, each node can be divided into multiple ones,
‘while unaffected lines are excluded as they do not influence the merging. For DCB (L2,L5], WizannMuno detects that
[22,13] belongs toNamed Def 1 and [L5,L5] belongs to Named Def?2. Consequently, a new node starting from L2 to L3
vwillreplace the original node for Named Def 1. As (L1,L1] remains unchanged it will be discarded. Furthermore, new
node from L5 to LS is also formed since [L6,L7], representing another portion of Named Def?, corresponds to the DCB
[6,17]. To indicate the code dependency from lines to previous lines within the same definition, an additional edge
from node [L6,L.7] to node [15,15] is established.

‘As WizanpMunor ie tasked with managing two code versions (VA and VB), some nodes in the ODG of VA must
‘possess a corresponding mirror node in the ODG of VB. WizaRbMEnGE also constructs a mapping of each node to its
Manat submit to ACM.

---

## Page 9

WizardMerge - Save Us From Merging Without Any Clues 9

matched node in the other version's ODG by matching the name of the definition and the DCB metadata linked tothe
node, Note that if one node is aligned with a DCB, it must have one unique mirror node with the other DCB in the
same DCB pair Ifthe node has a matched node, the matched node must be the mirror node and the definition name is
consistent with the current node. The matched node results contribute to the violated DCB detection via dependency
‘missing determination. Besides, the ODG's size can also be shrunk if there are nodes not attached to any DCBs, which
‘means they are not changed during merging, After the node deletion, the newly generated graph ie called Shrunk:
Dependency Graph (SDG).

4.32 Violated DCB Detection. Violated DCBs are the underlying cause of seemingly successful but erroneous merging,
‘To offer developers a comprehensive understanding of the merging process, WIZARDMERGE detects violated DCBs
based on an edge-sensitive algorithm and deems them as conflicts requiring resolution suggestions, Given the SDGs
and node matching relationship, WIZaRDMERGE traverses each edge in the two versions of the SDG separately. Each,
edge, starting from and ending at u, signifies that o relies on u. WizanbMuncz determines which version of the DCB
associated with w and o is ultimately applied in the merge results (ce. fom VA, VB, or conflict). WIZARDMERGE then,
ascertains whether these nodes represent violated DCBs by the applied versions and matching nodes of w and v. We
introduce all the safe (ie. not violated DCBs detected) and violated scenarios as follows.

Symbols Definition. Let the SDGs from VA. and VB be represented at G4 = < V(Gq),E(Ga) > and Gp = <
V(Gs),E(Gs) >, where V indicates the vertices set and E indicates the edges st For each vertex 0 € V, we represent
its mimor node as Mi(e) and use metch(Mi(o)) to indicate whether the mirror node of isa matching node (True or
False) belongs to only one ofthe Vy, Va, and Ve verter sets representing not applied, applied, and confit node sets
respectively. For cach edge e(,u) € Zit represents an edge from to ,indzatng o's dependency on According to
{he construction principles of SDG, there are two base facts:

# Ifo eV, then Mi(o) €Vy, and vice versa.

# Ifo € Vo, then Mi) €Vo.
‘Ascach 0¢ Va must indicate another verter (ie, Mi(e) belongs to Vig, WizanpManas only analyzes the cases where
9 €Vaor0€ Ve to avoid duplicate traversal.

Safe Scenaris. For each o and an edge:
as safe

(0, u), we define the scenarios satisfying any ofthe following four formulas

ve Vawe Va )
1 € Vau ¢ Va,match(Mi(u)) = True )
ve VoueVa )

0 Vou ¢ Va,match(Mi(u)
In SDGs, the above formulas can be regarded as an edge-sensitive detection algorithm, which is visualized as depicted
in Figure 6, For Equation 1, fboth w and o are applied, it means the dependency relation between nodes within the
same SDG (as shown in Scenario 1 of Figure 6). By the same token, Scenario 3 alo illustrates such a direct dependency
represented by Equation 3. For Equation 2 Scenario in Figure 6), 9 is applied but wis either not applied (u € Viz) or in
confit status (u € Vc). Nevertheless, if the mirror of u matches w (match(Mi(u)) = True), it means that at least inthe
other SDG, «can find its comesponding dependency applied (i, Mi(u)) although w is not applied, or both versions of
conflicted hold the same dependency for v. To this end, WIZARDMERGE utes a virtual dependency edge from 0 to

Mansi abit to ACM.

rue C)

---

## Page 10

10 Qingyu Zhang, Junzhe Li, Jiayi Lin, Jie Ding, Lanteng Lin, and Chensiong Qian

[Mi(u) to indicate a cross dependency between nodes from different SDGs. Similarly, Equation 4 (Seenario 4) is aleo

regarded as safe with the same reason.

u u
Sxxmenil Semmng
v
u u Mi(u)
Scenario 3 ycenario 6
v v

v
oO Node from Vy oO Node from Vy CO Node from Vo

==» Maiching — Virtual Dependency Edge ——» Dependency Edge

Fig. 6. Edge traversal scenarios visualization

(0,1), all the scenarios not detected as safe arc regarded as violated,

Violated Scenarios. For cach 9 and an edge

scenarios, These scenarios are:

2 € Vasu ¢ Va.match(Mi(u)) = Falee 0
2 € Vo,u¢ Va.match(Mi(u)) = Falze ©

‘The Scenario and Scenario 6 in Figure 6 visualize Equation 5 and Equation 6 respectively. Once w ie not applied or in
conflict, and the mirror node of u does not match it (match(Mi(u)) = False), it means that 0 misses dependency from w
in both SDGs (when u € Vi), or Mi(u) holds a different dependency from u, which does not match 2's requirement
(when w € Vo).

‘WizaxpMence identifies these cases are incorrectly handled by Git merge and marks the corresponding e, u, Mi(0)
and Mi(u) as violated. Note that vertices with conflicts are always treated as violated, regardless of whether they are
encountered in safe or violated scenarios. Finally, the DCB attached to these vertices are also marked as violated.
Manat abit to ACM.

---

## Page 11

WizardMerge - Save Us From Merging Without Any Clues n

YD Mirror,

MDG_Node_Di

MDG_Node_A

i) Not Applied Node!

_ Applied Node

— + MDG Edge
= SDG Edge

Mirror
MDG_Node_C

E+. Mirvor
MDG_Node E~

Fig. 7. MDG example. Each node in MDG contains exactly a pair of mirror nodes in SDG. Each edge i built there fat least one
SDG edge from one SDG nede to another.

4133. Priorty-Oriented Classification. WizaRDMERGE further minimizes the number of nodes that require processing
and constructs a Minimum Dependency Graph (MDG) to emulate the dependencies ofthe merged version of ede.
Inthe MDG, each node will consist of cither an applied node or a confict node and its matching node, and the edges
between nodes will be created based on various SDGs accordingto the applying status. Specifically, while establishing
the MDG, WizannMunce will traverse each conflict node or node associated with violated DCBs, and based on the
applying status of the node, twill switch to the corresponding version of SDG and continue traversing the nodes
in that particular SDG. Particularly, if the node isin conflict status, then the outcoming edges of both the mirror of|
the node and itsef would be iterated. Concurrently, WizaRDMERGE sets up a directed edge between corresponding
nodes for the MDG, relying on the encountered edges during traversal. Utimately, the MDG encapsulates the
dependencies ofthe two versions of code to be merged.

Figure 7 shows an instance illustrating how MDG is built from SDG. Each MDG node is created from a pair of nodes
related to conflict or violated DCBs. SDG node A-Eare from VA and all Mirar nodes are from VB. Starting from A and
its mirror, WzanpMsRox iterates the outcoming edges and finds B is inked toa violated DCB. Then, Wiza RoMERGE
creates a new node MDG_Node_B containing B and its mirror. As Bis not applied while its mirror node is applied,
‘WizannMncr iterates the outcoming edges of the mirror of B and finds a pair of conflict nodes (..,E and Mirror),
which forms MDG Node_E. WizaRoMEncr then iterates the outcoming edges of both B and Mirror because of the
conllct. By the same token, MDG_Node_D and MDG_Node_C are created.

‘WizampMencr then employs the Tarjan algorithm [39] to condense the MDG nodes and eliminate cycles. During
this process, nodes in the same strongly connected component are regarded as having identical provty as these nodes
nay reference each other within the code. In the case ofthe directed acyclic MDG, WizarnMrncr divides the MDG
{nto muiiple subgraphs based on connectivity. All nodes within cach subgraph are grouped, and each node is assigned
« prorty through the topological sorting of the subgraph, Nodes that appear later inthe topological orde signify that
other nodes have greater dependencies on them, while the nodes themselves have fewer dependencies. Consequently,
these nodes are assigned higher priority, enabling developers to address them earlier. In Figure 7, MDG_Node_

Manes bite ACM.

sential

---

## Page 12

2 Qingyu Zhang, Junzhe Li, Jiayi Lin, Jie Ding, Lanteng Lin, and Cheniong Qian

[MDG._Node_D and MDG.Node_E are in the same cyclic of MDG and are thus assigned the same resolution priority as
they depend on each other. After calculating the topological order, Wiz RDMERGE suggests that the DCB resolution
priority should be (MDG Node_C, MDG_Node_D, MDG.Node_, (MDG_Node_B}and {MDG_Node_A\.

4 Evaluation
In this section, we evaluate the effectiveness of W1zARDMERGE, aiming to answer the folowing research questions:

‘¢ RW: Can WizampMuncr help reduce the time consumption of merging conflicts?

‘¢ RQ2: Can WizampMuncr detect violated DCBs related to conflicts applied by Git?

4¢ RQS: Can WizanpMuncr detect violated DCBs unrelated to conflicts applied by Git?

Dataset Collection. We choose Firefox [29], Linux Kernel [30], MySQL [31], PHP [31], and LLVM [25] asthe target
projects because oftheir widespread usage as open-source software. Additionally, they boast extensive code bases and
sare maintained by multiple developers, resulting in repositories that feature intricate commits and merging scenarin.
We collect 22 conflict pairs from Firefox, 43 from Linux Kemel, 17 from LLVM, 46 from PHP, and 99 from MySQL with
1 committing timeframe from 2021 to 2024. For each pair of conflicts, we only focus on the files in C/C++ format and
guarantee they cither have at leat five conflict regions or violated DCBs detected. On average, a canfict contains
152433 lines of codes and i related to 6. files.

Evaluate with State-of the-art (SOTA) Systems. We initially intended to assess W124nDManct alongside other cutting
edge systems. However, JDIME [1], Mastery [45], Spork: [23], SafeMerge [35], AutoMerge [44], InteliMerge [34] and
‘SoMany Conflicts [3] encountered language incompatibility issues within our datasets. Besides, migrating WIZARD-
‘Menor tothe languages these systems support is impractical because as an LLVM-based project, WIZARDMERGE requires
‘the fll support from the LLVM framework, while the compiler front ends of Java [37], Typescript [6], and Javascript [5]
are till under development. Furthermore, FSTMerge[5] purports scalability contingent upon users implementing the
merging engine for specific languages. Unfortunately its C/C++ parsers are either underdeveloped (ie, accommodating
only basic syntax) or unimplemented, rendering it unsuitable for real-world projects In addition, as mentioned in §2.2,
‘the machine learning approaches [3, 11~13, 15,38, 43] meet the sequence length limitation challenge when facing the
large-tcale targets we chose

Evaluation Environment. We evaluated WizanDMEnGE on an AMD EPYC 7713P server with 64/128 cores/threads,
[NVIDIA Geforce RTX 4090 (24GB GPU memory), and 256GB memory, running Ubuntu 20.04 with kernel 54.0,

4.1. Effectiveness on Conflict Resolution,
To adares
(Git menge w/ and w/o WaanpMance) for merging conflict pairs. As WizanDMincr only provides suggestions instead
of resolutions of conflicts, We have to assign two types of technical staff to the manual merging phase: skilled staffs,
researchers with deep understanding of the project, and ordinary staffs, researchers having general understanding of|
‘the project. Initially, the ordinary staff employed WizaRDMERGE to analyze the two versions and performed the merge
utilizing the suggestions provided by WizannManct, Subsequently the skilled staff manually merged the two candidate
versions using Git merge without any additional assistance. We meticulously recorded the number of conflicts that can
bbe analyzed by WHZARDMERGE and the time taken (with precision to the minute) from the commencement of conflict
analysis to their complete resolution.

Manat abit to ACM.

RQ, we conducted a comparative analysis ofthe time consumption associated with two distinct approaches

---

## Page 13

WizardMerge - Save Us From Merging Without Any Clues 13

Table 1. Conflict coverage of WaanoMence. Total Num indicates thetotal numberof conflict in each dataset. Analyzed Num
indicates the numberof conflicts succesfully analyzed by WazanoMence. Cover Rate indicates the ratio ofthe above two columns

Dataset ‘Analyzed Num Total Num Cover Rate
Fire 19 10 px
imax Kernel ® es ss2%5
uve n n 966%
Pa 26 2s onsen,
MySQL pr a e26tm,

[Before comparing the effectiveness of merging w/ and w/o WIZARDMERGE, we first evaluate its ability to arse
conflicts, As shown in Table 1, we calculated the total number of conflicts for each dataset and recorded how many
conflicts can be successfully analyzed by WizaRDMERGE. WizaRDMERGE can give suggestions of more than 80% conflicts
from the five targets. Notice that rome conflicts cannot be handled by WizARDMERGE, we manually dig the reasons
Dbchind the failure. In the design section, we mentioned that WizaabMzncr's analysis is based on LVM IR. However,

in textual merging results, same conflicts are caused by the content of comments or preprocessing instructions (eg.

‘include and macro definition)-Since comments do not belong tothe code scope, and the preprocesting instructions will
bbe expanded in the Clang front end, LLVMIR willlose the text information ofthese conflicts, and finally WizannMncn
‘will not give suggestions related to them.

TSLHO LS LMITSISHESIORSEFIESH
Canc Par ID

Fig. 8. Manual merging time w/ and wo WeanoMesce.

“The effectiveness of menging w/ and w/o WizanpMencs is quantified by time consumption andie shown in Figure §.
‘When the time consumption of merging a pair of commits exccede 70 minutes, we will mark it as "> 70° and stop
zesolving it In 135 out of 277 confit scenarios, WIZARDMENGE contributes to reducing version conflict resclution time,
and asthe time consumption w/o WizaRDMERGE increases, the assistance ability of WizaRDMERGE also becomes more
obvious. Remarkably, for the 208th conflict pair, manual merging w/o WIZARDMERGE costs more than 70 minutes, while
‘with the help of WiZaRDMEnGE, the merging process can be finished in 35 minutes. This exceptional performance is
attributed tothe Priority. Oriented Classification module in W1zaRDMERGE, which provides categorization and fixes
priority recommendations for manually resolved code blocks. Simultaneously, the Violated DCB Detection module

Manat abit to ACM.

---

## Page 14

“4 Qingyu Zhang, Junzhe Li, Jiayi Lin, Jie Ding, Lanteng Lin, and Cheniong Qian

in WizanpMrncr automatically identifies code blocks related to conllicts. Ths feature simplifies developers’ efforts
in analyzing dependencies and avoids the substantial time spent on the manual in-depth exploration of extensive
code space required by Git merge. However, WizaRDMERGE may not optimize the time consumption for all data
points. In 18 out of 277 conflict cases, using WizaRDMERGE could even increase the burden of conflict resolution. This
occurs when only a small number of conflicts are present and few code blocks are affected, rendering WIZARDMERGE’s
assistance not substantial Additionally, as mentioned above, WIZARDMERGE fails to generate suggestions on comments
or preprocessing instructions

LUsting 1. Case study forconflit& violated DCB classification and WeeanoMence-produced resolving order assignment

Gite —git a/gt/Lnyors/serolbartata,h by gt/layere/erollbartate.h
bygre/tayere/Scrolibaroata-b
Sos areteInyere/seribaraata

al. sSerolitracktngth, uintedt
° sBcrollbar.ayeriype(oerollbarLayarypa Thunb),

: retera serolibaroata(abirection, sThabiatio, aThurhstert, athunbLeagth,

Manat abit to ACM.

---

## Page 15

WizardMerge - Save Us From Merging Without Any Clues 15

Listing 1 presents a case study lustrating how WizaRDMERGE classifies the conflicts and violated DCBs and atsigns
‘resolving privileges between VA 145436 and VB 28194%e, Initially, both versions introduce distinct modifications to the
ScrollberDataconstructor, resulting ina merge conflict (lines 817)Subsequently, both versions make different alterations
to the parameter types of the CreateForThumb method in the SerollbarData class, leading to another merge conflict
(lines 25-41). However, Git's strategy reports these conflicts directly to the developer without providing additional
clues. In contrast, WizanDMEnGE utilizes dependency analysis between definitions to discern thatthe constructor of
ScrollbarData willbe called by CreateForThumb (line 42). Consequently, WizaRDMERGE clasifies the two conflicts into
‘one group, assigning higher fx priority to lines 8-17 than to lines 25-41.

‘Additionally, VA and VB present diverse implementations of the function body of the CreateForThumb method.
According tothe three-way merging algorithm, Git applied the code snippets from VB (lines 45-46) after detecting these
toro conflicts, Moreover, within the pair of DCBs represented by lines 43-46 although lines 45-46 are applied without
conflicts, it cannot ensure the constructor of SerollbarData receives the correct parameter types when both conflicts
are present. This is because, ifthe developer chooses to apply all VA modifications when resolving these conflicts, the
applied lines 45-46 from VB will ack essential parameters, aThumbMinL ength and aThumblsAsyncDraggabl, used by
ScrollbarData constructor and the types of aThumbStart, aThumbLength, aScollTraekStart, and aScrllTrackLength will
‘not match the ScrollharDate constructor prototype. Nevertheless, WIZARDMERGE detects lines 45-46 and its mirror DCB
(Ge, lines 43-44) through its innovative dependency analysis approach, treating them as violated DCBs. It reports them
to developers along with other conflicts, Since lines 43-46 depend not only on ines 8-17 (function call) but also on lines
25.41 (definition of prototype), these two violated DCBs are grouped with the previous two conflicts, with fixed priority
coming after the two conflicts, The resolution suggestions provided by WIZARDMERGE are shown in lines 49-58 of
Listing 1, where items prefixed with C indicate pars of DCBs with conflicts, and items prefixed with A or B indicate
DCB from VA or VB considered as violated DCBs,

Conchision: Our evaluation confirms WIARDMERGE can give suggestions for 72.45% of the conflicts. We
analyze the time expenditures associated with conflict resolution and their corresponding DCB w/ and w/o the
utilization of WizaRDMERGE to answering RQ1. On average, WizaIDMEnGE demonstrates a 23.85% reduction
in resolution time. This underscores the efficiency and practicality of WizanDMEnGE in facilitating conflict
resolution, even inthe context of large-scale real-world software.

42. Violated DCB Detection
‘To address both RQ2 and RQ3, our initial step involves establishing the ground truth regarding the mumber of violated
CBs by manually merging two code versions using Git merge. Subsequently, we employ WIZARDMERGE to conduct
an analysis and document the identified violated DCBs, As not all code segments within a DCB pertain to violated.

Mansi abit to ACM.

---

## Page 16

6 Qingyu Zhang, Junzhe Li, Jiayi Lin, Jie Ding, Lanteng Lin, and Cheniong Qian

Table. Violated DCB detection records. CR VA represents the LOC of CRviolsted DCB in VA, and CR VB represents the LaC of CR
Violated DCB in VB. The format n/m indicates that there are m LoCs in total, of which WeeanpMence detects n. Similarly, CU VA,
represents the LoC of CU violated DCB in VA and CUE represents the LoC of CU violated DCB in VB. The format n/m indicates
that WeanoMence detect m LoCs ae CU violated DCB, ofwhich n are confirmed. Recall and Precision witht and+ stand for VA
and VB respective

Dataset CRVA CRVB CUA CUVB Recall} Recall Precisiont Precision}
Firefox ‘151/174 —«112/i23‘75/652 «9/717 _—«RSBOR LOOK ——«LLSOR «13.7%
Linux Ker- 107/131 156/192 171/1245 108/905 S168% 125% «STIR 11.60%
nel

LLVM aise 1623, N/A NA 67.00% © 957% = NA NA

PEP sais2 49/139 «226/324 213/358 69.70% 35K —69TSH —59.50%
MySQL 12274/131003171/3469 1750/4597 1307/2427 93.60% LAI 38.07% SBR

DCB, we utilize Line of Code (LoC) for careful quantification ofthe violated DCBs. These violated DCBs are Further
categorized into Conflict Related (CR) and Conflict-Unrelated (CU) violated DCB.

42.1 Conflict Related Violated DCBs. In addition to addressing conflict DCB, developers should reassess the DCBs
automatically applied by Git merge. Resolving one conflict might potentially impact other sets of DCBs. Throughout
cour evaluation, we recorded the LoC for DCBs linked to conflicts in both versions. This data reflects WizanDMEncE’s
capability in elucidating the correlation between conflicts and violated DCBs.

‘Table 2 illustrates the CR-violated DCB detection results of WizaRDMaRGE. As VA and B's codes are different, the
LoC of CR-violated DCBs in the same diff pair may vary as well. Thus, we documented the LoC from both VA and VB.
According to the evaluation, 167 out of 227 conflict scenarios are confirmed to contain CR-violated DCBs. For most of
the targets, WizaRDMEnGE successflly detected more than 60% CR-violated DCBs, providing a promising assistive cue
for developers in ececuting manual fixes and facilitating them in exploring the CR-violated DCBs, Noticeably, although
‘MySQL datasets have the mort LaC of CR-violated DCBs, WiZaRDMERGE's detection recall even reaches 93.69%. For
LLVM and PHP, WizanpMuncr showed moderate recall.

We alto delved inta the reasons behind WizaRpMEnGr's inability to detect CR-violated DCBs in certain datasets,
Firstly, the Linux Kernel encountered compilation failure with the "00" option, a crucial setting for preserving debug
information and preventing code optimization. Although "Og" can be used for better debug ability, itis akin to ‘OL?
resulting in the elimination of significant debug information [14]. For instance, inlined functions are inserted at the
invocation location and subsequently removed from the original code. As W12aRpMence depends on the collaboration
of debug information and LLVM IR, the absence of debug information leads tothe failure of node generation and DCB
‘matching. Secondly, some datascts include violated dependencies among local variables within a specifi function or
‘macro definition, which Wiz 2pMunct’s analysis currently does not support.

Listing 2. Case study for conflict related violated DCBs

Manat submit to ACM.

---

## Page 17

WizardMerge - Save Us From Merging Without Any Clues 7

= a1 aed yne

17,6 $145,000 tract sand.guawe ¢

Listing 2 presents an intriguing care study revealing conflct-elated violated DCBs detected by WHZARDMERGE
between VA 5c5e0eS and VB lacfe2c. In VA, the definition for virine_r_dmais present, whereas atthe same position in
VB, virtnet interrupt coalesce is defined. Git identifies these versions different modifications asa conflict and prompts
‘manual resolution for developers. Subsequently, Git designates other DCBs as notin conflict and applies them based
oon their consistency with the base version's DCRs. Eventually, the code snippets from lines 24, 31, and 50-51 (from
VA) along with 37-40 (Grom VB) are applied. Despite the differences between VA and VB in lines 46-51, lines 45-49
from VB are consistent with the base version's snippets. According tothe three-way merging strategy, it prefers to
apply lines 50-51 from VA. In contrast, W1za2DMEnGr analyzes all modified code and discovers that even though the
conflict is highlighted, some related DCBs have been applied without alerting the developers. For instance, applying
lines 37-40 from VB assumes the presence of struct virtnet_nq_dma. Ye, ifthe developer opts for lines 15-17 as the
conflict resolution, the structure becomes undefined within that context Similarly, applying lines 24,31, and 50-51 from
VA assumes the definition of struct virinet_interrupt coalesce, while they will cause compilation errors ifthe developer

Manat abit to ACM.

---

## Page 18

18 Qingyu Zhang, Junzhe Li, Jiayi Lin, Jie Ding, Lanteng Lin, and Cheniong Qian

Listing 3. The definitions of function EnsureNSS nitialized
CChromeOrContat ate the same in both versions

sting 4. Diference between commits 42956504539 and
206204233 in flexpcomibaseinsCOMPtch

chooses lines 9-13 as the conflict resolution. Consequently, WIZARDMERGE identifies these code snippets as violated
DCBsin conjunction with the conflict. Furthermore, WIZaRDMERGE assigns higher priority to the confit, signifying
the dependency of other DCBs on the conflict and suggesting developers resolve the conflict before reconsidering these
violated DCB.

422 Conflict-Unrelated Violated DCBs. Throughout this evaluation, we document the LoC for DCBs unrelated to
conflicts in both variant versions reported by WizanDMence. Following this, we manually inspected the identified
CBs and calculated the number of LoC genuinely violated among them.

‘The results of WizaRDMrnar's detection of CU-violated DCB are also presented in Table 2, As CU-viclated DCBs
are isolated from conflicts, WizannM(ERGE demonstrates the capability to detect them, even in scenarios where it fails to
hhandle any conflicts. In total, WizannMunGe detected CU-violated DCBs in 137 out of 227 conflict scenarios, while no
(CU-violated DCBs were foundiin LVM. These DCBs, lacking necessary dependent definitions and automatically applied
bby Git merge without developer notification, do not lead to conflicts but can result in compile errors or even latent bugs
affecting users at runtime, While WizaRDMERGE show cas
valuation also highlights instances of false positive CU-violated DCBs. This arises from the lack of function body
analysis in WizanDMeRGE. According to WiZARDMEnGE’s DCB detection algorithm §33.2, the relationship between
‘two DCBs from the same function relies on line numbers, withthe later one forcibly dependent on the former one. If
the two DCBs are applied from different versions, WIZARDMERGE marks them as CU-violated DCBs, even if they are
‘unrelated. This conservative algorithm may result in more false positives but WizaRDMEXCE prioritizes its application
to prevent overlooking the detection of CUviolated DCBs.

Listing 3 and Listing 4 present another case study where WIZARDMERGE explores conflict-unelated violated DCBs
‘within Firefox (between versions VA {295656 and VB 2206204), which were overlooked by Git merge. Specifically, in
the file nsNSSComponentcpp, both versions define the function EncureNSSInitializedChromeOrContent withthe samme
contents, In Listing 3 lines 6-7, pointer to nalSupports is created via the template class nsCOMPr. The definition of,
rnsCOMPtr is found in nsCOMPtrh, However, in addition tothe generic template, a template specialization [11] ie also
defined for nsISupperts in VB, as shown in lines 11-15 in Listing 4. This specialization of nsCOMPrr for nsISupports
allows users to employ nsCOMPirensSupports similarly to how they use nsISupprts” and void’, essentially as a
‘catch-all’ pointer pointing to any valid [XP]COM interface [0]. When attempting to merge nsCOMPtrh from the two
versions Git applies the file from VA because the ile from VB is identical to the ile from the base version. Consequently,
in the function EnsureNSSInitiaizedChromeOrContent, the usage of nsCOMPtrenslSupports> will fllow lines 4-7 in
Manag submited to ACM.

its potential in uncovering violated dependency issues, the

---

## Page 19

WizardMerge - Save Us From Merging Without Any Clues 1

Listing 4, causing nss to only be able to point to the single [XPJOOM-correct nsSupports instance within an object. This
inconsistency can lead to a potential type confusion bug [19, 21]. Since no compilation error occurs after successfully
being applied by Git merge, it would be extremely challenging for developers to notice such a bug, not to mention
identify the root cause. With the assistance of WIzaRDMERGE, the loss of template specialization can be detected in the
‘merged version, thereby notifying developers to addr

it manually

Conchision: We astess the efficacy of WiZaRDMERGE in identifying CR-violated and CU-violated DCBs by
‘quantifying these DCBs in terms of Lines of Code (LoC), addressing both RQ2 and RQS. The results reveal
that WIZARDMuRGE achieves a detection recall of 80.09% and 73.71% for CR-viclated DCBs from both merging
candidates in the five target projects on average. Furthermore, despite attaining precision rates of 33.26% and.
34.66% for CUl-violated DCB in the four target projects on average, WIZARDMENGE effectively reveals deeply
concealed CU-violated DCBs that are overlooked by developers. To underscore the impacts of CR-viclated
and CU-violated DCBs, we present cate studies that further clucidate the rationale behind WizaRDMERGE’s
noteworthy findings.

3 Discussion

Inthis section, we discuss the present limitations in WizaRDMEGE’s design and explore potential avenues for enhancing,
WaarpMance.

[Improve Graph Generation. As discussed in §3.3, WIZARDMERGE incorporates three node types and five edge types to
construct the ODG. While it adeptly manages mumerous merging scenarios, the evaluation outcomes highlighted in.
{4 suggest that this incompleteness could result in an inability to address certain conilicts or violated DCB instances.
For example, WizAnDMERGE cannot infer the dependency from a global variable to a function if the global variable
is initialized by the function. This is because, during the compilation process, the initial values of global variables
are established before the execution. In contrast, a function's return value is accessible only after being executed.
‘When initializing global variables with a function’s return value, the compiler transforms the function invocation
into initialization code. To ensure the execution of this initialization code before the main program commences, the
compiler positions this code within a specific section of the executable file, commonly denoted as the "init" segment.
‘This separation facilitates the execution ofthese initialization routines prior tothe initiation of the main program logic.
We leave updating the features as our future work to adapt to such scenarios

Assess Code Optimization, WizanoMuEncE conducts dependency analysis based on the compilation metadata, Never:
theless it overlooks a substantial amount of code information due to compilation optimizations like unused function.
climination and vectorization [27]. As an interim measure, WizanDMEncx presently enforces the "OO" option. However,
some projects require compilation optimization for successful building. Fr instance, the Linux kernel relies on opti-
‘ization to eliminate redundant code segments and disallows the "00" option. Inthe future, we aim to explore how to
adapt our approach to arbitrary optimization options.

6 Related Work:

Structured Merging. Mastery [45] introduces an innovative structured merging algorithm that employs a dual
approach: top-down and bottom-up traversal. It can minimize redundant computations and enable the elegant and.
Mansi submited to ACM.

---

## Page 20

20 Qingyu Zhang, Junzhe Li, Jiayi Lin, Jie Ding, Lanteng Lin, and Cheniong Qian

ficient handling of the code that has been relocated from its original postion to another location. Spork [23] extends
‘the merging algorithm from the 3DM [25] to maintain the well-structured code format and language-rpecific constructs
‘To fully retain the original functionality of the merged code, SafeMerge [35] leverages a verification algorithm for
analyzing distinct edits independently and combining them to achieve a comprehensive demonstration of conflict
freedom.

Semi-structured Merging. JDime [4] introduces a structured merge approach with auto-tuning that dynamically
adjusts the merge process by switching between unstructured and structured merging based on the presence of conflicts
PSTMerge [5] stands as an carly example of semi-structured merging, offering a framework for the development
of semi-structured merging, Intelimerge [‘4] transforms source code into a program element graph via semantic
analysis, matches vertices with similar semantics, merges matched vertices’ textual content, and ultimately translates
‘the resulting graph back into source cade files.

‘The above works, however, cannot help the developers with resolving conflicts. On the contrary, Wiza RDMERGE is
constructed atop Git merge, furnishing developers with clues to aid manual resolution instead of dictating final merging
decisions.

62. Conflict Resolution

Confit Resolution Assistance. TIPMerge [2] evaluates the developers’ experience with the key files based on the
‘project and branch history [J] to determine the most suitable developers for merging tasks. Automerge [14] relies on
version space algebra [25] to represent the program space and implement a ranking mechanism to identify resolution
‘within the program space that is highly likely to align with the developer's expectations. SoManyConflits [33] is
implemented by modeling conflicts and their interrelations as a graph and employing classical graph algorithms to
offer recommendations for resolving multiple conflicts more effcienty.

“Machine Learning Approaches, MergeBERT [3] reframes conflict resolution asa classification problem and leverages
token-level three-way differencing and a transformer encoder model ta build a neural program merge framework,
Gmerge [13] investigates the visbility of automated merge conflict resolution through k-shot learning utilizing pre-
‘rained large language models (LLMs), such as GPT-3 [16]. MergeGen [13] treats conflict resolution as a generation
task, which can produce new codes that do not exis in input, and produce more flcible combinations.

‘The essential difference compared with existing assistance systems and machine learning approaches is that Wa-
‘ADMERGE considers all modified code snippets, which further helps developers resolv the violated DCBs in merging,

7 Conclusion

We introduce WIZARDMEnGE, a novel code-merging assistance system leveraging the merging results of Git and
dependency analysis based on LLVM IR, named definition range, and debug information. Via the dependency analysis,
WrzanpMencr is able to detect the lose of dependency of the named definitions, therefore providing more accurate
merging order suggestions including conflicts and applied code blocks forthe developers. Our evaluation demonstrates
‘that WizaRnMunce significantly diminishes confict merging time costs. Beyond addressing conflicts, WizanDMERCE
provides merging suggestions for most ofthe code blocks potentially affected by the conflicts. Moreover, WIZARD-
‘Mena exhibits the capability to identify conflic-unrelated code blocks which should require manual intervention yet
automatically applied by Git.

Manat abit to ACM.

---

## Page 21

WizardMerge - Save Us From Merging Without Any Clues a

References

[n} ala Aci, Palo Borba, nd Gaherme Cavan 2018 Undertunding semistructured merge confit characterises in opensource ora
projets. Empirical Seffre Brinig 73 (018), 251-2085

[2] Mebhor Ahmed, Caius Brindesu, Unme Aya Manna, Cals Jensen and Anita Sarma 2017. An empires examination f the raticship
eee code nme nd merge conic In 2017 ACM/IEEE International Sympaiam on Empirical Sofware Enpnering and Meomremert (ESE)
IEEE, 5-7.

[5] Wad Aldndas, Na Meng and Francie Servant 2023, Automatic predctin of developer! resktion for software merge confi. jarnelof
Sytem and Sftrare 206 (2029, 1136

[a] Sven Apel, lf Laenich, and Chustan Lega 2012, Structured merge wth auto-tuning blancng pein and performance In Procedings of
the 20h IEEE/ACM intentional Confiene on AtomatedSoftvareEnineng 120-139.

[5] Sven Apel org Lib, Benjamin Brand Chistian Langues and Chitin Kistner 2011, Semistroctared merge retaking merge in revision
conteel system. In rcedigs ofthe 1th ACM SIGSOFT eypaiu andthe Ith Eurepeon conference on Foundation of fare eincering,

[o) Apsara 2021 Ue LLM by JorScaptTypeSeript h/t pecaniuse ten oraseptypenspt 7, Assen 24Feh

[r] Attn 2022 it Mere. pe worl com/gttodalnuing-branches/g-merge, Ascend: 02-Oct

[s} Guthenme Corcan, Paulo Hor nd Pola Acco. 2017 Bralunting and improving semistructured merge. Prcedings of the ACM on
‘Progemming Languages 1, OOPSLA (201), 1-27.

[o] Caan Conta, Jai Fg, Leonardo Maras Anta Sarma, 2046, IPMarge: recommending expert fr ntegating changes om branches
1 Proceedings ofthe 206 2th ACM SIGSOFT ntertionl Symposium on Foundation of Sofware Enpnewing 5234

[to] CpAlgetios 202% Segment Tree tp/ep-algrtn.com/atastrctarsinegnent oti Accel: 20-08

[ut] Hlzabeth Dine, od My tow, Alem Seyatkorsiy, Christin Bird Mayu Nai, ar Shavers Lah 202, Deeper eing to merge
progam IEE Tronatios on Software Eainering W, (212) 1599-16,

[t2] Jina Deng, Qiao Zhu, Zey Su, Ying Lou and Das Hho, a). Merge Cone Revelation: Clanton of Genertin? (4)

[3] Jina Dong. Quo Zhu, Zey Sn, ing ou and Dan Hao. 202 Merge Conf esto: Clasifation or Genera? 2133 TEEEACM
nterntionl Conference on atomatd Software Erginerng (ASE) TEE 1652-168,

[14] Changin Da. 2018, kernel hacking: CC opinion for beter chug experience (Op) tpn Aside 75369. Accel: 24 Jan,

[tS] Pat lag, Heleno de Campo eiog Edoardo Opumoras sd Leonard Gest Paling Mura 23, Toward acute evommensatins of
‘merge conflicts resin arategien Infrmaton nd Softer Teale (23,1075

[16] tains Foi and Manin Chae 200. GPT: utr, scopes and commence. Mindsand Machine 30 (2020, 61-184

[a7] Gi. 200. peso cm. Accemed 2023-0

[ts] ie 2003 Ge Merge. z:gi-sm condone merge Accent 223-02.

[19] tan Haller, Yasok Jon, Hui Peg, Mathias ayer Ceitiana Gili, Herbert Bo, and rie van der Kou. 2016 Types: Pract Type
(Confusion Detection In Proceedings of the 2016 ACM SGSAC Confrence on Computer and Communications Sart Vienna, Aarts) (CCS 8
Azsciton for Computing Machinery, New Yat, NY, USA, 517-528. Ip:/oiog 041452976740 3978105

[20] Kaman Handy 203. The Bigg Coden in History, ae Meare by Line f Cade parr astetatincom bogs Sey Rothe
igget-codeane--hitary-armeurel-by Hiner of cole, Acree: 02 Jon,

[at] ase Jeon Priam Biswas Sot Car Byoungyonng Lee, and Mathie Pax 2017 Hedy: fit Detection of Type Confusion ree far
(Cs a Procednr ofthe 2017 ACM SGSAC Confrence on Computer and Conmaniton Sear ty (Dalla, Texas, USA) (CCS 7} Ascii for
Computing Machinery, New Yor NY, USA, 2972-2807. ipaoion/10S14/920056215082

(22) Jin Kana 2014 GIT merge ccs bat nrodce comptioneror te ackoveln com quo 770g merge-mcentl~
ut ntodicevconplation enor Acne: 32a

(23) Sion Laan Jan Remy Fler, Beast Bandy, and Martin Monperu. 202. pk Stretred Mere fr Java with Formating Preservation IEE
“ramcion on Sofware Engine 8,1 (2022, 6483.

[24] CheisLatner 208, LLYM and Clang-Next generation cpr techno In The BSD cofrnc Ve 51-20

[a5] Term Ln, Steven AWelfian, deo Domingos and Daniel S Weld 208 Programming by demonstration using vein pace serra, Mac
ering 53 (09), 11-156

(26) anced Lndhokm, 2004. thee-way mene for XML documents Ia Poeeings of he 2004 ACM sympa on Dosenteginerng 10

[27] LOM, 2008 LLVM'sAnabse and Traafm Passes tp /lvmordocs/Psn bl, Aces 3D

[es] LIVM, 2026 The LLVM Compiler Infrae ite/lvmor. Accel 027:

[op] Moni 2024 Mewar Gecko Repost tp:/emexila og Ace: 23-Dec

[0] Mona 2024 GeckosdersauCOMPtch. te/gthshcon/mailagecko-de oy a20<204735%4E afc both 2Abaepomse!
snsCOMiNch Accel 202601-25,

[st] Myst 2024 MySQL bupe/der mye. Acceed 2004p

[st] YeihiNikimer and Katrin Maruyama. 2016. Supporting merge confit resation by wing ne rained code change hier In 20 IEEE 2rd
Inerntianl Confience on Sftre Anal, Elution, and Reining (ANER} Vo 1. IEEE, 66-64

Manat abit to ACM.

---

## Page 22

2 Qingyu Zhang, Junzhe Li, Jiayi Lin, Jie Ding, Lanteng Lin, and Chensiong Qian

[83] Bo Shen, Wei Zhang, Aun Yu, ian Shi, Hayan Zhao and Zhi fn. 2001 SaManyConficts: Reve many merge colts interactively and
systematic I 2021 3th IEEE/ACM beratinal Confrnet on Automated Seftrare Bini (ASE) IEE, 1291-1295

[24] Bo Shen, Wet Zhan, Huiyan Zio, Guangtai Ling, Zhi, and Quaeiang Wang 2019 IntliMerge:arutoring-aware oftware merging
technique Frcedagrofthe ACM on rgramming Languazes3, OOPSLA (219), 1-2

(25) Marcel Sou, il Dili, nd havendsK ahi 2018 Ved tee say progam merge. Prcenings of he ACM on Programming Longuager2,
OOPSLA (2018), 1-28

[36] Calvin Spelman. 2018, When a Clean Merge i Wrong, tp cturoup cmap 2018) 1when- lean merge-wrone. Acceed
2b

[a7] Alexey Sepacy 2020, What'sthe dilfeencebebveenKoiyNativ nd Jv bytecode ith compl ough LLVM feted? btp:tacovedone
‘cen/quetons/ 8801 85\what the difeeace beter natveant joa-ytecode-with-ompie thas, Acreaeak eh,

(a8) Alexey Sryathkiy, Sarah Flory, NegarGhoran, Todd Myowi, Elizabeth Dinlla, Christan Bird, Jn Jang, Nel Sundaern nd
‘Stover K Lak 2022, rogram merge conflict ocation via neural tranfomers In rowed ofthe Se ACM Jot Earp Sofvare
Entineing Confrcne and Symp onthe Foundation of Softarenginarng 22-13.

(a0] Robert jan 197. Depbfst search and liner graph algorithms. SAM journal on computing 121972) 86-16,

{ao} Linas Twas 2028 Lin Keel Oeil Repository. hps:gihubcomforaliins. Accented 223-0

[at] David Vandevoorde ad Nicola M Jontie. 02 Cr+ templates The comple id prtaie dimers Adnan Wey Pelco

(la) Aese Mii Viner 2023. The Mage of Way Merge tpg int omthe magic way merge Accel: 2022-0

[es Jide Zhang, Todd Mythowic, Mike Kaufnao, Ria Pac ad ShvendaK Labi 202, Using poe-trined lngunge modelo rake textual
spd semanic merge conics (experince pape) In Proceeding ofthe at ACM SGSCET International Symposium on Sofware Teng end Anabeic

[at] Fengmin Zhu and Fei He. 2018. Cont ealton Fr structured merge via version spce agen. Prcedings ofthe ACM on Programing
Language 2, OOPSLA (2018), 1-25.

[45] engin Zh XingyoXix,Dongy Feng, Na Meng and el He. 202. Matery Sie Coe wareStracured Merging In Itai Spar
on Dependale uf Bngnewng: There, Tl and Aplications Springs 70-87.

Manat abit to ACM.

---

