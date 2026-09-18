// 13_queue_array.cpp
// ECE2104 (DSA), Lecture 13: Queue, concepts and array representation.
// Handout session outcome: implement stacks and queues in memory.
//
// Build: g++ -Wall -Wextra -Wpedantic -std=c++17 -O2 -o /tmp/lec13 13_queue_array.cpp
//
// A stack uses ONE end, a queue uses TWO: enqueue writes at the rear, dequeue
// reads at the front, and that is where the trouble starts. The naive array
// queue moves both indices forward and never wraps, so the slots behind the
// front become unreachable. Section 4 prints the exact operation where that
// happens and counts the slots that are lost.

#include <iostream>
#include <iomanip>
#include <cstddef>
#include <cstdlib>
#include <new>
#include <chrono>
#include <malloc.h>
using namespace std;

// every heap request this lesson makes goes through heapAlloc, so the counters
// below are counts taken at the source rather than estimates
static size_t g_allocCalls = 0, g_allocBytes = 0;
static void *heapAlloc(size_t bytes) {
    g_allocCalls++;
    g_allocBytes += bytes;
    void *p = malloc(bytes);
    if (!p) { cerr << "out of memory\n"; exit(1); }
    return p;
}
static void heapFree(void *p) { free(p); }

// the naive queue of the lecture: front and rear both only move forward
template <typename T, size_t CAP>
class NaiveQueue {
public:
    bool enqueue(const T &value) {
        if (rear_ == CAP) { refusals_++; return false; }     // full, or just out of index space
        data_[rear_] = value;
        rear_++;
        return true;
    }
    bool dequeue(T &out) {
        if (front_ == rear_) return false;                   // empty
        out = data_[front_];
        front_++;
        return true;
    }
    bool front(T &out) const {
        if (front_ == rear_) return false;
        out = data_[front_];
        return true;
    }
    bool empty() const { return front_ == rear_; }
    bool full() const { return rear_ == CAP; }
    size_t size() const { return rear_ - front_; }
    size_t freeSlotsBelowFront() const { return front_; }     // unreachable space
    size_t freeSlotsAtRear() const { return CAP - rear_; }
    size_t refusals() const { return refusals_; }
    size_t frontIndex() const { return front_; }
    size_t rearIndex() const { return rear_; }
    static size_t capacity() { return CAP; }
    const T &raw(size_t i) const { return data_[i]; }
private:
    T data_[CAP]{};
    size_t front_ = 0;
    size_t rear_ = 0;
    size_t refusals_ = 0;
};

// the repair: the index does not move forward, it moves the way a clock hand does
template <typename T, size_t CAP>
class CircularQueue {
public:
    bool enqueue(const T &value) {
        if (count_ == CAP) { refusals_++; return false; }
        data_[rear_] = value;
        rear_ = (rear_ + 1) % CAP;
        wraps_ += (rear_ == 0) ? 1 : 0;
        count_++;
        return true;
    }
    bool dequeue(T &out) {
        if (count_ == 0) return false;
        out = data_[front_];
        front_ = (front_ + 1) % CAP;
        count_--;
        return true;
    }
    size_t size() const { return count_; }
    size_t refusals() const { return refusals_; }
    size_t wraps() const { return wraps_; }
    static size_t capacity() { return CAP; }
private:
    T data_[CAP]{};
    size_t front_ = 0;
    size_t rear_ = 0;
    size_t count_ = 0;
    size_t refusals_ = 0;
    size_t wraps_ = 0;
};

static void show(const NaiveQueue<int, 5> &q, const char *label) {
    cout << "  " << label << " frontIndex=" << q.frontIndex() << " rearIndex=" << q.rearIndex()
         << " size=" << q.size() << " empty=" << (q.empty() ? "yes" : "no")
         << " full=" << (q.full() ? "yes" : "no");
    cout << "  slots:";
    for (size_t i = 0; i < NaiveQueue<int, 5>::capacity(); i++) {
        if (i < q.frontIndex() || i >= q.rearIndex()) cout << " [--]";
        else cout << " [" << q.raw(i) << "]";
    }
    cout << "\n";
}

int main() {
    cout << "=== 1. the queue is two indices plus its array ===\n";
    cout << "  sizeof(int)=" << sizeof(int) << "\n";
    cout << "  sizeof(NaiveQueue<int,5>)    = " << sizeof(NaiveQueue<int, 5>) << "\n";
    cout << "  sizeof(CircularQueue<int,5>) = " << sizeof(CircularQueue<int, 5>) << "\n";
    cout << "  the array is " << 5 * sizeof(int) << " bytes, the bookkeeping is " << sizeof(size_t) * 3
         << " bytes in the naive form and " << sizeof(size_t) * 5 << " in the circular form\n";
    cout << "  enqueue writes at rear, then rear++   (one end)\n";
    cout << "  dequeue reads at front, then front++  (the OTHER end)\n";

    cout << "\n=== 2. FIFO proof: enqueue 10 20 30, dequeue three times ===\n";
    NaiveQueue<int, 5> q;
    int v = 0;
    for (int value : {10, 20, 30}) {
        q.enqueue(value);
        cout << "  enqueue(" << setw(2) << value << ")  rearIndex=" << q.rearIndex()
             << " size=" << q.size() << "\n";
    }
    while (q.dequeue(v)) cout << "  dequeue() -> " << v << "  frontIndex=" << q.frontIndex()
                              << " size=" << q.size() << "\n";
    cout << "  inputs were 10 20 30 and they came back 10 20 30: same order, opposite of the stack\n";
    cout << "  a queue is what a printer does with its jobs and what a scheduler does with processes\n";

    cout << "\n=== 3. operation log with the indices that matter ===\n";
    NaiveQueue<int, 5> q2;
    q2.enqueue(1); q2.enqueue(2); q2.enqueue(3); q2.enqueue(4); q2.enqueue(5);
    show(q2, "after 5 enqueues:");
    for (int i = 0; i < 3; i++) { q2.dequeue(v); cout << "  dequeue() -> " << v << "\n"; }
    show(q2, "after 3 dequeues:");
    cout << "  free slots BELOW front = " << q2.freeSlotsBelowFront()
         << ", free slots at rear = " << q2.freeSlotsAtRear()
         << ", live elements = " << q2.size() << "\n";

    cout << "\n=== 4. the exact point where the naive queue wastes slots ===\n";
    for (int i = 6; i <= 7; i++) {
        bool ok = q2.enqueue(i);
        cout << "  enqueue(" << i << ") -> " << (ok ? "stored" : "REFUSED (rear == capacity)")
             << "   rearIndex=" << q2.rearIndex()
             << " size=" << q2.size() << " refusals=" << q2.refusals() << "\n";
    }
    show(q2, "state at the refusal:");
    cout << "  the queue holds " << q2.size() << " elements, index " << q2.rearIndex()
         << " is off the end of the array, and indices 0 to " << q2.frontIndex() - 1
         << " (that is " << q2.freeSlotsBelowFront() << " slots) are still valid memory\n";
    cout << "  the exact operation is enqueue #6 of this sequence: after 5 enqueues and 3 dequeues,\n";
    cout << "  rear == capacity, so full() says yes while " << q2.freeSlotsBelowFront()
         << " of the " << q2.capacity() << " slots are unreachable. Space lost = frontIndex = "
         << q2.frontIndex() << " slots.\n";
    cout << "  drain the rest: ";
    while (q2.dequeue(v)) cout << v << " ";
    cout << "\n";
    show(q2, "after draining:");
    cout << "  now size=0 but rearIndex=" << q2.rearIndex()
         << ", and a naive queue that does not reset the indices can never enqueue again:\n";
    cout << "  dequeue says empty while full() still says full: " << (q2.full() && q2.empty() ? "both true at once" : "not both") << "\n";

    cout << "\n=== 5. the repair in one line, measured on the same sequence ===\n";
    cout << "  rear = (rear + 1) % capacity   and   front = (front + 1) % capacity\n";
    cout << "  the same script run on both forms: 5 enqueues, 3 dequeues, then 3 more enqueues\n";
    NaiveQueue<int, 5> naive;
    CircularQueue<int, 5> circ;
    for (int i = 1; i <= 5; i++) { naive.enqueue(i); circ.enqueue(i); }
    for (int i = 0; i < 3; i++) { naive.dequeue(v); circ.dequeue(v); }
    for (int i = 6; i <= 8; i++) { naive.enqueue(i); circ.enqueue(i); }
    cout << "  naive form    : refused " << naive.refusals() << " of those 3 enqueues, holds "
         << naive.size() << " elements\n";
    cout << "  circular form : refused " << circ.refusals() << " of those 3 enqueues, holds "
         << circ.size() << " elements\n";
    cout << "  every dequeue in the circular form frees its slot for exactly the next enqueue\n";

    cout << "\n=== 6. cost: 1,000,000 enqueues plus 1,000,000 dequeues ===\n";
    const size_t N = 1000000;
    g_allocCalls = 0; g_allocBytes = 0;
    CircularQueue<int, 1024> *fast = new (heapAlloc(sizeof(CircularQueue<int, 1024>))) CircularQueue<int, 1024>();
    size_t containerCalls = g_allocCalls, containerBytes = g_allocBytes;
    g_allocCalls = 0; g_allocBytes = 0;
    bool ordered = true;
    auto t0 = chrono::steady_clock::now();
    for (size_t i = 0; i < N; i++) {
        fast->enqueue((int)i);
        fast->dequeue(v);
        if (v != (int)i) ordered = false;      // the check is what keeps the loop observable
    }
    auto t1 = chrono::steady_clock::now();
    double nsPerOp = (double)chrono::duration_cast<chrono::nanoseconds>(t1 - t0).count() / (2.0 * (double)N);
    cout << "  the container asked the heap for " << containerCalls << " block of " << containerBytes
         << " bytes (the array lives inside the object)\n";
    cout << "  the 2,000,000 operations asked the heap for " << g_allocCalls << " blocks and "
         << g_allocBytes << " bytes\n";
    cout << "  every dequeue returned the value its enqueue put in, in order: " << (ordered ? "yes" : "NO") << "\n";
    cout << fixed << setprecision(3);
    cout << "  per operation: " << nsPerOp << " ns, index wraps survived = " << fast->wraps() << "\n";
    fast->~CircularQueue<int, 1024>();
    heapFree(fast);

    cout << "\n=== 7. the one line to remember ===\n";
    cout << "  a naive array queue is correct for at most capacity operations before the front abandons\n";
    cout << "  the space behind it; making the index wrap (front/rear mod capacity) is what turns a\n";
    cout << "  one-shot queue into a reusable one, and it costs one modulo per operation.\n";
    return 0;
}
