# ELC2132 · Data Structures & Algorithms Lab

**Semester 3 · Manipal University Jaipur · July-Nov 2026**

Everything for the DSA lab: the manuals, the **C++ sources** of every experiment, the
ready-to-submit records for Experiments 0 to 4, and the scripts that build and verify all of it.

All eleven programs are C++ (`g++ -std=c++17`), written in plain style: `int` everywhere,
`#define` constants, `cin` and `cout`, simple loops. The earlier C implementations are archived
in `Extra/c_versions/` and the older practice files in `Extra/`.

## Status

| Experiment | Topic | Source | Record |
|---|---|---|---|
| **Exp 0** | **Array operations: 1-D operations, insertion, linear search, 2-D pointers** | `Exp0_Array_Operations/` | **`Records/EXPERIMENT_0_RECORD.md`** |
| **Exp 1** | **Sorting using merge sort** | `Exp1_Merge_Sort/` | **`Records/EXPERIMENT_1_RECORD.md`** |
| **Exp 2** | **Sorting using quick sort** | `Exp2_Quick_Sort/` | **`Records/EXPERIMENT_2_RECORD.md`** |
| **Exp 3** | **Recursive programs: factorial, Fibonacci, Tower of Hanoi** | `Exp3_Recursion/` | **`Records/EXPERIMENT_3_RECORD.md`** |
| **Exp 4** | **Priority queue: unsorted array and sorted linked list** | `Exp4_Priority_Queue/` | **`Records/EXPERIMENT_4_RECORD.md`** |
| Exp 5 | Binary tree representations (array and linked) | not written yet | manual only |

## Folder map

| Folder | Contents |
|---|---|
| `Manuals/` | The student manuals for Experiments 0 to 5 |
| `Handouts/` | The course handout (syllabus, lab plan, assessment scheme) |
| `Exp0_Array_Operations/` | `exp0_1.cpp` (1-D operations), `exp0_2.cpp` (insert), `exp0_2_linear_search.cpp` (linear search), `exp0_3.cpp` (2-D pointers) |
| `Exp1_Merge_Sort/`, `Exp2_Quick_Sort/` | Exp 1 and Exp 2 sources |
| `Exp3_Recursion/` | `exp3_1_factorial.cpp`, `exp3_2_fibonacci.cpp`, `exp3_3_tower_of_hanoi.cpp` |
| `Exp4_Priority_Queue/` | `exp4_1_priority_queue_array.cpp`, `exp4_2_priority_queue_linkedlist.cpp` |
| `Records/` | The records for Experiments 0 to 4: markdown sources, one printable HTML, one A4 PDF |
| `Extra/` | `c_versions/` holds the earlier C implementations (same sub-folders), and the older C++ practice files are kept with a `_practice` suffix |
| `_scripts/` | `expect_run.py` (terminal-accurate transcripts), `inline_code.py` (pastes sources and transcripts into the records), `make_records_html.py` (builds the printable book), `check_results.sh` (runs all 11 programs and checks their results) |
| `_build/` | All compiled binaries, the sample-run transcripts in `_build/out/`, and `legacy_binaries/` (the loose binaries that used to sit in the lab folder) |

## Build and run

```sh
cd ~/Documents/MUJ-SEM-3/DAS-LAB
sh build.sh          # compiles all 11 C++ programs into _build/
sh run_demos.sh      # reruns every sample session into _build/out/
```

`build.sh` uses `g++ -Wall -Wextra -Wpedantic -std=c++17 -O2` and reports the warning status of
every file individually. All 11 programs compile with **zero warnings**.

`run_demos.sh` drives every program through a pseudo-terminal with `_scripts/expect_run.py`, so
the transcripts in `_build/out/` show the typed input where it was typed, exactly like a real
terminal session.

## What each experiment covers

**Experiment 0** covers 1-D array operations (initialization, sum, maximum, reversal), insertion
at a chosen index with the shifting loop, linear search with a comparison count, and 2-D array
access through pointer notation (`*((*(arr + i)) + j)`).

**Experiments 1 and 2** sort the same array `38 27 43 3 9 82 10`, first with merge sort and then
with quick sort, so that the two divide and conquer strategies can be compared:

| Property | Merge sort | Quick sort |
|---|---|---|
| Best and average time | O(n log n) | O(n log n) |
| Worst-case time | O(n log n) | O(n^2) |
| Extra space | O(n) for the temporary arrays | O(log n) recursion stack on average |
| In place | no | yes |
| Stable | yes | no |

**Experiment 3** implements three recursion patterns:

| Program | Pattern | Time | Space |
|---|---|---|---|
| Factorial | single recursive call | O(n) | O(n) |
| Fibonacci | two recursive calls (tree) | O(2^n) | O(n) |
| Tower of Hanoi | two calls with problem transformation | O(2^n) | O(n) |

**Experiment 4** implements the same priority-queue ADT twice, once with an unsorted array
(`insert` O(1), `extractMax`/`peek` O(n)) and once with a sorted linked list (`insert` O(n),
`extractMax`/`peek` O(1)), plus a menu with `insert`, `extractMax`, `peek`, `display` and
`isEmpty`.

## The records

Each record follows the structure of its manual (Aim, Algorithm, C++ Program, Results) and adds
an observation, the complexity analysis and a conclusion:

| File | Covers |
|---|---|
| `Records/EXPERIMENT_0_RECORD.md` | Part 1: initialization, sum, maximum, reversal. Part 2: (a) insertion, (b) linear search. Part 3: 2-D arrays through pointer notation |
| `Records/EXPERIMENT_1_RECORD.md` | Merge sort, with the full split and merge trace |
| `Records/EXPERIMENT_2_RECORD.md` | Quick sort, with the partition trace and a comparison against merge sort |
| `Records/EXPERIMENT_3_RECORD.md` | Factorial, Fibonacci and Tower of Hanoi, with measured call counts and move counts |
| `Records/EXPERIMENT_4_RECORD.md` | Max-priority queue as an unsorted array and as a sorted linked list |
| `Records/DAS-LAB_Records.html` | All five records in one printable page |
| `Records/DAS-LAB_Records.pdf` | The same, 55 pages A4, ready to print |

Name, registration number, batch and date are left blank in the header of every record.

The records contain no copy-pasted code: each program listing and each transcript is an include
marker such as

```
<!--CODE:Exp3_Recursion/exp3_1_factorial.cpp-->
<!--TEXT:_build/out/exp3_1_factorial.txt-->
```

that `_scripts/inline_code.py` expands from the real file. After editing a program or rerunning a
demo, one command brings every record back in step:

```sh
python3 _scripts/make_records_html.py
libreoffice --headless --convert-to pdf --outdir Records Records/DAS-LAB_Records.html
```

## Verification performed

* All 11 programs compile with `g++ -Wall -Wextra -Wpedantic -std=c++17` and no warnings.
* All 11 run clean under `-fsanitize=address,undefined`: no leaks, no undefined behaviour.
* The results are re-checked by `sh _scripts/check_results.sh`, which runs all 11 programs and
  searches their output for the expected values: 26 checks, all passing. The checked values are
  the same ones the C versions produced; only some printed messages were shortened.
* Measured results match theory: factorial up to `12!`, the biggest value that fits in an int; Fibonacci call count equals
  `2*F(n+1) - 1` for n = 5, 10, 15, 20, 25, 30; Tower of Hanoi move count equals `2^n - 1` for
  n = 1, 2, 3, 4, 5, 10, 15, 20; merge sort and quick sort both sort the manual's array to
  `3 9 10 27 38 43 82`.
* Edge cases checked: insert at index 0, at the end and with an invalid index; linear search for
  the first element (1 comparison), the seventh (7), the last (10) and for an absent key (10);
  extract and peek on an empty priority queue; equal priorities in both queue implementations.
* Every program listing and every transcript inside the records was diffed against its file and
  they match byte for byte.

## Where the older versions live

Nothing was deleted when the lab moved to C++:

| What | Where |
|---|---|
| The C implementations of all 11 programs | `Extra/c_versions/<same sub-folder>/` |
| The earlier C++ practice files (`fibonfact.cpp`, `towerofhanoi.cpp`, the two older priority queue programs) | `Extra/*_practice.cpp` |
| The earlier combined Exp 0 Part 2 program | `Extra/exp0_2_combined_practice.c` |

The moves are logged in `../_reorg/cpp_migration-<date>.tsv`, and
`../_reorg/undo_cpp_migration-<date>.sh` moves them back. Two of those destinations are now
occupied by the new C++ programs (the two Exp 4 files), and because the undo script uses `mv -n`
it never overwrites anything, so those archived practice files simply stay in `Extra/`.
`../_reorg/README.md` explains both cleanups.

## Note on the manual listings

The manuals print C listings. The C++ programs follow the same algorithms and produce the same
output, with these differences:

* The code stays deliberately plain: `int`, `#define`, `cin`/`cout`, plain loops and one small
  `swapValues` function. No casts, no vectors, no templates.
* The merge step of merge sort uses two temporary arrays `L[SIZE]` and `R[SIZE]` instead of
  run-time sized arrays, which C++ does not allow.
* The linked priority queue uses C++ `new` and `delete` instead of `malloc` and `free`.
* The factorial program keeps its result in an `int`, so it accepts numbers up to 12, where
  `12! = 479001600` still fits; anything bigger is rejected with a short message.
* Exp 0 Part 2 was split into the two programs the manual asks for, and the Turbo C listing in the
  manual (`<conio.h>`, `clrscr()`, `getch()`, `void main()`) was replaced by a portable version,
  since those calls only exist on DOS.
