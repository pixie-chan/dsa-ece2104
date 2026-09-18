/*
 * Experiment 0, Part 2 (b) : Search an element using linear search
 * Course                   : ELC2132 Data Structures & Algorithms Lab
 * Language                 : C++
 *
 * Aim                      : Search for a value in an array by comparing it with
 *                            every element from the first to the last, and report
 *                            the position where it is found.
 *
 * Idea                     : the loop stops as soon as a match is found, so the
 *                            number of comparisons is the position of the element
 *                            in the best case and N when it is not present.
 *
 * Compile : g++ -Wall -Wextra -Wpedantic -std=c++17 -o exp0_2_linear_search exp0_2_linear_search.cpp
 */

#include <iostream>
using namespace std;

#define N 10

int main()
{
    int a[N];
    int item = 0;
    int position = -1;    // -1 means "not found"
    int comparisons = 0;

    cout << "Enter " << N << " elements of the array:" << endl;
    for (int i = 0; i < N; i++) {
        cin >> a[i];
    }

    cout << "Enter the element to be searched: ";
    cin >> item;

    // Compare the item with every element until it matches.
    for (int i = 0; i < N; i++) {
        comparisons = comparisons + 1;
        if (a[i] == item) {
            position = i;
            break;
        }
    }

    if (position == -1) {
        cout << "Element Not Found (after " << comparisons << " comparisons)" << endl;
    } else {
        cout << "Element Found at Position = " << position + 1
             << " (0-based index " << position
             << ", after " << comparisons << " comparison(s))" << endl;
    }

    return 0;
}
