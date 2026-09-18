// 17_merge_sort.cpp
// ECE2104 (DSA), Lecture 17: merge sort, on an array and on a linked list.
// Handout session outcome: apply divide and conquer to sorting.
//
// Build: g++ -Wall -Wextra -Wpedantic -std=c++17 -O2 -o /tmp/s17 17_merge_sort.cpp
//
// Merge sort is the first sort whose cost does not depend on the input order in
// any interesting way: the recursion tree is fixed, only the number of
// comparisons inside each merge moves, and it moves within a factor of two.
// Every comparison, every merge and every element copy below is counted inside
// the merge loop. The same input array is built with the same LCG and the same
// seed as lesson 16, so n = 64 here is the array that lesson 16 sorted.
//
// The array version needs an auxiliary buffer of n elements. The linked list
// version needs none: it relinks nodes instead of moving values, and section 8
// prints both costs in bytes, measured.

#include <iostream>
#include <iomanip>
#include <cstddef>
#include <cstdlib>
#include <chrono>
#include <algorithm>
#include <new>
#include <malloc.h>
using namespace std;

// ---------- the heap helper: one door for every allocation in this lesson ----------
static unsigned long long g_allocCalls = 0, g_allocBytes = 0;
static void *heapAlloc(size_t bytes) {
    g_allocCalls++;
    g_allocBytes += bytes;
    void *p = malloc(bytes);
    if (!p) { cerr << "out of memory\n"; exit(1); }
    return p;
}
static void heapFree(void *p) { free(p); }

// ---------- counters, bumped where the work happens ----------
struct MergeStats {
    unsigned long long comparisons = 0;
    unsigned long long merges = 0;         // merge steps that combined two non-empty runs
    unsigned long long elementCopies = 0;  // values written back into the array
    unsigned long long tempWrites = 0;     // values written into the auxiliary buffer
    unsigned long long linksRewritten = 0; // list version: next pointers reassigned
    unsigned long long calls = 0;          // recursive calls entered
    unsigned long long maxDepth = 0;       // deepest simultaneous call, measured
    bool trace = false;
};

// ---------- the input: the same LCG, the same seed as lesson 16 ----------
static const unsigned long long LESSON_SEED = 20260918ULL;
static unsigned long long g_rngState = 0;
static void rngSeed(unsigned long long seed) { g_rngState = seed; }
static unsigned rngNext() {
    g_rngState = g_rngState * 6364136223846793005ULL + 1442695040888963407ULL;
    return (unsigned)(g_rngState >> 33);
}
static void buildInput(int *a, size_t n) {          // the random permutation of lesson 16
    rngSeed(LESSON_SEED);
    for (size_t i = 0; i < n; i++) a[i] = (int)i;
    for (size_t i = n; i > 1; i--) {
        size_t j = (size_t)(rngNext() % (unsigned)i);
        int tmpValue = a[i - 1];
        a[i - 1] = a[j];
        a[j] = tmpValue;
    }
}

// ---------- observables ----------
static unsigned long long checksum(const int *a, size_t n) {
    unsigned long long hashValue = 1469598103934665603ULL;
    unsigned long long sumValue = 0;
    for (size_t i = 0; i < n; i++) {
        hashValue ^= (unsigned long long)(unsigned)a[i];
        hashValue *= 1099511628211ULL;
        sumValue += (unsigned long long)(unsigned)a[i];
    }
    return hashValue ^ (sumValue << 1);
}
static long long sumOf(const int *a, size_t n) {
    long long total = 0;
    for (size_t i = 0; i < n; i++) total += a[i];
    return total;
}
static bool isNonDecreasing(const int *a, size_t n) {
    for (size_t i = 1; i < n; i++) if (a[i - 1] > a[i]) return false;
    return true;
}
static bool sameData(const int *a, const int *b, size_t n) {
    for (size_t i = 0; i < n; i++) if (a[i] != b[i]) return false;
    return true;
}
static unsigned log2Floor(unsigned long long value) {
    unsigned exponent = 0;
    while (value > 1) { value >>= 1; exponent++; }
    return exponent;
}
static void printArray(const char *label, const int *a, size_t n) {
    cout << "  " << label << ":";
    for (size_t i = 0; i < n; i++) cout << " " << setw(3) << a[i];
    cout << "\n";
}

// ================= the merge step: measure this once, then trust the recursion =================
static void mergeRanges(int *a, int *temp, size_t lo, size_t mid, size_t hi, MergeStats &st) {
    st.merges++;
    size_t i = lo, j = mid, k = lo;
    unsigned long long comparisonsHere = 0;
    while (i < mid && j < hi) {                    // both runs still have an element
        st.comparisons++;
        comparisonsHere++;
        if (a[i] <= a[j]) { temp[k++] = a[i++]; }  // <= keeps equal elements in input order
        else              { temp[k++] = a[j++]; }
        st.tempWrites++;
    }
    while (i < mid) { temp[k++] = a[i++]; st.tempWrites++; }   // the tail of the left run, no comparison
    while (j < hi)  { temp[k++] = a[j++]; st.tempWrites++; }   // the tail of the right run, no comparison
    for (size_t t = lo; t < hi; t++) { a[t] = temp[t]; st.elementCopies++; }
    if (st.trace) {
        cout << "    merge [" << lo << "," << mid << ") with [" << mid << "," << hi
             << ")  comparisons this merge = " << comparisonsHere << "  result:";
        for (size_t t = lo; t < hi; t++) cout << " " << setw(4) << a[t];
        cout << "\n";
    }
}
static void mergeSortRange(int *a, int *temp, size_t lo, size_t hi, MergeStats &st, size_t depth) {
    st.calls++;
    if (depth > st.maxDepth) st.maxDepth = depth;
    if (hi - lo < 2) return;                       // one element is sorted by definition: the base case
    size_t mid = lo + (hi - lo) / 2;
    mergeSortRange(a, temp, lo, mid, st, depth + 1);
    mergeSortRange(a, temp, mid, hi, st, depth + 1);
    mergeRanges(a, temp, lo, mid, hi, st);
}
static void mergeSortArray(int *a, size_t n, MergeStats &st) {
    if (n < 2) return;
    int *temp = (int *)heapAlloc(n * sizeof(int)); // ONE buffer for the whole sort, reused at every level
    mergeSortRange(a, temp, 0, n, st, 1);
    heapFree(temp);
}

// ================= the same algorithm on a singly linked list =================
struct ListNode {
    int value;
    ListNode *next;
};
static ListNode *buildList(const int *a, size_t n) {
    ListNode *head = nullptr;
    ListNode *tail = nullptr;
    for (size_t i = 0; i < n; i++) {
        ListNode *node = new (heapAlloc(sizeof(ListNode))) ListNode{a[i], nullptr};
        if (!head) head = node;
        else tail->next = node;
        tail = node;
    }
    return head;
}
static void freeList(ListNode *head) {
    while (head) {
        ListNode *nextNode = head->next;
        head->~ListNode();
        heapFree(head);
        head = nextNode;
    }
}
static size_t listLength(const ListNode *head) {
    size_t count = 0;
    for (const ListNode *node = head; node; node = node->next) count++;
    return count;
}
// the interview classic: two pointers, fast moves twice as fast as slow
static ListNode *splitHalves(ListNode *head) {
    ListNode *slow = head;
    ListNode *fast = head->next;
    while (fast && fast->next) {
        slow = slow->next;
        fast = fast->next->next;
    }
    ListNode *secondHalf = slow->next;
    slow->next = nullptr;
    return secondHalf;
}
static ListNode *mergeLists(ListNode *left, ListNode *right, MergeStats &st) {
    st.merges++;
    ListNode anchor{-1, nullptr};                  // a stack sentinel, not an allocation
    ListNode *tail = &anchor;
    while (left && right) {
        st.comparisons++;
        if (left->value <= right->value) { tail->next = left; left = left->next; }
        else                             { tail->next = right; right = right->next; }
        tail = tail->next;
        st.linksRewritten++;
    }
    tail->next = left ? left : right;               // whatever is left over is already sorted
    return anchor.next;
}
static ListNode *mergeSortList(ListNode *head, MergeStats &st, size_t depth) {
    st.calls++;
    if (depth > st.maxDepth) st.maxDepth = depth;
    if (!head || !head->next) return head;
    ListNode *secondHalf = splitHalves(head);
    ListNode *leftSorted = mergeSortList(head, st, depth + 1);
    ListNode *rightSorted = mergeSortList(secondHalf, st, depth + 1);
    return mergeLists(leftSorted, rightSorted, st);
}

// ================= the independent reference =================
// std::sort is an introsort written by somebody else: a genuinely independent
// method. For n <= 256 the local insertion sort below is checked against it too.
static void insertionReference(int *a, size_t n) {
    for (size_t i = 1; i < n; i++) {
        int heldValue = a[i];
        size_t j = i;
        while (j > 0 && a[j - 1] > heldValue) { a[j] = a[j - 1]; j--; }
        a[j] = heldValue;
    }
}
static bool confirmSorted(const int *input, const int *sorted, size_t n) {
    if (!isNonDecreasing(sorted, n)) return false;
    if (sumOf(input, n) != sumOf(sorted, n)) return false;   // any correct sort preserves the sum
    int *libraryRef = (int *)heapAlloc(n * sizeof(int));
    copy(input, input + n, libraryRef);
    sort(libraryRef, libraryRef + n);
    bool matchesLibrary = sameData(sorted, libraryRef, n);
    heapFree(libraryRef);
    if (!matchesLibrary) return false;
    if (n <= 256) {                                          // second, older method
        int *insertRef = (int *)heapAlloc(n * sizeof(int));
        copy(input, input + n, insertRef);
        insertionReference(insertRef, n);
        bool matchesInsertion = sameData(sorted, insertRef, n);
        heapFree(insertRef);
        if (!matchesInsertion) return false;
    }
    return true;
}

int main() {
    const size_t TINY = 8;
    const size_t N64 = 64;
    const size_t N1K = 1024;
    const size_t N64K = 65536;

    cout << "=== 1. the input, n = " << TINY << ", the same LCG seed as lesson 16 ===\n";
    int *a8 = (int *)heapAlloc(TINY * sizeof(int));
    buildInput(a8, TINY);
    printArray("random permutation, seed 20260918", a8, TINY);
    cout << "  merge sort never compares an element with anything outside its two runs, so the\n";
    cout << "  input order changes only HOW MANY of the merges take from left versus right\n";
    heapFree(a8);

    cout << "\n=== 2. one merge step, measured in isolation ===\n";
    // two runs already in order, so the merge is the only work in the array
    const size_t MERGE_N = 8;
    int *mergeInput = (int *)heapAlloc(MERGE_N * sizeof(int));
    int mergeLiteral[8] = {2, 5, 9, 11, 1, 4, 8, 10};
    for (size_t i = 0; i < MERGE_N; i++) mergeInput[i] = mergeLiteral[i];
    printArray("before", mergeInput, MERGE_N);
    cout << "  left run  [0,4) = 2 5 9 11, right run [4,8) = 1 4 8 10\n";
    int *mergeTemp = (int *)heapAlloc(MERGE_N * sizeof(int));
    MergeStats stMerge{};
    stMerge.trace = true;
    mergeRanges(mergeInput, mergeTemp, 0, 4, 8, stMerge);
    heapFree(mergeTemp);
    printArray("after ", mergeInput, MERGE_N);
    cout << "  comparisons in this merge = " << stMerge.comparisons
         << ", element copies = " << stMerge.elementCopies << ", merges = " << stMerge.merges << "\n";
    cout << "  a merge of two runs of p and q elements costs between min(p,q) and p+q-1\n";
    cout << "  comparisons: here " << stMerge.comparisons << " against the bounds 4 and 7\n";
    cout << "  order check: " << (isNonDecreasing(mergeInput, MERGE_N) ? "confirmed sorted" : "MISMATCH")
         << ", checksum = " << checksum(mergeInput, MERGE_N) << "\n";
    heapFree(mergeInput);

    cout << "\n=== 3. top-down merge sort on the array, every merge at n = " << TINY << " ===\n";
    int *traced = (int *)heapAlloc(TINY * sizeof(int));
    buildInput(traced, TINY);
    printArray("before", traced, TINY);
    MergeStats stTrace{};
    stTrace.trace = true;
    int *tracedTemp = (int *)heapAlloc(TINY * sizeof(int));
    mergeSortRange(traced, tracedTemp, 0, TINY, stTrace, 1);
    heapFree(tracedTemp);
    printArray("after ", traced, TINY);
    cout << "  merges = " << stTrace.merges << " (n-1 = " << TINY - 1 << "), element copies = "
         << stTrace.elementCopies << " (n*log2(n) = " << TINY * 3 << "), comparisons = "
         << stTrace.comparisons << ", deepest recursion = " << stTrace.maxDepth << "\n";
    cout << "  the level structure is visible above: 4 merges of size 2, 2 of size 4, 1 of size 8\n";
    cout << "  sorted correctly = " << (isNonDecreasing(traced, TINY) ? "yes" : "NO")
         << ", checksum = " << checksum(traced, TINY) << "\n";
    heapFree(traced);

    cout << "\n=== 4. counts for n = 8, 64, 1024, 65536, array version ===\n";
    cout << "  n        comparisons   merges   n-1       elem copies   n*log2(n)   depth   log2(n)+1\n";
    for (size_t n : {TINY, N64, N1K, N64K}) {
        int *input = (int *)heapAlloc(n * sizeof(int));
        int *work = (int *)heapAlloc(n * sizeof(int));
        buildInput(input, n);
        copy(input, input + n, work);
        MergeStats st{};
        mergeSortArray(work, n, st);
        unsigned lg = log2Floor(n);
        cout << "  " << setw(6) << n << "   " << setw(11) << st.comparisons << "   " << setw(6) << st.merges
             << "   " << setw(6) << n - 1 << "   " << setw(11) << st.elementCopies
             << "   " << setw(9) << (unsigned long long)n * lg
             << "   " << setw(5) << st.maxDepth << "   " << setw(9) << lg + 1 << "\n";
        cout << "         sorted output vs the independent reference sort: "
             << (confirmSorted(input, work, n) ? "confirmed" : "MISMATCH")
             << "   checksum = " << checksum(work, n) << "\n";
        heapFree(work);
        heapFree(input);
    }

    cout << "\n=== 5. the n log2 n closed form against the measured comparisons ===\n";
    for (size_t n : {TINY, N64, N1K, N64K}) {
        int *input = (int *)heapAlloc(n * sizeof(int));
        int *work = (int *)heapAlloc(n * sizeof(int));
        buildInput(input, n);
        copy(input, input + n, work);
        MergeStats st{};
        mergeSortArray(work, n, st);
        double nLog2n = (double)n * (double)log2Floor(n);
        cout << "  n = " << setw(6) << n
             << "  measured = " << setw(11) << st.comparisons
             << "  n log2 n = " << setw(11) << (unsigned long long)nLog2n
             << "  measured / (n log2 n) = " << fixed << setprecision(4)
             << (double)st.comparisons / nLog2n
             << "  measured / n = " << setprecision(3) << (double)st.comparisons / (double)n << "\n";
        cout << defaultfloat;
        cout << "         the ratio sits below 1 because a merge stops comparing as soon as one run\n";
        cout << "         runs out: the exact count is between n*log2(n)/2 and n*log2(n)-n+1, and only\n";
        cout << "         a measurement can say where in that band this input lands\n";
        cout << "         reference check: " << (confirmSorted(input, work, n) ? "confirmed" : "MISMATCH") << "\n";
        heapFree(work);
        heapFree(input);
    }

    cout << "\n=== 6. the same algorithm on a singly linked list ===\n";
    cout << "  n        comparisons   array vs list   merges   links rewritten   depth\n";
    for (size_t n : {TINY, N64, N1K, N64K}) {
        int *input = (int *)heapAlloc(n * sizeof(int));
        buildInput(input, n);
        int *work = (int *)heapAlloc(n * sizeof(int));
        copy(input, input + n, work);
        MergeStats stArray{};
        mergeSortArray(work, n, stArray);

        g_allocCalls = 0; g_allocBytes = 0;
        ListNode *head = buildList(input, n);
        unsigned long long nodeBytes = g_allocBytes;
        MergeStats stList{};
        ListNode *sortedHead = mergeSortList(head, stList, 1);
        // the list is walked back into an array so the two outputs can be compared as data
        int *listOut = (int *)heapAlloc(n * sizeof(int));
        size_t outIndex = 0;
        for (ListNode *node = sortedHead; node; node = node->next) listOut[outIndex++] = node->value;
        bool sameLength = (outIndex == n) && (listLength(sortedHead) == n);
        cout << "  " << setw(6) << n << "   " << setw(11) << stList.comparisons << "   "
             << setw(13) << stArray.comparisons << "   " << setw(6) << stList.merges
             << "   " << setw(15) << stList.linksRewritten << "   " << setw(5) << stList.maxDepth << "\n";
        cout << "         list output identical to the array output: "
             << (sameLength && sameData(listOut, work, n) ? "confirmed" : "MISMATCH")
             << ", sorted: " << (isNonDecreasing(listOut, n) ? "confirmed" : "MISMATCH")
             << ", checksum = " << checksum(listOut, n) << "\n";
        cout << "         comparison counts differ by "
             << (long long)stList.comparisons - (long long)stArray.comparisons
             << " (list " << stList.comparisons << " vs array " << stArray.comparisons << ")\n";
        cout << "         element copies in the list version = 0 by construction: merging a list\n";
        cout << "         reassigns next pointers, it never copies a value into a new slot\n";
        heapFree(listOut);
        freeList(sortedHead);
        heapFree(work);
        heapFree(input);
        cout << "         list nodes moved " << nodeBytes << " bytes through the heap with "
             << (n == 0 ? 0 : nodeBytes / n) << " bytes per node\n";
    }

    cout << "\n=== 7. recursion depth, measured, against log2(n) + 1 ===\n";
    cout << "  n        array depth   list depth   log2(n)+1   array calls   list calls\n";
    for (size_t n : {TINY, N64, N1K, N64K}) {
        int *input = (int *)heapAlloc(n * sizeof(int));
        buildInput(input, n);
        int *work = (int *)heapAlloc(n * sizeof(int));
        copy(input, input + n, work);
        MergeStats stArrayDepth{};
        mergeSortArray(work, n, stArrayDepth);
        heapFree(work);
        ListNode *head = buildList(input, n);
        MergeStats stListDepth{};
        ListNode *sortedHead = mergeSortList(head, stListDepth, 1);
        unsigned lg = log2Floor(n);
        cout << "  " << setw(6) << n << "   " << setw(12) << stArrayDepth.maxDepth
             << "   " << setw(11) << stListDepth.maxDepth
             << "   " << setw(10) << lg + 1
             << "   " << setw(12) << stArrayDepth.calls
             << "   " << setw(11) << stListDepth.calls << "\n";
        cout << "         array depth == log2(n)+1: "
             << (stArrayDepth.maxDepth == (unsigned long long)lg + 1 ? "confirmed" : "MISMATCH")
             << ", list depth == log2(n)+1: "
             << (stListDepth.maxDepth == (unsigned long long)lg + 1 ? "confirmed" : "MISMATCH") << "\n";
        freeList(sortedHead);
        heapFree(input);
    }
    cout << "  the recursion depth is why merge sort is the safe sort for a linked list and a\n";
    cout << "  risk for quick sort: the tree is balanced by construction, log2(n) frames, always\n";

    cout << "\n=== 8. space: the auxiliary buffer, measured ===\n";
    for (size_t n : {TINY, N64, N1K, N64K}) {
        int *input = (int *)heapAlloc(n * sizeof(int));
        buildInput(input, n);
        int *work = (int *)heapAlloc(n * sizeof(int));
        copy(input, input + n, work);
        int *tempProbe = (int *)heapAlloc(n * sizeof(int));
        size_t requestedBytes = n * sizeof(int);
        size_t usableBytes = malloc_usable_size(tempProbe);
        heapFree(tempProbe);
        MergeStats stSpace{};
        unsigned long long allocsBefore = g_allocCalls, bytesBefore = g_allocBytes;
        mergeSortArray(work, n, stSpace);
        cout << "  n = " << setw(6) << n
             << "  auxiliary array requested = " << setw(9) << requestedBytes << " bytes (n * sizeof(int))"
             << "  allocator usable size = " << setw(9) << usableBytes << " bytes\n";
        cout << "         the sort itself asked the heap for " << g_allocCalls - allocsBefore
             << " block of " << g_allocBytes - bytesBefore << " bytes: one buffer, not one per level\n";
        cout << "         element copies = " << stSpace.elementCopies << " = n * log2(n) = "
             << (unsigned long long)n * log2Floor(n)
             << ", so the BYTES moved are n log2 n * sizeof(int) while the SPACE is n\n";
        cout << "         reference check: " << (confirmSorted(input, work, n) ? "confirmed" : "MISMATCH") << "\n";
        heapFree(work);
        heapFree(input);
    }
    {
        // the list version needs no auxiliary array at all: measure its peak extra space
        const size_t n = N64;
        int *input = (int *)heapAlloc(n * sizeof(int));
        buildInput(input, n);
        g_allocCalls = 0; g_allocBytes = 0;
        ListNode *head = buildList(input, n);
        unsigned long long nodeBytes = g_allocBytes;
        unsigned long long allocsBeforeSort = g_allocCalls, bytesBeforeSort = g_allocBytes;
        MergeStats stListSpace{};
        ListNode *sortedHead = mergeSortList(head, stListSpace, 1);
        unsigned long long bufferCalls = g_allocCalls - allocsBeforeSort;
        unsigned long long bufferBytes = g_allocBytes - bytesBeforeSort;
        cout << "  list version, n = " << n << ": the nodes themselves are " << nodeBytes
             << " bytes\n";
        cout << "         the sort allocated " << bufferCalls << " blocks and " << bufferBytes
             << " bytes for an auxiliary buffer: the relinking needs none\n";
        int *listOutSpace = (int *)heapAlloc(n * sizeof(int));
        size_t outIndexSpace = 0;
        for (ListNode *node = sortedHead; node; node = node->next) listOutSpace[outIndexSpace++] = node->value;
        cout << "         extra space = the " << stListSpace.maxDepth << " live stack frames only, O(log n)\n";
        cout << "         array version extra space at the same n = " << n * sizeof(int)
             << " bytes contiguous; the list version pays " << nodeBytes << " bytes instead for its nodes\n";
        cout << "         reference check: "
             << (confirmSorted(input, listOutSpace, n) ? "confirmed" : "MISMATCH")
             << ", checksum = " << checksum(listOutSpace, n) << "\n";
        heapFree(listOutSpace);
        freeList(sortedHead);
        heapFree(input);
    }

    cout << "\n=== 9. timing, and both versions against the reference by element ===\n";
    {
        const size_t n = N64K;
        int *input = (int *)heapAlloc(n * sizeof(int));
        buildInput(input, n);
        int *work = (int *)heapAlloc(n * sizeof(int));
        copy(input, input + n, work);
        MergeStats stArrayTime{};
        auto tArrayStart = chrono::steady_clock::now();
        mergeSortArray(work, n, stArrayTime);
        auto tArrayEnd = chrono::steady_clock::now();
        double msArray = (double)chrono::duration_cast<chrono::microseconds>(tArrayEnd - tArrayStart).count() / 1000.0;
        cout << "  array version, n = " << n << ": " << fixed << setprecision(3) << msArray << " ms, comparisons "
             << stArrayTime.comparisons << ", element copies " << stArrayTime.elementCopies << "\n";
        cout << "    vs independent reference: " << (confirmSorted(input, work, n) ? "confirmed" : "MISMATCH")
             << ", checksum = " << checksum(work, n) << "\n";

        ListNode *head = buildList(input, n);
        MergeStats stListTime{};
        auto tListStart = chrono::steady_clock::now();
        ListNode *sortedHead = mergeSortList(head, stListTime, 1);
        auto tListEnd = chrono::steady_clock::now();
        double msList = (double)chrono::duration_cast<chrono::microseconds>(tListEnd - tListStart).count() / 1000.0;
        int *listOut = (int *)heapAlloc(n * sizeof(int));
        size_t outIndex = 0;
        for (ListNode *node = sortedHead; node; node = node->next) listOut[outIndex++] = node->value;
        cout << "  list version,  n = " << n << ": " << msList << " ms, comparisons "
             << stListTime.comparisons << ", element copies " << 0 << " (links rewritten "
             << stListTime.linksRewritten << ")\n";
        cout << "    output identical to the array version by element: "
             << ((outIndex == n) && sameData(listOut, work, n) ? "confirmed" : "MISMATCH")
             << ", checksum = " << checksum(listOut, n) << "\n";
        cout << defaultfloat;
        cout << "  the array version is faster per comparison because a merge scans two arrays in\n";
        cout << "  order; the list version allocates per node and chases pointers. Both are O(n log n)\n";
        cout << "  and the comparison counts are " << (stListTime.comparisons == stArrayTime.comparisons
             ? "identical, ratio " : "not identical, ratio ")
             << setprecision(6)
             << (double)stListTime.comparisons / (double)stArrayTime.comparisons
             << (stListTime.comparisons == stArrayTime.comparisons ? " : confirmed" : " : they differ") << "\n";
        cout << defaultfloat;
        heapFree(listOut);
        freeList(sortedHead);
        heapFree(work);
        heapFree(input);
    }

    cout << "\n=== 10. the complexity table ===\n";
    cout << "  algorithm              best        average     worst       space        stable\n";
    cout << "  merge sort (array)     O(n log n)  O(n log n)  O(n log n)  O(n)         yes\n";
    cout << "  merge sort (list)      O(n log n)  O(n log n)  O(n log n)  O(log n)     yes\n";
    cout << "  insertion sort (16)    O(n)        O(n^2)      O(n^2)      O(1)         yes\n";
    cout << "  merge sort is the first sort here whose worst case equals its best case, and it is\n";
    cout << "  stable because the merge takes from the LEFT run on a tie (<=, never <)\n";
    cout << "  it pays for that with a second copy of the data, or with a second pointer per node\n";

    cout << "\n=== 11. the one line to remember ===\n";
    cout << "  split in half, sort each half, merge the two sorted halves in one linear pass. The\n";
    cout << "  recursion tree is log2(n) deep no matter what the input looks like, so the cost is\n";
    cout << "  ~n log2 n comparisons in every case, and the price is an n element buffer. On a\n";
    cout << "  linked list the buffer disappears because merging relinks nodes instead of copying.\n";
    return 0;
}
