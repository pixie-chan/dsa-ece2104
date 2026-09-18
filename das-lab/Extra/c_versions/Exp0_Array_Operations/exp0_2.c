/*
 * Experiment 0, Part 2 (a) : Insert an integer into a 1-D array
 * Course                   : ELC2132 Data Structures & Algorithms Lab
 *
 * Aim                      : Insert a new value at a chosen index of a 1-D array
 *                            by shifting every element from that index onwards
 *                            one position to the right.
 *
 * Idea                     : the array is given spare capacity, so the number of
 *                            elements in use (`size`) grows with every insertion
 *                            and no existing element is destroyed.
 *
 * Time complexity          : O(n) for the shifting loop
 * Space complexity         : O(1) extra
 *
 * Compile : gcc -Wall -Wextra -std=c11 -o exp0_2 exp0_2.c
 */

#include <stdio.h>

#define CAPACITY 10   /* how many elements the array can hold */

int main(void)
{
    int arr[CAPACITY] = {10, 20, 30};
    int size = 3;                  /* elements currently in use */
    int index, value, i;

    printf("Array before insertion (%d elements):\n", size);
    for (i = 0; i < size; ++i) {
        printf("arr[%d] = %d\n", i, arr[i]);
    }

    printf("Enter the index where you want to insert (0 to %d): ", size);
    if (scanf("%d", &index) != 1) {
        printf("Invalid input: expected an integer index.\n");
        return 1;
    }
    if (index < 0 || index > size) {
        printf("Invalid index! Please enter a valid index.\n");
        return 1;
    }
    if (size == CAPACITY) {
        printf("Array is full: cannot insert another element.\n");
        return 1;
    }

    printf("Enter the new value to insert: ");
    if (scanf("%d", &value) != 1) {
        printf("Invalid input: expected an integer value.\n");
        return 1;
    }

    /* Shift every element from `index` onwards one position to the right. */
    for (i = size; i > index; --i) {
        arr[i] = arr[i - 1];
    }

    /* Place the new value in the gap that the shift created. */
    arr[index] = value;
    size++;

    printf("Array after insertion (%d elements):\n", size);
    for (i = 0; i < size; ++i) {
        printf("arr[%d] = %d\n", i, arr[i]);
    }

    return 0;
}
