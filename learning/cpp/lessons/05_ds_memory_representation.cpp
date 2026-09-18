// 05_ds_memory_representation.cpp
// ECE2104 (DSA), Lecture 5: Introduction to Data Structures and Memory Representation
// Session outcome: define basic data structures and explain their memory allocation.
//
// Every number this program prints is real for the machine it runs on.
// Build: g++ -Wall -Wextra -std=c++17 -O2 -o /tmp/lec05 05_ds_memory_representation.cpp
//
// The point of the file: memory is one long lane of numbered bytes, and every
// data structure is a different way of arranging data along that lane. The
// arrangement decides the cost of every operation. That claim is printed with
// measurements, not asserted.

#include <iostream>
#include <iomanip>
#include <cstddef>
#include <chrono>
#include <string>
#include <malloc.h>
#include <cstdint>
using namespace std;

// ---------- the node of lecture 6, needed here to talk about "composite" ----------
struct Node { int data; Node* next; };

// ---------- three structs that differ only in FIELD ORDER ----------
struct P1 { int a; char b; char c; };   // wide field first
struct P2 { char a; int b; char c; };   // wide field in the middle: worst case
struct P3 { char a; char b; int c; };   // narrow fields first, then wide

static int  g_global = 7;               // lives in the data segment, not on the stack

// four nested stack frames, so we can watch the stack grow DOWNWARD.
// Each address is printed WHILE that frame is still alive, which is the only
// way to talk about it without undefined behaviour (returning &local or storing
// it in a global is a real bug, and -Wall says so: that is why this prints here).
__attribute__((noinline)) static void frame_probe(int depth) {
    volatile int local = depth;
    cout << "    frame depth " << depth << "  &local = " << (void*)&local
         << (depth == 0 ? "   <- innermost, LOWEST address\n" : "\n");
    if (depth > 0) frame_probe(depth - 1);
}

int main() {
    cout << "=== 1. sizes and alignment (bytes) ===\n";
    cout << "sizeof(char)="        << sizeof(char)
         << " sizeof(short)="      << sizeof(short)
         << " sizeof(int)="        << sizeof(int)
         << " sizeof(long)="       << sizeof(long)
         << " sizeof(float)="      << sizeof(float)
         << " sizeof(double)="     << sizeof(double)
         << " sizeof(void*)="      << sizeof(void*) << "\n";
    cout << "alignof(int)="        << alignof(int)
         << " alignof(double)="    << alignof(double)
         << " alignof(void*)="     << alignof(void*) << "\n";
    cout << "sizeof(Node)="        << sizeof(Node)
         << " alignof(Node)="      << alignof(Node) << "\n";

    cout << "\n=== 2. where each kind of object lives (real addresses) ===\n";
    int    stackLocal = 42;
    int   *heapOne    = new int(99);
    unsigned long long gap = (unsigned long long)(char*)&stackLocal
                           - (unsigned long long)(char*)heapOne;
    cout << "global (data segment) &g_global   = " << (void*)&g_global << "\n";
    cout << "stack  (local frame)  &stackLocal= " << (void*)&stackLocal << "\n";
    cout << "heap   (one new int)  heapOne     = " << (void*)heapOne << "\n";
    cout << "four nested calls, innermost frame LAST and LOWEST:\n";
    frame_probe(3);
    cout << "stack sits " << (gap >> 30) << " GiB above the heap in this run:\n";
    cout << "  they are two ends of the same address space, not one block.\n";

    cout << "\n=== 3. an array is contiguous: the address stride equals sizeof(T) ===\n";
    int a[5] = {10, 20, 30, 40, 50};
    cout << "base &a[0] = " << (void*)&a[0] << "\n";
    for (int i = 0; i < 5; i++) {
        cout << "  i=" << i
             << "  &a[" << i << "]=" << (void*)&a[i]
             << "  byte offset=" << ((char*)&a[i] - (char*)a)
             << "  value=" << a[i] << "\n";
    }
    cout << "index 4 in ONE step (base + 4*sizeof(int)) = "
         << (void*)((char*)a + 4 * sizeof(int)) << "  == &a[4] ? "
         << (((char*)a + 4 * sizeof(int)) == (char*)&a[4] ? "yes" : "no") << "\n";

    cout << "\n=== 3b. the same array seen as raw bytes (x86-64 is little-endian) ===\n";
    unsigned char *raw = (unsigned char*)a;
    for (int i = 0; i < 20; i++) {
        cout << "   a[0]+" << setw(2) << setfill(' ') << i << "  byte "
             << hex << setw(2) << setfill('0') << (int)raw[i] << dec << setfill(' ');
        if (i % 4 == 3) {
            cout << "   <- the 4 bytes above are a[" << (i / 4) << "] = " << a[i / 4];
        }
        cout << "\n";
    }
    double d = 3.5;
    cout << "   a double lives at " << (void*)&d << ", divisible by 8? "
         << (((uintptr_t)&d % 8 == 0) ? "yes (alignment rule)" : "no") << "\n";

    cout << "\n=== 4. a string is just bytes at an address ===\n";

    char s[] = "DSA";
    cout << "sizeof(s)=" << sizeof(s) << " (3 characters + 1 terminator)\n";
    for (size_t i = 0; i < sizeof(s); i++)
        cout << "  s[" << i << "] at " << (void*)&s[i] << "  hex="
             << uppercase << hex << setw(2) << setfill('0') << (int)(unsigned char)s[i]
             << dec << setfill(' ') << "\n";

    cout << "\n=== 5. padding: same fields, different order, different footprint ===\n";
    cout << "P1 {int a; char b; char c;}   sizeof=" << sizeof(P1)
         << "  offsetof(a)=" << offsetof(P1, a)
         << " offsetof(b)=" << offsetof(P1, b)
         << " offsetof(c)=" << offsetof(P1, c) << "\n";
    cout << "P2 {char a; int b; char c;}   sizeof=" << sizeof(P2)
         << "  offsetof(a)=" << offsetof(P2, a)
         << " offsetof(b)=" << offsetof(P2, b)
         << " offsetof(c)=" << offsetof(P2, c) << "\n";
    cout << "P3 {char a; char b; int c;}   sizeof=" << sizeof(P3)
         << "  offsetof(a)=" << offsetof(P3, a)
         << " offsetof(b)=" << offsetof(P3, b)
         << " offsetof(c)=" << offsetof(P3, c) << "\n";
    cout << "Node{int data; Node* next;}   sizeof=" << sizeof(Node)
         << "  offsetof(data)=" << offsetof(Node, data)
         << " offsetof(next)=" << offsetof(Node, next)
         << "  (4 bytes of padding so the pointer sits on an 8 byte boundary)\n";

    cout << "\n=== 6. the heap scatters: five new Node calls ===\n";
    Node *nodes[5];
    for (int i = 0; i < 5; i++) nodes[i] = new Node{i * 10, nullptr};
    for (int i = 0; i < 5; i++) {
        cout << "  node " << i << " at " << (void*)nodes[i];
        if (i) cout << "   delta from previous = "
                    << ((char*)nodes[i] - (char*)nodes[i - 1]) << " bytes";
        cout << "\n";
    }
    cout << "  requested per node = " << sizeof(Node)
         << " bytes, allocator actually hands out " << malloc_usable_size(nodes[0])
         << " usable bytes and charges a " << ((char*)nodes[1] - (char*)nodes[0])
         << " byte stride (the rest is its own header plus alignment)\n";
    for (int i = 0; i < 5; i++) delete nodes[i];
    delete heapOne;

    cout << "\n=== 7. cost is the shape: measured access time per element ===\n";
    const int N = 2000000;
    int *arr = new int[N];
    for (int i = 0; i < N; i++) arr[i] = i;
    long long sumArr = 0;
    auto t0 = chrono::steady_clock::now();
    for (int rep = 0; rep < 8; rep++)
        for (int i = 0; i < N; i++) sumArr += arr[i];        // sequential, cache friendly
    auto t1 = chrono::steady_clock::now();
    double nsArr = (double)chrono::duration_cast<chrono::nanoseconds>(t1 - t0).count() / (8.0 * N);

    Node *head = nullptr, *tail = nullptr;
    for (int i = 0; i < N; i++) {
        Node *n = new Node{i, nullptr};
        if (!tail) head = tail = n; else { tail->next = n; tail = n; }
    }
    long long sumList = 0;
    auto t2 = chrono::steady_clock::now();
    for (int rep = 0; rep < 8; rep++)
        for (Node *cur = head; cur; cur = cur->next) sumList += cur->data;   // chase pointers
    auto t3 = chrono::steady_clock::now();
    double nsList = (double)chrono::duration_cast<chrono::nanoseconds>(t3 - t2).count() / (8.0 * N);

    cout << fixed << setprecision(2);
    cout << "  array walk  (arr[i], contiguous, hardware prefetcher helps): "
         << nsArr << " ns/element\n";

    // same array, same number of visits, but the ORDER is scattered: locality gone
    long long sumRand = 0;
    unsigned int idx = 0;
    auto t4 = chrono::steady_clock::now();
    for (int rep = 0; rep < 8; rep++)
        for (int i = 0; i < N; i++) {
            idx = (idx + 15485863u) % (unsigned)N;      // prime stride, co-prime with N
            sumRand += arr[idx];
        }
    auto t5 = chrono::steady_clock::now();
    double nsRand = (double)chrono::duration_cast<chrono::nanoseconds>(t5 - t4).count() / (8.0 * N);
    cout << "  array walk  (arr[idx], same array, scattered order):        "
         << nsRand << " ns/element\n";

    cout << "  linked walk (cur = cur->next, one dependent load per hop):  "
         << nsList << " ns/element\n";
    cout << "  scattered array is " << (nsRand / nsArr) << "x the sequential one and the list is "
         << (nsList / nsArr) << "x it, SAME O(n) work, different SHAPE\n";
    cout << "  (sanity: sums " << sumArr << " and " << sumList << " agree? "
         << (sumArr == sumList ? "yes" : "no") << ", scattered sum " << sumRand << ")\n";

    while (head) { Node *n = head->next; delete head; head = n; }
    delete[] arr;

    cout << "\n=== 8. the one line to remember ===\n";
    cout << "  address of element i in a contiguous block = base + i * sizeof(element)\n";
    cout << "  that single multiply is why indexed access is O(1);\n";
    cout << "  in a scattered structure you must follow links, so it is O(n).\n";
    return 0;
}
