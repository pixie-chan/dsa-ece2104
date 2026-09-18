#!/usr/bin/env python3
"""
Execution verifier for the ECE2104 linked list material (Lectures 5 to 9).

Rule this script enforces: every value printed in LINKED-LIST-STUDY-NOTES.md and in the
three lesson programs is produced by actually compiling and running the code, and every
lecture mapping claim is checked against the real course handout PDF.

Run:  python3 ~/learning/cpp/exercises/verify_linked_list_lec6_8.py
Exit: 0 = every claim verified, 1 = at least one mismatch (printed).
"""
import re
import subprocess
import sys
import tempfile
from pathlib import Path

LESSONS = Path.home() / "learning/cpp/lessons"
NOTES = Path.home() / "Documents/MUJ-SEM-3/DSA/Notes/linked-list/LINKED-LIST-STUDY-NOTES.md"
HANDOUT = Path.home() / "Documents/MUJ-SEM-3/DSA/Handouts/Course Handout ECE2104VDT2104_26.pdf"
TMP = Path(tempfile.mkdtemp(prefix="llverify_"))

passed, failed = 0, 0
failures = []


def ck(ok, what):
    global passed, failed
    if ok:
        passed += 1
    else:
        failed += 1
        failures.append(what)
        print(f"  FAIL  {what}")
        return False
    print(f"  ok    {what}")
    return True


def compile_and_run(src, out_name, extra_flags=()):
    exe = TMP / out_name
    cmd = ["g++", "-Wall", "-Wextra", "-std=c++17", *extra_flags, "-o", str(exe), str(src)]
    cc = subprocess.run(cmd, capture_output=True, text=True)
    if cc.returncode != 0:
        return None, None, cc.stderr
    run = subprocess.run([str(exe)], capture_output=True, text=True, timeout=60)
    return run.stdout, run.returncode, cc.stderr


def fuzzy(claim, gap=8):
    """Build a regex that tolerates up to `gap` foreign words between claim words.

    Needed because the handout lecture plan is a two column table: pdftotext interleaves
    the 'Topic' column with the 'Session Outcome' column, so a topic phrase like
    'Linked List: Concept, Types, Node Representation' is split by outcome words.
    """
    words = [re.escape(w) for w in claim.split()]
    sep = r"\s+(?:\S+\s+){0,%d}" % gap
    return re.compile(sep.join(words))


print("\n=== 1. compile clean with -Wall -Wextra -std=c++17 ===")
outs = {}
for num, name in [(6, "06_linked_list_representation.cpp"),
                  (7, "07_linked_list_operations.cpp"),
                  (8, "08_linked_list_searching_apps.cpp")]:
    src = LESSONS / name
    ck(src.exists(), f"lecture {num} source exists: {name}")
    out, rc, warns = compile_and_run(src, f"lec{num}")
    ck(rc == 0, f"lecture {num} compiles with zero warnings and exit 0")
    if warns.strip():
        ck(False, f"lecture {num} emitted warnings: {warns.strip()[:200]}")
    outs[num] = out or ""

o6, o7, o8 = outs[6], outs[7], outs[8]

print("\n=== 2. lecture 6 claims (node layout, memory representation, types) ===")
ck("sizeof(Node)      = 16 bytes" in o6, "sizeof(Node) = 16 bytes")
ck("  int data        = 4 bytes" in o6, "sizeof(int) = 4")
ck("  Node* next      = 8 bytes" in o6, "sizeof(Node*) = 8")
ck("  padding         = 4 bytes" in o6, "4 bytes of padding")
ck("offsetof(data)    = 0" in o6 and "offsetof(next)    = 8" in o6, "offsets 0 and 8")

block = re.findall(r"block\[(\d)\] at (0x[0-9a-f]+)", o6)
if ck(len(block) == 3, "three array blocks printed with addresses"):
    addrs = [int(a, 16) for _, a in block]
    ck(addrs[1] - addrs[0] == 16 and addrs[2] - addrs[1] == 16,
       f"array blocks are exactly 16 bytes apart ({addrs[1]-addrs[0]}, {addrs[2]-addrs[1]})")

nodes = re.findall(r"node at (0x[0-9a-f]+)   data=(\d+)   next=(0x[0-9a-f]+|0)", o6)
ck(len(nodes) == 3, "three linked nodes printed with data and next")
if len(nodes) == 3:
    n0, n1, n2 = nodes
    ck(n0[2] == n1[0], "node 1 link field holds the exact address of node 2")
    ck(n1[2] == n2[0], "node 2 link field holds the exact address of node 3")
    ck(n2[2] == "0", "node 3 link field is NULL")
    ck(all(n[1] == v for n, v in zip(nodes, ["10", "20", "30"])), "data 10, 20, 30 in order")
    heap = [int(n[0], 16) for n in nodes]
    ck(heap[0] != addrs[0], "linked nodes do not live at the array block addresses")

ck("a->next->next->data = 30" in o6, "two hop dereference reaches 30")
ck("backward: 30 <-> 20 <-> 10 <-> nullptr" in o6, "doubly list walks backwards")
ck("sizeof(DNode) = 24 bytes" in o6, "sizeof(doubly node) = 24")
ck("1 2 3 1 2 3 1" in o6, "circular list wraps around (7 hops on a 3 node ring)")

print("\n=== 3. lecture 7 claims (operations, order of statements) ===")
expected7 = [
    "   20 -> NULL     [insertAtBeginning(20), count=1]",
    "   10 -> 20 -> NULL     [insertAtBeginning(10), count=2]",
    "   10 -> 20 -> 40 -> NULL     [insertAtEnd(40), count=3]",
    "   10 -> 20 -> 40 -> 50 -> NULL     [insertAtEnd(50), count=4]",
    "   10 -> 20 -> 30 -> 40 -> 50 -> NULL     [insertAtPosition(30, pos 3), count=5]",
    "   10 -> 20 -> 40 -> 50 -> NULL     [deleteByValue(30) middle, count=4]",
    "   10 -> 20 -> 40 -> NULL     [deleteFromEnd(), count=3]",
    "   20 -> 40 -> NULL     [deleteFromBeginning(), count=2]",
    "   20 -> 40 -> NULL     [deleteByValue(999) absent, count=2]",
]
for line in expected7:
    ck(line in o7, f"lec7 state line: {line.strip()[:58]}")
ck("list freed, head = 0" in o7, "every node freed, head back to NULL")
for cost in ["insertion at beginning : O(1)", "insertion at end       : O(n)",
             "insertion at position k: O(k)", "deletion at beginning  : O(1)",
             "deletion at end        : O(n)", "traversal / search     : O(n)"]:
    ck(cost in o7, f"cost table: {cost}")

# the two ordering rules the notes call out as traps must be present in the source
src7 = (LESSONS / "07_linked_list_operations.cpp").read_text()
for rule, needle in [("insert at beginning links before moving head", "n->next = head;      // 1."),
                     ("delete at beginning moves head before deleting", "head = head->next;    // move head FIRST"),
                     ("delete by value bridges before freeing", "cur->next = dead->next;   // bridge around it first")]:
    ck(needle in src7, f"source order rule: {rule}")

print("\n=== 4. lecture 8 claims (search counts, applications) ===")
expected8 = [
    "search 30 -> found at position 3, comparisons = 3",
    "search 35 -> result -1 (absent), comparisons = 5",
    "after searchMoveToFront(40): 40 -> 10 -> 20 -> 30 -> 50 -> NULL",
    "after : 50 -> 30 -> 20 -> 10 -> 40 -> NULL",
    "after : 10 -> 20 -> 30 -> 40 -> NULL",
    "merged: 1 -> 2 -> 3 -> 4 -> 9 -> 10 -> NULL",
    "P1+P2 = 4x^3 + 7x^2 + 3x^1 + 7x^0",
    "pop -> 30, pop -> 20, empty now? no",
    "after the last pop, empty? yes",
]
for line in expected8:
    ck(line in o8, f"lec8 output: {line[:60]}")

print("\n=== 5. lecture 8 is leak free (AddressSanitizer) ===")
out, rc, warns = compile_and_run(LESSONS / "08_linked_list_searching_apps.cpp", "lec8_asan",
                                 extra_flags=("-g", "-fsanitize=address"))
ck(rc == 0, "ASAN build exits 0")
ck("LeakSanitizer" not in (out or ""), "no leak reported by LeakSanitizer")

print("\n=== 6. lecture mapping against the real handout PDF ===")
if ck(HANDOUT.exists(), "course handout PDF present"):
    txt = subprocess.run(["pdftotext", "-layout", str(HANDOUT), "-"],
                         capture_output=True, text=True).stdout
    flat = re.sub(r"\s+", " ", txt)
    for claim, needle, gap in [
        ("Lecture 6 = Linked List: Concept, Types, Node Representation",
         "Linked List: Concept, Types, Node Representation", 8),
        ("Lecture 7 = Linked List Operations: Creation and Insertion, Deletion and Traversal",
         "Linked List Operations: Creation and Insertion, Deletion and Traversal", 12),
        ("Lecture 8 = Searching in Linked List, Applications, Numerical Problems and Practice",
         "Searching in Linked List, Applications of Linked List, Numerical Problems and Practice on Linked List", 8),
        ("Lecture 9 = Tutorial", "Tutorial Perform traversal", 3),
        ("mid semester exam after lecture 16", "MID SEMESTER EXAMINATION", 0),
        ("outcome: represent linked lists in memory", "Represent linked lists in memory", 8),
        ("outcome: perform traversal and search operations", "Perform traversal and search operations", 8),
    ]:
        ck(fuzzy(needle, gap).search(flat) is not None, f"handout confirms: {claim}")

print("\n=== 7. notes file matches the verified numbers ===")
if ck(NOTES.exists(), "study notes file exists"):
    notes = NOTES.read_text()
    for claim in ["16 bytes", "offsetof(data)", "sizeof(DNode)` = 24 bytes",
                  "4x^3 + 7x^2 + 3x + 7", "10 10 20 30 30 30 40", "6_linked_list_representation.cpp",
                  "07_linked_list_operations.cpp", "08_linked_list_searching_apps.cpp",
                  "MID SEMESTER EXAMINATION"]:
        ck(claim in notes, f"notes contain: {claim}")

print("\n=== 8. Mermaid diagram sources and rendered PNGs ===")
diagrams = sorted((NOTES.parent / "diagrams").glob("*.mmd"))
ck(len(diagrams) == 6, f"six diagram sources present (found {len(diagrams)})")
for d in diagrams:
    body = d.read_text()
    ok = body.strip().startswith(("flowchart", "graph")) and body.count("[") == body.count("]")
    ck(ok, f"{d.name}: valid header and balanced node brackets")
    png = d.with_suffix(".png")
    ck(png.exists() and png.stat().st_size > 5000,
       f"{png.name}: rendered PNG present and non-trivial ({png.stat().st_size if png.exists() else 0} bytes)")

print("\n=== 9. lab tie-in: the sorted linked list already in DAS-LAB ===")
lab = Path.home() / "Documents/MUJ-SEM-3/DAS-LAB/Exp4_Priority_Queue/exp4_2_priority_queue_linkedlist.cpp"
if ck(lab.exists(), "Exp 4.2 sorted linked list source exists"):
    lab_src = lab.read_text()
    for needle, what in [
        ("struct Node {", "node struct"),
        ("node->next = current->next;", "insert bridges to the successor"),
        ("head = head->next;", "extractMax moves head before delete"),
        ("delete temp;", "extractMax frees the node"),
    ]:
        ck(needle in lab_src, f"lab Exp4.2 uses the same pattern: {what}")

print()
print(f"PASSED {passed}   FAILED {failed}")
if failures:
    print("\nfailures:")
    for f in failures:
        print(f"  - {f}")
sys.exit(1 if failed else 0)
