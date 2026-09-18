// ============================================================================
// LINKED LIST FROM SCRATCH, part 2: THE TWO TRAPS (real, runnable, proven)
// compile + run:  g++ -Wall -Wextra -o /tmp/ll2 kiki_ll_trap.cpp && /tmp/ll2
// leak proof:     g++ -Wall -Wextra -g -fsanitize=address -o /tmp/ll2a kiki_ll_trap.cpp && /tmp/ll2a
// ============================================================================
#include <iostream>
using namespace std;

struct Node {
    int data;
    Node* next;
    Node(int v) : data(v), next(nullptr) {}
};

// ---------- TRAP A: wrong order -> the box points at itself -----------------
void pushFrontWrong(Node*& head, int v) {
    Node* n = new Node(v);
    head = n;            // head flag moved FIRST
    n->next = head;      // ...so this reads n->next = n. Ouroboros.
}

// ---------- TRAP B: pointer by value -> lost update + leaked box ------------
void pushFrontByValue(Node* head, int v) {   // no & : this is a photocopy
    Node* n = new Node(v);
    n->next = head;
    head = n;            // local copy changes, caller's head never moves
}                        // n is now unreachable = memory leak

// walk with a hop cap, otherwise trap A loops forever and you have to kill it
void printCapped(Node* head, int cap) {
    int hops = 0;
    Node* cur = head;
    while (cur != nullptr && hops < cap) { cout << cur->data << " -> "; cur = cur->next; ++hops; }
    cout << (cur == nullptr ? "nullptr" : "(... still going, never reached nullptr)") << endl;
    cout << "hops walked = " << hops << endl;
}

int main() {
    cout << "=== TRAP A: head moved before the coupling ===" << endl;
    Node* h = nullptr;
    pushFrontWrong(h, 42);
    cout << "what the list looks like: ";
    printCapped(h, 8);
    cout << "h->next == h ? " << (h->next == h ? "YES, it is a circle" : "no") << endl;
    delete h;                    // this one box is still freeable: nothing else points at it
    cout << "(one delete frees the box, but that arrow inside pointed at its own address)" << endl;

    cout << "\n=== TRAP B: Node* passed by value ===" << endl;
    Node* h2 = nullptr;
    pushFrontByValue(h2, 7);
    cout << "caller's head after the call = " << h2 << "   (still nullptr)" << endl;
    cout << "the box holding 7 exists but nothing points at it: 16 bytes lost" << endl;
    return 0;
}
