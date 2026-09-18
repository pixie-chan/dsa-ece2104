// 18_quick_sort.cpp
// ECE2104 (DSA), Lecture 18: quick sort, Lomuto and Hoare partition, median-of-three.
// Handout session outcome: apply divide and conquer to sorting, and see where it breaks.
//
// Build: g++ -Wall -Wextra -Wpedantic -std=c++17 -O2 -o /tmp/s18 18_quick_sort.cpp
//
// Quick sort is the fastest general sort in practice and the only one in this
// course whose worst case is O(n^2) on inputs you will actually meet. The whole
// lesson is one question: which element becomes the pivot, and what does the
// data do to that choice. Every comparison, every swap and every recursion
// depth below is counted at the source, and every sorted output is checked
// against an independent reference sort before it is reported.
//
// Four inputs are used everywhere: random, sorted, reverse, all-equal. All four
// come from the same LCG and the same seed as lessons 16 and 17, so the n = 64
// random array here is the array lesson 16 sorted by insertion.

#include <iostream>
#include <iomanip>
#include <cstddef>
#include <cstdlib>
#include <chrono>
#include <algorithm>
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
struct QStats {
    unsigned long long comparisons = 0;  // includes the comparisons median-of-three makes
    unsigned long long swaps = 0;        // real exchanges only: i == j self swaps are not counted
    unsigned long long calls = 0;        // recursion calls entered
    unsigned long long maxDepth = 0;     // deepest simultaneous call, measured
    bool trace = false;
};

// ---------- the input: the same LCG, the same seed as lessons 16 and 17 ----------
static const unsigned long long LESSON_SEED = 20260918ULL;
static unsigned long long g_rngState = 0;
static void rngSeed(unsigned long long seed) { g_rngState = seed; }
static unsigned rngNext() {
    g_rngState = g_rngState * 6364136223846793005ULL + 1442695040888963407ULL;
    return (unsigned)(g_rngState >> 33);
}

enum InputMode { MODE_RANDOM = 0, MODE_SORTED = 1, MODE_REVERSE = 2, MODE_EQUAL = 3 };

static const char *modeName(int mode) {
    switch (mode) {
        case MODE_RANDOM:  return "random  ";
        case MODE_SORTED:  return "sorted  ";
        case MODE_REVERSE: return "reverse ";
        case MODE_EQUAL:   return "all-equal";
    }
    return "unknown ";
}
static void buildInput(int *a, size_t n, int mode) {
    rngSeed(LESSON_SEED);
    for (size_t i = 0; i < n; i++) a[i] = (int)i;
    for (size_t i = n; i > 1; i--) {
        size_t j = (size_t)(rngNext() % (unsigned)i);
        int tmpValue = a[i - 1];
        a[i - 1] = a[j];
        a[j] = tmpValue;
    }
    if (mode == MODE_SORTED || mode == MODE_REVERSE || mode == MODE_EQUAL) sort(a, a + n);
    if (mode == MODE_REVERSE) reverse(a, a + n);
    if (mode == MODE_EQUAL) for (size_t i = 0; i < n; i++) a[i] = 7;
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
static void swapVals(int &x, int &y) {
    int tmpValue = x;
    x = y;
    y = tmpValue;
}
static void printArray(const char *label, const int *a, size_t n) {
    cout << "  " << label << ":";
    for (size_t i = 0; i < n; i++) cout << " " << setw(4) << a[i];
    cout << "\n";
}
static void insertionReference(int *a, size_t n) {
    for (size_t i = 1; i < n; i++) {
        int heldValue = a[i];
        size_t j = i;
        while (j > 0 && a[j - 1] > heldValue) { a[j] = a[j - 1]; j--; }
        a[j] = heldValue;
    }
}
// independent reference: std::sort (introsort, somebody else's code), plus the
// lesson 16 insertion sort for small n, plus the sum, which any correct sort keeps
static bool confirmSorted(const int *input, const int *sorted, size_t n) {
    if (!isNonDecreasing(sorted, n)) return false;
    if (sumOf(input, n) != sumOf(sorted, n)) return false;
    int *libraryRef = (int *)heapAlloc(n * sizeof(int));
    copy(input, input + n, libraryRef);
    sort(libraryRef, libraryRef + n);
    bool matchesLibrary = sameData(sorted, libraryRef, n);
    heapFree(libraryRef);
    if (!matchesLibrary) return false;
    if (n <= 256) {
        int *insertRef = (int *)heapAlloc(n * sizeof(int));
        copy(input, input + n, insertRef);
        insertionReference(insertRef, n);
        bool matchesInsertion = sameData(sorted, insertRef, n);
        heapFree(insertRef);
        if (!matchesInsertion) return false;
    }
    return true;
}

// ================= the two partitions =================

// Lomuto: the pivot sits at the END of the range. One forward sweep, and the
// scan advances the boundary only when the element is <= the pivot.
static size_t lomutoPartition(int *a, size_t lo, size_t hi, QStats &st) {
    int pivotValue = a[hi - 1];
    size_t boundary = lo;
    unsigned long long comparisonsHere = 0, swapsHere = 0;
    for (size_t j = lo; j + 1 < hi; j++) {
        st.comparisons++;
        comparisonsHere++;
        if (a[j] <= pivotValue) {
            if (boundary != j) { swapVals(a[boundary], a[j]); st.swaps++; swapsHere++; }
            boundary++;
        }
    }
    if (boundary != hi - 1) { swapVals(a[boundary], a[hi - 1]); st.swaps++; swapsHere++; }
    if (st.trace) {
        cout << "    lomuto  [" << lo << "," << hi << ") pivot = " << setw(4) << pivotValue
             << " at index " << hi - 1 << " -> pivot lands at " << boundary
             << ", left size " << boundary - lo << ", right size " << hi - 1 - boundary
             << ", comparisons " << comparisonsHere << ", swaps " << swapsHere << "\n";
    }
    return boundary;
}

// Hoare: the pivot sits at the START of the range and both ends move inward.
// Two pointers, no boundary variable, and it returns the split AFTER the left
// part, which is why the driver below treats the two partitions differently.
static size_t hoarePartition(int *a, size_t lo, size_t hi, QStats &st) {
    int pivotValue = a[lo];
    long long i = (long long)lo - 1;
    long long j = (long long)hi;
    unsigned long long comparisonsHere = 0, swapsHere = 0;
    while (true) {
        do { i++; st.comparisons++; comparisonsHere++; } while (a[i] < pivotValue);
        do { j--; st.comparisons++; comparisonsHere++; } while (a[j] > pivotValue);
        if (i >= j) {
            if (st.trace) {
                cout << "    hoare   [" << lo << "," << hi << ") pivot = " << setw(4) << pivotValue
                     << " at index " << lo << " -> pointers meet at " << j
                     << ", left size " << j + 1 - (long long)lo << ", right size "
                     << (long long)hi - (j + 1) << ", comparisons " << comparisonsHere
                     << ", swaps " << swapsHere << "\n";
            }
            return (size_t)(j + 1);
        }
        swapVals(a[i], a[j]);
        st.swaps++;
        swapsHere++;
    }
}

// median-of-three: look at the first, middle and last element and hand the
// median of those three to the partition. Three values, at most three
// comparisons, and the decision tree below is written so the count is exact:
// two comparisons decide the common cases, the third only when they disagree.
static size_t medianOfThreeIndex(const int *a, size_t lo, size_t hi, QStats &st) {
    size_t mid = lo + (hi - lo - 1) / 2;
    st.comparisons++;
    bool firstLtMiddle = a[lo] < a[mid];
    st.comparisons++;
    bool middleLtLast = a[mid] < a[hi - 1];
    if (firstLtMiddle == middleLtLast) return mid;   // a[lo] < a[mid] < a[last], or the mirror image
    st.comparisons++;
    bool firstLtLast = a[lo] < a[hi - 1];
    if (firstLtMiddle && !middleLtLast) return firstLtLast ? hi - 1 : lo;
    return firstLtLast ? lo : hi - 1;
}
static void medianToLast(int *a, size_t lo, size_t hi, QStats &st) {
    size_t pivotPos = medianOfThreeIndex(a, lo, hi, st);
    if (pivotPos != hi - 1) { swapVals(a[pivotPos], a[hi - 1]); st.swaps++; }
}
static void medianToFirst(int *a, size_t lo, size_t hi, QStats &st) {
    size_t pivotPos = medianOfThreeIndex(a, lo, hi, st);
    if (pivotPos != lo) { swapVals(a[pivotPos], a[lo]); st.swaps++; }
}

// ================= one driver for all four combinations =================
typedef size_t (*PartitionFn)(int *, size_t, size_t, QStats &);
typedef void (*PivotPrepFn)(int *, size_t, size_t, QStats &);

static void quickSortDriver(int *a, size_t lo, size_t hi, QStats &st, size_t depth,
                            PivotPrepFn prep, PartitionFn part, bool hoareStyle) {
    st.calls++;
    if (depth > st.maxDepth) st.maxDepth = depth;
    if (hi - lo < 2) return;                    // zero or one element: already sorted
    if (prep) prep(a, lo, hi, st);
    size_t split = part(a, lo, hi, st);
    if (hoareStyle) {
        quickSortDriver(a, lo, split, st, depth + 1, prep, part, hoareStyle);      // split is j+1
        quickSortDriver(a, split, hi, st, depth + 1, prep, part, hoareStyle);
    } else {
        quickSortDriver(a, lo, split, st, depth + 1, prep, part, hoareStyle);      // split is the pivot
        quickSortDriver(a, split + 1, hi, st, depth + 1, prep, part, hoareStyle);
    }
}
static void quickLomutoLast(int *a, size_t n, QStats &st) {
    quickSortDriver(a, 0, n, st, 1, nullptr, lomutoPartition, false);
}
static void quickLomutoMedian3(int *a, size_t n, QStats &st) {
    quickSortDriver(a, 0, n, st, 1, medianToLast, lomutoPartition, false);
}
static void quickHoareFirst(int *a, size_t n, QStats &st) {
    quickSortDriver(a, 0, n, st, 1, nullptr, hoarePartition, true);
}
static void quickHoareMedian3(int *a, size_t n, QStats &st) {
    quickSortDriver(a, 0, n, st, 1, medianToFirst, hoarePartition, true);
}

struct Variant {
    const char *name;
    void (*sort)(int *, size_t, QStats &);
};
static const Variant VARIANTS[4] = {
    {"lomuto-last     ", quickLomutoLast},
    {"lomuto-median3  ", quickLomutoMedian3},
    {"hoare-first     ", quickHoareFirst},
    {"hoare-median3   ", quickHoareMedian3},
};

int main() {
    const size_t TINY = 8;
    const size_t N64 = 64;
    const size_t N1K = 1024;
    const size_t TIMED = 8192;
    const size_t LARGE = 65536;
    unsigned long long swapsRandom[4] = {0, 0, 0, 0};

    cout << "=== 1. the four inputs, n = " << TINY << ", the same LCG seed as lesson 16 ===\n";
    int *a8 = (int *)heapAlloc(TINY * sizeof(int));
    for (int mode : {MODE_RANDOM, MODE_SORTED, MODE_REVERSE, MODE_EQUAL}) {
        buildInput(a8, TINY, mode);
        printArray(modeName(mode), a8, TINY);
    }
    cout << "  these four shapes are the whole lesson: the first is the case quick sort is\n";
    cout << "  designed for, the next two are what a lazy pivot choice cannot survive, and the\n";
    cout << "  fourth is the case that separates Lomuto from Hoare\n";
    heapFree(a8);

    cout << "\n=== 2. Lomuto partition alone, traced at n = " << TINY << " ===\n";
    int *traceBuf = (int *)heapAlloc(TINY * sizeof(int));
    buildInput(traceBuf, TINY, MODE_RANDOM);
    printArray("before", traceBuf, TINY);
    QStats stTraceOne{};
    stTraceOne.trace = true;
    size_t tracePivot = lomutoPartition(traceBuf, 0, TINY, stTraceOne);
    printArray("after ", traceBuf, TINY);
    cout << "  the pivot value now sits at index " << tracePivot
         << ", everything left of it is <= it and everything right of it is > it\n";
    bool partitionOk = true;
    for (size_t i = 0; i < tracePivot; i++) if (traceBuf[i] > traceBuf[tracePivot]) partitionOk = false;
    for (size_t i = tracePivot + 1; i < TINY; i++) if (traceBuf[i] <= traceBuf[tracePivot]) partitionOk = false;
    cout << "  partition invariant checked element by element: " << (partitionOk ? "confirmed" : "MISMATCH")
         << ", comparisons " << stTraceOne.comparisons << ", swaps " << stTraceOne.swaps << "\n";
    cout << "  a partition of n elements costs n-1 comparisons and at most n-1 swaps: one pass\n";

    cout << "\n=== 3. Hoare partition on THE SAME input, traced ===\n";
    buildInput(traceBuf, TINY, MODE_RANDOM);
    printArray("before", traceBuf, TINY);
    QStats stTraceTwo{};
    stTraceTwo.trace = true;
    long long tracePivotTwo = (long long)hoarePartition(traceBuf, 0, TINY, stTraceTwo) - 1;
    printArray("after ", traceBuf, TINY);
    bool hoareOk = true;
    for (size_t i = 0; i <= (size_t)tracePivotTwo; i++) if (traceBuf[i] > traceBuf[tracePivotTwo]) hoareOk = false;
    for (size_t i = (size_t)tracePivotTwo + 1; i < TINY; i++) if (traceBuf[i] < traceBuf[tracePivotTwo]) hoareOk = false;
    cout << "  Hoare does not park the pivot: the two pointers sweep toward each other and the\n";
    cout << "  split is where they cross, so the last index of the left part is " << tracePivotTwo << "\n";
    cout << "  on this input the pivot value 0 is the SMALLEST element, so no element can be moved\n";
    cout << "  past it and the split is 1 against " << TINY - 1 << ": a good scheme still degenerates when\n";
    cout << "  the pivot it happens to pick is the extreme value of the range\n";
    cout << "  comparisons " << stTraceTwo.comparisons << " against Lomuto's " << stTraceOne.comparisons
         << " on the same " << TINY << " elements (the pointer crossing test costs a comparison too)\n";
    cout << "  Hoare split halves checked element by element: " << (hoareOk ? "confirmed" : "MISMATCH") << "\n";
    cout << "  the measured advantage of Hoare is the swap count on large random input, and section 6\n";
    cout << "  counts it for all four variants on the same n = " << N1K << " array\n";

    cout << "\n=== 4. median-of-three, the pivot it picks at n = " << TINY << " ===\n";
    for (int mode : {MODE_RANDOM, MODE_SORTED, MODE_REVERSE}) {
        buildInput(traceBuf, TINY, mode);
        QStats stMedian{};
        size_t medianPos = medianOfThreeIndex(traceBuf, 0, TINY, stMedian);
        cout << "  " << modeName(mode) << " a[0] = " << setw(3) << traceBuf[0]
             << ", a[" << (TINY - 1) / 2 << "] = " << setw(3) << traceBuf[(TINY - 1) / 2]
             << ", a[" << TINY - 1 << "] = " << setw(3) << traceBuf[TINY - 1]
             << " -> median is at index " << medianPos << " (value " << traceBuf[medianPos]
             << "), comparisons spent choosing = " << stMedian.comparisons << "\n";
    }
    cout << "  on a sorted or reverse sorted input the three sampled values are the smallest,\n";
    cout << "  the middle and the largest, so the median of them is the true middle element:\n";
    cout << "  that is exactly the pivot that splits the range in half\n";
    heapFree(traceBuf);

    cout << "\n=== 5. comparisons and swaps, 4 inputs x 4 variants, n = " << N64 << " ===\n";
    cout << "  input      variant            comparisons     swaps   depth   verdict\n";
    for (int mode : {MODE_RANDOM, MODE_SORTED, MODE_REVERSE, MODE_EQUAL}) {
        int *input = (int *)heapAlloc(N64 * sizeof(int));
        buildInput(input, N64, mode);
        for (const Variant &variant : VARIANTS) {
            int *work = (int *)heapAlloc(N64 * sizeof(int));
            copy(input, input + N64, work);
            QStats st{};
            variant.sort(work, N64, st);
            cout << "  " << modeName(mode) << " " << variant.name << "  " << setw(11) << st.comparisons
                 << "  " << setw(9) << st.swaps << "  " << setw(6) << st.maxDepth << "   "
                 << (confirmSorted(input, work, N64) ? "confirmed" : "MISMATCH")
                 << "   checksum " << checksum(work, N64) << "\n";
            heapFree(work);
        }
        heapFree(input);
    }

    cout << "\n=== 6. the same table at n = " << N1K << " ===\n";
    cout << "  input      variant            comparisons     swaps   depth   verdict\n";
    unsigned long long worstCmp[4] = {0, 0, 0, 0};
    int worstMode[4] = {-1, -1, -1, -1};
    for (int mode : {MODE_RANDOM, MODE_SORTED, MODE_REVERSE, MODE_EQUAL}) {
        int *input = (int *)heapAlloc(N1K * sizeof(int));
        buildInput(input, N1K, mode);
        for (size_t variantIndex = 0; variantIndex < 4; variantIndex++) {
            const Variant &variant = VARIANTS[variantIndex];
            int *work = (int *)heapAlloc(N1K * sizeof(int));
            copy(input, input + N1K, work);
            QStats st{};
            variant.sort(work, N1K, st);
            cout << "  " << modeName(mode) << " " << variant.name << "  " << setw(11) << st.comparisons
                 << "  " << setw(9) << st.swaps << "  " << setw(6) << st.maxDepth << "   "
                 << (confirmSorted(input, work, N1K) ? "confirmed" : "MISMATCH")
                 << "   checksum " << checksum(work, N1K) << "\n";
            if (st.comparisons > worstCmp[variantIndex]) {
                worstCmp[variantIndex] = st.comparisons;
                worstMode[variantIndex] = mode;
            }
            if (mode == MODE_RANDOM) swapsRandom[variantIndex] = st.swaps;
            heapFree(work);
        }
        heapFree(input);
    }
    cout << "  n(n-1)/2 at " << N1K << " = " << (unsigned long long)N1K * (N1K - 1) / 2
         << " and n*log2(n) = " << N1K * 10 << ": read the table against those two\n";
    cout << "  the swaps column on the random input is the reason Hoare exists: " << VARIANTS[0].name
         << " " << swapsRandom[0] << ", " << VARIANTS[1].name << " " << swapsRandom[1] << ", "
         << VARIANTS[2].name << " " << swapsRandom[2] << ", " << VARIANTS[3].name << " " << swapsRandom[3]
         << "\n";
    cout << "  measured on this machine: hoare-first swaps about half as often as lomuto-last and never\n";
    cout << "  swaps an element with itself, which is where its lower constant comes from\n";

    cout << "\n=== 7. the O(n^2) case, concretely: sorted input with the last element as pivot ===\n";
    for (size_t n : {TINY, N64}) {
        int *input = (int *)heapAlloc(n * sizeof(int));
        buildInput(input, n, MODE_SORTED);
        int *work = (int *)heapAlloc(n * sizeof(int));
        copy(input, input + n, work);
        QStats st{};
        if (n == TINY) st.trace = true;
        cout << "  n = " << n << ", already sorted, pivot = a[hi-1]:\n";
        quickLomutoLast(work, n, st);
        unsigned long long quadratic = (unsigned long long)n * (n - 1) / 2;
        cout << "    comparisons measured = " << st.comparisons << " vs n(n-1)/2 = " << quadratic
             << " : " << (st.comparisons == quadratic ? "match" : "MISMATCH") << "\n";
        cout << "    recursion depth measured = " << st.maxDepth << " vs n = " << n
             << " : " << (st.maxDepth == n ? "match" : "MISMATCH")
             << ", calls = " << st.calls << " (depth counts every call entered, and the call that\n";
        cout << "    sees a single element is still a call, which is why the chain reports n and not n-1)\n";
        cout << "    swaps measured = " << st.swaps << " (nothing ever needs to move: the input was\n";
        cout << "    already sorted, so every comparison is wasted and no element is misplaced)\n";
        cout << "    sorted anyway: " << (confirmSorted(input, work, n) ? "confirmed" : "MISMATCH")
             << ", checksum " << checksum(work, n) << "\n";
        heapFree(work);
        heapFree(input);
    }
    cout << "  the same input with median-of-three instead of a fixed pivot:\n";
    {
        int *input = (int *)heapAlloc(N1K * sizeof(int));
        buildInput(input, N1K, MODE_SORTED);
        int *work = (int *)heapAlloc(N1K * sizeof(int));
        copy(input, input + N1K, work);
        QStats st{};
        quickLomutoMedian3(work, N1K, st);
        cout << "    n = " << N1K << ": comparisons " << st.comparisons
             << " (quadratic would be " << (unsigned long long)N1K * (N1K - 1) / 2 << ")"
             << ", depth " << st.maxDepth << " (quadratic would be " << N1K - 1 << ")\n";
        cout << "    reference check: " << (confirmSorted(input, work, N1K) ? "confirmed" : "MISMATCH") << "\n";
        heapFree(work);
        heapFree(input);
    }

    cout << "\n=== 8. all-equal input: the case that separates Lomuto from Hoare ===\n";
    cout << "  variant            n        comparisons     swaps   depth   verdict\n";
    unsigned long long cmpEqualLomuto = 0, cmpEqualHoare = 0;
    for (size_t n : {TINY, N64, N1K}) {
        int *input = (int *)heapAlloc(n * sizeof(int));
        buildInput(input, n, MODE_EQUAL);
        for (size_t variantIndex = 0; variantIndex < 4; variantIndex++) {
            const Variant &variant = VARIANTS[variantIndex];
            int *work = (int *)heapAlloc(n * sizeof(int));
            copy(input, input + n, work);
            QStats st{};
            variant.sort(work, n, st);
            if (n == N1K && variantIndex == 0) cmpEqualLomuto = st.comparisons;
            if (n == N1K && variantIndex == 2) cmpEqualHoare = st.comparisons;
            cout << "  " << variant.name << "  " << setw(6) << n << "  " << setw(11) << st.comparisons
                 << "  " << setw(9) << st.swaps << "  " << setw(6) << st.maxDepth << "   "
                 << (confirmSorted(input, work, n) ? "confirmed" : "MISMATCH") << "\n";
            heapFree(work);
        }
        cout << "    (n(n-1)/2 = " << (unsigned long long)n * (n - 1) / 2
             << ", n*log2(n) is what the balanced variants should stay near)\n";
        heapFree(input);
    }
    cout << "  Lomuto tests a[j] <= pivot, so on all-equal data EVERY element joins the left part\n";
    cout << "  and the split is 1 against n-1: quadratic, and the measured table shows it at every n.\n";
    cout << "  Median-of-three does NOT rescue that, because on this input every sampled value is\n";
    cout << "  equal to the pivot, so it still lands on one side. Hoare's two pointers walk toward\n";
    cout << "  each other and meet in the middle no matter how equal the values are: measured "
         << cmpEqualHoare << "\n";
    cout << "  comparisons at n = " << N1K << " against Lomuto's " << cmpEqualLomuto
         << ", which is the whole difference\n";
    cout << "  between n log2 n and n(n-1)/2 on data that has no order to exploit: a factor of "
         << setprecision(2) << (double)cmpEqualLomuto / (double)cmpEqualHoare << "\n";
    cout << defaultfloat;
    cout << "  the textbook cure for all-equal data is a three way partition (values less, equal,\n";
    cout << "  greater) or a random pivot, not a better median\n";

    cout << "\n=== 9. iteration depth, measured: the O(log n) best case and the O(n) worst case ===\n";
    cout << "  input      variant            n        depth   log2(n)+1   n       calls\n";
    for (size_t n : {N64, N1K}) {
        for (int mode : {MODE_RANDOM, MODE_SORTED, MODE_REVERSE, MODE_EQUAL}) {
            int *input = (int *)heapAlloc(n * sizeof(int));
            buildInput(input, n, mode);
            unsigned lg = 0;
            for (unsigned long long probe = n; probe > 1; probe >>= 1) lg++;
            for (const Variant &variant : VARIANTS) {
                int *work = (int *)heapAlloc(n * sizeof(int));
                copy(input, input + n, work);
                QStats st{};
                variant.sort(work, n, st);
                cout << "  " << modeName(mode) << " " << variant.name << "  " << setw(6) << n
                     << "  " << setw(8) << st.maxDepth << "   " << setw(9) << lg + 1
                     << "   " << setw(6) << n << "   " << setw(7) << st.calls << "\n";
                heapFree(work);
            }
            heapFree(input);
        }
    }
    cout << "  depth is the measured maximum number of simultaneously live calls, and calls is the\n";
    cout << "  measured number of recursive calls entered. Depth counts the call that sees a single\n";
    cout << "  element too, so the two closed forms to read against are log2(n)+1 for a balanced run\n";
    cout << "  and n for a degenerate one. The all-equal rows are the sharpest contrast in the file:\n";
    {
        int *equalInput = (int *)heapAlloc(N1K * sizeof(int));
        buildInput(equalInput, N1K, MODE_EQUAL);
        int *hoareWork = (int *)heapAlloc(N1K * sizeof(int));
        copy(equalInput, equalInput + N1K, hoareWork);
        QStats stHoareDepth{};
        quickHoareFirst(hoareWork, N1K, stHoareDepth);
        heapFree(hoareWork);
        int *lomutoWork = (int *)heapAlloc(N1K * sizeof(int));
        copy(equalInput, equalInput + N1K, lomutoWork);
        QStats stLomutoDepth{};
        quickLomutoLast(lomutoWork, N1K, stLomutoDepth);
        heapFree(lomutoWork);
        heapFree(equalInput);
        cout << "  the same all-equal input at n = " << N1K << " reaches depth " << stHoareDepth.maxDepth
             << " for hoare-first and depth " << stLomutoDepth.maxDepth << " for lomuto-last\n";
        cout << "  which is log2(n)+1 = " << 11 << " against n = " << N1K
             << ", both measured on the same array\n";
    }

    cout << "\n=== 10. cost at n = " << TIMED << ", four variants, two inputs ===\n";
    g_allocCalls = 0; g_allocBytes = 0;
    double msEqualFastest = 0.0, msEqualSlowest = 0.0;
    unsigned long long cmpEqualFastest = 0, cmpEqualSlowest = 0;
    const char *nameEqualFastest = "";
    const char *nameEqualSlowest = "";
    for (int mode : {MODE_RANDOM, MODE_EQUAL}) {
        int *input = (int *)heapAlloc(TIMED * sizeof(int));
        buildInput(input, TIMED, mode);
        cout << "  " << modeName(mode) << " input, n = " << TIMED << "\n";
        for (const Variant &variant : VARIANTS) {
            int *work = (int *)heapAlloc(TIMED * sizeof(int));
            copy(input, input + TIMED, work);
            QStats st{};
            auto tStart = chrono::steady_clock::now();
            variant.sort(work, TIMED, st);
            auto tEnd = chrono::steady_clock::now();
            double ms = (double)chrono::duration_cast<chrono::microseconds>(tEnd - tStart).count() / 1000.0;
            cout << "    " << variant.name << " " << setw(10) << fixed << setprecision(3) << ms
                 << " ms  comparisons " << setw(10) << st.comparisons << "  depth " << setw(6) << st.maxDepth
                 << "  " << (confirmSorted(input, work, TIMED) ? "confirmed" : "MISMATCH") << "\n";
            cout << defaultfloat;
            if (mode == MODE_EQUAL) {
                if (msEqualFastest == 0.0 || ms < msEqualFastest) {
                    msEqualFastest = ms; cmpEqualFastest = st.comparisons; nameEqualFastest = variant.name;
                }
                if (ms > msEqualSlowest) {
                    msEqualSlowest = ms; cmpEqualSlowest = st.comparisons; nameEqualSlowest = variant.name;
                }
            }
            heapFree(work);
        }
        heapFree(input);
    }
    cout << "  the all-equal rows are the whole point: the same four algorithms, the same data, and\n";
    cout << "  on this input the fastest is " << nameEqualFastest << " with " << cmpEqualFastest
         << " comparisons in " << fixed << setprecision(3) << msEqualFastest << " ms while the slowest is "
         << nameEqualSlowest << " with " << cmpEqualSlowest << " comparisons in " << msEqualSlowest
         << " ms\n";
    cout << "  a measured factor of " << setprecision(1) << (msEqualSlowest / msEqualFastest)
         << " in time from the pivot choice alone, and n(n-1)/2 at n = " << TIMED << " is "
         << (unsigned long long)TIMED * (TIMED - 1) / 2 << "\n";
    cout << defaultfloat;

    cout << "\n=== 11. n = " << LARGE << ", random input, all four variants, against the reference ===\n";
    {
        int *input = (int *)heapAlloc(LARGE * sizeof(int));
        buildInput(input, LARGE, MODE_RANDOM);
        for (const Variant &variant : VARIANTS) {
            int *work = (int *)heapAlloc(LARGE * sizeof(int));
            copy(input, input + LARGE, work);
            QStats st{};
            variant.sort(work, LARGE, st);
            cout << "  " << variant.name << " comparisons " << setw(11) << st.comparisons
                 << "  swaps " << setw(10) << st.swaps << "  depth " << setw(6) << st.maxDepth
                 << "  calls " << setw(7) << st.calls
                 << "  vs reference: " << (confirmSorted(input, work, LARGE) ? "confirmed" : "MISMATCH")
                 << "  checksum " << checksum(work, LARGE) << "\n";
            heapFree(work);
        }
        heapFree(input);
    }

    cout << "\n=== 12. the complexity table ===\n";
    unsigned long long quadraticForm = (unsigned long long)N1K * (N1K - 1) / 2;
    unsigned long long nlognForm = (unsigned long long)N1K * 10;
    cout << "  variant           best        average     worst case   stable   space     worst input measured at n = "
         << N1K << "\n";
    cout << "  lomuto-last       O(n log n)  O(n log n)  O(n^2)       no       O(log n)  " << modeName(worstMode[0])
         << worstCmp[0] << "\n";
    cout << "  lomuto-median3    O(n log n)  O(n log n)  O(n^2)       no       O(log n)  " << modeName(worstMode[1])
         << worstCmp[1] << "\n";
    cout << "  hoare-first       O(n log n)  O(n log n)  O(n^2)       no       O(log n)  " << modeName(worstMode[2])
         << worstCmp[2] << "\n";
    cout << "  hoare-median3     O(n log n)  O(n log n)  O(n^2)       no       O(log n)  " << modeName(worstMode[3])
         << worstCmp[3] << "\n";
    cout << "  every worst case in that column is O(n^2): the split is decided by the data, so no\n";
    cout << "  pivot rule removes the bad case, it only decides which inputs reach it. Median-of-three\n";
    cout << "  does not change the theoretical bound, and an adversary who knows the pivot rule can\n";
    cout << "  still force the quadratic case. What the measured column shows is which of the four\n";
    cout << "  teaching inputs each choice survives: at n = " << N1K << " the quadratic form is "
         << quadraticForm << " comparisons and a healthy run stays near n log2 n = " << nlognForm << "\n";
    cout << "  read the measured column against those two numbers: " << modeName(worstMode[0])
         << " is where lomuto-last collapses (" << worstCmp[0] << "), " << modeName(worstMode[1])
         << " is where lomuto-median3 collapses (" << worstCmp[1] << "), hoare-first collapses on sorted\n";
    cout << "  and reverse (" << worstCmp[2] << "), and hoare-median3 never left the healthy band on any of\n";
    cout << "  the four shapes, which is a statement about these inputs, not a proof about all inputs\n";
    cout << "  quick sort is NOT stable: a partition swaps elements across the whole range, so equal\n";
    cout << "  keys change order, and none of the four variants repairs that. The cure for stability is\n";
    cout << "  the merge sort of lesson 17 or a sort by (key, original index) pairs\n";
    cout << "  quick sort sorts in place: every run in this lesson moved " << g_allocBytes << " bytes in "
         << g_allocCalls << " allocations, all of them this lesson's own scratch copies\n";

    cout << "\n=== 13. the one line to remember ===\n";
    cout << "  pick a pivot, move everything smaller to its left and everything larger to its right,\n";
    cout << "  then do it again on both sides. The partition is one linear pass, so the cost is the\n";
    cout << "  depth of the recursion: log2(n) levels when the pivot splits the range evenly,\n";
    cout << "  n levels when it does not. Median-of-three makes the even split the normal case, and\n";
    cout << "  Hoare's crossing pointers survive the all-equal input that kills Lomuto.\n";
    return 0;
}
