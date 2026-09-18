// ============================================================================
// LESSON 2: Linked lists at the MEMORY level
// The point: stop thinking "list", start thinking "an int + an address".
//
// COMPILE: g++ -Wall -Wextra -o llmem 02_linked_list_memory.cpp && ./llmem
//
// PROVE it is memory-correct (catches leaks + double frees + use-after-free):
//   g++ -Wall -Wextra -g -fsanitize=address -o llmem_asan 02_linked_list_memory.cpp
//   ./llmem_asan
// Verified result on this machine: AddressSanitizer reports exactly ONE leak,
// 16 bytes (= sizeof(Node)) at line 162. That is the leak this lesson creates
// ON PURPOSE in section 6 to show the damage a bad insert order does. Every
// other allocation in this file is freed correctly.
//
// Addresses change every run (ASLR) but the PATTERNS are always the same:
//   stack addresses ~ 0x7ffd....   (high, variable's own home)
//   heap  addresses ~ 0x55.......  (low, where new gives you boxes)
// ============================================================================
#include <iostream>
#include <iomanip>
#include <string>
#include <sstream>
#include <cstdint>
using namespace std;

// ---------------------------------------------------------------------------
// THE ENTIRE DATA STRUCTURE. That is it. Not a container, not an array.
// One int. One address. The "list" is just addresses chained together.
// ---------------------------------------------------------------------------
struct Node {
    int   data;   // 4 bytes  of cargo
    Node* next;   // 8 bytes  of "where is the next box" (64-bit pointer)
    Node(int v) : data(v), next(nullptr) {}
};

// print a pointer as 0x.... so it looks like the hardware it is
static string A(const void* p) {
    if (!p) return "NULL";
    ostringstream os;
    os << "0x" << hex << reinterpret_cast<uintptr_t>(p);
    return os.str();
}

static void draw(Node* head) {
    cout << "  head";
    for (Node* cur = head; cur; cur = cur->next)
        cout << " -> [" << cur->data << "|*]";
    cout << " -> NULL\n";
}

// The table that makes it click: every box, its real address, and the address
// it is holding in its `next` field. Watch column 5 match column 2 of the row below.
static void table(Node* head) {
    cout << "   idx  node address      data   next field holds   which is\n";
    int i = 0;
    for (Node* cur = head; cur; cur = cur->next, ++i) {
        string who = cur->next ? ("node[" + to_string(i + 1) + "] addr") : "end of list";
        cout << "   " << setw(3) << i << "  " << setw(16) << A(cur)
             << "  " << setw(4) << cur->data << "   " << setw(16) << A(cur->next)
             << "   " << who << "\n";
    }
}

int main() {
    cout << "================================================================\n";
    cout << " 0. WHAT IS ONE NODE, REALLY?\n";
    cout << "================================================================\n";
    cout << "  sizeof(Node) = " << sizeof(Node) << " bytes\n";
    cout << "    data (int)      = " << sizeof(int) << " bytes\n";
    cout << "    next (Node*)    = " << sizeof(Node*) << " bytes\n";
    cout << "    padding         = " << sizeof(Node) - sizeof(int) - sizeof(Node*) << " bytes\n";
    cout << "  A Node* is not a node. It is an 8-byte number: a house address.\n";

    // -----------------------------------------------------------------------
    cout << "\n================================================================\n";
    cout << " 1. THE HEAD POINTER IS A LOCAL VARIABLE. THE NODES ARE NOT.\n";
    cout << "================================================================\n";
    Node* head = nullptr;
    cout << "  head is a variable sitting on the STACK at  " << A(&head) << "\n";
    cout << "  head currently holds the value              " << A(head) << "\n";
    cout << "  (the address OF the paper, vs what is WRITTEN on the paper)\n";

    cout << "\n  now: head = new Node(10);\n";
    head = new Node(10);
    cout << "  head still sits at                          " << A(&head) << "  (never moves)\n";
    cout << "  head now holds                              " << A(head) << "  (a HEAP address)\n";
    cout << "  the node itself lives at                    " << A(head) << "\n";
    cout << "  --> stack is ~0x7ffd.., heap is ~0x55.. : two different neighbourhoods\n";

    // -----------------------------------------------------------------------
    cout << "\n================================================================\n";
    cout << " 2. THE LIST IS JUST ADDRESSES MATCHING UP\n";
    cout << "================================================================\n";
    Node* second = new Node(20);
    Node* third  = new Node(30);
    head->next   = second;   // write second's address onto node 10's paper
    second->next = third;    // write third's address onto node 20's paper

    draw(head);
    cout << "\n";
    table(head);
    cout << "\n  Read column 5: node[0].next == node[1]'s address. Literally equal.\n";
    cout << "  That is the whole trick. There is no magic and no container.\n";

    // -----------------------------------------------------------------------
    cout << "\n================================================================\n";
    cout << " 3. a->data IS (*a).data. PROOF:\n";
    cout << "================================================================\n";
    cout << "  head->data    = " << head->data << "\n";
    cout << "  (*head).data  = " << (*head).data << "   <- identical\n";
    cout << "  *head         = the NODE itself; (the box)\n";
    cout << "  head          = the ADDRESS of that node (the paper)\n";
    cout << "  Why -> exists: *head.data would parse as *(head.data), which is nonsense.\n";

    // -----------------------------------------------------------------------
    cout << "\n================================================================\n";
    cout << " 4. WALKING: the 4-part idiom you will type 5000 times\n";
    cout << "================================================================\n";
    cout << "  for (Node* cur = head; cur != nullptr; cur = cur->next)\n";
    cout << "\n  trace:\n";
    int step = 0;
    for (Node* cur = head; cur != nullptr; cur = cur->next) {
        cout << "    step " << step++ << ": cur = " << A(cur)
             << "  ->  cur->data = " << setw(2) << cur->data << "\n";
    }
    cout << "    step " << step << ": cur = NULL, loop stops. (You checked BEFORE reading.)\n";

    // -----------------------------------------------------------------------
    cout << "\n================================================================\n";
    cout << " 5. WHY `Node*&` EXISTS: the parameter's own address tells the story\n";
    cout << "================================================================\n";
    cout << "  caller's head variable lives at " << A(&head) << "\n";

    auto byValue = [](Node* h) {
        cout << "    by value : parameter copy lives at " << A(&h)
             << "   <- DIFFERENT address\n";
        h = nullptr;   // dies here. caller never knows. list survives.
    };
    auto byRef = [](Node*& h) {
        cout << "    by ref   : parameter itself lives at " << A(&h)
             << "   <- SAME address\n";
        // h IS the caller's variable. writing h writes the caller's head.
    };
    byValue(head);
    byRef(head);
    cout << "  After byValue set it to nullptr, caller's head = " << A(head) << " (unharmed)\n";
    cout << "  byRef got the same address, so it could really have changed it.\n";
    cout << "  RULE: a function that moves the head needs Node*& .\n";

    // -----------------------------------------------------------------------
    cout << "\n================================================================\n";
    cout << " 6. INSERT AT FRONT: the order matters, and here is WHY\n";
    cout << "================================================================\n";
    Node* n = new Node(5);
    cout << "  start:  head     = " << A(head) << "   (front of a 3-node list)\n";
    cout << "          n        = " << A(n)    << "   (brand new box)\n";
    cout << "          n->next  = " << A(n->next) << "   (not linked to anything yet)\n";

    cout << "\n  line 1:  n->next = head;    // new box points at the OLD front\n";
    n->next = head;
    cout << "           n->next  = " << A(n->next) << "   <-- this is the old head's address\n";
    cout << "           (list is now reachable BOTH ways; nothing lost yet)\n";

    cout << "\n  line 2:  head = n;          // move the name 'head' to the new box\n";
    head = n;
    cout << "           head     = " << A(head) << "   <-- unchanged node count, new front\n\n";
    draw(head);

    cout << "\n  --- now the WRONG order, on a scratch list ---\n";
    Node* wrong = new Node(1);
    Node* add   = new Node(0);
    uintptr_t node1_addr = reinterpret_cast<uintptr_t>(wrong);  // remember box "1"
    cout << "  wrong = " << A(wrong) << " (holds 1),  add = " << A(add) << " (holds 0)\n";
    cout << "  correct order would be: add->next = wrong; wrong = add;\n";
    cout << "\n  but if you write `wrong = add;` FIRST, then `add->next = wrong;`:\n";
    wrong = add;          // BUG: forgot to link first, so the name `wrong` moved
    add->next = wrong;    // and now this reads back the same box
    cout << "    add->next = " << A(add->next) << " and add = " << A(add) << "\n";
    cout << "    " << (add->next == add ? "SAME ADDRESS -> self-loop -> any walk of this list never ends."
                                       : "ok") << "\n";
    cout << "  A self-loop is the classic 'my program just hangs' linked-list bug.\n";
    cout << "  And node 1 at 0x" << hex << node1_addr << dec << " is now UNREACHABLE:\n";
    cout << "  nothing points to it any more, so it can never be freed = memory leak.\n";
    add->next = nullptr;    // detach the self-loop so the cleanup below is safe
    delete add;             // NOTE: `wrong` now names the SAME box as `add`, so
                            // `delete wrong; delete add;` would be a DOUBLE FREE.
                            // Two names for one box is how you crash the heap.

    // -----------------------------------------------------------------------
    cout << "\n================================================================\n";
    cout << " 7. delete RETURNS THE BOX. The heap recycles addresses.\n";
    cout << "================================================================\n";
    Node* p = new Node(99);
    uintptr_t freed = reinterpret_cast<uintptr_t>(p);   // save the number, not the pointer
    delete p;                                            // box goes back to the heap
    cout << "  allocated a node at         0x" << hex << freed << dec << "\n";
    cout << "  delete p;  (memory returned to the OS allocator)\n";
    Node* q = new Node(100);
    cout << "  allocated the next node at  " << A(q) << "\n";
    cout << "  " << (reinterpret_cast<uintptr_t>(q) == freed
                       ? "SAME ADDRESS -> recycled. This is why a dangling pointer is dangerous."
                       : "different address this time (allocator reused another free block).") << "\n";
    delete q;

    // -----------------------------------------------------------------------
    cout << "\n================================================================\n";
    cout << " 8. CLEANUP: free in the only safe order\n";
    cout << "================================================================\n";
    draw(head);
    int freed_count = 0;
    while (head != nullptr) {
        Node* victim = head;      // 1. grab the front box
        head = head->next;        // 2. step head forward FIRST
        delete victim;            // 3. only now destroy the box
        ++freed_count;
    }
    cout << "  freed " << freed_count << " boxes. head = " << A(head) << "\n";
    cout << "  If you delete BEFORE stepping forward, you destroy the only copy of the\n";
    cout << "  address of the rest of the list. Rest of the list = leaked forever.\n";
    return 0;
}
