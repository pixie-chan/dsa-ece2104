// ============================================================================
// LINKED LIST FROM SCRATCH, part 1: the box, the ride, the two inserts
// compile + run:  g++ -Wall -Wextra -o /tmp/ll1 kiki_ll_basics.cpp && /tmp/ll1
// ============================================================================
#include <iostream>
using namespace std;

// ---- THE BOX ---------------------------------------------------------------
struct Node {
    int data;        // the cargo
    Node* next;      // the coupling: address of the next box (nullptr = last)
    Node(int v) : data(v), next(nullptr) {}   // constructor: new box, empty coupling
};

// ---- THE RIDE --------------------------------------------------------------
int length(Node* head) {
    int n = 0;
    for (Node* cur = head; cur != nullptr; cur = cur->next) n++;
    return n;
}

void printList(Node* head) {
    for (Node* cur = head; cur != nullptr; cur = cur->next)
        cout << cur->data << " -> ";
    cout << "nullptr" << endl;
}

// ---- THE TWO INSERTS -------------------------------------------------------
void pushFront(Node*& head, int v) {   // O(1): front door
    Node* n = new Node(v);
    n->next = head;        // 1. coupling clicks onto the old front box
    head = n;              // 2. head flag moves onto the new box
}

void pushBack(Node*& head, int v) {    // O(n): must walk to the last box
    Node* n = new Node(v);
    if (head == nullptr) { head = n; return; }   // empty train guard
    Node* cur = head;
    while (cur->next != nullptr) cur = cur->next;   // walk to the last box
    cur->next = n;                                   // hang the new box there
}

// ---- THE DELETE ------------------------------------------------------------
void removeValue(Node*& head, int v) {
    if (head == nullptr) return;
    if (head->data == v) {                 // v is in the front box
        Node* dead = head;
        head = head->next;                 // head skips over it first
        delete dead;                       // then free
        return;
    }
    Node* cur = head;
    while (cur->next != nullptr && cur->next->data != v) cur = cur->next;
    if (cur->next == nullptr) return;      // not found, do nothing
    Node* dead = cur->next;                // the box to kill
    cur->next = dead->next;                // bridge around it FIRST
    delete dead;                           // then free
}

// ---- THE CLEANUP -----------------------------------------------------------
void freeList(Node*& head) {
    while (head != nullptr) {
        Node* dead = head;
        head = head->next;   // step off before deleting
        delete dead;
    }
}

int main() {
    cout << "=== 1. three boxes, chained by hand ===" << endl;
    Node* a = new Node(10);
    Node* b = new Node(20);
    Node* c = new Node(30);
    a->next = b;              // 10 -> 20
    b->next = c;              // 20 -> 30, c->next stays nullptr = end
    Node* head = a;
    printList(head);
    cout << "length = " << length(head) << endl;

    cout << "\n=== 2. an array is one block, a list is scattered boxes ===" << endl;
    int arr[3] = {10, 20, 30};
    cout << "array  addresses: " << &arr[0] << "  " << &arr[1] << "  " << &arr[2]
         << "   (4 bytes apart, one block)" << endl;
    cout << "node   addresses: " << a << "  " << b << "  " << c
         << "   (far apart, no pattern)" << endl;
    cout << "node addresses come from `new` (heap). Run again and they change." << endl;

    cout << "\n=== 3. pushFront(5): head moves, one hop only ===" << endl;
    pushFront(head, 5);
    printList(head);

    cout << "\n=== 4. pushBack(40): the walk ===" << endl;
    pushBack(head, 40);
    printList(head);
    cout << "length = " << length(head) << endl;

    cout << "\n=== 5. removeValue(20): bridge around, then free ===" << endl;
    removeValue(head, 20);
    printList(head);

    cout << "\n=== 6. free the whole train, head lands on nullptr ===" << endl;
    freeList(head);
    cout << "head = " << head << ", length = " << length(head) << endl;
    return 0;
}
