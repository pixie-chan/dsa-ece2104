// ============================================================================
// ECE2104 DSA  |  LECTURE 7: Linked List Operations
//   "Creation and Insertion, Deletion and Traversal"   CO1
// compile + run: g++ -Wall -Wextra -o /tmp/lec7 07_linked_list_operations.cpp && /tmp/lec7
// ============================================================================
#include <iostream>
using namespace std;

struct Node {
    int data;
    Node* next;
    Node(int v) : data(v), next(nullptr) {}
};

// ---- TRAVERSAL: the loop every other operation is built from ---------------
void traverse(Node* head) {
    cout << "   ";
    for (Node* cur = head; cur != nullptr; cur = cur->next) cout << cur->data << " -> ";
    cout << "NULL";
}

int countNodes(Node* head) {
    int n = 0;
    for (Node* cur = head; cur != nullptr; cur = cur->next) n++;
    return n;
}

void state(const char* what, Node* head) {
    traverse(head);
    cout << "     [" << what << ", count=" << countNodes(head) << "]" << endl;
}

// ---- CREATION + INSERTION --------------------------------------------------
// (a) at the beginning: O(1)
void insertAtBeginning(Node*& head, int v) {
    Node* n = new Node(v);
    n->next = head;      // 1. link the new node to the old first node
    head = n;            // 2. move head onto the new node
}

// (b) at the end: O(n), you must walk to the last node
void insertAtEnd(Node*& head, int v) {
    Node* n = new Node(v);
    if (head == nullptr) { head = n; return; }
    Node* cur = head;
    while (cur->next != nullptr) cur = cur->next;
    cur->next = n;
}

// (c) at a given position (1-based, position 1 = beginning)
void insertAtPosition(Node*& head, int v, int pos) {
    if (pos <= 1 || head == nullptr) { insertAtBeginning(head, v); return; }
    Node* cur = head;
    for (int i = 1; i < pos - 1 && cur->next != nullptr; i++) cur = cur->next;
    Node* n = new Node(v);
    n->next = cur->next;   // new node takes over the rest of the chain
    cur->next = n;         // predecessor now points at the new node
}

// ---- DELETION --------------------------------------------------------------
// (a) from the beginning: O(1)
void deleteFromBeginning(Node*& head) {
    if (head == nullptr) return;
    Node* dead = head;
    head = head->next;    // move head FIRST
    delete dead;          // then free. Reverse these two and you free the list.
}

// (b) from the end: O(n)
void deleteFromEnd(Node*& head) {
    if (head == nullptr) return;
    if (head->next == nullptr) { deleteFromBeginning(head); return; }
    Node* cur = head;
    while (cur->next->next != nullptr) cur = cur->next;   // stop at second last
    delete cur->next;
    cur->next = nullptr;
}

// (c) by value: the general case, three sub-cases
void deleteByValue(Node*& head, int v) {
    if (head == nullptr) return;
    if (head->data == v) { deleteFromBeginning(head); return; }   // case 1: it is the first node
    Node* cur = head;
    while (cur->next != nullptr && cur->next->data != v) cur = cur->next;
    if (cur->next == nullptr) return;                             // case 2: value absent, do nothing
    Node* dead = cur->next;                                       // case 3: middle or last node
    cur->next = dead->next;   // bridge around it first
    delete dead;              // then free
}

int main() {
    Node* head = nullptr;

    cout << "EMPTY LIST" << endl;
    state("head = nullptr", head);

    cout << "\n--- INSERTION ---" << endl;
    insertAtBeginning(head, 20);   state("insertAtBeginning(20)", head);
    insertAtBeginning(head, 10);   state("insertAtBeginning(10)", head);
    insertAtEnd(head, 40);         state("insertAtEnd(40)", head);
    insertAtEnd(head, 50);         state("insertAtEnd(50)", head);
    insertAtPosition(head, 30, 3); state("insertAtPosition(30, pos 3)", head);

    cout << "\n--- DELETION ---" << endl;
    deleteByValue(head, 30);       state("deleteByValue(30) middle", head);
    deleteFromEnd(head);           state("deleteFromEnd()", head);
    deleteFromBeginning(head);     state("deleteFromBeginning()", head);
    state("(before attempting a missing value)", head);
    deleteByValue(head, 999);      state("deleteByValue(999) absent", head);

    cout << "\n--- COST TABLE (exam answer) ---" << endl;
    cout << "   insertion at beginning : O(1)" << endl;
    cout << "   insertion at end       : O(n)   (O(1) if you also keep a tail pointer)" << endl;
    cout << "   insertion at position k: O(k)" << endl;
    cout << "   deletion at beginning  : O(1)" << endl;
    cout << "   deletion at end        : O(n)   (O(1) if doubly linked)" << endl;
    cout << "   traversal / search     : O(n)" << endl;

    // free whatever is left
    while (head != nullptr) deleteFromBeginning(head);
    cout << "\nlist freed, head = " << head << endl;
    return 0;
}
