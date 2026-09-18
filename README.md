<div align="center">

# 📚 DSA · ECE2104 (Semester 3)

**Manipal University Jaipur · Data Structures and Algorithms**
Every number in here was measured on the machine that wrote it, not copied from a slide.

[![course](https://img.shields.io/badge/course-ECE2104%20%2F%20VDT2104-1b4d7a?style=for-the-badge)](https://github.com/pixie-chan)
[![language](https://img.shields.io/badge/C%2B%2B-17-00599C?style=for-the-badge&logo=cplusplus&logoColor=white)](https://github.com/pixie-chan)
[![compiler](https://img.shields.io/badge/g%2B%2B-15.2.0%20--Wall%20--Wextra%20-Wpedantic-3d6b3f?style=for-the-badge)](https://github.com/pixie-chan)
[![verified](https://img.shields.io/badge/verified-1%2C074%20checks%20across%206%20verifiers-7a3355?style=for-the-badge)](https://github.com/pixie-chan)
[![license](https://img.shields.io/badge/license-MIT-4a453f?style=for-the-badge)](LICENSE)

</div>

```
   ╭──────────────────────────────────────────────────────────────╮
   │   DSA  ·  ECE2104  ·  mid-term roadmap = lectures 1 to 16    │
   │                                                              │
   │   memory  →  nodes  →  lists  →  stacks  →  queues  →  ⤴     │
   │   base + i x sizeof(T)          the call stack is a stack    │
   ╰──────────────────────────────────────────────────────────────╯
```

## 🎯 What this repo is

My complete working set for **ECE2104 Data Structures and Algorithms** (theory) and **ELC2132 DSA Lab**.
Notes, interactive guides, lesson programs, verifiers and captured run outputs, all in one place so
version control is a single command.

| | |
|---|---|
| **Course** | ECE2104 / VDT2104, Manipal University Jaipur, Semester 3 |
| **Mid-term scope** | lectures 1 to 16 (confirmed from the handout lecture plan: the MID SEMESTER EXAMINATION row sits after lecture 16) |
| **Assessment** | Mid-Term 30 (closed book), CWS 30 (quiz, MOOC, assignment), End Term 40 |
| **Course outcomes** | CO1 lectures 1 to 10, CO2 lectures 11 to 16 |
| **Machine** | Lenovo LOQ 15IRX9, i7-13650HX, 24 GB RAM, RTX 4060, Ubuntu 26.04 LTS, g++ 15.2.0 |

## 🗺️ The lecture map, and what exists for each lecture

| L | Topic (handout wording) | Artifact in this repo | State |
|---|---|---|---|
| 1 | Introduction and Course Hand-out briefing | no artifact needed | n/a |
| 2 | Introduction to C++/Python, Features, Program Structure | `learning/cpp/lessons/01_basics.cpp` | ✅ |
| 3 | Basic Terms: Variables, Data Types, Operators, IO | `02_variables.cpp`, `03_control_flow.cpp` | ✅ |
| 4 | Control Statements and Functions | `03_control_flow.cpp`, `04_functions.cpp` | ✅ |
| 5 | Introduction to Data Structures and Memory Representation | `dsa/Notes/intro-memory/` guide + notes + verifier | ✅ verified |
| 6 | Linked List: Concept, Types, Node Representation | `dsa/Notes/linked-list/` + `06_linked_list_representation.cpp` | ✅ verified |
| 7 | Linked List Operations: Creation, Insertion, Deletion, Traversal | `07_linked_list_operations.cpp` + lec7 diagrams | ✅ |
| 8 | Searching in Linked List, Applications, Numerical Problems | `08_linked_list_searching_apps.cpp` + 12 solved numericals in the roadmap | ✅ |
| 9 | Tutorial | `dsa/Notes/linked-list/` section 7 | ✅ |
| 10 | Class Test | self-tests inside the two shipped guides | ✅ |
| 11 | Stack: Concept and array representation | `dsa/Notes/stacks-queues/` (section 4), `learning/dsa/stack-guide.html`, `11_stack_array.cpp` | ✅ |
| 12 | Stack using Linked List, Stack Application | same notes, section 5 (linked stack, brackets, reversal), `12_stack_linked_apps.cpp` | ✅ |
| 13 | Queue: Concepts and Array Representation | same notes, section 6 (FIFO proof, the exact waste point), `13_queue_array.cpp` | ✅ |
| 14 | Queue using Linked List and Circular Queue | same notes, section 7 (two wrap-arounds), `14_queue_linked_circular.cpp` | ✅ |
| 15 | Queue Applications | same notes, section 8 (round robin, queue from two stacks), `15_queue_apps.cpp` | ✅ |
| 16 | Recursion and Recursive Algorithms | `dsa/Notes/recursion/` notes + 4 diagrams, `09_recursion_frames.cpp` (measured frames) | ✅ verified |

The interactive **mid-term roadmap** with the full brief, the dependency chain, the seven session plan and
the numericals lives at `dsa/Notes/midterm-roadmap/DSA-MIDTERM-ROADMAP.html` (open it in a browser).

## 📦 Repo layout

```
dsa-ece2104/
├── dsa/                          theory course: notes, guides, roadmap, plans
│   ├── Notes/intro-memory/       lecture 5 visual guide + notes + 5 diagrams + tokens
│   ├── Notes/linked-list/        lectures 6 to 8 notes + 6 diagrams
│   ├── Notes/stacks-queues/      lectures 11 to 15 notes + 13 diagrams
│   ├── Notes/recursion/          lecture 16 notes + 4 diagrams
│   ├── Notes/midterm-roadmap/    the mid-term dossier (HTML), 10 diagrams, its apply
│   │                             list, patch sheet, review log and versions/
│   ├── Notes/mte-scope/          the 40 question mock paper + the 24 problem numeric set
│   ├── Notes/topic-wise/         the 14 projected topics explained, 17 diagrams
│   ├── Notes/DSA-STUDY-PLAN.md   18 week mastery plan (and .html)
│   └── Notes/C++-LANGUAGE-PLAN.md
├── das-lab/                      ELC2132 lab: Exp0 to Exp4 sources and captured outputs
├── learning/
│   ├── cpp/lessons/              lesson programs, lecture mapped (01 .. 23)
│   ├── cpp/exercises/            six verifiers and the captured run output
│   ├── cpp/linked-list-visual/   interactive linked list page
│   └── dsa/stack-guide.html      stack explainer page
├── sync.sh                       re-mirror the live trees into this repo
└── README.md
```

## 🔬 Every number is traceable

| Claim | Where the proof lives |
|---|---|
| `sizeof(int) = 4`, `sizeof(Node) = 16`, allocator stride `32`, usable `24` | `learning/cpp/exercises/lec05_run_output.txt` |
| one stack frame is `48` bytes, frames descend | same capture, section 2 |
| Hanoi with 3 disks is 7 calls and 7 moves, nesting 3 frames | `learning/cpp/exercises/lec16_recursion_output.txt` |
| the lecture plan, the exam cutoff and the CO mapping | handout read with `pdftotext -layout`, rows re-checked on 300 dpi page images |

Nothing here is quoted from a slide or invented to look tidy. Where something could not be measured,
the file says so.

## ✅ How to verify any of it

```bash
# lecture 5 memory representation: 81 checks plus a mutation battery
python3 learning/cpp/exercises/verify_ds_memory_lec5.py
python3 learning/cpp/exercises/verify_ds_memory_lec5.py --mutate

# lectures 6 to 8 linked list: 89 checks
python3 learning/cpp/exercises/verify_linked_list_lec6_8.py

# lectures 11 to 15 stacks and queues: 228 checks against the real captures
python3 learning/cpp/exercises/verify_stacks_queues_lec11_15.py

# lecture 16 recursion: 303 checks, re-runs the timings and the death depth
python3 learning/cpp/exercises/verify_recursion_lec16.py

# the topic-wise explained bank and the mock paper: 233 and 140 checks
python3 learning/cpp/exercises/verify_topic_wise.py
python3 learning/cpp/exercises/verify_mte_guide.py

# rebuild the lesson programs yourself (zero warnings is the contract)
cd learning/cpp/lessons
g++ -Wall -Wextra -Wpedantic -std=c++17 -O2 -o /tmp/lec05 05_ds_memory_representation.cpp && /tmp/lec05
g++ -Wall -Wextra -Wpedantic -std=c++17 -O2 -o /tmp/rec 09_recursion_frames.cpp && /tmp/rec

# re-render the mermaid diagrams (needs node and a chromium for mermaid-cli)
sh dsa/Notes/intro-memory/render-diagrams.sh
sh dsa/Notes/midterm-roadmap/render-diagrams.sh
```

## 🔄 Keeping it in sync

The live working copies stay where they belong (`~/Documents/MUJ-SEM-3/DSA`, `~/learning/cpp`, and so on).
This repo is a mirror, refreshed by one script:

```bash
sh ~/projects/dsa-ece2104/sync.sh
cd ~/projects/dsa-ece2104 && git add -A && git commit -m "sync: linked list numericals" && git push
```

## 🙈 Deliberately not uploaded

| Not here | Why |
|---|---|
| `Handouts/Course Handout ECE2104VDT2104_26.pdf` | it is the university's document, not mine to republish. It stays local in `~/Documents/MUJ-SEM-3/DSA/Handouts/` |
| compiled binaries and `_build/` output | regenerable, and they bloat a repo |
| gate screenshots (`**/.qa/*.png`) | megabytes of pixels that any run reproduces. Ignored at any depth now; the pattern used to match only a root-level `.qa`, so nested ones leaked into the history |

## 📄 License

MIT, see [LICENSE](LICENSE). Course handouts, lecture slides and question papers remain the property of
Manipal University Jaipur; nothing of theirs is redistributed here.

<div align="center">

**Built in a terminal, verified with real runs, no em dashes anywhere.** 🛠️

</div>
