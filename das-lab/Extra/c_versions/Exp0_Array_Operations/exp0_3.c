/*
 * Experiment 0, Pointers : 2-D array operations using pointer notation
 * Course                 : ELC2132 Data Structures & Algorithms Lab
 *
 * Aim                    : Print a 3x3 array and then multiply every element by
 *                          2, reaching every element only through pointer
 *                          arithmetic: *((*(arr + i)) + j)
 *
 * Time complexity        : O(rows * cols)
 * Space complexity       : O(1) extra (the array is modified in place)
 *
 * Compile : gcc -Wall -Wextra -std=c11 -o exp0_3 exp0_3.c
 */

#include <stdio.h>
#define ROWS 3
#define COLS 3

void printArray(int (*arr)[COLS], int rows, int cols)
{
    printf("Array elements:\n");
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            printf("%d ", *((*(arr + i)) + j));
        }
        printf("\n");
    }
}

void multiplyByTwo(int (*arr)[COLS], int rows, int cols)
{
    printf("Array elements multiplied by 2:\n");
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            *((*(arr + i)) + j) *= 2;
            printf("%d ", *((*(arr + i)) + j));
        }
        printf("\n");
    }
}

int main(void)
{
    int arr[ROWS][COLS] = { {1, 2, 3}, {4, 5, 6}, {7, 8, 9} };

    printArray(arr, ROWS, COLS);
    multiplyByTwo(arr, ROWS, COLS);

    return 0;
}
