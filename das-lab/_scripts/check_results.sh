#!/bin/sh
# Checks the results of all 11 programs against the values they are supposed to
# produce. Every check runs the real binary and greps its output for the expected
# result, so a broken program makes this script fail.
#
# Usage: sh _scripts/check_results.sh     (from the DAS-LAB folder)

set -e

LAB=$(cd "$(dirname "$0")/.." && pwd)
cd "$LAB"
BUILD=_build

passed=0
failed=0
total=0

# expect <label> <program> <input> <expected text>
expect() {
    label=$1
    program=$2
    input=$3
    want=$4

    # "|| true" because some checks deliberately use input that the program
    # rejects, and a rejected input makes the program exit with status 1.
    got=$(printf '%s' "$input" | "$BUILD/$program" || true)
    total=$((total + 1))

    if printf '%s' "$got" | grep -qF "$want"; then
        passed=$((passed + 1))
        echo "  PASS  $label"
    else
        failed=$((failed + 1))
        echo "  FAIL  $label"
        echo "        expected to find: $want"
    fi
}

SEARCH='5 12 7 23 45 3 19 8 30 11'
MENU='1
10 3
1
20 1
1
30 5
1
40 2
4
3
2
3
4
5
6'

echo "Experiment 0"
expect "1-D array: sum of 10 20 30"        exp0_1 ""                    "Sum of elements in the array: 60"
expect "1-D array: maximum element"        exp0_1 ""                    "Maximum element in the array: 30"
expect "1-D array: reversed order"         exp0_1 ""                    "30 20 10"
expect "insert 99 at index 1"              exp0_2 "1
99"                                                                     "arr[1] = 99"
expect "insert keeps all elements"         exp0_2 "1
99"                                                                     "(4 elements)"
expect "insert rejects index 9"            exp0_2 "9"                   "Invalid index"
expect "linear search finds 19"            exp0_2_linear_search "$SEARCH
19"                                                                     "Position = 7 (0-based index 6, after 7 comparison(s))"
expect "linear search finds the first"     exp0_2_linear_search "$SEARCH
5"                                                                      "Position = 1 (0-based index 0, after 1 comparison(s))"
expect "linear search misses 100"          exp0_2_linear_search "$SEARCH
100"                                                                    "Element Not Found (after 10 comparisons)"
expect "2-D array doubled"                 exp0_3 ""                    "14 16 18"

echo "Experiment 1 and 2"
expect "merge sort"                        exp1_merge_sort ""           "Sorted array: 3 9 10 27 38 43 82"
expect "quick sort"                        exp2_quick_sort ""           "Sorted array: 3 9 10 27 38 43 82"

echo "Experiment 3"
expect "factorial of 10"                   exp3_1_factorial "10"        "10! = 3628800"
expect "factorial of 12 (the limit)"       exp3_1_factorial "12"        "12! = 479001600"
expect "factorial rejects 13"              exp3_1_factorial "13"        "Please enter a number up to 12"
expect "factorial rejects -4"              exp3_1_factorial "-4"        "not defined for negative numbers"
expect "fibonacci F(8)"                    exp3_2_fibonacci "8"         "F(8) = 21"
expect "fibonacci call count for F(8)"     exp3_2_fibonacci "8"         "F(8): 67"
expect "hanoi with 3 disks"                exp3_3_tower_of_hanoi "3"    "Total number of moves = 7"
expect "hanoi with 20 disks is 2^20 - 1"   exp3_3_tower_of_hanoi "20"   "Total number of moves = 1048575"

echo "Experiment 4"
expect "array queue extracts the max"      exp4_1_priority_queue_array "$MENU"    "Extracted: data = 30, priority = 5"
expect "array queue then serves (10,3)"    exp4_1_priority_queue_array "$MENU"    "Highest priority element: data = 10, priority = 3"
expect "array queue display keeps order"   exp4_1_priority_queue_array "$MENU"    "data = 20, priority = 1"
expect "list queue extracts the max"       exp4_2_priority_queue_linkedlist "$MENU" "Extracted: data = 30, priority = 5"
expect "list queue then serves (10,3)"     exp4_2_priority_queue_linkedlist "$MENU" "Highest priority element: data = 10, priority = 3"
expect "list queue sorts by priority"      exp4_2_priority_queue_linkedlist "$MENU" "data = 40, priority = 2"

echo
echo "$passed/$total checks passed"
[ "$failed" -eq 0 ] || exit 1
