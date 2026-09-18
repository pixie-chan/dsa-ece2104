// ============================================================================
// ECE2104 DSA  |  LECTURE 8: Searching in a Linked List, Applications,
//                            Numerical Problems and Practice
// compile + run: g++ -Wall -Wextra -o /tmp/lec8 08_linked_list_searching_apps.cpp && /tmp/lec8
//
// The numerical problem below is the standard exam one:
//   "Given the list 10, 20, 30, 40, 50, search 30 and 35, and count comparisons."
//   "Reverse the list and print it." / "Remove duplicates."
// ============================================================================
#include <iostream>
using namespace std;

struct Node {
    int data;
    Node* next;
    Node(int v) : data(v), next(nullptr) {}
};

void traverse(Node* head) {
    for (Node* cur = head; cur != nullptr; cur = cur->next) cout << cur->data << " -> ";
    cout << "NULL" << endl;
}

// ---- LINEAR SEARCH: O(n), counting comparisons for the exam ----------------
int linearSearch(Node* head, int key, int& comparisons) {
    int position = 1;
    comparisons = 0;
    for (Node* cur = head; cur != nullptr; cur = cur->next, position++) {
        comparisons++;
        if (cur->data == key) return position;   // 1-based position of the key
    }
    return -1;                                   // -1 means not present
}

// ---- MOVE-TO-FRONT: the self-organising search (an "application") ----------
bool searchMoveToFront(Node*& head, int key) {
    if (head == nullptr || head->data == key) return head != nullptr;
    Node* prev = head;
    while (prev->next != nullptr && prev->next->data != key) prev = prev->next;
    if (prev->next == nullptr) return false;      // absent
    Node* found = prev->next;
    prev->next = found->next;                     // unlink
    found->next = head;                           // relink at the front
    head = found;
    return true;
}

// ---- APPLICATION 1: reverse in place (pointer manipulation) ----------------
void reverseList(Node*& head) {
    Node* prev = nullptr;
    Node* cur  = head;
    while (cur != nullptr) {
        Node* nxt = cur->next;   // 1. save the rest of the train
        cur->next = prev;        // 2. flip this node's arrow backwards
        prev = cur;              // 3. prev steps onto this node
        cur = nxt;               // 4. move on
    }
    head = prev;                 // the old last node is the new head
}

// ---- APPLICATION 2: remove duplicates from a sorted list -------------------
void removeDuplicatesSorted(Node* head) {
    for (Node* cur = head; cur != nullptr && cur->next != nullptr; ) {
        if (cur->data == cur->next->data) {
            Node* dup = cur->next;
            cur->next = dup->next;   // bridge over the duplicate
            delete dup;
        } else {
            cur = cur->next;         // only advance when no deletion happened
        }
    }
}

// ---- APPLICATION 3: merge two sorted lists (the base of merge sort) --------
Node* mergeSorted(Node* a, Node* b) {
    Node dummy(0);                 // DUMMY NODE: removes all the empty-list special cases
    Node* tail = &dummy;
    while (a != nullptr && b != nullptr) {
        if (a->data <= b->data) { tail->next = a; a = a->next; }
        else                    { tail->next = b; b = b->next; }
        tail = tail->next;
    }
    tail->next = (a != nullptr) ? a : b;   // hang the leftovers
    return dummy.next;                     // the real head
}

// ---- APPLICATION 4: polynomial addition (the classic linked list use) ------
// a polynomial is stored as nodes: coefficient + exponent, e.g. 5x^2 + 3x + 1
struct Poly {
    int coeff, exp;
    Poly* next;
    Poly(int c, int e) : coeff(c), exp(e), next(nullptr) {}
};

void insertTerm(Poly*& head, int c, int e) {
    Poly* n = new Poly(c, e);
    if (head == nullptr) { head = n; return; }
    Poly* cur = head;
    while (cur->next != nullptr) cur = cur->next;
    cur->next = n;
}

void printPoly(Poly* head) {
    bool first = true;
    for (Poly* cur = head; cur != nullptr; cur = cur->next) {
        if (!first && cur->coeff >= 0) cout << " + ";
        cout << cur->coeff << "x^" << cur->exp;
        first = false;
    }
    cout << endl;
}

Poly* addPoly(Poly* p1, Poly* p2) {
    Poly dummy(0, 0);
    Poly* tail = &dummy;
    while (p1 != nullptr && p2 != nullptr) {
        if (p1->exp > p2->exp)      { tail->next = new Poly(p1->coeff, p1->exp); p1 = p1->next; }
        else if (p1->exp < p2->exp) { tail->next = new Poly(p2->coeff, p2->exp); p2 = p2->next; }
        else { tail->next = new Poly(p1->coeff + p2->coeff, p1->exp); p1 = p1->next; p2 = p2->next; }
        tail = tail->next;
    }
    while (p1 != nullptr) { tail->next = new Poly(p1->coeff, p1->exp); tail = tail->next; p1 = p1->next; }
    while (p2 != nullptr) { tail->next = new Poly(p2->coeff, p2->exp); tail = tail->next; p2 = p2->next; }
    return dummy.next;
}

// ---- APPLICATION 5: stack as a linked list (preview of Lecture 12) ---------
// push_front = push, pop_front = pop: O(1) both, no fixed size limit
bool stackEmpty(Node* top) { return top == nullptr; }
void push(Node*& top, int v) { Node* n = new Node(v); n->next = top; top = n; }
int pop(Node*& top) {
    int v = top->data;
    Node* dead = top;
    top = top->next;
    delete dead;
    return v;
}

Node* build(int vals[], int n) {
    Node dummy(0); Node* tail = &dummy;
    for (int i = 0; i < n; i++) { tail->next = new Node(vals[i]); tail = tail->next; }
    return dummy.next;
}
void freeAll(Node*& head) { while (head) { Node* t = head; head = head->next; delete t; } }

int main() {
    cout << "=== 1. NUMERICAL PROBLEM: search a linked list ===" << endl;
    int vals[] = {10, 20, 30, 40, 50};
    Node* head = build(vals, 5);
    cout << "list: "; traverse(head);
    int comps = 0;
    int p30 = linearSearch(head, 30, comps);
    cout << "search 30 -> found at position " << p30 << ", comparisons = " << comps << endl;
    int p35 = linearSearch(head, 35, comps);
    cout << "search 35 -> result " << p35 << " (absent), comparisons = " << comps << endl;
    cout << "why " << comps << "? The list ran out before the key was found, so the whole list was scanned." << endl;
    cout << "best case 1 comparison (key is at head), worst case n = 5, average n/2 = 2.5" << endl;
    cout << "a sorted ARRAY would answer 35 in 3 comparisons by binary search; a linked list cannot: no index, no O(1) jump" << endl;

    cout << "\n=== 2. APPLICATION: move-to-front search ===" << endl;
    cout << "before: "; traverse(head);
    searchMoveToFront(head, 40);
    cout << "after searchMoveToFront(40): "; traverse(head);
    cout << "(the found node is promoted to the front, popular keys get cheap to find)" << endl;

    cout << "\n=== 3. APPLICATION: reverse in place ===" << endl;
    cout << "before: "; traverse(head);
    reverseList(head);
    cout << "after : "; traverse(head);

    cout << "\n=== 4. APPLICATION: remove duplicates (sorted list) ===" << endl;
    freeAll(head);
    int dup[] = {10, 10, 20, 30, 30, 30, 40};
    Node* d = build(dup, 7);
    cout << "before: "; traverse(d);
    removeDuplicatesSorted(d);
    cout << "after : "; traverse(d);

    cout << "\n=== 5. APPLICATION: merge two sorted lists ===" << endl;
    int a1[] = {1, 4, 9}, a2[] = {2, 3, 10};
    Node* m1 = build(a1, 3), *m2 = build(a2, 3);
    cout << "list A: "; traverse(m1);
    cout << "list B: "; traverse(m2);
    Node* merged = mergeSorted(m1, m2);
    cout << "merged: "; traverse(merged);

    cout << "\n=== 6. APPLICATION: polynomial addition (5x^2 + 3x + 1) + (4x^3 + 2x^2 + 6) ===" << endl;
    Poly* p1 = nullptr; insertTerm(p1, 5, 2); insertTerm(p1, 3, 1); insertTerm(p1, 1, 0);
    Poly* p2 = nullptr; insertTerm(p2, 4, 3); insertTerm(p2, 2, 2); insertTerm(p2, 6, 0);
    cout << "P1 = "; printPoly(p1);
    cout << "P2 = "; printPoly(p2);
    Poly* sum = addPoly(p1, p2);
    cout << "P1+P2 = "; printPoly(sum);
    cout << "(4x^3 + 7x^2 + 3x + 7: exponents matched, coefficients added)" << endl;

    cout << "\n=== 7. APPLICATION: stack built on a linked list (Lecture 12 preview) ===" << endl;
    Node* top = nullptr;
    push(top, 10); push(top, 20); push(top, 30);
    cout << "pushed 10, 20, 30 -> top is " << top->data << " (LIFO)" << endl;
    cout << "pop -> " << pop(top) << ", pop -> " << pop(top) << ", empty now? " << (stackEmpty(top) ? "yes" : "no") << endl;
    pop(top);
    cout << "after the last pop, empty? " << (stackEmpty(top) ? "yes" : "no") << endl;

    cout << "\n=== 8. WHY a linked list (viva answer) ===" << endl;
    cout << "  array : O(1) random access, fixed capacity, insertion in the middle shifts O(n) elements" << endl;
    cout << "  list  : O(n) access, unbounded capacity, insertion/deletion is O(1) once you hold the node" << endl;
    cout << "  cost  : 8 bytes of link per int, and the nodes are scattered so caches cannot help" << endl;

    freeAll(d); freeAll(merged);
    freeAll(head);
    while (p1) { Poly* t = p1; p1 = p1->next; delete t; }
    while (p2) { Poly* t = p2; p2 = p2->next; delete t; }
    while (sum) { Poly* t = sum; sum = sum->next; delete t; }
    return 0;
}
