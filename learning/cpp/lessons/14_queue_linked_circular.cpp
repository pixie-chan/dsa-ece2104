// 14_queue_linked_circular.cpp
// ECE2104 (DSA), Lecture 14: Queue using linked list and circular queue.
// Handout session outcome: implement stacks and queues in memory.
//
// Build: g++ -Wall -Wextra -Wpedantic -std=c++17 -O2 -o /tmp/lec14 14_queue_linked_circular.cpp
//
// Two ways to remove the naive queue's ceiling of "capacity operations":
// pay the heap for a node per element (linked queue), or bend the index space
// so the rear never falls off the end (circular queue). Sections 4 to 6 print
// the index trajectory through two complete wrap-arounds and the count formula
// evaluated on the real numbers from that run.

#include <iostream>
#include <iomanip>
#include <cstddef>
#include <cstdlib>
#include <new>
#include <chrono>
#include <string>
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

struct QNode {
    int data;
    QNode *next;
};

class LinkedQueue {
public:
    LinkedQueue() = default;
    LinkedQueue(const LinkedQueue &) = delete;
    LinkedQueue &operator=(const LinkedQueue &) = delete;
    ~LinkedQueue() { int v = 0; while (dequeue(v)) {} }

    void enqueue(int value) {                    // grows at the rear
        QNode *n = (QNode *)heapAlloc(sizeof(QNode));
        n->data = value;
        n->next = nullptr;
        if (rear_) rear_->next = n; else front_ = n;
        rear_ = n;
        count_++;
    }
    bool dequeue(int &out) {                     // shrinks at the front
        if (!front_) { underflows_++; return false; }
        QNode *n = front_;
        out = n->data;
        front_ = n->next;
        if (!front_) rear_ = nullptr;
        heapFree(n);
        count_--;
        return true;
    }
    size_t size() const { return count_; }
    size_t underflows() const { return underflows_; }
    const QNode *front() const { return front_; }
    const QNode *rear() const { return rear_; }
private:
    QNode *front_ = nullptr;
    QNode *rear_ = nullptr;
    size_t count_ = 0;
    size_t underflows_ = 0;
};

// count based circular queue: one extra integer removes the front == rear ambiguity
template <typename T, size_t CAP>
class CircularQueue {
public:
    bool enqueue(const T &value) {
        if (count_ == CAP) { refusals_++; return false; }
        data_[rear_] = value;
        rear_ = (rear_ + 1) % CAP;
        if (rear_ == 0) wraps_++;
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
    size_t frontIndex() const { return front_; }
    size_t rearIndex() const { return rear_; }
    size_t wraps() const { return wraps_; }
    size_t refusals() const { return refusals_; }
    static size_t capacity() { return CAP; }
    static size_t countFormula(size_t rear, size_t front) { return (rear + CAP - front) % CAP; }
private:
    T data_[CAP]{};
    size_t front_ = 0;
    size_t rear_ = 0;
    size_t count_ = 0;
    size_t wraps_ = 0;
    size_t refusals_ = 0;
};

// the textbook alternative: no count, one slot deliberately left empty
template <typename T, size_t CAP>
class SentinelQueue {
public:
    bool enqueue(const T &value) {
        if ((rear_ + 1) % CAP == front_) { refusals_++; return false; }
        data_[rear_] = value;
        rear_ = (rear_ + 1) % CAP;
        return true;
    }
    bool dequeue(T &out) {
        if (front_ == rear_) return false;
        out = data_[front_];
        front_ = (front_ + 1) % CAP;
        return true;
    }
    bool empty() const { return front_ == rear_; }
    bool full() const { return (rear_ + 1) % CAP == front_; }
    size_t frontIndex() const { return front_; }
    size_t rearIndex() const { return rear_; }
    size_t usable() const { return CAP - 1; }
    size_t refusals() const { return refusals_; }
private:
    T data_[CAP]{};
    size_t front_ = 0;
    size_t rear_ = 0;
    size_t refusals_ = 0;
};

int main() {
    cout << "=== 1. the linked queue in bytes ===\n";
    cout << "  sizeof(QNode{int data; QNode *next;}) = " << sizeof(QNode)
         << "  offsetof(data)=" << offsetof(QNode, data)
         << " offsetof(next)=" << offsetof(QNode, next) << "\n";
    cout << "  sizeof(LinkedQueue) = " << sizeof(LinkedQueue)
         << " bytes (front pointer, rear pointer, count, underflow counter)\n";
    cout << "  the queue holds TWO ends: enqueue writes through rear_, dequeue reads front_, both O(1)\n";
    cout << "  a linked queue never says full: it has no capacity to exceed\n";

    cout << "\n=== 2. enqueue and dequeue log with real node addresses ===\n";
    LinkedQueue lq;
    for (int value : {7, 8, 9}) {
        lq.enqueue(value);
        cout << "  enqueue(" << value << ")  node at " << (const void *)lq.rear()
             << "  front=" << (const void *)lq.front()
             << " rear=" << (const void *)lq.rear() << " size=" << lq.size() << "\n";
    }
    QNode *probeNode = new QNode{0, nullptr};
    cout << "  the front node stays put while the rear walks away from it: that is the whole queue\n";
    int v = 0;
    while (lq.dequeue(v))
        cout << "  dequeue() -> " << v << "  front now " << (const void *)lq.front()
             << " size=" << lq.size() << "\n";
    cout << "  dequeue on empty returns " << (lq.dequeue(v) ? "a value" : "false")
         << ", underflows=" << lq.underflows() << "\n";
    size_t probeUsable = malloc_usable_size((void *)probeNode);
    delete probeNode;
    cout << "  the allocator charges " << probeUsable
         << " usable bytes per node for the 16 byte request\n";

    cout << "\n=== 3. no ceiling: 200,000 enqueues through the linked queue ===\n";
    const size_t MANY = 200000;
    g_allocCalls = 0; g_allocBytes = 0;
    LinkedQueue big;
    for (size_t i = 0; i < MANY; i++) big.enqueue((int)i);
    size_t calls = g_allocCalls, requested = g_allocBytes;
    g_freeCalls = 0;
    while (big.dequeue(v)) {}
    cout << "  enqueued " << MANY << " elements with " << calls << " heap requests requesting "
         << requested << " bytes, and the drain made " << g_freeCalls << " free calls\n";
    cout << "  cost: one 16 byte node per element and a pointer per hop, in exchange for no limit\n";

    cout << "\n=== 4. circular queue: the index trajectory through TWO wrap-arounds ===\n";
    CircularQueue<int, 5> cq;
    cout << "  capacity = " << CircularQueue<int, 5>::capacity()
         << ", the script is enqueue enqueue dequeue dequeue, five times over\n";
    cout << "  " << left << setw(4) << "op" << setw(6) << "val" << setw(8) << "front"
         << setw(6) << "rear" << setw(7) << "count" << setw(7) << "wraps" << "note\n";
    int seq = 100;
    for (int round = 0; round < 5; round++) {
        for (int k = 0; k < 2; k++) {
            size_t beforeWraps = cq.wraps();
            bool ok = cq.enqueue(seq);
            cout << "  " << setw(4) << "enq" << setw(6) << seq << setw(8) << cq.frontIndex()
                 << setw(6) << cq.rearIndex() << setw(7) << cq.size() << setw(7) << cq.wraps()
                 << (ok ? "" : " REFUSED")
                 << (cq.wraps() != beforeWraps ? "  <- rear wrapped past the end of the array" : "") << "\n";
            seq++;
        }
        for (int k = 0; k < 2; k++) {
            cq.dequeue(v);
            cout << "  " << setw(4) << "deq" << setw(6) << v << setw(8) << cq.frontIndex()
                 << setw(6) << cq.rearIndex() << setw(7) << cq.size() << setw(7) << cq.wraps() << "\n";
        }
    }
    cout << right;
    cout << "  10 enqueues into a 5 slot array and the rear index came round " << cq.wraps()
         << " times: the same 5 slots served all of them\n";
    cout << "  a naive queue would have refused after 5 enqueues: this one is still running\n";

    cout << "\n=== 5. the count formula, evaluated on the states above ===\n";
    cout << "  count = (rear - front + capacity) mod capacity\n";
    CircularQueue<int, 5> probe;
    probe.enqueue(1); probe.enqueue(2); probe.enqueue(3);
    cout << "  state A: front=" << probe.frontIndex() << " rear=" << probe.rearIndex()
         << " -> formula = (" << probe.rearIndex() << " - " << probe.frontIndex() << " + 5) mod 5 = "
         << CircularQueue<int, 5>::countFormula(probe.rearIndex(), probe.frontIndex())
         << " and the stored count is " << probe.size() << "\n";
    int tmp = 0;
    probe.dequeue(tmp); probe.dequeue(tmp);
    cout << "  state B: front=" << probe.frontIndex() << " rear=" << probe.rearIndex()
         << " -> formula = (" << probe.rearIndex() << " - " << probe.frontIndex() << " + 5) mod 5 = "
         << CircularQueue<int, 5>::countFormula(probe.rearIndex(), probe.frontIndex())
         << " and the stored count is " << probe.size() << "\n";
    CircularQueue<int, 5> wrapped;
    for (int i = 1; i <= 5; i++) wrapped.enqueue(i);
    for (int i = 0; i < 3; i++) wrapped.dequeue(tmp);
    wrapped.enqueue(6);
    wrapped.enqueue(7);
    cout << "  state C (a wrapped one): front=" << wrapped.frontIndex() << " rear=" << wrapped.rearIndex()
         << ", bare (rear - front) as unsigned = " << (unsigned long long)(wrapped.rearIndex() - wrapped.frontIndex())
         << "\n    while the formula gives (" << wrapped.rearIndex() << " - " << wrapped.frontIndex() << " + 5) mod 5 = "
         << CircularQueue<int, 5>::countFormula(wrapped.rearIndex(), wrapped.frontIndex())
         << " and the stored count is " << wrapped.size() << "\n";
    cout << "  that wrapped number is the whole reason the + capacity sits in the formula\n";

    cout << "\n=== 6. full/empty ambiguity and the two ways out ===\n";
    cout << "  front == rear means EMPTY in the count-free design, and it is also what you get after\n";
    cout << "  capacity enqueues (rear caught up with front again), so one comparison cannot decide both.\n";
    CircularQueue<int, 5> counted;
    counted.enqueue(1); counted.enqueue(2); counted.enqueue(3);
    while (counted.dequeue(tmp)) {}
    cout << "  count based: front=" << counted.frontIndex() << " rear=" << counted.rearIndex()
         << " count=" << counted.size() << " -> empty, decided by the count\n";
    SentinelQueue<int, 5> sent;
    for (int i = 1; i <= 4; i++) sent.enqueue(i);
    cout << "  sentinel slot: after " << sent.usable() << " enqueues usable=" << sent.usable()
         << " front=" << sent.frontIndex() << " rear=" << sent.rearIndex()
         << " full=" << (sent.full() ? "yes" : "no") << " refusals=" << sent.refusals() << "\n";
    cout << "  the sentinel design pays one slot of the array to keep the test to one comparison:\n";
    cout << "  it stores " << sent.usable() << " of " << 5 << " slots, the count design stores all 5\n";

    cout << "\n=== 7. cost of the three queue shapes, timed in this one process ===\n";
    const size_t N = 1000000;
    bool fifoOk = true;
    g_allocCalls = 0; g_allocBytes = 0;
    LinkedQueue tq;
    auto t0 = chrono::steady_clock::now();
    for (size_t i = 0; i < N; i++) tq.enqueue((int)i);   // every node stays live across the whole
    // measured region, so the allocator calls cannot be optimised away
    for (size_t i = 0; i < N; i++) { tq.dequeue(v); if (v != (int)i) fifoOk = false; }
    auto t1b = chrono::steady_clock::now();
    size_t linkedCalls = g_allocCalls;
    double nsLinked = (double)chrono::duration_cast<chrono::nanoseconds>(t1b - t0).count() / (2.0 * (double)N);

    g_allocCalls = 0; g_allocBytes = 0;
    CircularQueue<int, 1024> *cqBig = new CircularQueue<int, 1024>();
    size_t circCalls = g_allocCalls;
    auto t2 = chrono::steady_clock::now();
    for (size_t i = 0; i < N; i++) {
        cqBig->enqueue((int)i);
        cqBig->dequeue(v);
        if (v != (int)i) fifoOk = false;
    }
    auto t3 = chrono::steady_clock::now();
    size_t circCallsDuring = g_allocCalls - circCalls;
    double nsCirc = (double)chrono::duration_cast<chrono::nanoseconds>(t3 - t2).count() / (2.0 * (double)N);
    delete cqBig;

    cout << "  fifo order verified in both loops: " << (fifoOk ? "yes" : "NO") << "\n";
    cout << fixed << setprecision(3);
    cout << "  linked queue  : " << nsLinked << " ns per operation, " << linkedCalls
         << " heap requests (one node per element, freed on the way out)\n";
    cout << "  circular queue: " << nsCirc << " ns per operation, " << circCallsDuring
         << " heap requests during the operations (the array is inside the object)\n";
    cout << "  the linked queue is " << setprecision(2) << (nsLinked / nsCirc)
         << "x slower per operation: that gap is the allocator, not the pointer arithmetic\n";
    cout << "  the circular queue pays one modulo per operation instead, and it needs a known capacity\n";

    cout << "\n=== 8. the one line to remember ===\n";
    cout << "  rear = (rear + 1) mod capacity turns the array into a ring; the queue stops wasting the\n";
    cout << "  space behind the front, and count = (rear - front + capacity) mod capacity tells you how\n";
    cout << "  many elements are inside it\n";
    return 0;
}
