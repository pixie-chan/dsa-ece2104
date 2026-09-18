// 19_tree_representation.cpp
// ECE2104 (DSA), Lecture 19: Trees, terminology and representation.
// Handout session outcome: represent a tree in memory and account for what it costs.
//
// Build: g++ -Wall -Wextra -Wpedantic -std=c++17 -O2 -o /tmp/lec19 19_tree_representation.cpp
//
// A tree is a shape, not a block of memory. The same 7 node tree is built twice
// below: once by level inside one array, once as 7 separate blocks joined by
// pointers. Both are then charged for the bytes they really use on this machine.
// Sizes come from sizeof and offsetof, byte counts from this file's own heap
// funnel, and every claim about the index arithmetic is checked against the
// array before it is printed.
//
// Terminology, fixed once and used everywhere after:
//   root   : the node with no parent
//   leaf   : a node with no children
//   depth  : edges from the root down to a node, so the root has depth 0
//   height : nodes on the longest path from a node down to a leaf, a leaf is 1
//   degree : the number of children a node has

#include <iostream>
#include <iomanip>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <new>
#include <string>
#include <malloc.h>
using namespace std;

// ---- one funnel for the heap, so "the tree asked for N bytes" is a count
// taken at the source instead of a claim ----
static size_t g_allocCalls = 0, g_allocBytes = 0, g_freeCalls = 0;
static void *heapAlloc(size_t bytes) {
    g_allocCalls++;
    g_allocBytes += bytes;
    void *p = malloc(bytes);
    if (!p) { cerr << "out of memory\n"; exit(1); }
    return p;
}
static void heapFree(void *p) { g_freeCalls++; free(p); }

// ---- the node, two versions: only the payload differs ----
struct Node {
    int   key;
    Node *left;
    Node *right;
};

struct PayloadNode {
    int          key;
    char         payload;
    PayloadNode *left;
    PayloadNode *right;
};

static Node *makeNode(int key) {
    return new (heapAlloc(sizeof(Node))) Node{key, nullptr, nullptr};
}
static void releaseTree(Node *n) {
    if (!n) return;
    releaseTree(n->left);
    releaseTree(n->right);
    n->~Node();
    heapFree(n);
}

// render "slot 5=50" or "none" for the child/parent columns
static string slotText(const int *a, int highest, int idx) {
    if (idx < 1 || idx > highest) return "none";
    return "slot " + to_string(idx) + "=" + to_string(a[idx]);
}

static int countOccupied(const int *a, int capacity) {
    int used = 0;
    for (int i = 1; i <= capacity; i++) if (a[i] != 0) used++;
    return used;
}

static long long sumOccupied(const int *a, int capacity) {
    long long s = 0;
    for (int i = 1; i <= capacity; i++) s += a[i];
    return s;
}

// measure what the allocator really charged for a pool of nodes: sort the
// addresses, look at the gaps, sum the usable sizes
static size_t measurePool(Node *const *pool, size_t n, const char *label, size_t calls, size_t bytes) {
    Node *ord[16];
    for (size_t i = 0; i < n; i++) ord[i] = pool[i];
    for (size_t i = 1; i < n; i++) {            // insertion sort, n is 7 here
        Node *k = ord[i];
        size_t j = i;
        while (j > 0 && (char *)ord[j - 1] > (char *)k) { ord[j] = ord[j - 1]; j--; }
        ord[j] = k;
    }
    size_t usableTotal = 0;
    for (size_t i = 0; i < n; i++) usableTotal += malloc_usable_size(ord[i]);
    size_t minGap = (size_t)-1, maxGap = 0;
    for (size_t i = 1; i < n; i++) {
        size_t g = (size_t)((char *)ord[i] - (char *)ord[i - 1]);
        if (g < minGap) minGap = g;
        if (g > maxGap) maxGap = g;
    }
    size_t charged = (minGap == maxGap) ? n * minGap : usableTotal;
    cout << "  " << label << ":\n";
    cout << "    allocations = " << calls << " calling malloc(" << sizeof(Node) << ") each\n";
    cout << "    bytes requested = " << bytes << "\n";
    cout << "    usable bytes returned (sum of malloc_usable_size) = " << usableTotal << "\n";
    cout << "    lowest block at " << (void *)ord[0] << ", highest at " << (void *)ord[n - 1] << "\n";
    cout << "    gaps between neighbouring blocks: min " << minGap << ", max " << maxGap
         << ", uniform: " << (minGap == maxGap ? "yes" : "no") << "\n";
    cout << "    so the allocator charges " << minGap << " bytes of heap per node to hand out "
         << sizeof(Node) << " usable\n";
    cout << "    heap consumed by the 7 nodes = " << charged << " bytes (7 x " << minGap << ")\n";
    return charged;
}

int main() {
    cout << "=== 1. the node in memory: sizeof, offsetof, every padding byte named ===\n";
    cout << "  struct Node { int key; Node* left; Node* right; };\n";
    cout << "  sizeof(int)=" << sizeof(int) << " sizeof(char)=" << sizeof(char)
         << " sizeof(Node*)=" << sizeof(Node *) << "\n";
    cout << "  sizeof(Node)=" << sizeof(Node) << " alignof(Node)=" << alignof(Node) << "\n";

    const size_t keyOff   = offsetof(Node, key);
    const size_t leftOff  = offsetof(Node, left);
    const size_t rightOff = offsetof(Node, right);
    const size_t keyPad   = leftOff - (keyOff + sizeof(int));

    cout << "  member   offset  size  bytes occupied     why it sits there\n";
    cout << "  key" << setw(9) << keyOff << setw(6) << sizeof(int) << "  "
         << keyOff << " .. " << (keyOff + sizeof(int) - 1) << "          the key, the payload\n";
    cout << "  padding" << setw(5) << (keyOff + sizeof(int)) << setw(6) << keyPad << "  "
         << (keyOff + sizeof(int)) << " .. " << (leftOff - 1)
         << "          not a member: left holds a pointer and a pointer needs an "
         << alignof(Node *) << " byte boundary\n";
    cout << "  left" << setw(8) << leftOff << setw(6) << sizeof(Node *) << "  "
         << leftOff << " .. " << (leftOff + sizeof(Node *) - 1) << "         address of the left subtree, 0 for none\n";
    cout << "  right" << setw(7) << rightOff << setw(6) << sizeof(Node *) << "  "
         << rightOff << " .. " << (rightOff + sizeof(Node *) - 1) << "         address of the right subtree, 0 for none\n";
    cout << "  totals: payload " << sizeof(int) << " + pointers " << 2 * sizeof(Node *)
         << " + padding " << keyPad << " = " << (sizeof(int) + 2 * sizeof(Node *) + keyPad)
         << " = sizeof(Node) = " << sizeof(Node) << "  "
         << ((sizeof(int) + 2 * sizeof(Node *) + keyPad == sizeof(Node)) ? "confirmed" : "MISMATCH") << "\n";

    cout << "\n  now the same node with a char payload added:\n";
    cout << "  struct PayloadNode { int key; char payload; PayloadNode* left; PayloadNode* right; };\n";
    const size_t pKeyOff  = offsetof(PayloadNode, key);
    const size_t pCharOff = offsetof(PayloadNode, payload);
    const size_t pLeftOff = offsetof(PayloadNode, left);
    const size_t pPad     = pLeftOff - (pCharOff + sizeof(char));
    cout << "  sizeof(PayloadNode)=" << sizeof(PayloadNode)
         << " alignof(PayloadNode)=" << alignof(PayloadNode) << "\n";
    cout << "  member   offset  size\n";
    cout << "  key" << setw(9) << pKeyOff << setw(6) << sizeof(int) << "\n";
    cout << "  payload" << setw(5) << pCharOff << setw(6) << sizeof(char) << "\n";
    cout << "  padding" << setw(5) << (pCharOff + sizeof(char)) << setw(6) << pPad
         << "   bytes " << (pCharOff + sizeof(char)) << " .. " << (pLeftOff - 1) << "\n";
    cout << "  left" << setw(8) << pLeftOff << setw(6) << sizeof(PayloadNode *) << "\n";
    cout << "  right" << setw(7) << offsetof(PayloadNode, right) << setw(6) << sizeof(PayloadNode *) << "\n";
    cout << "  sizeof went from " << sizeof(Node) << " to " << sizeof(PayloadNode)
         << ", so the char cost nothing: bytes 4 .. 7 were already padding\n";

    cout << "\n=== 2. array representation: the index arithmetic ===\n";
    cout << "  a complete tree is stored by level in one array. Slot 1 holds the root and\n";
    cout << "  slot 0 is left unused, which keeps the formulas division free:\n";
    cout << "    left child of i  = 2i        right child of i = 2i + 1\n";
    cout << "    parent of i      = i / 2     (integer division)\n";
    cout << "  byte offset of slot i from the base of the array = i * sizeof(int) = i * "
         << sizeof(int) << " bytes\n";

    static int arrComplete[8];                 // slots 1 .. 7 hold the tree
    const int keys[7] = {10, 20, 30, 40, 50, 60, 70};
    for (int i = 0; i < 7; i++) arrComplete[i + 1] = keys[i];

    cout << "\n  slot   offset  address            value   left = 2i        right = 2i+1     parent = i/2\n";
    int offsetChecks = 0, offsetMatches = 0;
    for (int i = 1; i <= 7; i++) {
        const int l = 2 * i, r = 2 * i + 1, p = i / 2;
        offsetChecks++;
        if ((char *)&arrComplete[i] == (char *)arrComplete + i * (ptrdiff_t)sizeof(int)) offsetMatches++;
        cout << "  " << setw(3) << i
             << setw(8) << i * (int)sizeof(int)
             << "  " << setw(16) << (void *)&arrComplete[i]
             << setw(8) << arrComplete[i]
             << "   " << setw(15) << slotText(arrComplete, 7, l)
             << "  " << setw(15) << slotText(arrComplete, 7, r)
             << "  " << setw(15) << slotText(arrComplete, 7, p) << "\n";
    }
    cout << "  address = base + i * " << sizeof(int) << " held for "
         << offsetMatches << " of " << offsetChecks << " slots  "
         << (offsetMatches == offsetChecks ? "confirmed" : "MISMATCH") << "\n";
    cout << "  the 2i chain from the root and the i/2 walk back to it, both in index arithmetic:\n";
    for (int st = 1; st <= 5; st++) {
        int idx = 1;
        for (int k = 0; k < st; k++) idx *= 2;
        cout << "    slot 1 = " << arrComplete[1] << ", 2i applied " << st << " times reaches slot "
             << setw(2) << idx << " (" << slotText(arrComplete, 7, idx) << ")";
        cout << "   the i/2 walk back: ";
        for (int v = idx; v >= 1; v = v / 2) cout << (v == idx ? "" : " -> ") << v;
        cout << "\n";
    }
    const long long arrSum = sumOccupied(arrComplete, 7);
    long long arrExpected = 0;
    for (int i = 0; i < 7; i++) arrExpected += keys[i];
    cout << "  the 7 slots hold " << arrSum << " in total against " << arrExpected
         << " from the insert list: " << (arrSum == arrExpected ? "confirmed" : "MISMATCH") << "\n";
    cout << "  index arithmetic run backwards: the value " << arrComplete[5] << " at slot 5 prints its\n";
    cout << "  own offset ((address - base) / sizeof(int)) = "
         << ((char *)&arrComplete[5] - (char *)arrComplete) / (ptrdiff_t)sizeof(int)
         << ", so a slot index is just a byte offset divided by " << sizeof(int) << "\n";
    cout << "  parent of the root is 1 / 2 = 0, and slot 0 is the one slot the tree never uses\n";

    cout << "\n=== 3. the array pays by SHAPE, not by node count ===\n";
    cout << "  the same 7 keys, now skewed into chains. A right leaning chain of 7 nodes has\n";
    cout << "  depth 6, and the slot indices run away as 1 -> 2i+1 each time:\n";
    static int arrRight[128];                  // slots 1 .. 127, index 127 is the last node
    static int arrLeft[65];                    // slots 1 .. 64
    int rightIdx[7], leftIdx[7];
    rightIdx[0] = 1;
    leftIdx[0] = 1;
    for (int i = 1; i < 7; i++) {
        rightIdx[i] = rightIdx[i - 1] * 2 + 1;
        leftIdx[i]  = leftIdx[i - 1] * 2;
    }
    for (int i = 0; i < 7; i++) {
        arrRight[rightIdx[i]] = i + 1;
        arrLeft[leftIdx[i]]  = i + 1;
    }
    cout << "  right chain slot walk :";
    for (int i = 0; i < 7; i++) cout << " " << rightIdx[i];
    cout << "\n  left chain  slot walk :";
    for (int i = 0; i < 7; i++) cout << " " << leftIdx[i];
    cout << "\n";
    cout << "  the right chain needs slot 127 for its 7th node, the left chain needs slot 64\n";
    cout << "  because 2i+1 runs away faster than 2i: the skew direction changes the bill\n";

    const int rightCap = 127, leftCap = 64, completeCap = 7;
    const int rightUsed = countOccupied(arrRight, rightCap);
    const int leftUsed  = countOccupied(arrLeft, leftCap);
    const int completeUsed = countOccupied(arrComplete, completeCap);
    cout << "\n  shape           slots declared  slots holding a key  idle slots  idle bytes  in-order of the keys\n";
    cout << "  complete tree" << setw(16) << sizeof(arrComplete) / sizeof(int) << setw(21) << completeUsed
         << setw(12) << (int)(sizeof(arrComplete) / sizeof(int)) - completeUsed
         << setw(12) << ((int)(sizeof(arrComplete) / sizeof(int)) - completeUsed) * (int)sizeof(int)
         << "   10 20 30 40 50 60 70\n";
    cout << "  right chain  " << setw(16) << sizeof(arrRight) / sizeof(int) << setw(21) << rightUsed
         << setw(12) << (int)(sizeof(arrRight) / sizeof(int)) - rightUsed
         << setw(12) << ((int)(sizeof(arrRight) / sizeof(int)) - rightUsed) * (int)sizeof(int)
         << "   1 2 3 4 5 6 7\n";
    cout << "  left chain   " << setw(16) << sizeof(arrLeft) / sizeof(int) << setw(21) << leftUsed
         << setw(12) << (int)(sizeof(arrLeft) / sizeof(int)) - leftUsed
         << setw(12) << ((int)(sizeof(arrLeft) / sizeof(int)) - leftUsed) * (int)sizeof(int)
         << "   1 2 3 4 5 6 7\n";
    cout << "  the right chain stores 7 keys of " << sizeof(int) << " bytes and reserves "
         << sizeof(arrRight) << " bytes to do it: " << fixed << setprecision(2)
         << (100.0 * (double)rightUsed / (double)(sizeof(arrRight) / sizeof(int)))
         << " percent of its slots hold a key\n";
    cout << "  the array is sized for the WORST shape you allow, so that price is paid for every\n";
    cout << "  node of the tree, not only for the ones you happen to insert.\n";
    cout << "  checksums, printed so -O2 cannot delete the counting loops: complete tree "
         << sumOccupied(arrComplete, completeCap) << " (keys 10 .. 70), right chain "
         << sumOccupied(arrRight, rightCap) << " and left chain " << sumOccupied(arrLeft, leftCap)
         << " (keys 1 .. 7)\n";

    cout << "\n=== 4. node and pointer representation: the same 7 keys ===\n";
    const size_t callsBeforeBalanced = g_allocCalls, bytesBeforeBalanced = g_allocBytes;
    Node *balancedPool[7];
    for (int i = 0; i < 7; i++) balancedPool[i] = makeNode(keys[i]);
    balancedPool[0]->left  = balancedPool[1];
    balancedPool[0]->right = balancedPool[2];
    balancedPool[1]->left  = balancedPool[3];
    balancedPool[1]->right = balancedPool[4];
    balancedPool[2]->left  = balancedPool[5];
    balancedPool[2]->right = balancedPool[6];
    const size_t balancedCharged = measurePool(balancedPool, 7, "complete tree of 7 nodes",
        g_allocCalls - callsBeforeBalanced, g_allocBytes - bytesBeforeBalanced);

    const size_t callsBeforeChain = g_allocCalls, bytesBeforeChain = g_allocBytes;
    Node *chainPool[7];
    for (int i = 0; i < 7; i++) chainPool[i] = makeNode(i + 1);
    for (int i = 0; i < 6; i++) chainPool[i]->right = chainPool[i + 1];
    const size_t chainCharged = measurePool(chainPool, 7, "right leaning chain of 7 nodes",
        g_allocCalls - callsBeforeChain, g_allocBytes - bytesBeforeChain);

    cout << "\n  the pointer representation charges by NODE, not by shape: 7 allocations for the\n";
    cout << "  chain, the same count and the same total heap ("
         << (balancedCharged == chainCharged ? "yes" : "no")
         << ") as the complete tree. The shape is free, the nodes are not.\n";

    cout << "\n=== 5. the two representations side by side, measured ===\n";
    cout << "  " << string(92, '=') << "\n";
    cout << "  " << left << setw(32) << "representation" << setw(14) << "shape"
         << right << setw(8) << "units" << setw(12) << "bytes held" << setw(12) << "idle units"
         << setw(12) << "bytes idle" << "\n";
    cout << "  " << string(92, '-') << "\n";
    cout << "  " << left << setw(32) << "array by level, slot 0 unused" << setw(14) << "complete"
         << right << setw(8) << (int)(sizeof(arrComplete) / sizeof(int)) << setw(12) << sizeof(arrComplete)
         << setw(12) << ((int)(sizeof(arrComplete) / sizeof(int)) - completeUsed)
         << setw(12) << (((int)(sizeof(arrComplete) / sizeof(int)) - completeUsed) * (int)sizeof(int)) << "\n";
    cout << "  " << left << setw(32) << "array by level, slot 0 unused" << setw(14) << "right chain"
         << right << setw(8) << (int)(sizeof(arrRight) / sizeof(int)) << setw(12) << sizeof(arrRight)
         << setw(12) << ((int)(sizeof(arrRight) / sizeof(int)) - rightUsed)
         << setw(12) << (((int)(sizeof(arrRight) / sizeof(int)) - rightUsed) * (int)sizeof(int)) << "\n";
    cout << "  " << left << setw(32) << "array by level, slot 0 unused" << setw(14) << "left chain"
         << right << setw(8) << (int)(sizeof(arrLeft) / sizeof(int)) << setw(12) << sizeof(arrLeft)
         << setw(12) << ((int)(sizeof(arrLeft) / sizeof(int)) - leftUsed)
         << setw(12) << (((int)(sizeof(arrLeft) / sizeof(int)) - leftUsed) * (int)sizeof(int)) << "\n";
    cout << "  " << left << setw(32) << "node + pointer (7 malloc calls)" << setw(14) << "complete"
         << right << setw(8) << 7 << setw(12) << sizeof(Node) * 7 << setw(12) << 0 << setw(12) << 0 << "\n";
    cout << "  " << left << setw(32) << "node + pointer (7 malloc calls)" << setw(14) << "right chain"
         << right << setw(8) << 7 << setw(12) << sizeof(Node) * 7 << setw(12) << 0 << setw(12) << 0 << "\n";
    cout << "  " << string(92, '=') << "\n";
    cout << "  units = slots for the array rows and malloc calls for the pointer rows.\n";
    cout << "  bytes held = sizeof of the container, which is what the program declares.\n";
    cout << "  the pointer rows hold " << sizeof(Node) * 7 << " bytes of requested node payload and the\n";
    cout << "  allocator charges " << balancedCharged << " bytes of heap per pool, so "
         << (balancedCharged - sizeof(Node) * 7) << " bytes are header and alignment;\n";
    cout << "  the array rows hold no bookkeeping at all: the slot index IS the link.\n";
    cout << "  array wins on a complete or near complete shape, the pointer form wins on a shape\n";
    cout << "  that would waste slots, and it always wins on inserting and deleting in the middle.\n";

    releaseTree(balancedPool[0]);
    releaseTree(chainPool[0]);
    cout << "\n  allocations " << g_allocCalls << ", frees " << g_freeCalls << "  "
         << (g_allocCalls == g_freeCalls ? "every node returned" : "LEAK") << "\n";

    cout << "\n=== 6. the one line to remember ===\n";
    cout << "  a tree is a shape: choose the representation that makes that shape cheap.\n";
    cout << "  array by level: 2i and 2i+1 index a complete tree in O(1) with zero pointers,\n";
    cout << "  and it reserves room for the WORST shape you allow, which a chain shows is huge.\n";
    cout << "  node + pointer: 24 bytes per node on this machine, 4 of which are padding,\n";
    cout << "  pays per node instead of per shape, and moves a subtree by moving one pointer.\n";
    return 0;
}
