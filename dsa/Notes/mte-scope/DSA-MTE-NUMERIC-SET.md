# DSA Numeric Set · Projected MTE Scope · ECE2104

| Item | Value |
|---|---|
| Course | ECE2104, Data Structures and Algorithms, MUJ Semester 3 |
| Set | 24 numerical problems on the projected MTE scope |
| Aim | the sorting, asymptotics, tree and byte-cost arithmetic the other three sets do not cover |
| Time | 90 to 120 minutes for the whole set at exam pace |
| Book rule | closed book, plain four-function calculator allowed |
| Answer key | last section of this file, 24 entries, numbered 1 to 1 with the problems |
| Computed by | `python3 /tmp/mte_answers.py`, integer and Fraction arithmetic |

**What this set covers that the earlier sets do not.** The recursion set, the stacks and queues set
and the intro and memory set already drill frame strides, capacity logs and offsets. This set drills
the topics the projected list adds: comparison and swap counts for the five sorts, asymptotic
certificates, operation counting on nested and halving loops, the three cases, tree height and node
counts, the complete and full tree formulas, traversal sequences, BST insert search and delete
comparison counts, representation byte cost and padding, and the operation-cost matrix.

**Measured constants.** Every constant below was measured on this machine, not taken from a slide.
Each carries the capture file it comes from.

| Constant | Value | Capture file |
|---|---|---|
| `sizeof(int)` | 4 | `lec05_run_output.txt` |
| `sizeof(void*)` | 8 | `lec05_run_output.txt` |
| `sizeof(Node)` | 16 | `lec05_run_output.txt` |
| allocator usable bytes for a 16 byte request | 24 | `lec05_run_output.txt` |
| allocator charged stride per node | 32 | `lec05_run_output.txt` |
| array base address in the run | `0x7ffe7c3131b0` | `lec05_run_output.txt` |
| sequential array walk | 0.19 ns per element | `lec05_run_output.txt` |
| scattered array walk | 2.46 ns per element | `lec05_run_output.txt` |
| linked list walk | 2.45 ns per element | `lec05_run_output.txt` |
| `sizeof(ArrayStack<int,5>)` | 40 | `lec11_15_run_output.txt` |
| linked stack push, 1 million elements | 16.027 ns | `lec11_15_run_output.txt` |
| linked stack pop, 1 million elements | 8.330 ns | `lec11_15_run_output.txt` |
| array stack push, 1 million elements | 0.639 ns | `lec11_15_run_output.txt` |
| array stack pop, 1 million elements | 0.323 ns | `lec11_15_run_output.txt` |
| array stack push, 2 million operations | 0.688 ns | `lec11_15_run_output.txt` |
| array stack pop, 2 million operations | 0.378 ns | `lec11_15_run_output.txt` |
| linked queue cost per operation | 14.023 ns | `lec11_15_run_output.txt` |
| circular queue cost per operation | 0.531 ns | `lec11_15_run_output.txt` |
| `sizeof(TreeNode)` | 24 | `lec16_recursion_output.txt` |
| stack limit from `getrlimit(RLIMIT_STACK)` | 8192 KiB | `lec16_recursion_output.txt` |
| one suspended call to `linearSumRec` | 48 bytes | `lec16_recursion_output.txt` |
| runaway child frame stride | 32 bytes | `lec16_recursion_output.txt` |

**Capture-anchored problems.** Nine of the 24 name a measured capture file in the problem text, so
their answers are directly checkable against a run: N11, N15, N16, N17, N18, N19, N22, N23 and N24.
`lec16_recursion_output.txt` carries six of the nine, because the tree, the frame strides and the
stack limit all live there. Work those first if time is short.

---

## Group 1 · sorting comparison and swap counts

| Q | Topic | Problem |
|---|---|---|
| N1 | Bubble sort | Sort `{5, 1, 4, 2, 8, 3}` by bubble sort with no early exit. Give the comparison count, the swap count, and the sorted array. Then give the comparison and swap counts for the same code on the already sorted input, and on the reverse sorted input. |
| N2 | Selection sort | Sort the same `{5, 1, 4, 2, 8, 3}` by selection sort. Give the comparison count, the swap count, and say which of the two is input independent. Then give the swap count on the already sorted input. |
| N3 | Insertion sort | Sort the same `{5, 1, 4, 2, 8, 3}` by insertion sort. Give the comparison count and the number of element shifts. Then give both numbers for the best case and the worst case over all inputs of size 6. |
| N4 | Merge sort | Sort `{8, 3, 5, 1, 7, 2, 6, 4}` by top-down merge sort. Give the number of merge calls, the total key comparisons for this input, the sorted output, and the recursion depth. Then give the worst case comparison bound for n = 8 and compare it with your measured count. |
| N5 | Quick sort | Sort `{30, 10, 50, 20, 40}` by quick sort with Lomuto partition and the last element as pivot. Give the swap calls and the data-changing swaps of the first partition, the array after the first partition, each pivot's final index, the deepest recursion level, and the whole-sort data-changing swap count. Then give the recursion depth on the already sorted input. |

## Group 2 · asymptotic certificates

| Q | Topic | Problem |
|---|---|---|
| N6 | Asymptotic notation | Show 3n² + 5n + 2 ∈ O(n²). Give the smallest integer c and the smallest n0 that works with it, verify the inequality at n = n0 − 1 and at n = n0, and give the slack at n = n0. |
| N7 | Asymptotic notation | Show 4n² + 3n ∈ Θ(n²). Give c₁, c₂ and the smallest n0 that work for both bounds together, and verify the tight boundary. |
| N8 | Asymptotic notation | Show 2n + 7 ∈ O(n) with the smallest integer c and its n0, and verify the boundary. Then show that n² is not O(n) by picking c = 1000 and exhibiting one n that breaks it. |

## Group 3 · operation counting on nested and halving loops

| Q | Topic | Problem |
|---|---|---|
| N9 | Finding Complexity | Give the iteration count and the Θ class of `for (i = 1; i <= n; i++) for (j = 1; j <= i; j++) work();`. Evaluate for n = 10 and n = 100. |
| N10 | Finding Complexity | Give the iteration count and the Θ class of `for (i = 1; i <= n; i++) for (j = i; j <= n; j++) for (k = j; k <= n; k++) work();`. Evaluate for n = 5, n = 6 and n = 10. |
| N11 | Finding Complexity | Give the iteration count of `while (n > 1) n = n / 2;` for n = 1000000 and for n = 1048576, with the Θ class. Then use `lec16_recursion_output.txt`: the stack limit is 8192 KiB, one suspended call costs 48 bytes for the linear shape and the runaway child measured 32 bytes. Compute the depth ceiling for both strides, the measured depth of the runaway child, and the difference and the percentage difference. |
| N12 | Finding Complexity | Give the total inner loop iterations and the Θ class of `for (i = 1; i <= n; i++) { j = i; while (j > 1) j = j / 2; }`. Evaluate for n = 16 and check your total against the closed form. |

## Group 4 · best case, worst case, average case

| Q | Topic | Problem |
|---|---|---|
| N13 | Finding Complexity | Linear search over an array of 1024 elements that does contain the key. Give the best case, the worst case and the average number of comparisons, and the count for a search that fails. |
| N14 | Best, worst, average | Insertion sort on 8 elements. Give the comparison count in the best case, in the worst case and in the average case, with the closed form for each. |
| N15 | Best, worst, average | Binary search on a sorted array of 1023 elements. Give the maximum comparison count and say why. Then compare it with linear search on the same array. Anchor: `lec16_recursion_output.txt` measured a complete binary tree of depth 9 at 1023 nodes, which is the balanced shape on those keys. |

## Group 5 · tree height, node counts, complete and full formulas

| Q | Topic | Problem |
|---|---|---|
| N16 | Trees fundamentals | A complete binary tree has height 9 measured in edges. Give the maximum node count and the minimum node count, with the formula for each, and give the number of edges. Anchor: `lec16_recursion_output.txt` reports exactly this node count for a depth 9 tree. |
| N17 | Complete and full trees | State the full binary tree identities n = 2L − 1 and internal = L − 1. Use them to give the leaves and the internal nodes of the 1023 node tree of N16, and to give the node count for a full tree with 100 leaves. Then read the identity backwards for 1023 nodes. Anchor: `lec16_recursion_output.txt` builds exactly this tree, so the leaf count you compute is the leaf count of a measured run. |
| N18 | Trees fundamentals | Give the minimum height and the maximum height for a binary tree of 1000 nodes, of 15 nodes and of 1023 nodes. Anchor: `lec16_recursion_output.txt` puts its 1023 nodes at height 9, which is exactly the minimum. |
| N19 | Trees fundamentals | Check the arithmetic of the measured tree in `lec16_recursion_output.txt`: 1023 nodes at 24 bytes per node, 1023 heap requests, 1023 inorder visits, keys 1 to 1023. Give the total bytes requested, the key sum, the node offsets and the size split. Say why the key sum is independent of the insertion order. |

## Group 6 · traversal sequences

| Q | Topic | Problem |
|---|---|---|
| N20 | Tree types and BST | Insert 8, 3, 10, 1, 6, 14, 4, 7, 13 in that order into an empty BST. Give the height in edges, then the inorder, preorder, postorder and level order sequences, and say which of the four is guaranteed to come out sorted and why. |

## Group 7 · BST insert, search and delete

| Q | Topic | Problem |
|---|---|---|
| N21 | Tree types and BST | For the tree of N20, give the number of key comparisons each insert costs, the total over all nine inserts, the comparisons to find 7, the comparisons to fail on 9, and the average comparisons for a successful search. Then contrast with a binary search on the same nine sorted keys. |
| N22 | Tree types and BST | Delete 3 from the tree of N20. It has two children, so give the three cases of deletion in one line each, name the node that replaces 3 and why, give the comparisons the delete walk costs and the pointer work, and give the inorder, the preorder and the height afterwards. Then check the resulting key set against the key sum of N19, whose measured values come from `lec16_recursion_output.txt`. |

## Group 8 · representation byte cost, padding and the operation-cost matrix

| Q | Topic | Problem |
|---|---|---|
| N23 | Array vs linked list | Compare `int a[1024]` with a 1024 node linked list. Give the array bytes, the bytes requested, the allocator usable bytes and the charged bytes for the list, and the ratio of charged bytes to array bytes. Then use the measured base from `lec05_run_output.txt` to give `&a[512]`, and check the capture's own five element example. Finally give the padding paid by the three struct forms and by the list node in `lec05_run_output.txt`, and the byte split of `ArrayStack<int,5>` in `lec11_15_run_output.txt`. |
| N24 | Different Operations | Fill the operation-cost matrix for the array against the linked representation over access, insert at the front, insert at the back, delete at the front and search, with the O cost of each. Then put the measured per element and per operation numbers from `lec05_run_output.txt` and `lec11_15_run_output.txt` into the same table and compute the ratios that matter. |

---

## Answer key

24 entries, numbered 1 to 1 with the problems above. Every step's arithmetic is written out.

### N1 · Bubble sort

Input `{5, 1, 4, 2, 8, 3}`, n = 6.

Comparisons: the outer loop runs n − 1 = 5 times with inner lengths 5, 4, 3, 2, 1, so the total is
n(n − 1) ÷ 2 = 6 × 5 ÷ 2 = **15**. This is input independent when there is no early exit.

Swaps, traced pass by pass: pass 1 does 4 (5 with 1, 5 with 4, 5 with 2, 8 with 3) and leaves
`1 4 2 5 3 8`; pass 2 does 2 (4 with 2, 5 with 3) and leaves `1 2 4 3 5 8`; pass 3 does 1 (4 with 3)
and leaves `1 2 3 4 5 8`; passes 4 and 5 do 0. Total **7 swaps**.

Sorted output: `1 2 3 4 5 8`.

Already sorted input: comparisons 15, swaps **0**. Reverse sorted input `{8, 5, 4, 3, 2, 1}`:
comparisons 15, swaps **15**, the worst case. Bubble sort reaches its best case only with the
early-exit flag, which for n = 6 cuts it to 5 comparisons, 0 swaps and 1 pass.

### N2 · Selection sort

Input `{5, 1, 4, 2, 8, 3}`.

Comparisons: the outer loop scans the unsorted tail with lengths 5, 4, 3, 2, 1, so
n(n − 1) ÷ 2 = 15, always, whatever the input.

Swaps: i = 0 finds min 1 at index 1 and swaps → `1 5 4 2 8 3`; i = 1 finds min 2 at index 3 and
swaps → `1 2 4 5 8 3`; i = 2 finds min 3 at index 5 and swaps → `1 2 3 5 8 4`; i = 3 finds min 4 at
index 5 and swaps → `1 2 3 4 8 5`; i = 4 finds min 5 at index 5 and swaps → `1 2 3 4 5 8`. Total
**5 swaps**. Already sorted input: comparisons 15 (unchanged), swaps **0**.

The lesson: comparisons are input independent at n(n − 1) ÷ 2, swaps are not. Selection sort never
does more than n − 1 = 5 swaps, which is why it is the choice when a swap is expensive.

### N3 · Insertion sort

Comparison counting rule: the inner `while` counts one comparison each time it tests `a[j] > key`,
including the test that fails and stops the scan.

Input `{5, 1, 4, 2, 8, 3}`: i = 1 gives 1 comparison and 1 shift; i = 2 gives 2 comparisons and
1 shift; i = 3 gives 3 comparisons and 2 shifts; i = 4 gives 1 comparison and 0 shifts; i = 5 gives
4 comparisons and 3 shifts. Comparisons = 1 + 2 + 3 + 1 + 4 = **11**. Shifts = 1 + 1 + 2 + 0 + 3 =
**7**. Sorted output `1 2 3 4 5 8`.

Best case, already sorted: 1 comparison per outer step and no shifts, so n − 1 = **5 comparisons,
0 shifts**. Worst case, reverse sorted: every element walks to the front, so
n(n − 1) ÷ 2 = **15 comparisons, 15 shifts**.

### N4 · Merge sort

Input `{8, 3, 5, 1, 7, 2, 6, 4}`.

Merge calls: n − 1 = 7, since the recursion tree has n leaves and n − 1 internal merges.

Comparison trace, level by level. Pairs: `8,3` needs 1, `5,1` needs 1, `7,2` needs 1, `6,4` needs 1.
Fours: `3,8` with `1,5` needs 3, `2,7` with `4,6` needs 3. Top: `1,3,5,8` with `2,4,6,7` needs 7.
Total = 1 + 1 + 1 + 1 + 3 + 3 + 7 = **17 comparisons**. Sorted output `1 2 3 4 5 6 7 8`.

Recursion depth = ⌈log₂ 8⌉ = **3**.

Worst case bound for n = 8: n·⌈log₂ n⌉ − 2^⌈log₂ n⌉ + 1 = 8 × 3 − 2³ + 1 = 24 − 8 + 1 = **17**, which
is exactly what this input cost. Best case bound: (n ÷ 2)·log₂ n = 4 × 3 = **12** comparisons.

### N5 · Quick sort

Input `{30, 10, 50, 20, 40}`, Lomuto partition, last element as pivot.

Partition 1, lo = 0, hi = 4, pivot = 40, i = −1. j = 0: 30 ≤ 40, so i = 0 and swap(0, 0), a self
swap. j = 1: 10 ≤ 40, so i = 1 and swap(1, 1), a self swap. j = 2: 50 is not ≤ 40, nothing.
j = 3: 20 ≤ 40, so i = 2 and swap(2, 3), the first real move, giving `30 10 20 50 40`. Finally
swap(i + 1, hi) = swap(3, 4), the second real move, giving `30 10 20 40 50` and putting the pivot at
index **3**.

So partition 1 does **4 swap calls of which 2 carry data**. The four pairs are (0,0) self, (1,1)
self, (2,3) real, (3,4) real.

Left part `{30, 10, 20}` with pivot 20: j = 0 leaves 30 alone, j = 1 gives i = 0 and swap(0, 1) real,
then swap(1, 2) real. Pivot 20 lands at index **1** and the array is `10 20 30 40 50`. Pivot landing
indices over the whole sort: **3, then 1**. Deepest recursion level: **2** nested partition calls
below the first.

Whole-sort data-changing swaps: 2 from partition 1 plus 2 from partition 2 = **4**.

Already sorted input `{10, 20, 30, 40, 50}` is the worst shape for the last-element pivot: every
element is ≤ the pivot, so nothing moves (0 data-changing swaps) but the partition peels off one
element at a time, giving a recursion depth of **4 = n − 1**.

### N6 · Certificate for 3n² + 5n + 2 ∈ O(n²)

Need 3n² + 5n + 2 ≤ c·n² for all n ≥ n0. Subtracting 3n² gives the requirement
(c − 3)n² − 5n − 2 ≥ 0.

With c = 4 that is n² − 5n − 2 ≥ 0, whose positive root is (5 + √(25 + 8)) ÷ 2 = (5 + √33) ÷ 2 =
5.3723, so the smallest integer n0 is **6**.

Check n = 5: 3(25) + 25 + 2 = 102 against 4(25) = 100, so 102 ≤ 100 is false. Check n = 6:
3(36) + 30 + 2 = 140 against 4(36) = 144, so 140 ≤ 144 holds. Slack at n0: 144 − 140 = **4**.

c = 3 is impossible because it needs 5n + 2 ≤ 0. Answer: **c = 4, n0 = 6**.

### N7 · Certificate for 4n² + 3n ∈ Θ(n²)

Lower bound: 4n² ≤ 4n² + 3n because 3n ≥ 0 for every n ≥ 1, so c₁ = **4** works with n0 = 1.

Upper bound: find the smallest integer c with 4n² + 3n ≤ c·n². Subtracting gives
(c − 4)n² − 3n ≥ 0. With c = 5 that is n² − 3n ≥ 0, which holds for n ≥ 3. Check n = 2:
4(4) + 6 = 22 against 5(4) = 20, fails. Check n = 3: 4(9) + 9 = 45 against 5(9) = 45, holds exactly,
so the boundary is tight. c = 4 is impossible because it needs 3n ≤ 0.

Answer: **c₁ = 4 for n ≥ 1, c₂ = 5 for n ≥ 3, so n0 = 3 certifies both bounds at once.**

### N8 · 2n + 7 ∈ O(n), and n² ∉ O(n)

Upper bound: need 2n + 7 ≤ c·n. With c = 3 that is 2n + 7 ≤ 3n, that is n ≥ 7. Check n = 6:
2(6) + 7 = 19 against 3(6) = 18, fails. Check n = 7: 2(7) + 7 = 21 against 3(7) = 21, holds exactly.
c = 2 is impossible because it needs 7 ≤ 0. Answer: **c = 3, n0 = 7**, tight at the boundary.

Refuting n² ∈ O(n): take c = 1000 and n = c + 1 = 1001. Then n² = 1001² = 1002001 and
c·n = 1000 × 1001 = 1001000, so n² ≤ c·n is false. In general n² ≤ c·n forces n ≤ c, which fails for
n = max(c, n0) + 1, so no pair (c, n0) can exist.

### N9 · Double loop

`for (i = 1; i <= n; i++) for (j = 1; j <= i; j++) work();`

The inner body runs i times for each i, so the count is Σ(i = 1 to n) i = n(n + 1) ÷ 2.

n = 10: 10 × 11 ÷ 2 = **55**. n = 100: 100 × 101 ÷ 2 = **5050**. Both match a direct count.

Θ class: **Θ(n²)** with the constant 1/2.

### N10 · Triple loop

`for (i = 1; i <= n; i++) for (j = i; j <= n; j++) for (k = j; k <= n; k++) work();`

The count is C(n + 2, 3) = n(n + 1)(n + 2) ÷ 6.

n = 5: 5 × 6 × 7 ÷ 6 = 210 ÷ 6 = **35**, confirmed by a direct count.
n = 6: 6 × 7 × 8 ÷ 6 = 336 ÷ 6 = **56**. n = 10: 10 × 11 × 12 ÷ 6 = 1320 ÷ 6 = **220**.

Θ class: **Θ(n³)** with the constant 1/6.

### N11 · Halving loop and the measured stack ceiling

`while (n > 1) n = n / 2;` runs ⌊log₂ n⌋ times, because the value halves until it reaches 1.

n = 1000000: 2^19 = 524288 ≤ 1000000 < 2^20 = 1048576, so the count is **19**.
n = 1048576 = 2^20: the count is **20**. Θ class: **Θ(log n)**.

From `lec16_recursion_output.txt`: the stack limit is 8192 KiB = 8192 × 1024 = **8388608 bytes**.

| Stride | Division | Ceiling |
|---|---|---|
| 48 bytes per call, the linear shape | 8388608 ÷ 48 | **174762** calls, remainder 32 bytes |
| 96 bytes per call, the four argument shape | 8388608 ÷ 96 | **87381** calls, remainder 32 bytes |
| 32 bytes per call, the runaway child | 8388608 ÷ 32 | **262144** calls, remainder 0 |

The capture's own numbers: 8388608 ÷ 48 = 174762 (it prints 174762, which is the floor, so the
ceiling in whole calls is 174762 and the 174763rd call overruns). The runaway child's predicted
ceiling is 8388608 ÷ 32 = 262144 and its **measured** depth was **261579**.

Difference: 262144 − 261579 = **565 calls short**, which as a percentage is
565 ÷ 262144 = **0.215530%** lower. At 32 bytes per frame the measured depth is 261579 × 32 = 8370528
bytes = 8174 KiB against the 8192 KiB limit, so the guard page ate the last 18 KiB. The same capture
also measured a safe run at depth 87381 = 174762 ÷ 2, which used about 4095 KiB of the 8192 KiB.

### N12 · Nested halving

`for (i = 1; i <= n; i++) { j = i; while (j > 1) j = j / 2; }`

The inner count for a given i is ⌊log₂ i⌋, so the total is Σ(i = 1 to n) ⌊log₂ i⌋.

For n = 16 = 2⁴: i = 1 gives 0; i = 2 and 3 give 1 each; i = 4 to 7 give 2 each; i = 8 to 15 give 3
each; i = 16 gives 4. Total = 0 + 2(1) + 4(2) + 8(3) + 1(4) = 0 + 2 + 8 + 24 + 4 = **38**.

Closed form check for n = 2^k: Σ(j·2^j, j = 0 to k − 1) + k = (k − 2)·2^k + 2 + k. With k = 4:
(4 − 2) × 16 + 2 + 4 = 32 + 2 + 4 = **38**, which matches.

Θ class: **Θ(n log n)**.

### N13 · Linear search, all cases

Array of 1024 elements that does contain the key. Best case: the key is in the first slot, **1
comparison**. Worst case: the key is in the last slot, **1024 comparisons**.

Average: the key is equally likely to be in any slot, so the total over all 1024 positions is
1 + 2 + ... + 1024 = 1024 × 1025 ÷ 2 = 524800, and the average is 524800 ÷ 1024 = 1025 ÷ 2 =
**512.5 comparisons**, which is (n + 1) ÷ 2.

Failing search: the scan always runs to the end, so it is **1024 comparisons** every time, with no
best or average case at all.

### N14 · Insertion sort, three cases for n = 8

Best: the input is already sorted, so each of the 7 outer steps tests once and stops, giving
n − 1 = **7 comparisons and 0 shifts**.

Worst: the input is reverse sorted, so element i walks past all i − 1 elements before it, giving
n(n − 1) ÷ 2 = 8 × 7 ÷ 2 = **28 comparisons and 28 shifts**.

Average: on a random permutation each element walks about half way, giving n(n − 1) ÷ 4 =
8 × 7 ÷ 4 = **14 comparisons**.

### N15 · Binary search against linear search on 1023 elements

Maximum comparisons = ⌈log₂(n + 1)⌉ = ⌈log₂ 1024⌉ = ⌈10⌉ = **10**. Each comparison halves the
remaining range, and 2¹⁰ = 1024 ≥ 1023, so 9 halvings cannot cover 1023 elements.

Anchor: `lec16_recursion_output.txt` measured a complete binary tree of depth 9 with exactly 1023
nodes, which is 2^(9+1) − 1, the perfect shape for those keys. A search down that tree walks 9 edges
and visits at most 10 nodes, the same number.

Linear search on the same 1023 elements needs up to **1023 comparisons**, so the ratio is
1023 ÷ 10 = **102.30×** in favour of the halving search.

### N16 · Complete binary tree of height 9

Maximum nodes for height h = 2^(h + 1) − 1 = 2^10 − 1 = 1024 − 1 = **1023**, the perfect tree, in
which every level is full.

Minimum nodes for a complete tree of height h = 2^h = 2^9 = **512**. The derivation: in a complete
tree every level except possibly the last must be full, so levels 0 to h − 1 contribute
2^0 + 2^1 + ... + 2^(h − 1) = 2^h − 1 = 511 nodes, and the tree still has to reach height h, so at
least 1 more node sits at level h, giving 511 + 1 = **512**.

Edges: n − 1 = 1023 − 1 = **1022**, which is the count of parent to child links.

Anchor: `lec16_recursion_output.txt` prints "a complete binary tree of depth 9: nodes =
2^(10) − 1 = 1023", matching the maximum formula exactly. Note that it also prints
`sizeof(TreeNode) = 24` for a two pointer tree node, against 16 for the one pointer list node,
because the second pointer costs another 8 bytes and the padding stays at 4.

### N17 · Full binary tree identities

The identities: a full (proper) binary tree, where every node has 0 or 2 children, with L leaves has
internal = L − 1 internal nodes and n = internal + L = 2L − 1 nodes in total.

For the 1023 node tree of N16, which is perfect and therefore full:
L = (n + 1) ÷ 2 = (1023 + 1) ÷ 2 = 1024 ÷ 2 = **512 leaves**, and internal = L − 1 =
**511 internal nodes**. Check: 512 + 511 = 1023.

For 100 leaves: n = 2(100) − 1 = **199 nodes** and internal = 100 − 1 = **99**. Check:
99 + 100 = 199.

Read backwards for 1023 nodes: L = (1023 + 1) ÷ 2 = **512**, and 2(512) − 1 = 1023 confirms it.

### N18 · Height bounds from the node count

Minimum height for n nodes = ⌈log₂(n + 1)⌉ − 1, which is the height of the shallowest complete tree
that can hold them. Maximum height = n − 1, the degenerate chain where every node has one child.

| n | ⌈log₂(n + 1)⌉ | Minimum height | Maximum height |
|---|---|---|---|
| 15 | 4, since 2⁴ = 16 | 3 | 14 |
| 1000 | 10, since 2¹⁰ = 1024 | 9 | 999 |
| 1023 | 10, since 2¹⁰ = 1024 | 9 | 1022 |

Anchor: `lec16_recursion_output.txt` puts its 1023 nodes at height 9, which is exactly
⌈log₂ 1024⌉ − 1 = 10 − 1 = 9, so the measured tree sits on the minimum height for its node count.

### N19 · The measured tree, arithmetic checked

| Item | Arithmetic | Value | Capture line |
|---|---|---|---|
| nodes | 2^(9 + 1) − 1 | 1023 | "nodes = 2^(10) − 1 = 1023" |
| bytes requested | 1023 × 24 | 24552 | "24552 bytes requested" |
| heap requests | 1 per node | 1023 | "1023 heap requests" |
| inorder visits | 1 per node | 1023 | "recursive inorder visits = 1023" |
| key sum | 1023 × 1024 ÷ 2 | 523776 | "523776" |

Node offsets and split: `sizeof(TreeNode) = 24`, data at offset 0, left at
offset **8**, right at offset **16**. The split is 4 payload + **4 padding** at offsets 4 to 7 + 8 for
the left pointer + 8 for the right pointer = 4 + 4 + 8 + 8 = **24**, which matches the capture's
"24 usable bytes for that 24 byte request" and its note that glibc's own bookkeeping lives outside
those 24 bytes, so 24552 is bytes requested and not bytes resident.

Why the key sum is order independent: the sum of a set is a commutative operation, so any insertion
order that puts the same 1023 keys into the tree gives the same inorder multiset and the same sum.
The insertion order changes the **shape** and therefore the height and the comparison counts, not the
key multiset.

### N20 · Traversal sequences

Insert order 8, 3, 10, 1, 6, 14, 4, 7, 13 gives this tree, with each key placed by the BST rule:

```
            8
          /   \
         3     10
        / \      \
       1   6      14
          / \    /
         4   7  13
```

Height in edges = **3**, from 8 down to 4, to 7, or to 13.

| Traversal | Sequence |
|---|---|
| inorder, left node right | 1, 3, 4, 6, 7, 8, 10, 13, 14 |
| preorder, node left right | 8, 3, 1, 6, 4, 7, 10, 14, 13 |
| postorder, left right node | 1, 4, 7, 6, 3, 13, 14, 10, 8 |
| level order, breadth first | 8, 3, 10, 1, 6, 14, 4, 7, 13 |

Inorder is the one guaranteed to come out sorted, because it emits the whole left subtree, which the
BST rule makes smaller, then the node, then the whole right subtree, which is larger. The result is
verified: `1, 3, 4, 6, 7, 8, 10, 13, 14` is the sorted key set.

### N21 · BST comparison counts

Insert comparisons, one per node visited on the search path, with the empty tree costing 0:

| Key | Comparisons | Why |
|---|---|---|
| 8 | 0 | first key, the tree is empty |
| 3 | 1 | against 8, go left |
| 10 | 1 | against 8, go right |
| 1 | 2 | against 8, then 3, go left |
| 6 | 2 | against 8, then 3, go right |
| 14 | 2 | against 8, then 10, go right |
| 4 | 3 | against 8, 3, 6, go left |
| 7 | 3 | against 8, 3, 6, go right |
| 13 | 3 | against 8, 10, 14, go left |

Total insert comparisons = 0 + 1 + 1 + 2 + 2 + 2 + 3 + 3 + 3 = **17**.

Search 7: compare 8, then 3, then 6, then 7, so **4 comparisons** and found.
Search 9: compare 8 (9 > 8, go right), then 10 (9 < 10, go left), then the left child of 10 is null,
so **2 key comparisons** and then a null check: not found.

Comparisons for every key: 8 costs 1, 3 costs 2, 10 costs 2, 1 costs 3, 6 costs 3, 14 costs 3, 4 costs
4, 7 costs 4, 13 costs 4. Sum = 1 + 2 + 2 + 3 + 3 + 3 + 4 + 4 + 4 = **26**, so the average successful
search is 26 ÷ 9 = **2.8889 comparisons**.

Binary search on the same nine sorted keys needs at most ⌈log₂(9 + 1)⌉ = **4 comparisons**, with a
flat 4 at every position, against the BST's 1 to 4 depending on the shape. Same bound, different
distribution: the BST is faster for the root and slower for the deepest leaves.

### N22 · Deleting a node with two children

The three cases in one line each. Case 1, no children: unlink from the parent and free. Case 2, one
child: hand the child up to the parent in place of the node. Case 3, two children: replace the node's
key with its inorder successor, then delete that successor, which by construction has no left child,
so it collapses to case 1 or case 2.

Deleting **3** from the tree of N20 triggers case 3, because 3 has the children 1 and 6. The inorder
successor of 3 is the leftmost node of its right subtree: the right subtree is 6 with children 4 and
7, and its leftmost node is **4**, so 4 is the key that replaces 3. It replaces it because the
successor is the smallest key that is still larger than 3, so the ordering holds with no other node
moving. Node 4 is a leaf, so its own removal is case 1.

Comparisons the delete walk costs: 2 to find 3 (against 8, then 3), then 2 more to descend to the
successor (against 6, then 4), so **4 comparisons**. Pointer work: 1 write to lift 4's key into the
node that held 3, then 1 write to unlink the leaf.

After the delete: inorder `1, 4, 6, 7, 8, 10, 13, 14`; preorder `8, 4, 1, 6, 7, 10, 14, 13`; height
still **3**. The key set loses exactly one key, 9 keys become 8, and the new inorder sum is
1 + 4 + 6 + 7 + 8 + 10 + 13 + 14 = 63, which is the old sum 1 + 3 + 4 + 6 + 7 + 8 + 10 + 13 + 14 = 66
minus the deleted key 3, that is 66 − 3 = 63, matching.

### N23 · Array against node representation, and padding

| Quantity | Arithmetic | Value |
|---|---|---|
| `int a[1024]` | 1024 × 4 | 4096 bytes |
| 1024 nodes, requested | 1024 × 16 | 16384 bytes |
| 1024 nodes, allocator usable | 1024 × 24 | 24576 bytes |
| 1024 nodes, charged at the measured stride | 1024 × 32 | 32768 bytes |
| ratio, charged against array | 32768 ÷ 4096 | 8× |
| ratio, requested against array | 16384 ÷ 4096 | 4× |

So the linked form costs **4× the payload** in bytes requested and **8×** in bytes the allocator
actually charges, and the same 1024 integers in an array cost 4096 bytes.

`&a[512]` from the measured base in `lec05_run_output.txt`: the capture prints
`base &a[0] = 0x7ffe7c3131b0` with a 4 byte stride, so
`&a[512] = 0x7ffe7c3131b0 + 512 × 4 = 0x7ffe7c3131b0 + 2048 = 0x7ffe7c3139b0`.

The capture's own five element example: the array of 5 ints is 5 × 4 = **20 bytes**, the same 5
values as nodes charge 5 × 32 = **160 bytes**, and 160 ÷ 20 = **8**, the same ratio at small scale.

Padding paid, from the padding family in `lec05_run_output.txt`:

| Struct | Fields | sizeof | Padding | Where the padding sits |
|---|---|---|---|---|
| P1 | int, char, char | 8 | 2 | after `c`, to round 6 up to 8 |
| P2 | char, int, char | 12 | 6 | 3 after `a` to align `b` at 4, 3 after `c` to round to 12 |
| P3 | char, char, int | 8 | 2 | 2 after `b` to align `c` at 4 |
| Node | int, Node* | 16 | 4 | 4 after `data`, so `next` starts at offset 8 |

Same fields, same count, three different footprints: 8, 12 and 8 bytes. Reordering the fields is a
free memory win.

The byte split from `lec11_15_run_output.txt`: `sizeof(ArrayStack<int,5>) = 40`, made of 5 × 4 = 20
bytes of array, **4 bytes of padding** at offsets 20 to 23, and 16 bytes of bookkeeping (`top_` at
offset 24, `refusals_` at offset 32), so 20 + 4 + 16 = 40.

### N24 · Operation-cost matrix

The O costs, one fact per row.

| Operation | Array | Linked |
|---|---|---|
| Access element i | O(1), via `base + i × sizeof(T)` | O(n), one dependent load per hop |
| Insert at the front | O(n), everything shifts | O(1), two pointer writes |
| Insert at the back | O(1) amortised, O(n) on a grow | O(1) with a tail pointer, O(n) without |
| Delete at the front | O(n), everything shifts back | O(1), one pointer write |
| Search a value | O(n) | O(n) |

The measured costs, from `lec05_run_output.txt` and `lec11_15_run_output.txt`.

| Operation | Array | Linked | Ratio |
|---|---|---|---|
| Walk one element | 0.19 ns sequential | 2.45 ns | 12.64× |
| Walk one element, array visited out of order | 2.46 ns | 2.45 ns | 12.69× against sequential |
| Push, 1 million elements | 0.639 ns | 16.027 ns | 25.08× |
| Pop, 1 million elements | 0.323 ns | 8.330 ns | 25.79× |
| Push and pop average, 2 million operations, array | (0.688 + 0.378) ÷ 2 = 0.533 ns | | |
| One queue operation | circular 0.531 ns | linked 14.023 ns | 26.42× |
| Memory per element | 4 bytes | 32 bytes charged | 8× |

The ratio arithmetic, and which figures are the capture's own. The capture prints 12.69× for the
scattered walk against the sequential one and 12.64× for the linked walk. Recomputing from the
rounded two decimal figures gives 2.46 ÷ 0.19 = **12.95×** and 2.45 ÷ 0.19 = **12.89×**, so the
printed per element times are rounded and the capture's **printed ratios are the authoritative
ones**. The stack pop ratio is 8.330 ÷ 0.323 = **25.79×**, exact from the printed figures. The push
ratio is 16.027 ÷ 0.639 = **25.08×**. The queue ratio recomputes to 14.023 ÷ 0.531 = **26.41×**
against the **26.42×** the capture prints, the third rounding artifact in the same table. The
capture's per push spread line is top end 0.1 ns against bottom end 484.7 ns
with a printed ratio of 4267.2×, where 20000 pushes at 484.7 ns is 20000 × 484.7 = 9694000 ns =
**9.694 ms** and 20000 pushes at 0.1 ns is 2000 ns = **2 µs**; the 0.1 ns figure is printed rounded
to one decimal, so 4267.2× is the capture's number and not a recomputed one.

The shape lesson: access element i separates the two representations, and it is the only row on
which they differ in the O column. The other rows trade a shift against a pointer, which is where
the constant factors come from, and the measured constants are the 25× and 26× above.

---

## Files and sources

| Source | What was taken |
|---|---|
| `~/learning/cpp/exercises/lec05_run_output.txt` | sizes, array base, padding family, per element walk timings, node stride |
| `~/learning/cpp/exercises/lec11_15_run_output.txt` | stack and queue sizes, push and pop timings, queue cost ratio |
| `~/learning/cpp/exercises/lec16_recursion_output.txt` | tree node counts, frame strides, stack limit, runaway depth, key sum |
| `python3 /tmp/mte_answers.py` | every non measured number in this set, in integer and Fraction arithmetic |

Every number that is not quoted from a capture above was produced by that script. Nothing in this
set is hand-waved.
