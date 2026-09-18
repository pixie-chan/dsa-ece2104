/*
 * Experiment 0, Pointers : 2-D array operations using pointer notation
 * Course                 : ELC2132 Data Structures & Algorithms Lab
 * Language               : C++
 *
 * Aim                    : Print a 3x3 array and then multiply every element by
 *                          2, reaching every element only through pointer
 *                          arithmetic: *((*(arr + i)) + j)
 *
 * Compile : g++ -Wall -Wextra -Wpedantic -std=c++17 -o exp0_3 exp0_3.cpp
 */

#include <iostream>
using namespace std;

#define ROWS 3
#define COLS 3

// arr is a pointer to a row of the 2-D array, so the function knows the row
// length and the pointer arithmetic below lands on the right element.
void printArray(int (*arr)[COLS], int rows, int cols)
{
    cout << "Array elements:" << endl;
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            cout << *((*(arr + i)) + j) << " ";
        }
        cout << endl;
    }
}

void multiplyByTwo(int (*arr)[COLS], int rows, int cols)
{
    cout << "Array elements multiplied by 2:" << endl;
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            *((*(arr + i)) + j) = *((*(arr + i)) + j) * 2;
            cout << *((*(arr + i)) + j) << " ";
        }
        cout << endl;
    }
}

int main()
{
    int arr[ROWS][COLS] = { {1, 2, 3}, {4, 5, 6}, {7, 8, 9} };

    printArray(arr, ROWS, COLS);
    multiplyByTwo(arr, ROWS, COLS);

    return 0;
}
