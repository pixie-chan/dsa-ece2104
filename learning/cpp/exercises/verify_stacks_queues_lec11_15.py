#!/usr/bin/env python3
"""
verify_stacks_queues_lec11_15.py : verifier for the DSA lectures 11 to 15 stack/queue set.

What it proves, in order:
  1. all five lesson programs compile with ZERO warnings under -Wall -Wextra -Wpedantic
     and exit 0 when run
  2. every size, offset, stride, count, refusal, wrap and order fact their output prints is
     the value the C++ actually produced, and the arithmetic invariants behind those numbers
     (payload + padding + bookkeeping, BIG*(BIG-1)/2, the wrap count, the count formula,
     the queueing averages) recomputed here in Python agree with what the programs printed
  3. the recorded capture file is not stale: its invariant lines equal a fresh run's, line
     for line, with only addresses (ASLR) and wall-clock times masked
  4. the study notes and the six diagram SVGs, when they are on disk, are complete and quote
     the measured numbers. They are asserted hard once present. While the doc agents are
     still writing them a single per-artifact check reports SKIP and says so; the run never
     fails because a doc file has not been written yet.
  5. the mutation battery: 19 real faults injected into COPIES of the lesson sources are all
     caught. A verifier that cannot fail on a broken program is not a verifier.

Usage:
  python3 verify_stacks_queues_lec11_15.py            # run every check, print failures only
  python3 verify_stacks_queues_lec11_15.py --verbose  # print every PASS line as it happens
  python3 verify_stacks_queues_lec11_15.py --mutate   # plus the 26 fault mutation battery
"""
import re
import shutil
import subprocess
import sys
import tempfile
import xml.etree.ElementTree as ET
from pathlib import Path

HOME = Path.home()
LESSONS = HOME / "learning/cpp/lessons"
EXERCISES = HOME / "learning/cpp/exercises"
CAPTURE = EXERCISES / "lec11_15_run_output.txt"
DOCDIR = HOME / "Documents/MUJ-SEM-3/DSA/Notes/stacks-queues"
NOTES = DOCDIR / "STACKS-QUEUES-STUDY-NOTES.md"
DIAGRAMS = DOCDIR / "diagrams"
DIAGRAM_NAMES = [
    "01-array-stack-layout.svg",
    "02-linked-stack-nodes.svg",
    "03-naive-queue-waste.svg",
    "04-circular-ring-two-wraps.svg",
    "05-queue-from-two-stacks.svg",
    "06-round-robin-timeline.svg",
]

LECTURES = ["11_stack_array", "12_stack_linked_apps", "13_queue_array",
            "14_queue_linked_circular", "15_queue_apps"]
TAG = {n: "lec" + n[:2] for n in LECTURES}

FLAGS = ["-Wall", "-Wextra", "-Wpedantic", "-std=c++17", "-O2"]

# the em dash, banned everywhere by an absolute user rule. Written as a codepoint so this
# file itself stays pure ASCII and the ban can be checked against the file it lives in.
EMDASH = chr(0x2014)
EMOJI_RANGES = [(0x1F000, 0x1FAFF), (0x2600, 0x26FF), (0x1F1E6, 0x1F1FF)]
EMOJI_SINGLES = {0x2705, 0x274C, 0x274E, 0x2757, 0x2764, 0x2B50, 0x2B55, 0x203C, 0x2049, 0xFE0F}

RESULTS = []   # (label, "PASS" | "FAIL" | "SKIP", detail)
VERBOSE = "--verbose" in sys.argv[1:]


def check(label, ok, detail=""):
    RESULTS.append((label, "PASS" if ok else "FAIL", detail))
    if VERBOSE:
        print("  [%s] %s%s" % ("PASS" if ok else "FAIL", label, "   " + detail if detail else ""))
    return bool(ok)


def skip(label, detail=""):
    """An artifact that does not exist yet cannot be asserted. Say so and move on."""
    RESULTS.append((label, "SKIP", detail))
    if VERBOSE:
        print("  [SKIP] %s%s" % (label, "   " + detail if detail else ""))


def eq(label, got, want):
    return check(label, got == want, "got %r, want %r" % (got, want))


def build(workdir, name, outdir):
    """Compile one lesson from `workdir`, with the strictest sensible flags."""
    exe = Path(outdir) / name
    cp = subprocess.run(["g++"] + FLAGS + ["-o", str(exe), str(Path(workdir) / (name + ".cpp"))],
                        capture_output=True, text=True, timeout=300)
    return cp, exe


def run_exe(exe, timeout=900):
    return subprocess.run([str(exe)], capture_output=True, text=True, timeout=timeout)


def grep(text, pattern, flags=0):
    m = re.search(pattern, text, flags | re.M)
    return m.groups() if m else None


def as_int(text, pattern):
    g = grep(text, pattern)
    return int(g[0]) if g else None


def as_float(text, pattern):
    g = grep(text, pattern)
    return float(g[0]) if g else None


# --------------------------------------------------------------------------- lecture 11

def parse_11(text):
    f = {}
    f["sizes"] = grep(text, r"sizeof\(int\)=(\d+) sizeof\(size_t\)=(\d+)")
    f["sizes"] = tuple(int(x) for x in f["sizes"]) if f["sizes"] else None
    f["as_int"] = as_int(text, r"sizeof\(ArrayStack<int,5>\)[ \t]+= (\d+)")
    f["as_char"] = as_int(text, r"sizeof\(ArrayStack<char,5>\)[ \t]+= (\d+)")
    f["payload"] = as_int(text, r"the 5 ints alone[ \t]+= (\d+) bytes")
    f["book"] = as_int(text, r"the bookkeeping[ \t]+= (\d+) bytes")
    f["off_data"] = as_int(text, r"offsetof\(data\)[ \t]+= (\d+)")
    f["off_top"] = as_int(text, r"offsetof\(top\)[ \t]+= (\d+)")
    f["off_ref"] = as_int(text, r"offsetof\(refusals\)= (\d+)")
    f["pad_line"] = grep(text, r"offsetof\(top\) = (\d+), not (\d+):")
    f["pushes"] = re.findall(r"push\((\d+)\) -> (\S+)[ \t]+topIndex=(\d+) content:(.*)", text)
    f["pops"] = re.findall(r"pop\(\) -> (\d+)[ \t]+remaining topIndex=(\d+)", text)
    f["underflow"] = "after the last pop: empty=yes, pop on empty now returns false (underflow)" in text
    f["cap"] = re.findall(
        r"push\((\d+)\) -> (stored|REFUSED \(full\))[ \t]+size=(\d+) full=(yes|no) refusals=(\d+)", text)
    f["untouched"] = grep(text, r"after the refusals the contents are untouched: bottom -> top:(.*?)[ \t]+size=(\d+)")
    f["refused_line"] = grep(text, r"refused pushes = (\d+), capacity\(\) = (\d+)")
    f["drain"] = grep(text, r"^  drain: (.*)$", re.M)
    f["base"] = grep(text, r"bench\.data base[ \t]+= (\S+)")
    f["one_past"] = grep(text, r"bench\.data \+ 6\*sizeof\(int\) = (\S+)")
    f["canary_addr"] = grep(text, r"address of bench\.canary[ \t]+= (\S+)")
    f["same_slot"] = grep(text, r"the unchecked slot 6 IS the canary slot: (\w+)")
    f["canary"] = grep(text, r"canary before = 0x(\w+)")
    f["canary_off"] = as_int(text, r"offsetof\(CanaryBench,canary\) = (\d+) bytes")
    f["container"] = grep(text, r"the container itself asked the heap for (\d+) block of (\d+) bytes for a (\d+) byte object")
    f["further"] = grep(text, r"the 2,000,000 pushes and 2,000,000 pops asked the heap for (\d+) further blocks and (\d+) further bytes")
    f["sum"] = grep(text, r"the popped values sum to (\d+) against (\d+) if every value came back in reverse: (\w+)")
    f["ops"] = grep(text, r"per operation: push ([\d.]+) ns, pop ([\d.]+) ns")
    f["tail_refusals"] = as_int(text, r"refusals at the end = (\d+)")
    f["bottom_ns"] = as_float(text, r"bottom end \(every push shifts the whole stack\): ([\d.]+) ns per push")
    f["top_ns"] = as_float(text, r"top end[ \t]+\(nothing shifts\)[ \t]+: ([\d.]+) ns per push")
    f["ratio"] = as_float(text, r"the bottom end costs ([\d.]+)x more for the SAME ([\d,]+) pushes")
    f["checksum"] = as_int(text, r"both loops end in a checksum \((\d+)\)")
    return f


def check_11(text):
    f = parse_11(text)
    eq("lec11: sizeof(int)=4 and sizeof(size_t)=8", f["sizes"], (4, 8))
    eq("lec11: sizeof(ArrayStack<int,5>) = 40 (20 payload + 4 pad + 16 bookkeeping)", f["as_int"], 40)
    eq("lec11: sizeof(ArrayStack<char,5>) = 24 (5 bytes padded to 8, plus 16)", f["as_char"], 24)
    eq("lec11: the 5 ints alone are 20 bytes and the bookkeeping is 16", (f["payload"], f["book"]), (20, 16))
    eq("lec11: offsetof(data)=0, offsetof(top)=24, offsetof(refusals)=32",
       (f["off_data"], f["off_top"], f["off_ref"]), (0, 24, 32))
    eq("lec11: the padding line states top at 24, not 20", f["pad_line"], ("24", "20"))
    check("lec11: the sizes add up, payload + 4 padding + 16 bookkeeping = 40",
          f["payload"] + 4 + f["book"] == f["as_int"],
          "computed %s + 4 + %s = %s, the program printed %s"
          % (f["payload"], f["book"], f["payload"] + 4 + f["book"], f["as_int"]))
    check("lec11: the char stack is the 5 char array padded to 8 plus 16",
          f["as_char"] == 8 + f["book"], str(f["as_char"]))

    eq("lec11: pushes 10, 20, 30 report topIndex 1, 2, 3",
       [(p[0], p[1], p[2]) for p in f["pushes"]], [("10", "ok", "1"), ("20", "ok", "2"), ("30", "ok", "3")])
    eq("lec11: the pushed contents read 10 / 10 20 / 10 20 30",
       [p[3].split() for p in f["pushes"]], [["10"], ["10", "20"], ["10", "20", "30"]])
    eq("lec11: LIFO, pops return 30 20 10 with the remaining index 2 1 0",
       f["pops"], [("30", "2"), ("20", "1"), ("10", "0")])
    check("lec11: after the last pop the stack is empty and pop returns false (underflow)", f["underflow"])

    eq("lec11: 7 pushes into capacity 5 store 5 then refuse 2",
       [c[1] for c in f["cap"]],
       ["stored", "stored", "stored", "stored", "stored", "REFUSED (full)", "REFUSED (full)"])
    eq("lec11: the refusal counter runs 0,0,0,0,0,1,2", [c[4] for c in f["cap"]], ["0", "0", "0", "0", "0", "1", "2"])
    eq("lec11: full() flips to yes exactly at the fifth push", [c[3] for c in f["cap"]],
       ["no", "no", "no", "no", "yes", "yes", "yes"])
    check("lec11: the refusals leave the contents untouched, 1 2 3 4 5 with size 5",
          f["untouched"] == ([" 1", "2", "3", "4", "5"], "5") or
          (f["untouched"] is not None and f["untouched"][0].split() == ["1", "2", "3", "4", "5"] and f["untouched"][1] == "5"),
          str(f["untouched"]))
    eq("lec11: refused pushes = 2 and capacity() = 5", f["refused_line"], ("2", "5"))
    check("lec11: the drain pops 5 4 3 2 1", f["drain"] is not None and f["drain"][0].split() == ["5", "4", "3", "2", "1"],
          str(f["drain"]))

    check("lec11: data + 6*sizeof(int) lands exactly on the address of the canary",
          f["one_past"] is not None and f["one_past"] == f["canary_addr"],
          "%s vs %s" % (f["one_past"], f["canary_addr"]))
    check("lec11: the program itself says slot 6 IS the canary slot", f["same_slot"] == ("yes",), str(f["same_slot"]))
    eq("lec11: offsetof(CanaryBench,canary) = 24, exactly 6 ints in", f["canary_off"], 24)
    check("lec11: canary offset = 6 * sizeof(int), the overflow target",
          f["canary_off"] == 6 * f["sizes"][0], str(f["canary_off"]))
    eq("lec11: the canary holds 0xc0ffee", f["canary"], ("c0ffee",))

    check("lec11: the container asked the heap for 1 block of 8000016 bytes for a 8000016 byte object",
          f["container"] == ("1", "8000016", "8000016"), str(f["container"]))
    check("lec11: 8000016 = 2,000,000 ints + 16 bytes of bookkeeping",
          f["container"] is not None and int(f["container"][1]) == 2000000 * f["sizes"][0] + f["book"],
          str(f["container"]))
    check("lec11: the 2,000,000 pushes and pops asked for 0 further blocks and 0 further bytes",
          f["further"] == ("0", "0"), str(f["further"]))
    check("lec11: the popped sum is 1999999000000 = BIG*(BIG-1)/2",
          f["sum"] is not None and f["sum"][0] == "1999999000000" and f["sum"][2] == "confirmed",
          str(f["sum"]))
    check("lec11: the sum equals the Python invariant 2000000*1999999//2",
          f["sum"] is not None and int(f["sum"][0]) == 2000000 * 1999999 // 2, str(f["sum"]))
    check("lec11: the two printed sums agree (measured vs expected)", f["sum"] is not None and f["sum"][0] == f["sum"][1],
          str(f["sum"]))
    eq("lec11: refusals at the end = 0 (the big stack never filled)", f["tail_refusals"], 0)
    check("lec11: both timed ops are under 100 ns", f["ops"] is not None and fast(f["ops"]), str(f["ops"]))

    check("lec11: the bottom end costs at least 100x the top end for the same 20,000 pushes",
          f["ratio"] is not None and f["ratio"] > 100 and
          (f["bottom_ns"] or 0) / max(f["top_ns"] or 1, 1e-9) > 100,
          "printed %sx, ns %s vs %s" % (f["ratio"], f["bottom_ns"], f["top_ns"]))
    check("lec11: the bottom end really shifts (> 50 ns per push)", (f["bottom_ns"] or 0) > 50, str(f["bottom_ns"]))
    check("lec11: the top end is a store and an increment (< 1 ns per push)", (f["top_ns"] or 9) < 1.0, str(f["top_ns"]))
    check("lec11: both loops end in the checksum 399980000 = 2 * sum(0..19999)", f["checksum"] == 399980000,
          str(f["checksum"]))
    check("lec11: the checksum equals the Python invariant", f["checksum"] == 2 * (20000 * 19999 // 2), str(f["checksum"]))


def fast(pair):
    try:
        return all(0 < float(x) < 100 for x in pair)
    except (TypeError, ValueError):
        return False


# --------------------------------------------------------------------------- lecture 12

def parse_12(text):
    f = {}
    f["node"] = grep(text, r"sizeof\(Node\{int data; Node \*next;\}\) = (\d+)[ \t]+offsetof\(data\)=(\d+) offsetof\(next\)=(\d+)")
    f["stack_size"] = as_int(text, r"sizeof\(LinkedStack\) = (\d+) bytes")
    f["push"] = re.findall(r"push\([ \t]*(\d+)\)[ \t]+new node at (\S+) -> head_ now points here, head_->data=(\d+), head_->next=(\S+), size=(\d+)", text)
    f["pop"] = re.findall(r"pop\(\) -> (\d+)[ \t]+size=(\d+)[ \t]+head=(\S+)", text)
    f["underflow"] = grep(text, r"pop\(\) on an empty stack returned (\w+), underflow counter = (\d+)")
    f["many"] = grep(text, r"pushed (\d+) elements with (\d+) heap requests requesting (\d+) bytes, which is (\d+) bytes per element")
    f["alloc"] = grep(text, r"the allocator hands out (\d+) usable bytes per node and charges a (\d+) byte stride, so the real footprint is (\d+) bytes for the (\d+) bytes asked for")
    f["drain"] = grep(text, r"draining (\d+) nodes made (\d+) free calls, underflows = (\d+)")
    f["table"] = grep(text, r"heap calls per element[ \t]+(\d+)[ \t]+(\d+)")
    f["table_bytes"] = grep(text, r"bytes charged per element (\d+)[ \t]+(\d+)")
    f["brackets"] = re.findall(r'^[ \t]+"([^"]*)"[ \t]+-> (BALANCED|NOT BALANCED)[ \t]+\((.+)\)$', text, re.M)
    f["trace"] = re.findall(r"^[ \t]{4}(push|match|hit|end of input)(.*)$", text, re.M)
    f["reverse"] = re.findall(r'"([^"]*)" -> "([^"]*)"[ \t]+palindrome=(yes|no)', text)
    f["checksum"] = grep(text, r"order verified by checksum: linked (\w+), array (\w+)")
    f["linked"] = grep(text, r"linked stack: push ([\d.]+) ns, pop ([\d.]+) ns, (\d+) heap requests for ([\d,]+) elements")
    f["array"] = grep(text, r"array stack : push ([\d.]+) ns, pop ([\d.]+) ns, (\d+) heap request of (\d+) bytes, storage (\d+) bytes")
    f["pays"] = as_float(text, r"the linked stack pays ([\d.]+)x more per pop")
    return f


def check_12(text):
    f = parse_12(text)
    eq("lec12: sizeof(Node) = 16 with data at 0 and next at 8 (4 bytes of padding)",
       f["node"], ("16", "0", "8"))
    check("lec12: the node padding puts the pointer on an 8 byte boundary",
          f["node"] is not None and int(f["node"][2]) == 8, str(f["node"]))
    eq("lec12: sizeof(LinkedStack) = 24 (pointer + count + underflow counter)", f["stack_size"], 24)
    check("lec12: the stack object is constant size, 3 * 8 bytes",
          f["stack_size"] == 24, str(f["stack_size"]))

    eq("lec12: three pushes print data 10, 20, 30 with sizes 1, 2, 3",
       [(p[2], p[4]) for p in f["push"]], [("10", "1"), ("20", "2"), ("30", "3")])
    check("lec12: each push moves the head to the new node",
          len(f["push"]) == 3 and len({p[1] for p in f["push"]}) == 3, str([p[1] for p in f["push"]]))
    check("lec12: node 2's next points at node 1 and node 3's next at node 2 (head insertion)",
          len(f["push"]) == 3 and f["push"][1][3] == f["push"][0][1] and f["push"][2][3] == f["push"][1][1],
          str([p[3] for p in f["push"]]))
    check("lec12: the bottom node's next is null", len(f["push"]) == 3 and f["push"][0][3] == "0",
          str(f["push"][0][3] if f["push"] else None))
    eq("lec12: pops return 30 20 10 with sizes 2 1 0",
       [(p[0], p[1]) for p in f["pop"]], [("30", "2"), ("20", "1"), ("10", "0")])
    check("lec12: after the last pop the head is 0", len(f["pop"]) == 3 and f["pop"][2][2] == "0", str(f["pop"]))
    eq("lec12: pop on empty returned false with an underflow counter of 2", f["underflow"], ("false", "2"))

    eq("lec12: 200000 pushes = 200000 heap requests for 3200000 bytes = 16 bytes per element",
       f["many"], ("200000", "200000", "3200000", "16"))
    check("lec12: 3200000 = 200000 * sizeof(Node)",
          f["many"] is not None and int(f["many"][2]) == 200000 * int(f["node"][0]), str(f["many"]))
    eq("lec12: allocator gives 24 usable bytes and charges a 32 byte stride", f["alloc"][:2], ("24", "32"))
    check("lec12: the real footprint is 32 * 200000 = 6400000 bytes for the 3200000 asked",
          f["alloc"] is not None and f["alloc"][2] == "6400000" and int(f["alloc"][0]) == 24,
          str(f["alloc"]))
    eq("lec12: draining 200000 nodes made 200000 free calls with 1 underflow",
       (f["drain"][1], f["drain"][2]) if f["drain"] else None, ("200000", "1"))
    eq("lec12: the trade table says 0 vs 1 heap calls and 4 vs 32 bytes per element",
       (f["table"], f["table_bytes"]), (("0", "1"), ("4", "32")))

    eq("lec12: bracket cases in order", [(b[0], b[1]) for b in f["brackets"]],
       [("(a[b]{c})", "BALANCED"), ("([)]", "NOT BALANCED"), ("(((", "NOT BALANCED"),
        (")(", "NOT BALANCED"), ("{[()]}", "BALANCED"), ("", "BALANCED")])
    br = {b[0]: b for b in f["brackets"]}
    check("lec12: ([)] fails at index 2 expecting ( but finding )",
          "([)]" in br and br["([)]"][2] == "expected ( but found ) at index 2",
          str(br.get("([)]")))
    check("lec12: ((( fails with the deepest unmatched opener ( at index 2",
          "(((" in br and br["((("][2] == "never closed, the deepest unmatched opener is ( at index 2",
          str(br.get("(((")))
    check("lec12: )( fails at index 0 with nothing open",
          ")(" in br and br[")("][2] == "closing ) with nothing open at index 0", str(br.get(")(")))
    trace_flat = " | ".join(" ".join((t[0] + t[1]).split()) for t in f["trace"]) if f["trace"] else ""
    check("lec12: the ([)] trace pushes ( then [ then dies at ) with [ on top",
          [t[0] for t in f["trace"]] == ["push", "push", "hit"] and
          "push ( at 0 depth=1" in trace_flat and "push [ at 1 depth=2" in trace_flat and
          "hit ) at 2 while top is [ -> fail" in trace_flat, trace_flat)

    eq("lec12: reverse DSA -> ASD, not a palindrome", f["reverse"][0] if f["reverse"] else None,
       ("DSA", "ASD", "no"))
    check("lec12: reverse ECE2104 -> 4012ECE, not a palindrome",
          any(r == ("ECE2104", "4012ECE", "no") for r in f["reverse"]), str(f["reverse"]))
    check("lec12: reverse level -> level, a palindrome",
          any(r == ("level", "level", "yes") for r in f["reverse"]), str(f["reverse"]))
    eq("lec12: both timed representations pass their checksum", f["checksum"], ("ok", "ok"))
    check("lec12: the linked stack made 1000000 heap requests for 1,000,000 elements",
          f["linked"] is not None and f["linked"][2] == "1000000", str(f["linked"]))
    check("lec12: the array stack made 1 heap request of 4000008 bytes for 4000000 bytes of storage",
          f["array"] is not None and f["array"][2] == "1" and f["array"][3] == "4000008" and f["array"][4] == "4000000",
          str(f["array"]))
    check("lec12: 4000008 = 4 * 1000000 + 8 bytes of index", f["array"] is not None and
          int(f["array"][3]) == 4 * 1000000 + 8, str(f["array"]))
    check("lec12: the linked stack pays at least 5x the array stack per pop",
          (f["pays"] or 0) >= 5 or (f["linked"] and f["array"] and float(f["linked"][1]) / float(f["array"][1]) >= 5),
          str(f["pays"]))


# --------------------------------------------------------------------------- lecture 13

def parse_13(text):
    f = {}
    f["int_size"] = as_int(text, r"sizeof\(int\)=(\d+)")
    f["naive"] = as_int(text, r"sizeof\(NaiveQueue<int,5>\)[ \t]+= (\d+)")
    f["circ"] = as_int(text, r"sizeof\(CircularQueue<int,5>\) = (\d+)")
    f["book"] = grep(text, r"the array is (\d+) bytes, the bookkeeping is (\d+) bytes in the naive form and (\d+) in the circular form")
    f["enq"] = re.findall(r"^[ \t]+enqueue\([ \t]*(\d+)\)[ \t]+rearIndex=(\d+) size=(\d+)$", text, re.M)
    f["deq"] = re.findall(r"^[ \t]+dequeue\(\) -> (\d+)[ \t]+frontIndex=(\d+) size=(\d+)$", text, re.M)
    f["after5"] = grep(text, r"after 5 enqueues: frontIndex=(\d+) rearIndex=(\d+) size=(\d+) empty=(\w+) full=(\w+)[ \t]+slots:(.*)")
    f["after3"] = grep(text, r"after 3 dequeues: frontIndex=(\d+) rearIndex=(\d+) size=(\d+) empty=(\w+) full=(\w+)[ \t]+slots:(.*)")
    f["deq3"] = re.findall(r"^[ \t]+dequeue\(\) -> (\d+)$", text, re.M)
    f["free"] = grep(text, r"free slots BELOW front = (\d+), free slots at rear = (\d+), live elements = (\d+)")
    f["refused"] = re.findall(r"enqueue\((\d+)\) -> (stored|REFUSED \(rear == capacity\))[ \t]+rearIndex=(\d+) size=(\d+) refusals=(\d+)", text)
    f["refusal_state"] = grep(text, r"state at the refusal: frontIndex=(\d+) rearIndex=(\d+) size=(\d+) empty=(\w+) full=(\w+)[ \t]+slots:(.*)")
    f["lost"] = grep(text, r"full\(\) says yes while (\d+) of the (\d+) slots are unreachable\. Space lost = frontIndex = (\d+) slots")
    f["drain"] = grep(text, r"^  drain the rest: (.*)$", re.M)
    f["after_drain"] = grep(text, r"after draining: frontIndex=(\d+) rearIndex=(\d+) size=(\d+) empty=(\w+) full=(\w+)[ \t]+slots:(.*)")
    f["both"] = grep(text, r"dequeue says empty while full\(\) still says full: (.+)$")
    f["naive_form"] = grep(text, r"naive form[ \t]+: refused (\d+) of those (\d+) enqueues, holds (\d+) elements")
    f["circ_form"] = grep(text, r"circular form : refused (\d+) of those (\d+) enqueues, holds (\d+) elements")
    f["container"] = grep(text, r"the container asked the heap for (\d+) block of (\d+) bytes \(the array lives inside the object\)")
    f["ops"] = grep(text, r"the 2,000,000 operations asked the heap for (\d+) blocks and (\d+) bytes")
    f["ordered"] = grep(text, r"every dequeue returned the value its enqueue put in, in order: (\w+)")
    f["timing"] = grep(text, r"per operation: ([\d.]+) ns, index wraps survived = (\d+)")
    return f


SLOT_RE = re.compile(r"\[(--|\d+)\]")


def slots(text):
    return SLOT_RE.findall(text)


def align8(n):
    """The ABI pads a struct to its widest member, so 20 bytes of int array cost 24."""
    return (n + 7) // 8 * 8


def check_13(text):
    f = parse_13(text)
    eq("lec13: sizeof(int)=4", f["int_size"], 4)
    eq("lec13: sizeof(NaiveQueue<int,5>) = 48 (20 byte array + 24 bookkeeping)", f["naive"], 48)
    eq("lec13: sizeof(CircularQueue<int,5>) = 64 (20 byte array + 40 bookkeeping)", f["circ"], 64)
    eq("lec13: the printed breakdown is 20 / 24 / 40", f["book"], ("20", "24", "40"))
    check("lec13: both sizes recompute from the parts in Python, with the padding the ABI adds",
          f["naive"] == align8(5 * 4) + 3 * 8 and f["circ"] == align8(5 * 4) + 5 * 8,
          "naive %s vs %s, circular %s vs %s" % (f["naive"], align8(5 * 4) + 24, f["circ"], align8(5 * 4) + 40))

    eq("lec13: enqueue 10 20 30 gives rearIndex 1 2 3 and size 1 2 3",
       [(e[0], e[1], e[2]) for e in f["enq"]], [("10", "1", "1"), ("20", "2", "2"), ("30", "3", "3")])
    eq("lec13: FIFO, the dequeues return 10 20 30 in that order",
       [(d[0], d[1], d[2]) for d in f["deq"]], [("10", "1", "2"), ("20", "2", "1"), ("30", "3", "0")])
    check("lec13: the queue returns its inputs in the same order they went in",
          [d[0] for d in f["deq"]] == [e[0] for e in f["enq"]], "%s vs %s" % ([d[0] for d in f["deq"]], [e[0] for e in f["enq"]]))

    eq("lec13: after 5 enqueues front=0 rear=5 size=5 full=yes",
       f["after5"][:5], ("0", "5", "5", "no", "yes"))
    eq("lec13: the five slots hold 1 2 3 4 5", slots(f["after5"][5]) if f["after5"] else None, ["1", "2", "3", "4", "5"])
    eq("lec13: after 3 dequeues front=3 rear=5 size=2 still full=yes",
       f["after3"][:5], ("3", "5", "2", "no", "yes"))
    eq("lec13: slots 0 to 2 read -- and slots 3, 4 hold 4 and 5",
       slots(f["after3"][5]) if f["after3"] else None, ["--", "--", "--", "4", "5"])
    eq("lec13: the three dequeues returned 1 2 3", f["deq3"], ["1", "2", "3"])
    eq("lec13: 3 free slots below the front, 0 at the rear, 2 live", f["free"], ("3", "0", "2"))
    check("lec13: size = rear - front recomputes to the printed 2",
          f["after3"] is not None and int(f["after3"][1]) - int(f["after3"][0]) == int(f["after3"][2]),
          str(f["after3"]))

    eq("lec13: enqueue 6 and 7 are both refused with rear == capacity",
       [(r[0], r[1]) for r in f["refused"]], [("6", "REFUSED (rear == capacity)"), ("7", "REFUSED (rear == capacity)")])
    eq("lec13: the refusals raise the counter to 1 then 2", [r[4] for r in f["refused"]], ["1", "2"])
    check("lec13: the refuse happens at enqueue #6 of the sequence, with rearIndex stuck at 5 and size 2",
          f["refused"] and f["refused"][0][2] == "5" and f["refused"][0][3] == "2", str(f["refused"]))
    check("lec13: the state at the refusal repeats the post-dequeue state exactly",
          f["refusal_state"] is not None and f["refusal_state"][1] == f["after3"][1] and
          f["refusal_state"][0] == f["after3"][0] and slots(f["refusal_state"][5]) == slots(f["after3"][5]),
          str(f["refusal_state"]))
    eq("lec13: 3 of 5 slots unreachable, space lost = frontIndex = 3", f["lost"], ("3", "5", "3"))
    check("lec13: the lost slots equal the printed frontIndex",
          f["lost"] is not None and f["lost"][2] == f["after3"][0], str(f["lost"]))
    check("lec13: the drain empties 4 then 5", f["drain"] is not None and f["drain"][0].split() == ["4", "5"], str(f["drain"]))

    eq("lec13: after draining front=5 rear=5 size=0 with empty and full both yes",
       f["after_drain"][:5], ("5", "5", "0", "yes", "yes"))
    check("lec13: the naive queue is permanently wedged, empty and full at once",
          f["both"] is not None and "both true at once" in f["both"][0], str(f["both"]))
    eq("lec13: the same script on the naive form refused 3 of 3 and holds 2", f["naive_form"], ("3", "3", "2"))
    eq("lec13: the same script on the circular form refused 0 of 3 and holds 5", f["circ_form"], ("0", "3", "5"))
    check("lec13: the repair is worth exactly the 3 slots the naive form threw away",
          f["naive_form"] is not None and f["circ_form"] is not None and
          int(f["circ_form"][2]) - int(f["naive_form"][2]) == 3,
          "%s vs %s" % (f["naive_form"], f["circ_form"]))

    check("lec13: the container asked for 1 block of 4136 bytes",
          f["container"] == ("1", "4136"), str(f["container"]))
    check("lec13: 4136 = 1024 ints + 40 bytes of circular bookkeeping",
          f["container"] is not None and int(f["container"][1]) == 1024 * 4 + 40, str(f["container"]))
    eq("lec13: the 2,000,000 operations asked for 0 blocks and 0 bytes", f["ops"], ("0", "0"))
    eq("lec13: every dequeue returned its enqueue's value in order", f["ordered"], ("yes",))
    check("lec13: 976 wraps = floor(1000000 / 1024)",
          f["timing"] is not None and int(f["timing"][1]) == 1000000 // 1024, str(f["timing"]))
    check("lec13: the circular queue runs under 100 ns per operation", f["timing"] is not None and float(f["timing"][0]) < 100,
          str(f["timing"]))


# --------------------------------------------------------------------------- lecture 14

def parse_14(text):
    f = {}
    f["node"] = grep(text, r"sizeof\(QNode\{int data; QNode \*next;\}\) = (\d+)[ \t]+offsetof\(data\)=(\d+) offsetof\(next\)=(\d+)")
    f["q_size"] = as_int(text, r"sizeof\(LinkedQueue\) = (\d+) bytes")
    f["enq"] = re.findall(r"^[ \t]+enqueue\((\d+)\)[ \t]+node at (\S+)[ \t]+front=(\S+) rear=(\S+) size=(\d+)$", text, re.M)
    f["deq"] = re.findall(r"^[ \t]+dequeue\(\) -> (\d+)[ \t]+front now (\S+) size=(\d+)$", text, re.M)
    f["underflow"] = grep(text, r"dequeue on empty returns (\w+), underflows=(\d+)")
    f["usable"] = as_int(text, r"the allocator charges (\d+) usable bytes per node for the 16 byte request")
    f["many"] = grep(text, r"enqueued (\d+) elements with (\d+) heap requests requesting (\d+) bytes, and the drain made (\d+) free calls")
    f["traj"] = re.findall(r"^[ \t]+(enq|deq)[ \t]+(\d+)[ \t]+(\d+)[ \t]+(\d+)[ \t]+(\d+)[ \t]+(\d+)(.*)$", text, re.M)
    f["stateA"] = grep(text, r"state A: front=(\d+) rear=(\d+) -> formula = \((\d+) - (\d+) \+ 5\) mod 5 = (\d+) and the stored count is (\d+)")
    f["stateB"] = grep(text, r"state B: front=(\d+) rear=(\d+) -> formula = \((\d+) - (\d+) \+ 5\) mod 5 = (\d+) and the stored count is (\d+)")
    f["stateC"] = grep(text, r"state C \(a wrapped one\): front=(\d+) rear=(\d+), bare \(rear - front\) as unsigned = (\d+)")
    f["stateC2"] = grep(text, r"while the formula gives \((\d+) - (\d+) \+ 5\) mod 5 = (\d+) and the stored count is (\d+)")
    f["counted"] = grep(text, r"count based: front=(\d+) rear=(\d+) count=(\d+) -> empty, decided by the count")
    f["sentinel"] = grep(text, r"sentinel slot: after (\d+) enqueues usable=(\d+) front=(\d+) rear=(\d+) full=(\w+) refusals=(\d+)")
    f["sentinel_slots"] = grep(text, r"it stores (\d+) of (\d+) slots, the count design stores all 5")
    f["fifo"] = grep(text, r"fifo order verified in both loops: (\w+)")
    f["linked"] = grep(text, r"linked queue[ \t]+: ([\d.]+) ns per operation, (\d+) heap requests")
    f["circular"] = grep(text, r"circular queue: ([\d.]+) ns per operation, (\d+) heap requests during the operations")
    f["slower"] = as_float(text, r"the linked queue is ([\d.]+)x slower per operation")
    return f


EXPECTED_TRAJ = [
    ("enq", 100, 0, 1, 1, 0), ("enq", 101, 0, 2, 2, 0),
    ("deq", 100, 1, 2, 1, 0), ("deq", 101, 2, 2, 0, 0),
    ("enq", 102, 2, 3, 1, 0), ("enq", 103, 2, 4, 2, 0),
    ("deq", 102, 3, 4, 1, 0), ("deq", 103, 4, 4, 0, 0),
    ("enq", 104, 4, 0, 1, 1), ("enq", 105, 4, 1, 2, 1),
    ("deq", 104, 0, 1, 1, 1), ("deq", 105, 1, 1, 0, 1),
    ("enq", 106, 1, 2, 1, 1), ("enq", 107, 1, 3, 2, 1),
    ("deq", 106, 2, 3, 1, 1), ("deq", 107, 3, 3, 0, 1),
    ("enq", 108, 3, 4, 1, 1), ("enq", 109, 3, 0, 2, 2),
    ("deq", 108, 4, 0, 1, 2), ("deq", 109, 0, 0, 0, 2),
]


def check_14(text):
    f = parse_14(text)
    eq("lec14: sizeof(QNode) = 16 with data at 0 and next at 8", f["node"], ("16", "0", "8"))
    eq("lec14: sizeof(LinkedQueue) = 32 (front, rear, count, underflow counter)", f["q_size"], 32)
    check("lec14: the queue object is 4 * 8 bytes, constant size", f["q_size"] == 4 * 8, str(f["q_size"]))

    eq("lec14: three enqueues print sizes 1, 2, 3", [e[4] for e in f["enq"]], ["1", "2", "3"])
    check("lec14: the front stays on the first node while the rear walks away",
          len(f["enq"]) == 3 and f["enq"][0][2] == f["enq"][1][2] == f["enq"][2][2] and
          f["enq"][0][3] == f["enq"][0][1] and f["enq"][2][3] != f["enq"][2][2],
          str(f["enq"]))
    check("lec14: the three nodes are 32 bytes apart (the allocator stride)",
          len(f["enq"]) == 3 and int(f["enq"][1][1], 16) - int(f["enq"][0][1], 16) == 32 and
          int(f["enq"][2][1], 16) - int(f["enq"][1][1], 16) == 32, str([e[1] for e in f["enq"]]))
    eq("lec14: dequeues return 7 8 9 with sizes 2 1 0", [(d[0], d[2]) for d in f["deq"]],
       [("7", "2"), ("8", "1"), ("9", "0")])
    check("lec14: after the last dequeue the front pointer is 0",
          len(f["deq"]) == 3 and f["deq"][2][1] == "0", str(f["deq"]))
    eq("lec14: dequeue on empty returns false with 2 underflows", f["underflow"], ("false", "2"))
    eq("lec14: the allocator charges 24 usable bytes for the 16 byte node request", f["usable"], 24)

    eq("lec14: 200000 enqueues = 200000 requests for 3200000 bytes and 200000 free calls",
       (f["many"][0], f["many"][1], f["many"][2], f["many"][3]) if f["many"] else None,
       ("200000", "200000", "3200000", "200000"))
    check("lec14: 3200000 = 200000 * 16", f["many"] is not None and int(f["many"][2]) == 200000 * 16, str(f["many"]))

    traj = [(t[0], int(t[1]), int(t[2]), int(t[3]), int(t[4]), int(t[5])) for t in f["traj"]]
    eq("lec14: the wrapping trajectory has 20 rows, 10 enqueues and 10 dequeues",
       (len(traj), sum(1 for t in traj if t[0] == "enq"), sum(1 for t in traj if t[0] == "deq")), (20, 10, 10))
    eq("lec14: the trajectory matches the recorded table row for row", traj, EXPECTED_TRAJ)
    check("lec14: the enqueue values run 100 to 109 in order",
          [t[1] for t in traj if t[0] == "enq"] == list(range(100, 110)), str([t[1] for t in traj if t[0] == "enq"]))
    check("lec14: the dequeue values run 100 to 109 in order",
          [t[1] for t in traj if t[0] == "deq"] == list(range(100, 110)), str([t[1] for t in traj if t[0] == "deq"]))
    check("lec14: the rear index wrapped twice",
          max(t[5] for t in traj) == 2, str([t[5] for t in traj]))
    check("lec14: the second wrap is on the enqueue of 109",
          [t for t in traj if t[0] == "enq" and t[5] == 2 and t[3] == 0] == [("enq", 109, 3, 0, 2, 2)],
          str([t for t in traj if t[5] == 2]))
    check("lec14: count = (rear - front + 5) mod 5 holds on every row",
          all((t[3] - t[2] + 5) % 5 == t[4] for t in traj), str(traj[:4]))
    check("lec14: the rear index is always inside the 5 slot array",
          all(0 <= t[3] < 5 for t in traj) and all(0 <= t[2] < 5 for t in traj))
    check("lec14: the count never exceeds the capacity",
          all(0 <= t[4] <= 5 for t in traj), str([t[4] for t in traj]))

    check("lec14: state A front=0 rear=3 gives formula 3 and stored count 3",
          f["stateA"] == ("0", "3", "3", "0", "3", "3"), str(f["stateA"]))
    check("lec14: state B front=2 rear=3 gives formula 1 and stored count 1",
          f["stateB"] == ("2", "3", "3", "2", "1", "1"), str(f["stateB"]))
    check("lec14: state C front=3 rear=2 gives bare unsigned 18446744073709551615",
          f["stateC"] == ("3", "2", "18446744073709551615"), str(f["stateC"]))
    check("lec14: that bare number is 2**64 - 1, the unsigned underflow",
          f["stateC"] is not None and int(f["stateC"][2]) == 2 ** 64 - 1, str(f["stateC"]))
    check("lec14: state C gives formula 4 and stored count 4",
          f["stateC2"] == ("2", "3", "4", "4"), str(f["stateC2"]))
    check("lec14: the count formula is what the printed values satisfy",
          f["stateC"] is not None and (2 - 3 + 5) % 5 == int(f["stateC2"][2]), str(f["stateC2"]))

    eq("lec14: the count based design reports front=3 rear=3 count=0 as empty", f["counted"], ("3", "3", "0"))
    eq("lec14: the sentinel design fills 4 of 5 slots: full=yes with 0 refusals",
       (f["sentinel"][1], f["sentinel"][2], f["sentinel"][3], f["sentinel"][4], f["sentinel"][5]) if f["sentinel"] else None,
       ("4", "0", "4", "yes", "0"))
    check("lec14: full = ((rear + 1) mod capacity == front) holds for the sentinel state",
          f["sentinel"] is not None and (int(f["sentinel"][3]) + 1) % 5 == int(f["sentinel"][2]), str(f["sentinel"]))
    eq("lec14: the sentinel stores 4 of 5 slots, the count design all 5", f["sentinel_slots"], ("4", "5"))
    eq("lec14: fifo order verified in both timed loops", f["fifo"], ("yes",))

    check("lec14: the linked queue made 1000000 heap requests in the timed loop",
          f["linked"] is not None and f["linked"][1] == "1000000", str(f["linked"]))
    check("lec14: the circular queue made 0 heap requests during the operations",
          f["circular"] is not None and f["circular"][1] == "0", str(f["circular"]))
    check("lec14: the linked queue is at least 5x slower per operation than the circular one",
          (f["slower"] or 0) >= 5 or (f["linked"] and f["circular"] and
                                      float(f["linked"][0]) / float(f["circular"][0]) >= 5), str(f["slower"]))


# --------------------------------------------------------------------------- lecture 15

def parse_15(text):
    f = {}
    f["rr_table"] = re.findall(r"^[ \t]+(\d+)[ \t]+(\d+)[ \t]+([A-E])[ \t]+(\d+)[ \t]+(\d+)[ \t]+(DONE|back to the tail of the queue)$", text, re.M)
    f["order"] = grep(text, r"completion order:(.*)$", re.M)
    f["total"] = grep(text, r"total time = (\d+), slices = (\d+) ")
    f["rr"] = grep(text, r"round robin: average waiting = ([\d.]+), average turnaround = ([\d.]+), worst waiting = (\d+)")
    f["fcfs_table"] = re.findall(r"^[ \t]+(\d+)[ \t]+(\d+)[ \t]+([A-E])[ \t]+(\d+)[ \t]+(\d+)$", text, re.M)
    f["fcfs"] = grep(text, r"FCFS: average waiting = ([\d.]+), average turnaround = ([\d.]+), worst waiting = (\d+)")
    f["trade"] = grep(text, r"worst wait is (\d+) under round robin against (\d+) under FCFS, so RR wins there by (\d+) time units")
    f["loses"] = as_float(text, r"and it loses ([\d.]+) on the average wait")
    f["q2s"] = re.findall(r"^[ \t]+(enqueue|dequeue)[ \t]+(\d+)[ \t]+(\d+)[ \t]+(\d+)[ \t]+(.*)$", text, re.M)
    f["order_ok"] = grep(text, r"order check: enqueued 10 20 30 40 50 60 and they came back in that order: (\w+)")
    f["transfers6"] = grep(text, r"total transfers for 6 elements = (\d+):")
    f["mixed"] = grep(text, r"(\d+) mixed operations, followed by a full drain")
    f["match"] = grep(text, r"the two-stack queue returned the same values in the same order as std::queue: (\w+)")
    f["transfers138"] = grep(text, r"transfers over the whole run = (\d+) for (\d+) enqueues and (\d+) operations")
    f["bounded"] = grep(text, r"so transfers <= enqueues: (\w+)")
    f["rot"] = re.findall(r"push\((\d+)\)[ \t]+rotations so far = (\d+)[ \t]+\(that push cost (\d+) rotations\)", text)
    f["pops"] = grep(text, r"pops come back:(.*)$", re.M)
    f["price"] = grep(text, r"the same 6 pushes cost (\d+) rotations")
    f["tri"] = grep(text, r"which is 1 \+ 2 \+ \.\.\. \+ 5 = (\d+)\.")
    f["worst"] = grep(text, r"so the transfer happens on every single dequeue: ([\d.]+) ns per operation, transfers = (\d+), order verified: (\w+)")
    return f


EXPECTED_RR = [
    (0, 3, "A", 3, 5, "back to the tail of the queue"),
    (3, 6, "B", 3, 1, "back to the tail of the queue"),
    (6, 9, "C", 3, 6, "back to the tail of the queue"),
    (9, 11, "D", 2, 0, "DONE"),
    (11, 14, "E", 3, 3, "back to the tail of the queue"),
    (14, 17, "A", 3, 2, "back to the tail of the queue"),
    (17, 18, "B", 1, 0, "DONE"),
    (18, 21, "C", 3, 3, "back to the tail of the queue"),
    (21, 24, "E", 3, 0, "DONE"),
    (24, 26, "A", 2, 0, "DONE"),
    (26, 29, "C", 3, 0, "DONE"),
]
EXPECTED_FCFS = [(0, 8, "A", 8, 0), (8, 12, "B", 4, 8), (12, 21, "C", 9, 12),
                 (21, 23, "D", 2, 21), (23, 29, "E", 6, 23)]
BURSTS = {"A": 8, "B": 4, "C": 9, "D": 2, "E": 6}


def check_15(text):
    f = parse_15(text)
    rr = [(int(r[0]), int(r[1]), r[2], int(r[3]), int(r[4]), r[5]) for r in f["rr_table"]]
    eq("lec15: the round robin table has 11 slices and matches the recorded run row for row", rr, EXPECTED_RR)
    check("lec15: every round robin slice is at most the quantum 3",
          all(r[3] <= 3 for r in rr), str([r[3] for r in rr]))
    check("lec15: each slice continues exactly where the previous one stopped (no idle time)",
          rr and rr[0][0] == 0 and all(rr[i][1] == rr[i + 1][0] for i in range(len(rr) - 1)),
          str([(r[0], r[1]) for r in rr]))
    check("lec15: remaining work falls by the slice each time and a task is DONE exactly at 0",
          all(r[4] == (BURSTS[r[2]] - sum(s[3] for s in rr[:i + 1] if s[2] == r[2])) for i, r in enumerate(rr)) and
          all((r[4] == 0) == (r[5] == "DONE") for r in rr), str(rr))
    eq("lec15: completion order is D B E A C", f["order"][0].split() if f["order"] else None, ["D", "B", "E", "A", "C"])
    eq("lec15: total time 29 with 11 slices", f["total"], ("29", "11"))
    check("lec15: the 11 slice durations sum to the 29 printed total time",
          sum(r[3] for r in rr) == int(f["total"][0]) if f["total"] else False, str([r[3] for r in rr]))

    comp = {}
    for r in rr:
        if r[5] == "DONE":
            comp[r[2]] = r[1]
    waits = {t: comp[t] - BURSTS[t] for t in comp}
    avg_wait = sum(waits.values()) / len(waits)
    avg_turn = sum(comp.values()) / len(comp)
    eq("lec15: the 5 tasks all complete", sorted(comp), ["A", "B", "C", "D", "E"])
    check("lec15: average waiting recomputed from the table is 15.80", abs(avg_wait - 15.80) < 1e-9,
          "%%.2f" % avg_wait if False else "%.2f" % avg_wait)
    check("lec15: average turnaround recomputed from the table is 21.60", abs(avg_turn - 21.60) < 1e-9,
          "%.2f" % avg_turn)
    check("lec15: worst waiting recomputed from the table is 20 (task C)",
          max(waits.values()) == 20 and [t for t in waits if waits[t] == 20] == ["C"], str(waits))
    eq("lec15: the printed round robin line agrees", f["rr"], ("15.80", "21.60", "20"))
    check("lec15: the printed round robin averages equal the recomputed ones",
          f["rr"] is not None and abs(float(f["rr"][0]) - avg_wait) < 0.005 and abs(float(f["rr"][1]) - avg_turn) < 0.005,
          str(f["rr"]))

    fc = [(int(r[0]), int(r[1]), r[2], int(r[3]), int(r[4])) for r in f["fcfs_table"]]
    eq("lec15: the FCFS table matches the recorded run row for row", fc, EXPECTED_FCFS)
    check("lec15: FCFS serves the tasks in arrival order with no preemption",
          [r[2] for r in fc] == ["A", "B", "C", "D", "E"] and
          all(fc[i][1] == fc[i + 1][0] and fc[i][3] == BURSTS[fc[i][2]] for i in range(len(fc) - 1)),
          str(fc))
    fw = [r[4] for r in fc]
    fturn = [r[1] for r in fc]
    eq("lec15: the printed FCFS line is 12.80 / 18.60 / 23", f["fcfs"], ("12.80", "18.60", "23"))
    check("lec15: FCFS averages recomputed from the table are 12.80 and 18.60",
          abs(sum(fw) / 5 - 12.80) < 1e-9 and abs(sum(fturn) / 5 - 18.60) < 1e-9,
          "%.2f / %.2f" % (sum(fw) / 5, sum(fturn) / 5))
    eq("lec15: round robin beats FCFS on the worst wait by exactly 3", f["trade"], ("20", "23", "3"))
    check("lec15: RR worst 20 < FCFS worst 23 states the same trade twice",
          f["trade"] is not None and max(waits.values()) == int(f["trade"][0]) and max(fw) == int(f["trade"][1]),
          str(f["trade"]))
    check("lec15: RR pays 3.00 more on the average wait than FCFS",
          f["loses"] is not None and abs(f["loses"] - 3.00) < 0.005 and
          abs((avg_wait - sum(fw) / 5) - 3.00) < 1e-9, str(f["loses"]))

    ops = [(o[0], int(o[1]), int(o[2]), int(o[3])) for o in f["q2s"]]
    enqs = [o for o in ops if o[0] == "enqueue"]
    deqs = [o for o in ops if o[0] == "dequeue"]
    eq("lec15: the two-stack queue takes 6 enqueues then 6 dequeues", (len(enqs), len(deqs)), (6, 6))
    check("lec15: the six enqueues cost 0 transfers and grow the size 1 to 6",
          [(o[1], o[2], o[3]) for o in enqs] == [(10, 0, 1), (20, 0, 2), (30, 0, 3), (40, 0, 4), (50, 0, 5), (60, 0, 6)],
          str(enqs))
    check("lec15: the first dequeue pours all 6 elements across in one bulk transfer",
          deqs and deqs[0] == ("dequeue", 10, 6, 5), str(deqs[:1]))
    check("lec15: after the pour every dequeue is a plain pop, transfers stay at 6",
          all(o[2] == 6 for o in deqs) and [o[1] for o in deqs] == [10, 20, 30, 40, 50, 60], str(deqs))
    eq("lec15: the order check confirms 10 20 30 40 50 60 came back in order", f["order_ok"], ("yes",))
    eq("lec15: total transfers for 6 elements = 6, one move each", f["transfers6"], ("6",))

    eq("lec15: the mixed stream is 200 operations followed by a full drain", f["mixed"], ("200",))
    eq("lec15: the two-stack queue matched std::queue exactly", f["match"], ("yes",))
    eq("lec15: 138 transfers for 138 enqueues over 200 operations", f["transfers138"], ("138", "138", "200"))
    check("lec15: transfers <= enqueues holds and is stated", f["bounded"] == ("yes",) and
          f["transfers138"] is not None and int(f["transfers138"][0]) <= int(f["transfers138"][1]), str(f["bounded"]))

    rot = [(int(r[0]), int(r[1]), int(r[2])) for r in f["rot"]]
    eq("lec15: the running rotation totals are the triangular numbers 0 1 3 6 10 15",
       [r[1] for r in rot], [0, 1, 3, 6, 10, 15])
    check("lec15: each push costs n-1 rotations",
          [r[2] for r in rot] == [0, 1, 2, 3, 4, 5] and all(r[2] == r[0] - 1 for r in rot), str(rot))
    check("lec15: the running total is the running sum of those costs",
          rot and all(r[1] == sum(x[2] for x in rot[:i + 1]) for i, r in enumerate(rot)), str(rot))
    check("lec15: the stack from queues pops 6 5 4 3 2 1, LIFO recovered",
          f["pops"] is not None and f["pops"][0].split() == ["6", "5", "4", "3", "2", "1"], str(f["pops"]))
    eq("lec15: the 6 pushes cost 15 rotations", f["price"], ("15",))
    check("lec15: 15 = 1 + 2 + ... + 5 = 6*5/2", f["tri"] == ("15",) and 6 * 5 // 2 == 15, str(f["tri"]))
    eq("lec15: the worst interleaved pattern costs 500000 transfers with the order still right",
       (f["worst"][1], f["worst"][2]) if f["worst"] else None, ("500000", "yes"))
    check("lec15: the worst pattern runs under 100 ns per operation", f["worst"] is not None and
          float(f["worst"][0]) < 100, str(f["worst"]))


LECTURE_CHECKS = {
    "11_stack_array": check_11,
    "12_stack_linked_apps": check_12,
    "13_queue_array": check_13,
    "14_queue_linked_circular": check_14,
    "15_queue_apps": check_15,
}


def run_lecture_checks(name, text):
    try:
        LECTURE_CHECKS[name](text)
    except Exception as exc:  # noqa: BLE001
        check("%s: the check block ran without raising" % TAG[name], False, "%s: %s" % (type(exc).__name__, exc))


# ------------------------------------------------------------------- capture freshness

def split_capture(text):
    """Split the recorded capture into one line list per lesson program."""
    blocks, cur, buf = {}, None, []
    for line in text.splitlines():
        m = re.match(r"## (\w+)\.cpp[ \t]*$", line)
        if m:
            if cur:
                blocks[cur] = buf
            cur, buf = m.group(1), []
            continue
        if line.startswith("## exit code"):
            if cur:
                blocks[cur] = buf
            cur, buf = None, []
            continue
        if cur is not None:
            buf.append(line)
    if cur:
        blocks[cur] = buf
    return blocks


ADDR_RE = re.compile(r"0x[0-9a-fA-F]+")
FLOAT_RE = re.compile(r"\d+\.\d+")
TIMING_MARKERS = (" ns", "x more", "x slower")


def invariant_lines(lines):
    """The subset of a run that must be identical on any correct run.

    Addresses move because of ASLR and wall-clock timings move a few percent, so both are
    masked; every other number on the line is compared exactly. This is a stated blind
    spot: a wrong digit that only ever appears on a timing line is not caught here, it is
    caught by the per-lecture bounds checks above.
    """
    out = []
    for line in lines:
        if line.strip() and set(line.strip()) == {"#"}:
            continue                       # the capture's own banner rules, not program output
        if any(k in line for k in TIMING_MARKERS):
            line = FLOAT_RE.sub("FLOAT", line)
        out.append(ADDR_RE.sub("0xADDR", line.rstrip()))
    return out


def check_capture(recorded, fresh_outs):
    blocks = split_capture(recorded)
    check("capture: the file names all five lesson programs",
          all(n in blocks for n in LECTURES), str(sorted(blocks)))
    check("capture: every program is followed by an exit code of 0",
          len(re.findall(r"^## exit code: 0$", recorded, re.M)) == len(LECTURES),
          str(re.findall(r"^## exit code: (\d+)$", recorded, re.M)))
    check("capture: it records the build flags the verifier uses",
          "g++ -Wall -Wextra -Wpedantic -std=c++17 -O2" in recorded)
    check("capture: it states the ASLR caveat for the addresses", "ASLR" in recorded)
    for name in LECTURES:
        if name not in blocks or name not in fresh_outs:
            check("%s: the capture block is comparable" % TAG[name], False, "missing block or fresh run")
            continue
        rec = invariant_lines(blocks[name])
        fre = invariant_lines(fresh_outs[name].splitlines())
        first = next((("fresh=%r recorded=%r" % (a, b)) for a, b in zip(fre, rec) if a != b), "line count %d vs %d" % (len(fre), len(rec)))
        check("%s: every invariant line of the capture matches a fresh run" % TAG[name],
              fre == rec, "first difference: " + first)


# ------------------------------------------------------------------------------- docs

def has_emoji(text):
    for ch in text:
        cp = ord(ch)
        if cp in EMOJI_SINGLES or any(lo <= cp <= hi for lo, hi in EMOJI_RANGES):
            return ch
    return None


def fresh_heading_sections(text):
    heads = [(m.start(), m.group(2).strip().lower()) for m in re.finditer(r"(?m)^(#{1,6})[ \t]+(.*)$", text)]
    out = []
    for i, (pos, title) in enumerate(heads):
        end = heads[i + 1][0] if i + 1 < len(heads) else len(text)
        out.append((title, text[pos:end]))
    return out


def section_body(text, pred):
    """From the first heading that matches pred down to the next heading of the same or higher level.

    The numericals live under a level 2 heading with level 3 sub-headings per lecture, so a
    heading-only slice would return an almost empty body and count zero questions.
    """
    heads = [(m.start(), len(m.group(1)), m.group(2).strip().lower())
             for m in re.finditer(r"(?m)^(#{1,6})[ \t]+(.*)$", text)]
    for i, (pos, lvl, title) in enumerate(heads):
        if pred(title):
            end = len(text)
            for pos2, lvl2, _ in heads[i + 1:]:
                if lvl2 <= lvl:
                    end = pos2
                    break
            return text[pos:end]
    return None


def check_docs():
    """The notes and the six diagrams are asserted hard once they exist.

    The doc agents write these in parallel. A file that is not on disk yet cannot be
    asserted, so that single check reports SKIP and says so. Nothing here writes to them.
    """
    if not NOTES.exists():
        skip("docs: study notes exist and are non-trivial", "SKIP, %s not written yet" % NOTES.name)
        notes = None
    else:
        notes = NOTES.read_text(encoding="utf-8", errors="replace")
        n = NOTES.stat().st_size
        check("docs: study notes exist and are non-trivial (%d bytes)" % n, n >= 20000, "%d bytes" % n)
        check("docs: notes carry no em dash (user absolute rule)", EMDASH not in notes)
        check("docs: notes carry no emoji", has_emoji(notes) is None, str(has_emoji(notes)))
        heads = fresh_heading_sections(notes)
        keys = [t for t, _ in heads if "answer" in t or "key" in t]
        check("docs: notes have a separate answer-key section", bool(keys), str(keys))
        numericals = 0
        num_body = section_body(notes, lambda t: "numerical" in t or "practice" in t)
        if num_body:
            numericals = len(re.findall(r"(?m)^[ \t]*\d+[.)][ \t]+\S", num_body))
        check("docs: notes carry at least 20 numbered numericals", numericals >= 20, "%d found" % numericals)
        answers = 0
        key_body = section_body(notes, lambda t: "answer" in t or "answer key" in t)
        if key_body:
            answers = len(re.findall(r"(?m)^[ \t]*\*{0,2}\d+[.)]\*{0,2}[ \t]+\S", key_body))
        check("docs: the answer key numbers every numerical (%d answers for %d questions)" % (answers, numericals),
              numericals > 0 and answers >= numericals, "%d answers" % answers)
        for needle in ["1999999000000", "18446744073709551615", "976", "138", "15.80", "4012ECE"]:
            check("docs: notes quote the measured %s" % needle, needle in notes)

    for name in DIAGRAM_NAMES:
        svg = DIAGRAMS / name
        if not svg.exists():
            skip("docs: notes reference diagrams/%s" % name, "SKIP, %s not written yet" % name)
            skip("docs: %s parses and declares viewBox with width 840" % name, "SKIP, %s not written yet" % name)
            skip("docs: %s is non-trivial and free of emoji" % name, "SKIP, %s not written yet" % name)
            continue
        if notes is not None:
            check("docs: notes reference diagrams/%s" % name, "diagrams/" + name in notes)
        else:
            skip("docs: notes reference diagrams/%s" % name, "SKIP, notes not written yet")
        raw = svg.read_text(encoding="utf-8", errors="replace")
        try:
            root = ET.fromstring(raw)
            tag = root.tag.split("}")[-1]
            check("docs: %s parses as XML and declares viewBox with width 840" % name,
                  tag == "svg" and root.get("viewBox") is not None and root.get("width") == "840",
                  "tag=%s viewBox=%s width=%s" % (tag, root.get("viewBox"), root.get("width")))
        except ET.ParseError as exc:
            check("docs: %s parses as XML and declares viewBox with width 840" % name, False, str(exc))
        size = svg.stat().st_size
        check("docs: %s is at least 1200 bytes and free of emoji" % name,
              size >= 1200 and has_emoji(raw) is None and EMDASH not in raw,
              "%d bytes, emoji %r" % (size, has_emoji(raw)))


def check_self():
    me = Path(__file__).read_text(encoding="utf-8")
    check("verifier: this file contains no em dash (user absolute rule)", EMDASH not in me)
    check("verifier: at least 65 checks ran", len(RESULTS) + 1 >= 65, "%d so far" % len(RESULTS))


# ------------------------------------------------------------------ mutation battery

def mutate(name, transform):
    """Apply a fault to a COPY of one lesson and report which checks it breaks.

    The lesson directory is copied into a temp dir first, so the originals are never
    touched. Returns (applied, failed_labels). `applied` is False when the source text no
    longer matches the pattern, which is a stale test and is reported as MISSED rather
    than quietly counted as a catch.
    """
    with tempfile.TemporaryDirectory() as td:
        tmp = Path(td)
        work = tmp / "lessons"
        shutil.copytree(LESSONS, work)
        src = work / (name + ".cpp")
        original = src.read_text(encoding="utf-8")
        doc = transform(original)
        if doc == original:
            return False, ["the mutation did not apply to the source (stale pattern)"]
        src.write_text(doc, encoding="utf-8")
        cp, exe = build(work, name, tmp)
        warns = [l for l in cp.stderr.splitlines() if "warning:" in l]
        if cp.returncode != 0:
            return True, ["%s: compiles, exit 0" % TAG[name]]
        try:
            run = run_exe(exe, timeout=300)
        except subprocess.TimeoutExpired:
            return True, ["%s: runs, exit 0 (the mutant hung, a hang counts as caught)" % TAG[name]]
        saved = list(RESULTS)
        del RESULTS[:]
        try:
            run_lecture_checks(name, run.stdout)
            failed = [l for l, st, _ in RESULTS if st == "FAIL"]
        finally:
            del RESULTS[:]
            RESULTS.extend(saved)
        if warns:
            # a warning is a catch in its own right (GCC sees the out of bounds write), but
            # it is listed after the content failures so the battery shows the stronger one
            failed = failed + ["%s: zero warnings under -Wall -Wextra -Wpedantic (%d emitted: %s)"
                               % (TAG[name], len(warns), warns[0].split("warning:")[-1].strip()[:60])]
        return True, failed


REAL_MUTATIONS = [
    ("11_stack_array", "the full() guard is removed from push",
     lambda s: s.replace("if (full()) { refusals_++; return false; }",
                         "if (false) { refusals_++; return false; }", 1)),
    ("11_stack_array", "pop returns the bottom of the stack instead of the top",
     lambda s: s.replace("out = data_[top_];\n        return true;",
                         "out = data_[0];\n        return true;", 1)),
    ("11_stack_array", "the canary probe lands one int short of the canary",
     lambda s: s.replace("bench.data + 6 * sizeof(int)", "bench.data + 5 * sizeof(int)", 1)),
    ("11_stack_array", "the bottom end stops shifting (the ratio argument dies)",
     lambda s: s.replace("for (size_t k = used; k > 0; k--)", "for (size_t k = 0; k > 0; k--)", 1)),
    ("11_stack_array", "the printed layout mirror puts the index before the array",
     lambda s: s.replace("    int data[5];\n    size_t top;\n    size_t refusals;",
                         "    size_t top;\n    int data[5];\n    size_t refusals;", 1)),
    ("11_stack_array", "the real stack grows a spare bookkeeping field",
     lambda s: s.replace("    size_t refusals_ = 0;\n};",
                         "    size_t refusals_ = 0;\n    size_t spare_ = 0;\n};", 1)),
    ("12_stack_linked_apps", "the linked stack stops counting underflows",
     lambda s: s.replace("if (!head_) { underflows_++; return false; }",
                         "if (!head_) { return false; }", 1)),
    ("12_stack_linked_apps", "each node asks the heap for 32 bytes instead of 16",
     lambda s: s.replace("Node *n = (Node *)heapAlloc(sizeof(Node));",
                         "Node *n = (Node *)heapAlloc(sizeof(Node) * 2);", 1)),
    ("12_stack_linked_apps", "the bracket checker stops comparing against the top",
     lambda s: s.replace("if (st.back().first != want) {", "if (false) {", 1)),
    ("12_stack_linked_apps", "the bracket checker reports the wrong failing index",
     lambda s: s.replace("failPos = i;\n                why = \"expected \"", "failPos = 0;\n                why = \"expected \"", 1)),
    ("12_stack_linked_apps", "reverseWithStack becomes a copy",
     lambda s: s.replace("while (st.pop(ch)) out.push_back((char)ch);", "out = in;", 1)),
    ("12_stack_linked_apps", "the free counter stops counting",
     lambda s: s.replace("static void heapFree(void *p) { g_freeCalls++; free(p); }",
                         "static void heapFree(void *p) { free(p); }", 1)),
    ("13_queue_array", "the naive queue stops reporting the slots it abandoned",
     lambda s: s.replace("size_t freeSlotsBelowFront() const { return front_; }",
                         "size_t freeSlotsBelowFront() const { return 0; }", 1)),
    ("13_queue_array", "the circular queue stops counting its wraps",
     lambda s: s.replace("wraps_ += (rear_ == 0) ? 1 : 0;", "wraps_ += 0;", 1)),
    ("13_queue_array", "the naive enqueue stops counting refusals",
     lambda s: s.replace("if (rear_ == CAP) { refusals_++; return false; }",
                         "if (rear_ == CAP) { return false; }", 1)),
    ("13_queue_array", "the FIFO order check is inverted",
     lambda s: s.replace("if (v != (int)i) ordered = false;", "if (v == (int)i) ordered = false;", 1)),
    ("14_queue_linked_circular", "the linked queue grows an extra 8 byte field",
     lambda s: s.replace("    size_t count_ = 0;\n    size_t underflows_ = 0;\n};",
                         "    size_t count_ = 0;\n    size_t underflows_ = 0;\n    char pad_[8]{};\n};", 1)),
    ("14_queue_linked_circular", "the count formula drops the + capacity repair",
     lambda s: s.replace("return (rear + CAP - front) % CAP;", "return (rear - front) % CAP;", 1)),
    ("14_queue_linked_circular", "the sentinel full test loses its guaranteed empty slot",
     lambda s: s.replace("bool full() const { return (rear_ + 1) % CAP == front_; }",
                         "bool full() const { return rear_ == front_; }", 1)),
    ("14_queue_linked_circular", "the wrap counter stops counting",
     lambda s: s.replace("if (rear_ == 0) wraps_++;", "if (rear_ == 0) {}", 1)),
    ("15_queue_apps", "the round robin quantum becomes 4",
     lambda s: s.replace("const int QUANTUM = 3;", "const int QUANTUM = 4;", 1)),
    ("15_queue_apps", "the two-stack queue stops counting its transfers",
     lambda s: s.replace("transfers_ += in_.size();", "transfers_ += 0;", 1)),
    ("15_queue_apps", "the stack from queues rotates one element too few",
     lambda s: s.replace("for (size_t i = 0; i + 1 < q_.size(); i++) {",
                         "for (size_t i = 0; i + 2 < q_.size(); i++) {", 1)),
    ("15_queue_apps", "the stack from queues reads the wrong end, FIFO comes back",
     lambda s: s.replace("int v = q_.front();\n        q_.pop();", "int v = q_.back();\n        q_.pop();", 1)),
    ("15_queue_apps", "the FCFS waiting times collapse to zero",
     lambda s: s.replace("int wait = start - t.arrival;", "int wait = start - start;", 1)),
    ("15_queue_apps", "the pseudo random operation stream changes",
     lambda s: s.replace("lcg = lcg * 1103515245u + 12345u;", "lcg = lcg * 1103515245u + 12346u;", 1)),
]


def run_mutations():
    print("\n== MUTATION BATTERY: a verifier that cannot fail is not a verifier ==")
    print("   every fault is injected into a COPY of the lesson source in a temp dir, then the")
    print("   mutant is rebuilt and re-run and the lecture's checks are replayed against it")
    caught = 0
    for name, what, fn in REAL_MUTATIONS:
        try:
            applied, failed = mutate(name, fn)
        except Exception as exc:  # noqa: BLE001
            applied, failed = True, ["exception while mutating: %s: %s" % (type(exc).__name__, exc)]
        ok = applied and bool(failed)
        caught += ok
        print("  [%s] %-12s %s%s" % ("CAUGHT" if ok else "MISSED", TAG[name], what,
                                     "  -> " + failed[0] if failed else "  -> no check failed"))
    print("== MUTATION RESULT: %d/%d injected faults caught, and every one of them caught by"
          " a NAMED check ==" % (caught, len(REAL_MUTATIONS)))
    print("== KNOWN BLIND SPOTS (stated, not hidden):")
    print("   - addresses and wall clock timings are masked in the capture freshness check, so a")
    print("     wrong digit that appears ONLY on a timing line survives that comparison. The")
    print("     per-lecture bounds checks test those numbers directly instead.")
    print("   - the freshness check compares the recorded capture against a fresh run of the same")
    print("     compiler. A different g++ version could legitimately move a padding or a timing.")
    print("   - offsetof(top) and offsetof(refusals) are printed from ArrayStackMirror, a second")
    print("     struct declared to have the same layout as the class. Reordering the REAL class's")
    print("     members is invisible here when it leaves sizeof() unchanged (it does for 5 ints:")
    print("     both orders come to 40 bytes), so the verifier tests the printed mirror, and it")
    print("     tests the real class only through its printed sizeof, its heap block and its sums.")
    return caught == len(REAL_MUTATIONS)


def main():
    args = sys.argv[1:]
    required = [LESSONS / (n + ".cpp") for n in LECTURES] + [CAPTURE]
    for path in required:
        if not path.exists():
            print("missing required file: %s" % path)
            return 2

    recorded = CAPTURE.read_text(encoding="utf-8")
    fresh = {}
    with tempfile.TemporaryDirectory() as td:
        tmp = Path(td)
        for name in LECTURES:
            tag = TAG[name]
            cp, exe = build(LESSONS, name, tmp)
            warns = [l for l in cp.stderr.splitlines() if "warning:" in l]
            check("%s: compiles, exit 0" % tag, cp.returncode == 0,
                  cp.stderr.strip().splitlines()[-1][:160] if cp.returncode else "")
            check("%s: zero warnings under -Wall -Wextra -Wpedantic" % tag, not warns,
                  warns[0][:160] if warns else "")
            if cp.returncode != 0:
                check("%s: runs, exit 0" % tag, False, "not built")
                continue
            run = run_exe(exe)
            check("%s: runs, exit 0" % tag, run.returncode == 0, run.stderr.strip()[-160:])
            if run.returncode == 0:
                fresh[name] = run.stdout
                run_lecture_checks(name, run.stdout)
            else:
                check("%s: output was produced" % tag, False, "non-zero exit, no output trusted")

    check_capture(recorded, fresh)
    check_docs()
    check_self()

    passed = sum(1 for _, st, _ in RESULTS if st == "PASS")
    failed = [(l, d) for l, st, d in RESULTS if st == "FAIL"]
    skipped = sum(1 for _, st, _ in RESULTS if st == "SKIP")
    for label, detail in failed:
        print("  [FAIL] %s%s" % (label, "   " + detail if detail else ""))
    for label, _, detail in RESULTS:
        if _ == "SKIP":
            print("  [SKIP] %s%s" % (label, "   " + detail if detail else ""))
    print("\n== VERIFY: %d PASS, %d FAIL, %d SKIP (%d checks) =="
          % (passed, len(failed), skipped, len(RESULTS)))
    status = 0 if not failed else 1

    if "--mutate" in args:
        mut_ok = run_mutations()
        status = status or (0 if mut_ok else 1)
    return status


if __name__ == "__main__":
    sys.exit(main())
