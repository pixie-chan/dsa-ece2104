// 11_stack_array.cpp
// ECE2104 (DSA), Lecture 11: Stack, concept and array representation.
// Handout session outcome: implement stacks and queues in memory.
//
// Build: g++ -Wall -Wextra -Wpedantic -std=c++17 -O2 -o /tmp/lec11 11_stack_array.cpp
//
// A stack is an array plus ONE integer. That integer is the whole data
// structure: it says where the next push writes and which element pop returns.
// LIFO is not a property of the array, it is a property of using ONE end for
// both operations, and section 6b measures what happens when you use the
// other end instead. Every number below is measured on this machine.

#include <iostream>
#include <iomanip>
#include <cstddef>
#include <cstdlib>
#include <new>
#include <chrono>
#include <string>
#include <malloc.h>
using namespace std;

// ---- every heap request this lesson makes goes through heapAlloc, so "the
// array stack allocates nothing" is a count taken at the source, not a claim ----
static size_t g_allocCalls = 0, g_allocBytes = 0;
static void *heapAlloc(size_t bytes) {
    g_allocCalls++;
    g_allocBytes += bytes;
    void *p = malloc(bytes);
    if (!p) { cerr << "out of memory\n"; exit(1); }
    return p;
}
static void heapFree(void *p) { free(p); }

template <typename T, size_t CAP>
class ArrayStack {
public:
    bool push(const T &value) {
        if (full()) { refusals_++; return false; }
        data_[top_] = value;
        top_++;
        return true;
    }
    bool pop(T &out) {
        if (empty()) return false;
        top_--;
        out = data_[top_];
        return true;
    }
    bool top(T &out) const {
        if (empty()) return false;
        out = data_[top_ - 1];
        return true;
    }
    bool empty() const { return top_ == 0; }
    bool full() const { return top_ == CAP; }
    size_t size() const { return top_; }
    size_t refusals() const { return refusals_; }
    static size_t capacity() { return CAP; }
    const T *base() const { return data_; }
    const T &raw(size_t i) const { return data_[i]; }
private:
    T data_[CAP]{};
    size_t top_ = 0;
    size_t refusals_ = 0;
};

// identical layout, named fields only so offsetof can be printed
struct ArrayStackMirror {
    int data[5];
    size_t top;
    size_t refusals;
};

// the bench that shows where an unchecked push would land
struct CanaryBench {
    int data[6];
    unsigned long canary;
};

static void dump(const ArrayStack<int, 5> &s, const char *label) {
    cout << "  " << label << " bottom -> top:";
    if (s.size() == 0) cout << " (empty)";
    for (size_t i = 0; i < s.size(); i++) cout << " " << s.raw(i);
    cout << "   size=" << s.size() << "\n";
}

int main() {
    cout << "=== 1. the whole data structure in bytes ===\n";
    cout << "  sizeof(int)=" << sizeof(int) << " sizeof(size_t)=" << sizeof(size_t) << "\n";
    cout << "  sizeof(ArrayStack<int,5>)  = " << sizeof(ArrayStack<int, 5>) << "\n";
    cout << "  sizeof(ArrayStack<char,5>) = " << sizeof(ArrayStack<char, 5>) << "\n";
    cout << "  the 5 ints alone           = " << 5 * sizeof(int) << " bytes\n";
    cout << "  the bookkeeping            = " << sizeof(size_t) * 2 << " bytes (top index + refusal counter)\n";
    cout << "  offsetof(data)    = " << offsetof(ArrayStackMirror, data)    << "\n";
    cout << "  offsetof(top)     = " << offsetof(ArrayStackMirror, top)     << "\n";
    cout << "  offsetof(refusals)= " << offsetof(ArrayStackMirror, refusals) << "\n";
    cout << "  offsetof(top) = " << offsetof(ArrayStackMirror, top)
         << ", not 20: size_t wants an 8 byte boundary, so 4 bytes of padding sit after the 5 ints\n";
    cout << "  the array comes first and the index trails it, which is why the base address never moves\n";

    cout << "\n=== 2. push and pop operation log (LIFO proof) ===\n";
    ArrayStack<int, 5> s;
    int v = 0;
    for (int value : {10, 20, 30}) {
        bool ok = s.push(value);
        cout << "  push(" << setw(2) << value << ") -> " << (ok ? "ok  " : "REFUSED")
             << "  topIndex=" << s.size() << " content:";
        for (size_t i = 0; i < s.size(); i++) cout << " " << s.raw(i);
        cout << "\n";
    }
    cout << "  inputs were 10 20 30, pops come out in the reverse order:\n";
    while (s.pop(v)) cout << "  pop() -> " << v << "   remaining topIndex=" << s.size() << "\n";
    cout << "  after the last pop: empty=" << (s.empty() ? "yes" : "no")
         << ", pop on empty now returns " << (s.pop(v) ? "a value" : "false (underflow)") << "\n";

    cout << "\n=== 3. the three operations in one line each ===\n";
    cout << "  push(x) : data[top] = x; top++      both O(1), nothing shifts, nothing allocates\n";
    cout << "  pop()   : top--; return data[top]   O(1)\n";
    cout << "  top()   : return data[top - 1]      O(1), the stack is not modified\n";

    cout << "\n=== 4. the capacity limit is real: 7 pushes into a 5 slot stack ===\n";
    ArrayStack<int, 5> full5;
    for (int i = 1; i <= 7; i++) {
        bool ok = full5.push(i);
        cout << "  push(" << i << ") -> " << (ok ? "stored" : "REFUSED (full)")
             << "   size=" << full5.size() << " full=" << (full5.full() ? "yes" : "no")
             << " refusals=" << full5.refusals() << "\n";
    }
    dump(full5, "after the refusals the contents are untouched:");
    cout << "  refused pushes = " << full5.refusals()
         << ", capacity() = " << ArrayStack<int, 5>::capacity()
         << ", overflow is DETECTED, not silently written\n";
    int discarded = 0;
    cout << "  drain: ";
    while (full5.pop(discarded)) cout << discarded << " ";
    cout << "\n";

    cout << "\n=== 5. where an unchecked push would write (address arithmetic) ===\n";
    CanaryBench bench{};
    bench.canary = 0xC0FFEEULL;
    const char *onePast = (const char *)bench.data + 6 * sizeof(int);
    const char *canaryAddr = (const char *)&bench.canary;
    cout << "  bench.data base            = " << (const void *)bench.data << "\n";
    cout << "  bench.data + 6*sizeof(int) = " << (const void *)onePast << "\n";
    cout << "  address of bench.canary    = " << (const void *)canaryAddr << "\n";
    cout << "  the unchecked slot 6 IS the canary slot: " << (onePast == canaryAddr ? "yes" : "no") << "\n";
    cout << "  canary before = 0x" << hex << bench.canary << dec
         << " (that is the byte a stack with no full() check would overwrite)\n";
    cout << "  offsetof(CanaryBench,canary) = " << offsetof(CanaryBench, canary)
         << " bytes, exactly 6 ints in\n";

    cout << "\n=== 6. cost: 2,000,000 pushes then 2,000,000 pops ===\n";
    const size_t BIG = 2000000;
    g_allocCalls = 0; g_allocBytes = 0;
    ArrayStack<int, BIG> *big = new (heapAlloc(sizeof(ArrayStack<int, BIG>))) ArrayStack<int, BIG>();
    cout << "  the container itself asked the heap for " << g_allocCalls << " block of "
         << g_allocBytes << " bytes for a " << sizeof(ArrayStack<int, BIG>) << " byte object\n";
    g_allocCalls = 0; g_allocBytes = 0;
    auto t0 = chrono::steady_clock::now();
    for (size_t i = 0; i < BIG; i++) big->push((int)i);
    auto t1 = chrono::steady_clock::now();
    long long sum = 0;
    int x = 0;
    auto t2 = chrono::steady_clock::now();
    for (size_t i = 0; i < BIG; i++) { big->pop(x); sum += x; }
    auto t3 = chrono::steady_clock::now();
    double nsPush = (double)chrono::duration_cast<chrono::nanoseconds>(t1 - t0).count() / (double)BIG;
    double nsPop = (double)chrono::duration_cast<chrono::nanoseconds>(t3 - t2).count() / (double)BIG;
    long long expected = (long long)BIG * ((long long)BIG - 1) / 2;
    cout << "  the 2,000,000 pushes and 2,000,000 pops asked the heap for " << g_allocCalls
         << " further blocks and " << g_allocBytes << " further bytes\n";
    cout << "  the popped values sum to " << sum << " against " << expected
         << " if every value came back in reverse: " << (sum == expected ? "confirmed" : "MISMATCH") << "\n";
    cout << fixed << setprecision(3);
    cout << "  per operation: push " << nsPush << " ns, pop " << nsPop << " ns\n";
    cout << "  refusals at the end = " << big->refusals() << "\n";
    big->~ArrayStack<int, BIG>();
    heapFree(big);

    cout << "\n=== 6b. why the TOP end and not the BOTTOM end ===\n";
    const size_t SMALL = 20000;
    static int shiftBuf[SMALL];
    volatile long long sink = 0;
    auto t4 = chrono::steady_clock::now();
    size_t used = 0;
    for (size_t i = 0; i < SMALL; i++) {                    // push at index 0, every element moves
        for (size_t k = used; k > 0; k--) shiftBuf[k] = shiftBuf[k - 1];
        shiftBuf[0] = (int)i;
        used++;
    }
    auto t5 = chrono::steady_clock::now();
    for (size_t i = 0; i < used; i++) sink += shiftBuf[i];   // the sink is what keeps the work alive
    ArrayStack<int, SMALL> topEnd;
    auto t6 = chrono::steady_clock::now();
    for (size_t i = 0; i < SMALL; i++) topEnd.push((int)i);
    auto t7 = chrono::steady_clock::now();
    while (topEnd.pop(x)) sink += x;
    double nsShift = (double)chrono::duration_cast<chrono::nanoseconds>(t5 - t4).count() / (double)SMALL;
    double nsTop = (double)chrono::duration_cast<chrono::nanoseconds>(t7 - t6).count() / (double)SMALL;
    cout << "  bottom end (every push shifts the whole stack): " << setprecision(1) << fixed << nsShift << " ns per push\n";
    cout << "  top end    (nothing shifts)                    : " << nsTop << " ns per push\n";
    cout << "  the bottom end costs " << (nsShift / nsTop) << "x more for the SAME 20,000 pushes\n";
    cout << "  both loops end in a checksum (" << sink << ") so the compiler cannot delete the work\n";
    cout << "  (the gap grows with n, which is the whole argument for choosing the end)\n";

    cout << "\n=== 7. the one line to remember ===\n";
    cout << "  a stack is an array plus one index; push stores at the index and moves it,\n";
    cout << "  pop moves it back and returns the slot. O(1) both, zero allocation after the array exists.\n";
    return 0;
}
