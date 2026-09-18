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
