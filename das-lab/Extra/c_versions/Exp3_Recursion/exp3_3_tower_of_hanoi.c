/*
 * Experiment 3.3 : Tower of Hanoi using recursion
 * Course          : ELC2132 Data Structures & Algorithms Lab
 *
 * Aim             : Move n disks from the source rod A to the destination rod C
 *                   using the auxiliary rod B, moving one disk at a time and
 *                   never placing a larger disk on a smaller one.
 *
 * Base case       : n = 1, move the single disk directly from source to destination
 * Recursive case  : move n-1 disks source -> auxiliary, then disk n
 *                   source -> destination, then n-1 disks auxiliary -> destination
 *
 * Time complexity : O(2^n) - exactly 2^n - 1 disk moves
 * Space complexity: O(n)   - n + 1 frames on the call stack
 *
 * Compile : gcc -Wall -Wextra -std=c11 -o tower_of_hanoi exp3_3_tower_of_hanoi.c
 */

#include <stdio.h>

#define MAX_DISKS 20   /* 2^20 - 1 = 1,048,575 moves, still readable on screen */

static unsigned long long move_count = 0;

/* Moves n disks from rod 'source' to rod 'destination' via rod 'auxiliary'. */
void towerOfHanoi(int n, char source, char auxiliary, char destination)
{
    if (n == 1) {
        /* Base case: only one disk, move it straight to the destination. */
        move_count++;
        printf("Move %2llu: disk 1 from %c to %c\n", move_count, source, destination);
        return;
    }

    /* Step 1: move the top n-1 disks out of the way, onto the auxiliary rod. */
    towerOfHanoi(n - 1, source, destination, auxiliary);

    /* Step 2: move the largest remaining disk to the destination rod. */
    move_count++;
    printf("Move %2llu: disk %d from %c to %c\n", move_count, n, source, destination);

    /* Step 3: move the n-1 disks from the auxiliary rod onto the destination. */
    towerOfHanoi(n - 1, auxiliary, source, destination);
}

int main(void)
{
    int n;

    printf("Enter the number of disks (1 to %d): ", MAX_DISKS);
    if (scanf("%d", &n) != 1 || n < 1 || n > MAX_DISKS) {
        printf("Invalid input: enter an integer between 1 and %d.\n", MAX_DISKS);
        return 1;
    }

    printf("\nSequence of moves for %d disks (A = source, B = auxiliary, C = destination):\n", n);
    towerOfHanoi(n, 'A', 'B', 'C');

    printf("\nTotal number of moves = %llu  (= 2^%d - 1)\n", move_count, n);

    return 0;
}
