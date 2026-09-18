// 21_bst_operations.cpp
// ECE2104 (DSA), Lecture 21: Binary search tree operations.
// Handout session outcome: implement insert, search, and delete on a BST.
//
// Build: g++ -Wall -Wextra -Wpedantic -std=c++17 -O2 -o /tmp/lec21 21_bst_operations.cpp
//
// The BST property: for every node, everything in the left subtree is smaller
// and everything in the right subtree is larger. Insert walks that rule down to
// a null pointer and puts the node there. Search follows the same rule and stops
// early or reaches a null pointer. Delete is the interesting one: it has three
// cases and they are shown here one at a time, each on a freshly built tree,
// with the in-order walk printed before and after so the result is visible.
//
//   the tree this file builds, from the insert order
//   50, 30, 70, 20, 40, 60, 80, 35                (in-order: 20 30 35 40 50 60 70 80)
//
//              50
//           /      \          level 1, the root and its two children
//         30        70
//        /  \      /  \       level 2, four nodes
//      20    40  60    80
//           /
//         35                  level 3, a left child of 40
//
// height is counted in nodes: the longest root to leaf path above has 4 nodes.

#include <iostream>
#include <iomanip>
#include <string>
#include <cstdlib>
#include <new>
using namespace std;

struct Node {
    int   key;
    Node *left;
    Node *right;
};

static size_t g_allocCalls = 0, g_freeCalls = 0;
static void *heapAlloc(size_t bytes) {
    g_allocCalls++;
    void *p = malloc(bytes);
    if (!p) { cerr << "out of memory\n"; exit(1); }
    return p;
}
static void heapFree(void *p) { g_freeCalls++; free(p); }

static Node *makeNode(int key) {
    return new (heapAlloc(sizeof(Node))) Node{key, nullptr, nullptr};
}

static void destroyTree(Node *n) {
    if (!n) return;
    destroyTree(n->left);
    destroyTree(n->right);
    n->~Node();
    heapFree(n);
}

static int heightOf(const Node *n) {
    if (!n) return 0;
    const int l = heightOf(n->left), r = heightOf(n->right);
    return 1 + (l > r ? l : r);
}

static void inorderStr(const Node *n, string &out) {
    if (!n) return;
    inorderStr(n->left, out);
    if (!out.empty()) out += ' ';
    out += to_string(n->key);
    inorderStr(n->right, out);
}

static const Node *findNode(const Node *n, int key) {
    while (n) {
        if (key == n->key) return n;
        n = (key < n->key) ? n->left : n->right;
    }
    return nullptr;
}

static int childCount(const Node *n) {
    if (!n) return 0;
    return (n->left ? 1 : 0) + (n->right ? 1 : 0);
}

static const char *caseLabel(int children) {
    if (children == 0) return "delete case 1: a leaf, unlink it and the rest of the tree is untouched";
    if (children == 1) return "delete case 2: one child, the child takes the place of the removed node";
    return "delete case 3: two children, the in-order successor takes the key, then case 1 or 2 removes it";
}

// in-order successor: the smallest key in the tree that is greater than key.
// If the node has a right subtree the answer is the leftmost node of it,
// otherwise it is the last node we turned left at on the way down.
static bool successorOf(const Node *root, int key, int &out) {
    const Node *cur = root;
    const Node *candidate = nullptr;
    while (cur) {
        if (key < cur->key) { candidate = cur; cur = cur->left; }
        else if (key > cur->key) { cur = cur->right; }
        else break;
    }
    if (!cur) return false;                       // the key is not in the tree at all
    if (cur->right) {
        const Node *m = cur->right;
        while (m->left) m = m->left;
        out = m->key;
        return true;
    }
    if (!candidate) return false;                 // the key is the maximum, nothing comes after it
    out = candidate->key;
    return true;
}

// delete, recursive, returning the new subtree root so the parent link is rewired
static Node *deleteAt(Node *n, int key, bool &found) {
    if (!n) { found = false; return nullptr; }
    if (key < n->key) { n->left = deleteAt(n->left, key, found); return n; }
    if (key > n->key) { n->right = deleteAt(n->right, key, found); return n; }
    found = true;                                 // this is the node to remove
    if (!n->left) {                               // case 1, and the first half of case 2
        Node *r = n->right;
        n->~Node();
        heapFree(n);
        return r;
    }
    if (!n->right) {                              // the other half of case 2
        Node *l = n->left;
        n->~Node();
        heapFree(n);
        return l;
    }
    Node *s = n->right;                           // case 3: find the in-order successor
    while (s->left) s = s->left;
    n->key = s->key;                              // its key takes this slot, then it is removed below
    bool inner = false;                           // the inner call always finds the key it is given
    n->right = deleteAt(n->right, s->key, inner);
    return n;
}

class Bst {
public:
    Bst() = default;
    Bst(const Bst &) = delete;
    Bst &operator=(const Bst &) = delete;
    ~Bst() { destroyTree(root_); }

    void insert(int key) {
        Node **link = &root_;
        while (*link) {
            if (key < (*link)->key) link = &(*link)->left;
            else if (key > (*link)->key) link = &(*link)->right;
            else return;                            // one node per key, duplicates ignored
        }
        *link = makeNode(key);
        size_++;
    }

    bool search(int key, int &comparisons) const {
        comparisons = 0;
        const Node *cur = root_;
        while (cur) {
            comparisons++;                          // one comparison per node visited
            if (key == cur->key) return true;
            cur = (key < cur->key) ? cur->left : cur->right;
        }
        return false;
    }

    bool minKey(int &out) const {
        if (!root_) return false;
        const Node *cur = root_;
        while (cur->left) cur = cur->left;
        out = cur->key;
        return true;
    }

    bool maxKey(int &out) const {
        if (!root_) return false;
        const Node *cur = root_;
        while (cur->right) cur = cur->right;
        out = cur->key;
        return true;
    }

    bool successor(int key, int &out) const { return successorOf(root_, key, out); }
    int height() const { return heightOf(root_); }
    size_t size() const { return size_; }
    const Node *root() const { return root_; }

    bool remove(int key) {
        bool found = false;
        root_ = deleteAt(root_, key, found);
        if (found) size_--;
        return found;
    }

private:
    Node  *root_ = nullptr;
    size_t size_ = 0;
};

static const int INSERT_ORDER[8] = {50, 30, 70, 20, 40, 60, 80, 35};
static const char *STANDARD_INORDER = "20 30 35 40 50 60 70 80";

static void buildStandard(Bst &t) {
    for (int key : INSERT_ORDER) t.insert(key);
}

// the same tree drawn level by level, from the real pointers
static void printLevels(const Node *root) {
    if (!root) return;
    const Node *cur[16];
    size_t n = 1;
    cur[0] = root;
    int depth = 0;
    while (n) {
        cout << "    depth " << depth << ":";
        const Node *next[16];
        size_t m = 0;
        for (size_t i = 0; i < n; i++) {
            cout << " " << cur[i]->key;
            if (cur[i]->left) next[m++] = cur[i]->left;
            if (cur[i]->right) next[m++] = cur[i]->right;
        }
        cout << "\n";
        for (size_t i = 0; i < m; i++) cur[i] = next[i];
        n = m;
        depth++;
    }
}

// one delete case, on its own freshly built tree, in-order shown before and after
static void deleteCase(const char *title, int key, const char *expectedAfter) {
    cout << "\n" << title << "\n";
    Bst t;
    buildStandard(t);
    string before;
    inorderStr(t.root(), before);
    const Node *target = findNode(t.root(), key);
    const int kids = childCount(target);
    const int heightBefore = t.height();
    cout << "  tree rebuilt from the same insert order, in-order before: " << before << "\n";
    cout << "  before matches the standard sequence: "
         << (before == STANDARD_INORDER ? "confirmed" : "MISMATCH") << "\n";
    cout << "  target key " << key << " is present: " << (target ? "yes" : "no")
         << ", its children: " << kids << "\n";
    cout << "  " << caseLabel(kids) << "\n";
    if (kids == 1) {
        cout << "  the single child is " << (target->left ? target->left->key : target->right->key)
             << " and it moves up into the " << key << " slot\n";
    }
    if (kids == 2) {
        int succ = 0;
        const bool hasSucc = successorOf(t.root(), key, succ);
        cout << "  the in-order successor of " << key << " is "
             << (hasSucc ? to_string(succ) : string("none"))
             << ", the leftmost node of its right subtree\n";
    }
    const bool removed = t.remove(key);
    string after;
    inorderStr(t.root(), after);
    int probeComps = 0;
    const bool stillThere = t.search(key, probeComps);
    cout << "  remove(" << key << ") returned " << (removed ? "true" : "false") << "\n";
    cout << "  in-order after : " << after << "\n";
    cout << "  expected       : " << expectedAfter << "\n";
    cout << "  verdict        : " << (after == expectedAfter ? "match" : "MISMATCH") << "\n";
    cout << "  size " << t.size() << " (was 8), height " << t.height() << " (was " << heightBefore
         << "), search(" << key << ") now returns " << (stillThere ? "found" : "not found")
         << " in " << probeComps << " comparisons\n";
}

// median first insertion, which is the shape you would build by hand
static void insertRange(Bst &t, int lo, int hi) {
    if (lo > hi) return;
    const int mid = lo + (hi - lo) / 2;
    t.insert(mid);
    insertRange(t, lo, mid - 1);
    insertRange(t, mid + 1, hi);
}

int main() {
    cout << "=== 1. the tree and the property ===\n";
    cout << "  insert order: 50 30 70 20 40 60 80 35\n";
    cout << "  for every node: left subtree smaller, right subtree larger. Nothing is sorted\n";
    cout << "  during insertion, the ORDER of the keys is carried by the shape alone.\n";

    {
        Bst t;
        buildStandard(t);
        string ino;
        inorderStr(t.root(), ino);
        cout << "\n  nodes inserted: " << t.size() << ", height: " << t.height() << "\n";
        cout << "  in-order walk: " << ino << "\n";
        cout << "  the in-order walk of a BST is the keys in sorted order: "
             << (ino == STANDARD_INORDER ? "confirmed" : "MISMATCH") << "\n";
        cout << "  the shape read level by level from the real pointers:\n";
        printLevels(t.root());
    }

    cout << "\n=== 2. search: the comparison count for a hit and for a miss ===\n";
    {
        Bst t;
        buildStandard(t);
        int comparisons = 0;
        const int hits[4] = {50, 35, 80, 20};
        for (int key : hits) {
            const bool found = t.search(key, comparisons);
            cout << "  search(" << setw(2) << key << ") -> " << (found ? "found    " : "not found")
                 << " comparisons = " << comparisons
                 << "   one comparison per node on the path\n";
        }
        const int misses[3] = {10, 55, 90};
        for (int key : misses) {
            const bool found = t.search(key, comparisons);
            cout << "  search(" << setw(2) << key << ") -> " << (found ? "found    " : "not found")
                 << " comparisons = " << comparisons
                 << "   reached a null pointer, so the key is not in the tree\n";
        }
        long long totalHits = 0, totalMiss = 0;
        int c = 0;
        for (int key : INSERT_ORDER) { t.search(key, c); totalHits += c; }
        const int absent[8] = {5, 15, 25, 45, 55, 65, 75, 85};
        for (int key : absent) { t.search(key, c); totalMiss += c; }
        cout << "  the 8 present keys cost " << totalHits << " comparisons in total, "
             << fixed << setprecision(2) << (double)totalHits / 8.0 << " on average\n";
        cout << "  the 8 absent keys cost " << totalMiss << " comparisons in total, "
             << (double)totalMiss / 8.0 << " on average\n";
        cout << "  a search costs at most the height of the tree, and this tree has height "
             << t.height() << "\n";
    }

    cout << "\n=== 3. minimum, maximum, successor, and height ===\n";
    {
        Bst t;
        buildStandard(t);
        int mn = 0, mx = 0;
        const bool hasMin = t.minKey(mn);
        const bool hasMax = t.maxKey(mx);
        cout << "  minimum: " << (hasMin ? to_string(mn) : string("none"))
             << " found by walking left until there is no left child\n";
        cout << "  maximum: " << (hasMax ? to_string(mx) : string("none"))
             << " found by walking right until there is no right child\n";
        const int keys[7] = {20, 35, 40, 50, 70, 80, 65};
        for (int key : keys) {
            int succ = 0;
            const bool present = (findNode(t.root(), key) != nullptr);
            const bool hasSucc = t.successor(key, succ);
            cout << "  successor(" << setw(2) << key << ") = "
                 << (hasSucc ? to_string(succ) : string("none"));
            if (!hasSucc)
                cout << "  (the key is "
                     << (present ? "the maximum, nothing follows it" : "absent from the tree") << ")";
            cout << "\n";
        }
        cout << "  height of this tree: " << t.height() << " nodes on the longest path\n";
        cout << "  the path that makes it " << t.height() << ": 50 -> 30 -> 40 -> 35\n";
        Bst emptyTree;
        cout << "  an empty tree has height " << emptyTree.height()
             << " and an empty in-order walk, which is the base case of every recursion here\n";
    }

    deleteCase("=== 4. delete case 1: a leaf ===", 20, "30 35 40 50 60 70 80");
    deleteCase("=== 5. delete case 2: a node with one child ===", 40, "20 30 35 50 60 70 80");
    deleteCase("=== 6. delete case 3: a node with two children ===", 50, "20 30 35 40 60 70 80");

    cout << "\n=== 7. case 3 in detail: why the successor works ===\n";
    {
        Bst t;
        buildStandard(t);
        string ino;
        inorderStr(t.root(), ino);
        const int heightBefore = t.height();
        int succ = 0;
        const bool hasSucc = t.successor(50, succ);
        cout << "  in-order before : " << ino << "\n";
        cout << "  removing 50, the root, which has two children (30 and 70)\n";
        cout << "  successor(50) = " << (hasSucc ? to_string(succ) : string("none"))
             << ": the smallest key larger than 50\n";
        cout << "  the successor key is copied into the 50 slot and the successor node is then\n";
        cout << "  deleted from the right subtree, where it has no left child, so it is case 1\n";
        cout << "  or case 2 again. Copying keeps the property: everything left of the new key is\n";
        cout << "  smaller and everything right of it is larger, which the in-order walk shows.\n";
        const bool removed = t.remove(50);
        string after;
        inorderStr(t.root(), after);
        cout << "  remove(50) returned " << (removed ? "true" : "false") << "\n";
        cout << "  in-order after  : " << after << "\n";
        cout << "  still sorted: " << (after == "20 30 35 40 60 70 80" ? "yes" : "NO")
             << ", size " << t.size() << ", height " << t.height() << " (was " << heightBefore << ")\n";
    }

    cout << "\n=== 8. the degenerate case: a sorted insert order ===\n";
    {
        const int N = 31;
        Bst sortedT;
        for (int i = 1; i <= N; i++) sortedT.insert(i);          // ascending: worst case
        Bst balancedT;
        insertRange(balancedT, 1, N);                            // median first: best case

        string expected;
        for (int i = 1; i <= N; i++) { if (!expected.empty()) expected += ' '; expected += to_string(i); }
        string sortedIno, balancedIno;
        inorderStr(sortedT.root(), sortedIno);
        inorderStr(balancedT.root(), balancedIno);
        int mn = 0, mx = 0;
        sortedT.minKey(mn);
        sortedT.maxKey(mx);

        cout << "  the same " << N << " keys, two insertion orders:\n";
        cout << "  " << left << setw(30) << "insert order" << setw(14) << "height" << "what the shape is\n";
        cout << "  " << setw(30) << "1, 2, 3, ... 31 (sorted)" << setw(14) << sortedT.height()
             << "a chain: every node has only a right child\n";
        cout << "  " << setw(30) << "16, 8, 24, ... (median)" << setw(14) << balancedT.height()
             << "perfectly balanced, 5 levels\n";
        cout << "  in-order of both is still the sorted list: "
             << (sortedIno == expected && balancedIno == expected ? "confirmed" : "MISMATCH")
             << ", so the SAME data behaves completely differently\n";
        cout << "  a perfect tree of " << N << " nodes has 5 levels and the median order hits it exactly;"
             << " the\n  sorted order reaches the other end with " << sortedT.height() << " levels\n";

        int c1 = 0, c2 = 0;
        const bool f1 = sortedT.search(31, c1);
        const bool f2 = balancedT.search(31, c2);
        cout << "\n  search(31) in the sorted tree  : " << c1 << " comparisons, found: " << (f1 ? "yes" : "no") << "\n";
        cout << "  search(31) in the balanced tree: " << c2 << " comparisons, found: " << (f2 ? "yes" : "no") << "\n";
        long long total1 = 0, total2 = 0;
        int c = 0;
        for (int k = 1; k <= N; k++) { sortedT.search(k, c); total1 += c; }
        for (int k = 1; k <= N; k++) { balancedT.search(k, c); total2 += c; }
        cout << "  comparisons for all " << N << " keys: sorted " << total1 << " ("
             << fixed << setprecision(2) << (double)total1 / N << " each), balanced " << total2 << " ("
             << (double)total2 / N << " each)\n";
        cout << "  ratio: the degenerate tree does " << ((double)total1 / (double)total2)
             << "x the comparisons for identical data, O(n) against O(log n)\n";
        cout << "  min " << mn << ", max " << mx << " in the sorted tree, reached by walking left and\n";
        cout << "  walking right for " << N - 1 << " steps at most instead of a full scan\n";
        cout << "  checksum: (comparisons, size) sorted = (" << total1 << ", " << sortedT.size()
             << "), balanced = (" << total2 << ", " << balancedT.size() << ")\n";
    }

    cout << "\n=== 9. the one line to remember ===\n";
    cout << "  a BST is a sorted sequence stored as shape, which makes search, insert and\n";
    cout << "  successor all cost the height of the tree. That is log n ONLY if the tree is\n";
    cout << "  balanced; inserting a sorted list gives a chain with height n and a linear search.\n";
    cout << "  Delete is three cases, and the two child case is solved by the in-order successor,\n";
    cout << "  whose key keeps the sequence sorted, after which the removal is a leaf removal.\n";
    cout << "  nodes allocated " << g_allocCalls << ", nodes freed " << g_freeCalls << "  "
         << (g_allocCalls == g_freeCalls ? "every node returned" : "LEAK") << "\n";
    return 0;
}
