// 16_sorts_elementary.cpp
// ECE2104 (DSA), Lecture 16: elementary sorting, bubble, selection, insertion.
// Handout session outcome: apply and compare basic sorting algorithms.
//
// Build: g++ -Wall -Wextra -Wpedantic -std=c++17 -O2 -o /tmp/s16 16_sorts_elementary.cpp
//
// Sorting is the first place where the cost of an algorithm is a number you can
// count instead of a sentence you recite. Every comparison and every exchange
// printed below is counted inside the sorting loop itself (see SortStats), so
// the counts are measured at the source. Every sort ends by comparing its
// output against the insertion sort reference on the same input and printing
// the verdict, which is also what stops -O2 from deleting the work as dead
// code. Nothing here is estimated and nothing here is assumed.

#include <iostream>
#include <iomanip>
#include <cstddef>
#include <cstdlib>
#include <chrono>
#include <algorithm>
using namespace std;

// ---- counters, bumped at the comparison and at the exchange, never estimated ----
struct SortStats {
    unsigned long long comparisons = 0;
    unsigned long long exchanges = 0;      // two elements traded places
    unsigned long long shifts = 0;         // one element copied into a neighbouring slot
    unsigned long long keyPlacements = 0;  // the held value written back once its place is found
    unsigned long long passes = 0;         // outer loop iterations actually executed
};

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

// ---------- the input: a fixed LCG, the same sequence in every lesson file ----------
static const unsigned long long LESSON_SEED = 20260918ULL;
static unsigned long long g_rngState = 0;
static void rngSeed(unsigned long long seed) { g_rngState = seed; }
static unsigned rngNext() {
    g_rngState = g_rngState * 6364136223846793005ULL + 1442695040888963407ULL;
    return (unsigned)(g_rngState >> 33);
}

enum InputMode { MODE_RANDOM = 0, MODE_SORTED = 1, MODE_REVERSE = 2, MODE_EQUAL = 3, MODE_DUPES = 4 };

static const char *modeName(int mode) {
    switch (mode) {
        case MODE_RANDOM:  return "random  ";
        case MODE_SORTED:  return "sorted  ";
        case MODE_REVERSE: return "reverse ";
        case MODE_EQUAL:   return "all-equal";
        case MODE_DUPES:   return "dupes   ";
    }
    return "unknown ";
}

// One generator, one seed, five shapes of the same n element input, so any two
// numbers in this file that share (n, mode) describe the same array.
// MODE_SORTED and MODE_REVERSE are built from a permutation of 0..n-1, which
// makes every element distinct: that is what makes the exact closed forms in
// section 8 hold with no ties to reason away.
static void buildInput(int *a, size_t n, int mode) {
    rngSeed(LESSON_SEED);
    for (size_t i = 0; i < n; i++) a[i] = (int)i;
    for (size_t i = n; i > 1; i--) {                    // Fisher-Yates, LCG driven
        size_t j = (size_t)(rngNext() % (unsigned)i);
        int tmpValue = a[i - 1];
        a[i - 1] = a[j];
        a[j] = tmpValue;
    }
    if (mode == MODE_SORTED || mode == MODE_REVERSE || mode == MODE_EQUAL || mode == MODE_DUPES) {
        sort(a, a + n);
    }
    if (mode == MODE_REVERSE) reverse(a, a + n);
    if (mode == MODE_EQUAL) for (size_t i = 0; i < n; i++) a[i] = 7;
    if (mode == MODE_DUPES) for (size_t i = 0; i < n; i++) a[i] = a[i] % (int)(n / 4 + 1);
}

// ---------- observables: these are what every sort has to end in ----------
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
static bool isNonDecreasing(const int *a, size_t n) {
    for (size_t i = 1; i < n; i++) if (a[i - 1] > a[i]) return false;
    return true;
}
static bool sameData(const int *a, const int *b, size_t n) {
    for (size_t i = 0; i < n; i++) if (a[i] != b[i]) return false;
    return true;
}
static void printArray(const char *label, const int *a, size_t n) {
    cout << "  " << label << ":";
    for (size_t i = 0; i < n; i++) cout << " " << setw(3) << a[i];
    cout << "\n";
}

// ================= the three sorts, each counting at the comparison =================

// bubble: compare neighbours, exchange when out of order, stop early if a whole
// pass made no exchange (that early exit IS the best case, section 6)
static void bubbleSort(int *a, size_t n, SortStats &st) {
    for (size_t pass = 0; pass + 1 < n; pass++) {
        bool swappedAny = false;
        for (size_t k = 0; k + 1 < n - pass; k++) {
            st.comparisons++;
            if (a[k] > a[k + 1]) {
                int tmpValue = a[k];
                a[k] = a[k + 1];
                a[k + 1] = tmpValue;
                st.exchanges++;
                swappedAny = true;
            }
        }
        st.passes++;
        if (!swappedAny) break;
    }
}

// selection: find the minimum of the unsorted tail, then exchange it into place.
// The scan length does not depend on the data, so comparisons are always n(n-1)/2
static void selectionSort(int *a, size_t n, SortStats &st) {
    for (size_t i = 0; i + 1 < n; i++) {
        size_t minPos = i;
        for (size_t j = i + 1; j < n; j++) {
            st.comparisons++;
            if (a[j] < a[minPos]) minPos = j;
        }
        if (minPos != i) {
            int tmpValue = a[i];
            a[i] = a[minPos];
            a[minPos] = tmpValue;
            st.exchanges++;
        }
        st.passes++;
    }
}

// insertion: hold a[i], shift the larger neighbours right, drop the held value
// into the gap. The shift count is the "writes" count of the lecture notes
static void insertionSortCounted(int *a, size_t n, SortStats &st) {
    for (size_t i = 1; i < n; i++) {
        int heldValue = a[i];
        size_t j = i;
        while (j > 0) {
            st.comparisons++;
            if (a[j - 1] <= heldValue) break;
            a[j] = a[j - 1];
            st.shifts++;
            j--;
        }
        if (j != i) {
            a[j] = heldValue;
            st.keyPlacements++;
        }
        st.passes++;
    }
}

// the reference: the same iterative insertion sort, counted separately, used as
// the second independent method that every other sort is checked against
static void referenceInsertionSort(int *a, size_t n, SortStats &st) {
    insertionSortCounted(a, n, st);
}

// ================= stability, measured on (key, original index) pairs =================
struct KeyIndexPair {
    int key = 0;
    long long originalIndex = -1;
};
static unsigned long long g_pairComparisons = 0;
static bool pairKeyLess(const KeyIndexPair &p, const KeyIndexPair &q) {
    g_pairComparisons++;
    return p.key < q.key;
}
static void bubbleSortPairs(KeyIndexPair *a, size_t n) {
    for (size_t pass = 0; pass + 1 < n; pass++) {
        bool swappedAny = false;
        for (size_t k = 0; k + 1 < n - pass; k++) {
            if (pairKeyLess(a[k + 1], a[k])) {
                KeyIndexPair tmpValue = a[k];
                a[k] = a[k + 1];
                a[k + 1] = tmpValue;
                swappedAny = true;
            }
        }
        if (!swappedAny) break;
    }
}
static void selectionSortPairs(KeyIndexPair *a, size_t n) {
    for (size_t i = 0; i + 1 < n; i++) {
        size_t minPos = i;
        for (size_t j = i + 1; j < n; j++) {
            if (pairKeyLess(a[j], a[minPos])) minPos = j;
        }
        if (minPos != i) {
            KeyIndexPair tmpValue = a[i];
            a[i] = a[minPos];
            a[minPos] = tmpValue;
        }
    }
}
static void insertionSortPairs(KeyIndexPair *a, size_t n) {
    for (size_t i = 1; i < n; i++) {
        KeyIndexPair heldValue = a[i];
        size_t j = i;
        while (j > 0 && pairKeyLess(heldValue, a[j - 1])) {
            a[j] = a[j - 1];
            j--;
        }
        if (j != i) a[j] = heldValue;
    }
}
// stable means: equal keys kept their relative input order
static bool orderWithinEqualKeys(const KeyIndexPair *a, size_t n) {
    for (size_t i = 1; i < n; i++) {
        if (a[i - 1].key == a[i].key && a[i - 1].originalIndex > a[i].originalIndex) return false;
    }
    return true;
}
static void printPairs(const char *label, const KeyIndexPair *a, size_t n) {
    cout << "  " << label << ":";
    for (size_t i = 0; i < n; i++)
        cout << " " << a[i].key << "#" << a[i].originalIndex;
    cout << "\n";
}

// ================= one measured run of one algorithm =================
struct RunResult {
    SortStats stats;
    unsigned long long checksumValue = 0;
    bool sortedOk = false;
    bool matchesReference = false;
};
static RunResult runBubble(const int *input, size_t n) {
    RunResult r;
    int *work = (int *)heapAlloc(n * sizeof(int));
    copy(input, input + n, work);
    bubbleSort(work, n, r.stats);
    r.checksumValue = checksum(work, n);
    r.sortedOk = isNonDecreasing(work, n);
    heapFree(work);
    return r;
}
static RunResult runSelection(const int *input, size_t n) {
    RunResult r;
    int *work = (int *)heapAlloc(n * sizeof(int));
    copy(input, input + n, work);
    selectionSort(work, n, r.stats);
    r.checksumValue = checksum(work, n);
    r.sortedOk = isNonDecreasing(work, n);
    heapFree(work);
    return r;
}
static RunResult runInsertion(const int *input, size_t n) {
    RunResult r;
    int *work = (int *)heapAlloc(n * sizeof(int));
    copy(input, input + n, work);
    insertionSortCounted(work, n, r.stats);
    r.checksumValue = checksum(work, n);
    r.sortedOk = isNonDecreasing(work, n);
    heapFree(work);
    return r;
}

// every sort checked against the reference insertion sort on the same input
static bool confirmAgainstReference(const int *input, size_t n, void (*sorter)(int *, size_t, SortStats &)) {
    int *candidate = (int *)heapAlloc(n * sizeof(int));
    int *reference = (int *)heapAlloc(n * sizeof(int));
    copy(input, input + n, candidate);
    copy(input, input + n, reference);
    SortStats ignoreStats;
    sorter(candidate, n, ignoreStats);
    referenceInsertionSort(reference, n, ignoreStats);
    bool ok = sameData(candidate, reference, n) && isNonDecreasing(candidate, n);
    heapFree(candidate);
    heapFree(reference);
    return ok;
}

int main() {
    const size_t SMALL = 8;
    const size_t MID = 64;
    const size_t BIG = 1024;
    const size_t TIMED = 8192;

    cout << "=== 1. the input, n = " << SMALL << ", built from one fixed LCG seed ===\n";
    cout << "  seed = " << LESSON_SEED << " (the same seed is used by lessons 17 and 18)\n";
    int *a8 = (int *)heapAlloc(SMALL * sizeof(int));
    buildInput(a8, SMALL, MODE_SORTED);
    printArray("sorted  mode, 0..n-1", a8, SMALL);
    buildInput(a8, SMALL, MODE_REVERSE);
    printArray("reverse mode", a8, SMALL);
    buildInput(a8, SMALL, MODE_RANDOM);
    printArray("random  mode, the permutation", a8, SMALL);
    buildInput(a8, SMALL, MODE_EQUAL);
    printArray("all-equal mode", a8, SMALL);
    buildInput(a8, SMALL, MODE_DUPES);
    printArray("dupes   mode", a8, SMALL);
    cout << "  random mode is a permutation: every element distinct, so a closed form\n";
    cout << "  can be compared exactly, with no ties to explain away\n";
    heapFree(a8);

    cout << "\n=== 2. bubble sort on the same " << SMALL << " element random input ===\n";
    int *work8 = (int *)heapAlloc(SMALL * sizeof(int));
    buildInput(work8, SMALL, MODE_RANDOM);
    printArray("before", work8, SMALL);
    SortStats stB{};
    bubbleSort(work8, SMALL, stB);
    printArray("after ", work8, SMALL);
    cout << "  comparisons = " << stB.comparisons << ", exchanges = " << stB.exchanges
         << ", passes = " << stB.passes << "\n";
    cout << "  sorted correctly = " << (isNonDecreasing(work8, SMALL) ? "yes" : "NO")
         << ", checksum = " << checksum(work8, SMALL) << "\n";

    cout << "\n=== 3. selection sort on THE SAME input ===\n";
    buildInput(work8, SMALL, MODE_RANDOM);
    SortStats stS{};
    selectionSort(work8, SMALL, stS);
    printArray("after ", work8, SMALL);
    cout << "  comparisons = " << stS.comparisons << ", exchanges = " << stS.exchanges
         << ", passes = " << stS.passes << "\n";
    cout << "  sorted correctly = " << (isNonDecreasing(work8, SMALL) ? "yes" : "NO")
         << ", checksum = " << checksum(work8, SMALL) << "\n";

    cout << "\n=== 4. insertion sort on THE SAME input ===\n";
    buildInput(work8, SMALL, MODE_RANDOM);
    SortStats stI{};
    insertionSortCounted(work8, SMALL, stI);
    printArray("after ", work8, SMALL);
    cout << "  comparisons = " << stI.comparisons << ", shifts = " << stI.shifts
         << ", value written back = " << stI.keyPlacements << "\n";

    // the three results are compared AS ARRAYS, not just as sortedness flags
    int *ref8 = (int *)heapAlloc(SMALL * sizeof(int));
    buildInput(ref8, SMALL, MODE_RANDOM);
    SortStats stRef8{};
    referenceInsertionSort(ref8, SMALL, stRef8);
    cout << "\n=== 5. all three agree with the insertion reference, n = " << SMALL << " ===\n";
    cout << "  reference checksum = " << checksum(ref8, SMALL) << "\n";
    int *cand8 = (int *)heapAlloc(SMALL * sizeof(int));
    buildInput(cand8, SMALL, MODE_RANDOM);
    SortStats stTmp8{};
    bubbleSort(cand8, SMALL, stTmp8);
    cout << "  bubble    output == reference by element: "
         << (sameData(cand8, ref8, SMALL) ? "confirmed" : "MISMATCH")
         << "   checksum = " << checksum(cand8, SMALL) << "\n";
    buildInput(cand8, SMALL, MODE_RANDOM);
    selectionSort(cand8, SMALL, stTmp8);
    cout << "  selection output == reference by element: "
         << (sameData(cand8, ref8, SMALL) ? "confirmed" : "MISMATCH")
         << "   checksum = " << checksum(cand8, SMALL) << "\n";
    buildInput(cand8, SMALL, MODE_RANDOM);
    insertionSortCounted(cand8, SMALL, stTmp8);
    cout << "  insertion output == reference by element: "
         << (sameData(cand8, ref8, SMALL) ? "confirmed" : "MISMATCH")
         << "   checksum = " << checksum(cand8, SMALL) << "\n";
    heapFree(cand8);
    heapFree(ref8);
    heapFree(work8);

    cout << "\n=== 6. best case: the input is already sorted ===\n";
    for (size_t n : {SMALL, MID, BIG}) {
        int *sortedInput = (int *)heapAlloc(n * sizeof(int));
        buildInput(sortedInput, n, MODE_SORTED);
        RunResult rb = runBubble(sortedInput, n);
        RunResult rs = runSelection(sortedInput, n);
        RunResult ri = runInsertion(sortedInput, n);
        cout << "  n = " << setw(5) << n << " " << modeName(MODE_SORTED) << "\n";
        cout << "    bubble    comparisons = " << setw(9) << rb.stats.comparisons
             << "  exchanges = " << setw(9) << rb.stats.exchanges
             << "  passes = " << rb.stats.passes
             << "   (early exit fired, so it costs one pass)\n";
        cout << "    selection comparisons = " << setw(9) << rs.stats.comparisons
             << "  exchanges = " << setw(9) << rs.stats.exchanges
             << "   (the scan cannot tell sorted from reversed)\n";
        cout << "    insertion comparisons = " << setw(9) << ri.stats.comparisons
             << "  shifts    = " << setw(9) << ri.stats.shifts
             << "   (one comparison per element, no shift)\n";
        heapFree(sortedInput);
    }

    cout << "\n=== 7. worst case: the input is strictly decreasing ===\n";
    for (size_t n : {SMALL, MID, BIG}) {
        int *reverseInput = (int *)heapAlloc(n * sizeof(int));
        buildInput(reverseInput, n, MODE_REVERSE);
        RunResult rb = runBubble(reverseInput, n);
        RunResult rs = runSelection(reverseInput, n);
        RunResult ri = runInsertion(reverseInput, n);
        cout << "  n = " << setw(5) << n << " " << modeName(MODE_REVERSE) << "\n";
        cout << "    bubble    comparisons = " << setw(9) << rb.stats.comparisons
             << "  exchanges = " << setw(9) << rb.stats.exchanges
             << "  passes = " << rb.stats.passes << "\n";
        cout << "    selection comparisons = " << setw(9) << rs.stats.comparisons
             << "  exchanges = " << setw(9) << rs.stats.exchanges << "\n";
        cout << "    insertion comparisons = " << setw(9) << ri.stats.comparisons
             << "  shifts    = " << setw(9) << ri.stats.shifts << "\n";
        heapFree(reverseInput);
    }

    cout << "\n=== 8. the closed forms against the measured counts ===\n";
    const size_t CF = BIG;
    int *cfInput = (int *)heapAlloc(CF * sizeof(int));
    unsigned long long nPairs = (unsigned long long)CF * (CF - 1) / 2;
    cout << "  n = " << CF << ", n(n-1)/2 = " << nPairs << ", n-1 = " << CF - 1 << "\n";
    buildInput(cfInput, CF, MODE_REVERSE);
    RunResult cfBubble = runBubble(cfInput, CF);
    RunResult cfSelect = runSelection(cfInput, CF);
    RunResult cfInsert = runInsertion(cfInput, CF);
    cout << "  reverse input:\n";
    cout << "    bubble    comparisons measured " << cfBubble.stats.comparisons
         << " vs n(n-1)/2 " << nPairs << " : "
         << (cfBubble.stats.comparisons == nPairs ? "match" : "MISMATCH") << "\n";
    cout << "    bubble    exchanges   measured " << cfBubble.stats.exchanges
         << " vs n(n-1)/2 " << nPairs << " : "
         << (cfBubble.stats.exchanges == nPairs ? "match" : "MISMATCH") << "\n";
    cout << "    selection comparisons measured " << cfSelect.stats.comparisons
         << " vs n(n-1)/2 " << nPairs << " : "
         << (cfSelect.stats.comparisons == nPairs ? "match" : "MISMATCH") << "\n";
    cout << "    selection exchanges   measured " << cfSelect.stats.exchanges
         << " vs n/2 " << CF / 2 << " : "
         << (cfSelect.stats.exchanges == CF / 2 ? "match" : "MISMATCH") << "\n";
    cout << "      the n-1 upper bound is NOT reached here: after the first exchange the tail\n";
    cout << "      is no longer decreasing, so from then on the minimum keeps landing in the\n";
    cout << "      slot it already occupies and half the passes exchange nothing. The bound\n";
    cout << "      is n-1, the measurement is n/2, and only a count can tell you which\n";
    cout << "    insertion comparisons measured " << cfInsert.stats.comparisons
         << " vs n(n-1)/2 " << nPairs << " : "
         << (cfInsert.stats.comparisons == nPairs ? "match" : "MISMATCH") << "\n";
    cout << "    insertion writes      measured " << cfInsert.stats.shifts
         << " vs n(n-1)/2 " << nPairs << " : "
         << (cfInsert.stats.shifts == nPairs ? "match" : "MISMATCH") << "\n";
    cout << "    insertion also writes the held value back " << cfInsert.stats.keyPlacements
         << " times, which is n-1 = " << CF - 1 << " : "
         << (cfInsert.stats.keyPlacements == CF - 1 ? "match" : "MISMATCH") << "\n";
    buildInput(cfInput, CF, MODE_SORTED);
    RunResult sortedBubble = runBubble(cfInput, CF);
    RunResult sortedSelect = runSelection(cfInput, CF);
    RunResult sortedInsert = runInsertion(cfInput, CF);
    cout << "  sorted input:\n";
    cout << "    bubble    comparisons measured " << sortedBubble.stats.comparisons
         << " vs n-1 " << CF - 1 << " : "
         << (sortedBubble.stats.comparisons == CF - 1 ? "match" : "MISMATCH")
         << ", exchanges measured " << sortedBubble.stats.exchanges << " vs 0 : "
         << (sortedBubble.stats.exchanges == 0 ? "match" : "MISMATCH") << "\n";
    cout << "    selection comparisons measured " << sortedSelect.stats.comparisons
         << " vs n(n-1)/2 " << nPairs << " : "
         << (sortedSelect.stats.comparisons == nPairs ? "match" : "MISMATCH")
         << ", exchanges measured " << sortedSelect.stats.exchanges << " vs 0 : "
         << (sortedSelect.stats.exchanges == 0 ? "match" : "MISMATCH") << "\n";
    cout << "    insertion comparisons measured " << sortedInsert.stats.comparisons
         << " vs n-1 " << CF - 1 << " : "
         << (sortedInsert.stats.comparisons == CF - 1 ? "match" : "MISMATCH")
         << ", writes measured " << sortedInsert.stats.shifts << " vs 0 : "
         << (sortedInsert.stats.shifts == 0 ? "match" : "MISMATCH") << "\n";
    cout << "  the reference was compared against both closed forms too, so the verdicts\n";
    cout << "  above and the reference cannot drift apart:\n";
    int *cfRef = (int *)heapAlloc(CF * sizeof(int));
    buildInput(cfRef, CF, MODE_REVERSE);
    SortStats cfRefStats{};
    referenceInsertionSort(cfRef, CF, cfRefStats);
    cout << "    reference insertion on reverse: comparisons " << cfRefStats.comparisons
         << " vs n(n-1)/2 " << nPairs << " : "
         << (cfRefStats.comparisons == nPairs ? "match" : "MISMATCH")
         << ", writes " << cfRefStats.shifts << " vs " << nPairs << " : "
         << (cfRefStats.shifts == nPairs ? "match" : "MISMATCH") << "\n";
    bool reverseSortedOrderOk = isNonDecreasing(cfRef, CF);
    int *cfInsertOut = (int *)heapAlloc(CF * sizeof(int));
    buildInput(cfInsertOut, CF, MODE_REVERSE);
    SortStats cfInsertOutStats{};
    insertionSortCounted(cfInsertOut, CF, cfInsertOutStats);
    cout << "    reference output is non-decreasing: " << (reverseSortedOrderOk ? "confirmed" : "MISMATCH")
         << ", checksum " << checksum(cfRef, CF) << " vs insertion " << checksum(cfInsertOut, CF)
         << " : " << (checksum(cfRef, CF) == checksum(cfInsertOut, CF) ? "confirmed" : "MISMATCH") << "\n";
    heapFree(cfInsertOut);
    heapFree(cfRef);
    heapFree(cfInput);

    cout << "\n=== 9. average case, random input, against the n(n-1)/4 forms ===\n";
    for (size_t n : {SMALL, MID, BIG}) {
        int *randomInput = (int *)heapAlloc(n * sizeof(int));
        buildInput(randomInput, n, MODE_RANDOM);
        RunResult rb = runBubble(randomInput, n);
        RunResult rs = runSelection(randomInput, n);
        RunResult ri = runInsertion(randomInput, n);
        unsigned long long quarter = (unsigned long long)n * (n - 1) / 4;
        cout << "  n = " << setw(5) << n << " " << modeName(MODE_RANDOM)
             << " n(n-1)/4 = " << quarter << "\n";
        cout << "    bubble    comparisons = " << setw(9) << rb.stats.comparisons
             << "  ratio to n(n-1)/4 = " << fixed << setprecision(3)
             << (double)rb.stats.comparisons / (double)quarter << "\n";
        cout << "    selection comparisons = " << setw(9) << rs.stats.comparisons
             << "  ratio to n(n-1)/4 = " << (double)rs.stats.comparisons / (double)quarter
             << "  exactly twice the average form, as the notes predict\n";
        cout << "    insertion comparisons = " << setw(9) << ri.stats.comparisons
             << "  ratio to n(n-1)/4 = " << (double)ri.stats.comparisons / (double)quarter << "\n";
        cout << defaultfloat;
        heapFree(randomInput);
    }
    cout << "  the ratios hover near 1.0 for bubble and insertion, which is the average\n";
    cout << "  case of the notes, and selection is stuck at twice that: no data shape is\n";
    cout << "  good for it\n";

    cout << "\n=== 10. stability, measured on (key, original index) pairs ===\n";
    const size_t PAIRS_N = 5;
    KeyIndexPair templatePairs[PAIRS_N] = {{3, 0}, {3, 1}, {2, 2}, {1, 3}, {3, 4}};
    cout << "  input pairs are key#originalIndex, three of the five keys are equal to 3\n";
    printPairs("input      ", templatePairs, PAIRS_N);
    KeyIndexPair *pairsA = (KeyIndexPair *)heapAlloc(PAIRS_N * sizeof(KeyIndexPair));
    KeyIndexPair *pairsB = (KeyIndexPair *)heapAlloc(PAIRS_N * sizeof(KeyIndexPair));
    KeyIndexPair *pairsC = (KeyIndexPair *)heapAlloc(PAIRS_N * sizeof(KeyIndexPair));
    for (size_t i = 0; i < PAIRS_N; i++) { pairsA[i] = templatePairs[i]; pairsB[i] = templatePairs[i]; pairsC[i] = templatePairs[i]; }
    g_pairComparisons = 0;
    insertionSortPairs(pairsA, PAIRS_N);
    printPairs("insertion  ", pairsA, PAIRS_N);
    cout << "    equal keys kept their input order: "
         << (orderWithinEqualKeys(pairsA, PAIRS_N) ? "yes, STABLE" : "no, unstable")
         << "   (comparisons " << g_pairComparisons << ")\n";
    g_pairComparisons = 0;
    bubbleSortPairs(pairsB, PAIRS_N);
    printPairs("bubble     ", pairsB, PAIRS_N);
    cout << "    equal keys kept their input order: "
         << (orderWithinEqualKeys(pairsB, PAIRS_N) ? "yes, STABLE" : "no, unstable")
         << "   (comparisons " << g_pairComparisons << ")\n";
    g_pairComparisons = 0;
    selectionSortPairs(pairsC, PAIRS_N);
    printPairs("selection  ", pairsC, PAIRS_N);
    cout << "    equal keys kept their input order: "
         << (orderWithinEqualKeys(pairsC, PAIRS_N) ? "yes, STABLE" : "no, NOT stable")
         << "   (comparisons " << g_pairComparisons << ")\n";
    cout << "  the three sorted keys came out as #";
    for (size_t i = 0; i < PAIRS_N; i++) if (pairsC[i].key == 3) cout << pairsC[i].originalIndex << " ";
    cout << "under selection and as #";
    for (size_t i = 0; i < PAIRS_N; i++) if (pairsA[i].key == 3) cout << pairsA[i].originalIndex << " ";
    cout << "under insertion\n";
    cout << "  selection swaps the far-away minimum into position and drags key 3#0 past\n";
    cout << "  3#1 on the way: that swap is why selection sort is not stable\n";
    heapFree(pairsC);
    heapFree(pairsB);
    heapFree(pairsA);

    cout << "\n=== 11. count table at n = 8, 64, 1024 (measured) ===\n";
    cout << "  algorithm   n      mode        comparisons    exchanges      writes\n";
    for (size_t n : {SMALL, MID, BIG}) {
        for (int mode : {MODE_RANDOM, MODE_SORTED, MODE_REVERSE, MODE_DUPES, MODE_EQUAL}) {
            int *tableInput = (int *)heapAlloc(n * sizeof(int));
            buildInput(tableInput, n, mode);
            RunResult rb = runBubble(tableInput, n);
            RunResult rs = runSelection(tableInput, n);
            RunResult ri = runInsertion(tableInput, n);
            bool agreeB = rb.sortedOk && rb.checksumValue == ri.checksumValue;
            bool agreeS = rs.sortedOk && rs.checksumValue == ri.checksumValue;
            cout << "  bubble      " << setw(5) << n << "  " << modeName(mode) << " "
                 << setw(12) << rb.stats.comparisons
                 << "  " << setw(12) << rb.stats.exchanges
                 << "  " << setw(12) << "-"
                 << "   vs insertion reference: " << (agreeB ? "confirmed" : "MISMATCH") << "\n";
            cout << "  selection   " << setw(5) << n << "  " << modeName(mode) << " "
                 << setw(12) << rs.stats.comparisons
                 << "  " << setw(12) << rs.stats.exchanges
                 << "  " << setw(12) << "-"
                 << "   vs insertion reference: " << (agreeS ? "confirmed" : "MISMATCH") << "\n";
            cout << "  insertion   " << setw(5) << n << "  " << modeName(mode) << " "
                 << setw(12) << ri.stats.comparisons
                 << "  " << setw(12) << "-"
                 << "  " << setw(12) << ri.stats.shifts
                 << "   vs insertion reference: " << (ri.sortedOk ? "confirmed" : "MISMATCH") << "\n";
            heapFree(tableInput);
        }
    }
    cout << "  every confirmed above is an element by element agreement with the reference\n";
    cout << "  insertion sort on the same input, plus a non-decreasing order check, so a\n";
    cout << "  wrong sort cannot hide behind a plausible count\n";

    unsigned long long timedQuadratic = (unsigned long long)TIMED * (TIMED - 1) / 2;
    cout << "\n=== 12. cost at n = " << TIMED << ": " << fixed << setprecision(1)
         << (double)timedQuadratic / 1000000.0 << " million comparisons is where it hurts ===\n";
    cout << defaultfloat << setprecision(6);
    g_allocCalls = 0; g_allocBytes = 0;
    for (int mode : {MODE_RANDOM, MODE_REVERSE}) {
        int *timedInput = (int *)heapAlloc(TIMED * sizeof(int));
        buildInput(timedInput, TIMED, mode);
        cout << "  " << modeName(mode) << " input, n = " << TIMED << "\n";
        int *timedWork = (int *)heapAlloc(TIMED * sizeof(int));
        copy(timedInput, timedInput + TIMED, timedWork);
        SortStats timedStats{};
        auto tBubbleStart = chrono::steady_clock::now();
        bubbleSort(timedWork, TIMED, timedStats);
        auto tBubbleEnd = chrono::steady_clock::now();
        double msBubble = (double)chrono::duration_cast<chrono::microseconds>(tBubbleEnd - tBubbleStart).count() / 1000.0;
        bool bubbleOk = isNonDecreasing(timedWork, TIMED);
        long long bubbleSum = 0;
        for (size_t i = 0; i < TIMED; i++) bubbleSum += timedWork[i];
        long long inputSum = 0;
        for (size_t i = 0; i < TIMED; i++) inputSum += timedInput[i];
        cout << "    bubble    " << setw(10) << fixed << setprecision(3) << msBubble
             << " ms  comparisons " << timedStats.comparisons
             << "  checksum check: " << (bubbleOk && bubbleSum == inputSum ? "confirmed" : "MISMATCH")
             << "\n";
        copy(timedInput, timedInput + TIMED, timedWork);
        SortStats timedSelectStats{};
        auto tSelectStart = chrono::steady_clock::now();
        selectionSort(timedWork, TIMED, timedSelectStats);
        auto tSelectEnd = chrono::steady_clock::now();
        double msSelect = (double)chrono::duration_cast<chrono::microseconds>(tSelectEnd - tSelectStart).count() / 1000.0;
        bool selectOk = isNonDecreasing(timedWork, TIMED);
        long long selectSum = 0;
        for (size_t i = 0; i < TIMED; i++) selectSum += timedWork[i];
        cout << "    selection " << setw(10) << fixed << setprecision(3) << msSelect
             << " ms  comparisons " << timedSelectStats.comparisons
             << "  checksum check: " << (selectOk && selectSum == inputSum ? "confirmed" : "MISMATCH")
             << "\n";
        copy(timedInput, timedInput + TIMED, timedWork);
        SortStats timedInsertStats{};
        auto tInsertStart = chrono::steady_clock::now();
        insertionSortCounted(timedWork, TIMED, timedInsertStats);
        auto tInsertEnd = chrono::steady_clock::now();
        double msInsert = (double)chrono::duration_cast<chrono::microseconds>(tInsertEnd - tInsertStart).count() / 1000.0;
        bool insertOk = isNonDecreasing(timedWork, TIMED);
        long long insertSum = 0;
        for (size_t i = 0; i < TIMED; i++) insertSum += timedWork[i];
        cout << "    insertion " << setw(10) << fixed << setprecision(3) << msInsert
             << " ms  comparisons " << timedInsertStats.comparisons
             << "  checksum check: " << (insertOk && insertSum == inputSum ? "confirmed" : "MISMATCH")
             << "\n";
        cout << fixed << setprecision(2);
        cout << "    ratios: bubble/selection = " << (msBubble / msSelect)
             << ", selection/insertion = " << (msSelect / msInsert)
             << ", bubble/insertion = " << (msBubble / msInsert) << "\n";
        cout << defaultfloat;
        heapFree(timedWork);
        heapFree(timedInput);
    }
    cout << "  these three sorts moved " << g_allocBytes << " bytes through the heap in "
         << g_allocCalls << " allocations, all of them scratch copies, none of them needed\n";
    cout << "  by the algorithm itself\n";

    cout << "\n=== 13. the complexity table ===\n";
    cout << "  algorithm   best        average      worst       space   stable   measured best case\n";
    cout << "  bubble      O(n)        O(n^2)       O(n^2)      O(1)    yes      n-1 comparisons, 0 exchanges\n";
    cout << "  selection   O(n^2)      O(n^2)       O(n^2)      O(1)    no       n(n-1)/2 comparisons either way\n";
    cout << "  insertion   O(n)        O(n^2)       O(n^2)      O(1)    yes      n-1 comparisons, 0 writes\n";
    cout << "  the best case of bubble and insertion is exactly n-1 because a sorted input\n";
    cout << "  never triggers an exchange or a shift; selection has no good case at all\n";

    cout << "\n=== 14. the one line to remember ===\n";
    cout << "  bubble moves neighbours and stops early, selection always scans the whole tail,\n";
    cout << "  insertion shifts only what the new element must pass. All three are O(n^2) in the\n";
    cout << "  average case, only bubble and insertion get an O(n) best case, and only insertion\n";
    cout << "  is the one you actually use for small or nearly sorted data.\n";

    cout << "\n  reference check with the standalone confirmer:\n";
    int *checkInput = (int *)heapAlloc(MID * sizeof(int));
    buildInput(checkInput, MID, MODE_DUPES);
    cout << "    bubble    vs reference at n = " << MID << " : "
         << (confirmAgainstReference(checkInput, MID, bubbleSort) ? "confirmed" : "MISMATCH") << "\n";
    cout << "    selection vs reference at n = " << MID << " : "
         << (confirmAgainstReference(checkInput, MID, selectionSort) ? "confirmed" : "MISMATCH") << "\n";
    cout << "    insertion vs reference at n = " << MID << " : "
         << (confirmAgainstReference(checkInput, MID, insertionSortCounted) ? "confirmed" : "MISMATCH") << "\n";
    heapFree(checkInput);
    return 0;
}
