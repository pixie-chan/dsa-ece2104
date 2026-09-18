// 15_queue_apps.cpp
// ECE2104 (DSA), Lecture 15: Queue applications.
// Handout session outcome: implement stacks and queues in memory.
//
// Build: g++ -Wall -Wextra -Wpedantic -std=c++17 -O2 -o /tmp/lec15 15_queue_apps.cpp
//
// Two applications, both scheduled by a queue: round-robin CPU scheduling
// (section 2) and a queue built out of two stacks (section 4). Section 5
// measures what the two-stack trick costs, which is where the amortised O(1)
// argument stops being hand waving.

#include <iostream>
#include <iomanip>
#include <cstddef>
#include <cstdlib>
#include <new>
#include <chrono>
#include <queue>
#include <stack>
#include <string>
#include <vector>
using namespace std;

struct Task {
    char name;
    int burst;      // total work in time units
    int remaining;  // work left
    int arrival = 0;
    int completion = 0;
};

// ---------- the one queue in this file that is meant to be used twice ----------
class QueueFromStacks {
public:
    void enqueue(int value) {
        in_.push(value);
        enqueued_++;
    }
    bool dequeue(int &out) {
        if (out_.empty()) {
            transfers_ += in_.size();
            while (!in_.empty()) { out_.push(in_.top()); in_.pop(); }
        }
        if (out_.empty()) return false;
        out = out_.top();
        out_.pop();
        return true;
    }
    size_t size() const { return in_.size() + out_.size(); }
    size_t transfers() const { return transfers_; }
    size_t enqueued() const { return enqueued_; }
private:
    stack<int> in_;
    stack<int> out_;
    size_t transfers_ = 0;
    size_t enqueued_ = 0;
};

class StackFromQueues {
public:
    void push(int value) {
        q_.push(value);
        for (size_t i = 0; i + 1 < q_.size(); i++) {     // rotate the new element to the front
            q_.push(q_.front());
            q_.pop();
            rotations_++;
        }
    }
    int pop() {
        int v = q_.front();
        q_.pop();
        return v;
    }
    size_t size() const { return q_.size(); }
    size_t rotations() const { return rotations_; }
private:
    queue<int> q_;
    size_t rotations_ = 0;
};

int main() {
    cout << "=== 1. what a queue gives you ===\n";
    cout << "  first in, first out: the element that has waited longest leaves first\n";
    cout << "  that single rule is the scheduler, the printer spooler, the keyboard buffer, and BFS\n";

    cout << "\n=== 2. round robin scheduling over 5 tasks (quantum = 3) ===\n";
    const int QUANTUM = 3;
    vector<Task> tasks = {{'A', 8, 8}, {'B', 4, 4}, {'C', 9, 9}, {'D', 2, 2}, {'E', 6, 6}};
    queue<int> ready;
    for (size_t i = 0; i < tasks.size(); i++) ready.push((int)i);
    int clock = 0, switches = 0;
    vector<char> finishOrder;
    cout << "  " << left << setw(6) << "start" << setw(5) << "end" << setw(6) << "task"
         << setw(10) << "ran" << setw(11) << "remaining" << "state\n";
    while (!ready.empty()) {
        int i = ready.front();
        ready.pop();
        int slice = (tasks[i].remaining < QUANTUM) ? tasks[i].remaining : QUANTUM;
        int start = clock;
        clock += slice;
        tasks[i].remaining -= slice;
        switches++;
        bool done = tasks[i].remaining == 0;
        if (done) { tasks[i].completion = clock; finishOrder.push_back(tasks[i].name); }
        cout << "  " << setw(6) << start << setw(5) << clock << setw(6) << tasks[i].name
             << setw(10) << slice << setw(11) << tasks[i].remaining
             << (done ? "DONE" : "back to the tail of the queue") << "\n";
        if (!done) ready.push(i);
    }
    cout << right;
    cout << "  completion order:";
    for (char c : finishOrder) cout << " " << c;
    cout << "\n  total time = " << clock << ", slices = " << switches
         << " (one context switch per slice)\n";
    double rrWait = 0, rrTurn = 0;
    int rrMaxWait = 0;
    for (const Task &t : tasks) {
        int turn = t.completion - t.arrival;
        int wait = turn - t.burst;
        rrWait += wait;
        rrTurn += turn;
        if (wait > rrMaxWait) rrMaxWait = wait;
    }
    cout << fixed << setprecision(2);
    cout << "  round robin: average waiting = " << (rrWait / (double)tasks.size())
         << ", average turnaround = " << (rrTurn / (double)tasks.size())
         << ", worst waiting = " << rrMaxWait << "\n";

    cout << "\n=== 3. the same five tasks served first come first served ===\n";
    int c2 = 0;
    double fcfsWait = 0, fcfsTurn = 0;
    int fcfsMaxWait = 0;
    cout << "  " << left << setw(6) << "start" << setw(5) << "end" << setw(6) << "task"
         << setw(10) << "burst" << "waiting\n";
    for (const Task &t : tasks) {
        int start = c2;
        c2 += t.burst;
        int wait = start - t.arrival;
        fcfsWait += wait;
        fcfsTurn += (c2 - t.arrival);
        if (wait > fcfsMaxWait) fcfsMaxWait = wait;
        cout << "  " << setw(6) << start << setw(5) << c2 << setw(6) << t.name
             << setw(10) << t.burst << wait << "\n";
    }
    cout << right;
    cout << "  FCFS: average waiting = " << (fcfsWait / (double)tasks.size())
         << ", average turnaround = " << (fcfsTurn / (double)tasks.size())
         << ", worst waiting = " << fcfsMaxWait << "\n";
    cout << "  measured trade: worst wait is " << rrMaxWait << " under round robin against " << fcfsMaxWait
         << " under FCFS, so RR wins there by " << (fcfsMaxWait - rrMaxWait) << " time units\n";
    cout << "  and it loses " << setprecision(2) << ((rrWait - fcfsWait) / (double)tasks.size())
         << " on the average wait. Nobody starves under RR; the average pays for it.\n";

    cout << "\n=== 4. a queue built from two stacks ===\n";
    cout << "  enqueue pushes onto the IN stack, dequeue pops from the OUT stack.\n";
    cout << "  when OUT is empty, everything is poured from IN to OUT, which reverses it twice, so\n";
    cout << "  the oldest element ends up on top of OUT. That is the FIFO order, made out of LIFO parts.\n";
    QueueFromStacks q2s;
    cout << "  " << left << setw(8) << "op" << setw(6) << "value" << setw(12) << "transfers"
         << setw(8) << "size" << "note\n";
    for (int i = 1; i <= 6; i++) {
        q2s.enqueue(i * 10);
        cout << "  " << setw(8) << "enqueue" << setw(6) << i * 10 << setw(12) << q2s.transfers()
             << setw(8) << q2s.size() << "pushed onto IN, no transfer yet\n";
    }
    int v = 0;
    vector<int> cameBack;
    size_t transfersAtFirstDequeue = 0;
    while (q2s.dequeue(v)) {
        if (cameBack.empty()) transfersAtFirstDequeue = q2s.transfers();
        cameBack.push_back(v);
        cout << "  " << setw(8) << "dequeue" << setw(6) << v << setw(12) << q2s.transfers()
             << setw(8) << q2s.size()
             << (q2s.transfers() == 6 && q2s.size() == 5 ? "one bulk transfer of 6, then plain pops\n" : "\n");
    }
    cout << right;
    bool orderOk = (cameBack.size() == 6);
    for (size_t i = 0; i < cameBack.size(); i++) if (cameBack[i] != (int)(i + 1) * 10) orderOk = false;
    cout << "  order check: enqueued 10 20 30 40 50 60 and they came back in that order: "
         << (orderOk ? "yes" : "NO") << "\n";
    cout << "  total transfers for 6 elements = " << transfersAtFirstDequeue
         << ": each element moved from IN to OUT exactly once,\n";
    cout << "  so a dequeue costs 1 pop on average even though the worst single one cost 6\n";

    cout << "\n=== 5. queue from two stacks against std::queue on the same operation stream ===\n";
    QueueFromStacks mine;
    queue<int> reference;
    bool match = true;
    unsigned int lcg = 12345u;
    size_t ops = 200;
    for (size_t i = 0; i < ops; i++) {
        lcg = lcg * 1103515245u + 12345u;
        bool doEnqueue = (mine.size() == 0) || ((lcg >> 16) % 3 != 0);   // roughly two enqueues per dequeue
        if (doEnqueue) {
            int value = (int)((lcg >> 8) % 1000);
            mine.enqueue(value);
            reference.push(value);
        } else {
            int a = 0, b = 0;
            bool okA = mine.dequeue(a);
            bool okB = !reference.empty();
            if (okB) { b = reference.front(); reference.pop(); }
            if (okA != okB || a != b) match = false;
        }
    }
    while (!reference.empty()) {
        int a = 0, b = reference.front();
        reference.pop();
        if (!mine.dequeue(a) || a != b) match = false;
    }
    cout << "  " << ops << " mixed operations, followed by a full drain\n";
    cout << "  the two-stack queue returned the same values in the same order as std::queue: "
         << (match ? "yes" : "NO") << "\n";
    cout << "  transfers over the whole run = " << mine.transfers() << " for " << mine.enqueued()
         << " enqueues and " << ops << " operations\n";
    cout << "  each element is poured from IN to OUT at most once, so transfers <= enqueues: "
         << (mine.transfers() <= mine.enqueued() ? "yes" : "NO") << "\n";

    cout << "\n=== 6. the reverse trick: a stack from two queues ===\n";
    StackFromQueues s2q;
    size_t runningRotations = 0;
    cout << "  push into the queue, then rotate the other n-1 elements behind it so it sits at the front\n";
    for (int i = 1; i <= 6; i++) {
        s2q.push(i);
        runningRotations = s2q.rotations();
        cout << "  push(" << i << ")  rotations so far = " << runningRotations
             << "  (that push cost " << (i - 1) << " rotations)\n";
    }
    cout << "  pops come back:";
    while (s2q.size()) cout << " " << s2q.pop();
    cout << "\n  LIFO recovered out of two FIFO queues, but look at the price: the same 6 pushes cost "
         << runningRotations << " rotations,\n";
    cout << "  which is 1 + 2 + ... + 5 = " << (6 * 5) / 2
         << ". A push is O(n) here against O(1) for a real stack: the trick works, it just is not free.\n";

    cout << "\n=== 7. cost of the two-stack queue, timed ===\n";
    const size_t N = 500000;
    QueueFromStacks big;
    bool bigOk = true;
    auto t0 = chrono::steady_clock::now();
    for (size_t i = 0; i < N; i++) {
        big.enqueue((int)i);
        big.dequeue(v);
        if (v != (int)i) bigOk = false;          // the check is what keeps the loop observable
    }
    auto t1 = chrono::steady_clock::now();
    double nsPerOp = (double)chrono::duration_cast<chrono::nanoseconds>(t1 - t0).count() / (2.0 * (double)N);
    cout << fixed << setprecision(3);
    cout << "  " << N << " enqueues and " << N << " dequeues, every dequeue immediately after its enqueue\n";
    cout << "  so the transfer happens on every single dequeue: " << nsPerOp
         << " ns per operation, transfers = " << big.transfers()
         << ", order verified: " << (bigOk ? "yes" : "NO") << "\n";
    cout << "  (that pattern IS the worst case for the two-stack design; a real workload batches, and\n";
    cout << "   the measured 6 element run above shows the good case: 1 transfer per element in total)\n";

    cout << "\n=== 8. the one line to remember ===\n";
    cout << "  a queue is the shape of anything that must serve arrival order, and any structure that\n";
    cout << "  can reverse a sequence twice can imitate it. Check the cost: round robin pays in average\n";
    cout << "  waiting, the two-stack queue pays in transfers, both own their place\n";
    return 0;
}
