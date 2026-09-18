// 20_tree_traversals.cpp
// ECE2104 (DSA), Lecture 20: Tree traversals, in-order, pre-order, post-order, level order.
// Handout session outcome: traverse a binary tree in all four orders.
//
// Build: g++ -Wall -Wextra -Wpedantic -std=c++17 -O2 -o /tmp/lec20 20_tree_traversals.cpp
//
// The tree below has 7 nodes and a fixed shape, printed once as a picture so
// every sequence that follows can be read against it. The four orders are then
// produced twice where it matters: recursively, and for in-order also with an
// explicit stack, which is how the recursion is really implemented. Every
// sequence is checked in code against the expected string, and the recursion
// depth is measured rather than asserted.
//
//   fixed shape (the named tree)      the skew case: a chain of 7 nodes
//            4                                1
//          /   \                              |
//         2     6                             2
//        / \   / \                            |
//       1   3 5   7                             3   (and so on down to 7)
//
//   depth of node = edges above it, height = nodes on the longest path down

#include <iostream>
#include <iomanip>
#include <string>
#include <queue>
#include <stack>
#include <cstdlib>
#include <new>
using namespace std;

struct Node {
    int   key;
    Node *left;
    Node *right;
};

static size_t g_allocCalls = 0, g_allocBytes = 0, g_freeCalls = 0;
static void *heapAlloc(size_t bytes) {
    g_allocCalls++;
    g_allocBytes += bytes;
    void *p = malloc(bytes);
    if (!p) { cerr << "out of memory\n"; exit(1); }
    return p;
}
static void heapFree(void *p) { g_freeCalls++; free(p); }

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

// the fixed 7 node tree: 4 at the root, 1 2 3 under the left child, 5 6 7 under the right
static Node *buildNamedTree() {
    Node *n1 = makeNode(1), *n2 = makeNode(2), *n3 = makeNode(3), *n4 = makeNode(4);
    Node *n5 = makeNode(5), *n6 = makeNode(6), *n7 = makeNode(7);
    n4->left = n2; n4->right = n6;
    n2->left = n1; n2->right = n3;
    n6->left = n5; n6->right = n7;
    return n4;
}

// a right leaning chain of n nodes, keys 1 .. n in path order
static Node *buildChain(int n) {
    Node *root = makeNode(1);
    Node *cur = root;
    for (int i = 2; i <= n; i++) { cur->right = makeNode(i); cur = cur->right; }
    return root;
}

static void appendKey(string &out, int key) {
    if (!out.empty()) out += ' ';
    out += to_string(key);
}

// ---------- the four orders, recursively ----------
static void inorderRec(const Node *n, string &out) {
    if (!n) return;
    inorderRec(n->left, out);
    appendKey(out, n->key);
    inorderRec(n->right, out);
}

static void preorderRec(const Node *n, string &out) {
    if (!n) return;
    appendKey(out, n->key);
    preorderRec(n->left, out);
    preorderRec(n->right, out);
}

static void postorderRec(const Node *n, string &out) {
    if (!n) return;
    postorderRec(n->left, out);
    postorderRec(n->right, out);
    appendKey(out, n->key);
}

// ---------- level order, queue based ----------
static void levelOrder(const Node *root, string &out) {
    if (!root) return;
    queue<const Node *> q;
    q.push(root);
    while (!q.empty()) {
        const Node *cur = q.front();
        q.pop();
        appendKey(out, cur->key);
        if (cur->left) q.push(cur->left);
        if (cur->right) q.push(cur->right);
    }
}

// level by level, with the node count of each level, which is where the 2^d
// growth of a binary tree becomes visible
static size_t levelLines(const Node *root) {
    size_t widest = 0;
    if (!root) return 0;
    queue<const Node *> q;
    q.push(root);
    size_t level = 0;
    while (!q.empty()) {
        const size_t count = q.size();
        if (count > widest) widest = count;
        cout << "  depth " << level << " (" << count << (count == 1 ? " node" : " nodes") << "):";
        for (size_t i = 0; i < count; i++) {
            const Node *cur = q.front();
            q.pop();
            cout << " " << cur->key;
            if (cur->left) q.push(cur->left);
            if (cur->right) q.push(cur->right);
        }
        cout << "\n";
        level++;
    }
    return widest;
}

// ---------- iterative in-order with an explicit stack, the recursion made visible ----------
static void inorderIterative(const Node *root, string &out, size_t &maxStack) {
    stack<const Node *> st;
    const Node *cur = root;
    maxStack = 0;
    while (cur || !st.empty()) {
        while (cur) {
            st.push(cur);
            if (st.size() > maxStack) maxStack = st.size();
            cur = cur->left;
        }
        cur = st.top();
        st.pop();
        appendKey(out, cur->key);
        cur = cur->right;
    }
}

// recursion depth of a recursive in-order walk, counted, not guessed
static void inorderWithDepth(const Node *n, int depth, int &maxDepth, string &out) {
    if (!n) return;
    inorderWithDepth(n->left, depth + 1, maxDepth, out);
    appendKey(out, n->key);
    if (depth > maxDepth) maxDepth = depth;
    inorderWithDepth(n->right, depth + 1, maxDepth, out);
}

// real frames: this walk records the address of a live local at every level,
// so the stack cost of the recursion is measured in bytes, not in words
struct FrameProbe {
    char *first = nullptr;
    char *last = nullptr;
    int   maxDepth = 0;
    int   frames = 0;
};

static void probeFrames(const Node *n, int depth, FrameProbe &fp) {
    int marker = depth;                        // a real local, so a real frame slot
    char *here = (char *)&marker;
    if (depth == 1) fp.first = here;
    fp.last = here;
    fp.frames++;
    if (depth > fp.maxDepth) fp.maxDepth = depth;
    if (n->left)  probeFrames(n->left, depth + 1, fp);
    if (n->right) probeFrames(n->right, depth + 1, fp);
}

// ---------- verification: every printed sequence is checked, not trusted ----------
static size_t g_checks = 0, g_failures = 0;

static bool verify(const char *label, const string &got, const char *expected) {
    const bool ok = (got == expected);
    g_checks++;
    if (!ok) g_failures++;
    cout << "  " << left << setw(24) << label
         << " got: " << setw(24) << got
         << " expected: " << setw(24) << expected
         << (ok ? " match" : " MISMATCH") << "\n";
    return ok;
}

static size_t keyCount(const string &s) {
    if (s.empty()) return 0;
    size_t n = 1;
    for (char c : s) if (c == ' ') n++;
    return n;
}

int main() {
    cout << "=== 1. the tree as a picture, then as the thing memory actually holds ===\n";
    cout << "  the named 7 node tree:\n";
    cout << "             4            depth 0, the root\n";
    cout << "           /   \\\n";
    cout << "          2     6         depth 1\n";
    cout << "         / \\   / \\\n";
    cout << "        1   3 5   7       depth 2, all four are leaves\n";
    cout << "  keys 1 2 3 4 5 6 7 placed so that an in-order walk returns them sorted:\n";
    cout << "  this is a binary search tree by construction, which is why in-order is the\n";
    cout << "  order you use when you want the keys back in order.\n";

    Node *tree = buildNamedTree();
    cout << "\n  sizeof(Node)=" << sizeof(Node) << " bytes, nodes allocated=" << g_allocCalls
         << ", bytes from the heap=" << g_allocBytes << "\n";
    cout << "  the root holds " << tree->key << ", its children hold "
         << tree->left->key << " and " << tree->right->key << "\n";

    cout << "\n=== 2. in-order: left subtree, node, right subtree ===\n";
    cout << "  visit rule: descend left, print the node, descend right. Recursive one liner.\n";
    string ino;
    inorderRec(tree, ino);
    const bool inoOk = verify("in-order", ino, "1 2 3 4 5 6 7");

    cout << "\n=== 3. pre-order: node, left subtree, right subtree ===\n";
    cout << "  the node is printed BEFORE its subtrees, so the root is always first:\n";
    string pre;
    preorderRec(tree, pre);
    const bool preOk = verify("pre-order", pre, "4 2 1 3 6 5 7");

    cout << "\n=== 4. post-order: left subtree, right subtree, node ===\n";
    cout << "  the node is printed AFTER its subtrees, so the root is always last:\n";
    string post;
    postorderRec(tree, post);
    const bool postOk = verify("post-order", post, "1 3 2 5 7 6 4");

    cout << "\n=== 5. level order: breadth first, queue based ===\n";
    cout << "  a queue, not the call stack: pop a node, print it, push its children at the\n";
    cout << "  back. FIFO is what makes the walk run across each level before going deeper.\n";
    string lvl;
    levelOrder(tree, lvl);
    const bool lvlOk = verify("level order", lvl, "4 2 6 1 3 5 7");
    cout << "\n  the same walk drawn level by level, with the count on each level:\n";
    const size_t widest = levelLines(tree);
    cout << "  the levels hold 1, 2 and 4 nodes in that order, so the widest level is "
         << widest << " nodes:\n";
    cout << "  a level of a binary tree holds at most 2^depth nodes, which is the whole\n";
    cout << "  reason a balanced tree has height near log2(n) and the search stays fast.\n";

    cout << "\n=== 6. iterative in-order with an explicit stack ===\n";
    cout << "  push the left spine, pop, print, then move to the popped node's right child.\n";
    cout << "  This is exactly what the recursive version does, with the frames spelled out.\n";
    string iter;
    size_t maxStack = 0;
    inorderIterative(tree, iter, maxStack);
    const bool iterOk = verify("in-order iterative", iter, "1 2 3 4 5 6 7");
    cout << "  the two in-order walks agree: " << (ino == iter ? "yes" : "NO") << "\n";
    cout << "  the explicit stack held at most " << maxStack << " nodes, which is the height of\n";
    cout << "  this tree (3): both formulations pay the tree height in memory, n either way.\n";

    cout << "\n=== 7. the four sequences against the shape, in one table ===\n";
    cout << "  " << left << setw(18) << "order" << setw(24) << "sequence" << "what it is for\n";
    cout << "  " << setw(18) << "in-order" << setw(24) << ino << "sorted keys, the BST check\n";
    cout << "  " << setw(18) << "pre-order" << setw(24) << pre << "copy or serialise a tree\n";
    cout << "  " << setw(18) << "post-order" << setw(24) << post << "free a tree, evaluate an expression\n";
    cout << "  " << setw(18) << "level order" << setw(24) << lvl << "shortest paths, level by level printing\n";
    const bool sameKeys = (keyCount(ino) == 7 && keyCount(pre) == 7 && keyCount(post) == 7
                           && keyCount(lvl) == 7);
    cout << "  all four sequences hold " << keyCount(ino) << ", " << keyCount(pre) << ", "
         << keyCount(post) << " and " << keyCount(lvl)
         << " keys, one per node of the 7 node tree: " << (sameKeys ? "confirmed" : "MISMATCH") << "\n";
    cout << "  every order visits every node of a 7 node tree, so all four are O(n);\n";
    cout << "  the difference is WHEN each node is visited, and that is what makes them useful.\n";

    cout << "\n=== 8. the skew case: a chain, where recursion depth equals n ===\n";
    const int CHAIN_N = 7;
    Node *chain = buildChain(CHAIN_N);
    cout << "  keys 1 .. " << CHAIN_N << " inserted along the right child only, so the tree IS a list:\n";
    cout << "  1 -> 2 -> 3 -> 4 -> 5 -> 6 -> 7 downwards, height " << CHAIN_N
         << " and no branching at all\n";
    string chainIno, chainPre, chainPost, chainLvl;
    inorderRec(chain, chainIno);
    preorderRec(chain, chainPre);
    postorderRec(chain, chainPost);
    levelOrder(chain, chainLvl);
    bool chainOk = true;
    chainOk = verify("chain in-order", chainIno, "1 2 3 4 5 6 7") && chainOk;
    chainOk = verify("chain pre-order", chainPre, "1 2 3 4 5 6 7") && chainOk;
    chainOk = verify("chain post-order", chainPost, "7 6 5 4 3 2 1") && chainOk;
    chainOk = verify("chain level order", chainLvl, "1 2 3 4 5 6 7") && chainOk;
    cout << "  note that in-order and pre-order print the same sequence on a right leaning\n";
    cout << "  chain: with no left child, there is nothing between the node and its subtree.\n";

    int chainDepth = 0;
    string chainDepthSeq;
    inorderWithDepth(chain, 1, chainDepth, chainDepthSeq);
    int treeDepth = 0;
    string treeDepthSeq;
    inorderWithDepth(tree, 1, treeDepth, treeDepthSeq);
    cout << "\n  recursion depth, counted inside the walk:\n";
    cout << "    chain  of " << CHAIN_N << " nodes: depth = " << chainDepth
         << ", so depth equals n: " << (chainDepth == CHAIN_N ? "yes" : "no") << "\n";
    cout << "    named tree of 7 nodes : depth = " << treeDepth
         << ", against the height of a balanced 7 node tree\n";
    cout << "  same node count, " << chainDepth << " frames against " << treeDepth
         << ": that is a factor of " << fixed << setprecision(2)
         << ((double)chainDepth / (double)treeDepth) << " in stack use\n";
    cout << "  and the depth checksum is the same sequence both times: "
         << (chainDepthSeq == chainIno && treeDepthSeq == ino ? "yes" : "no") << "\n";

    FrameProbe fp;
    probeFrames(chain, 1, fp);
    const long long span = (fp.frames > 1) ? (long long)(fp.first - fp.last) : 0;
    const double perCall = (fp.frames > 1) ? (double)span / (double)(fp.frames - 1) : 0.0;
    cout << "\n  the frames themselves, the address of a live local at each level:\n";
    cout << "    first frame at " << (void *)fp.first << ", deepest at " << (void *)fp.last << "\n";
    cout << "    " << fp.frames << " frames recorded, so " << (fp.frames - 1) << " gaps between them\n";
    cout << "    the stack grows down on x86-64, so the deepest frame sits " << span
         << " bytes below the first\n";
    cout << "    that is " << fixed << setprecision(1) << perCall << " bytes per suspended call\n";
    cout << "    charging every live frame, the peak is " << fp.maxDepth << " x " << perCall
         << " = " << (fp.maxDepth * perCall) << " bytes of stack for " << CHAIN_N << " nodes\n";
    cout << "    measured max depth from the probe = " << fp.maxDepth
         << ", equal to n = " << CHAIN_N << ": " << (fp.maxDepth == CHAIN_N ? "yes" : "no") << "\n";

    cout << "\n=== 9. verification summary and teardown ===\n";
    cout << "  sequences checked in code: " << g_checks << ", mismatches: " << g_failures
         << "  " << (g_failures == 0 ? "all sequences matched the expected text" : "FAILURES PRESENT") << "\n";
    cout << "  all eight named sequences agreed: "
         << (inoOk && preOk && postOk && lvlOk && iterOk && chainOk && g_failures == 0 ? "yes" : "no") << "\n";
    releaseTree(tree);
    releaseTree(chain);
    cout << "  nodes allocated " << g_allocCalls << ", nodes freed " << g_freeCalls << "  "
         << (g_allocCalls == g_freeCalls ? "every node returned" : "LEAK") << "\n";

    cout << "\n=== 10. the one line to remember ===\n";
    cout << "  a traversal is one recursion with the print in three different places:\n";
    cout << "  before the children (pre), between them (in), after them (post), and a queue\n";
    cout << "  instead of the call stack gives level order. All four are O(n) in time, and\n";
    cout << "  the recursion depth is the height of the tree, which is log n only if the\n";
    cout << "  tree is balanced and n when it has degenerated into a chain.\n";
    return 0;
}
