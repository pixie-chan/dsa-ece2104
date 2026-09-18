#include <iostream>
using namespace std;
#define MAX 100

struct Element {
    int data;
    int priority;
};

Element pq[MAX];
int n = 0; // current size

bool isEmpty() {
    return n == 0;
}

bool isFull() {
    return n == MAX;
}

// Insert: O(1), just append at end
void insertItem(int data, int priority) {
    if (isFull()) {
        cout << "Priority queue is full, cannot insert " << data << endl;
        return;
    }
    pq[n].data = data;
    pq[n].priority = priority;
    n++;
    cout << "Inserted (" << data << ", priority " << priority << ")" << endl;
}

// Returns index of max priority element. Ties: first inserted wins.
int maxIndex() {
    int idx = 0;
    for (int i = 1; i < n; i++) {
        if (pq[i].priority > pq[idx].priority)
            idx = i;
    }
    return idx;
}

// Peek: O(n), scan for max
void peek() {
    if (isEmpty()) {
        cout << "Queue is empty, nothing to peek" << endl;
        return;
    }
    int idx = maxIndex();
    cout << "Max priority element: data = " << pq[idx].data
         << ", priority = " << pq[idx].priority << endl;
}

// Extract max: O(n) scan + O(n) shift to remove
void extractMax() {
    if (isEmpty()) {
        cout << "Queue is empty, cannot extract" << endl;
        return;
    }
    int idx = maxIndex();
    cout << "Extracted: data = " << pq[idx].data
         << ", priority = " << pq[idx].priority << endl;
    for (int i = idx; i < n - 1; i++)
        pq[i] = pq[i + 1];
    n--;
}

void display() {
    if (isEmpty()) {
        cout << "Queue is empty" << endl;
        return;
    }
    cout << "Queue (unsorted order): ";
    for (int i = 0; i < n; i++)
        cout << "(" << pq[i].data << "," << pq[i].priority << ") ";
    cout << endl;
}

int main() {
    int choice, data, priority;
    do {
        cout << "\n--- Max Priority Queue (Unsorted Array) ---\n";
        cout << "1. Insert\n2. Extract Max\n3. Peek\n4. Display\n5. Exit\n";
        cout << "Enter choice: ";
        if (!(cin >> choice)) break;
        switch (choice) {
            case 1:
                cout << "Enter data and priority: ";
                cin >> data >> priority;
                insertItem(data, priority);
                break;
            case 2:
                extractMax();
                break;
            case 3:
                peek();
                break;
            case 4:
                display();
                break;
            case 5:
                cout << "Exiting..." << endl;
                break;
            default:
                cout << "Invalid choice" << endl;
        }
    } while (choice != 5);
    return 0;
}
