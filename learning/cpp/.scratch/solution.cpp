// ============================================================================
// LESSON 3: EXERCISE FILE. You write the 6 functions. The test main checks you.
//
// COMPILE + RUN: g++ -Wall -Wextra -o llex 03_linked_list_exercise.cpp && ./llex
//
// Right now every test FAILS, because each function is a stub that does nothing.
// Your job: fill in the 6 bodies marked TODO. Do not touch the test code below
// the line that says "STOP READING HERE".
//
// Order to do them in: 1, 2, 3 (easy warm-ups) then 4, 5, 6 (the real ones).
// After each one, recompile and watch that test flip to PASS.
// ============================================================================
#include <iostream>
#include <string>
#include <vector>
using namespace std;

struct Node {
    int   data;
    Node* next;
    Node(int v) : data(v), next(nullptr) {}
};

// ---------------------------------------------------------------------------
// TODO 1: return how many boxes are in the list. Empty list -> 0.
//   Walk with: for (Node* cur = head; cur != nullptr; cur = cur->next)
// ---------------------------------------------------------------------------
int length(Node* head) {
    int n = 0;
    for (Node* cur = head; cur != nullptr; cur = cur->next) ++n;
    return n;
}

// ---------------------------------------------------------------------------
// TODO 2: put value v at the FRONT. Must work on an EMPTY list too.
//   Why Node*& ? Because this function has to change the caller's head.
//   3 lines: make the box, point it at the old head, move head onto it.
// ---------------------------------------------------------------------------
void pushFront(Node*& head, int v) {
    Node* n = new Node(v);
    n->next = head;
    head = n;
}

// ---------------------------------------------------------------------------
// TODO 3: put value v at the END. Must work on an EMPTY list too.
//   Empty list is the case that breaks people: if head == nullptr there is no
//   "last node" to walk to, so handle it before you start walking.
// ---------------------------------------------------------------------------
void pushBack(Node*& head, int v) {
    Node* n = new Node(v);
    if (head == nullptr) { head = n; return; }
    Node* cur = head;
    while (cur->next != nullptr) cur = cur->next;
    cur->next = n;
}

// ---------------------------------------------------------------------------
// TODO 4: return true if any box holds v, else false.
// ---------------------------------------------------------------------------
bool contains(Node* head, int v) {
    for (Node* cur = head; cur != nullptr; cur = cur->next)
        if (cur->data == v) return true;
    return false;
}

// ---------------------------------------------------------------------------
// TODO 5: delete the FIRST box holding v. Do nothing if v is absent.
//   This is the one that teaches you why deletion needs TWO pointers:
//   to unlink a box you must edit the box BEFORE it, so you keep `prev`.
//   Special case: if the box to delete is the head, there is no prev, so you
//   move head forward instead.
// ---------------------------------------------------------------------------
void removeValue(Node*& head, int v) {
    if (head == nullptr) return;
    if (head->data == v) { Node* old = head; head = head->next; delete old; return; }
    Node* prev = head;
    while (prev->next != nullptr && prev->next->data != v) prev = prev->next;
    if (prev->next == nullptr) return;
    Node* old = prev->next;
    prev->next = old->next;
    delete old;
}

// ---------------------------------------------------------------------------
// TODO 6: reverse the list IN PLACE. Do NOT build a new list, do NOT use a
//   vector. Three pointers, walk once. This is THE classic interview question.
//   Sketch:
//     prev = nullptr
//     cur  = head
//     while cur:
//         nxt      = cur->next   // save the rest before you break the link
//         cur->next = prev       // flip this box's arrow backwards
//         prev     = cur         // prev catches up
//         cur      = nxt         // advance
//     head = prev
//   The order of those 4 lines is the entire difficulty. If you get it wrong
//   you lose the rest of the list. Draw the 3 boxes on paper first.
// ---------------------------------------------------------------------------
void reverse(Node*& head) {
    Node* prev = nullptr;
    Node* cur  = head;
    while (cur != nullptr) {
        Node* nxt = cur->next;
        cur->next = prev;
        prev = cur;
        cur = nxt;
    }
    head = prev;
}

// ============================================================================
// STOP READING HERE. Test harness below, do not modify.
// ============================================================================
static int passed = 0, failed = 0;

static void check(bool ok, const string& what) {
    cout << (ok ? "  PASS  " : "  FAIL  ") << what << "\n";
    ok ? ++passed : ++failed;
}

static string show(Node* head) {
    string s = "head";
    for (Node* c = head; c; c = c->next) s += " -> " + to_string(c->data);
    return s + " -> NULL";
}

static Node* build(const vector<int>& vals) {
    Node* h = nullptr;
    for (int i = (int)vals.size() - 1; i >= 0; --i) {
        Node* n = new Node(vals[i]);
        n->next = h;
        h = n;
    }
    return h;
}

static void freeAll(Node*& head) {
    while (head) { Node* t = head; head = head->next; delete t; }
}

int main() {
    cout << "--- 1. length ---\n";
    Node* a = build({10, 20, 30});
    check(length(a) == 3,  "length of 3-node list == 3");
    Node* empty = nullptr;
    check(length(empty) == 0, "length of empty list == 0");
    freeAll(a);

    cout << "\n--- 2. pushFront ---\n";
    Node* b = nullptr;
    pushFront(b, 30);
    check(show(b) == "head -> 30 -> NULL", "pushFront onto EMPTY list gives 1 node");
    pushFront(b, 20);
    pushFront(b, 10);
    check(show(b) == "head -> 10 -> 20 -> 30 -> NULL", "three pushFronts read 10,20,30");
    check(length(b) == 3, "length agrees after 3 pushFronts");
    freeAll(b);

    cout << "\n--- 3. pushBack ---\n";
    Node* c = nullptr;
    pushBack(c, 10);
    check(show(c) == "head -> 10 -> NULL", "pushBack onto EMPTY list gives 1 node");
    pushBack(c, 20);
    pushBack(c, 30);
    check(show(c) == "head -> 10 -> 20 -> 30 -> NULL", "pushBack keeps order 10,20,30");
    freeAll(c);

    cout << "\n--- 4. contains ---\n";
    Node* d = build({5, 6, 7});
    check(contains(d, 5)  == true,  "finds the head value");
    check(contains(d, 7)  == true,  "finds the tail value");
    check(contains(d, 6)  == true,  "finds a middle value");
    check(contains(d, 99) == false, "reports false for an absent value");
    check(contains(nullptr, 1) == false, "reports false on an empty list");
    freeAll(d);

    cout << "\n--- 5. removeValue ---\n";
    Node* e = build({1, 2, 3});
    removeValue(e, 2);
    check(show(e) == "head -> 1 -> 3 -> NULL", "removes a middle node");
    removeValue(e, 1);
    check(show(e) == "head -> 3 -> NULL", "removes the HEAD node (no prev exists)");
    removeValue(e, 3);
    check(show(e) == "head -> NULL", "removes the last remaining node");
    removeValue(e, 42);
    check(show(e) == "head -> NULL", "removing an absent value changes nothing");
    Node* e2 = build({9, 9, 9});
    removeValue(e2, 9);
    check(show(e2) == "head -> 9 -> 9 -> NULL", "removes only the FIRST match");
    freeAll(e); freeAll(e2);

    cout << "\n--- 6. reverse ---\n";
    Node* f = build({1, 2, 3, 4});
    reverse(f);
    check(show(f) == "head -> 4 -> 3 -> 2 -> 1 -> NULL", "reverses a 4-node list");
    reverse(f);
    check(show(f) == "head -> 1 -> 2 -> 3 -> 4 -> NULL", "reversing twice restores it");
    Node* g = build({7});
    reverse(g);
    check(show(g) == "head -> 7 -> NULL", "reverses a 1-node list");
    Node* h = nullptr;
    reverse(h);
    check(h == nullptr, "reverses an empty list without crashing");
    freeAll(f); freeAll(g);

    cout << "\n================================\n";
    cout << "  passed " << passed << ", failed " << failed << "\n";
    if (failed == 0) cout << "  all green. Now run it under -fsanitize=address to check for leaks.\n";
    cout << "================================\n";
    return failed == 0 ? 0 : 1;
}
