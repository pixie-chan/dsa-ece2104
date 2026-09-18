# DSA Mock Paper · Projected MTE Scope · ECE2104

| Item | Value |
|---|---|
| Course | ECE2104, Data Structures and Algorithms, MUJ Semester 3 |
| Paper | Mid-Term Examination, projected scope |
| Exam window | 21 to 29 September 2026 |
| Book rule | closed book, no notes, no calculator beyond a plain four-function one |
| Time allowed | 60 minutes |
| Paper total | 40 marks |
| Printed raw total | 100 marks |
| Scale | printed score = raw score × 0.4, so one raw mark is 0.4 exam marks |
| Questions | 40 |
| Scope | the 14 topics of the teacher's projected list, nothing else |

**How to use this paper.** Sit it closed book in one 60 minute sitting. Do the arithmetic on
paper, because that is where the marks are lost, not in the definitions. The answer key is the
last section of this file, numbered 1 to 1 with the questions, and every numerical answer there
shows the arithmetic rather than the value alone.

**Marking line, questions per topic.** All 14 projected topics are examined. The count in the
last column is the number of questions in this paper that carry that topic tag.

| # | Topic tag | Questions | Count |
|---|---|---|---|
| 1 | Array | A3, B4, C1 | 3 |
| 2 | Pointers | A4, B5, C2 | 3 |
| 3 | Stack by Array | A5, B6, C3 | 3 |
| 4 | Queue | A6, B7, C4 | 3 |
| 5 | Linked List | A7, B8, C5 | 3 |
| 6 | Merge Sort | B9, C6, D5 | 3 |
| 7 | Quick Sort | B10, C7 | 2 |
| 8 | Algorithm fundamentals | A2, B1 | 2 |
| 9 | Asymptotic notation | A1, B2, C8 | 3 |
| 10 | Finding Complexity | B3, B15, C9 | 3 |
| 11 | Trees fundamentals | A8, B11, D3 | 3 |
| 12 | Formulation in C | A10, B14, D1 | 3 |
| 13 | Tree types and BST | A9, B12, C10, D4 | 4 |
| 14 | Different Operations | B13, D2 | 2 |
| | **total** | | **40** |

**Marking plan by section.**

| Section | Questions | Marks each | Raw subtotal |
|---|---|---|---|
| A, one-line definition or output | 10 | 1 | 10 |
| B, short answer and trace | 15 | 10 of them at 3, 5 of them at 2 | 40 |
| C, numerical | 10 | 3 | 30 |
| D, code writing or design | 5 | 4 | 20 |
| **total** | **40** | | **100** |

Section B carries 3 marks for questions B1, B2, B3, B5, B7, B8, B9, B10, B12, B13 and 2 marks for
B4, B6, B11, B14, B15. That split is the only way to reach exactly 100 raw marks while keeping
Section A at 1 mark and Section D at 4 marks.

Every constant used below was measured on this machine and is quoted with its capture file:

| Constant | Value | Capture |
|---|---|---|
| `sizeof(int)` | 4 | `lec05_run_output.txt` |
| `sizeof(void*)` | 8 | `lec05_run_output.txt` |
| `sizeof(Node{int; Node*})` | 16 | `lec05_run_output.txt`, `lec11_15_run_output.txt` |
| allocator stride per node | 32 | `lec05_run_output.txt` |
| allocator usable bytes per 16 byte request | 24 | `lec05_run_output.txt` |
| `sizeof(ArrayStack<int,5>)` | 40 | `lec11_15_run_output.txt` |
| recursion frame, one argument | 48 bytes | `lec16_recursion_output.txt` |
| recursion frame, four arguments | 96 bytes | `lec16_recursion_output.txt` |
| stack limit, `getrlimit(RLIMIT_STACK)` | 8192 KiB | `lec16_recursion_output.txt` |

---

## Section A · one line each · 1 mark each · 10 marks

| Q | Marks | Topic tag | Question |
|---|---|---|---|
| A1 | 1 | [Asymptotic notation] | Write the formal definition of Θ(g(n)) in one line. |
| A2 | 1 | [Algorithm fundamentals] | Define an algorithm and name its five properties. |
| A3 | 1 | [Array] | `int a[5] = {10,20,30,40,50};` starts at `0x7ffe7c3131b0`. Give `&a[3]` and the value stored there. |
| A4 | 1 | [Pointers] | State `sizeof(void*)` on a 64 bit machine and say why. Then say how far `(char*)p + 1` moves against `(int*)p + 1`. |
| A5 | 1 | [Stack by Array] | A 5 slot array stack is pushed 10, then 20, then 30. Give the output of `pop()`, then `pop()`, then `top()`. |
| A6 | 1 | [Queue] | Define a FIFO queue in one line. Then give the two values that leave when 10, 20, 30 are enqueued and `dequeue()` is called twice. |
| A7 | 1 | [Linked List] | The list is `10 -> 20 -> 30 -> NULL`. Give the traversal output and `sizeof` of one node. |
| A8 | 1 | [Trees fundamentals] | Define root, leaf, height and degree, one line each. |
| A9 | 1 | [Tree types and BST] | State the binary search tree ordering invariant in one line. |
| A10 | 1 | [Formulation in C] | Give `sizeof` for `struct Node { int data; Node* next; };` on a 64 bit machine and say where the padding sits. |

---

## Section B · short answer and trace · 2 or 3 marks each · 40 marks

| Q | Marks | Topic tag | Question |
|---|---|---|---|
| B1 | 3 | [Algorithm fundamentals] | List the five properties of an algorithm. Say which one a recursion with no base case violates, and quote the measured evidence for it. |
| B2 | 3 | [Asymptotic notation] | Show that 3n² + 5n + 2 ∈ O(n²). Give the smallest integer c and the smallest n0 that work with it, and verify at n = n0. |
| B3 | 3 | [Finding Complexity] | Give the exact iteration count and the Θ class of `for (i = 1; i <= n; i++) for (j = 1; j <= i; j++) work();`. Evaluate for n = 10 and n = 100. |
| B4 | 2 | [Array] | `int a[5]` starts at `0x7ffe7c3131b0` and `sizeof(int) = 4`. Give the address of `a[3]` and of `a[4]`, and say in one line why this access is O(1) while a list walk is O(n). |
| B5 | 3 | [Pointers] | `struct Node { int data; Node* next; };` is 16 bytes and not 12. Explain why. Then count the pointer writes in `Node* p = new Node(v); p->next = head; head = p;` and say what breaks if the two statements are written in the other order. |
| B6 | 2 | [Stack by Array] | Seven values, 1 to 7, are pushed into a 5 slot array stack. Give the number refused, the contents afterwards, and the drain output. |
| B7 | 3 | [Queue] | A circular queue has capacity 5, `front = 2`, `rear = 4`. Give the element count and the slot the next enqueue writes, then the new rear. Then explain why the naive array queue can never enqueue again once it has been fully drained. |
| B8 | 3 | [Linked List] | Given `10 -> 20 -> 40 -> 50 -> NULL`, insert 30 so the list becomes sorted, then delete the last node. Give the pointer writes for each step and the cost of each. |
| B9 | 3 | [Merge Sort] | Trace merge sort on `{8, 3, 5, 1, 7, 2, 6, 4}`. Draw the split tree, give the merge at each level, and give the total number of key comparisons. |
| B10 | 3 | [Quick Sort] | Take `{30, 10, 50, 20, 40}` with Lomuto partition and the last element as pivot. Give every swap of the first partition, the array after it, and the pivot's final index. |
| B11 | 2 | [Trees fundamentals] | A complete binary tree has height 9. Give the maximum and the minimum number of nodes, with the formula for each. |
| B12 | 3 | [Tree types and BST] | Insert 8, 3, 10, 1, 6, 14, 4, 7, 13 into an empty BST. Draw the tree, give its height in edges, and give the inorder sequence. |
| B13 | 3 | [Different Operations] | Compare the array and the linked list on four operations: access element i, insert at the front, delete at the front, and search a value. Give the O cost and, where the capture has it, the measured nanoseconds per element. |
| B14 | 2 | [Formulation in C] | Write the declaration of a singly linked list node and of a head pointer. Then write the signature of a function that inserts at the front and changes the caller's head. Explain what the `&` in that signature does and what breaks without it. |
| B15 | 2 | [Finding Complexity] | Give the Θ class of `for (i = 1; i <= n; i++) { j = i; while (j > 1) j = j / 2; }` and its exact inner iteration count for n = 16. |

---

## Section C · numerical · 3 marks each · 30 marks

| Q | Marks | Topic tag | Question |
|---|---|---|---|
| C1 | 3 | [Array] | `int a[8]` starts at `0x1000` and `sizeof(int) = 4`. Give the total bytes, `&a[5]` and `&a[7]`, and the number of computing steps each address costs. |
| C2 | 3 | [Pointers] | Give `sizeof(int)`, `sizeof(void*)` and `sizeof(Node{int data; Node* next;})`. Break the 16 bytes into payload, padding and pointer, give the padding offset, and say how far `p + 1` advances when `p` is a `Node*`. |
| C3 | 3 | [Stack by Array] | `sizeof(ArrayStack<int,5>)` is 40 and the five ints alone are 20 bytes. Break the 40 into array, padding and bookkeeping, give the three offsets, and say how many pushes out of seven are refused. |
| C4 | 3 | [Queue] | A circular queue has capacity 5, `front = 2` and `rear = 4`. Compute the count with the formula, give the next enqueue slot and the new rear, and state the count after that enqueue and whether the queue is then full. |
| C5 | 3 | [Linked List] | A linked list holds 300 integers. Give the bytes requested, the bytes the allocator reports as usable, the bytes charged at the measured stride, the bytes the same 300 values cost in an array, and the ratio of charged to array. |
| C6 | 3 | [Merge Sort] | Give the worst case number of key comparisons merge sort performs on n = 8, using the bound n·⌈log₂ n⌉ − 2^⌈log₂ n⌉ + 1. Check it against the worked example of B9. |
| C7 | 3 | [Quick Sort] | For the Lomuto partition of `{30, 10, 50, 20, 40}`, give the number of swap calls, the number of swaps that actually move data, and the pivot's final index. Then give the swap count for the whole sort. |
| C8 | 3 | [Asymptotic notation] | Show that 5n² + 3n + 1 ∈ O(n²). Give the smallest integer c and the smallest n0, and verify the inequality at n = n0 − 1 and n = n0. |
| C9 | 3 | [Finding Complexity] | A loop body runs n² times and a second loop beside it halves a counter until it reaches 1. For n = 1024, give both iteration counts, the total, the dominant term and the resulting Θ. |
| C10 | 3 | [Tree types and BST] | A BST holds 15 nodes. Give the minimum height, the maximum height, and the average number of comparisons for a successful search in the balanced case, with the level arithmetic. |

---

## Section D · code writing or design · 4 marks each · 20 marks

| Q | Marks | Topic tag | Question |
|---|---|---|---|
| D1 | 4 | [Formulation in C] | In C, declare a struct for a singly linked list node, and write two functions: one that inserts a value at the front of the list, one that counts the nodes. State the cost of each. |
| D2 | 4 | [Different Operations] | In C, write one function that deletes the first node holding a given value from a singly linked list. It must handle all three cases: the value is in the head, the value is in the middle or the tail, and the value is absent. State the cost of each case. |
| D3 | 4 | [Trees fundamentals] | In C, declare a binary tree node, then write a recursive inorder traversal and a recursive height function. State the cost of each and say what recursion depth the traversal needs. |
| D4 | 4 | [Tree types and BST] | In C, write a BST insert and a BST search. State the worst case cost of each and say what input shape produces that worst case. |
| D5 | 4 | [Merge Sort] | In C, write merge sort with its merge helper. Give the recurrence for the running time, solve it for n a power of two, and state the Θ. State the extra memory the merge helper costs. |

---

## Answer key

Numbered 1 to 1 with the questions above. The arithmetic is written out, not just the value.

### Section A · 1 mark each

| Q | Topic | Answer |
|---|---|---|
| A1 | Asymptotic notation | Θ(g(n)) = { f(n) : there exist positive constants c₁, c₂ and n₀ such that 0 ≤ c₁·g(n) ≤ f(n) ≤ c₂·g(n) for all n ≥ n₀ }. Two constants, one n₀, one inequality chain. |
| A2 | Algorithm fundamentals | An algorithm is a finite sequence of unambiguous, effectively computable steps that takes zero or more inputs, produces one or more outputs, and terminates. The five properties: input, output, definiteness, finiteness, effectiveness. |
| A3 | Array | `&a[3] = 0x7ffe7c3131b0 + 3 × 4 = 0x7ffe7c3131bc`, and the value there is 40. |
| A4 | Pointers | `sizeof(void*) = 8`, because an address is 64 bits on a 64 bit machine. `(char*)p + 1` moves 1 byte, `(int*)p + 1` moves 4 bytes, because pointer arithmetic is type aware. |
| A5 | Stack by Array | `pop()` gives 30, `pop()` gives 20, `top()` gives 10. That is LIFO, and the stack still holds 1 element. |
| A6 | Queue | A FIFO queue is a linear structure where the element that has waited longest leaves first. Enqueue 10, 20, 30 then dequeue twice gives 10, then 20. |
| A7 | Linked List | The traversal prints `10 20 30`. `sizeof(Node) = 4 + 4 padding + 8 = 16` bytes. |
| A8 | Trees fundamentals | Root: the one node with no parent. Leaf: a node with no children. Height: the number of edges on the longest path from the root to a leaf. Degree: the number of children of a node. |
| A9 | Tree types and BST | For every node, every key in its left subtree is smaller than the node's key, and every key in its right subtree is larger. |
| A10 | Formulation in C | `sizeof = 16`. The 4 byte `int` sits at offset 0, 4 padding bytes sit at offsets 4 to 7, and the 8 byte pointer sits at offset 8 so that it starts on an 8 byte boundary. |

### Section B · 2 or 3 marks each

**B1 (3 marks, Algorithm fundamentals).** Input, output, definiteness, finiteness, effectiveness.
A recursion with no base case breaks **finiteness**: it never terminates. The measured evidence is
in `lec16_recursion_output.txt`, where a forked child recursed with no base case and died at depth
261579 with a 32 byte frame, against the prediction 8388608 ÷ 32 = 262144, an error of
|261579 − 262144| ÷ 262144 = 565 ÷ 262144 = 0.2155%.

**B2 (3 marks, Asymptotic notation).** Need 3n² + 5n + 2 ≤ c·n² for all n ≥ n₀. With c = 4 the
requirement becomes n² − 5n − 2 ≥ 0, whose positive root is (5 + √33) ÷ 2 = 5.3723, so n₀ = 6.
Check at the boundary: n = 5 gives 3(25) + 25 + 2 = 102 against 4(25) = 100, fails. n = 6 gives
3(36) + 30 + 2 = 140 against 4(36) = 144, holds with 4 to spare. c = 3 is impossible because it
would need 5n + 2 ≤ 0. Answer: **c = 4, n₀ = 6**.

**B3 (3 marks, Finding Complexity).** The inner loop runs i times for each i, so the total is
Σ(i = 1 to n) i = n(n + 1) ÷ 2, which is Θ(n²). For n = 10: 10 × 11 ÷ 2 = 55.
For n = 100: 100 × 101 ÷ 2 = 5050.

**B4 (2 marks, Array).** `&a[3] = 0x7ffe7c3131b0 + 12 = 0x7ffe7c3131bc`.
`&a[4] = 0x7ffe7c3131b0 + 16 = 0x7ffe7c3131c0`. It is O(1) because the address is computed by one
multiply and one add, `base + i × sizeof(T)`, with no search. A list walk has to follow i links, one
dependent load each, so it is O(n).

**B5 (3 marks, Pointers).** The `int` is 4 bytes at offset 0; the `Node*` is 8 bytes and must start
at a multiple of 8, so the next legal offset is 8 and 4 padding bytes sit at offsets 4 to 7.
4 + 4 + 8 = 16, not 12. The two statements make **two** pointer writes: `p->next = head`, then
`head = p`. If `head = p` is written first, the new node's `next` still holds garbage or null, the
old list becomes unreachable behind the new head, and every node behind it leaks.

**B6 (2 marks, Stack by Array).** Pushes 1 to 5 are stored (size becomes 5 and `full()` turns yes at
the fifth). Pushes 6 and 7 are refused because `full()` is true, so **2 are refused** and the size
stays 5. Contents afterwards, bottom to top: `1 2 3 4 5`. The drain prints `5 4 3 2 1`. Overflow was
detected, not written.

**B7 (3 marks, Queue).** Count = (rear − front + capacity) mod capacity = (4 − 2 + 5) mod 5 =
7 mod 5 = **2**. The next enqueue writes slot 4 (`rear = 4`), then the new rear is
(4 + 1) mod 5 = **0**. After that enqueue the count is 2 + 1 = 3, and the queue is not full
(full means count = 5). The naive array queue can never enqueue again after a full drain because
front and rear both reach capacity: front 5, rear 5, size 0, so `empty()` and `full()` are both true
at once, `dequeue` reports empty and `full()` still refuses the enqueue.

**B8 (3 marks, Linked List).** Insert 30 needs 2 pointer writes: the new node's `next` to the
successor (40), then the predecessor's (20's) `next` to the new node. Cost O(k) to reach position k,
here k = 3, then O(1) for the writes. Result `10 -> 20 -> 30 -> 40 -> 50 -> NULL`. Delete the last
node needs the second last node held: 1 pointer write (`40->next = nullptr`), then `delete` on the
old tail. Cost O(n) to reach it, O(1) for the write, and O(1) for the whole delete only if the
second last node is already held.

**B9 (3 marks, Merge Sort).** Splits: `{8,3,5,1,7,2,6,4}` → `{8,3,5,1}` and `{7,2,6,4}`;
`{8,3,5,1}` → `{8,3}` and `{5,1}`; `{7,2,6,4}` → `{7,2}` and `{6,4}`; each pair of two splits into
singletons. Merges: `8,3` → 1 comparison → `3,8`; `5,1` → 1 comparison → `1,5`; `3,8` with `1,5` → 3
comparisons → `1,3,5,8`; `7,2` → 1 → `2,7`; `6,4` → 1 → `4,6`; `2,7` with `4,6` → 3 → `2,4,6,7`;
`1,3,5,8` with `2,4,6,7` → 7 comparisons → `1,2,3,4,5,6,7,8`. Total = 1 + 1 + 3 + 1 + 1 + 3 + 7 =
**17 comparisons**, with 7 merge calls and a recursion depth of ⌈log₂ 8⌉ = 3.

**B10 (3 marks, Quick Sort).** Pivot is 40, the last element. Scan j = 0 to 3, moving any element
≤ 40 to the left: 30 ≤ 40 keeps i at 0 (self swap), 10 ≤ 40 gives i = 1 (self swap), 50 is not
moved, 20 ≤ 40 gives i = 2 and swaps positions 2 and 3, so the array is `30, 10, 20, 50, 40`. The
final swap puts the pivot at i + 1 = 3, giving `30, 10, 20, 40, 50`. So **4 swap calls, of which 2
move data**, and the **pivot lands at index 3**. The left part has 3 keys, the right part has 1 key,
and the whole sort costs 4 data-changing swaps.

**B11 (2 marks, Trees fundamentals).** Maximum nodes = 2^(h+1) − 1 = 2^10 − 1 = **1023**.
Minimum nodes = 2^h = 2^9 = **512**. The measured tree in `lec16_recursion_output.txt` is the
maximum case: 1023 nodes at depth 9.

**B12 (3 marks, Tree types and BST).** Insert order 8, 3, 10, 1, 6, 14, 4, 7, 13 gives
8 at the root; 3 left of 8; 10 right of 8; 1 left of 3; 6 right of 3; 14 right of 10; 4 left of 6;
7 right of 6; 13 left of 14. Height in edges = **3** (8 → 3 → 6 → 4). Inorder =
`1, 3, 4, 6, 7, 8, 10, 13, 14`, which is the sorted order, exactly as the BST property promises.

**B13 (3 marks, Different Operations).** Access element i: array O(1) by `base + i × sizeof(T)`,
linked O(n) by hops. Insert at the front: array O(n) because everything shifts, linked O(1) as two
writes. Delete at the front: array O(n) for the same shift, linked O(1). Search a value: O(n) in
both, so the layout does not help there. The measured constants from `lec05_run_output.txt` are
0.19 ns per element for the sequential array walk, 2.46 ns for the same array visited out of order,
and 2.45 ns for the linked walk, which is 12.69× and 12.64× the sequential walk: the same O(n)
work, a different constant.

**B14 (2 marks, Formulation in C).**

```c
struct Node { int data; struct Node* next; };
struct Node* head = NULL;
void insertFront(struct Node*& head, int v);   /* head by reference */
```

The `&` makes the parameter an alias for the caller's pointer variable, so `head = p` inside the
function changes the caller's head. Without it the parameter is a copy, the insert happens on the
copy, the caller's head stays unchanged or NULL, and the new node leaks.

**B15 (2 marks, Finding Complexity).** The outer loop runs n times. The inner `while` on i runs
⌊log₂ i⌋ times, so the total is Σ(i = 1 to 16) ⌊log₂ i⌋ = 0 + 2×1 + 4×2 + 8×3 + 4 =
0 + 2 + 8 + 24 + 4 = **38** for n = 16. In general the count grows like n·log₂ n, so the class is
**Θ(n log n)**.

### Section C · 3 marks each

**C1 (Array).** Total bytes = 8 × 4 = **32** = `0x20`. `&a[5] = 0x1000 + 5 × 4 = 0x1000 + 20 =
`**`0x1014`**. `&a[7] = 0x1000 + 28 = `**`0x101c`**. Each address is one multiply and one add, so
**1 computing step**, which is what makes indexed access O(1).

**C2 (Pointers).** `sizeof(int) = 4`, `sizeof(void*) = 8`, `sizeof(Node) = 16`. The 16 bytes are
4 payload + **4 padding at offsets 4 to 7** + 8 pointer, and `offsetof(next) = 8`. `p + 1` on a
`Node*` advances **16 bytes**, the size of the pointed-to type.

**C3 (Stack by Array).** The five ints are 5 × 4 = **20** bytes at offsets 0 to 19. `top_` is a
`size_t`, so it must start at a multiple of 8: the next multiple after 20 is 24, so **4 padding**
bytes sit at offsets 20 to 23. Bookkeeping is `top_` 8 + `refusals_` 8 = **16**. Total
20 + 4 + 16 = **40**. Offsets: `data` 0, `top` 24, `refusals` 32. Of seven pushes, 5 are stored and
**2 are refused**.

**C4 (Queue).** Count = (rear − front + capacity) mod capacity = (4 − 2 + 5) mod 5 = 7 mod 5 =
**2**. The next enqueue writes slot 4, then rear = (4 + 1) mod 5 = **0**. After that enqueue the
count is **3**, and full would need count = 5, so the queue is **not full**.

**C5 (Linked List).** Requested: 300 × 16 = **4800** bytes. Usable: 300 × 24 = **7200** bytes.
Charged at the measured 32 byte stride: 300 × 32 = **9600** bytes. Array of 300 ints:
300 × 4 = **1200** bytes. Ratio charged ÷ array = 9600 ÷ 1200 = **8**.

**C6 (Merge Sort).** For n = 8, ⌈log₂ n⌉ = 3, so the bound gives 8 × 3 − 2³ + 1 = 24 − 8 + 1 =
**17 comparisons**. The worked example of B9 needed exactly 17, so the example sits on the worst
case bound. The 7 merge calls are n − 1 = 8 − 1 = 7.

**C7 (Quick Sort).** Lomuto on `{30, 10, 50, 20, 40}` with pivot 40: **4 swap calls**, of which
**2 carry data** (positions 2 and 3, then the final swap of positions 3 and 4). The pivot lands at
index **3**. Over the whole sort the data-changing swaps are: partition 1 gives 2, partition 2 on
the left part `{30, 10, 20}` with pivot 20 gives 2 more, so **4 data-changing swaps** in total.

**C8 (Asymptotic notation).** Need 5n² + 3n + 1 ≤ c·n² for all n ≥ n₀. With c = 6 the requirement is
n² − 3n − 1 ≥ 0, whose positive root is (3 + √13) ÷ 2 = 3.3028, so n₀ = 4. Check n = 3:
5(9) + 9 + 1 = 55 against 6(9) = 54, fails. n = 4: 5(16) + 12 + 1 = 93 against 6(16) = 96, holds
with 3 to spare. c = 5 is impossible because it needs 3n + 1 ≤ 0. Answer: **c = 6, n₀ = 4**.

**C9 (Finding Complexity).** n² = 1024 × 1024 = **1048576** iterations. The halving loop runs
⌊log₂ 1024⌋ = **10** iterations, because 2^10 = 1024. Total = 1048576 + 10 = **1048586**. The
dominant term is n², so the whole block is **Θ(n²)**.

**C10 (Tree types and BST).** Minimum height = ⌈log₂(n + 1)⌉ − 1 = ⌈log₂ 16⌉ − 1 = 4 − 1 = **3**
edges. Maximum height = n − 1 = **14** edges, the degenerate chain. For the balanced case the level
cost is 1 comparison for the 1 node at level 1, 2 for the 2 nodes at level 2, 3 for the 4 nodes at
level 3 and 4 for the 8 nodes at level 4: 1(1) + 2(2) + 3(4) + 4(8) = 1 + 4 + 12 + 32 = 49, so the
average successful search costs 49 ÷ 15 = **3.2667 comparisons**, against a worst case of 15.

### Section D · 4 marks each

**D1 (Formulation in C).**

```c
#include <stdio.h>

struct Node { int data; struct Node* next; };

/* insert at the front: two pointer writes, O(1) */
void insertFront(struct Node*& head, int v) {
    struct Node* p = new Node;      /* or malloc in C: (struct Node*)malloc(sizeof(struct Node)) */
    p->data = v;
    p->next = head;                 /* write 1: the new node takes over the old first node */
    head = p;                       /* write 2: head moves onto the new node */
}

/* count the nodes: one hop per node, O(n) time, O(1) extra space */
int countNodes(struct Node* head) {
    int c = 0;
    for (struct Node* cur = head; cur != nullptr; cur = cur->next) c++;
    return c;
}
```

Insert at the front is O(1) because nothing is traversed and nothing shifts. Counting is O(n) because
it visits every node exactly once. The node is 16 bytes (4 payload, 4 padding, 8 pointer).

**D2 (Different Operations).**

```c
int deleteByValue(struct Node*& head, int v) {
    if (head == nullptr) return 0;                 /* empty list, nothing to do */
    if (head->data == v) {                         /* case 1: the head holds it, O(1) */
        struct Node* dead = head;
        head = head->next;                         /* move the pointer FIRST */
        delete dead;                               /* then free, never the other way round */
        return 1;
    }
    struct Node* cur = head;                       /* case 3: middle or tail, O(n) to reach */
    while (cur->next != nullptr && cur->next->data != v) cur = cur->next;
    if (cur->next == nullptr) return 0;            /* case 2: absent, the list is untouched */
    struct Node* dead = cur->next;
    cur->next = dead->next;                        /* splice it out, one pointer write */
    delete dead;
    return 1;
}
```

Costs: case 1 is O(1). Case 2 is O(n) to prove absence and changes nothing. Case 3 is O(n) to reach
the predecessor, then O(1) for the splice and the free. Verified run shape: `10 -> 20 -> 40 -> 50`
after deleting 30, and `deleteByValue(999)` leaves the list untouched.

**D3 (Trees fundamentals).**

```c
struct TNode { int data; struct TNode* left; struct TNode* right; };

/* inorder: left, node, right.  O(n) time, O(h) stack */
void inorder(struct TNode* t) {
    if (t == nullptr) return;
    inorder(t->left);
    printf("%d ", t->data);
    inorder(t->right);
}

/* height in edges: -1 for an empty tree, 0 for one node */
int height(struct TNode* t) {
    if (t == nullptr) return -1;
    int l = height(t->left), r = height(t->right);
    return 1 + (l > r ? l : r);
}
```

Inorder visits every node once, so O(n) time. Height visits every node once, also O(n) time. The
stack used is the height of the tree, O(h), measured at 48 bytes per suspended call in
`lec16_recursion_output.txt`, so a 1023 node perfect tree of height 9 needs 10 frames, about
480 bytes. Inorder on a BST prints the keys in sorted order, which is the proof the BST property
holds. The measured capture built exactly this tree: 1023 nodes, 24552 bytes requested at 24 bytes
per node, 1023 inorder visits, keys summing to 1023 × 1024 ÷ 2 = 523776.

**D4 (Tree types and BST).**

```c
struct TNode { int data; struct TNode* left; struct TNode* right; };

struct TNode* insertBST(struct TNode* t, int v) {
    if (t == nullptr) {
        struct TNode* n = new TNode;
        n->data = v; n->left = n->right = nullptr;
        return n;
    }
    if (v < t->data)      t->left  = insertBST(t->left,  v);
    else if (v > t->data) t->right = insertBST(t->right, v);
    return t;                      /* equal key: no duplicate, nothing changes */
}

struct TNode* searchBST(struct TNode* t, int v) {
    while (t != nullptr && t->data != v)
        t = (v < t->data) ? t->left : t->right;
    return t;                      /* NULL means absent */
}
```

Both walk one path from the root, so both are O(h), where h is the height. The worst case is a
degenerate chain of n nodes, giving O(n) comparisons for a search: insert the keys already in sorted
order and the tree is a list in disguise. The sorted input 1, 2, 3, ..., n is exactly that shape. On
the balanced side, h = ⌈log₂(n + 1)⌉ − 1, so the search is O(log n), which for 1023 nodes is height 9
and at most 10 node visits.

**D5 (Merge Sort).**

```c
void merge(int a[], int lo, int mid, int hi, int tmp[]) {
    int i = lo, j = mid + 1, k = lo;
    while (i <= mid && j <= hi) {
        if (a[i] <= a[j]) tmp[k++] = a[i++];   /* one key comparison per element taken */
        else              tmp[k++] = a[j++];
    }
    while (i <= mid) tmp[k++] = a[i++];        /* leftover tail, no more comparisons */
    while (j <= hi)  tmp[k++] = a[j++];
    for (i = lo; i <= hi; i++) a[i] = tmp[i];  /* copy back, the price of the helper array */
}

void mergeSort(int a[], int lo, int hi, int tmp[]) {
    if (lo >= hi) return;                      /* base case: 0 or 1 element is sorted */
    int mid = lo + (hi - lo) / 2;
    mergeSort(a, lo, mid, tmp);
    mergeSort(a, mid + 1, hi, tmp);
    merge(a, lo, mid, hi, tmp);
}
```

Recurrence: T(n) = 2·T(n ÷ 2) + Θ(n), with T(1) = Θ(1). Substituting n = 2^k gives
T(2^k) = 2·T(2^(k−1)) + 2^k, so T(n) = n·(k + 1) = n·log₂ n + n, which is **Θ(n log n)**. The level
count is k + 1 because each halving adds one level below the top and the recursion stops at single
elements. The merge helper needs a temporary array of n elements, so the extra memory is **Θ(n)**.
The worst case key comparisons are n·⌈log₂ n⌉ − 2^⌈log₂ n⌉ + 1, which for n = 8 is 8 × 3 − 8 + 1 = 17.

---

## Files and sources

| Source | What was taken |
|---|---|
| `~/learning/cpp/exercises/lec05_run_output.txt` | sizes, offsets, array base, padding family, per element timings |
| `~/learning/cpp/exercises/lec11_15_run_output.txt` | stack and queue sizes, node stride, capacity log, queue costs |
| `~/learning/cpp/exercises/lec16_recursion_output.txt` | frame strides, tree node counts, stack limit, key sum |

Every non measured number in this paper (sorting counts, loop counts, asymptotic certificates, tree
formulas, BST comparison counts) was computed by `python3 /tmp/mte_answers.py` with integer
arithmetic, and the run output is reproduced in `DSA-MTE-NUMERIC-SET.md`.
