/*
 * Experiment 3.3 : Tower of Hanoi using recursion
 * Course          : ELC2132 Data Structures & Algorithms Lab
 * Language        : C++
 *
 * Aim             : Move n disks from rod A to rod C using rod B, one disk at a
 *                   time, and never put a bigger disk on a smaller one.
 *
 * Base case       : n = 1, move that single disk straight to the destination
 * Recursive case  : move the top n-1 disks to the helper rod, move disk n to the
 *                   destination, then move the n-1 disks on top of it
 *
 * Time complexity : O(2^n), the number of moves is exactly 2^n - 1
 * Space complexity: O(n) calls waiting on the stack
 *
 * Compile : g++ -Wall -Wextra -Wpedantic -std=c++17 -o tower_of_hanoi exp3_3_tower_of_hanoi.cpp
 */

#include <iostream>
using namespace std;

#define MAX_DISKS 20

int moveCount = 0;

// Move n disks from 'source' to 'destination' using 'helper' as the spare rod.
void towerOfHanoi(int n, char source, char helper, char destination)
{
    if (n == 1) {
        // Base case: only one disk left, move it directly.
        moveCount = moveCount + 1;
        cout << "Move " << moveCount << ": disk 1 from " << source
             << " to " << destination << endl;
        return;
    }

    // Step 1: move the top n-1 disks out of the way onto the helper rod.
    towerOfHanoi(n - 1, source, destination, helper);

    // Step 2: move the biggest disk to the destination.
    moveCount = moveCount + 1;
    cout << "Move " << moveCount << ": disk " << n << " from " << source
         << " to " << destination << endl;

    // Step 3: move the n-1 disks from the helper rod onto the destination.
    towerOfHanoi(n - 1, helper, source, destination);
}

int main()
{
    int n = 0;

    cout << "Enter the number of disks (1 to " << MAX_DISKS << "): ";
    cin >> n;

    if (n < 1 || n > MAX_DISKS) {
        cout << "Please enter a number between 1 and " << MAX_DISKS << "." << endl;
        return 1;
    }

    cout << endl;
    cout << "Moves for " << n << " disks (A = source, B = helper, C = destination):" << endl;
    towerOfHanoi(n, 'A', 'B', 'C');

    cout << endl;
    cout << "Total number of moves = " << moveCount << endl;

    return 0;
}
