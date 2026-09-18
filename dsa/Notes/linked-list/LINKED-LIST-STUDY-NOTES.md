# Linked List · ECE2104 (DSA), MUJ Sem 3

Mapped to the official lecture plan, nothing skipped, nothing invented.

| Lecture | Official topic | Session outcome | CO | File |
|---|---|---|---|---|
| 5 | Introduction to Data Structures and Memory Representation | define basic data structures, explain memory allocation | 1 | (background for 6) |
| 6 | Linked List: Concept, Types, Node Representation | represent linked lists in memory | 1 | `06_linked_list_representation.cpp` |
| 7 | Linked List Operations: Creation and Insertion, Deletion and Traversal | represent linked lists in memory | 1 | `07_linked_list_operations.cpp` |
| 8 | Searching in Linked List, Applications, Numerical Problems and Practice | perform traversal and search operations | 1 | `08_linked_list_searching_apps.cpp` |
| 9 | Tutorial | perform traversal and search operations | 1 | this note, section 7 |

Assessment: in-class quiz, mid term, end term. The handout's lecture plan puts
"MID SEMESTER EXAMINATION" after lecture 16, so lectures 5 to 9 (this note) are mid term material.

---

## 1. Why the linked list exists (Lecture 6 opening)

| | array | linked list |
|---|---|---|
| memory | one contiguous block | nodes scattered, one `new` per node |
| access node i | `base + i x size`, O(1) | walk i hops, O(n) |
| capacity | fixed at declaration | grows until malloc fails |
| insert or delete in the middle | shift O(n) elements | O(1) after the predecessor is held |
| per element overhead | none | 8 bytes for the link (16 byte node for an int) |
| cache behaviour | good, sequential | poor, nodes are cold and far apart |

Definition to write in the exam: a linked list is a linear data structure in which each element
(node) stores its data plus the address of the next element, so consecutive elements need not be
in consecutive memory locations.

## 2. Node representation (Lecture 6)

```cpp
struct Node {
    int data;      // info field, the actual value
    Node* next;    // link field, address of the successor, nullptr at the last node
};
```

Measured on this machine (g++ 15.2.0, x86-64):

| quantity | value |
|---|---|
| `sizeof(int)` | 4 |
| `sizeof(Node*)` | 8 |
| `offsetof(data)` | 0 |
| `offsetof(next)` | 8 |
| `sizeof(Node)` | 16 (4 bytes of padding, the pointer must sit at an 8 byte boundary) |

A linked list of n nodes therefore costs about `n x 16` bytes plus the head pointer, against
`n x 4` bytes for the same numbers in an array.

## 3. Representation in memory: array of nodes vs linked nodes

Real addresses from the lesson run.

```
ARRAY OF NODES (single allocation)              LINKED NODES (one new each)
block[0] 0x7ffef936b420  data 10 next 0         node 1 0x5d470edb2030  data 10 next 0x...2050
block[1] 0x7ffef936b430  data 20 next 0         node 2 0x5d470edb2050  data 20 next 0x...2070
block[2] 0x7ffef936b440  data 30 next 0         node 3 0x5d470edb2070  data 30 next 0 (NULL)
                ^ 16 bytes apart                                ^ heap addresses, no arithmetic
                                                                  pattern, they change every run
```

Also legal and sometimes asked: an array of nodes where the `next` field holds an index instead
of a pointer (that is how many real implementations inside operating systems do it).

## 4. Types of linked list (Lecture 6)

| type | structure | nodes | use | trap |
|---|---|---|---|---|
| singly | head -> a -> b -> NULL | 16 bytes each | stacks, queues, most exam questions | cannot step backwards |
| doubly | NULL <- a <-> b -> NULL | 24 bytes each | LRU cache, browser history | every insert or delete touches 4 pointers |
| circular | last node points to the first, no NULL | 16 bytes each | round robin scheduling, playlists | `while (cur != nullptr)` never terminates |

Measured: `sizeof(DNode)` = 24 bytes for a doubly node, against 16 for a singly node.

## 5. Operators you need for linked list code (Lecture 7 syntax)

| C++ | meaning | example |
|---|---|---|
| `Node* p = new Node(v);` | ask the heap for one node and construct it | the node lives until `delete` |
| `p->data` | follow the pointer, then take the field | 10 |
| `p->next` | the link field itself | an address |
| `head = p->next;` | move the head pointer one step | no data is copied |
| `delete p;` | return the node to the heap | dangling pointer if you keep using `p` |
| `head == nullptr` | the list is empty | must be checked in every operation |
| `void f(Node*& head)` | the head pointer by reference | without `&` the caller's head never changes |

## 6. Operations (Lecture 7)

Traversal, the loop every operation is built from:

```cpp
for (Node* cur = head; cur != nullptr; cur = cur->next)
    cout << cur->data << " -> ";
```

Insert at beginning, O(1):

```cpp
Node* n = new Node(v);
n->next = head;   // 1. the new node takes over the old first node
head = n;         // 2. head moves onto the new node
```

Insert at end, O(n): guard the empty list, walk `while (cur->next != nullptr)`, then `cur->next = n`.

Insert at position k, O(k): walk to node k-1, then `n->next = cur->next; cur->next = n;`.

Delete by value has exactly three cases:

1. the value is in the first node: `head = head->next;` then `delete dead;`
2. the value is absent: stop, change nothing
3. the value is in the middle or the last node: `cur->next = dead->next;` then `delete dead;`

Verified run of the operations file:

```
10 -> 20 -> 30 -> 40 -> 50 -> NULL     [after insertAtPosition(30, pos 3)]
10 -> 20 -> 40 -> 50 -> NULL           [deleteByValue(30)]
10 -> 20 -> 40 -> NULL                 [deleteFromEnd()]
20 -> 40 -> NULL                       [deleteFromBeginning()]
20 -> 40 -> NULL                       [deleteByValue(999), absent, untouched]
```

Cost table (write this in the exam):

| operation | singly | note |
|---|---|---|
| insert at beginning | O(1) | |
| insert at end | O(n) | O(1) with a tail pointer |
| insert at position k | O(k) | |
| delete at beginning | O(1) | |
| delete at end | O(n) | O(1) if doubly linked |
| search or traverse | O(n) | |
| access node i | O(n) | this is the price of the scattered layout |

## 7. Tutorial sheet (Lecture 9) with worked answers

**Q1. Represent the list 10, 20, 30 in memory and draw it.**
Three nodes at heap addresses `A`, `B`, `C`; `A.next = B`, `B.next = C`, `C.next = nullptr`;
`head = A`. Draw the boxes with an arrow from each link field to the next box and NULL at the end.

**Q2. Write the traversal loop and count the nodes of 10, 20, 30, 40, 50.**
The loop above; the count is 5, one increment per hop.

**Q3. Search for 30 in 10, 20, 30, 40, 50 and state the comparisons.**
Found at position 3, 3 comparisons (measured). Search for 35: 5 comparisons, the list runs out,
report absent. Best case 1, worst case n, average (n+1)/2 = 3 for n = 5.

**Q4. Why can binary search not be used on a linked list?**
Binary search needs the middle element in O(1). A linked list reaches the middle in O(n/2) hops,
which destroys the O(log n) advantage. (A skip list is the structure that fixes this, not in this
syllabus.)

**Q5. Insert 5 at the beginning, 60 at the end, 35 at position 4 of the list of five nodes.**
`5 -> 10 -> 20 -> 30 -> 40 -> 50 -> 60` after the first two; inserting 35 at position 4 gives
`5 -> 10 -> 20 -> 35 -> 30 -> 40 -> 50 -> 60`. Cost: O(1), O(n), O(4).

**Q6. Delete the first node and the last node.**
First: O(1), move head then free. Last: O(n), walk to the second last node, free its successor and
set its `next` to NULL.

**Q7. Reverse the list in place.**
Three pointers: `prev = nullptr`, `cur = head`; per node save `nxt = cur->next`, flip
`cur->next = prev`, then advance `prev = cur; cur = nxt`. Finally `head = prev`. O(n) time,
O(1) extra space, no new nodes.

**Q8. Polynomial addition, P1 = 5x^2 + 3x + 1, P2 = 4x^3 + 2x^2 + 6.**
Match exponents, add coefficients: **4x^3 + 7x^2 + 3x + 7** (verified in the lesson run).

**Q9. Merge two sorted lists without creating new nodes.**
Dummy node plus tail pointer; always attach the smaller head; at the end attach the leftover
chain. O(n + m) time, O(1) extra space.

**Q10. Remove duplicates from a sorted list.**
One pointer: while `cur` and `cur->next` exist, if their data is equal, splice out `cur->next`
(`cur->next = cur->next->next`) and delete it, otherwise advance `cur`. Verified: 10 10 20 30 30 30 40
becomes 10 20 30 40. Cost O(n).

## 8. Traps (this is where marks are lost)

| trap | what happens | fix |
|---|---|---|
| head moved before the link | the node points at itself, traversal never ends | link first (`n->next = head`), then `head = n` |
| `Node*` instead of `Node*&` | the insert happens on a photocopy, caller's head stays NULL, node leaks | pass the head by reference, or return the new head |
| `delete` before moving head | head dangles on freed memory | move the pointer first, then delete |
| forgetting the empty list guard | `cur->next` on a NULL head, segfault | `if (head == nullptr)` first |
| not freeing nodes | leak, 16 bytes per lost node | every `new` gets one matching `delete`, walk the list at the end |
| `while (cur != nullptr)` on a circular list | infinite loop | walk a fixed n steps, or detect the cycle |
| losing the successor before the flip in reverse | the rest of the list is unreachable | save `nxt` first |

## 9. Self-check questions

1. State the two fields of a node and why the second one is a pointer.
2. `sizeof(Node)` for an int-valued singly node on a 64 bit machine, and where the padding goes.
3. Give the memory addresses story: why are array nodes 16 bytes apart and linked nodes not.
4. Name three types of linked list and one real use of each.
5. Why is insert-at-beginning O(1) while insert-at-end is O(n)?
6. What does `Node*&` change, and what breaks without it?
7. In which order do you move head and delete when removing the first node, and why?
8. List the three cases of delete-by-value.
9. Explain move-to-front search and which workload it helps.
10. Reverse the list `7 -> 3 -> 9` on paper and give the resulting head.
11. Why is binary search impossible on a singly linked list?
12. Add the polynomials `3x^2 + 2` and `5x^3 + 4x^2 + x` using the node representation.

Answers: see the lesson files, every value above was produced by compiling and running them
(g++ 15.2.0, `-Wall -Wextra -std=c++17`, plus one AddressSanitizer run with zero leaks).

## 10. Files

| file | contents |
|---|---|
| `~/learning/cpp/lessons/06_linked_list_representation.cpp` | node layout, array vs linked addresses, singly, doubly, circular |
| `~/learning/cpp/lessons/07_linked_list_operations.cpp` | traversal, 3 inserts, 3 deletes, cost table |
| `~/learning/cpp/lessons/08_linked_list_searching_apps.cpp` | search with comparison counts, move-to-front, reverse, dedupe, merge, polynomial add, linked stack |
| `diagrams/*.mmd` | six Mermaid sources (rendered, no editor needed: the PNGs sit beside them) |
| `~/learning/cpp/exercises/verify_linked_list_lec6_8.py` | the verifier: compiles the three programs, asserts every printed value, checks the lecture mapping against the handout PDF, checks the diagrams (78 checks, 0 failures) |
| `~/scripts/render-ll-diagrams.sh` | re-renders the six diagrams to PNG |

Diagrams (all rendered next to the sources, each one a focused view, not one giant tree):

| diagram | what it shows |
|---|---|
| `lec6-memory.png` | array block vs scattered heap nodes, with the head pointer |
| `lec6-types.png` | singly, doubly and circular side by side with use and trap |
| `lec7-insert.png` | the three insert patterns with their complexity |
| `lec7-delete.png` | the delete decision flow with the two ordering traps |
| `lec8-search.png` | search 30 vs search 35 with comparison counts |
| `lec8-applications.png` | the six applications and where they lead later in the course |

Build all three:

```bash
cd ~/learning/cpp/lessons && g++ -Wall -Wextra -std=c++17 -o /tmp/lec6 06_linked_list_representation.cpp && /tmp/lec6
python3 ~/learning/cpp/exercises/verify_linked_list_lec6_8.py
sh ~/scripts/render-ll-diagrams.sh
```
