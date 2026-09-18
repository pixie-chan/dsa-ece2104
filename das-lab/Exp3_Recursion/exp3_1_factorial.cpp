/*
 * Experiment 3.1 : Factorial of a number using recursion
 * Course          : ELC2132 Data Structures & Algorithms Lab
 * Language        : C++
 *
 * Aim             : Find n! = n * (n-1) * (n-2) * ... * 1, with 0! = 1.
 *
 * Base case       : factorial(0) = 1 and factorial(1) = 1
 * Recursive case  : factorial(n) = n * factorial(n - 1)
 *
 * Compile : g++ -Wall -Wextra -Wpedantic -std=c++17 -o factorial exp3_1_factorial.cpp
 */

#include <iostream>
using namespace std;

#define MAX_N 12   // 12! = 479001600, the biggest factorial that fits in an int

int factorial(int n)
{
    if (n == 0 || n == 1) {
        return 1;                    // base case, the recursion stops here
    }
    return n * factorial(n - 1);     // recursive case
}

int main()
{
    int n = 0;

    cout << "Enter a number (0 to " << MAX_N << "): ";
    cin >> n;

    if (n < 0) {
        cout << "Factorial is not defined for negative numbers." << endl;
        return 1;
    }
    if (n > MAX_N) {
        cout << "Please enter a number up to " << MAX_N
             << ", bigger values do not fit in an int." << endl;
        return 1;
    }

    cout << n << "! = " << factorial(n) << endl;

    return 0;
}
