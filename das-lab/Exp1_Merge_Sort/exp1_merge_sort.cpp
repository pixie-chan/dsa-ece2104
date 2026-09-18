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
