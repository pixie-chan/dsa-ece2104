/*
 * Experiment 3.1 : Factorial of a non-negative integer using recursion
 * Course          : ELC2132 Data Structures & Algorithms Lab
 *
 * Aim             : Compute n! = n * (n-1) * (n-2) * ... * 1, with 0! = 1.
 *
 * Base case       : factorial(0) = 1, factorial(1) = 1
 * Recursive case  : factorial(n) = n * factorial(n - 1)
 *
 * Time complexity : O(n)  - exactly n + 1 calls, one multiplication per call
 * Space complexity: O(n)  - n + 1 stack frames are alive at the deepest point
 *
 * Compile : gcc -Wall -Wextra -std=c11 -o factorial exp3_1_factorial.c
 */

#include <stdio.h>

#define MAX_SAFE_N 20   /* 20! fits in 64 bits, 21! overflows unsigned long long */

/* Returns n! by direct recursion. */
unsigned long long factorial(int n)
{
    if (n == 0 || n == 1) {
        return 1;                               /* base case: stop recursing */
    }
    return (unsigned long long)n * factorial(n - 1);   /* recursive case */
}

int main(void)
{
    int n;

    printf("Enter a non-negative integer (0 to %d): ", MAX_SAFE_N);
    if (scanf("%d", &n) != 1) {
        printf("Invalid input: expected an integer.\n");
        return 1;
    }
    if (n < 0) {
        printf("Invalid input: factorial of a negative number is not defined.\n");
        return 1;
    }
    if (n > MAX_SAFE_N) {
        printf("n is too large: %d! is the largest factorial that fits in 64 bits.\n",
               MAX_SAFE_N);
        return 1;
    }

    printf("%d! = %llu\n", n, factorial(n));

    return 0;
}
