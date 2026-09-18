/*
 * Experiment 2 : Sorting using quick sort
 * Course       : ELC2132 Data Structures & Algorithms Lab
 *
 * Aim          : Sort the array 38 27 43 3 9 82 10 using quick sort.
 *
 * Idea         : take the last element as the pivot (Lomuto partition), move
 *                every smaller element to its left, drop the pivot into its
 *                final position, then sort both partitions recursively.
 *
 * Time complexity : O(n log n) average, O(n^2) worst case for a sorted input
 * Space complexity: O(log n) average recursion depth, O(n) worst case
 *
 * Compile : gcc -Wall -Wextra -std=c11 -o quick_sort exp2_quick_sort.c
 */

#include <stdio.h>
#define SIZE 7

void swap(int *a, int *b)
{
    int temp = *a;
    *a = *b;
    *b = temp;
}

int partition(int arr[], int low, int high)
{
    int pivot = arr[high];
    int i = low - 1;

    for (int j = low; j < high; ++j) {
        if (arr[j] < pivot) {
            ++i;
            swap(&arr[i], &arr[j]);
        }
    }
    swap(&arr[i + 1], &arr[high]);
    return i + 1;
}

void quickSort(int arr[], int low, int high)
{
    if (low < high) {
        int pi = partition(arr, low, high);
        quickSort(arr, low, pi - 1);
        quickSort(arr, pi + 1, high);
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

    quickSort(arr, 0, SIZE - 1);

    printf("Sorted array: ");
    for (int i = 0; i < SIZE; ++i) {
        printf("%d ", arr[i]);
    }
    printf("\n");

    return 0;
}
