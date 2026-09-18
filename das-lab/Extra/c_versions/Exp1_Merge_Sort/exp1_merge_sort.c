/*
 * Experiment 1 : Sorting using merge sort
 * Course       : ELC2132 Data Structures & Algorithms Lab
 *
 * Aim          : Sort the array 38 27 43 3 9 82 10 using merge sort.
 *
 * Idea         : divide the array into two halves, sort each half recursively,
 *                then merge the two sorted halves with one linear merge pass.
 *
 * Time complexity : O(n log n) in the best, average and worst case
 * Space complexity: O(n) for the temporary left and right arrays
 *
 * Compile : gcc -Wall -Wextra -std=c11 -o merge_sort exp1_merge_sort.c
 */

#include <stdio.h>
#define SIZE 7

void merge(int arr[], int left, int mid, int right)
{
    int i, j, k;
    int n1 = mid - left + 1;
    int n2 = right - mid;
    int L[n1], R[n2];

    for (i = 0; i < n1; ++i) {
        L[i] = arr[left + i];
    }
    for (j = 0; j < n2; ++j) {
        R[j] = arr[mid + 1 + j];
    }

    i = 0;
    j = 0;
    k = left;

    while (i < n1 && j < n2) {
        if (L[i] <= R[j]) {
            arr[k] = L[i];
            ++i;
        } else {
            arr[k] = R[j];
            ++j;
        }
        ++k;
    }

    while (i < n1) {
        arr[k] = L[i];
        ++i;
        ++k;
    }

    while (j < n2) {
        arr[k] = R[j];
        ++j;
        ++k;
    }
}

void mergeSort(int arr[], int left, int right)
{
    if (left < right) {
        int mid = left + (right - left) / 2;
        mergeSort(arr, left, mid);
        mergeSort(arr, mid + 1, right);
        merge(arr, left, mid, right);
    }
}

int main(void)
{
    int arr[SIZE] = {38, 27, 43, 3, 9, 82, 10};

    printf("Original array: ");
    for (int i = 0; i < SIZE; ++i) {
        printf("%d ", arr[i]);
    }
    printf("\n");

    mergeSort(arr, 0, SIZE - 1);

    printf("Sorted array: ");
    for (int i = 0; i < SIZE; ++i) {
        printf("%d ", arr[i]);
    }
    printf("\n");

    return 0;
}
