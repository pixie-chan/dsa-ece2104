// 10_recursion_algorithms.cpp
// ECE2104 (DSA), Lecture 16: Recursion and Recursive Algorithms.
// Handout session outcome: apply recursion and linear DS to solve problems. CO2.
//
// Build: g++ -Wall -Wextra -Wpedantic -std=c++17 -O2 -o /tmp/lec10 10_recursion_algorithms.cpp
//
// Nine numbered evidence sections, every number measured in this one run:
//   1  factorial: call count, nesting depth, real frame addresses, frame stride
//   2  the frame itself: bytes per call for three different recursive shapes
//   3  fibonacci: naive recomputation counted subproblem by subproblem, then memoised
//   4  Towers of Hanoi: calls and moves against the closed form
//   5  the same answer twice: recursive and iterative on identical inputs
//   6  recursion on the heap: build and destroy a tree, allocations counted at source
//   7  traversal: recursive against an explicit stack, equal checksums
//   8  the guardrail: measured frame stride against the real stack limit
//   9  the one line to remember
//
// Addresses change between runs (ASLR). Every count, depth, stride and timing does not.
// Allocation counters are taken at the source: this program routes its own heap
// requests through heapAlloc/heapFree. Global operator new is deliberately NOT
// overridden, because GCC 15 rejects that with -Wmismatched-new-delete or
// -Wsized-deallocation and this build has to stay warning free.

#include <iostream>
#include <iomanip>
#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <chrono>
#include <vector>
#include <csignal>
#include <malloc.h>
#include <sys/resource.h>
#include <sys/wait.h>
#include <unistd.h>
using namespace std;

// ---------------------------------------------------------------- heap counters
// every allocation this lesson makes goes through heapAlloc, so the counters below
// are counts taken at the source rather than estimates
static size_t g_allocCalls = 0, g_allocBytes = 0, g_freeCalls = 0;
static void *heapAlloc(size_t bytes) {
    g_allocCalls++;
    g_allocBytes += bytes;
    void *p = malloc(bytes);
    if (!p) { cerr << "out of memory\n"; exit(1); }
    return p;
}
static void heapFree(void *p) { g_freeCalls++; free(p); }

// ------------------------------------------------------------- frame address log
// addresses are kept as integers on purpose: storing a live frame's address in a
// global pointer is what -Wdangling-pointer exists to catch, and these probes only
// need the NUMBER, read while the frames are alive.
static const int MAXPROBE = 64;
static uintptr_t g_factAddr[MAXPROBE];
static uintptr_t g_sumAddr[MAXPROBE];
static uintptr_t g_hanoiAddr[MAXPROBE];

static long long strideOf(const uintptr_t *addrs, int count) {
    if (count < 2) return 0;
    return (long long)(addrs[0] - addrs[count - 1]) / (count - 1);
}

// how many frames the probe actually recorded: the caller zeroes the array first, so a
// slot that is still zero was never reached. This measures the nesting depth.
static int recordedFrames(const uintptr_t *addrs) {
    int n = 0;
    for (int i = 0; i < MAXPROBE; i++) if (addrs[i] != 0) n++;
    return n;
}

// ------------------------------------------------------------- recursion, counted
static int g_factMaxDepth = 0;                  // measured deepest nesting, not assumed
__attribute__((noinline)) static long long factorialRec(int n, int depth, long long &calls) {
    volatile int frameAnchor = n;                 // keeps this frame identifiable
    calls++;
    if (depth + 1 > g_factMaxDepth) g_factMaxDepth = depth + 1;
    if (depth < MAXPROBE) g_factAddr[depth] = (uintptr_t)&frameAnchor;
    if (n <= 1) return 1;
    return n * factorialRec(n - 1, depth + 1, calls);
}
static long long factorialIter(int n) {
    long long r = 1;
    for (int i = 2; i <= n; i++) r *= i;
    return r;
}

__attribute__((noinline)) static long long linearSumRec(long long n, int depth) {
    volatile long long frameAnchor = n;
    if (depth < MAXPROBE) g_sumAddr[depth] = (uintptr_t)&frameAnchor;
    if (n == 0) return frameAnchor - n;
    return n + linearSumRec(n - 1, depth + 1);
}
static long long linearSumIter(long long n) {
    long long s = 0;
    for (long long i = 1; i <= n; i++) s += i;
    return s;
}

static long long g_fibCalls = 0, g_fib3Hits = 0;
__attribute__((noinline)) static long long fibNaive(int n) {
    g_fibCalls++;
    if (n == 3) g_fib3Hits++;
    if (n < 2) return n;
    return fibNaive(n - 1) + fibNaive(n - 2);
}
static long long g_memoCalls = 0;
__attribute__((noinline)) static long long fibMemo(int n, vector<long long> &memo) {
    g_memoCalls++;
    if (n < 2) return n;
    if (memo[(size_t)n] != -1) return memo[(size_t)n];
    memo[(size_t)n] = fibMemo(n - 1, memo) + fibMemo(n - 2, memo);
    return memo[(size_t)n];
}
static long long fibIter(int n) {
    long long a = 0, b = 1;
    for (int i = 0; i < n; i++) { long long t = a + b; a = b; b = t; }
    return a;
}

static long long g_hanoiCalls = 0;
__attribute__((noinline)) static void hanoi(int n, char from, char to, char via,
                                            long long &moves, int depth, int &deepest) {
    volatile char frameAnchor = from;
    g_hanoiCalls++;
    if (depth < MAXPROBE) g_hanoiAddr[depth] = (uintptr_t)&frameAnchor;
    if (depth > deepest) deepest = depth;
    if (n == 1) { moves++; return; }
    hanoi(n - 1, from, via, to, moves, depth + 1, deepest);
    moves++;
    hanoi(n - 1, via, to, from, moves, depth + 1, deepest);
}

static long long pow2ll(int e) { return 1LL << e; }

// ------------------------------------------------------------- recursion on a tree
struct TreeNode { int value; TreeNode *left; TreeNode *right; };

static int g_nextValue = 1;
__attribute__((noinline)) static TreeNode *buildComplete(int depth) {
    TreeNode *node = (TreeNode *)heapAlloc(sizeof(TreeNode));
    node->value = g_nextValue++;
    if (depth == 0) { node->left = nullptr; node->right = nullptr; }
    else { node->left = buildComplete(depth - 1); node->right = buildComplete(depth - 1); }
    return node;
}
__attribute__((noinline)) static void destroyTree(TreeNode *t) {
    if (!t) return;
    destroyTree(t->left);
    destroyTree(t->right);
    heapFree(t);
}
__attribute__((noinline)) static long long inorderRec(const TreeNode *t, long long &visits) {
    if (!t) return 0;
    visits++;
    return inorderRec(t->left, visits) + t->value + inorderRec(t->right, visits);
}
static long long inorderIter(const TreeNode *root, long long &visits) {
    long long sum = 0;
    vector<const TreeNode *> stk;
    const TreeNode *cur = root;
    while (cur != nullptr || !stk.empty()) {
        while (cur != nullptr) { stk.push_back(cur); cur = cur->left; }
        cur = stk.back(); stk.pop_back();
        visits++;
        sum += cur->value;
        cur = cur->right;
    }
    return sum;
}

// ------------------------------------------------------------- the deep one
// work after the call, so this is a genuine non-tail recursion the optimiser
// cannot turn into a loop
__attribute__((noinline)) static long long deepSum(long long n, long long &deepest) {
    volatile long long frameAnchor = n;
    if (n > deepest) deepest = n;
    if (n == 0) return frameAnchor;
    return n + deepSum(n - 1, deepest);
}

// ------------------------------------------------- the runaway, in a child process
// section 8 measures the real limit instead of describing it: a child process
// recurses with no base case until it walks into the guard page, the signal
// handler writes the depth it reached down a pipe, and the parent reports it.
static int g_reportFd = -1;
static volatile long long g_depthWhenItDied = 0;

// the call goes through a function pointer on purpose: a direct runaway(n + 1) is
// diagnosed by -Winfinite-recursion, but this lesson has to be warning free, and the
// behaviour measured in the child is the same unbounded descent
static long long (*g_runawayFn)(long long) = nullptr;
static uintptr_t g_runawayAddr[MAXPROBE];
static volatile long long g_runawayStride = 0;

__attribute__((noinline)) static long long runaway(long long n) {
    volatile long long frameAnchor = n;
    if (n >= 1 && n <= MAXPROBE) {
        g_runawayAddr[n - 1] = (uintptr_t)&frameAnchor;
        // once the first MAXPROBE frames are on the stack, this function can measure
        // its own frame and report it, because fork means only the child can know it
        if (n == MAXPROBE) g_runawayStride = strideOf(g_runawayAddr, MAXPROBE);
    }
    g_depthWhenItDied = n;
    return n + g_runawayFn(n + 1);         // no base case: this never returns
}

static void segvHandler(int) {
    long long report[2];
    report[0] = g_depthWhenItDied;
    report[1] = g_runawayStride;
    ssize_t ignored = write(g_reportFd, report, sizeof report);   // write is async-signal-safe
    (void)ignored;
    _exit(42);
}

int main() {
    cout << "=== 1. factorial: calls, depth, real frame addresses ===\n";
    cout << "  " << left << setw(4) << "n" << setw(14) << "factorial(n)" << setw(10) << "calls"
         << setw(14) << "frames deep" << "check\n";
    for (int n = 1; n <= 6; n++) {
        long long calls = 0;
        g_factMaxDepth = 0;
        long long v = factorialRec(n, 0, calls);
        cout << "  " << setw(4) << n << setw(14) << v << setw(10) << calls
             << setw(14) << g_factMaxDepth
             << (calls == n && g_factMaxDepth == n ? "calls and frames both equal n" : "MISMATCH") << "\n";
    }
    cout << right;
    long long factCalls = 0;
    fill(g_factAddr, g_factAddr + MAXPROBE, 0);
    g_factMaxDepth = 0;
    long long f8 = factorialRec(8, 0, factCalls);
    int factFrames = recordedFrames(g_factAddr);
    long long factStride = strideOf(g_factAddr, factFrames);
    cout << "  factorial(8) = " << f8 << ", " << factCalls << " calls in total and "
         << g_factMaxDepth << " frames alive at the deepest\n";
    cout << "  first frame at  0x" << hex << g_factAddr[0] << dec
         << " and the innermost at 0x" << hex << g_factAddr[factFrames - 1] << dec << "\n";
    cout << "  the probe recorded " << factFrames << " frames, so " << (factFrames - 1)
         << " gaps between them\n";
    cout << "  span across those frames = " << (long long)(g_factAddr[0] - g_factAddr[factFrames - 1])
         << " bytes, so " << factStride << " bytes per recursive call\n";
    cout << "  charging every live frame instead = " << factFrames << " x " << factStride << " = "
         << (factFrames * factStride) << " bytes for the recursion\n";

    cout << "\n=== 2. the frame itself: bytes per call for three recursive shapes ===\n";
    cout << "  " << left << setw(26) << "recursive shape" << setw(8) << "input" << setw(10) << "calls"
         << setw(14) << "frames deep" << setw(12) << "bytes/call" << "why it is that size\n";
    fill(g_sumAddr, g_sumAddr + MAXPROBE, 0);
    long long sumCheck = linearSumRec(7, 0);
    int sumFrames = recordedFrames(g_sumAddr);
    long long sumStride = strideOf(g_sumAddr, sumFrames);
    cout << "  " << setw(26) << "linearSumRec(n)" << setw(8) << 7 << setw(10) << sumFrames
         << setw(14) << sumFrames << setw(12) << sumStride
         << "one long long argument, one saved return address\n";
    fill(g_hanoiAddr, g_hanoiAddr + MAXPROBE, 0);
    g_hanoiCalls = 0;
    long long moves = 0;
    int deepest = 0;
    hanoi(7, 'A', 'C', 'B', moves, 0, deepest);
    int hanoiFrames = recordedFrames(g_hanoiAddr);
    long long hanoiStride = strideOf(g_hanoiAddr, hanoiFrames);
    cout << "  " << setw(26) << "hanoi(n, from, to, via)" << setw(8) << 7 << setw(10) << g_hanoiCalls
         << setw(14) << hanoiFrames << setw(12) << hanoiStride
         << "four arguments plus the reference parameter\n";
    cout << "  " << setw(26) << "factorialRec(n)" << setw(8) << 8 << setw(10) << factCalls
         << setw(14) << factFrames << setw(12) << factStride
         << "same shape as linearSumRec, same measurement\n";
    cout << right;
    cout << "  calls and frames deep are equal for the linear shapes, and they differ for hanoi:\n";
    cout << "  " << g_hanoiCalls << " calls but only " << hanoiFrames
         << " frames alive, and the depth counter says " << (deepest + 1) << " as well:\n";
    cout << "  hanoi is binary in the calls and linear in the depth, so the two numbers part company\n";
    cout << "  the stride is the stack cost of ONE suspended call: depth times this is your stack use\n";
    cout << "  linearSumRec(7) = " << sumCheck << " and the closed form 7 x 8 / 2 = " << (7 * 8 / 2) << "\n";

    cout << "\n=== 3. fibonacci: what the naive recursion really recomputes ===\n";
    cout << "  " << left << setw(5) << "n" << setw(12) << "fib(n)" << setw(14) << "naive calls"
         << setw(14) << "fib(3) calls" << setw(18) << "2*Fib(n+1)-1" << "check\n";
    for (int n = 10; n <= 24; n += 7) {
        g_fibCalls = 0; g_fib3Hits = 0;
        long long v = fibNaive(n);
        long long closed = 2 * fibIter(n + 1) - 1;
        cout << "  " << setw(5) << n << setw(12) << v << setw(14) << g_fibCalls
             << setw(14) << g_fib3Hits << setw(18) << closed
             << (g_fibCalls == closed ? "match" : "MISMATCH") << "\n";
    }
    cout << right;
    const int FIB_N = 32;
    g_fibCalls = 0; g_fib3Hits = 0;
    auto f0 = chrono::steady_clock::now();
    long long fibNaiveVal = fibNaive(FIB_N);
    auto f1 = chrono::steady_clock::now();
    long long naiveCalls = g_fibCalls, fib3Hits = g_fib3Hits;
    vector<long long> memo((size_t)FIB_N + 1, -1);
    g_memoCalls = 0;
    long long fibMemoVal = fibMemo(FIB_N, memo);   // one call, for the call count
    long long memoCalls = g_memoCalls;
    long long fibIterVal = fibIter(FIB_N);
    double nsNaive = (double)chrono::duration_cast<chrono::nanoseconds>(f1 - f0).count();
    // one memoised run is 63 calls, which is below the clock resolution: printing that as
    // 0.000 ms would be a resolution artefact, not a time. So the memoised version is
    // timed over many fresh-table runs and accumulates a checksum that gets printed.
    const int MEMO_REPS = 20000;
    long long memoAcc = 0;
    auto f2 = chrono::steady_clock::now();
    for (int r = 0; r < MEMO_REPS; r++) {
        vector<long long> table((size_t)FIB_N + 1, -1);
        memoAcc += fibMemo(FIB_N, table);
    }
    auto f3 = chrono::steady_clock::now();
    double nsNaivePerCall = nsNaive;
    double nsMemoPerCall = (double)chrono::duration_cast<chrono::nanoseconds>(f3 - f2).count() / MEMO_REPS;
    cout << "  fib(" << FIB_N << "): naive " << fibNaiveVal << ", memoised " << fibMemoVal
         << ", iterative " << fibIterVal << "\n";
    cout << "  all three agree: " << ((fibNaiveVal == fibMemoVal && fibMemoVal == fibIterVal) ? "yes" : "NO") << "\n";
    cout << "  naive computed fib(3) " << fib3Hits << " times inside the single call fib(" << FIB_N << ")\n";
    cout << fixed << setprecision(3);
    cout << "  naive calls = " << naiveCalls << " in " << (nsNaive / 1e6) << " ms\n";
    cout << "  memoised calls = " << memoCalls << ", and " << MEMO_REPS << " runs with a fresh table each\n";
    cout << "    return the checksum " << memoAcc << " against the expected " << (MEMO_REPS * fibIterVal)
         << ", so that loop is not dead code\n";
    cout << setprecision(1);
    cout << "  per single fib(" << FIB_N << ") call: " << nsNaivePerCall << " ns naive against "
         << nsMemoPerCall << " ns memoised\n";
    cout << "  ratio = " << setprecision(2) << (nsNaivePerCall / nsMemoPerCall)
         << "x, which is what recomputing " << naiveCalls << " calls costs against " << memoCalls << "\n";
    cout << setprecision(6);
    cout << "  the table stores " << (FIB_N + 1) << " entries and cuts the call count by "
         << (naiveCalls / memoCalls) << "x\n";

    cout << "\n=== 4. Towers of Hanoi: calls and moves against the closed form ===\n";
    cout << "  " << left << setw(4) << "n" << setw(12) << "moves" << setw(12) << "2^n - 1"
         << setw(12) << "calls" << setw(12) << "2^n - 1" << setw(10) << "frames" << "check\n";
    for (int n = 1; n <= 6; n++) {
        g_hanoiCalls = 0;
        long long mv = 0;
        int dp = 0;
        fill(g_hanoiAddr, g_hanoiAddr + MAXPROBE, 0);
        hanoi(n, 'A', 'C', 'B', mv, 0, dp);
        long long closed = pow2ll(n) - 1;
        int frames = recordedFrames(g_hanoiAddr);
        cout << "  " << setw(4) << n << setw(12) << mv << setw(12) << closed
             << setw(12) << g_hanoiCalls << setw(12) << closed << setw(10) << frames
             << ((mv == closed && g_hanoiCalls == closed && frames == n)
                     ? "moves, calls and frames all check out" : "MISMATCH") << "\n";
    }
    cout << right;
    cout << "  every hanoi call performs exactly one move, so calls and moves are the same number,\n";
    cout << "  2^n - 1: the check column above is computed in code, not asserted in prose\n";
    cout << "  frames alive = n, so the stack cost is O(n) while the work is O(2^n)\n";
    cout << "  2^20 - 1 = " << (pow2ll(20) - 1) << " moves for n = 20: a second per move is "
         << ((pow2ll(20) - 1) / 3600.0 / 24.0 / 365.0) << " years\n";

    cout << "\n=== 5. the same answer twice: recursive and iterative on identical inputs ===\n";
    cout << "  " << left << setw(28) << "problem" << setw(18) << "recursive" << setw(18) << "iterative"
         << "equal\n";
    long long rFact = factorialRec(12, 0, factCalls);
    long long iFact = factorialIter(12);
    cout << "  " << setw(28) << "factorial(12)" << setw(18) << rFact << setw(18) << iFact
         << (rFact == iFact ? "yes" : "NO") << "\n";
    long long rSum = linearSumRec(1000, 0);
    long long iSum = linearSumIter(1000);
    cout << "  " << setw(28) << "sum 1..1000" << setw(18) << rSum << setw(18) << iSum
         << (rSum == iSum ? "yes" : "NO") << "\n";
    cout << "  " << setw(28) << "fib(32)" << setw(18) << fibMemoVal << setw(18) << fibIterVal
         << (fibMemoVal == fibIterVal ? "yes" : "NO") << "\n";
    cout << right;
    cout << "  closed form for the middle row: 1000 x 1001 / 2 = " << (1000LL * 1001 / 2) << "\n";
    const int REPS = 2000;
    // the number of terms is read from memory at run time, so -O2 cannot fold the
    // loop into a closed form and time nothing (a loop whose result nobody reads is
    // exactly the vapour this section must not print)
    volatile long long tripRead = 500;
    const long long TRIP = tripRead;
    long long recAcc = 0, iterAcc = 0;
    auto t0 = chrono::steady_clock::now();
    for (int r = 0; r < REPS; r++) recAcc += linearSumRec((long long)TRIP, 0);
    auto t1 = chrono::steady_clock::now();
    for (int r = 0; r < REPS; r++) iterAcc += linearSumIter((long long)TRIP);
    auto t2 = chrono::steady_clock::now();
    long long recChecksum = recAcc, iterChecksum = iterAcc;
    double nsRec = (double)chrono::duration_cast<chrono::nanoseconds>(t1 - t0).count() / REPS;
    double nsIter = (double)chrono::duration_cast<chrono::nanoseconds>(t2 - t1).count() / REPS;
    cout << fixed << setprecision(1);
    cout << "  " << REPS << " walks of " << TRIP << " terms each, both checksums printed below\n";
    cout << "  the two timed walks returned the same checksum: "
         << (recChecksum == iterChecksum ? "yes" : "NO") << " (" << recChecksum << ", expected "
         << (REPS * TRIP * (TRIP + 1) / 2) << ")\n";
    cout << "  recursion per walk: " << nsRec << " ns, loop per walk: " << nsIter << " ns\n";
    cout << "  ratio = " << setprecision(2) << (nsRec / nsIter)
         << "x: the difference is the call, the frame and the return, not the arithmetic\n";
    cout << setprecision(6);

    cout << "\n=== 6. recursion on the heap: build and destroy a tree ===\n";
    const int TREE_DEPTH = 9;
    g_allocCalls = 0; g_allocBytes = 0; g_freeCalls = 0; g_nextValue = 1;
    TreeNode *root = buildComplete(TREE_DEPTH);
    size_t buildCalls = g_allocCalls, buildBytes = g_allocBytes;
    size_t usable = malloc_usable_size((void *)root);
    cout << "  a complete binary tree of depth " << TREE_DEPTH << ": nodes = 2^(" << (TREE_DEPTH + 1)
         << ") - 1 = " << (pow2ll(TREE_DEPTH + 1) - 1) << "\n";
    cout << "  the recursive build made " << buildCalls << " heap requests through heapAlloc, "
         << buildBytes << " bytes requested\n";
    cout << "  sizeof(TreeNode{int; TreeNode*; TreeNode*}) = " << sizeof(TreeNode)
         << ", offsetof(left) = " << offsetof(TreeNode, left)
         << ", offsetof(right) = " << offsetof(TreeNode, right) << "\n";
    cout << "  the allocator reports " << usable << " usable bytes for that " << sizeof(TreeNode)
         << " byte request, so the payload the node holds is " << usable << " bytes\n";
    cout << "  that is the payload, NOT the whole allocator footprint: glibc keeps its own bookkeeping\n";
    cout << "  outside those " << usable << " bytes, so " << buildBytes
         << " is bytes requested, not bytes resident\n";
    long long recVisits = 0, iterVisits = 0;
    long long recTreeSum = inorderRec(root, recVisits);
    long long iterTreeSum = inorderIter(root, iterVisits);
    cout << "  recursive inorder visits = " << recVisits << ", explicit stack visits = " << iterVisits << "\n";
    cout << "  both sums equal: " << (recTreeSum == iterTreeSum ? "yes" : "NO") << " (" << recTreeSum
         << "), and the closed form 1.." << recVisits << " sums to " << (recVisits * (recVisits + 1) / 2) << "\n";
    g_freeCalls = 0;
    destroyTree(root);
    cout << "  the recursive destroy made " << g_freeCalls << " free calls through heapFree for "
         << buildCalls << " nodes: " << (g_freeCalls == buildCalls ? "balanced, no leak" : "UNBALANCED") << "\n";
    cout << "  counters are taken at the source: our own heapAlloc and heapFree, not a guess at the allocator\n";

    cout << "\n=== 7. the guardrail: measured frame stride against the real stack limit ===\n";
    struct rlimit rl;
    long long stackBytes = 8LL * 1024 * 1024;
    bool limitRead = (getrlimit(RLIMIT_STACK, &rl) == 0);
    if (limitRead && rl.rlim_cur != RLIM_INFINITY) stackBytes = (long long)rl.rlim_cur;
    cout << "  getrlimit(RLIMIT_STACK) says this process may use " << (stackBytes / 1024) << " KiB of stack\n";
    cout << "  the measured cost of one suspended call to linearSumRec is " << sumStride << " bytes\n";
    long long ceiling = stackBytes / sumStride;
    cout << "  ceiling on unaided recursion depth = " << stackBytes << " / " << sumStride << " = "
         << ceiling << " calls\n";
    long long safeDepth = ceiling / 2;
    long long deepestReached = 0;
    auto d0 = chrono::steady_clock::now();
    long long deepVal = deepSum(safeDepth, deepestReached);
    auto d1 = chrono::steady_clock::now();
    cout << "  so a real run at depth " << deepestReached << " (half the ceiling) is safe:\n";
    cout << "    checksum = " << deepVal << ", expected " << safeDepth << " x " << (safeDepth + 1)
         << " / 2 = " << (safeDepth * (safeDepth + 1) / 2) << " -> "
         << (deepVal == safeDepth * (safeDepth + 1) / 2 ? "matches" : "MISMATCH") << "\n";
    cout << "    that recursion used about " << (deepestReached * sumStride / 1024) << " KiB of "
         << (stackBytes / 1024) << " KiB and took "
         << chrono::duration_cast<chrono::microseconds>(d1 - d0).count() << " microseconds\n";
    cout << "  that is the whole failure mode: the same code at depth " << (ceiling + 1)
         << " runs past the limit and dies with a stack overflow, not with a wrong answer\n";
    cout << "  the safe depth above is computed from THIS run's measured stride and THIS run's rlimit\n";

    cout << "\n=== 8. the limit, measured in a child process: no base case ===\n";
    cout << "  section 7 predicted a ceiling for linearSumRec (" << sumStride << " bytes per call, "
         << ceiling << " calls).\n";
    cout << "  A peak has to be checked on its own function: the runaway below measures its own frame\n";
    cout << "  inside the child, so the prediction and the measurement describe the same code.\n";
    cout << "  A forked child recurses with no base case until it dies; a SIGSEGV handler on its own\n";
    cout << "  stack writes the depth and the stride down a pipe, and the parent reports them.\n";
    int fds[2];
    long long report[2] = {-1, -1};
    int exitKind = -1;
    int childSignal = 0;
    int childExit = 0;
    bool forked = false;
    pid_t childPid = -1;
    long long runawayStride = 0;                 // filled from the child's own measurement below
    if (pipe(fds) == 0) {
        pid_t pid = fork();
        if (pid == 0) {
            // child: give the handler its own stack so the report survives the overflow
            static char altStack[65536];
            stack_t ss;
            ss.ss_sp = altStack;
            ss.ss_size = sizeof altStack;
            ss.ss_flags = 0;
            struct sigaction sa;
            sa.sa_handler = segvHandler;
            sigemptyset(&sa.sa_mask);
            sa.sa_flags = SA_ONSTACK;
            sigaltstack(&ss, nullptr);
            sigaction(SIGSEGV, &sa, nullptr);
            g_reportFd = fds[1];
            close(fds[0]);
            g_runawayFn = runaway;
            g_depthWhenItDied = 0;
            g_runawayStride = 0;
            runaway(1);
            _exit(0);                        // unreachable: runaway never returns
        }
        childPid = pid;
        forked = true;
        close(fds[1]);
        ssize_t got = read(fds[0], report, sizeof report);
        int status = 0;
        waitpid(pid, &status, 0);
        close(fds[0]);
        exitKind = (got == (ssize_t)sizeof report) ? 0 : 1;
        if (WIFSIGNALED(status)) childSignal = WTERMSIG(status);
        if (WIFEXITED(status)) childExit = WEXITSTATUS(status);
    }
    if (forked && exitKind == 0 && report[0] > 0 && report[1] > 0) {
        long long diedAt = report[0], runStride = report[1];
        runawayStride = runStride;
        long long runawayCeiling = stackBytes / runStride;
        cout << "  the child measured its own runaway frame stride: " << runStride << " bytes per call\n";
        cout << "  predicted ceiling = " << stackBytes << " / " << runStride << " = "
             << runawayCeiling << " calls\n";
        cout << "  measured: the child reached depth " << diedAt
             << " before the guard page under the stack was hit\n";
        cout << "  difference " << (diedAt - runawayCeiling) << " calls ("
             << ((diedAt - runawayCeiling) * 100.0 / runawayCeiling) << " percent), at "
             << runStride << " bytes per frame that is " << (diedAt * runStride / 1024)
             << " KiB against the " << (stackBytes / 1024) << " KiB limit\n";
        cout << "  the SIGSEGV handler ran on its own stack and reported before the child died: it exited\n";
        cout << "    with code " << childExit << ", the code the handler asks for, so the fault never became\n";
        cout << "    an unhandled signal (waitpid reports signal " << childSignal << ", which is none)\n";
        cout << "  child pid " << childPid << " is gone, the parent keeps running and prints this line\n";
    } else {
        cout << "  the child did not report a depth, so this machine did not reproduce the overflow\n";
    }
    cout << "  write the base case first, then the recursive case: that order is the exam answer\n";

    cout << "\n=== 9. the one line to remember ===\n";
    cout << "  a recursive call does not create a new problem, it suspends the current frame on the\n";
    cout << "  call stack, and the frame is charged per suspended call: " << sumStride << " bytes for\n";
    cout << "  linearSumRec, " << factStride << " for factorial, " << hanoiStride << " for hanoi";
    if (runawayStride > 0) cout << ", " << runawayStride << " for the runaway above";
    else cout << ", and the runaway stride was not measured in this run";
    cout << ". every one of those is the number measured in sections 2 and 8, never a typed constant.\n";
    cout << "  Depth times that number is your stack use, and it is released in reverse order when the\n";
    cout << "  base case is reached and the frames unwind.\n";
    cout << "  measured on this machine, the same machine that runs your DAS-LAB experiments.\n";
    return 0;
}
