# Experiment 4: Implementation of Priority Queues: Concepts and Operations

**Course:** ELC2132 Data Structures & Algorithms Lab  ·  **Language:** C++ (`g++ -std=c++17`)
**Name:** .............................  **Reg. No.:** .............................
**Batch:** ...................  **Date:** ...................

Source folder: `DAS-LAB/Exp4_Priority_Queue/`  ·  Manual: `DAS-LAB/Manuals/Exp 4_Student.pdf`

---

## Aim

Write C++ programs to understand the concept of a priority queue as an abstract data type, and to
implement it using both an unsorted array and a sorted linked list.

---

## Basic understanding

A priority queue is an abstract data type in which every element carries a priority, and the
element with the highest priority is always served first, regardless of the order in which the
elements were inserted. This is a fundamental departure from an ordinary queue, which is
strictly First-In-First-Out (FIFO).

A useful analogy is a hospital emergency room: a patient with a critical injury is treated before
a patient with a minor cut, even if the latter arrived first. The queue is ordered by urgency
(priority), not by arrival time.

| Operation | Description |
|---|---|
| `insert(item, priority)` | Adds a new element with its associated priority |
| `extractMax()` | Removes and returns the element with the highest priority |
| `peek()` | Returns the highest-priority element without removing it |
| `isEmpty()` | Checks whether the queue currently holds any elements |

There are two natural strategies for storing the elements, and each pushes the cost onto a
different operation:

* **Unsorted array**: new elements are simply appended at the end, so insertion is fast, but
  extraction must scan every element to find the maximum.
* **Sorted linked list**: nodes are kept in descending order of priority at all times, so
  extraction is instant (the maximum is always the head node), but insertion must traverse the
  list to find the correct position and relink the pointers.

| Operation | Unsorted array (Part A) | Sorted linked list (Part B) |
|---|---|---|
| Insert | O(1) | O(n) |
| Extract max | O(n) | O(1) |
| Peek | O(n) | O(1) |

---

## 4.1 Priority Queue Using an Unsorted Array

### Aim

Implement a max-priority queue in C++ using an unsorted array, supporting `insert`, `extractMax`,
`peek` and `isEmpty`.

### Algorithm

1. Start.
2. Declare a structure `Element` with the members `data` and `priority`, an array
   `pq[MAX]` to store the elements, and an integer `n` initialised to 0.
3. **insert(data, priority)**: if `n == MAX`, report overflow; otherwise store the element at
   `pq[n]` and increment `n` (no ordering is maintained, so this is O(1)).
4. **maxIndex()**: set `idx = 0`; for `i = 1` to `n - 1`, if `pq[i].priority > pq[idx].priority`
   then set `idx = i`; return `idx`. This is the linear scan that makes peek and extractMax
   O(n) in the unsorted representation.
5. **peek()**: if `n == 0`, report that the queue is empty; otherwise print
   `pq[maxIndex()]` without removing it.
6. **extractMax()**: if `n == 0`, report that the queue is empty; otherwise set
   `idx = maxIndex()`, print `pq[idx]`, shift every element after `idx` one position to the left and
   decrement `n`.
7. **isEmpty()**: return `n == 0`.
8. Display the menu (insert, extract max, peek, display, isEmpty, exit) and repeat until the
   user chooses to exit.
9. Stop.

### C++ Program

<!--CODE:Exp4_Priority_Queue/exp4_1_priority_queue_array.cpp-->
```cpp
/*
 * Experiment 4.1 : Max-priority queue using an UNSORTED ARRAY
 * Course          : ELC2132 Data Structures & Algorithms Lab
 * Language        : C++
 *
 * Aim             : Implement a priority queue using an unsorted array with the
 *                   operations insert, extractMax, peek and isEmpty.
 *
 * Idea            : a new element is simply put at the end, so insert is O(1).
 *                   The biggest priority is found by checking every element, so
 *                   peek and extractMax are O(n).
 *
 *   insert      O(1)
 *   peek        O(n)
 *   extractMax  O(n)
 *   isEmpty     O(1)
 *
 * Compile : g++ -Wall -Wextra -Wpedantic -std=c++17 -o pq_array exp4_1_priority_queue_array.cpp
 */

#include <iostream>
using namespace std;

#define MAX 100

// One element of the queue: the value and the priority it is served with.
struct Element {
    int data;
    int priority;
};

Element pq[MAX];
int n = 0;      // how many elements are in the queue

bool isEmpty()
{
    return n == 0;
}

bool isFull()
{
    return n == MAX;
}

// Insert at the end, no ordering is kept.
void insertItem(int data, int priority)
{
    if (isFull()) {
        cout << "Queue is full, cannot insert " << data << endl;
        return;
    }

    pq[n].data = data;
    pq[n].priority = priority;
    n = n + 1;

    cout << "Inserted: data = " << data << ", priority = " << priority << endl;
}

// Find the position of the element with the biggest priority.
int maxIndex()
{
    int index = 0;

    for (int i = 1; i < n; i++) {
        if (pq[i].priority > pq[index].priority) {
            index = i;
        }
    }
    return index;
}

// Show the biggest priority without removing it.
void peek()
{
    if (isEmpty()) {
        cout << "Queue is empty, nothing to peek" << endl;
        return;
    }

    int index = maxIndex();
    cout << "Highest priority element: data = " << pq[index].data
         << ", priority = " << pq[index].priority << endl;
}

// Remove the element with the biggest priority.
void extractMax()
{
    if (isEmpty()) {
        cout << "Queue is empty, nothing to extract" << endl;
        return;
    }

    int index = maxIndex();
    cout << "Extracted: data = " << pq[index].data
         << ", priority = " << pq[index].priority << endl;

    // Close the gap by moving the following elements one place to the left.
    for (int i = index; i < n - 1; i++) {
        pq[i] = pq[i + 1];
    }
    n = n - 1;
}

void display()
{
    if (isEmpty()) {
        cout << "Queue is empty" << endl;
        return;
    }

    cout << "Queue elements (in the order they were inserted, size = " << n << "):" << endl;
    for (int i = 0; i < n; i++) {
        cout << "  [" << i << "] data = " << pq[i].data
             << ", priority = " << pq[i].priority << endl;
    }
}

int main()
{
    int choice = 0;
    int data = 0;
    int priority = 0;

    do {
        cout << endl;
        cout << "--- Max Priority Queue (unsorted array) ---" << endl;
        cout << "1. Insert" << endl;
        cout << "2. Extract max" << endl;
        cout << "3. Peek" << endl;
        cout << "4. Display" << endl;
        cout << "5. isEmpty" << endl;
        cout << "6. Exit" << endl;
        cout << "Enter your choice: ";
        cin >> choice;

        if (choice == 1) {
            cout << "Enter data and priority: ";
            cin >> data >> priority;
            insertItem(data, priority);
        } else if (choice == 2) {
            extractMax();
        } else if (choice == 3) {
            peek();
        } else if (choice == 4) {
            display();
        } else if (choice == 5) {
            if (isEmpty()) {
                cout << "Queue is empty" << endl;
            } else {
                cout << "Queue is not empty" << endl;
            }
        } else if (choice == 6) {
            cout << "Exiting." << endl;
        } else {
            cout << "Wrong choice, enter a number from 1 to 6." << endl;
        }
    } while (choice != 6);

    return 0;
}
```

### Output

Four elements are inserted in the order (10, priority 3), (20, 1), (30, 5), (40, 2), so the highest
priority belongs to the third element inserted. The transcript below is the actual session inside a
terminal, with the typed menu choices echoed after their prompts.

<!--TEXT:_build/out/exp4_1_pq_array.txt-->
```

--- Max Priority Queue (unsorted array) ---
1. Insert
2. Extract max
3. Peek
4. Display
5. isEmpty
6. Exit
Enter your choice: 1
Enter data and priority: 10 3
Inserted: data = 10, priority = 3

--- Max Priority Queue (unsorted array) ---
1. Insert
2. Extract max
3. Peek
4. Display
5. isEmpty
6. Exit
Enter your choice: 1
Enter data and priority: 20 1
Inserted: data = 20, priority = 1

--- Max Priority Queue (unsorted array) ---
1. Insert
2. Extract max
3. Peek
4. Display
5. isEmpty
6. Exit
Enter your choice: 1
Enter data and priority: 30 5
Inserted: data = 30, priority = 5

--- Max Priority Queue (unsorted array) ---
1. Insert
2. Extract max
3. Peek
4. Display
5. isEmpty
6. Exit
Enter your choice: 1
Enter data and priority: 40 2
Inserted: data = 40, priority = 2

--- Max Priority Queue (unsorted array) ---
1. Insert
2. Extract max
3. Peek
4. Display
5. isEmpty
6. Exit
Enter your choice: 4
Queue elements (in the order they were inserted, size = 4):
  [0] data = 10, priority = 3
  [1] data = 20, priority = 1
  [2] data = 30, priority = 5
  [3] data = 40, priority = 2

--- Max Priority Queue (unsorted array) ---
1. Insert
2. Extract max
3. Peek
4. Display
5. isEmpty
6. Exit
Enter your choice: 3
Highest priority element: data = 30, priority = 5

--- Max Priority Queue (unsorted array) ---
1. Insert
2. Extract max
3. Peek
4. Display
5. isEmpty
6. Exit
Enter your choice: 2
Extracted: data = 30, priority = 5

--- Max Priority Queue (unsorted array) ---
1. Insert
2. Extract max
3. Peek
4. Display
5. isEmpty
6. Exit
Enter your choice: 3
Highest priority element: data = 10, priority = 3

--- Max Priority Queue (unsorted array) ---
1. Insert
2. Extract max
3. Peek
4. Display
5. isEmpty
6. Exit
Enter your choice: 4
Queue elements (in the order they were inserted, size = 3):
  [0] data = 10, priority = 3
  [1] data = 20, priority = 1
  [2] data = 40, priority = 2

--- Max Priority Queue (unsorted array) ---
1. Insert
2. Extract max
3. Peek
4. Display
5. isEmpty
6. Exit
Enter your choice: 5
Queue is not empty

--- Max Priority Queue (unsorted array) ---
1. Insert
2. Extract max
3. Peek
4. Display
5. isEmpty
6. Exit
Enter your choice: 6
Exiting.
```

### Observation

The display shows the array in insertion order, which confirms that `insert` performs no
ordering at all and is therefore O(1). Even so, `peek` correctly reported the element with
priority 5, and `extractMax` removed that same element, after which `peek` returned the next
highest priority, 3. Both of these operations had to scan the array, which is the O(n) cost of
the unsorted representation. The `isEmpty` check returned "not empty" while three elements
remained, and the empty-queue case was checked separately: extracting and peeking on an
empty queue print an error message instead of reading invalid memory.

---

## 4.2 Priority Queue Using a Sorted Linked List

### Aim

Implement a max-priority queue in C++ using a singly linked list kept in descending order of
priority, supporting `insert`, `extractMax`, `peek` and `isEmpty`.

### Algorithm

1. Start.
2. Define a self-referential structure `Node` with the members `data`, `priority` and
   `next`, and declare `head = NULL` and `count = 0`.
3. **insert(data, priority)**:
   1. Allocate a new node and store the data and priority in it.
   2. If `head == NULL` or the new priority is greater than `head->priority`, make the new node
      the head (it outranks every element already in the list).
   3. Otherwise traverse the list while the next node has a priority greater than or equal to
      the new priority, then insert the new node after that position. This keeps the list sorted
      in descending order and makes insertion O(n).
   4. Increment `count`.
4. **peek()**: if `head == NULL`, report that the queue is empty; otherwise print the data and
   priority of the head node. Because the list is always sorted, this is O(1).
5. **extractMax()**: if `head == NULL`, report that the queue is empty; otherwise print the
   head node, unlink it by setting `head = head->next`, free the removed node and decrement
   `count`. This is O(1).
6. **isEmpty()**: return `head == NULL`.
7. Display the menu and repeat until the user chooses to exit; free all remaining nodes
   before terminating.
8. Stop.

### C++ Program

<!--CODE:Exp4_Priority_Queue/exp4_2_priority_queue_linkedlist.cpp-->
```cpp
/*
 * Experiment 4.2 : Max-priority queue using a SORTED LINKED LIST
 * Course          : ELC2132 Data Structures & Algorithms Lab
 * Language        : C++
 *
 * Aim             : Implement a priority queue using a singly linked list that is
 *                   always kept in descending order of priority, with the
 *                   operations insert, extractMax, peek and isEmpty.
 *
 * Idea            : the list stays sorted, so the biggest priority is always the
 *                   first node. peek and extractMax are O(1), but insert has to
 *                   walk the list to find the right place, so it is O(n).
 *
 *   insert      O(n)
 *   peek        O(1)
 *   extractMax  O(1)
 *   isEmpty     O(1)
 *
 * Compile : g++ -Wall -Wextra -Wpedantic -std=c++17 -o pq_list exp4_2_priority_queue_linkedlist.cpp
 */

#include <iostream>
using namespace std;

// One node of the list: the value, its priority and the address of the next node.
struct Node {
    int data;
    int priority;
    Node* next;
};

Node* head = NULL;   // the element with the biggest priority
int count = 0;       // how many elements are in the list

bool isEmpty()
{
    return head == NULL;
}

// Insert the new element so that the list stays in descending order.
void insertItem(int data, int priority)
{
    Node* node = new Node();
    node->data = data;
    node->priority = priority;
    node->next = NULL;

    // Case 1: the new element has the biggest priority, so it becomes the head.
    if (head == NULL || priority > head->priority) {
        node->next = head;
        head = node;
    } else {
        // Case 2: walk until the next node has a smaller priority.
        Node* current = head;
        while (current->next != NULL && current->next->priority >= priority) {
            current = current->next;
        }
        node->next = current->next;
        current->next = node;
    }

    count = count + 1;
    cout << "Inserted: data = " << data << ", priority = " << priority << endl;
}

// Show the biggest priority without removing it.
void peek()
{
    if (isEmpty()) {
        cout << "Queue is empty, nothing to peek" << endl;
        return;
    }

    cout << "Highest priority element: data = " << head->data
         << ", priority = " << head->priority << endl;
}

// Remove the first node, which always has the biggest priority.
void extractMax()
{
    if (isEmpty()) {
        cout << "Queue is empty, nothing to extract" << endl;
        return;
    }

    Node* temp = head;
    cout << "Extracted: data = " << temp->data
         << ", priority = " << temp->priority << endl;

    head = head->next;
    delete temp;
    count = count - 1;
}

void display()
{
    if (isEmpty()) {
        cout << "Queue is empty" << endl;
        return;
    }

    cout << "Queue elements (highest priority first, size = " << count << "):" << endl;
    for (Node* current = head; current != NULL; current = current->next) {
        cout << "  data = " << current->data << ", priority = " << current->priority;
        if (current == head) {
            cout << "  <- head (max)";
        }
        cout << endl;
    }
}

// Free the remaining nodes before the program ends.
void freeQueue()
{
    while (head != NULL) {
        Node* temp = head;
        head = head->next;
        delete temp;
    }
    count = 0;
}

int main()
{
    int choice = 0;
    int data = 0;
    int priority = 0;

    do {
        cout << endl;
        cout << "--- Max Priority Queue (sorted linked list) ---" << endl;
        cout << "1. Insert" << endl;
        cout << "2. Extract max" << endl;
        cout << "3. Peek" << endl;
        cout << "4. Display" << endl;
        cout << "5. isEmpty" << endl;
        cout << "6. Exit" << endl;
        cout << "Enter your choice: ";
        cin >> choice;

        if (choice == 1) {
            cout << "Enter data and priority: ";
            cin >> data >> priority;
            insertItem(data, priority);
        } else if (choice == 2) {
            extractMax();
        } else if (choice == 3) {
            peek();
        } else if (choice == 4) {
            display();
        } else if (choice == 5) {
            if (isEmpty()) {
                cout << "Queue is empty" << endl;
            } else {
                cout << "Queue is not empty" << endl;
            }
        } else if (choice == 6) {
            cout << "Exiting." << endl;
        } else {
            cout << "Wrong choice, enter a number from 1 to 6." << endl;
        }
    } while (choice != 6);

    freeQueue();
    return 0;
}
```

### Output

The same four elements are inserted in the same order as in Part A, so that the two
implementations can be compared directly. The transcript below is again the actual terminal
session, with the typed menu choices echoed after their prompts.

<!--TEXT:_build/out/exp4_2_pq_list.txt-->
```

--- Max Priority Queue (sorted linked list) ---
1. Insert
2. Extract max
3. Peek
4. Display
5. isEmpty
6. Exit
Enter your choice: 1
Enter data and priority: 10 3
Inserted: data = 10, priority = 3

--- Max Priority Queue (sorted linked list) ---
1. Insert
2. Extract max
3. Peek
4. Display
5. isEmpty
6. Exit
Enter your choice: 1
Enter data and priority: 20 1
Inserted: data = 20, priority = 1

--- Max Priority Queue (sorted linked list) ---
1. Insert
2. Extract max
3. Peek
4. Display
5. isEmpty
6. Exit
Enter your choice: 1
Enter data and priority: 30 5
Inserted: data = 30, priority = 5

--- Max Priority Queue (sorted linked list) ---
1. Insert
2. Extract max
3. Peek
4. Display
5. isEmpty
6. Exit
Enter your choice: 1
Enter data and priority: 40 2
Inserted: data = 40, priority = 2

--- Max Priority Queue (sorted linked list) ---
1. Insert
2. Extract max
3. Peek
4. Display
5. isEmpty
6. Exit
Enter your choice: 4
Queue elements (highest priority first, size = 4):
  data = 30, priority = 5  <- head (max)
  data = 10, priority = 3
  data = 40, priority = 2
  data = 20, priority = 1

--- Max Priority Queue (sorted linked list) ---
1. Insert
2. Extract max
3. Peek
4. Display
5. isEmpty
6. Exit
Enter your choice: 3
Highest priority element: data = 30, priority = 5

--- Max Priority Queue (sorted linked list) ---
1. Insert
2. Extract max
3. Peek
4. Display
5. isEmpty
6. Exit
Enter your choice: 2
Extracted: data = 30, priority = 5

--- Max Priority Queue (sorted linked list) ---
1. Insert
2. Extract max
3. Peek
4. Display
5. isEmpty
6. Exit
Enter your choice: 3
Highest priority element: data = 10, priority = 3

--- Max Priority Queue (sorted linked list) ---
1. Insert
2. Extract max
3. Peek
4. Display
5. isEmpty
6. Exit
Enter your choice: 4
Queue elements (highest priority first, size = 3):
  data = 10, priority = 3  <- head (max)
  data = 40, priority = 2
  data = 20, priority = 1

--- Max Priority Queue (sorted linked list) ---
1. Insert
2. Extract max
3. Peek
4. Display
5. isEmpty
6. Exit
Enter your choice: 5
Queue is not empty

--- Max Priority Queue (sorted linked list) ---
1. Insert
2. Extract max
3. Peek
4. Display
5. isEmpty
6. Exit
Enter your choice: 6
Exiting.
```

### Observation

The display shows the list already in descending order of priority after the four insertions,
which confirms that `insert` walks the list and places every node in its correct position, making
it O(n). The element `(30, 5)` was placed at the head even though it was inserted third, and the
element `(40, 2)` landed between `(10, 3)` and `(20, 1)`. Because the head is always the
maximum, `peek` and `extractMax` needed no traversal and returned `(30, 5)` immediately, after
which `(10, 3)` became the new head. Every allocated node was released with `delete`, both on
extraction and by `freeQueue` at exit; a run under AddressSanitizer and UndefinedBehaviorSanitizer
reported no leaks and no errors.

---

## Comparison of the two implementations

Both programs produced exactly the same sequence of results for the same input (insert four
elements, peek, extract max, peek again), which shows that the abstract data type behaves
identically even though the storage strategy is completely different. Only the cost of the
operations moves:

| Operation | Unsorted array | Sorted linked list |
|---|---|---|
| Insert | O(1): append at `pq[size]` | O(n): traverse to the correct position |
| Peek | O(n): linear scan for the maximum | O(1): the head is the maximum |
| Extract max | O(n): scan, then shift the hole left | O(1): unlink the head |
| Storage | fixed capacity `MAX`, possible overflow | grows on demand, needs `new`/`delete` |
| Order of elements in memory | insertion order | descending priority |

So neither representation is better in all cases: the array is preferable when insertions
dominate, and the linked list is preferable when the highest-priority element is extracted
repeatedly, which is exactly the access pattern of a scheduler or an event-driven simulation.

## Conclusion

Both priority queues were implemented in C++ and executed successfully. The unsorted-array
version performed `insert` in constant time but had to scan the array for `peek` and
`extractMax`; the sorted-linked-list version kept itself ordered at insertion cost so that
`peek` and `extractMax` became constant-time operations. For the same input sequence both
versions always returned the element with the highest priority first, which verified that a
priority queue is an abstract data type, that is, defined by its operations rather than by its
implementation. The programs also handled the boundary cases correctly: inserting into a full
array, extracting from or peeking into an empty queue, equal priorities (the element inserted
earlier is served first in both implementations) and the release of all dynamically allocated
nodes. Hence the aim of understanding and implementing a priority queue using both an
unsorted array and a sorted linked list was achieved.

---

### Files

| File | Description |
|---|---|
| `Exp4_Priority_Queue/exp4_1_priority_queue_array.cpp` | Part A: max-priority queue using an unsorted array |
| `Exp4_Priority_Queue/exp4_2_priority_queue_linkedlist.cpp` | Part B: max-priority queue using a sorted linked list |
| `_build/out/exp4_1_pq_array.txt` | Raw sample run of Part A |
| `_build/out/exp4_2_pq_list.txt` | Raw sample run of Part B |
