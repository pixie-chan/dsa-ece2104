# 🎯 DSA Mastery Plan — Semester + LeetCode Medium/Hard
## ECE2104 | Jul–Nov 2026 | 18-Week Battle Plan

> **Goal:** Ace the semester AND solve LeetCode Medium-Hard confidently.
> **Commitment:** 3–4 hours/day, 6 days/week (Sunday = rest/light review).
> **Total problems target:** ~200 LeetCode problems + course assignments.

---

## 📋 HOW TO USE THIS PLAN

Each week has:
- **📚 Syllabus topic** — what your lectures cover that week
- **🧠 Learn** — what to study (theory + understanding)
- **💻 Practice** — specific LeetCode problems (by number + name)
- **✅ Checkpoint** — how to know you've leveled up

**Rules:**
1. **No copying solutions.** Stuck for 20 min → hint only. 30 min → read solution, UNDERSTAND it, then re-implement from scratch next day.
2. **Time yourself.** Track solve time per problem. Medium should take <30 min by week's end.
3. **Pattern > Problem.** After solving 3 problems of the same type, write down the PATTERN in your own words.
4. **C++ is your weapon.** All solutions in C++. The exam uses C++. LeetCode accepts C++. No Python shortcuts.

---

## 🗓️ PHASE 1: FOUNDATIONS (Weeks 1–3)
### Goal: C++ fluency + array/string fundamentals

---

### WEEK 1 — C++ Crash Course + Arrays
**Syllabus:** Lectures 1–5 (Intro, C++ basics, data types, memory)

> 📘 Language prep: if C++ still feels shaky, follow `C++-LANGUAGE-PLAN.md` (same folder,
> 12 steps, ~2.5 weeks) before starting Week 1's problems.

**🧠 Learn (Days 1–3):**
- C++ syntax: variables, data types, operators, I/O
- Control flow: if/else, switch, for, while, do-while
- Functions: pass by value, pass by reference (`&`), return
- Arrays: declaration, traversal, 2D arrays
- **CRITICAL:** Pointers & references — `*`, `&`, pointer arithmetic
  - Resource: Programiz C++ Pointers tutorial
- Time complexity intro: O(1), O(n), O(n²) — what they mean practically

**💻 Practice (Days 4–6):**

| # | Problem | Difficulty | Pattern |
|---|---------|------------|---------|
| 1 | Two Sum (#1) | Easy | Hash Map lookup |
| 2 | Contains Duplicate (#217) | Easy | Hash Set |
| 3 | Valid Anagram (#242) | Easy | Frequency count |
| 4 | Max Subarray (#53) | Medium | Kadane's algorithm |
| 5 | Best Time to Buy & Sell Stock (#121) | Easy | Single pass tracking |
| 6 | Product of Array Except Self (#238) | Medium | Prefix/Suffix |
| 7 | Two Sum II (#167) | Medium | Two Pointers sorted |
| 8 | Sort Colors (#75) | Medium | Dutch National Flag |
| 9 | Merge Sorted Array (#88) | Easy | Two pointer from end |
| 10 | Move Zeroes (#283) | Easy | Two pointer swap |

**✅ Checkpoint:** Can you solve Two Sum variants without thinking? Can you explain O(n) vs O(n²) with real examples?

---

### WEEK 2 — Strings + Two Pointers + Sliding Window
**Syllabus:** Lectures 3–4 continued (control, functions)

**🧠 Learn:**
- C++ string manipulation: `string`, `substr`, `find`, `getline`
- Two Pointers technique: opposite ends, same direction
- Sliding Window: fixed window, variable window
- Hash maps in C++: `unordered_map`, `unordered_set`

**💻 Practice:**

| # | Problem | Difficulty | Pattern |
|---|---------|------------|---------|
| 11 | Valid Palindrome (#125) | Easy | Two Pointers |
| 12 | Valid Palindrome II (#680) | Medium | Two Pointers + skip |
| 13 | Container With Most Water (#11) | Medium | Two Pointers shrink |
| 14 | Trapping Rain Water (#42) | Hard | Two Pointers / Stack |
| 15 | Longest Substring Without Repeating (#3) | Medium | Sliding Window + Set |
| 16 | Minimum Size Subarray Sum (#209) | Medium | Sliding Window |
| 17 | Longest Repeating Character Replacement (#424) | Medium | Sliding Window |
| 18 | Permutation in String (#567) | Medium | Fixed Window + freq |
| 19 | Group Anagrams (#49) | Medium | Sorted key hashmap |
| 20 | Top K Frequent Elements (#347) | Medium | Bucket sort |

**✅ Checkpoint:** Can you look at a string problem and instantly know "sliding window" vs "two pointer" vs "hash map"? If yes, you've leveled up.

---

### WEEK 3 — Linked Lists (Beginner)
**Syllabus:** Lectures 5–7 (Intro to DS, Linked List concept, operations)

**🧠 Learn:**
- What is a linked list and why it exists (vs arrays)
- Singly linked list: node structure, traversal
- Operations: insertion (head, tail, position), deletion, search
- **DUMMY NODE technique** — solves 80% of linked list problems
- Fast & Slow pointers (Floyd's cycle detection)
- C++ structs + dynamic memory: `new`, `delete`, `->`

**💻 Practice:**

| # | Problem | Difficulty | Pattern |
|---|---------|------------|---------|
| 21 | Reverse Linked List (#206) | Easy | Pointer manipulation |
| 22 | Merge Two Sorted Lists (#21) | Easy | Dummy node |
| 23 | Linked List Cycle (#141) | Easy | Fast-slow pointer |
| 24 | Middle of Linked List (#876) | Easy | Fast-slow pointer |
| 25 | Remove Nth From End (#19) | Medium | Two pointer gap |
| 26 | Reorder List (#143) | Medium | Find mid + reverse + merge |
| 27 | Add Two Numbers (#2) | Medium | Dummy node + carry |
| 28 | Copy List with Random Pointer (#138) | Medium | HashMap or interleaving |
| 29 | Linked List Cycle II (#142) | Medium | Floyd's detection |
| 30 | LRU Cache (#146) | Medium | HashMap + DLL |

**✅ Checkpoint:** Can you reverse a linked list on paper in <2 min? Can you draw fast/slow pointer on cycle problems?

---

## 🗓️ PHASE 2: LINEAR DS (Weeks 4–6)
### Goal: Stacks, Queues, Recursion mastery

---

### WEEK 4 — Stacks
**Syllabus:** Lectures 11–12 (Stack concept, array/linked representation)

**🧠 Learn:**
- Stack: LIFO, push/pop/peek/isEmpty
- Array-based vs linked-list-based stack
- When to use stack: matching, nested structures, monotonic patterns
- **Monotonic Stack** — the pattern that solves 60% of stack problems
- C++ `std::stack`, `std::vector` as stack

**💻 Practice:**

| # | Problem | Difficulty | Pattern |
|---|---------|------------|---------|
| 31 | Valid Parentheses (#20) | Easy | Matching stack |
| 32 | Min Stack (#155) | Medium | Auxiliary stack |
| 33 | Evaluate Reverse Polish Notation (#150) | Medium | Operand stack |
| 34 | Daily Temperatures (#739) | Medium | Monotonic stack |
| 35 | Largest Rectangle in Histogram (#84) | Hard | Monotonic stack |
| 36 | Decode String (#394) | Medium | Nested stack |
| 37 | Asteroid Collision (#735) | Medium | Simulation stack |
| 38 | Car Fleet (#853) | Medium | Sort + monotonic stack |
| 39 | Simplify Path (#71) | Medium | String stack |
| 40 | Basic Calculator II (#227) | Medium | Number stack |

**✅ Checkpoint:** Can you draw a monotonic stack step-by-step? Can you solve "nested structure" problems (brackets, decode string) without second-guessing?

---

### WEEK 5 — Queues + Deque + Circular Queue
**Syllabus:** Lectures 13–15 (Queue, linked queue, circular queue, applications)

**🧠 Learn:**
- Queue: FIFO, enqueue/dequeue/peek
- Circular queue: why it matters (no wasted space)
- Deque (double-ended queue): `std::deque`
- Queue usage: BFS foundation, sliding window maximum, task scheduling

**💻 Practice:**

| # | Problem | Difficulty | Pattern |
|---|---------|------------|---------|
| 41 | Implement Queue using Stacks (#232) | Easy | Two-stack trick |
| 42 | Implement Stack using Queues (#225) | Easy | One/two queue |
| 43 | Sliding Window Maximum (#239) | Hard | Monotonic deque |
| 44 | Number of Recent Calls (#933) | Easy | Queue as window |
| 45 | Design Circular Deque (#641) | Medium | Array-based deque |
| 46 | Task Scheduler (#621) | Medium | Greedy + queue |
| 47 | First Unique Character (#387) | Easy | Queue + freq map |
| 48 | Rotting Oranges (#994) | Medium | Multi-source BFS |
| 49 | Open the Lock (#752) | Medium | BFS on states |
| 50 | Perfect Squares (#279) | Medium | BFS levels |

**✅ Checkpoint:** Can you implement circular queue from scratch? Can you spot "this is BFS" just from reading a problem?

---

### WEEK 6 — Recursion 🧱 (THE WALL)
**Syllabus:** Lectures 16 (Recursion + Recursive Algorithms) → **MID-TERM PREP**

**🧠 Learn:**
- **This is your most important week. Do NOT rush it.**
- Recursion basics: base case, recursive case, call stack
- How to think recursively: trust the function, don't trace every frame
- Recursion patterns: linear recursion, tail recursion, divide-and-conquer, backtracking
- **Draw the call stack on paper** for every problem until it clicks
- Time complexity of recursive functions: recurrence relations (for exam)

**💻 Practice:**

| # | Problem | Difficulty | Pattern |
|---|---------|------------|---------|
| 51 | Factorial / Fibonacci (implement both) | Easy | Basic recursion |
| 52 | Power of Two (#231) | Easy | Bit manipulation / recursion |
| 53 | Reverse String (#344) | Easy | Two pointer recursion |
| 54 | Merge Sort (implement from scratch) | Medium | Divide & conquer |
| 55 | Quick Sort (implement from scratch) | Medium | Partition + recurse |
| 56 | Subsets (#78) | Medium | Backtracking |
| 57 | Permutations (#46) | Medium | Backtracking |
| 58 | Combination Sum (#39) | Medium | Backtracking + reuse |
| 59 | Combination Sum II (#40) | Medium | Backtracking no reuse |
| 60 | Subsets II (#90) | Medium | Backtracking + dedup |

**MID-TERM PREP (last 2 days of week):**
- Review all problems from weeks 1–6
- Practice: implement linked list, stack, queue from scratch in <15 min each
- Write out Big-O for every algorithm you've learned
- Do 3–5 problems from each topic as speed runs

**✅ Checkpoint:** Can you solve Permutations (#46) without looking at any reference? Can you draw the call tree for `subsets([1,2,3])`? If yes, recursion has clicked.

---

## 🗓️ PHASE 3: TREES (Weeks 7–10)
### Goal: Binary trees, BSTs, Heaps

---

### WEEK 7 — Binary Trees
**Syllabus:** Lectures 17–19 (Tree terminology, Binary Trees, BST, Traversals)

**🧠 Learn:**
- Tree terminology: root, leaf, height, depth, degree, sibling, parent
- Binary Tree: properties, types (full, complete, perfect, balanced)
- **Three traversals:** In-order, Pre-order, Post-order — MEMORIZE the order
- Level-order traversal (BFS with queue)
- Tree representation: node struct with left/right pointers
- **Recursive tree thinking:** most tree problems = "solve for left + solve for right + combine"

**💻 Practice:**

| # | Problem | Difficulty | Pattern |
|---|---------|------------|---------|
| 61 | Invert Binary Tree (#226) | Easy | Recursive swap |
| 62 | Maximum Depth (#104) | Easy | DFS recursion |
| 63 | Same Tree (#100) | Easy | Compare two trees |
| 64 | Subtree of Another Tree (#572) | Easy | DFS + compare |
| 65 | Diameter of Binary Tree (#543) | Easy | DFS + max tracking |
| 66 | Balanced Binary Tree (#110) | Easy | DFS height check |
| 67 | Binary Tree Level Order Traversal (#102) | Medium | BFS with queue |
| 68 | Binary Tree Right Side View (#199) | Medium | BFS last-node-per-level |
| 69 | Lowest Common Ancestor (#236) | Medium | Recursive post-order |
| 70 | Binary Tree Maximum Path Sum (#124) | Hard | DFS + global max |

**✅ Checkpoint:** Can you write in-order/pre-order/post-order traversal from memory? Can you solve LCA without help?

---

### WEEK 8 — Binary Search Trees + Red-Black Trees
**Syllabus:** Lectures 20–21 (Red-Black Trees + presentation)

**🧠 Learn:**
- BST property: left < root < right
- BST operations: search, insert, delete (3 cases for delete!)
- BST allows sorted-order traversal (in-order = sorted)
- **Red-Black Trees:** memorize the 5 properties, rotation cases
  - Don't try to derive from scratch — learn the patterns
  - Left rotation, right rotation, color flip
  - Insert: at most 2 rotations
  - Delete: at most 3 rotations
- Why self-balancing matters: BST worst case = O(n), balanced = O(log n)

**💻 Practice:**

| # | Problem | Difficulty | Pattern |
|---|---------|------------|---------|
| 71 | Validate BST (#98) | Medium | Range validation |
| 72 | Search in BST (#700) | Easy | Recursive search |
| 73 | Insert into BST (#701) | Easy | Recursive insert |
| 74 | Delete Node in BST (#450) | Medium | 3-case delete |
| 75 | Kth Smallest Element in BST (#230) | Medium | In-order + count |
| 76 | Inorder Successor in BST (#285) | Medium | Track predecessor |
| 77 | Convert Sorted Array to BST (#108) | Easy | Binary search split |
| 78 | Lowest Common Ancestor of BST (#235) | Easy | BST property |
| 79 | Range Sum of BST (#938) | Easy | Pruned DFS |
| 80 | Serialize and Deserialize BST (#449) | Medium | Traversal + rebuild |

**✅ Checkpoint:** Can you delete from a BST and explain all 3 cases? Can you validate a BST in O(n)?

---

### WEEK 9 — Heaps + Priority Queues + Huffman
**Syllabus:** Lectures 22–25 (Heaps, Priority Queue, Huffman's Algorithm, Class Test 2)

**🧠 Learn:**
- Min-heap vs Max-heap: parent ≤ children (min) or parent ≥ children (max)
- Heap as array: parent at `i`, children at `2i+1` and `2i+2`
- Heap operations: insert (sift up), extract-min/max (sift down), heapify
- Priority Queue in C++: `std::priority_queue`
- Huffman coding: greedy + heap — build tree from frequencies
- When to use heap: "top K", "median stream", "merge K sorted"

**💻 Practice:**

| # | Problem | Difficulty | Pattern |
|---|---------|------------|---------|
| 81 | Kth Largest Element in Array (#215) | Medium | Min-heap of size K |
| 82 | Last Stone Weight (#1046) | Easy | Max-heap simulation |
| 83 | K Closest Points to Origin (#973) | Medium | Max-heap of size K |
| 84 | Task Scheduler (#621) | Medium | Greedy + heap |
| 85 | Find Median from Data Stream (#295) | Hard | Two heaps |
| 86 | Merge K Sorted Lists (#23) | Hard | Min-heap + linked list |
| 87 | Top K Frequent Elements (#347) | Medium | Bucket sort / heap |
| 88 | Reorganize String (#767) | Medium | Greedy + heap |
| 89 | Kth Largest in Stream (#703) | Easy | Min-heap |
| 90 | Huffman coding (implement from scratch) | — | Greedy + tree build |

**✅ Checkpoint:** Can you implement a min-heap from scratch (insert + extract)? Can you solve "Top K" problems in <10 min?

---

### WEEK 10 — Trees Deep Dive + Student Presentations Prep
**Syllabus:** Lectures 21, 32–34 (Presentations on Trees/Sorting/Searching)

**💻 Practice (mixed tree problems — harder ones):**

| # | Problem | Difficulty | Pattern |
|---|---------|------------|---------|
| 91 | Binary Tree Cameras (#968) | Hard | DP on trees |
| 92 | Count Good Nodes (#1448) | Medium | DFS with max tracking |
| 93 | Path Sum III (#437) | Medium | Prefix sum + DFS |
| 94 | Vertical Order Traversal (#987) | Hard | BFS + sorting |
| 95 | Serialize and Deserialize BT (#297) | Hard | Pre-order + null markers |
| 96 | Flatten BT to Linked List (#114) | Medium | Reverse post-order |
| 97 | Construct BT from Preorder and Inorder (#105) | Medium | Recursive build |
| 98 | Binary Tree Cameras (#968) | Hard | Tree DP |
| 99 | Word Search II (#212) | Hard | Trie + DFS backtracking |
| 100 | Implement Trie (#208) | Medium | Trie node class |

**✅ Checkpoint:** Can you solve any "construct tree from traversals" problem? Can you implement a Trie from scratch?

---

## 🗓️ PHASE 4: SORTING + SEARCHING (Weeks 11–13)
### Goal: All major sorting algorithms + binary search mastery

---

### WEEK 11 — Sorting Algorithms I (Basic + Merge Sort)
**Syllabus:** Lectures 26–27 (Bubble, Selection, Insertion, Merge Sort)

**🧠 Learn:**
- **Bubble Sort:** O(n²) — repeatedly swap adjacent. Stable.
- **Selection Sort:** O(n²) — find min, put at front. Not stable.
- **Insertion Sort:** O(n²) — build sorted prefix. Stable, good for small/nearly-sorted.
- **Merge Sort:** O(n log n) — divide, conquer, merge. Stable. THE most important sort.
  - Implement merge sort from scratch 3 times until it's muscle memory.
  - Master the merge step — it appears in counting inversions, etc.

**💻 Practice:**

| # | Problem | Difficulty | Pattern |
|---|---------|------------|---------|
| 101 | Sort an Array (#912) | Medium | Implement merge sort |
| 102 | Merge Intervals (#56) | Medium | Sort + merge |
| 103 | Insert Interval (#57) | Medium | Scan + merge |
| 104 | Count Inversions (#varies) | Hard | Modified merge sort |
| 105 | Sort Colors (#75) | Medium | One-pass 3-way |
| 106 | Meeting Rooms II (#253) | Medium | Sort + min-heap |
| 107 | Non-overlapping Intervals (#435) | Medium | Greedy + sort |
| 108 | Minimum Number of Arrows (#452) | Medium | Interval scheduling |
| 109 | H-Index (#274) | Medium | Sort / counting |
| 110 | Largest Number (#179) | Medium | Custom comparator |

**✅ Checkpoint:** Can you implement merge sort in <5 min? Can you solve interval merge problems by identifying the sort-key instantly?

---

### WEEK 12 — Sorting Algorithms II (Heap Sort + Quick Sort)
**Syllabus:** Lectures 28–29 (Heap Sort, Quick Sort)

**🧠 Learn:**
- **Heap Sort:** O(n log n), in-place, not stable. Build max-heap, extract max repeatedly.
- **Quick Sort:** O(n log n) average, O(n²) worst case. In-place, not stable.
  - Pivot selection: random, median-of-three
  - Partition schemes: Lomuto, Hoare
- **Quick Select:** O(n) average — find Kth element without full sort
- When to use which: space constraints, stability needs, data distribution

**💻 Practice:**

| # | Problem | Difficulty | Pattern |
|---|---------|------------|---------|
| 111 | Kth Largest Element (#215) | Medium | Quick Select |
| 112 | Sort an Array (#912) | Medium | Implement quick sort |
| 113 | Wiggle Sort (#280) | Medium | Single pass swap |
| 114 | Wiggle Sort II (#324) | Hard | Quick select + index mapping |
| 115 | Array of Doubled Pairs (#954) | Medium | Sort + greedy check |
| 116 | Custom Sort String (#791) | Medium | Custom comparator |
| 117 | Relative Sort Array (#1122) | Easy | Counting sort |
| 118 | Minimum Absolute Difference (#1200) | Easy | Sort + scan |
| 119 | 3Sum (#15) | Medium | Sort + two pointers |
| 120 | 4Sum (#18) | Medium | Sort + two pointers |

**✅ Checkpoint:** Can you implement quicksort with random pivot? Can you explain when merge sort beats quicksort and vice versa?

---

### WEEK 13 — Searching + Hashing
**Syllabus:** Lectures 30–31 (Searching, Hashing, collision resolution)

**🧠 Learn:**
- Binary Search: O(log n) on sorted array — the template to memorize
- Binary Search variants: find target, find first/last, find boundary, search on answer
- **Hash tables:** hash functions, collision resolution (chaining, open addressing)
- Hash map patterns: frequency counting, two-sum style, grouping
- C++: `unordered_map`, `unordered_set` — O(1) average

**💻 Practice:**

| # | Problem | Difficulty | Pattern |
|---|---------|------------|---------|
| 121 | Binary Search (#704) | Easy | Classic template |
| 122 | Search Insert Position (#35) | Easy | Lower bound |
| 123 | First Bad Version (#278) | Easy | Binary search on answer |
| 124 | Search in Rotated Sorted Array (#33) | Medium | Modified BS |
| 125 | Find Minimum in Rotated (#153) | Medium | Modified BS |
| 126 | Search a 2D Matrix (#74) | Medium | BS on flattened |
| 127 | Koko Eating Bananas (#875) | Medium | BS on answer |
| 128 | Time Based Key-Value Store (#981) | Medium | BS + hash map |
| 129 | Design HashMap (#706) | Easy | Hash + chaining |
| 130 | Subarray Sum Equals K (#560) | Medium | Prefix sum + hashmap |

**✅ Checkpoint:** Can you binary search on rotated array? Can you identify "binary search on answer" problems (Koko Bananas style)?

---

## 🗓️ PHASE 5: GRAPHS + ALGORITHM ANALYSIS (Weeks 14–16)
### Goal: Graph representations, BFS/DFS, Dijkstra, Big-O mastery

---

### WEEK 14 — Graph Foundations + BFS/DFS
**Syllabus:** Lectures 35–36 (Graph representation, BFS, DFS)

**🧠 Learn:**
- Graph terminology: vertex, edge, directed/undirected, weighted, cyclic/acyclic
- Representations: adjacency matrix vs adjacency list (know when to use which)
- **BFS:** queue-based, level-by-level, finds shortest path in unweighted graphs
- **DFS:** stack/recursion-based, explores deep first, good for cycles/connectivity
- Visited tracking: avoid infinite loops
- Connected components, cycle detection

**💻 Practice:**

| # | Problem | Difficulty | Pattern |
|---|---------|------------|---------|
| 131 | Number of Islands (#200) | Medium | DFS/BFS on grid |
| 132 | Flood Fill (#733) | Easy | DFS/BFS |
| 133 | Clone Graph (#133) | Medium | BFS + hashmap |
| 134 | Course Schedule (#207) | Medium | Topological sort (DFS) |
| 135 | Course Schedule II (#210) | Medium | Topological sort |
| 136 | Number of Connected Components (#323) | Medium | DFS/BFS union |
| 137 | Graph Valid Tree (#261) | Medium | N-1 edges + connected |
| 138 | Pacific Atlantic Water Flow (#417) | Medium | Reverse BFS from edges |
| 139 | Rotting Oranges (#994) | Medium | Multi-source BFS |
| 140 | Word Ladder (#127) | Hard | BFS shortest path |

**✅ Checkpoint:** Can you build adjacency list from edge list? Can you solve cycle detection in directed/undirected graphs from memory?

---

### WEEK 15 — Dijkstra + Shortest Path + Advanced Graphs
**Syllabus:** Lectures 37–38 (Dijkstra's Algorithm, Graph traversal)

**🧠 Learn:**
- **Dijkstra's:** O((V+E) log V) — shortest path in weighted non-negative graphs
  - Uses min-heap (priority queue)
  - Greedy: always expand closest unvisited node
- Bellman-Ford: handles negative weights (O(VE)) — know it exists
- **Union-Find (Disjoint Set):** union by rank + path compression
  - Used for: dynamic connectivity, Kruskal's MST
- Minimum Spanning Tree: Prim's, Kruskal's (know the concepts)

**💻 Practice:**

| # | Problem | Difficulty | Pattern |
|---|---------|------------|---------|
| 141 | Network Delay Time (#743) | Medium | Dijkstra |
| 142 | Cheapest Flights Within K Stops (#787) | Medium | Modified Dijkstra / Bellman-Ford |
| 143 | Swim in Rising Water (#778) | Hard | Dijkstra on grid |
| 144 | Path with Minimum Effort (#1631) | Medium | Dijkstra binary search |
| 145 | Accounts Merge (#721) | Medium | Union-Find |
| 146 | Redundant Connection (#684) | Medium | Union-Find cycle |
| 147 | Surrounded Regions (#130) | Medium | DFS from borders |
| 148 | Longest Consecutive Sequence (#128) | Medium | HashSet / Union-Find |
| 149 | Number of Provinces (#547) | Medium | DFS / Union-Find |
| 150 | Is Graph Bipartite? (#785) | Medium | BFS/DFS coloring |

**✅ Checkpoint:** Can you implement Dijkstra from scratch with a min-heap? Can you implement Union-Find with path compression?

---

### WEEK 16 — Algorithm Analysis + Graph Review
**Syllabus:** Lectures 39–40 (Synergy between DS & algorithms, choice factors)

**🧠 Learn:**
- **Big-O, Big-Ω, Big-Θ:** master these for exam
- Amortized analysis: why push() is O(1) even when resize happens
- Time-space tradeoffs: when to use more memory for speed
- **When to use what:** the decision framework

| Problem Type | First Thought | DS to Use |
|---|---|---|
| "Find element" | Hash map or binary search | HashMap / BST |
| "Top K" | Heap or quickselect | Min-heap of size K |
| "Sorted order" | Sort first | Merge sort / Quick sort |
| "Shortest path unweighted" | BFS | Queue |
| "Shortest path weighted" | Dijkstra | Min-heap + adjacency list |
| "Check all combinations" | Backtracking | Recursion tree |
| "Interval scheduling" | Sort by end point | Greedy |
| "Connected components" | DFS/BFS or Union-Find | Both |
| "Nested structures" | Stack | Monotonic stack |
| "Subarray sum" | Prefix sum | HashMap + prefix |

**💻 Practice:**
- Review all problems from weeks 1–15 (redo any you struggled with)
- Write complexity analysis for every algorithm you've implemented
- Solve 5 "explain the approach" problems (whiteboard style)

**✅ Checkpoint:** Can you look at a new problem and identify the pattern in <2 minutes? Can you write Big-O for any algorithm you've learned?

---

## 🗓️ PHASE 6: BACKTRACKING + DYNAMIC PROGRAMMING INTRO (Weeks 17–18)
### Goal: Round out hard problem types + revision

---

### WEEK 17 — Backtracking + Advanced Patterns
**Syllabus:** Lectures 41–42 (Presentations, Class Test 3)

**🧠 Learn:**
- Backtracking template: choose → explore → unchoose
- Subsets, Permutations, Combinations — the three pillars
- Constraint satisfaction: N-Queens, Sudoku solver
- Introducing DP: memoization (top-down) vs tabulation (bottom-up)

**💻 Practice:**

| # | Problem | Difficulty | Pattern |
|---|---------|------------|---------|
| 151 | Subsets (#78) | Medium | Backtracking |
| 152 | Subsets II (#90) | Medium | Backtracking + sort dedup |
| 153 | Permutations (#46) | Medium | Backtracking |
| 154 | Permutations II (#47) | Medium | Backtracking + freq map |
| 155 | Combination Sum (#39) | Medium | Backtracking + reuse |
| 156 | Letter Combinations (#17) | Medium | Backtracking |
| 157 | N-Queens (#51) | Hard | Backtracking + pruning |
| 158 | Word Search (#79) | Medium | DFS + backtracking |
| 159 | Palindrome Partitioning (#131) | Medium | Backtracking + palindrome check |
| 160 | Climbing Stairs (#70) | Easy | DP intro (Fibonacci variant) |

**✅ Checkpoint:** Can you solve Permutations II (#47) with a frequency map approach? Can you write the N-Queens backtracking template?

---

### WEEK 18 — Dynamic Programming Foundations + FINAL REVIEW
**Syllabus:** End of course — revision + final exam prep

**🧠 Learn:**
- DP thought process: "Can I break this into subproblems?"
- Top-down (memoization) vs Bottom-up (tabulation)
- Classic DP patterns: 1D, 2D, knapsack variants
- Review ALL patterns from the semester

**💻 Practice:**

| # | Problem | Difficulty | Pattern |
|---|---------|------------|---------|
| 161 | Climbing Stairs (#70) | Easy | 1D DP |
| 162 | House Robber (#198) | Medium | 1D DP |
| 163 | Coin Change (#322) | Medium | Unbounded knapsack |
| 164 | Longest Increasing Subsequence (#300) | Medium | 1D DP / patience sort |
| 165 | Word Break (#139) | Medium | DP + dictionary |
| 166 | Unique Paths (#62) | Medium | 2D DP grid |
| 167 | Minimum Path Sum (#64) | Medium | 2D DP grid |
| 168 | Decode Ways (#91) | Medium | 1D DP |
| 169 | Maximum Product Subarray (#152) | Medium | DP with min/max tracking |
| 170 | Edit Distance (#72) | Hard | 2D DP classic |

**FINAL REVIEW — The last 3 days:**
1. Go through ALL patterns listed in the table above
2. Redo any problem you couldn't solve in <20 min on first attempt
3. Write out Big-O for every algorithm
4. Do 3 mock "unseen problem" sessions: pick 5 random medium problems, timed 30 min each

---

## 📊 TOTAL PROBLEM COUNT

| Phase | Weeks | Problems | Difficulty Mix |
|---|---|---|---|
| Phase 1: Foundations | 1–3 | 30 | 12 Easy, 16 Medium, 2 Hard |
| Phase 2: Linear DS | 4–6 | 30 | 7 Easy, 18 Medium, 5 Hard |
| Phase 3: Trees | 7–10 | 40 | 12 Easy, 20 Medium, 8 Hard |
| Phase 4: Sorting+Search | 11–13 | 30 | 5 Easy, 22 Medium, 3 Hard |
| Phase 5: Graphs | 14–16 | 20 | 2 Easy, 16 Medium, 2 Hard |
| Phase 6: DP+Backtrack | 17–18 | 20 | 3 Easy, 14 Medium, 3 Hard |
| **TOTAL** | **18 weeks** | **170** | **41 Easy, 106 Medium, 23 Hard** |

Plus ~30 practice problems from textbooks/exam prep = **~200 total**.

---

## 🛠️ RESOURCES

| Resource | What for | Link |
|---|---|---|
| **NeetCode 150** | Curated problem list + video explanations | neetcode.io |
| **Sean Prashad Patterns** | Problems grouped by pattern | seanprashad.com/leetcode-patterns |
| **LeetCode** | Practice platform | leetcode.com |
| **Visualgo** | Visualize algorithms | visualgo.net |
| **Programiz** | C++ reference | programiz.com/cpp-programming |
| **Kanetkar textbook** | Indian exam-oriented problems | (course textbook #1) |

---

## ⚡ DAILY SCHEDULE TEMPLATE

```
┌─────────────────────────────────────────────────────┐
│  DAILY DSA ROUTINE (3.5 hours)                      │
├─────────────────────────────────────────────────────┤
│  [30 min]  Review yesterday's problems (spaced rep) │
│  [30 min]  Read theory for today's topic            │
│  [60 min]  Solve 2-3 new LeetCode problems          │
│  [30 min]  Implement DS/algorithm from scratch      │
│  [30 min]  Review + write pattern notes             │
│  [10 min]  Log progress (solved/time/thoughts)      │
└─────────────────────────────────────────────────────┘
```

---

## 🚨 MILESTONE TRACKER

| Week | Milestone | Status |
|---|---|---|
| 3 | Solve any Easy array/string in <5 min | ⬜ |
| 6 | Solve Permutations/Backtracking from scratch | ⬜ |
| 6 | **MID-TERM READY** (score 25+/30) | ⬜ |
| 10 | Solve tree problems without drawing tree first | ⬜ |
| 13 | Implement merge sort + quicksort from memory | ⬜ |
| 15 | Implement Dijkstra from scratch | ⬜ |
| 16 | Identify problem pattern in <2 min | ⬜ |
| 18 | Solve random LeetCode Medium in <25 min | ⬜ |
| 18 | **END-TERM READY** (score 32+/40) | ⬜ |

---

*Last updated: 2026-07-23*
*Plan by: Kiki 🖤*
