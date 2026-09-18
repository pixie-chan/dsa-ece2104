# Experiment 2: Sorting using Quick Sort

**Course:** ELC2132 Data Structures & Algorithms Lab  ·  **Language:** C++ (`g++ -std=c++17`)
**Name:** .............................  **Reg. No.:** .............................
**Batch:** ...................  **Date:** ...................

Source: `DAS-LAB/Exp2_Quick_Sort/exp2_quick_sort.cpp`  ·  Manual: `DAS-LAB/Manuals/Exp 2_Student.pdf`

---

## Aim

Write a C++ program that implements quick sort and use it to sort the array
`38 27 43 3 9 82 10`.

---

## Basic understanding

Quick sort also works by divide and conquer, but the division is done by a partition step rather
than by simply cutting the array in half. One element is chosen as the pivot, every element
smaller than the pivot is moved to its left and every larger element to its right, and the pivot
then sits in the position it will occupy in the final sorted array. The two sides of the pivot are
then sorted by the same procedure. The program follows the Lomuto scheme, which takes the last
element of the range as the pivot, keeps a boundary index `i` of the elements known to be smaller
than the pivot, and swaps an element into that region whenever it is smaller than the pivot. The
sorting happens inside the original array, so no temporary array is needed.

---

## Algorithm

**partition(arr, low, high)**

1. Set `pivot = arr[high]`, the last element of the range.
2. Set `i = low - 1`, the boundary of the region holding values smaller than the pivot.
3. For `j = low` to `high - 1`:
   1. If `arr[j] < pivot`, increment `i` and swap `arr[i]` with `arr[j]`.
4. Swap `arr[i + 1]` with `arr[high]`, which places the pivot just after the smaller values.
5. Return `i + 1`, the final index of the pivot.

**quickSort(arr, low, high)**

1. If `low >= high`, the range holds at most one element and is already sorted, so return.
2. Call `partition(arr, low, high)` and store the result in `pi`.
3. Recursively sort the left side: `quickSort(arr, low, pi - 1)`.
4. Recursively sort the right side: `quickSort(arr, pi + 1, high)`.

**main()**

1. Start.
2. Declare the array `arr` with the values `{38, 27, 43, 3, 9, 82, 10}`.
3. Print the original array.
4. Call `quickSort(arr, 0, SIZE - 1)`.
5. Print the sorted array.
6. Stop.

---

## C++ Program

<!--CODE:Exp2_Quick_Sort/exp2_quick_sort.cpp-->
```cpp
/*
 * Experiment 2 : Sorting using quick sort
 * Course       : ELC2132 Data Structures & Algorithms Lab
 * Language     : C++
 *
 * Aim          : Sort the array 38 27 43 3 9 82 10 using quick sort.
 *
 * Idea         : take the last element as the pivot, move every smaller element
 *                to its left, put the pivot in its final position, then sort the
 *                two sides with the same function.
 *
 * Time complexity : O(n log n) on average, O(n^2) in the worst case
 * Space complexity: O(log n) on average for the recursion
 *
 * Compile : g++ -Wall -Wextra -Wpedantic -std=c++17 -o quick_sort exp2_quick_sort.cpp
 */

#include <iostream>
using namespace std;

#define SIZE 7

// Exchange the values of the two variables the pointers point to.
void swapValues(int *a, int *b)
{
    int temp = *a;
    *a = *b;
    *b = temp;
}

// Put the pivot in its final place: smaller values on the left, bigger on the right.
int partition(int arr[], int low, int high)
{
    int pivot = arr[high];
    int i = low - 1;              // last position of the "smaller than pivot" part

    for (int j = low; j < high; j++) {
        if (arr[j] < pivot) {
            i++;
            swapValues(&arr[i], &arr[j]);
        }
    }

    swapValues(&arr[i + 1], &arr[high]);   // place the pivot after the smaller values
    return i + 1;
}

void quickSort(int arr[], int low, int high)
{
    if (low < high) {
        int pi = partition(arr, low, high);

        quickSort(arr, low, pi - 1);    // sort the left side of the pivot
        quickSort(arr, pi + 1, high);   // sort the right side of the pivot
    }
}

int main()
{
    int arr[SIZE] = {38, 27, 43, 3, 9, 82, 10};

    cout << "Original array: ";
    for (int i = 0; i < SIZE; i++) {
        cout << arr[i] << " ";
    }
    cout << endl;

    quickSort(arr, 0, SIZE - 1);

    cout << "Sorted array: ";
    for (int i = 0; i < SIZE; i++) {
        cout << arr[i] << " ";
    }
    cout << endl;

    return 0;
}
```

---

## Results

<!--TEXT:_build/out/exp2_quick_sort.txt-->
```
Original array: 38 27 43 3 9 82 10
Sorted array: 3 9 10 27 38 43 82
```

The array printed by the program after sorting is `3 9 10 27 38 43 82`, which is the ascending
order of the given values and is identical to the result produced by merge sort in Experiment 1.

---

## Observation

Every call to `partition` fixes exactly one element, the pivot, in its final position. Tracing the
calls for the given input gives:

| Call | Range being partitioned | Pivot | Array after the partition | Pivot's final index |
|---|---|---|---|---|
| `quickSort(0, 6)` | `38 27 43 3 9 82 10` | 10 | `3 9 10 38 27 82 43` | 2 |
| `quickSort(0, 1)` | `3 9` | 9 | `3 9` | 1 |
| `quickSort(3, 6)` | `38 27 82 43` | 43 | `38 27 43 82` | 5 |
| `quickSort(3, 4)` | `38 27` | 27 | `27 38` | 3 |

The remaining calls are on ranges of a single element (`3` at index 0, `82` at index 6, `27` at
index 3 and `38` at index 4), which return immediately because `low >= high`. After the four
partitions the array reads `3 9 10 27 38 43 82`.

In the first partition, for example, the pivot is 10 and the scan moves `3` and `9` into the
smaller-than-pivot region, so the swap at the end puts 10 at index 2 with everything smaller on
its left and everything larger on its right. That single step already places 10 correctly and
never touches it again, which is why quick sort needs no merge pass at all.

The cost of one partition is a single scan of the range, so it is O(n) at each level, and the
number of levels depends on how evenly the pivot splits the data. When the pivot falls near the
middle, there are about `log n` levels and the total is **O(n log n)**; when it is always the
smallest or the largest element, the split is maximally uneven, the depth becomes `n`, and the
total degrades to **O(n^2)**. The array used here is not sorted, so the pivots 10, 9, 43 and 27
gave reasonably even splits. The recursion depth is `O(log n)` on average and `O(n)` in the worst
case, and since the algorithm sorts in place, no extra array is required.

A comparison of the two sorts implemented in Experiments 1 and 2:

| Property | Merge sort (Experiment 1) | Quick sort (Experiment 2) |
|---|---|---|
| Best and average time | O(n log n) | O(n log n) |
| Worst-case time | O(n log n) | O(n^2) |
| Extra space | O(n) for the temporary arrays | O(log n) recursion stack on average |
| In place | no | yes |
| Stable | yes | no |
| Comments | predictable, good for linked lists | usually faster in practice, needs a good pivot choice |

---

## Conclusion

Quick sort was implemented in C++ and used on the array `38 27 43 3 9 82 10`, producing
`3 9 10 27 38 43 82`, exactly the same result that merge sort produced in Experiment 1, which
confirms that both algorithms are correct.

The working of the algorithm is visible in the partition trace: each partition scans its range
once, moves every element smaller than the pivot to the left of the boundary and leaves the pivot
in its final position, so the pivot never has to be moved again. Sorting then continues only on
the two remaining sides.

The average time complexity is O(n log n), which makes quick sort competitive with merge sort,
but the worst case is O(n^2) and occurs when the pivot is always an extreme value, for example
when an already sorted array is sorted again with the last element chosen as the pivot. The
advantage over merge sort is that quick sort works in place and needs no extra array, only the
recursion stack. Hence the aim of implementing quick sort and sorting the given array with it was
achieved.
