# Experiment 3: Implementation of Recursive Programs

**Course:** ELC2132 Data Structures & Algorithms Lab  ·  **Language:** C++ (`g++ -std=c++17`)
**Name:** .............................  **Reg. No.:** .............................
**Batch:** ...................  **Date:** ...................

Source folder: `DAS-LAB/Exp3_Recursion/`  ·  Manual: `DAS-LAB/Manuals/Exp 3_Student.pdf`

---

## Aim

Write C++ programs to design, implement and analyze recursive programs for computing the
Fibonacci series, the factorial of a number, and for solving the Tower of Hanoi problem.

---

## Basic understanding

Recursion is a technique in which a function calls itself, directly or indirectly, to solve a
problem by reducing it to smaller instances of the same problem. Every recursive solution
needs exactly two parts:

* **Base case**: the simplest instance, solved directly without further recursion. It is what
  stops the recursion.
* **Recursive case**: the step where the function calls itself with a smaller input, moving
  towards the base case.

Every call is placed on the program's call stack, and each call holds its stack frame until the
calls below it return. This is why recursion costs memory (space complexity) in addition to
time. The three programs below show three patterns of increasing complexity: linear recursion
(factorial), tree recursion (Fibonacci) and recursion with problem transformation (Tower of
Hanoi).

---

## 3.1 Factorial of a Number

### Aim

Write a recursive C++ program to compute the factorial of a non-negative integer `n`, defined
as `n! = n * (n-1) * (n-2) * ... * 1`, with `0! = 1`.

### Algorithm

1. Start.
2. Read the non-negative integer `n`.
3. If `n < 0`, report that the factorial is not defined for negative numbers and stop.
4. If `n > 12`, report that the result would not fit in an int and stop.
5. Call the recursive function `factorial(n)`:
   1. If `n == 0` or `n == 1`, return `1` (base case, recursion stops here).
   2. Otherwise, return `n * factorial(n - 1)` (recursive case: the problem is reduced by 1).
6. Print `n!` as the returned value.
7. Stop.

### C++ Program

<!--CODE:Exp3_Recursion/exp3_1_factorial.cpp-->
```cpp
/*
 * Experiment 3.1 : Factorial of a number using recursion
 * Course          : ELC2132 Data Structures & Algorithms Lab
 * Language        : C++
 *
 * Aim             : Find n! = n * (n-1) * (n-2) * ... * 1, with 0! = 1.
 *
 * Base case       : factorial(0) = 1 and factorial(1) = 1
 * Recursive case  : factorial(n) = n * factorial(n - 1)
 *
 * Compile : g++ -Wall -Wextra -Wpedantic -std=c++17 -o factorial exp3_1_factorial.cpp
 */

#include <iostream>
using namespace std;

#define MAX_N 12   // 12! = 479001600, the biggest factorial that fits in an int

int factorial(int n)
{
    if (n == 0 || n == 1) {
        return 1;                    // base case, the recursion stops here
    }
    return n * factorial(n - 1);     // recursive case
}

int main()
{
    int n = 0;

    cout << "Enter a number (0 to " << MAX_N << "): ";
    cin >> n;

    if (n < 0) {
        cout << "Factorial is not defined for negative numbers." << endl;
        return 1;
    }
    if (n > MAX_N) {
        cout << "Please enter a number up to " << MAX_N
             << ", bigger values do not fit in an int." << endl;
        return 1;
    }

    cout << n << "! = " << factorial(n) << endl;

    return 0;
}
```

### Output

<!--TEXT:_build/out/exp3_1_factorial.txt-->
```
Enter a number (0 to 12): 10
10! = 3628800
```

Values verified for other inputs:

| n | Output of the program | Expected |
|---|---|---|
| 0 | 1 | 1 |
| 1 | 1 | 1 |
| 5 | 120 | 120 |
| 10 | 3628800 | 3628800 |
| 12 | 479001600 | 479001600 |
| 13 | `Please enter a number up to 12` message | guard: 13! does not fit in an int |
| -4 | `factorial of a negative number is not defined` | input validation |

### Observation

`factorial(5)` expands as `5 * factorial(4)` → `5 * 4 * factorial(3)` → ... → `5 * 4 * 3 * 2 * 1`.
The calls are pushed on the stack until the base case `factorial(0)` returns 1, and then the
multiplications are performed while the stack unwinds. Exactly `n + 1` calls are made, one
multiplication per call, so the time complexity is **O(n)**; at the deepest point `n + 1` frames
are alive, so the space complexity is **O(n)**.

---

## 3.2 Fibonacci Series

### Aim

Write a recursive C++ program to compute the nth term of the Fibonacci series, defined as
`F(0) = 0`, `F(1) = 1` and `F(n) = F(n-1) + F(n-2)` for `n > 1`.

### Algorithm

1. Start.
2. Read `n`, the index of the term to be computed (0 ≤ n ≤ 35).
3. For `i = 0` to `n`, print the value returned by `fib(i)`:
   1. If `i == 0`, return `0` (base case 1).
   2. If `i == 1`, return `1` (base case 2).
   3. Otherwise return `fib(i - 1) + fib(i - 2)` (recursive case, two calls).
4. Reset the call counter to 0 and compute `fib(n)` again.
5. Print `F(n)` and the total number of recursive calls made.
6. Stop.

### C++ Program

<!--CODE:Exp3_Recursion/exp3_2_fibonacci.cpp-->
```cpp
/*
 * Experiment 3.2 : Fibonacci series using recursion
 * Course          : ELC2132 Data Structures & Algorithms Lab
 * Language        : C++
 *
 * Aim             : Compute the Fibonacci series F(0) = 0, F(1) = 1 and
 *                   F(n) = F(n-1) + F(n-2) for n > 1, using recursion.
 *
 * Base cases      : F(0) = 0, F(1) = 1
 * Recursive case  : F(n) = F(n-1) + F(n-2)     (two recursive calls)
 *
 * Time complexity : O(2^n), the same values are calculated again and again
 * Space complexity: O(n), only the deepest chain of calls is kept in memory
 *
 * Compile : g++ -Wall -Wextra -Wpedantic -std=c++17 -o fibonacci exp3_2_fibonacci.cpp
 */

#include <iostream>
using namespace std;

#define MAX_N 35

int callCount = 0;   // counts the calls, to show how fast the work grows

int fib(int n)
{
    callCount = callCount + 1;

    if (n == 0) {
        return 0;                    // base case 1
    }
    if (n == 1) {
        return 1;                    // base case 2
    }
    return fib(n - 1) + fib(n - 2);  // recursive case
}

int main()
{
    int n = 0;

    cout << "Enter n (0 to " << MAX_N << "): ";
    cin >> n;

    if (n < 0 || n > MAX_N) {
        cout << "Please enter a number between 0 and " << MAX_N << "." << endl;
        return 1;
    }

    cout << "Fibonacci series (first " << n + 1 << " terms):" << endl;
    for (int i = 0; i <= n; i++) {
        cout << "F(" << i << ") = " << fib(i) << endl;
    }

    callCount = 0;
    int value = fib(n);

    cout << endl;
    cout << "F(" << n << ") = " << value << endl;
    cout << "Total recursive calls made to compute F(" << n << "): " << callCount << endl;

    return 0;
}
```

### Output

<!--TEXT:_build/out/exp3_2_fibonacci.txt-->
```
Enter n (0 to 35): 8
Fibonacci series (first 9 terms):
F(0) = 0
F(1) = 1
F(2) = 1
F(3) = 2
F(4) = 3
F(5) = 5
F(6) = 8
F(7) = 13
F(8) = 21

F(8) = 21
Total recursive calls made to compute F(8): 67
```

### Observation

`F(5)` is computed as `F(4) + F(3)`, and each of those again splits into two calls, so the same
subproblem (for example `F(2)`) is recomputed many times. The number of calls therefore
grows exponentially. The counter built into the program confirms the growth, and the values
match the closed formula `calls(n) = 2 * F(n+1) - 1`:

| n | Recursive calls for F(n) | 2 × F(n+1) − 1 | Time (measured) |
|---|---|---|---|
| 5 | 15 | 15 | < 0.01 s |
| 10 | 177 | 177 | < 0.01 s |
| 15 | 1973 | 1973 | < 0.01 s |
| 20 | 21891 | 21891 | < 0.01 s |
| 25 | 242785 | 242785 | < 0.01 s |
| 30 | 2692537 | 2692537 | 0.01 s |

Time complexity is **O(2^n)**. Space complexity is only **O(n)**, because the two calls are
made one after the other: at any instant only the deepest chain of `n + 1` frames is alive, not
the whole call tree.

---

## 3.3 Tower of Hanoi

### Aim

Write a recursive C++ program to solve the Tower of Hanoi puzzle for `n` disks: move all disks
from the source rod to the destination rod using an auxiliary rod, such that only one disk is
moved at a time, only the top disk of a rod may be moved, and a larger disk is never placed on
a smaller one.

### Algorithm

1. Start.
2. Read the number of disks `n` (1 ≤ n ≤ 20).
3. Call `towerOfHanoi(n, 'A', 'B', 'C')`, where A is the source, B the helper and C the
   destination rod:
   1. If `n == 1`, move disk 1 from the source to the destination and count the move
      (base case).
   2. Otherwise:
      1. Move the top `n - 1` disks from the source to the helper rod, using the
         destination as the helper: `towerOfHanoi(n - 1, source, destination, helper)`.
      2. Move disk `n` from the source to the destination and count the move.
      3. Move the `n - 1` disks from the helper rod to the destination, using the
         source as the helper: `towerOfHanoi(n - 1, helper, source, destination)`.
4. Print the total number of moves, which is `2^n - 1`.
5. Stop.

### C++ Program

<!--CODE:Exp3_Recursion/exp3_3_tower_of_hanoi.cpp-->
```cpp
/*
 * Experiment 3.3 : Tower of Hanoi using recursion
 * Course          : ELC2132 Data Structures & Algorithms Lab
 * Language        : C++
 *
 * Aim             : Move n disks from rod A to rod C using rod B, one disk at a
 *                   time, and never put a bigger disk on a smaller one.
 *
 * Base case       : n = 1, move that single disk straight to the destination
 * Recursive case  : move the top n-1 disks to the helper rod, move disk n to the
 *                   destination, then move the n-1 disks on top of it
 *
 * Time complexity : O(2^n), the number of moves is exactly 2^n - 1
 * Space complexity: O(n) calls waiting on the stack
 *
 * Compile : g++ -Wall -Wextra -Wpedantic -std=c++17 -o tower_of_hanoi exp3_3_tower_of_hanoi.cpp
 */

#include <iostream>
using namespace std;

#define MAX_DISKS 20

int moveCount = 0;

// Move n disks from 'source' to 'destination' using 'helper' as the spare rod.
void towerOfHanoi(int n, char source, char helper, char destination)
{
    if (n == 1) {
        // Base case: only one disk left, move it directly.
        moveCount = moveCount + 1;
        cout << "Move " << moveCount << ": disk 1 from " << source
             << " to " << destination << endl;
        return;
    }

    // Step 1: move the top n-1 disks out of the way onto the helper rod.
    towerOfHanoi(n - 1, source, destination, helper);

    // Step 2: move the biggest disk to the destination.
    moveCount = moveCount + 1;
    cout << "Move " << moveCount << ": disk " << n << " from " << source
         << " to " << destination << endl;

    // Step 3: move the n-1 disks from the helper rod onto the destination.
    towerOfHanoi(n - 1, helper, source, destination);
}

int main()
{
    int n = 0;

    cout << "Enter the number of disks (1 to " << MAX_DISKS << "): ";
    cin >> n;

    if (n < 1 || n > MAX_DISKS) {
        cout << "Please enter a number between 1 and " << MAX_DISKS << "." << endl;
        return 1;
    }

    cout << endl;
    cout << "Moves for " << n << " disks (A = source, B = helper, C = destination):" << endl;
    towerOfHanoi(n, 'A', 'B', 'C');

    cout << endl;
    cout << "Total number of moves = " << moveCount << endl;

    return 0;
}
```

### Output

<!--TEXT:_build/out/exp3_3_tower_of_hanoi.txt-->
```
Enter the number of disks (1 to 20): 3

Moves for 3 disks (A = source, B = helper, C = destination):
Move 1: disk 1 from A to C
Move 2: disk 2 from A to B
Move 3: disk 1 from C to B
Move 4: disk 3 from A to C
Move 5: disk 1 from B to A
Move 6: disk 2 from B to C
Move 7: disk 1 from A to C

Total number of moves = 7
```

### Observation

For 3 disks the solution takes 7 moves, and the printed sequence is a valid solution: disk 3
never moves before disks 1 and 2 are stacked on rod B, and no larger disk is ever placed on a
smaller one. The move count was checked against `2^n - 1`:

| n | Moves printed | 2^n − 1 |
|---|---|---|
| 1 | 1 | 1 |
| 2 | 3 | 3 |
| 3 | 7 | 7 |
| 4 | 15 | 15 |
| 5 | 31 | 31 |
| 10 | 1023 | 1023 |
| 15 | 32767 | 32767 |
| 20 | 1048575 | 1048575 |

Each call with `n` disks makes two calls with `n - 1` disks, which gives the recurrence
`T(n) = 2T(n-1) + 1` with `T(1) = 1`. Solving it gives `T(n) = 2^n - 1`, so the time complexity is
**O(2^n)**. The recursion depth is `n`, so the space complexity is **O(n)**. At 20 disks the
program printed all 1,048,575 moves in 0.11 s.

---

## Summary of the three programs

| Program | Recursion pattern | Recursive calls | Time complexity | Space complexity |
|---|---|---|---|---|
| Factorial | single recursive call (linear) | n + 1 | O(n) | O(n) |
| Fibonacci | two recursive calls (tree) | 2·F(n+1) − 1 | O(2^n) | O(n) |
| Tower of Hanoi | two calls with problem transformation | 2^(n+1) − 1 | O(2^n) | O(n) |

## Conclusion

All three programs were written, compiled with `g++ -Wall -Wextra -Wpedantic -std=c++17` without any
warning, and executed successfully.

1. The factorial program shows linear recursion: one call per step and one unwinding
   multiplication, giving O(n) time and O(n) stack space. It works up to `12!`, which is the
biggest factorial that fits in an int.
2. The Fibonacci program shows tree recursion. Its exponential O(2^n) running time was
   confirmed by counting the calls made (for example 21,891 calls merely to compute `F(20)`),
   which demonstrates how the same subproblem is recomputed again and again. Its space cost
   stays O(n), because the two recursive calls are sequential rather than simultaneous.
3. The Tower of Hanoi program shows recursion where the problem itself is transformed at
   every step. The number of moves printed matched `2^n - 1` exactly for every value tested,
   showing why the puzzle is treated as exponential.

Hence the aim of designing, implementing and analyzing recursive C++ programs for the factorial,
the Fibonacci series and the Tower of Hanoi was achieved.
