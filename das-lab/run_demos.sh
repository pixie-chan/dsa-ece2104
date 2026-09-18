#!/bin/sh
# Regenerates the sample-run transcripts used in the lab records.
# Usage: sh run_demos.sh        (from the DAS-LAB folder)
#
# Every program is run inside a pseudo-terminal by _scripts/expect_run.py, so the
# typed input is echoed into the transcript exactly as it appears in a terminal.
# Transcripts are stored with trailing whitespace stripped, so that they match
# the blocks pasted into Records/*.md byte for byte.

set -e

OUT=_build/out
EXPECT="python3 _scripts/expect_run.py"
mkdir -p "$OUT"

# --- Experiment 0, Part 1: 1-D array operations ------------------------------------
$EXPECT _build/exp0_1 > "$OUT/exp0_1.txt"

# --- Experiment 0, Part 2: insertion and linear search -----------------------------
# insert the value 99 at index 1 of {10, 20, 30}
$EXPECT _build/exp0_2 1 99 > "$OUT/exp0_2_insert.txt"
# same program with an invalid index (guard case)
$EXPECT _build/exp0_2 9 > "$OUT/exp0_2_insert_bad_index.txt"

SEARCH_DATA='5 12 7 23 45 3 19 8 30 11'
$EXPECT _build/exp0_2_linear_search "$SEARCH_DATA|19"  > "$OUT/exp0_2b_search_found.txt"
$EXPECT _build/exp0_2_linear_search "$SEARCH_DATA|100" > "$OUT/exp0_2b_search_notfound.txt"

# --- Experiment 0, Pointers: 2-D array through pointer notation --------------------
$EXPECT _build/exp0_3 > "$OUT/exp0_3.txt"

# --- Experiment 1: merge sort ------------------------------------------------------
$EXPECT _build/exp1_merge_sort > "$OUT/exp1_merge_sort.txt"

# --- Experiment 2: quick sort ------------------------------------------------------
$EXPECT _build/exp2_quick_sort > "$OUT/exp2_quick_sort.txt"

# --- Experiment 3 -----------------------------------------------------------------
$EXPECT _build/exp3_1_factorial     10 > "$OUT/exp3_1_factorial.txt"
$EXPECT _build/exp3_2_fibonacci      8 > "$OUT/exp3_2_fibonacci.txt"
$EXPECT _build/exp3_3_tower_of_hanoi 3 > "$OUT/exp3_3_tower_of_hanoi.txt"

# --- Experiment 4 -----------------------------------------------------------------
# Same operation sequence through both implementations:
# insert (10,3) (20,1) (30,5) (40,2) | display | peek | extractMax | peek | display | isEmpty | exit
PQ_INPUT="1|10 3|1|20 1|1|30 5|1|40 2|4|3|2|3|4|5|6"
$EXPECT _build/exp4_1_priority_queue_array      "$PQ_INPUT" > "$OUT/exp4_1_pq_array.txt"
$EXPECT _build/exp4_2_priority_queue_linkedlist "$PQ_INPUT" > "$OUT/exp4_2_pq_list.txt"

echo "Transcripts written to $OUT/:"
ls -1 "$OUT"
