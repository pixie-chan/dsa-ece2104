# Experiment 1: Sorting using Merge Sort

**Course:** ELC2132 Data Structures & Algorithms Lab  ·  **Language:** C++ (`g++ -std=c++17`)
**Name:** .............................  **Reg. No.:** .............................
**Batch:** ...................  **Date:** ...................

Source: `DAS-LAB/Exp1_Merge_Sort/exp1_merge_sort.cpp`  ·  Manual: `DAS-LAB/Manuals/Exp 1_Student.pdf`

---

## Aim

Write a C++ program that implements merge sort and use it to sort the array
`38 27 43 3 9 82 10`.

---

## Basic understanding

Merge sort follows the divide and conquer strategy. The array is divided into two halves, each
half is sorted by the same procedure, and the two sorted halves are then combined by a merge step
that repeatedly takes the smaller of the two front elements. The division stops when a subarray
holds a single element, because an array of one element is already sorted. Unlike the sorts that
swap elements inside one array, merge sort needs a temporary array during the merge step, and it
is stable: elements that compare equal keep their original relative order, which is why the merge
step compares with `<=` and takes from the left half first.

---

## Algorithm

**mergeSort(arr, left, right)**

1. If `left >= right`, the subarray holds at most one element and is already sorted, so return.
2. Compute the middle index: `mid = left + (right - left) / 2`.
3. Recursively sort the left half: `mergeSort(arr, left, mid)`.
4. Recursively sort the right half: `mergeSort(arr, mid + 1, right)`.
5. Merge the two sorted halves: `merge(arr, left, mid, right)`.

**merge(arr, left, mid, right)**

1. Compute the sizes `n1 = mid - left + 1` and `n2 = right - mid`.
2. Copy `arr[left ... mid]` into a temporary array `L` and `arr[mid + 1 ... right]` into `R`.
3. Set `i = 0`, `j = 0`, `k = left`.
4. While `i < n1` and `j < n2`: if `L[i] <= R[j]`, copy `L[i]` into `arr[k]` and increment `i`;
   otherwise copy `R[j]` into `arr[k]` and increment `j`. Increment `k` after every copy.
5. Copy any elements left over in `L` into `arr`, then any elements left over in `R`.
6. Return.

**main()**

1. Start.
2. Declare the array `arr` with the values `{38, 27, 43, 3, 9, 82, 10}`.
3. Print the original array.
4. Call `mergeSort(arr, 0, SIZE - 1)`.
5. Print the sorted array.
6. Stop.

---

## C++ Program

<!--CODE:Exp1_Merge_Sort/exp1_merge_sort.cpp-->
```cpp
/*
 * Experiment 1 : Sorting using merge sort
 * Course       : ELC2132 Data Structures & Algorithms Lab
 * Language     : C++
 *
 * Aim          : Sort the array 38 27 43 3 9 82 10 using merge sort.
 *
 * Idea         : divide the array into two halves, sort each half with the same
 *                function, then merge the two sorted halves into one.
 *
 * Time complexity : O(n log n) in the best, average and worst case
 * Space complexity: O(n) for the two temporary arrays L and R
 *
 * Compile : g++ -Wall -Wextra -Wpedantic -std=c++17 -o merge_sort exp1_merge_sort.cpp
 */

#include <iostream>
using namespace std;

#define SIZE 7

// Merge the two sorted halves arr[left..mid] and arr[mid+1..right].
void merge(int arr[], int left, int mid, int right)
{
    int L[SIZE], R[SIZE];      // SIZE is enough, each half is smaller than the whole
    int n1 = mid - left + 1;
    int n2 = right - mid;

    // Copy both halves into the temporary arrays.
    for (int i = 0; i < n1; i++) {
        L[i] = arr[left + i];
    }
    for (int j = 0; j < n2; j++) {
        R[j] = arr[mid + 1 + j];
    }

    int i = 0;
    int j = 0;
    int k = left;

    // Take the smaller element from the front of L or R, until one runs out.
    while (i < n1 && j < n2) {
        if (L[i] <= R[j]) {
            arr[k] = L[i];
            i++;
        } else {
            arr[k] = R[j];
            j++;
        }
        k++;
    }

    // Copy whatever is left over.
    while (i < n1) {
        arr[k] = L[i];
        i++;
        k++;
    }
    while (j < n2) {
        arr[k] = R[j];
        j++;
        k++;
    }
}

void mergeSort(int arr[], int left, int right)
{
    if (left < right) {
        int mid = left + (right - left) / 2;

        mergeSort(arr, left, mid);        // sort the left half
        mergeSort(arr, mid + 1, right);   // sort the right half
        merge(arr, left, mid, right);     // merge the two sorted halves
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

    mergeSort(arr, 0, SIZE - 1);

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

<!--TEXT:_build/out/exp1_merge_sort.txt-->
```
Original array: 38 27 43 3 9 82 10
Sorted array: 3 9 10 27 38 43 82
```

The array printed by the program after sorting is `3 9 10 27 38 43 82`, which is the ascending
order of the given values.

---

## Observation

The recursion divides the array down to single elements and then merges them back in sorted
order. Recording the call sequence for the given input gives the following trace:

| Step | Subarray being processed | Action | Result |
|---|---|---|---|
| 1 | `38 27 43 3 9 82 10` | split at `mid = 3` | `38 27 43 3` and `9 82 10` |
| 2 | `38 27 43 3` | split at `mid = 1` | `38 27` and `43 3` |
| 3 | `38 27` | split at `mid = 0`, then merge | `27 38` |
| 4 | `43 3` | split at `mid = 2`, then merge | `3 43` |
| 5 | `27 38` and `3 43` | merge | `3 27 38 43` |
| 6 | `9 82 10` | split at `mid = 5` | `9 82` and `10` |
| 7 | `9 82` | split at `mid = 4`, then merge | `9 82` |
| 8 | `9 82` and `10` | merge | `9 10 82` |
| 9 | `3 27 38 43` and `9 10 82` | final merge | `3 9 10 27 38 43 82` |

The trace shows why the total work is `n log n`: the array is halved about `log2(n)` times, which
is 3 levels for 7 elements, and every level performs one merge pass that touches each element
once. The recurrence is

```
T(n) = 2 T(n/2) + O(n)      with T(1) = O(1)
```

which solves to **O(n log n)**. Unlike quick sort, this bound holds in the best, average and
worst case, and the input order does not matter: a reversed array and a sorted array are both
split the same way. The cost is the temporary `L` and `R` arrays, which together hold `n`
elements, so the space complexity is **O(n)** in addition to the `O(log n)` recursion depth.

---

## Conclusion

Merge sort was implemented in C++ and used to sort the array `38 27 43 3 9 82 10`, producing
`3 9 10 27 38 43 82` as required. The program divides the array into halves until each part holds
a single element, and then merges the parts back in order, which the trace of the nine steps above
makes visible.

The running time is O(n log n) in every case, because the division depth is log n and each level
does one linear merge pass, which makes merge sort more predictable than quick sort, whose worst
case is O(n^2). The price is the extra O(n) memory for the temporary arrays, and the fact that the
sort is not in place. Because the merge step takes from the left half whenever the two front
values are equal, the algorithm is stable, which matters when records are sorted by one field and
then by another. Hence the aim of implementing merge sort was achieved.
