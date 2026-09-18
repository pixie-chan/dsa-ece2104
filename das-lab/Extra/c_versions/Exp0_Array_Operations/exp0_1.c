/*
 * Experiment 0, Part 1 : 1-D array operations
 * Course               : ELC2132 Data Structures & Algorithms Lab
 *
 * Aim                  : Demonstrate (a) initialization, (b) finding the sum,
 *                        (c) finding the maximum element and (d) reversing a
 *                        1-D array of SIZE elements.
 *
 * Time complexity      : O(n) for each of the four operations
 * Space complexity     : O(1) extra (the reversal swaps elements in place)
 *
 * Compile : gcc -Wall -Wextra -std=c11 -o exp0_1 exp0_1.c
 */

#include <stdio.h>
#define SIZE 3

int main(void)
{
    int arr[SIZE] = {10, 20, 30};
    int sum = 0;
    int max = arr[0];
    int temp;

    printf("Elements of the array:\n");
    for (int i = 0; i < SIZE; ++i) {
        printf("arr[%d] = %d\n", i, arr[i]);
    }

    for (int i = 0; i < SIZE; ++i) {
        sum += arr[i];
    }
    printf("Sum of elements in the array: %d\n", sum);

    for (int i = 1; i < SIZE; ++i) {
        if (arr[i] > max) {
            max = arr[i];
        }
    }
    printf("Maximum element in the array: %d\n", max);

    for (int i = 0; i < SIZE / 2; ++i) {
        temp = arr[i];
        arr[i] = arr[SIZE - 1 - i];
        arr[SIZE - 1 - i] = temp;
    }

    printf("Reversed array:\n");
    for (int i = 0; i < SIZE; ++i) {
        printf("%d ", arr[i]);
    }
    printf("\n");

    return 0;
}
