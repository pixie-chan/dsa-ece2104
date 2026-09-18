/*
 * Experiment 3.2 : Fibonacci series using recursion
 * Course          : ELC2132 Data Structures & Algorithms Lab
 * Language        : C++
 *
 * Aim             : Compute the Fibonacci series F(0) = 0, F(1) = 1 and
 *                   F(n) = F(n-1) + F(n-2) for n > 1, using recursion.
 *
 * Base cases      : F(0) = 0, F(1) = 1
 * Recursive case  : F(n) = F(n-1) + F(n-2)     (two recursive calls)
 *
 * Time complexity : O(2^n), the same values are calculated again and again
 * Space complexity: O(n), only the deepest chain of calls is kept in memory
 *
 * Compile : g++ -Wall -Wextra -Wpedantic -std=c++17 -o fibonacci exp3_2_fibonacci.cpp
 */

#include <iostream>
using namespace std;

#define MAX_N 35

int callCount = 0;   // counts the calls, to show how fast the work grows

int fib(int n)
{
    callCount = callCount + 1;

    if (n == 0) {
        return 0;                    // base case 1
    }
    if (n == 1) {
        return 1;                    // base case 2
    }
    return fib(n - 1) + fib(n - 2);  // recursive case
}

int main()
{
    int n = 0;

    cout << "Enter n (0 to " << MAX_N << "): ";
    cin >> n;

    if (n < 0 || n > MAX_N) {
        cout << "Please enter a number between 0 and " << MAX_N << "." << endl;
        return 1;
    }

    cout << "Fibonacci series (first " << n + 1 << " terms):" << endl;
    for (int i = 0; i <= n; i++) {
        cout << "F(" << i << ") = " << fib(i) << endl;
    }

    callCount = 0;
    int value = fib(n);

    cout << endl;
    cout << "F(" << n << ") = " << value << endl;
    cout << "Total recursive calls made to compute F(" << n << "): " << callCount << endl;

    return 0;
}
