// 12_stack_linked_apps.cpp
// ECE2104 (DSA), Lecture 12: Stack using linked list, stack applications.
// Handout session outcome: implement stacks and queues in memory.
//
// Build: g++ -Wall -Wextra -Wpedantic -std=c++17 -O2 -o /tmp/lec12 12_stack_linked_apps.cpp
//
// The linked stack keeps the SAME last-in-first-out rule as lecture 11 and
// throws away the capacity limit, paying for it with one heap node per element
// and one pointer chased per operation. Sections 1 to 5 measure that trade,
// sections 6 to 8 spend it on the two applications named in the lecture.

#include <iostream>
#include <iomanip>
#include <cstddef>
#include <cstdlib>
#include <new>
#include <chrono>
#include <string>
#include <vector>
#include <malloc.h>
using namespace std;

// every heap request this lesson makes goes through heapAlloc, so the counters
// below are counts taken at the source rather than estimates
static size_t g_allocCalls = 0, g_allocBytes = 0, g_freeCalls = 0;
static void *heapAlloc(size_t bytes) {
    g_allocCalls++;
    g_allocBytes += bytes;
    void *p = malloc(bytes);
    if (!p) { cerr << "out of memory\n"; exit(1); }
    return p;
}
static void heapFree(void *p) { g_freeCalls++; free(p); }

struct Node {
    int data;
    Node *next;
};

class LinkedStack {
public:
    LinkedStack() = default;
    LinkedStack(const LinkedStack &) = delete;
    LinkedStack &operator=(const LinkedStack &) = delete;
    ~LinkedStack() { int v = 0; while (pop(v)) {} }

    void push(int value) {                 // new node becomes the head
        Node *n = (Node *)heapAlloc(sizeof(Node));
        n->data = value;
        n->next = head_;
        head_ = n;
        count_++;
    }
    bool pop(int &out) {
        if (!head_) { underflows_++; return false; }
        Node *n = head_;
        out = n->data;
        head_ = n->next;
        heapFree(n);
        count_--;
        return true;
    }
    bool top(int &out) const {
        if (!head_) return false;
        out = head_->data;
        return true;
    }
    bool empty() const { return head_ == nullptr; }
    size_t size() const { return count_; }
    size_t underflows() const { return underflows_; }
    const Node *head() const { return head_; }
private:
    Node *head_ = nullptr;
    size_t count_ = 0;
    size_t underflows_ = 0;
};

// a small array stack, kept here only so the two representations can be timed
// against each other in ONE process on the SAME data
template <size_t CAP>
class ArrayStack {
public:
    bool push(int value) { if (top_ == CAP) return false; data_[top_++] = value; return true; }
    bool pop(int &out) { if (top_ == 0) return false; out = data_[--top_]; return true; }
private:
    int data_[CAP]{};
    size_t top_ = 0;
};

static bool balanced(const string &text, size_t &failPos, string &why, string *trace) {
    vector<pair<char, size_t>> st;                 // char + its index in text
    for (size_t i = 0; i < text.size(); i++) {
        char c = text[i];
        if (c == '(' || c == '[' || c == '{') {
            st.push_back({c, i});
            if (trace) *trace += "    push " + string(1, c) + " at " + to_string(i) + "  depth=" + to_string(st.size()) + "\n";
        } else if (c == ')' || c == ']' || c == '}') {
            char want = (c == ')') ? '(' : (c == ']') ? '[' : '{';
            if (st.empty()) {
                failPos = i; why = "closing " + string(1, c) + " with nothing open";
                if (trace) *trace += "    hit " + string(1, c) + " at " + to_string(i) + " with an EMPTY stack -> fail\n";
                return false;
            }
            if (st.back().first != want) {
                failPos = i;
                why = "expected " + string(1, want) + " but found " + string(1, c);
                if (trace) *trace += "    hit " + string(1, c) + " at " + to_string(i) + " while top is "
                                     + string(1, st.back().first) + " -> fail\n";
                return false;
            }
            if (trace) *trace += "    match " + string(1, c) + " at " + to_string(i)
                                 + " with " + string(1, st.back().first) + " from " + to_string(st.back().second)
                                 + "  depth=" + to_string(st.size() - 1) + "\n";
            st.pop_back();
        }
    }
    if (!st.empty()) {
        failPos = st.back().second;
        why = "never closed, the deepest unmatched opener is " + string(1, st.back().first);
        if (trace) *trace += "    end of input with depth " + to_string(st.size()) + " -> fail at " + to_string(failPos) + "\n";
        return false;
    }
    failPos = string::npos;
    why = "all pairs matched";
    if (trace) *trace += "    end of input with depth 0 -> balanced\n";
    return true;
}

static string reverseWithStack(const string &in) {
    LinkedStack st;
    for (char c : in) st.push((unsigned char)c);
    string out;
    int ch = 0;
    while (st.pop(ch)) out.push_back((char)ch);
    return out;
}

int main() {
    cout << "=== 1. the node and the stack in bytes ===\n";
    cout << "  sizeof(Node{int data; Node *next;}) = " << sizeof(Node)
         << "  offsetof(data)=" << offsetof(Node, data)
         << " offsetof(next)=" << offsetof(Node, next) << "\n";
    cout << "  4 payload bytes + 4 padding bytes + 8 pointer bytes = 16, the padding exists so the\n";
    cout << "  pointer sits on an 8 byte boundary (lecture 5 measured the same rule)\n";
    cout << "  sizeof(LinkedStack) = " << sizeof(LinkedStack) << " bytes: one pointer, one count, one counter\n";
    cout << "  so the stack object itself is constant size and every element lives on the heap\n";

    cout << "\n=== 2. push log: the newest node becomes the head ===\n";
    LinkedStack s;
    for (int value : {10, 20, 30}) {
        s.push(value);
        cout << "  push(" << setw(2) << value << ")  new node at " << (const void *)s.head()
             << " -> head_ now points here, head_->data=" << s.head()->data
             << ", head_->next=" << (const void *)s.head()->next << ", size=" << s.size() << "\n";
    }
    cout << "  the head moved every time: LIFO is head insertion, nothing is traversed to push\n";

    cout << "\n=== 3. pop log and the real underflow failure ===\n";
    int v = 0;
    while (s.pop(v)) cout << "  pop() -> " << v << "  size=" << s.size()
                          << "  head=" << (const void *)s.head() << "\n";
    bool emptyPop = s.pop(v);
    cout << "  pop() on an empty stack returned " << (emptyPop ? "a value" : "false")
         << ", underflow counter = " << s.underflows() << " (the drain loop's last attempt plus this one)\n";
    cout << "  a linked stack cannot overflow, it can only underflow: the two failure modes are not symmetric\n";

    cout << "\n=== 4. no capacity limit: 200,000 pushes ===\n";
    const size_t MANY = 200000;
    g_allocCalls = 0; g_allocBytes = 0;
    LinkedStack big;
    for (size_t i = 0; i < MANY; i++) big.push((int)i);
    size_t calls = g_allocCalls, requested = g_allocBytes;
    size_t usableNode = malloc_usable_size((void *)big.head());
    size_t chargedPerNode = (size_t)labs((long)((const char *)big.head() - (const char *)big.head()->next));
    cout << "  pushed " << big.size() << " elements with " << calls << " heap requests requesting "
         << requested << " bytes, which is " << (requested / MANY) << " bytes per element\n";
    cout << "  the allocator hands out " << usableNode << " usable bytes per node and charges a "
         << chargedPerNode << " byte stride, so the real footprint is " << (chargedPerNode * MANY)
         << " bytes for the " << requested << " bytes asked for\n";
    cout << "  no full() check exists anywhere: the limit is the machine's memory, not the container\n";
    g_allocCalls = 0; g_allocBytes = 0; g_freeCalls = 0;
    int popped = 0;
    while (big.pop(popped)) {}
    cout << "  draining " << MANY << " nodes made " << g_freeCalls
         << " free calls, underflows = " << big.underflows() << "\n";

    cout << "\n=== 5. the trade in one table, measured ===\n";
    cout << "  " << left << setw(26) << "property" << setw(26) << "array stack (lecture 11)" << "linked stack\n";
    cout << "  " << setw(26) << "heap calls per element" << setw(26) << "0" << "1\n";
    cout << "  " << setw(26) << "bytes charged per element" << setw(26) << sizeof(int) << chargedPerNode << "\n";
    cout << "  " << setw(26) << "capacity limit" << setw(26) << "fixed at compile time" << "none\n";
    cout << "  " << setw(26) << "overflow possible" << setw(26) << "yes, detected" << "no\n";
    cout << "  " << setw(26) << "underflow possible" << setw(26) << "yes, detected" << "yes, detected\n";
    cout << "  " << setw(26) << "push/pop work" << setw(26) << "O(1)" << "O(1)\n";
    cout << "  " << setw(26) << "locality" << setw(26) << "contiguous, cache friendly" << " " << "scattered, pointer chase\n";
    cout << right;

    cout << "\n=== 6. application 1: balanced bracket checker ===\n";
    const vector<string> cases = {"(a[b]{c})", "([)]", "(((", ")(", "{[()]}", ""};
    for (const string &c : cases) {
        size_t failPos = 0;
        string why;
        bool ok = balanced(c, failPos, why, nullptr);
        cout << "  \"" << c << "\"  -> " << (ok ? "BALANCED" : "NOT BALANCED")
             << "  (" << why;
        if (!ok) cout << " at index " << failPos;
        cout << ")\n";
    }
    cout << "  the stack is what makes this O(n): an opened bracket is pushed, a closing bracket must\n";
    cout << "  match the TOP. A counter would pass \"([)]\", a stack catches it at index 2. Trace of that case:\n";
    size_t fp = 0;
    string why, trace;
    balanced("([)]", fp, why, &trace);
    cout << trace;

    cout << "\n=== 7. application 2: reverse a string with the stack ===\n";
    for (const char *c : {"DSA", "ECE2104", "level"}) {
        string text(c);
        string r = reverseWithStack(text);
        cout << "  \"" << text << "\" -> \"" << r << "\"   palindrome=" << (text == r ? "yes" : "no") << "\n";
    }
    cout << "  reversing is push everything then pop everything: the LIFO rule IS the reversal\n";

    cout << "\n=== 8. cost of the two representations, timed in this one process ===\n";
    const size_t N = 1000000;
    long long sumL = 0;
    g_allocCalls = 0; g_allocBytes = 0;
    LinkedStack ls;
    auto t0 = chrono::steady_clock::now();
    for (size_t i = 0; i < N; i++) ls.push((int)i);
    auto t1 = chrono::steady_clock::now();
    for (size_t i = 0; i < N; i++) { ls.pop(popped); sumL += popped; }
    auto t2 = chrono::steady_clock::now();
    size_t linkedCalls = g_allocCalls;
    double nsLinkedPush = (double)chrono::duration_cast<chrono::nanoseconds>(t1 - t0).count() / (double)N;
    double nsLinkedPop = (double)chrono::duration_cast<chrono::nanoseconds>(t2 - t1).count() / (double)N;

    g_allocCalls = 0; g_allocBytes = 0;
    ArrayStack<1000000> *as = new (heapAlloc(sizeof(ArrayStack<1000000>))) ArrayStack<1000000>();
    size_t arrayCalls = g_allocCalls;
    size_t arrayBytes = g_allocBytes;
    auto t3 = chrono::steady_clock::now();
    for (size_t i = 0; i < N; i++) as->push((int)i);
    auto t4 = chrono::steady_clock::now();
    long long sumA = 0;
    for (size_t i = 0; i < N; i++) { as->pop(popped); sumA += popped; }
    auto t5 = chrono::steady_clock::now();
    double nsArrayPush = (double)chrono::duration_cast<chrono::nanoseconds>(t4 - t3).count() / (double)N;
    double nsArrayPop = (double)chrono::duration_cast<chrono::nanoseconds>(t5 - t4).count() / (double)N;
    as->~ArrayStack<1000000>();
    heapFree(as);

    long long expected = (long long)N * ((long long)N - 1) / 2;
    cout << "  order verified by checksum: linked " << (sumL == expected ? "ok" : "MISMATCH")
         << ", array " << (sumA == expected ? "ok" : "MISMATCH")
         << " (the checksum is what stops the compiler deleting the loops)\n";
    cout << fixed << setprecision(3);
    cout << "  linked stack: push " << nsLinkedPush << " ns, pop " << nsLinkedPop << " ns, "
         << linkedCalls << " heap requests for 1,000,000 elements\n";
    cout << "  array stack : push " << nsArrayPush << " ns, pop " << nsArrayPop << " ns, "
         << arrayCalls << " heap request of " << arrayBytes << " bytes, storage " << sizeof(int) * N << " bytes\n";
    cout << "  the linked stack pays " << setprecision(1) << (nsLinkedPop / nsArrayPop)
         << "x more per pop, and that is the allocator: the array stack never asked it\n";
    cout << "  for anything after the single block was reserved\n";
    cout << "  what it bought: no ceiling. Choose the array when the bound is known, the links when it is not.\n";

    cout << "\n=== 9. the one line to remember ===\n";
    cout << "  a linked stack is head insertion plus head removal; the top of the stack is the head pointer,\n";
    cout << "  which is why push and pop are O(1) with no shifting and why the stack has no capacity\n";
    return 0;
}
