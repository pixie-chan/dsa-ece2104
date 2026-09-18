/*
 * Experiment 0, Part 2 (b) : Search an element using linear search
 * Course                   : ELC2132 Data Structures & Algorithms Lab
 *
 * Aim                      : Search for a value in a 1-D array by comparing it
 *                            with every element from the first to the last, and
 *                            report the position where it is found.
 *
 * Idea                     : the loop stops as soon as a match is found, so the
 *                            number of comparisons is the position of the element
 *                            in the best case and n in the worst case (not
 *                            present). The comparison count is printed as proof.
 *
 * Time complexity          : O(n)
 * Space complexity         : O(1) extra
 *
 * Compile : gcc -Wall -Wextra -std=c11 -o exp0_2_linear_search exp0_2_linear_search.c
 */

#include <stdio.h>

#define N 10

int main(void)
{
    int a[N];
    int item, i;
    int position = -1;      /* -1 means "not found" */
    int comparisons = 0;

    printf("Enter %d elements of the array:\n", N);
    for (i = 0; i < N; ++i) {
        if (scanf("%d", &a[i]) != 1) {
            printf("Invalid input: expected %d integers.\n", N);
            return 1;
        }
    }

    printf("Enter the element to be searched: ");
    if (scanf("%d", &item) != 1) {
        printf("Invalid input: expected an integer.\n");
        return 1;
    }

    /* Compare the key with every element until it matches or the array ends. */
    for (i = 0; i < N; ++i) {
        comparisons++;
        if (a[i] == item) {
            position = i;
            break;
        }
    }

    if (position == -1) {
        printf("Element Not Found (after %d comparisons)\n", comparisons);
    } else {
        printf("Element Found at Position = %d (0-based index %d, after %d comparison(s))\n",
               position + 1, position, comparisons);
    }

    return 0;
}
