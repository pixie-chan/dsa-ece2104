/*
 * Experiment 0, Part 1 : 1-D array operations
 * Course               : ELC2132 Data Structures & Algorithms Lab
 * Language             : C++
 *
 * Aim                  : Demonstrate (a) initialization, (b) finding the sum,
 *                        (c) finding the maximum element and (d) reversing a
 *                        1-D array of SIZE elements.
 *
 * Compile : g++ -Wall -Wextra -Wpedantic -std=c++17 -o exp0_1 exp0_1.cpp
 */

#include <iostream>
using namespace std;

#define SIZE 3

int main()
{
    int arr[SIZE] = {10, 20, 30};
    int sum = 0;
    int max = arr[0];
    int temp;

    // (a) Initialization: print the array with its index numbers.
    cout << "Elements of the array:" << endl;
    for (int i = 0; i < SIZE; i++) {
        cout << "arr[" << i << "] = " << arr[i] << endl;
    }

    // (b) Add every element to get the sum.
    for (int i = 0; i < SIZE; i++) {
        sum = sum + arr[i];
    }
    cout << "Sum of elements in the array: " << sum << endl;

    // (c) Compare every element with max and keep the biggest one.
    for (int i = 1; i < SIZE; i++) {
        if (arr[i] > max) {
            max = arr[i];
        }
    }
    cout << "Maximum element in the array: " << max << endl;

    // (d) Reverse the array by swapping the two ends.
    for (int i = 0; i < SIZE / 2; i++) {
        temp = arr[i];
        arr[i] = arr[SIZE - 1 - i];
        arr[SIZE - 1 - i] = temp;
    }

    cout << "Reversed array:" << endl;
    for (int i = 0; i < SIZE; i++) {
        cout << arr[i] << " ";
    }
    cout << endl;

    return 0;
}
