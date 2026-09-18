// ============================================================================
// ECE2104 DSA  |  LECTURE 6: Linked List, Concept, Types, Node Representation
//   session outcome: "Represent linked lists in memory"        CO1
// compile + run: g++ -Wall -Wextra -o /tmp/lec6 06_linked_list_representation.cpp && /tmp/lec6
// ============================================================================
#include <iostream>
#include <cstddef>     // offsetof
#include <iomanip>
using namespace std;

// ---- the node: TWO fields, that is the whole representation ----------------
struct Node {
    int   data;    // info field
    Node* next;    // link field: address of the next node
    Node(int v) : data(v), next(nullptr) {}
};

// ---- the doubly linked node: THREE fields (Lecture 6, "Types") -------------
struct DNode {
    DNode* prev;   // address of the previous node
    int    data;
    DNode* next;
    DNode(int v) : prev(nullptr), data(v), next(nullptr) {}
};

int main() {
    cout << "=== 1. a node is one int plus one pointer ===" << endl;
    cout << "sizeof(Node)      = " << sizeof(Node) << " bytes" << endl;
    cout << "  int data        = " << sizeof(int) << " bytes" << endl;
    cout << "  Node* next      = " << sizeof(Node*) << " bytes" << endl;
    cout << "  padding         = " << sizeof(Node) - sizeof(int) - sizeof(Node*) << " bytes (the compiler aligns the pointer)" << endl;
    cout << "offsetof(data)    = " << offsetof(Node, data) << "  (the box starts here)" << endl;
    cout << "offsetof(next)    = " << offsetof(Node, next) << "  (the coupling sits here)" << endl;

    cout << "\n=== 2. array of nodes = CONTIGUOUS memory ===" << endl;
    Node block[3] = { Node(10), Node(20), Node(30) };
    for (int i = 0; i < 3; i++)
        cout << "  block[" << i << "] at " << &block[i]
             << "   data=" << block[i].data
             << "   next=" << block[i].next << endl;
    cout << "  node i+1 is exactly " << sizeof(Node) << " bytes after node i: the whole list is ONE allocation" << endl;

    cout << "\n=== 3. linked nodes = SCATTERED memory ===" << endl;
    Node* a = new Node(10);
    Node* b = new Node(20);
    Node* c = new Node(30);
    a->next = b;      // link field of a holds &b
    b->next = c;      // link field of b holds &c
    c->next = nullptr;// end of list
    for (Node* cur = a; cur != nullptr; cur = cur->next)
        cout << "  node at " << cur << "   data=" << cur->data
             << "   next=" << cur->next << endl;
    cout << "  addresses have no arithmetic pattern: each `new` asked the heap for " << sizeof(Node) << " bytes" << endl;
    cout << "  head = " << a << "   (the ONLY way in; lose head, lose the list)" << endl;

    cout << "\n=== 4. TYPE 1: singly linked list (one link, one direction) ===" << endl;
    cout << "  10 --> 20 --> 30 --> nullptr      a->next->next->data = " << a->next->next->data << endl;

    cout << "\n=== 5. TYPE 2: doubly linked list (two links, both directions) ===" << endl;
    DNode* d1 = new DNode(10);
    DNode* d2 = new DNode(20);
    DNode* d3 = new DNode(30);
    d1->next = d2; d2->prev = d1;
    d2->next = d3; d3->prev = d2;
    cout << "  forward : ";
    for (DNode* cur = d1; cur != nullptr; cur = cur->next) cout << cur->data << " <-> ";
    cout << "nullptr" << endl;
    cout << "  backward: ";
    for (DNode* cur = d3; cur != nullptr; cur = cur->prev) cout << cur->data << " <-> ";
    cout << "nullptr" << endl;
    cout << "  cost: sizeof(DNode) = " << sizeof(DNode) << " bytes per node (one extra pointer)" << endl;

    cout << "\n=== 6. TYPE 3: circular linked list (last node points back to first) ===" << endl;
    Node* x = new Node(1);
    Node* y = new Node(2);
    Node* z = new Node(3);
    x->next = y; y->next = z;
    z->next = x;                    // <-- the only line that makes it circular
    cout << "  1 --> 2 --> 3 --> back to 1 --> ... forever" << endl;
    cout << "  NEVER write `while (cur != nullptr)` on this list: it never becomes nullptr" << endl;
    cout << "  walk exactly n steps instead: ";
    Node* cur = x;
    for (int i = 0; i < 7; i++) { cout << cur->data << " "; cur = cur->next; }
    cout << "  (7 hops around a 3 node ring)" << endl;

    cout << "\n=== 7. types summary ===" << endl;
    cout << "  singly   : prev access impossible, least memory, most interview questions" << endl;
    cout << "  doubly   : O(1) backward step, +1 pointer per node, used by LRU cache" << endl;
    cout << "  circular : no nullptr end, used by round robin schedulers, music playlists" << endl;

    // cleanup: every new gets exactly one delete
    delete a; delete b; delete c;
    delete d1; delete d2; delete d3;
    delete x; delete y; delete z;
    return 0;
}
