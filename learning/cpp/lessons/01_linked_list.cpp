// LESSON 1: Linked List — a train of boxes
// COMPILE: g++ -Wall -o lesson1 01_linked_list.cpp && ./lesson1
#include <iostream>
using namespace std;

// A Node is ONE box of the train.
//    data = the cargo inside the box
//    next = pointer to the NEXT box (nullptr if this is the last one)
struct Node {
    int data;
    Node* next;

    Node(int val) : data(val), next(nullptr) {}   // constructor: builds a box
};

// walk the train from head, printing every box
void printList(Node* head) {
    Node* cur = head;
    while (cur != nullptr) {
        cout << cur->data << " -> ";
        cur = cur->next;          // hop to the next box
    }
    cout << "NULL\n";
}

// how many boxes are in the train?
int size(Node* head) {
    int n = 0;
    while (head != nullptr) { n++; head = head->next; }
    return n;
}

// insert at the FRONT — very fast (O(1))
void pushFront(Node*& head, int val) {
    Node* n = new Node(val);   // build a new box
    n->next = head;            // its next = the old first box
    head = n;                  // the new box becomes first
}

// insert at the END — slower (O(n)): must walk to the last box
void pushBack(Node*& head, int val) {
    Node* n = new Node(val);
    if (head == nullptr) { head = n; return; }   // empty train: it's the head
    Node* cur = head;
    while (cur->next != nullptr) cur = cur->next; // walk to last box
    cur->next = n;                                // attach after it
}

// remove the FRONT box — fast (O(1))
void popFront(Node*& head) {
    if (head == nullptr) return;          // empty, nothing to remove
    Node* old = head;                     // remember the box
    head = head->next;                    // head skips to the next box
    delete old;                           // free its memory
}

// ---- broken version (TRAP): head passed BY VALUE ----
void brokenPushFront(Node* head, int val) {
    Node* n = new Node(val);
    n->next = head;
    head = n;   // only changes a COPY... the caller's head stays untouched
}

int main() {
    cout << "=== 1. build: three pushFront calls ===\n";
    Node* head = nullptr;                 // empty train: no head box yet
    pushFront(head, 30);
    pushFront(head, 20);
    pushFront(head, 10);
    printList(head);   // expect: 10 -> 20 -> 30 -> NULL

    cout << "\n=== 2. pushBack: 40, then 50 ===\n";
    pushBack(head, 40);
    pushBack(head, 50);
    printList(head);   // expect: 10 -> 20 -> 30 -> 40 -> 50 -> NULL

    cout << "\n=== 3. popFront: remove the first box ===\n";
    popFront(head);
    printList(head);   // expect: 20 -> 30 -> 40 -> 50 -> NULL

    cout << "\nsize = " << size(head) << "\n";

    cout << "\n=== 4. TRAP: wrong order makes a self-loop ===\n";
    Node* h = nullptr;
    Node* x = new Node(1);
    h = x;             // WRONG: assign head BEFORE fixing next
    x->next = h;       // now x points to ITSELF -> infinite loop!
    cout << (x->next == x ? "SELF-LOOP detected: this list would never end"
                          : "ok") << "\n";
    delete x;

    cout << "\n=== 5. TRAP: passing head by value ===\n";
    Node* h2 = nullptr;
    brokenPushFront(h2, 7);                 // looks right, but...
    cout << "size after brokenPushFront = " << size(h2)
         << "  (should be 1!)\n";

    cout << "\n=== 6. cleanup: free every box ===\n";
    while (head != nullptr) {
        Node* tmp = head;
        head = head->next;
        delete tmp;
    }
    cout << "all boxes freed, head is now "
         << (head == nullptr ? "nullptr" : "?") << "\n";
    return 0;
}