/*
 * Experiment 0, Part 2 (a) : Insert an integer into a 1-D array
 * Course                   : ELC2132 Data Structures & Algorithms Lab
 * Language                 : C++
 *
 * Aim                      : Insert a new value at a chosen index of a 1-D array
 *                            by shifting every element from that index onwards
 *                            one position to the right.
 *
 * Idea                     : the array has spare capacity, so the number of
 *                            elements in use (size) grows with every insertion
 *                            and no existing element is lost.
 *
 * Compile : g++ -Wall -Wextra -Wpedantic -std=c++17 -o exp0_2 exp0_2.cpp
 */

#include <iostream>
using namespace std;

#define CAPACITY 10   // how many elements the array can hold

int main()
{
    int arr[CAPACITY] = {10, 20, 30};
    int size = 3;        // elements currently in use
    int index = 0;
    int value = 0;

    cout << "Array before insertion (" << size << " elements):" << endl;
    for (int i = 0; i < size; i++) {
        cout << "arr[" << i << "] = " << arr[i] << endl;
    }

    cout << "Enter the index where you want to insert (0 to " << size << "): ";
    cin >> index;

    if (index < 0 || index > size) {
        cout << "Invalid index! Please enter a valid index." << endl;
        return 1;
    }
    if (size == CAPACITY) {
        cout << "Array is full, cannot insert another element." << endl;
        return 1;
    }

    cout << "Enter the new value to insert: ";
    cin >> value;

    // Shift every element from index onwards one place to the right.
    for (int i = size; i > index; i--) {
        arr[i] = arr[i - 1];
    }

    // Put the new value into the empty place that the shifting created.
    arr[index] = value;
    size = size + 1;

    cout << "Array after insertion (" << size << " elements):" << endl;
    for (int i = 0; i < size; i++) {
        cout << "arr[" << i << "] = " << arr[i] << endl;
    }

    return 0;
}
