# Experiment 0: Array Operations

**Course:** ELC2132 Data Structures & Algorithms Lab  ·  **Language:** C++ (`g++ -std=c++17`)
**Name:** .............................  **Reg. No.:** .............................
**Batch:** ...................  **Date:** ...................

Manuals: `DAS-LAB/Manuals/Exp 0_Array Operations 1.pdf`, `Exp 0_Array Operations 2.pdf`,
`Exp 0_Array Operations Pointers.pdf`

This experiment has three parts:

| Part | Topic | Source |
|---|---|---|
| 1 | 1-D array operations: initialization, sum, maximum, reversal | `Exp0_Array_Operations/exp0_1.cpp` |
| 2 | Array operations: (a) insertion, (b) linear search | `Exp0_Array_Operations/exp0_2.cpp`, `exp0_2_linear_search.cpp` |
| 3 | 2-D array operations using pointer notation | `Exp0_Array_Operations/exp0_3.cpp` |

---

## Part 1: 1-D array operations

### Aim

Write a C++ program that demonstrates the following 1-D array operations on an array of SIZE
elements:

* (a) **Initialization**: declare the array and fill it with values
* (b) **Finding the sum**: add every element of the array
* (c) **Finding the maximum element**: report the largest value stored
* (d) **Reversing the array**: turn the order of the elements around

### Algorithm

1. Start.
2. Declare an integer array `arr[SIZE]` and initialize it with the values `{10, 20, 30}` (part a).
3. Print every element with its index using a loop from `i = 0` to `SIZE - 1` (part a).
4. Set `sum = 0`; for `i = 0` to `SIZE - 1`, add `arr[i]` to `sum`; print `sum` (part b).
5. Set `max = arr[0]`; for `i = 1` to `SIZE - 1`, if `arr[i] > max` then set `max = arr[i]`;
   print `max` (part c).
6. For `i = 0` to `SIZE / 2 - 1`, swap `arr[i]` with `arr[SIZE - 1 - i]` (part d).
7. Print the reversed array (part d).
8. Stop.

### C++ Program

<!--CODE:Exp0_Array_Operations/exp0_1.cpp-->
```cpp
/*
 * Experiment 0, Part 1 : 1-D array operations
 * Course               : ELC2132 Data Structures & Algorithms Lab
 * Language             : C++
 *
 * Aim                  : Demonstrate (a) initialization, (b) finding the sum,
 *                        (c) finding the maximum element and (d) reversing a
 *                        1-D array of SIZE elements.
 *
 * Compile : g++ -Wall -Wextra -Wpedantic -std=c++17 -o exp0_1 exp0_1.cpp
 */

#include <iostream>
using namespace std;

#define SIZE 3

int main()
{
    int arr[SIZE] = {10, 20, 30};
    int sum = 0;
    int max = arr[0];
    int temp;

    // (a) Initialization: print the array with its index numbers.
    cout << "Elements of the array:" << endl;
    for (int i = 0; i < SIZE; i++) {
        cout << "arr[" << i << "] = " << arr[i] << endl;
    }

    // (b) Add every element to get the sum.
    for (int i = 0; i < SIZE; i++) {
        sum = sum + arr[i];
    }
    cout << "Sum of elements in the array: " << sum << endl;

    // (c) Compare every element with max and keep the biggest one.
    for (int i = 1; i < SIZE; i++) {
        if (arr[i] > max) {
            max = arr[i];
        }
    }
    cout << "Maximum element in the array: " << max << endl;

    // (d) Reverse the array by swapping the two ends.
    for (int i = 0; i < SIZE / 2; i++) {
        temp = arr[i];
        arr[i] = arr[SIZE - 1 - i];
        arr[SIZE - 1 - i] = temp;
    }

    cout << "Reversed array:" << endl;
    for (int i = 0; i < SIZE; i++) {
        cout << arr[i] << " ";
    }
    cout << endl;

    return 0;
}
```

### Results

<!--TEXT:_build/out/exp0_1.txt-->
```
Elements of the array:
arr[0] = 10
arr[1] = 20
arr[2] = 30
Sum of elements in the array: 60
Maximum element in the array: 30
Reversed array:
30 20 10
```

### Observation

The four operations run one after another on the same array, and each one is visible in the
output above:

| Part | Operation | Result printed | Loop used |
|---|---|---|---|
| (a) | Initialization and printing | `arr[0] = 10`, `arr[1] = 20`, `arr[2] = 30` | `for i = 0` to `SIZE - 1` |
| (b) | Sum of the elements | `Sum of elements in the array: 60` | `for i = 0` to `SIZE - 1`, `sum += arr[i]` |
| (c) | Maximum element | `Maximum element in the array: 30` | `for i = 1` to `SIZE - 1`, compare with `max` |
| (d) | Reversal | `30 20 10` | `for i = 0` to `SIZE / 2 - 1`, swap with `arr[SIZE - 1 - i]` |

The sum was verified by hand: 10 + 20 + 30 = 60. The maximum loop starts at `i = 1` because
`max` is already initialized with `arr[0]`, so the first element is never compared with itself.
The reversal loop runs only up to `SIZE / 2` because each iteration fixes two elements at once,
one from each end, and swapping in place needs no second array. All four operations are single
passes over the array, so each one is **O(n)** in time and **O(1)** in extra space.

---

## Part 2 (a): Insert an integer in the array

### Aim

Write a C++ program to insert an integer value at a chosen index of a 1-D array, shifting the
elements that follow it one position to the right.

### Algorithm

1. Start.
2. Declare an array `arr` with capacity 10, store the values `{10, 20, 30}` in it and set
   `size = 3`, the number of elements actually in use.
3. Print the array before the insertion.
4. Read the index at which the value is to be inserted.
5. If the index is less than 0 or greater than `size`, print an error message and stop.
6. If `size` has already reached the capacity, report that the array is full and stop.
7. Read the value to be inserted.
8. For `i = size` down to `index + 1`, copy `arr[i - 1]` into `arr[i]` (shift right).
9. Store the new value at `arr[index]` and increase `size` by 1.
10. Print the array after the insertion.
11. Stop.

### C++ Program

<!--CODE:Exp0_Array_Operations/exp0_2.cpp-->
```cpp
/*
 * Experiment 0, Part 2 (a) : Insert an integer into a 1-D array
 * Course                   : ELC2132 Data Structures & Algorithms Lab
 * Language                 : C++
 *
 * Aim                      : Insert a new value at a chosen index of a 1-D array
 *                            by shifting every element from that index onwards
 *                            one position to the right.
 *
 * Idea                     : the array has spare capacity, so the number of
 *                            elements in use (size) grows with every insertion
 *                            and no existing element is lost.
 *
 * Compile : g++ -Wall -Wextra -Wpedantic -std=c++17 -o exp0_2 exp0_2.cpp
 */

#include <iostream>
using namespace std;

#define CAPACITY 10   // how many elements the array can hold

int main()
{
    int arr[CAPACITY] = {10, 20, 30};
    int size = 3;        // elements currently in use
    int index = 0;
    int value = 0;

    cout << "Array before insertion (" << size << " elements):" << endl;
    for (int i = 0; i < size; i++) {
        cout << "arr[" << i << "] = " << arr[i] << endl;
    }

    cout << "Enter the index where you want to insert (0 to " << size << "): ";
    cin >> index;

    if (index < 0 || index > size) {
        cout << "Invalid index! Please enter a valid index." << endl;
        return 1;
    }
    if (size == CAPACITY) {
        cout << "Array is full, cannot insert another element." << endl;
        return 1;
    }

    cout << "Enter the new value to insert: ";
    cin >> value;

    // Shift every element from index onwards one place to the right.
    for (int i = size; i > index; i--) {
        arr[i] = arr[i - 1];
    }

    // Put the new value into the empty place that the shifting created.
    arr[index] = value;
    size = size + 1;

    cout << "Array after insertion (" << size << " elements):" << endl;
    for (int i = 0; i < size; i++) {
        cout << "arr[" << i << "] = " << arr[i] << endl;
    }

    return 0;
}
```

### Results

Insertion of the value 99 at index 1:

<!--TEXT:_build/out/exp0_2_insert.txt-->
```
Array before insertion (3 elements):
arr[0] = 10
arr[1] = 20
arr[2] = 30
Enter the index where you want to insert (0 to 3): 1
Enter the new value to insert: 99
Array after insertion (4 elements):
arr[0] = 10
arr[1] = 99
arr[2] = 20
arr[3] = 30
```

Guard case, an index outside the valid range (9 instead of 0 to 3):

<!--TEXT:_build/out/exp0_2_insert_bad_index.txt-->
```
Array before insertion (3 elements):
arr[0] = 10
arr[1] = 20
arr[2] = 30
Enter the index where you want to insert (0 to 3): 9
Invalid index! Please enter a valid index.
```

### Observation

Before the insertion the array held three elements in use, and after inserting 99 at index 1 it
holds four, so the shift moved `20` to index 2 and `30` to index 3 and no value was lost. The
two other positions were checked as well: inserting at index 0 puts the new value first and
pushes all three existing elements right, inserting at index 3 (the current `size`) appends the
value at the end without any shifting at all, and a negative index or an index beyond `size` is
rejected with an error message before anything is modified.

The shifting loop is the whole cost of the operation. In the worst case, which is an insertion at
index 0, all `n` elements move one place, so insertion is **O(n)** in time and needs no extra
array, which is **O(1)** extra space.

---

## Part 2 (b): Search an element using linear search

### Aim

Write a C++ program to search for a value in an array by comparing it with every element in turn,
from the first element to the last, and to report the position at which it is found.

### Algorithm

1. Start.
2. Declare an array `a` of `N = 10` integers.
3. Read `N` values into the array.
4. Read the element to be searched (the key).
5. Set `position = -1` and `comparisons = 0`.
6. For `i = 0` to `N - 1`:
   1. Increase `comparisons` by 1.
   2. If `a[i]` is equal to the key, set `position = i` and leave the loop.
7. If `position` is still -1, print `Element Not Found`; otherwise print the position of the
   element (1-based) and its 0-based index.
8. Print the number of comparisons that were made.
9. Stop.

### C++ Program

<!--CODE:Exp0_Array_Operations/exp0_2_linear_search.cpp-->
```cpp
/*
 * Experiment 0, Part 2 (b) : Search an element using linear search
 * Course                   : ELC2132 Data Structures & Algorithms Lab
 * Language                 : C++
 *
 * Aim                      : Search for a value in an array by comparing it with
 *                            every element from the first to the last, and report
 *                            the position where it is found.
 *
 * Idea                     : the loop stops as soon as a match is found, so the
 *                            number of comparisons is the position of the element
 *                            in the best case and N when it is not present.
 *
 * Compile : g++ -Wall -Wextra -Wpedantic -std=c++17 -o exp0_2_linear_search exp0_2_linear_search.cpp
 */

#include <iostream>
using namespace std;

#define N 10

int main()
{
    int a[N];
    int item = 0;
    int position = -1;    // -1 means "not found"
    int comparisons = 0;

    cout << "Enter " << N << " elements of the array:" << endl;
    for (int i = 0; i < N; i++) {
        cin >> a[i];
    }

    cout << "Enter the element to be searched: ";
    cin >> item;

    // Compare the item with every element until it matches.
    for (int i = 0; i < N; i++) {
        comparisons = comparisons + 1;
        if (a[i] == item) {
            position = i;
            break;
        }
    }

    if (position == -1) {
        cout << "Element Not Found (after " << comparisons << " comparisons)" << endl;
    } else {
        cout << "Element Found at Position = " << position + 1
             << " (0-based index " << position
             << ", after " << comparisons << " comparison(s))" << endl;
    }

    return 0;
}
```

### Results

The key 19 is present in the array (it is the seventh value entered):

<!--TEXT:_build/out/exp0_2b_search_found.txt-->
```
Enter 10 elements of the array:
5 12 7 23 45 3 19 8 30 11
Enter the element to be searched: 19
Element Found at Position = 7 (0-based index 6, after 7 comparison(s))
```

The key 100 is not present, so the loop has to examine the whole array:

<!--TEXT:_build/out/exp0_2b_search_notfound.txt-->
```
Enter 10 elements of the array:
5 12 7 23 45 3 19 8 30 11
Enter the element to be searched: 100
Element Not Found (after 10 comparisons)
```

### Observation

The element 19 was found at position 7 after 7 comparisons, which is exactly its distance from
the start of the array, and the element 100 was not found, which forced all 10 comparisons.
Two more cases were checked to complete the picture:

| Key | Position reported | Comparisons | Why |
|---|---|---|---|
| 5 (first element) | 1 | 1 | best case, the first comparison succeeds |
| 19 (seventh element) | 7 | 7 | the loop stops as soon as it matches |
| 11 (last element) | 10 | 10 | the match is found only at the last element |
| 100 (absent) | not found | 10 | the loop must examine every element |

The number of comparisons grows in step with the position of the element, so the best case is
**O(1)** and the average and worst cases are **O(n)**. Linear search needs no sorted input and
no extra memory, which is its advantage over binary search, but on a large array it is much
slower.

One portability note: the listing printed in the manual uses `#include <conio.h>`, `clrscr()`,
`getch()` and `void main()`, which belong to the old Turbo C compiler on DOS and do not exist on
Linux. A portable equivalent was written above with `#include <iostream>`, `int main()` and `cin`,
so that the program compiles with any modern C++ compiler.

---

## Part 3: 2-D array operations using pointer notation

### Aim

Write a C++ program that demonstrates 2-D array operations using pointer notation: print a 3x3
array, then multiply each of its elements by 2, reaching every element through pointer
arithmetic rather than through subscript notation.

### Algorithm

1. Start.
2. Declare a 2-D array `arr[ROWS][COLS]` and initialize it with the values 1 to 9 arranged in
   three rows.
3. Call `printArray(arr, ROWS, COLS)`:
   1. For every row `i`, for every column `j`, print `*((*(arr + i)) + j)`.
4. Call `multiplyByTwo(arr, ROWS, COLS)`:
   1. For every row `i`, for every column `j`, multiply `*((*(arr + i)) + j)` by 2 and print the
      new value.
5. Stop.

### C++ Program

<!--CODE:Exp0_Array_Operations/exp0_3.cpp-->
```cpp
/*
 * Experiment 0, Pointers : 2-D array operations using pointer notation
 * Course                 : ELC2132 Data Structures & Algorithms Lab
 * Language               : C++
 *
 * Aim                    : Print a 3x3 array and then multiply every element by
 *                          2, reaching every element only through pointer
 *                          arithmetic: *((*(arr + i)) + j)
 *
 * Compile : g++ -Wall -Wextra -Wpedantic -std=c++17 -o exp0_3 exp0_3.cpp
 */

#include <iostream>
using namespace std;

#define ROWS 3
#define COLS 3

// arr is a pointer to a row of the 2-D array, so the function knows the row
// length and the pointer arithmetic below lands on the right element.
void printArray(int (*arr)[COLS], int rows, int cols)
{
    cout << "Array elements:" << endl;
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            cout << *((*(arr + i)) + j) << " ";
        }
        cout << endl;
    }
}

void multiplyByTwo(int (*arr)[COLS], int rows, int cols)
{
    cout << "Array elements multiplied by 2:" << endl;
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            *((*(arr + i)) + j) = *((*(arr + i)) + j) * 2;
            cout << *((*(arr + i)) + j) << " ";
        }
        cout << endl;
    }
}

int main()
{
    int arr[ROWS][COLS] = { {1, 2, 3}, {4, 5, 6}, {7, 8, 9} };

    printArray(arr, ROWS, COLS);
    multiplyByTwo(arr, ROWS, COLS);

    return 0;
}
```

### Results

<!--TEXT:_build/out/exp0_3.txt-->
```
Array elements:
1 2 3
4 5 6
7 8 9
Array elements multiplied by 2:
2 4 6
8 10 12
14 16 18
```

### Observation

The array is printed twice: first as stored, and then with every element doubled. The doubled
values are correct for all nine positions (1 becomes 2, 5 becomes 10, 9 becomes 18), which shows
that the pointer expression reaches every element of the 2-D array and can also modify it.

The expression `*((*(arr + i)) + j)` works because a 2-D array is an array of rows: `arr` decays
to a pointer to the first row, `arr + i` steps to row `i`, `*(arr + i)` is that row (which itself
decays to a pointer to its first element), and adding `j` and dereferencing gives the element in
column `j` of that row. The function parameter `int (*arr)[COLS]` is a pointer to an array of
`COLS` integers, which is why the function knows the row length and pointer arithmetic lands on
the correct element.

Both functions visit every element once, so the time complexity is **O(rows × cols)**, which is
**O(9)** for the 3x3 array used here, and the array is modified in place, so no extra memory is
needed.

---

## Conclusion

All the array operations asked for in Experiment 0 were written in C++, compiled with
`g++ -Wall -Wextra -Wpedantic -std=c++17` with no warnings, and executed successfully.

1. A 1-D array can be initialized at declaration, and the three operations requested on it
   (sum, maximum, reversal) each need only a single pass over the elements, which is why all of
   them are O(n) in time and O(1) in extra space.
2. Inserting into an array is not free: because the elements live in consecutive memory
   locations, everything after the insertion point has to be shifted one place to the right,
   which costs O(n) time. The program keeps a separate `size` counter and spare capacity so that
   an insertion grows the array instead of destroying the last element.
3. Linear search compares the key with the elements one by one and stops at the first match, so
   its cost depends on where the element sits: 1 comparison for the first element, 7 for the
   seventh, and 10 when the element is absent.
4. A 2-D array can be handled entirely through pointer notation. The expression
   `*((*(arr + i)) + j)` moves one row at a time and then one column at a time, and pointer
   arithmetic can both read and modify the elements, which shows how arrays and pointers are
   related in C.

Hence the aim of implementing 1-D array operations, insertion, linear search and 2-D array
operations with pointer notation was achieved.

---

### Files

| File | Description |
|---|---|
| `Exp0_Array_Operations/exp0_1.cpp` | Part 1: initialization, sum, maximum and reversal of a 1-D array |
| `Exp0_Array_Operations/exp0_2.cpp` | Part 2 (a): insertion at a given index by shifting right |
| `Exp0_Array_Operations/exp0_2_linear_search.cpp` | Part 2 (b): linear search with a comparison count |
| `Exp0_Array_Operations/exp0_3.cpp` | Part 3: 2-D array printing and doubling with pointer notation |
| `_build/out/exp0_*.txt` | The transcripts shown above |
