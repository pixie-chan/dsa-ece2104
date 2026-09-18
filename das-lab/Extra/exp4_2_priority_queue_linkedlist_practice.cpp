#include <iostream>
using namespace std;

struct Node {
    int data;
    int priority;
    Node* next;
};

Node* head = NULL;

bool isEmpty() {
    return head == NULL;
}

// Insert: O(n), keep list sorted in descending priority.
// Equal priority: new node goes after existing ones (FIFO within same priority).
void insertItem(int data, int priority) {
    Node* temp = new Node();
    temp->data = data;
    temp->priority = priority;
    temp->next = NULL;

    if (head == NULL || priority > head->priority) {
        temp->next = head;
        head = temp;
    } else {
        Node* curr = head;
        while (curr->next != NULL && curr->next->priority >= priority)
            curr = curr->next;
        temp->next = curr->next;
        curr->next = temp;
    }
    cout << "Inserted (" << data << ", priority " << priority << ")" << endl;
}

// Peek: O(1), head is always max
void peek() {
    if (isEmpty()) {
        cout << "Queue is empty, nothing to peek" << endl;
        return;
    }
    cout << "Max priority element: data = " << head->data
         << ", priority = " << head->priority << endl;
}

// Extract max: O(1), remove head
void extractMax() {
    if (isEmpty()) {
        cout << "Queue is empty, cannot extract" << endl;
        return;
    }
    Node* temp = head;
    head = head->next;
    cout << "Extracted: data = " << temp->data
         << ", priority = " << temp->priority << endl;
    delete temp;
}

void display() {
    if (isEmpty()) {
        cout << "Queue is empty" << endl;
        return;
    }
    cout << "Queue (descending priority): ";
    Node* curr = head;
    while (curr != NULL) {
        cout << "(" << curr->data << "," << curr->priority << ") ";
        curr = curr->next;
    }
    cout << endl;
}

int main() {
    int choice, data, priority;
    do {
        cout << "\n--- Max Priority Queue (Sorted Linked List) ---\n";
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
