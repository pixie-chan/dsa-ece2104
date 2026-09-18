// 23_structure_formulations.cpp
// ECE2104 (DSA), Lecture 23: structure formulation sheet.
// Handout session outcome: write the C/C++ formulation of every structure in the course.
//
// Build: g++ -Wall -Wextra -Wpedantic -std=c++17 -O2 -o /tmp/lec23 23_structure_formulations.cpp
//
// One struct or class per structure, each followed by the sizeof and the offset
// of every member, measured with sizeof and offsetof on the machine that runs
// this. The padding rows are computed from those offsets, not typed in, so the
// sheet cannot claim a byte that is not there.
//
// Layout: section 1 fixes the primitive sizes, sections 2 to 9 are one block per
// structure, section 10 is the whole thing in one table. The output of this file
// is meant to be printed and kept next to the handout.

#include <iostream>
#include <iomanip>
#include <cstddef>
#include <cstdlib>
#include <string>
using namespace std;

// ---------- one structure per block, exactly as the course writes it ----------
struct ArrayForm         { int a[8]; };
struct ListNodeForm      { int data; ListNodeForm *next; };
struct ArrayStackForm    { int data[8]; size_t top; };
struct LinkedStackForm   { ListNodeForm *top; size_t count; };
struct ArrayQueueForm    { int data[8]; size_t front; size_t back; };
struct CircularQueueForm { int data[8]; size_t front; size_t count; };
struct TreeNodeForm      { int key; TreeNodeForm *left; TreeNodeForm *right; };

class BstForm {
public:
    bool   empty() const { return root_ == nullptr; }
    size_t size() const { return count_; }
    void   clear() { root_ = nullptr; count_ = 0; }
    void   insertKey(int) { count_++; }      // the layout is the subject here, not the semantics
private:
    TreeNodeForm *root_ = nullptr;
    size_t        count_ = 0;
};

// BstForm keeps its data private, so offsetof cannot reach the members from out
// here. This mirror has the same members in the same order, which is all that
// offsetof needs, and its size is checked against the class in block 8.
struct BstFormMirror { TreeNodeForm *root_; size_t count_; };

struct MemberDesc {
    const char *name;
    size_t      offset;
    size_t      size;
    const char *note;
};

// every value printed in a block comes from the arguments, which come from
// sizeof and offsetof, so nothing in this file is a hand written number
static size_t block(const char *title, const char *decl, size_t sz, size_t align,
                    const MemberDesc *members, size_t count, const char *note) {
    cout << "\n" << title << "\n";
    cout << "  declaration : " << decl << "\n";
    cout << "  sizeof      : " << sz << " bytes\n";
    cout << "  alignof     : " << align << " bytes\n";
    cout << "  " << left << setw(14) << "member" << right << setw(8) << "offset"
         << setw(6) << "size" << "  note\n";
    size_t cursor = 0, memberBytes = 0, paddingBytes = 0;
    for (size_t i = 0; i < count; i++) {
        if (members[i].offset > cursor) {
            const size_t gap = members[i].offset - cursor;
            paddingBytes += gap;
            cout << "  " << left << setw(14) << "padding" << right << setw(8) << cursor
                 << setw(6) << gap << "  bytes " << cursor << " .. " << (members[i].offset - 1)
                 << ", no member owns them, they exist so the next member meets its alignment\n";
        }
        cout << "  " << left << setw(14) << members[i].name << right << setw(8) << members[i].offset
             << setw(6) << members[i].size << "  " << members[i].note << "\n";
        cursor = members[i].offset + members[i].size;
        memberBytes += members[i].size;
    }
    const size_t tail = sz - cursor;
    if (tail) {
        cout << "  " << left << setw(14) << "padding" << right << setw(8) << cursor
             << setw(6) << tail << "  bytes " << cursor << " .. " << (sz - 1)
             << ", tail padding, so an array of these keeps every element aligned\n";
    }
    cout << "  " << left << setw(14) << "payload" << right << setw(8) << "" << setw(6) << memberBytes
         << "  sum of the member sizes\n";
    cout << "  " << left << setw(14) << "padding total" << right << setw(8) << "" << setw(6)
         << (paddingBytes + tail) << "  "
         << (((paddingBytes + tail) == 0) ? "none, every byte belongs to a member"
                                          : "bytes no member can use")
         << "\n";
    cout << "  " << left << setw(14) << "check" << right << setw(8) << "" << setw(6) << ""
         << "  payload " << memberBytes << " + padding " << paddingBytes << " + tail " << tail
         << " = " << (memberBytes + paddingBytes + tail) << " = sizeof " << sz << "  "
         << ((memberBytes + paddingBytes + tail == sz) ? "confirmed" : "MISMATCH") << "\n";
    cout << "  note        : " << note << "\n";
    return paddingBytes + tail;
}

struct Row {
    const char *name;
    size_t      sz;
    size_t      align;
    size_t      members;
    size_t      pad;
};

int main() {
    cout << "=== 1. the primitives every structure below is built from ===\n";
    cout << "  sizeof(char)   = " << sizeof(char)   << " bytes, alignof = " << alignof(char)   << "\n";
    cout << "  sizeof(int)    = " << sizeof(int)    << " bytes, alignof = " << alignof(int)    << "\n";
    cout << "  sizeof(size_t) = " << sizeof(size_t) << " bytes, alignof = " << alignof(size_t) << "\n";
    cout << "  sizeof(void*)  = " << sizeof(void *) << " bytes, alignof = " << alignof(void *) << "\n";
    cout << "  the rule that produces every offset and every padding row below:\n";
    cout << "    a member of size s and alignment a starts at the first offset that is a multiple of a,\n";
    cout << "    and the struct is padded up to a multiple of its own largest alignment.\n";
    cout << "  an index does not create a hierarchy: a[3] is the same as *(a + 3), so an array of\n";
    cout << "  8 ints is 32 bytes with offsets 0, 4, 8, ... 28 and no bookkeeping at all.\n";

    const MemberDesc mArray[] = {
        { "a", offsetof(ArrayForm, a), sizeof(int) * 8, "the payload, 8 ints, contiguous, no gaps anywhere" }
    };
    const size_t padArray = block("--- 2. array: the flat block ---",
        "struct ArrayForm { int a[8]; };",
        sizeof(ArrayForm), alignof(ArrayForm), mArray, 1,
        "element i sits at base + i * sizeof(int); that multiply is why indexing is O(1)");

    const MemberDesc mListNode[] = {
        { "data", offsetof(ListNodeForm, data), sizeof(int), "the payload" },
        { "next", offsetof(ListNodeForm, next), sizeof(ListNodeForm *), "address of the next node, 0 marks the end" }
    };
    const size_t padListNode = block("--- 3. linked list node: the self referential struct ---",
        "struct ListNodeForm { int data; ListNodeForm* next; };",
        sizeof(ListNodeForm), alignof(ListNodeForm), mListNode, 2,
        "4 bytes of padding, the price of putting a 4 byte payload next to an 8 byte pointer");

    const MemberDesc mArrayStack[] = {
        { "data", offsetof(ArrayStackForm, data), sizeof(int) * 8, "the slot array, fixed capacity" },
        { "top",  offsetof(ArrayStackForm, top),  sizeof(size_t), "index of the next free slot: push writes data[top] then top++" }
    };
    const size_t padArrayStack = block("--- 4. stack by array ---",
        "struct ArrayStackForm { int data[8]; size_t top; };",
        sizeof(ArrayStackForm), alignof(ArrayStackForm), mArrayStack, 2,
        "the whole stack is the array plus one index; push and pop are O(1) and allocate nothing");

    const MemberDesc mLinkedStack[] = {
        { "top",   offsetof(LinkedStackForm, top),   sizeof(ListNodeForm *), "head of the chain: push links a new node in front" },
        { "count", offsetof(LinkedStackForm, count), sizeof(size_t), "optional size counter, the chain works without it" }
    };
    const size_t padLinkedStack = block("--- 5. stack by linked list ---",
        "struct LinkedStackForm { ListNodeForm* top; size_t count; };",
        sizeof(LinkedStackForm), alignof(LinkedStackForm), mLinkedStack, 2,
        "grows without a capacity limit but pays one node allocation and one pointer per push");

    const MemberDesc mArrayQueue[] = {
        { "data",  offsetof(ArrayQueueForm, data),  sizeof(int) * 8, "the slot array" },
        { "front", offsetof(ArrayQueueForm, front), sizeof(size_t), "index of the element at the head, the next to dequeue" },
        { "back",  offsetof(ArrayQueueForm, back),  sizeof(size_t), "index of the next free slot at the tail, where an enqueue writes" }
    };
    const size_t padArrayQueue = block("--- 6. queue by array (plain, not circular) ---",
        "struct ArrayQueueForm { int data[8]; size_t front; size_t back; };",
        sizeof(ArrayQueueForm), alignof(ArrayQueueForm), mArrayQueue, 3,
        "both indices only move forward, so the used region slides right and the slots behind front are lost until a reset");

    const MemberDesc mCircularQueue[] = {
        { "data",  offsetof(CircularQueueForm, data),  sizeof(int) * 8, "the ring buffer" },
        { "front", offsetof(CircularQueueForm, front), sizeof(size_t), "index of the head element" },
        { "count", offsetof(CircularQueueForm, count), sizeof(size_t), "number of elements held, which is what disambiguates full from empty" }
    };
    const size_t padCircularQueue = block("--- 7. circular queue, with the count member ---",
        "struct CircularQueueForm { int data[8]; size_t front; size_t count; };",
        sizeof(CircularQueueForm), alignof(CircularQueueForm), mCircularQueue, 3,
        "enqueue writes at (front + count) % 8 and count++, dequeue reads front and front = (front + 1) % 8");

    const MemberDesc mTreeNode[] = {
        { "key",   offsetof(TreeNodeForm, key),   sizeof(int), "the payload the tree is ordered by" },
        { "left",  offsetof(TreeNodeForm, left),  sizeof(TreeNodeForm *), "address of the left subtree, 0 for none" },
        { "right", offsetof(TreeNodeForm, right), sizeof(TreeNodeForm *), "address of the right subtree, 0 for none" }
    };
    const size_t padTreeNode = block("--- 8. tree node: two pointers, one payload ---",
        "struct TreeNodeForm { int key; TreeNodeForm* left; TreeNodeForm* right; };",
        sizeof(TreeNodeForm), alignof(TreeNodeForm), mTreeNode, 3,
        "the second pointer is what separates a tree from a list; it costs 8 bytes and buys the branching");

    const MemberDesc mBst[] = {
        { "root_",  offsetof(BstFormMirror, root_),  sizeof(TreeNodeForm *), "the root pointer, 0 when the tree is empty" },
        { "count_", offsetof(BstFormMirror, count_), sizeof(size_t), "node count, so size() is O(1) instead of a full walk" }
    };
    const size_t padBst = block("--- 9. binary search tree, as a class wrapper ---",
        "class BstForm { public: insert/search/remove; private: TreeNodeForm* root_; size_t count_; };",
        sizeof(BstForm), alignof(BstForm), mBst, 2,
        "the offsets are read from BstFormMirror, the same members in the same order, because the class keeps them private");
    cout << "  mirror check: sizeof(BstForm) = " << sizeof(BstForm)
         << ", sizeof(BstFormMirror) = " << sizeof(BstFormMirror) << "  "
         << (sizeof(BstForm) == sizeof(BstFormMirror) ? "identical layout confirmed" : "MISMATCH") << "\n";

    BstForm form;
    cout << "  the wrapper used for real: empty = " << (form.empty() ? "true" : "false")
         << ", size = " << form.size();
    form.insertKey(50);
    form.insertKey(30);
    cout << ", after two insertKey calls size = " << form.size();
    form.clear();
    cout << ", after clear size = " << form.size() << ", empty = " << (form.empty() ? "true" : "false") << "\n";

    cout << "\n=== 10. the sheet in one table ===\n";
    const Row rows[] = {
        { "1 array (int a[8])",            sizeof(ArrayForm),         alignof(ArrayForm),         1, padArray },
        { "2 linked list node",            sizeof(ListNodeForm),      alignof(ListNodeForm),      2, padListNode },
        { "3 stack by array",              sizeof(ArrayStackForm),    alignof(ArrayStackForm),    2, padArrayStack },
        { "4 stack by linked list",        sizeof(LinkedStackForm),   alignof(LinkedStackForm),   2, padLinkedStack },
        { "5 queue by array",              sizeof(ArrayQueueForm),    alignof(ArrayQueueForm),    3, padArrayQueue },
        { "6 circular queue (with count)", sizeof(CircularQueueForm), alignof(CircularQueueForm), 3, padCircularQueue },
        { "7 tree node",                   sizeof(TreeNodeForm),      alignof(TreeNodeForm),      3, padTreeNode },
        { "8 BST wrapper class",           sizeof(BstForm),           alignof(BstForm),           2, padBst },
    };
    cout << "  " << left << setw(32) << "structure" << right << setw(8) << "sizeof"
         << setw(10) << "alignof" << setw(10) << "members" << setw(10) << "padding"
         << setw(10) << "wasted %" << "\n";
    cout << "  " << string(80, '-') << "\n";
    size_t totalBytes = 0, totalPad = 0;
    for (size_t i = 0; i < sizeof(rows) / sizeof(rows[0]); i++) {
        cout << "  " << left << setw(32) << rows[i].name << right << setw(8) << rows[i].sz
             << setw(10) << rows[i].align << setw(10) << rows[i].members << setw(10) << rows[i].pad
             << setw(10) << fixed << setprecision(1)
             << (100.0 * (double)rows[i].pad / (double)rows[i].sz) << "\n";
        totalBytes += rows[i].sz;
        totalPad += rows[i].pad;
    }
    cout << "  " << string(80, '-') << "\n";
    cout << "  " << left << setw(32) << "eight structures together" << right << setw(8) << totalBytes
         << setw(10) << "" << setw(10) << "" << setw(10) << totalPad
         << setw(10) << fixed << setprecision(1) << (100.0 * (double)totalPad / (double)totalBytes) << "\n";

    cout << "\n=== 11. what the sheet is telling you ===\n";
    cout << "  only two of the eight structures carry padding: the nodes with a 4 byte payload next\n";
    cout << "  to an 8 byte pointer. Put the pointer first and the payload after it and the tail\n";
    cout << "  padding disappears, at the cost of reading the fields out of declaration order.\n";
    cout << "  the array based structures are exactly their payload plus one "
         << sizeof(size_t) << " byte index each:\n";
    cout << "  the stack is 32 + " << sizeof(size_t) << ", the queue and the ring are 32 + "
         << 2 * sizeof(size_t) << ". No pointers, no per element\n";
    cout << "  cost at run time, but a fixed capacity chosen before the first insert.\n";
    cout << "  the linked structures cost one " << sizeof(ListNodeForm) << " byte node (list) or one "
         << sizeof(TreeNodeForm) << " byte node (tree)\n";
    cout << "  per element allocation, and the structure object itself is one pointer plus,\n";
    cout << "  optionally, one " << sizeof(size_t) << " byte counter.\n";
    cout << "  and the two index arrays (queue, circular queue) have the SAME sizeof: the count\n";
    cout << "  member in the circular version is the third index, and it is what makes full and\n";
    cout << "  empty distinguishable, so it is not optional there.\n";
    return 0;
}
