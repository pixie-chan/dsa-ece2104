/*
 * Experiment 4.2 : Max-priority queue using a SORTED LINKED LIST
 * Course          : ELC2132 Data Structures & Algorithms Lab
 * Language        : C++
 *
 * Aim             : Implement a priority queue using a singly linked list that is
 *                   always kept in descending order of priority, with the
 *                   operations insert, extractMax, peek and isEmpty.
 *
 * Idea            : the list stays sorted, so the biggest priority is always the
 *                   first node. peek and extractMax are O(1), but insert has to
 *                   walk the list to find the right place, so it is O(n).
 *
 *   insert      O(n)
 *   peek        O(1)
 *   extractMax  O(1)
 *   isEmpty     O(1)
 *
 * Compile : g++ -Wall -Wextra -Wpedantic -std=c++17 -o pq_list exp4_2_priority_queue_linkedlist.cpp
 */

#include <iostream>
using namespace std;

// One node of the list: the value, its priority and the address of the next node.
struct Node {
    int data;
    int priority;
    Node* next;
};

Node* head = NULL;   // the element with the biggest priority
int count = 0;       // how many elements are in the list

bool isEmpty()
{
    return head == NULL;
}

// Insert the new element so that the list stays in descending order.
void insertItem(int data, int priority)
{
    Node* node = new Node();
    node->data = data;
    node->priority = priority;
    node->next = NULL;

    // Case 1: the new element has the biggest priority, so it becomes the head.
    if (head == NULL || priority > head->priority) {
        node->next = head;
        head = node;
    } else {
        // Case 2: walk until the next node has a smaller priority.
        Node* current = head;
        while (current->next != NULL && current->next->priority >= priority) {
            current = current->next;
        }
        node->next = current->next;
        current->next = node;
    }

    count = count + 1;
    cout << "Inserted: data = " << data << ", priority = " << priority << endl;
}

// Show the biggest priority without removing it.
void peek()
{
    if (isEmpty()) {
        cout << "Queue is empty, nothing to peek" << endl;
        return;
    }

    cout << "Highest priority element: data = " << head->data
         << ", priority = " << head->priority << endl;
}

// Remove the first node, which always has the biggest priority.
void extractMax()
{
    if (isEmpty()) {
        cout << "Queue is empty, nothing to extract" << endl;
        return;
    }

    Node* temp = head;
    cout << "Extracted: data = " << temp->data
         << ", priority = " << temp->priority << endl;

    head = head->next;
    delete temp;
    count = count - 1;
}

void display()
{
    if (isEmpty()) {
        cout << "Queue is empty" << endl;
        return;
    }

    cout << "Queue elements (highest priority first, size = " << count << "):" << endl;
    for (Node* current = head; current != NULL; current = current->next) {
        cout << "  data = " << current->data << ", priority = " << current->priority;
        if (current == head) {
            cout << "  <- head (max)";
        }
        cout << endl;
    }
}

// Free the remaining nodes before the program ends.
void freeQueue()
{
    while (head != NULL) {
        Node* temp = head;
        head = head->next;
        delete temp;
    }
    count = 0;
}

int main()
{
    int choice = 0;
    int data = 0;
    int priority = 0;

    do {
        cout << endl;
        cout << "--- Max Priority Queue (sorted linked list) ---" << endl;
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

    freeQueue();
    return 0;
}
