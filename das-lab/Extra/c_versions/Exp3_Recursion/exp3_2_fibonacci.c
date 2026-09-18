/*
 * Experiment 3.2 : nth term and series of Fibonacci numbers using recursion
 * Course          : ELC2132 Data Structures & Algorithms Lab
 *
 * Aim             : Compute the Fibonacci series F(0) = 0, F(1) = 1 and
 *                   F(n) = F(n-1) + F(n-2) for n > 1, using recursion.
 *
 * Base cases      : F(0) = 0, F(1) = 1
 * Recursive case  : F(n) = F(n-1) + F(n-2)      (two recursive calls)
 *
 * Time complexity : O(2^n) - the call tree nearly doubles at every level,
 *                   because F(n-1) and F(n-2) recompute the same subproblems
 * Space complexity: O(n)   - only the deepest chain of n + 1 frames is alive
 *
 * Compile : gcc -Wall -Wextra -std=c11 -o fibonacci exp3_2_fibonacci.c
 */

#include <stdio.h>

#define MAX_N 35   /* keeps the exponential call tree under a second */

static unsigned long long call_count = 0;   /* counts calls, for the analysis */

/* Returns the nth Fibonacci number by tree recursion. */
unsigned long long fib(int n)
{
    call_count++;

    if (n == 0) {
        return 0;                       /* base case 1 */
    }
    if (n == 1) {
        return 1;                       /* base case 2 */
    }
    return fib(n - 1) + fib(n - 2);     /* recursive case: two calls */
}

int main(void)
{
    int n;

    printf("Enter n (0 to %d): ", MAX_N);
    if (scanf("%d", &n) != 1 || n < 0 || n > MAX_N) {
        printf("Invalid input: enter an integer between 0 and %d.\n", MAX_N);
        return 1;
    }

    printf("Fibonacci series (first %d terms):\n", n + 1);
    for (int i = 0; i <= n; i++) {
        printf("F(%d) = %llu\n", i, fib(i));
    }

    call_count = 0;
    unsigned long long value = fib(n);

    printf("\nF(%d) = %llu\n", n, value);
    printf("Total recursive calls made to compute F(%d): %llu\n", n, call_count);

    return 0;
}
