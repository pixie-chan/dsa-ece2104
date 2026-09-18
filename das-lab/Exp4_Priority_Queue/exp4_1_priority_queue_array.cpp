/*
 * Experiment 4.1 : Max-priority queue using an UNSORTED ARRAY
 * Course          : ELC2132 Data Structures & Algorithms Lab
 * Language        : C++
 *
 * Aim             : Implement a priority queue using an unsorted array with the
 *                   operations insert, extractMax, peek and isEmpty.
 *
 * Idea            : a new element is simply put at the end, so insert is O(1).
 *                   The biggest priority is found by checking every element, so
 *                   peek and extractMax are O(n).
 *
 *   insert      O(1)
 *   peek        O(n)
 *   extractMax  O(n)
 *   isEmpty     O(1)
 *
 * Compile : g++ -Wall -Wextra -Wpedantic -std=c++17 -o pq_array exp4_1_priority_queue_array.cpp
 */

#include <iostream>
using namespace std;

#define MAX 100

// One element of the queue: the value and the priority it is served with.
struct Element {
    int data;
    int priority;
};

Element pq[MAX];
int n = 0;      // how many elements are in the queue

bool isEmpty()
{
    return n == 0;
}

bool isFull()
{
    return n == MAX;
}

// Insert at the end, no ordering is kept.
void insertItem(int data, int priority)
{
    if (isFull()) {
        cout << "Queue is full, cannot insert " << data << endl;
        return;
    }

    pq[n].data = data;
    pq[n].priority = priority;
    n = n + 1;

    cout << "Inserted: data = " << data << ", priority = " << priority << endl;
}

// Find the position of the element with the biggest priority.
int maxIndex()
{
    int index = 0;

    for (int i = 1; i < n; i++) {
        if (pq[i].priority > pq[index].priority) {
            index = i;
        }
    }
    return index;
}

// Show the biggest priority without removing it.
void peek()
{
    if (isEmpty()) {
        cout << "Queue is empty, nothing to peek" << endl;
        return;
    }

    int index = maxIndex();
    cout << "Highest priority element: data = " << pq[index].data
         << ", priority = " << pq[index].priority << endl;
}

// Remove the element with the biggest priority.
void extractMax()
{
    if (isEmpty()) {
        cout << "Queue is empty, nothing to extract" << endl;
        return;
    }

    int index = maxIndex();
    cout << "Extracted: data = " << pq[index].data
         << ", priority = " << pq[index].priority << endl;

    // Close the gap by moving the following elements one place to the left.
    for (int i = index; i < n - 1; i++) {
        pq[i] = pq[i + 1];
    }
    n = n - 1;
}

void display()
{
    if (isEmpty()) {
        cout << "Queue is empty" << endl;
        return;
    }

    cout << "Queue elements (in the order they were inserted, size = " << n << "):" << endl;
    for (int i = 0; i < n; i++) {
        cout << "  [" << i << "] data = " << pq[i].data
             << ", priority = " << pq[i].priority << endl;
    }
}

int main()
{
    int choice = 0;
    int data = 0;
    int priority = 0;

    do {
        cout << endl;
        cout << "--- Max Priority Queue (unsorted array) ---" << endl;
        cout << "1. Insert" << endl;
        cout << "2. Extract max" << endl;
        cout << "3. Peek" << endl;
        cout << "4. Display" << endl;
        cout << "5. isEmpty" << endl;
        cout << "6. Exit" << endl;
        cout << "Enter your choice: ";
        cin >> choice;

        if (choice == 1) {
            cout << "Enter data and priority: ";
            cin >> data >> priority;
            insertItem(data, priority);
        } else if (choice == 2) {
            extractMax();
        } else if (choice == 3) {
            peek();
        } else if (choice == 4) {
            display();
        } else if (choice == 5) {
            if (isEmpty()) {
                cout << "Queue is empty" << endl;
            } else {
                cout << "Queue is not empty" << endl;
            }
        } else if (choice == 6) {
            cout << "Exiting." << endl;
        } else {
            cout << "Wrong choice, enter a number from 1 to 6." << endl;
        }
    } while (choice != 6);

    return 0;
}
