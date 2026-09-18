# 🚀 C++ Language Plan for DSA (ECE2104)

The language you need for this course is **C++**. Built from the course handout.
Use with: `~/Documents/MUJ-SEM-3/DSA/DSA-STUDY-PLAN.md` (full 18-week semester plan).
This doc is the LANGUAGE bootstrap: 12 steps, ~2 to 3 weeks, 1 to 1.5 hours/day.

---

## WHY C++ AND NOT PYTHON?

The handout says "using C++ or Python", but the evidence points one way:

| Evidence | What it says |
|---|---|
| Syllabus textbook #1 | Yashavant Kanetkar, *Data Structures Through C++* |
| Other textbooks | 6 of 8 are C++ (Drozdek, Goodrich, Hubbard, Weiss, Shaffer, Lewis) |
| Course Outcome 5 | "develop **C++** programs for simple applications" |
| Your DAS Lab (ELC2132) | already C++: `dsa-lab/stack1.cpp` is a C++ class |
| Your current skill level | you already did C up to pointers, heap, linked lists, quicksort |

C++ is literally "C plus plus": everything you know in C still works. You are closer
to C++ than to Python. So: **C++ it is.**

---

## THE ROADMAP (12 steps)

```
PHASE A  C++ as "C with nicer clothes"        steps 1-4   → handout Lectures 2-3
PHASE B  Where C++ beats C for DSA            steps 5-8   → handout Lectures 4-5
PHASE C  The DSA toolbox (OOP + STL)          steps 9-12  → handout Lectures 5-8, 11, CO1
```

Every step = **READ** (your files) + **CODE** (you type it) + **CHECK** (you pass it).

---

# PHASE A: C++ AS "C WITH NICER CLOTHES"

## STEP 1: First C++ program + the compiler

**Maps to**: Lecture 2 (Introduction to C++, Features, Program Structure)

**READ**: `~/learning/cpp/lessons/01_basics.cpp`

**KEY IDEA**: C++ = C + tools. Same machine, same memory, but `printf` becomes `cout`,
`scanf` becomes `cin`, and you get `string` as a real type.

```
   C                         C++
   -----------------         --------------------------------
   #include <stdio.h>        #include <iostream>
   printf("hi %d", x);       cout << "hi " << x;      ( << = "pump into screen")
   scanf("%d", &x);          cin >> x;                ( >> = "pull from keyboard")
```

**CODE**: type this yourself, compile, run:
```cpp
#include <iostream>
using namespace std;

int main() {
    cout << "Hello DSA" << endl;
    return 0;
}
```
Compile: `g++ -Wall -std=c++17 -o hello hello.cpp` → run: `./hello`

**CHECK**: What does `endl` do that `"\n"` does not? (flush the output buffer).
If you don't know, that's fine: it matters only for debugging later, look it up when curious.

---

## STEP 2: Variables, data types, input/output

**Maps to**: Lecture 3 (Basic Terms: Variables, Data Types, Operators, Input/Output)

**READ**: `~/learning/cpp/lessons/02_variables.cpp`
Contrast with your C lessons: `~/learning/c/lessons/01_printf.c` and `02_scanf.c`

**KEY IDEA**: The types you already know from C are joined by `bool` and `string`.
New in C++: `int x{5};` brace init (yells at you for overflow, C's `int x = 5;` stays silent).

```
C basic types        C++ adds
-----------          ---------
int, float, double   bool      (true/false, not 1/0)
char                 string    ("hello" is no longer char[])
```

**CODE**: write a program that asks for your name (getline!) and age, prints
"X, you are Y years old. In 10 years you will be Y+10."
Use `getline(cin, name)` for names with spaces. Cin alone stops at the space.

**CHECK**: What happens if you `cin >>` a number but type a letter? (broken input state)
Why do we use `getline` for names? (spaces). If your program eats the Enter key after
a number, you just met `cin.ignore()`: search it, one line, done.

---

## STEP 3: Operators

**Maps to**: Lecture 3

**READ**: `~/learning/c/lessons/04_operators.c` (operators are IDENTICAL to C, skim it)

**KEY IDEA**: All of C's operators work: `+ - * / %`, `== != < > <= >=`, `&& || !`, `+=` etc.
C++ adds exactly one new operator you will use constantly with streams:

```
  C       C++
  ---     ---------------
          <<   "put into"  (cout << x)
          >>   "take from" (cin >> x)
```

**CODE**: write a "number chop" program: read a number, print its last digit
(`n % 10`) and its remaining part (`n / 10`).

**CHECK**: What is `7 % 3`? What is `7 / 3` (ints)? This modulo trick is the backbone
of stack/queue circular buffers later. Never skip it.

---

## STEP 4: Control flow

**Maps to**: Lecture 4 (Control Statements and Functions, overview)

**READ**: `~/learning/cpp/lessons/03_control_flow.cpp` + `~/learning/c/lessons/05_control_flow.c`
and `06_loops.c` (same syntax, C lessons are fine)

**KEY IDEA**: `if/else`, `switch`, `for`, `while`, `do-while`: all byte-for-byte the same
as C. The ONLY C++ addition you see in class:

```
for (int x : myVector)   →  range-based for, reads every element without an index
```

**CODE**: print a multiplication table 1..10 with nested loops. Then rewrite the inner
loop as a `while`. Both must give identical output.

**CHECK**: When does `switch` beat a chain of `if/else`? (fixed integer options, exam
asks this). Know `break` inside switch, and what happens WITHOUT it. (fallthrough!)

---

# PHASE B: WHERE C++ BEATS C FOR DSA

## STEP 5: Functions + the reference `&` (the big deal)

**Maps to**: Lecture 4 (Functions) + recursion teaser (Lecture 16)

**READ**: `~/learning/cpp/lessons/04_functions.cpp` (this lesson was broken, fixed Aug 17:
the celsiusToFahrenheit prototype was missing, lesson now compiles). Contrast with C:
`~/learning/c/lessons/07_functions.c`

**KEY IDEA**: In C, functions pass COPIES of arguments. To change the original you used
pointers (`*`). C++ gives you **references** (`&`): an alias, no copy, no dereference.

```
C version                    C++ version (the same swap)
----------                   ---------------------------
void swap(int *a, int *b){   void swap(int &a, int &b){
    int t = *a;                  int t = a;
    *a = *b;                     a = b;
    *b = t;                      b = t;
}                            }
swap(&x, &y);                swap(x, y);   ← simpler at the call site
```

DSA uses references EVERYWHERE: sorting swaps, tree insert nodes, `findMin(root, ...)`.

**CODE**: write `void swap(int& a, int& b)` and use it on two variables. Then write
`int factorial(int n)` recursively BY HAND from the notes, and trace `factorial(4)`
on paper: how many calls stack up?

**CHECK**: Recursion needs a base case, or what? (infinite recursion = stack overflow
crash). Can `main` call a function defined after it? (only with a prototype, you just
fixed this exact bug!).

---

## STEP 6: std::string, the real string type

**Maps to**: Lectures 3-4 (syntax) + everything else (strings in trees? no, but in token
processing, yes)

**READ**: none of your lessons cover this deeply yet, so this step is new territory.
`std::string` lives in `<string>`.

**KEY IDEA**:
```
C:        char name[100];  fixed size, manual \0, strcpy, strcat, strcmp
C++:      string name;     grows itself, has methods, = , + , ==  all work
```
Methods you will use in DSA: `s.length()`, `s.substr(start, len)`, `s.find(x)`,
`s[i]`, `getline(cin, s)`, `s.push_back(c)`, `to_string(n)`, `stoi(s)`.

**CODE**: read a line, print it reversed (loop `s.length()-1` down to 0). Then check
if the line is a palindrome (compare s[i] vs s[n-1-i], bail early on mismatch).

**CHECK**: What does `s.find('x')` return if not found? (string::npos, a huge number).
Why must you compare with `!= string::npos` instead of `== -1`? (portability)

---

## STEP 7: Arrays and std::vector (dynamic arrays)

**Maps to**: Lecture 5 (Introduction to Data Structures and Memory Representation)

**READ**: `~/learning/c/lessons/08_arrays.c`, `09_arrays_strings.c`,
`10_stack_vs_heap.c` (memory!), and `11_globals_and_segments.c`

**KEY IDEA**:

```
C array (fixed, static)          C++ vector (dynamic, on the heap)
-----------------------          -------------------------------
int a[10];                       vector<int> v;          (empty)
no size stored                   v.size()
can't grow                       v.push_back(42)        (grows automatically)
                                 v.pop_back()
0-based indexing, same           v[i], same 0-based
```

`std::vector` is the workhorse container of DSA labs. Know it before anything else.

**CODE**: push 10 numbers into a vector, print them with a range-for:
`for (int x : v)`. Then remove the last 3 with pop_back. Print its size.

**CHECK**: Difference between `v.size()` and `v.capacity()`? (size = elements now,
capacity = memory reserved). Capacity shoots up in doublings, that is amortized O(1)
push_back, exam phrase: "amortized constant". Big-O of random access in a vector? (O(1),
contiguous memory, same as array).

---

## STEP 8: Pointers, new/delete, and `->` (linked list fuel)

**Maps to**: Lectures 5-6 (Linked List: Concept, Types, Node Representation)

**READ**: `~/learning/c/lessons/10_stack_vs_heap.c` and `11_globals_and_segments.c`
(MALLOC lives on the heap. C++ says: use `new` instead of `malloc`.)

**KEY IDEA**:

```
C:  Node* n = (Node*) malloc(sizeof(Node));   free(n);
C++:Node* n = new Node();                      delete n;
                         ↑ constructor runs, memory + object init in one shot
```

Linked list node in C++ (you wrote a C version in `c/practice/linked_lists/list.c`):
```cpp
struct Node {
    int data;
    Node* next;        // C++ allows "Node*" inside the struct, C makes you write "struct Node*"
};
```
Arrow rules: `n->data` means "the thing n points to, its data".

**CODE**: build a 3-node list by hand with `new`, walk it with a while loop, print
each data, then `delete` all three in order. (This is the skeleton for every lab.)

**CHECK**: What is a dangling pointer? (freed memory, still holding an address).
Rule to survive: after `delete n;` set `n = nullptr;`. `nullptr` is C++'s honest NULL.

---

# PHASE C: THE DSA TOOLBOX

## STEP 9: struct vs class, your first Abstract Data Type

**Maps to**: CO1 (Explain concepts of object-oriented programming using C++/Python)
+ Lecture 11 (Stack: concept and array representation)

**READ**: `~/learning/dsa-lab/stack1.h` + `stack1.cpp` + `main.cpp` (your OWN stack
from today! this step is re-reading it and understanding what you already built).

**KEY IDEA**: struct and class are the same thing, one difference:

```
struct Node { ... };      → everything public (default)
class Stack { ... };      → everything private (default), you must say public:
```

An ADT (abstract data type) = data + the operations allowed on it, with the guts hidden:

```
class Stack {
private:
    int arr[100];    // data: hidden, users can't touch
    int top;
public:
    void push(int x);   // operations: the only door
    int pop();
};
```

**CODE**: write a `class Counter` from scratch: private int value, public methods
`increment()`, `getValue()`, constructor setting value to 0. Use it in main.

**CHECK**: Why hide the data? (so nobody can corrupt top/arr and break the stack
invariant). Your own stack1.cpp: what would break if main could set `top = -5` directly?

---

## STEP 10: STL: the official DSA toolkit

**Maps to**: Lectures 11-15 (Stack, Queue) and 22-23 (Heap/Priority Queue) prep

**KEY IDEA**: C++ ships ready-made versions of everything the course teaches.
One line each. You MUST know these exist before the labs, and know the cost:

| STL container    | What it is                  | push        | pop      | peek    |
|------------------|-----------------------------|-------------|----------|---------|
| `stack<T>`       | LIFO stack                  | push()      | pop()    | top()   |
| `queue<T>`       | FIFO queue                  | push()      | pop()    | front() |
| `priority_queue<T>` | max-heap                | push()      | pop()    | top()   |
| `vector<T>`      | dynamic array               | push_back() | pop_back()| [i]    |
| `map<K,V>`       | sorted key-value tree (log n)| [k]=v      | erase(k) | find(k) |
| `unordered_map<K,V>` | hash table (O(1) avg)    | [k]=v      | erase(k) | find(k) |

**CODE**: `stack<int> s;` push 1,2,3 → print top, pop, print top again. Same with queue.
Then `unordered_map<string,int>`: count how many times each word appears in a sentence
(loop words, `count[word]++`). This is literally the Two Sum weapon from the semester plan.

**CHECK**: Lab hint: the course makes you IMPLEMENT stack/queue yourself (array +
linked versions), that is the assignment. STL is your cross-check tool: run both,
outputs must match. Know the Big-O of each STL operation for viva: O(1) stack/queue
push-pop, O(log n) priority_queue push-pop, O(1) avg unordered_map.

---

## STEP 11: Recursion deep dive (the wall you must climb)

**Maps to**: Lecture 16 (Recursion and Recursive Algorithms), it is the single most
examined mental skill in this course (trees ARE recursion)

**READ**: the recursion section inside `~/learning/cpp/lessons/04_functions.cpp`
(factorial worked example + mental model comments)

**KEY IDEA**: every recursion = base case (stop) + recursive case (shrink problem).
Trees later: "solve left, solve right, combine" is the SAME shape as factorial.

```
factorial(3):              countdown(n):           tree height(n):
 3 * fact(2)                 n=3 → 2 → 1 → 0       1 + max(height(left), height(right))
 3 * (2 * fact(1))           base case n=0, stop    ← that triple-lines pattern runs
 3 * (2 * (1 * fact(0)))                             every tree problem you will see
 3 * (2 * (1 * 1)) = 6
```

**CODE**: write recursive (1) factorial, (2) fibonacci, (3) countdown, (4) sum of array
`int sum(int a[], int n)` where the subcall is `sum(a, n-1) + a[n-1]`.
Then trace fibonacci(5) on paper. Count how many times fib(3) is computed. (That
redundancy is why we later learn DP, but the exam just wants the recursion.)

**CHECK**: write the BASE CASE first, always. What happens without it? (stack overflow,
segfault). Handout mentions "recursive algorithms" in outcomes: viva will ask
"which is better, iterative or recursive?": answer = same logic, recursion is cleaner
for trees, but eats stack frames, iterative avoids the overhead.

---

## STEP 12: THE GAUNTLET: linked list, stack, queue from scratch in C++

**Maps to**: Lectures 6-8 (all linked list ops) + 11-15 (stack/queue array + linked).
This is your DAS Lab assignment baseline, and the mid-term speed run.

**READ before writing**: `~/learning/c/practice/linked_lists/list.c` +
`~/learning/c/practice/linked_lists/addresses.c` (your C linked list work, port to C++)

**CODE (3 programs, one per day)**:
1. **SinglyLinkedList** class: Node struct with `new`/`delete`, methods insertAtHead,
   insertAtTail, deleteValue, print, search. Walk every method manually with a whiteboard
   drawing: arrows BEFORE you change them, or you leak the list. (Draw-then-code rule.)
2. **Stack** class: array version (your stack1.cpp) AND linked version (reuse the Node
   from program 1, push_front/pop_front). Same behavior, two memories.
3. **Queue** class: array version (circular buffer with the `%` from step 3!) and linked
   version (insert at tail, delete at head).

**CHECK (this is the whole course's foundation)**: can you, with a blank editor and a
15-minute timer, write all three data structures from memory? That speed run is your
week-6 mid-term prep (the semester plan says exactly this). If yes: you are fluent
enough that everything after (trees, heaps, graphs) is just more of the same shape
with different pointers.

---

## SUGGESTED PACE (2.5 weeks, 1-1.5 hr/day)

| Day  | Step                            | Handout lecture covered |
|------|---------------------------------|-------------------------|
| 1    | Step 1: compiler + hello        | Lec 2                   |
| 2    | Step 2: variables + I/O         | Lec 3                   |
| 3    | Step 3: operators               | Lec 3                   |
| 4    | Step 4: control flow            | Lec 4                   |
| 5    | Step 5: functions + references  | Lec 4                   |
| 6    | Step 6: std::string             | Lec 3-4                 |
| 7    | Step 7: arrays + vector         | Lec 5                   |
| 8    | Step 8: pointers + new/delete   | Lec 5-6                 |
| 9    | Step 9: struct vs class (ADT)   | CO1 / Lec 11            |
| 10   | Step 10: STL tour               | Lec 11-15, 22-23        |
| 11   | Step 11: recursion deep dive    | Lec 16                  |
| 12-15| Step 12: the gauntlet (3 labs)  | Lec 6-8, 11-15          |

Finish by the time lectures hit **Linked List operations (Lec 7)** and you will cruise
the rest of the semester. The DSA-STUDY-PLAN.md then takes over from its own Week 3.

---

## PITFALLS (the things that eat beginners)

1. `cin >> x` leaves the Enter in the buffer. Fix: `cin.ignore()` after numeric input
   before getline. Or the next getline comes back empty and you debug for an hour.
2. Forgetting `delete` = memory leak; deleting twice = crash. Rule: every `new` gets
   exactly one matching `delete`, then set `= nullptr`.
3. `using namespace std;` is fine for labs, but the exam/industry marks it as sloppy.
   Use `std::cout` in your exam answers, or don't write it. (Know both forms.)
4. `vector<int> v[10];` is 10 vectors. `vector<int> v(10);` is ONE vector of size 10.
   This parens/brackets trap is a classic viva question.
5. When comparing strings: `==` works, do NOT strcmp.

---

## RESOURCES

- Your own lessons: `~/learning/cpp/lessons/` and `~/learning/c/lessons/` (primary)
- Programiz C++ tutorials (pointers, references, vectors): programiz.com/cpp-programming
- learncpp.com (only if you want the full depth)
- Your own lab code: `~/learning/dsa-lab/` (proof you already know more than you think)

Good luck. The language is the easy 20%. The data structures are the other 80%,
and you now have the toolkit to meet them.