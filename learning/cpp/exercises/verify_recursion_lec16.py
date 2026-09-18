#!/usr/bin/env python3
"""
verify_recursion_lec16.py : verifier for the ECE2104 (DSA) lecture 16 recursion set.

This is the file the study notes already promise, by this exact path:
  RECURSION-STUDY-NOTES.md, section "Files in this folder", row
  "~/learning/cpp/exercises/verify_recursion_lec16.py | recompiles, re-runs and checks every
  number above".

What it proves, in order:
  1. 10_recursion_algorithms.cpp compiles with ZERO warnings under
     -Wall -Wextra -Wpedantic -std=c++17 -O2 (the notes' build line) and exits 0 when run
  2. every count, depth, stride, size, offset, checksum and closed form the lesson prints is
     the value the C++ actually produced, recomputed here in Python from the printed numbers
  3. an INDEPENDENT probe, a second program with its own counters, re-derives the factorial,
     fibonacci, hanoi, tree and stack-limit facts. A wrong self-report inside the lesson
     cannot hide behind itself, and the probe is what measures the fibonacci stack depth the
     lesson never prints (diagram 03 claims a number for it)
  4. the recorded capture is not stale: its invariant lines equal a FRESH run line for line,
     with only the genuinely volatile slots masked (ASLR addresses, wall-clock timings, the
     guard-page death depth, the child pid). Those masked slots are then asserted separately
     against the arithmetic they claim and against a band, never against equality
  5. RECURSION-STUDY-NOTES.md and the three diagram SVGs are on disk, quote the measured
     numbers, and every closed form, percentage, ceiling and ratio in the notes is recomputed
  6. the mutation battery: real faults injected into COPIES of the program, the capture, the
     notes and the diagrams are all caught by a NAMED check. A verifier that cannot fail on a
     broken artifact is not a verifier.

Run:
  python3 ~/learning/cpp/exercises/verify_recursion_lec16.py
  python3 ~/learning/cpp/exercises/verify_recursion_lec16.py --verbose
  python3 ~/learning/cpp/exercises/verify_recursion_lec16.py --no-mutate
Exit: 0 = every claim verified, 1 = at least one mismatch, 2 = a required file is missing.

Known, stated limits of this verifier (not hidden):
  - the guard-page death depth in section 8 genuinely moves a few hundred calls between runs
    (the notes quote 261640, the saved capture holds 261579, a fresh run lands a third value).
    It is checked as a sample: within 1 percent of the computed ceiling of 262144, and
    internally consistent with the "difference" and KiB numbers printed beside it.
  - timings are samples. They are checked for order of magnitude and for internal consistency
    (ratio equals nsRec over nsIter), never for equality with the capture.
  - the notes' claim that overriding operator new emits -Wmismatched-new-delete is asserted
    only for the half this verifier could reproduce on g++ 15.2.0, which is
    -Wsized-deallocation. The mismatched half is reported as a remark, not counted as verified.
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
PROGRAM = LESSONS / "10_recursion_algorithms.cpp"
CAPTURE = EXERCISES / "lec16_recursion_output.txt"
PREP = EXERCISES / "lec16_recursion_prep_probe_output.txt"
NOTESDIR = HOME / "Documents/MUJ-SEM-3/DSA/Notes/recursion"
NOTES = NOTESDIR / "RECURSION-STUDY-NOTES.md"
DIAGRAMS = NOTESDIR / "diagrams"
DIAGRAM_NAMES = ["01-call-stack-growth.svg",
                 "02-call-stack-unwind.svg",
                 "03-fibonacci-tree-vs-stack-depth.svg"]
SELF = EXERCISES / "verify_recursion_lec16.py"
FLAGS = ["-Wall", "-Wextra", "-Wpedantic", "-std=c++17", "-O2"]
BUILD_LINE = "g++ -Wall -Wextra -Wpedantic -std=c++17 -O2 -o /tmp/lec10 10_recursion_algorithms.cpp"

RESULTS = []          # (label, PASS|FAIL, detail)
REPORTING = True      # False while a mutation replays the checks into a scratch list
REMARKS = []          # honest notes about samples and unverified halves
EMDASH = "\u2014"
TIMES = "\u00d7"      # the multiplication sign the notes use
MU = "\u00b5"

EMOJI_RANGES = [(0x1F000, 0x1F2FF), (0x1F300, 0x1FAFF), (0x1F1E6, 0x1F1FF),
                (0x2600, 0x27BF), (0x2B00, 0x2BFF), (0xFE0F, 0xFE0F)]


_VERBOSE = False
COLLECT = None      # a scratch failure list, used while a mutation replays the checks


def check(label, ok, detail=""):
    if not ok:
        if COLLECT is not None:
            COLLECT.append((label, detail))
        if REPORTING:
            RESULTS.append((label, "FAIL", detail))
            if _VERBOSE:
                print("  [FAIL] %s   %s" % (" ".join(label.split()), detail))
    elif REPORTING:
        RESULTS.append((label, "PASS", ""))
        if _VERBOSE:
            print("  [PASS] %s" % " ".join(label.split()))
    return bool(ok)


def eq(label, got, want):
    return check(" ".join(label.split()), got == want, "got %r, want %r" % (got, want))


def at(d, key, idx, default=0):
    """A parsed tuple element, safely: a mutant can make the whole line unparseable."""
    v = d.get(key)
    return v[idx] if isinstance(v, tuple) and idx < len(v) else default


def note(text):
    if REPORTING and text not in REMARKS:
        REMARKS.append(text)


def has_emoji(text):
    for ch in text:
        cp = ord(ch)
        if any(lo <= cp <= hi for lo, hi in EMOJI_RANGES):
            return ch
    return None


def build(src_dir, src_name, outdir, exe_name="prog"):
    """Compile with the notes' exact flags. Returns (ok, warnings, stderr, exe)."""
    exe = Path(outdir) / exe_name
    cp = subprocess.run(["g++"] + FLAGS + ["-o", str(exe), str(Path(src_dir) / src_name)],
                        capture_output=True, text=True)
    warns = [l for l in cp.stderr.splitlines() if "warning:" in l]
    return cp.returncode == 0 and exe.exists(), warns, cp.stderr, exe


def run_exe(exe, timeout=300):
    return subprocess.run([str(exe)], capture_output=True, text=True, timeout=timeout)


# ======================================================================================
# the independent probe
# ======================================================================================
# A second program, written for the verifier alone: its own counters, its own recurrences,
# and the one number the lesson never prints, the deepest fib stack. If the lesson's
# self-report is wrong, this disagrees with it.
PROBE_SRC = r"""
#include <iostream>
#include <cstddef>
#include <vector>
#include <sys/resource.h>
using namespace std;

static long long gCalls = 0, gFib3 = 0;
static int gCur = 0, gDeep = 0;
static long long fibNaive(int n) {
    gCalls++; gCur++;
    if (gCur > gDeep) gDeep = gCur;
    if (n == 3) gFib3++;
    if (n < 2) { gCur--; return n; }
    long long r = fibNaive(n - 1) + fibNaive(n - 2);
    gCur--;
    return r;
}
static long long fibIter(int n) {
    long long a = 0, b = 1;
    for (int i = 0; i < n; i++) { long long t = a + b; a = b; b = t; }
    return a;
}
static long long gMemoCalls = 0;
static long long fibMemo(int n, vector<long long> &m) {
    gMemoCalls++;
    if (n < 2) return n;
    if (m[(size_t)n] != -1) return m[(size_t)n];
    m[(size_t)n] = fibMemo(n - 1, m) + fibMemo(n - 2, m);
    return m[(size_t)n];
}
static long long gFactCalls = 0;
static int gFactDeep = 0;
static long long factRec(int n, int d) {
    gFactCalls++;
    if (d + 1 > gFactDeep) gFactDeep = d + 1;
    if (n <= 1) return 1;
    return n * factRec(n - 1, d + 1);
}
static long long gHanoiCalls = 0, gHanoiMoves = 0;
static int gHanoiCur = 0, gHanoiDeep = 0;
static void hanoi(int n) {
    gHanoiCalls++; gHanoiCur++;
    if (gHanoiCur > gHanoiDeep) gHanoiDeep = gHanoiCur;
    if (n == 1) { gHanoiMoves++; gHanoiCur--; return; }
    hanoi(n - 1);
    gHanoiMoves++;
    hanoi(n - 1);
    gHanoiCur--;
}
struct TN { int value; TN *left; TN *right; };

int main() {
    for (int n = 1; n <= 8; n++) {
        gFactCalls = 0; gFactDeep = 0;
        long long v = factRec(n, 0);
        cout << "FACT n=" << n << " v=" << v << " calls=" << gFactCalls
             << " depth=" << gFactDeep << "\n";
    }
    for (int n : {4, 5, 32}) {
        gCalls = 0; gFib3 = 0; gCur = 0; gDeep = 0;
        long long v = fibNaive(n);
        cout << "FIB n=" << n << " v=" << v << " calls=" << gCalls << " depth=" << gDeep
             << " fib3=" << gFib3 << " closed=" << (2 * fibIter(n + 1) - 1) << "\n";
    }
    for (int n = 10; n <= 24; n += 7) {
        gCalls = 0; gFib3 = 0; gCur = 0; gDeep = 0;
        long long v = fibNaive(n);
        cout << "FIBROW n=" << n << " v=" << v << " calls=" << gCalls << " fib3=" << gFib3
             << " closed=" << (2 * fibIter(n + 1) - 1) << "\n";
    }
    {
        vector<long long> m(33, -1);
        gMemoCalls = 0;
        long long v = fibMemo(32, m);
        long long written = 0;
        for (size_t i = 0; i < m.size(); i++) if (m[i] != -1) written++;
        cout << "MEMO n=32 v=" << v << " calls=" << gMemoCalls << " slots=" << (long long)m.size()
             << " written=" << written << "\n";
    }
    for (int n = 1; n <= 6; n++) {
        gHanoiCalls = 0; gHanoiMoves = 0; gHanoiCur = 0; gHanoiDeep = 0;
        hanoi(n);
        cout << "HANOI n=" << n << " moves=" << gHanoiMoves << " calls=" << gHanoiCalls
             << " frames=" << gHanoiDeep << " closed=" << ((1LL << n) - 1) << "\n";
    }
    cout << "HANOI20 moves=" << ((1LL << 20) - 1) << " closed=" << ((1LL << 20) - 1) << "\n";
    cout << "TREE nodes=" << ((1LL << 10) - 1) << " sizeof=" << (long long)sizeof(TN)
         << " offl=" << (long long)offsetof(TN, left) << " offr=" << (long long)offsetof(TN, right)
         << " sum=" << (1023LL * 1024 / 2) << "\n";
    struct rlimit rl;
    long long stackBytes = 8LL * 1024 * 1024;
    if (getrlimit(RLIMIT_STACK, &rl) == 0 && rl.rlim_cur != RLIM_INFINITY)
        stackBytes = (long long)rl.rlim_cur;
    cout << "STACK kib=" << (stackBytes / 1024) << " bytes=" << stackBytes
         << " floor48=" << (stackBytes / 48) << " ceil48=" << ((stackBytes + 47) / 48)
         << " floor96=" << (stackBytes / 96) << " ceil96=" << ((stackBytes + 95) / 96)
         << " floor32=" << (stackBytes / 32) << " half=" << ((stackBytes / 48) / 2) << "\n";
    return 0;
}
"""


def parse_probe(text):
    d = {}
    d["fact"] = {}
    for m in re.finditer(r"(?m)^FACT n=(\d+) v=(\d+) calls=(\d+) depth=(\d+)$", text):
        d["fact"][int(m.group(1))] = tuple(int(m.group(i)) for i in (2, 3, 4))
    d["fib"] = {}
    for m in re.finditer(r"(?m)^FIB n=(\d+) v=(\d+) calls=(\d+) depth=(\d+) fib3=(\d+) closed=(\d+)$", text):
        d["fib"][int(m.group(1))] = tuple(int(m.group(i)) for i in range(2, 7))
    d["fibrow"] = {}
    for m in re.finditer(r"(?m)^FIBROW n=(\d+) v=(\d+) calls=(\d+) fib3=(\d+) closed=(\d+)$", text):
        d["fibrow"][int(m.group(1))] = tuple(int(m.group(i)) for i in (2, 3, 4, 5))
    m = re.search(r"(?m)^MEMO n=32 v=(\d+) calls=(\d+) slots=(\d+) written=(\d+)$", text)
    d["memo"] = tuple(int(m.group(i)) for i in range(1, 5)) if m else None
    d["hanoi"] = {}
    for m in re.finditer(r"(?m)^HANOI n=(\d+) moves=(\d+) calls=(\d+) frames=(\d+) closed=(\d+)$", text):
        d["hanoi"][int(m.group(1))] = tuple(int(m.group(i)) for i in (2, 3, 4, 5))
    m = re.search(r"(?m)^HANOI20 moves=(\d+) closed=(\d+)$", text)
    d["hanoi20"] = tuple(int(m.group(i)) for i in (1, 2)) if m else None
    m = re.search(r"(?m)^TREE nodes=(\d+) sizeof=(\d+) offl=(\d+) offr=(\d+) sum=(\d+)$", text)
    d["tree"] = tuple(int(m.group(i)) for i in range(1, 6)) if m else None
    m = re.search(r"(?m)^STACK kib=(\d+) bytes=(\d+) floor48=(\d+) ceil48=(\d+) "
                  r"floor96=(\d+) ceil96=(\d+) floor32=(\d+) half=(\d+)$", text)
    d["stack"] = tuple(int(m.group(i)) for i in range(1, 9)) if m else None
    return d


# ======================================================================================
# parsing the lesson program's output
# ======================================================================================
def flatten(text):
    """Collapse every run of whitespace, so a line the program wrapped by hand still matches."""
    return re.sub(r"\s+", " ", text)


def one(d, key, pattern, cast=int, flags=0):
    """Pull one group out of the flattened output, recording the key when it is absent."""
    m = re.search(pattern, d["_flat"], flags)
    if not m:
        d["_missing"].append(key)
        d[key] = None
        return None
    gs = m.groups()
    d[key] = tuple(cast(g) for g in gs) if len(gs) > 1 else cast(gs[0])
    return d[key]


def parse_program(text):
    """Every number the program prints, in one dict. An absent line is recorded, not raised."""
    d = {"_flat": flatten(text), "_missing": []}
    flat = d["_flat"]
    d["fact_rows"] = [tuple(int(x) for x in m.groups()) for m in
                      re.finditer(r"(?m)^ +(\d+) +(\d+) +(\d+) +(\d+) +calls and frames both equal n$", text)]
    d["fact8"] = one(d, "fact8", r"factorial\(8\) = (\d+), (\d+) calls in total and (\d+) frames alive at the deepest")
    d["fact8_addr"] = one(d, "fact8_addr", r"first frame at (0x[0-9a-f]+) and the innermost at (0x[0-9a-f]+)", str)
    d["probe_frames"] = one(d, "probe_frames", r"the probe recorded (\d+) frames, so (\d+) gaps between them")
    d["span"] = one(d, "span", r"span across those frames = (\d+) bytes, so (\d+) bytes per recursive call")
    d["charge"] = one(d, "charge", r"charging every live frame instead = (\d+) x (\d+) = (\d+) bytes")
    d["shape_linear"] = one(d, "shape_linear", r"linearSumRec\(n\) 7 (\d+) (\d+) (\d+) one long long argument")
    d["shape_hanoi"] = one(d, "shape_hanoi", r"hanoi\(n, from, to, via\) 7 (\d+) (\d+) (\d+) four arguments")
    d["shape_fact"] = one(d, "shape_fact", r"factorialRec\(n\) 8 (\d+) (\d+) (\d+) same shape")
    d["hanoi_sep"] = one(d, "hanoi_sep", r"(\d+) calls but only (\d+) frames alive, and the depth counter says (\d+) as well")
    d["sum7"] = one(d, "sum7", r"linearSumRec\(7\) = (\d+) and the closed form 7 x 8 / 2 = (\d+)")
    d["fib_rows"] = [tuple(int(x) for x in m.groups()) for m in
                     re.finditer(r"(?m)^ +(\d+) +(\d+) +(\d+) +(\d+) +(\d+) +match$", text)]
    d["fib32_vals"] = one(d, "fib32_vals", r"fib\(32\): naive (\d+), memoised (\d+), iterative (\d+)")
    d["fib32_agree"] = one(d, "fib32_agree", r"all three agree: (\w+)", str)
    d["fib3_hits"] = one(d, "fib3_hits", r"naive computed fib\(3\) (\d+) times inside the single call fib\(32\)")
    d["fib32_naive"] = one(d, "fib32_naive", r"naive calls = (\d+) in (\d+\.\d+) ms", float, flags=re.S)
    d["fib32_memo"] = one(d, "fib32_memo", r"memoised calls = (\d+), and (\d+) runs with a fresh table each")
    d["memo_checksum"] = one(d, "memo_checksum", r"return the checksum (\d+) against the expected (\d+)")
    d["fib32_percall"] = one(d, "fib32_percall", r"per single fib\(32\) call: (\d+\.\d+) ns naive against (\d+\.\d+) ns memoised", float)
    d["fib32_ratio"] = one(d, "fib32_ratio", r"ratio = (\d+\.\d+)x, which is what recomputing (\d+) calls costs against (\d+)", float, flags=re.S)
    d["memo_table"] = one(d, "memo_table", r"the table stores (\d+) entries and cuts the call count by (\d+)x")
    d["hanoi_rows"] = [tuple(int(x) for x in m.groups()) for m in
                       re.finditer(r"(?m)^ +(\d+) +(\d+) +(\d+) +(\d+) +(\d+) +(\d+) +moves, calls and frames all check out$", text)]
    d["hanoi20"] = one(d, "hanoi20", r"2\^20 - 1 = (\d+) moves for n = 20: a second per move is (\d+\.\d+) years", float, flags=re.S)
    d["s5_closed"] = one(d, "s5_closed", r"closed form for the middle row: 1000 x 1001 / 2 = (\d+)")
    d["s5_reps"] = one(d, "s5_reps", r"(\d+) walks of (\d+) terms each")
    d["s5_checksum"] = one(d, "s5_checksum", r"returned the same checksum: (\w+) \((\d+), expected (\d+)\)", str)
    d["s5_times"] = one(d, "s5_times", r"recursion per walk: (\d+\.\d+) ns, loop per walk: (\d+\.\d+) ns", float)
    d["s5_ratio"] = one(d, "s5_ratio", r"ratio = (\d+\.\d+)x: the difference", float)
    d["tree"] = one(d, "tree", r"a complete binary tree of depth (\d+): nodes = 2\^\((\d+)\) - 1 = (\d+)")
    d["tree_build"] = one(d, "tree_build", r"the recursive build made (\d+) heap requests through heapAlloc, (\d+) bytes requested")
    d["tree_sizeof"] = one(d, "tree_sizeof", r"sizeof\(TreeNode\{int; TreeNode\*; TreeNode\*\}\) = (\d+), offsetof\(left\) = (\d+), offsetof\(right\) = (\d+)")
    d["tree_usable"] = one(d, "tree_usable", r"the allocator reports (\d+) usable bytes for that (\d+) byte request")
    d["tree_visits"] = one(d, "tree_visits", r"recursive inorder visits = (\d+), explicit stack visits = (\d+)")
    d["tree_sum"] = one(d, "tree_sum", r"both sums equal: (\w+) \((\d+)\), and the closed form 1\.\.(\d+) sums to (\d+)", str)
    d["tree_free"] = one(d, "tree_free", r"the recursive destroy made (\d+) free calls through heapFree for (\d+) nodes: (\w+), no leak", str)
    d["stack"] = one(d, "stack", r"getrlimit\(RLIMIT_STACK\) says this process may use (\d+) KiB")
    d["s7_stride"] = one(d, "s7_stride", r"the measured cost of one suspended call to linearSumRec is (\d+) bytes")
    d["s7_ceiling"] = one(d, "s7_ceiling", r"ceiling on unaided recursion depth = (\d+) / (\d+) = (\d+) calls")
    d["s7_safe"] = one(d, "s7_safe", r"so a real run at depth (\d+) \(half the ceiling\) is safe")
    d["s7_checksum"] = one(d, "s7_checksum", r"checksum = (\d+), expected (\d+) x (\d+) / 2 = (\d+) -> (\w+)", str)
    d["s7_used"] = one(d, "s7_used", r"that recursion used about (\d+) KiB of (\d+) KiB and took (\d+) microseconds")
    d["s7_fatal"] = one(d, "s7_fatal", r"the same code at depth (\d+) runs past the limit")
    d["s8_pred"] = one(d, "s8_pred", r"section 7 predicted a ceiling for linearSumRec \((\d+) bytes per call, (\d+) calls\)")
    d["s8_stride"] = one(d, "s8_stride", r"the child measured its own runaway frame stride: (\d+) bytes per call")
    d["s8_ceiling"] = one(d, "s8_ceiling", r"predicted ceiling = (\d+) / (\d+) = (\d+) calls")
    d["s8_died"] = one(d, "s8_died", r"measured: the child reached depth (\d+) before the guard page")
    d["s8_diff"] = one(d, "s8_diff", r"difference (-?\d+) calls \((-?\d+\.\d+) percent\), at (\d+) bytes per frame that is (\d+) KiB against the (\d+) KiB limit", float)
    d["s8_exit"] = one(d, "s8_exit", r"with code (\d+), the code the handler asks for")
    d["s8_sig"] = one(d, "s8_sig", r"waitpid reports signal (\d+), which is none")
    d["s8_pid"] = one(d, "s8_pid", r"child pid (\d+) is gone")
    d["s9_strides"] = one(d, "s9_strides", r"per suspended call: (\d+) bytes for linearSumRec, (\d+) for factorial, (\d+) for hanoi, (\d+) for the runaway")
    return d


# ======================================================================================
# the checks
# ======================================================================================

def missing_report(d):
    miss = d.get("_missing") or []
    check("program: every printed line the verifier reads is present (%d read)" % len(d),
          not miss, "missing: %s" % ", ".join(miss[:8]))


def checks_build(env):
    b = env["build"]
    last = (b["stderr"].strip().splitlines() or [""])[-1][:200]
    check("build: the notes' build line compiles the lesson and g++ exits 0", b["ok"], last)
    check("build: zero warnings under -Wall -Wextra -Wpedantic", not b["warnings"],
          b["warnings"][0][:200] if b["warnings"] else "")
    src = env["program"]
    check("build: the source carries the documented build line as a comment",
          ("// Build: " + BUILD_LINE) in src,
          "the comment line '// Build: %s' is not in the source" % BUILD_LINE)
    check("build: global operator new is deliberately not overridden (notes C6)",
          "void *operator new" not in src and "void* operator new" not in src
          and "void *operator delete" not in src,
          "an operator new/delete overload is declared in the source")
    check("build: the source routes its own allocations through heapAlloc/heapFree (notes C6)",
          "static void *heapAlloc" in src and "static void heapFree" in src
          and src.count("heapAlloc(") >= 2 and src.count("heapFree(") >= 2)


def checks_run(env):
    runs = env["runs"]
    check("run: the lesson exits 0 when executed", env["run_ok"] and len(runs) >= 1)
    text = runs[0] if runs else ""
    for tag in ("=== 1. factorial", "=== 2. the frame itself", "=== 3. fibonacci",
                "=== 4. Towers of Hanoi", "=== 5. the same answer twice",
                "=== 6. recursion on the heap", "=== 7. the guardrail",
                "=== 8. the limit, measured in a child process",
                "=== 9. the one line to remember"):
        check("run: the output carries section %s" % tag, tag in flatten(text))
    d = env["parsed"]
    missing_report(d)
    indep = env["indep"]

    # ---- section 1: factorial, calls, depth, frame addresses
    eq("s1: the factorial table is 1..6 with the six measured rows",
       d.get("fact_rows"),
       [(1, 1, 1, 1), (2, 2, 2, 2), (3, 6, 3, 3), (4, 24, 4, 4), (5, 120, 5, 5), (6, 720, 6, 6)])
    eq("s1: factorial(8) = 40320 with 8 calls and 8 frames alive", d.get("fact8"), (40320, 8, 8))
    eq("s1: every factorial row has calls == n and depth == n (T(n) = n + 1, notes N19)",
       [(r[0], r[2], r[3]) for r in (d.get("fact_rows") or [])],
       [(n, n, n) for n in range(1, 7)])
    eq("s1: the probe recorded 8 frames and therefore 7 gaps", d.get("probe_frames"), (8, 7))
    eq("s1: the measured span across eight frames is 336 bytes, stride 48", d.get("span"), (336, 48))
    eq("s1: charging every live frame is 8 x 48 = 384", d.get("charge"), (8, 48, 384))
    check("s1: the span is 7 gaps of 48 and not 8 frames of 48 (notes N2, C2)",
          d.get("span") == (336, 48) and 336 != 8 * 48 and 336 == 7 * 48 and 8 * 48 == 384)
    eq("s1: the span arithmetic recomputed here", (d.get("span") or (0, 0))[0], 336)
    fa = d.get("fact8_addr") or ("0x0", "0x0")
    check("s1: the two printed frame addresses are real, ordered addresses (ASLR, not quoted)",
          fa[0].startswith("0x") and fa[1].startswith("0x") and len(fa[0]) > 6
          and int(fa[0], 16) > int(fa[1], 16),
          "outermost %s innermost %s" % fa)
    if indep:
        eq("s1: the independent probe agrees on the whole factorial table",
           indep["fact"].get(8), (40320, 8, 8))
        eq("s1: the independent probe agrees factorial(6) = 720 with depth 6",
           indep["fact"].get(6), (720, 6, 6))

    # ---- section 2: the frame itself
    eq("s2: linearSumRec(7) has 8 calls and 8 frames at a 48 byte stride",
       d.get("shape_linear"), (8, 8, 48))
    eq("s2: hanoi(7) has 127 calls but only 7 frames, at a 96 byte stride",
       d.get("shape_hanoi"), (127, 7, 96))
    eq("s2: factorialRec(8) sits at the same 48 byte stride as linearSumRec",
       d.get("shape_fact"), (8, 8, 48))
    eq("s2: 127 hanoi calls and 7 live frames with the depth counter agreeing",
       d.get("hanoi_sep"), (127, 7, 7))
    eq("s2: linearSumRec(7) = 28 and the closed form agrees", d.get("sum7"), (28, 28))
    check("s2: 127 == 2^7 - 1, the hanoi call count is the closed form",
          at(d, "shape_hanoi", 0) == 2 ** 7 - 1)
    check("s2: the three measured frame sizes are exactly 48, 96 and 48 bytes (notes section 3)",
          [at(d, "shape_linear", 2), at(d, "shape_hanoi", 2), at(d, "shape_fact", 2)] == [48, 96, 48])

    # ---- section 3: fibonacci
    eq("s3: the fib table rows are the three measured rows",
       d.get("fib_rows"),
       [(10, 55, 177, 21, 177), (17, 1597, 5167, 610, 5167), (24, 46368, 150049, 17711, 150049)])
    check("s3: every fib row's call count equals 2*Fib(n+1) - 1 as the program prints it",
          all(r[2] == r[4] for r in (d.get("fib_rows") or [])))
    eq("s3: fib(32) is 2178309 from all three implementations", d.get("fib32_vals"), (2178309, 2178309, 2178309))
    eq("s3: all three implementations agree", d.get("fib32_agree"), "yes")
    eq("s3: fib(3) is recomputed 832040 times inside one fib(32)", d.get("fib3_hits"), 832040)
    eq("s3: the naive call count is 7049155", (d.get("fib32_naive") or (0,))[0], 7049155)
    eq("s3: the memoised run makes 63 calls over 20000 fresh-table runs", d.get("fib32_memo"), (63, 20000))
    eq("s3: the memo checksum is 20000 x 2178309 = 43566180000", d.get("memo_checksum"), (43566180000, 43566180000))
    check("s3: 20000 x 2178309 really is 43566180000 (the notes' equation)",
          20000 * 2178309 == 43566180000 == (d.get("memo_checksum") or (0,))[0])
    eq("s3: the memo table holds 33 entries, which is n + 1, and the cut is 111891x",
       d.get("memo_table"), (33, 111891))
    check("s3: 7049155 // 63 = 111891, the printed call count reduction",
          (d.get("memo_table") or (0, 0))[1] == 7049155 // 63)
    eq("s3: the printed call count 63 is 2n - 1 for n = 32, never 2n + 1",
       (d.get("fib32_memo") or (0,))[0], 2 * 32 - 1)
    check("s3: the source allocates the memo table and prints its size from the same n + 1 expression",
          env["program"].count("(size_t)FIB_N + 1") >= 2 and "FIB_N + 1) << \" entries" in env["program"],
          "the allocation and the printed table size no longer come from one expression, so the "
          "printed 33 would not describe the table the run actually built")
    check("s3: the naive against memoised ratio is internally consistent with the printed ns",
          abs((d.get("fib32_ratio") or (0,))[0] - (d.get("fib32_percall") or (1, 1))[0]
              / (d.get("fib32_percall") or (1, 1))[1]) <= 0.01 * (d.get("fib32_ratio") or (1,))[0],
          "ratio %r vs ns pair %r" % (d.get("fib32_ratio"), d.get("fib32_percall")))
    if indep:
        eq("s3: the independent probe counts the same 7049155 naive calls",
           indep["fib"].get(32, (0, 0, 0, 0, 0))[1], 7049155)
        eq("s3: the independent probe counts the same 832040 fib(3) hits",
           indep["fib"].get(32, (0, 0, 0, 0, 0))[3], 832040)
        eq("s3: the independent probe makes the same 63 memoised calls over 33 slots",
           indep["memo"][:3], (2178309, 63, 33) if indep["memo"] else None)
        eq("s3: the independent probe agrees on the three fib rows call for call",
           {int(k): v[:3] for k, v in indep["fibrow"].items()},
           {10: (55, 177, 21), 17: (1597, 5167, 610), 24: (46368, 150049, 17711)})
        note("memo table: the 33 printed entries are the table's capacity (n + 1); only 31 slots "
             "(indices 2..32) are ever written, because n < 2 returns before the table is read. "
             "The notes read the program's own capacity line, so 33 is right as a capacity, and "
             "the probe measures the written count at %s." % (indep["memo"][3] if indep["memo"] else "?"))

    # ---- section 4: hanoi
    eq("s4: the hanoi table is n = 1..6 with moves = calls = 2^n - 1 and frames = n",
       [(r[0], r[1], r[3], r[5]) for r in (d.get("hanoi_rows") or [])],
       [(n, 2 ** n - 1, 2 ** n - 1, n) for n in range(1, 7)])
    check("s4: every hanoi row has moves == calls == the printed closed form",
          all(r[1] == r[2] == r[3] == r[4] for r in (d.get("hanoi_rows") or [])))
    eq("s4: 2^20 - 1 = 1048575 moves and 0.033250 years at a move per second",
       (d.get("hanoi20") or (0, 0.0))[0], 1048575)
    check("s4: the printed years equal 1048575 / 3600 / 24 / 365",
          abs((d.get("hanoi20") or (0, 0.0))[1] - 1048575 / 3600.0 / 24.0 / 365.0) < 1e-5,
          "printed %r" % (d.get("hanoi20"),))
    if indep:
        eq("s4: the independent probe reproduces the whole hanoi table",
           {k: v[:3] for k, v in indep["hanoi"].items()},
           {n: (2 ** n - 1, 2 ** n - 1, n) for n in range(1, 7)})

    # ---- section 5: recursive against iterative
    flat5 = flatten(text)
    eq("s5: factorial(12) is 479001600 both ways",
       re.search(r"factorial\(12\) (\d+) (\d+) yes", flat5).groups() if re.search(r"factorial\(12\) (\d+) (\d+) yes", flat5) else None,
       ("479001600", "479001600"))
    eq("s5: the 1..1000 sum is 500500 both ways",
       re.search(r"sum 1\.\.1000 (\d+) (\d+) yes", flat5).groups() if re.search(r"sum 1\.\.1000 (\d+) (\d+) yes", flat5) else None,
       ("500500", "500500"))
    eq("s5: fib(32) is 2178309 both ways",
       re.search(r"fib\(32\) (\d+) (\d+) yes", flat5).groups() if re.search(r"fib\(32\) (\d+) (\d+) yes", flat5) else None,
       ("2178309", "2178309"))
    eq("s5: the closed form for the middle row is 500500", d.get("s5_closed"), 1000 * 1001 // 2)
    eq("s5: 2000 walks of 500 terms each", d.get("s5_reps"), (2000, 500))
    eq("s5: both timed walks return the checksum 250500000 and they agree", d.get("s5_checksum"),
       ("yes", "250500000", "250500000"))
    check("s5: 2000 x 500 x 501 / 2 = 250500000, the closed form the notes quote",
          2000 * 500 * 501 // 2 == 250500000 == int((d.get("s5_checksum") or (0, 0, 0))[1]))
    check("s5: the printed ratio equals the two printed ns per walk",
          abs((d.get("s5_ratio") or 0) - (d.get("s5_times") or (1, 1))[0] / (d.get("s5_times") or (1, 1))[1])
          <= 0.01 * (d.get("s5_ratio") or 1),
          "ratio %r ns pair %r" % (d.get("s5_ratio"), d.get("s5_times")))

    # ---- section 6: the heap tree
    eq("s6: a depth 9 tree is 2^10 - 1 = 1023 nodes", d.get("tree"), (9, 10, 1023))
    eq("s6: the recursive build makes 1023 requests for 24552 bytes", d.get("tree_build"), (1023, 24552))
    check("s6: 24552 / 1023 = 24 bytes requested per node (notes N12)",
          (d.get("tree_build") or (0, 0))[1] // (d.get("tree_build") or (1,))[0] == 24)
    eq("s6: sizeof(TreeNode) = 24 with offsetof left 8 and right 16", d.get("tree_sizeof"), (24, 8, 16))
    eq("s6: the allocator reports 24 usable bytes for the 24 byte request", d.get("tree_usable"), (24, 24))
    eq("s6: recursion and the explicit stack both visit 1023 nodes", d.get("tree_visits"), (1023, 1023))
    eq("s6: both sums are equal and total 523776 over 1..1023", d.get("tree_sum"), ("yes", "523776", "1023", "523776"))
    check("s6: 1023 x 1024 / 2 = 523776, the closed form the notes quote (notes N13)",
          1023 * 1024 // 2 == 523776 == int((d.get("tree_sum") or (0, "", 0, 0))[3]))
    eq("s6: the recursive destroy makes 1023 free calls for 1023 nodes and no leak",
       d.get("tree_free"), ("1023", "1023", "balanced"))
    if indep:
        eq("s6: the independent probe agrees on sizeof and both offsets", indep["tree"][1:4], (24, 8, 16))
        eq("s6: the independent probe agrees on the node count and the value sum",
           (indep["tree"][0], indep["tree"][4]), (1023, 523776))

    # ---- section 7: the guardrail
    eq("s7: the stack limit is 8192 KiB", d.get("stack"), 8192)
    eq("s7: the linearSumRec stride is 48 bytes", d.get("s7_stride"), 48)
    eq("s7: the ceiling is 8388608 / 48 = 174762 calls", d.get("s7_ceiling"), (8388608, 48, 174762))
    eq("s7: the safe run is at depth 87381, half the ceiling", d.get("s7_safe"), 87381)
    eq("s7: the checksum at depth 87381 is 3817763271 and matches the closed form",
       d.get("s7_checksum"), ("3817763271", "87381", "87382", "3817763271", "matches"))
    check("s7: 87381 x 87382 / 2 = 3817763271 and 174762 / 2 = 87381",
          87381 * 87382 // 2 == 3817763271 and 174762 // 2 == 87381)
    eq("s7: the deep run used about 4095 KiB of the 8192 KiB limit", (d.get("s7_used") or (0, 0, 0))[:2], (4095, 8192))
    check("s7: 87381 x 48 / 1024 = 4095 KiB, the arithmetic behind that line",
          (87381 * 48) // 1024 == 4095 == (d.get("s7_used") or (0, 0, 0))[0])
    eq("s7: the fatal depth printed is ceiling + 1 = 174763", d.get("s7_fatal"), 174763)
    if indep:
        st = indep["stack"]
        eq("s7: the independent probe reads the same 8192 KiB limit and 8388608 bytes", st[:2], (8192, 8388608))
        eq("s7: the independent probe computes the same ceilings 174762 / 87382 / 262144",
           (st[2], st[5], st[6]), (174762, 87382, 262144))

    # ---- section 8: the runaway, measured in a child process
    eq("s8: section 7's prediction is restated as 48 bytes and 174762 calls", d.get("s8_pred"), (48, 174762))
    eq("s8: the child measured its own runaway stride at 32 bytes per call", d.get("s8_stride"), 32)
    eq("s8: the predicted ceiling is 8388608 / 32 = 262144 calls", d.get("s8_ceiling"), (8388608, 32, 262144))
    died = (d.get("s8_died") or 0)
    check("s8: the measured death depth is within 1 percent of the 262144 prediction (a sample)",
          262144 - died >= 0 and (262144 - died) / 262144 < 0.01,
          "death depth %r is outside 1 percent of 262144" % died)
    check("s8: the death depth is a plausible magnitude, not a placeholder",
          250000 <= died <= 262144, "death depth %r" % died)
    diff = d.get("s8_diff") or (0, 0.0, 0, 0, 0)
    eq("s8: the printed difference is death depth minus the ceiling", diff[0], died - 262144)
    check("s8: the printed percentage is that difference over the ceiling",
          abs(diff[1] - diff[0] * 100.0 / 262144) < 0.001, "diff %r percent %r" % (diff[0], diff[1]))
    eq("s8: the runaway frame is still 32 bytes in the difference line", diff[2], 32)
    eq("s8: the consumed stack is death depth x 32 / 1024 KiB", diff[3], died * 32 // 1024)
    eq("s8: the limit in that line is still 8192 KiB", diff[4], 8192)
    eq("s8: the handler exited with code 42 and waitpid reported no signal",
       (d.get("s8_exit"), d.get("s8_sig")), (42, 0))
    if indep:
        eq("s8: the independent probe computes the same runaway ceiling 262144",
           indep["stack"][6], 262144)

    # ---- section 9: the one line to remember
    eq("s9: the closing line charges 48 / 48 / 96 / 32 bytes for the four shapes",
       d.get("s9_strides"), (48, 48, 96, 32))
    check("s9: every stride in the closing line is a number measured in sections 2 and 8, not typed",
          d.get("s9_strides") == (d.get("s7_stride"), at(d, "shape_fact", 2),
                                  at(d, "shape_hanoi", 2), d.get("s8_stride")))


def checks_multirun(env):
    """Three fresh runs: the invariant lines must be identical in all three."""
    runs = env["runs"]
    if len(runs) < 3:
        check("fresh: the lesson was run three times", False, "%d runs" % len(runs))
        return
    masked = [[mask_line(l) for l in r.splitlines() if l.strip()] for r in runs]
    check("fresh: every invariant line is identical across three fresh runs",
          masked[0] == masked[1] == masked[2],
          "run 2 or 3 differs at line %s" % next((i for i, (a, b) in
                                                  enumerate(zip(masked[0], masked[1])) if a != b), "?"))
    timings = [timing_tuple(r) for r in runs]
    check("fresh: the volatile slots (timings, death depth) were recorded in every run",
          all(t is not None for t in timings), "%r" % (timings,))
    note("three fresh runs: recursive walk %s ns, loop %s ns, naive fib(32) %s ms, "
         "memoised %s ns per call, death depth %s"
         % (["%.1f" % t[0] for t in timings], ["%.1f" % t[1] for t in timings],
            ["%.3f" % t[2] for t in timings], ["%.1f" % t[3] for t in timings],
            [t[4] for t in timings]))
    # the notes' claim about the spread of a timing, checked against what this machine does
    vals = [t[0] for t in timings]
    check("fresh: the recursive walk stays inside a sane band, so a timing claim is assertable",
          all(200 <= v <= 200000 for v in vals), "%r" % (vals,))


def timing_tuple(text):
    flat = flatten(text)
    m = re.search(r"recursion per walk: (\d+\.\d+) ns, loop per walk: (\d+\.\d+) ns", flat)
    n = re.search(r"naive calls = \d+ in (\d+\.\d+) ms", flat)
    p = re.search(r"per single fib\(32\) call: \d+\.\d+ ns naive against (\d+\.\d+) ns memoised", flat)
    s = re.search(r"reached depth (\d+)", flat)
    if not (m and n and p and s):
        return None
    return (float(m.group(1)), float(m.group(2)), float(n.group(1)), float(p.group(1)), int(s.group(1)))


def mask_line(line):
    """Hide only the genuinely volatile slots; every count, depth, stride and checksum stays visible.

    Addresses move with ASLR. The wall clock moves by nature. The guard-page death depth moves
    by a few hundred calls because the environment block above the stack does not have the same
    size twice. Everything else in this output is supposed to be reproducible, so it is compared
    byte for byte by the caller.
    """
    s = re.sub(r"0x[0-9a-fA-F]+", "0xADDR", line)
    s = re.sub(r" in \d+\.\d+ ms", " in <TIME> ms", s)
    s = re.sub(r"per single fib\(32\) call: \d+\.\d+ ns naive against \d+\.\d+ ns memoised",
               "per single fib(32) call: <T> ns naive against <T> ns memoised", s)
    s = re.sub(r"ratio = \d+\.\d+x, which is", "ratio = <R>x, which is", s)
    s = re.sub(r"a second per move is \d+\.\d+ years", "a second per move is <Y> years", s)
    s = re.sub(r"recursion per walk: \d+\.\d+ ns, loop per walk: \d+\.\d+ ns",
               "recursion per walk: <T> ns, loop per walk: <T> ns", s)
    s = re.sub(r"ratio = \d+\.\d+x: the difference", "ratio = <R>x: the difference", s)
    s = re.sub(r"took \d+ microseconds", "took <US> microseconds", s)
    s = re.sub(r"reached depth \d+", "reached depth <DEPTH>", s)
    s = re.sub(r"difference -?\d+ calls \(-?\d+\.\d+ percent\)", "difference <D> calls (<P> percent)", s)
    s = re.sub(r"that is \d+ KiB against", "that is <K> KiB against", s)
    s = re.sub(r"child pid \d+ is gone", "child pid <PID> is gone", s)
    return s


def capture_sections(text):
    lines = text.splitlines()
    a_start = b_start = exit_at = None
    for i, l in enumerate(lines):
        if l.startswith("## A.") and a_start is None:
            a_start = i + 1
        if l.startswith("## B.") and b_start is None:
            b_start = i + 1
        if l.startswith("## exit code:") and exit_at is None:
            exit_at = i
    a = [l for l in lines[a_start:exit_at] if l.strip()]
    a = [l for l in a if not l.startswith("###")]
    b = [l for l in lines[b_start:] if l.strip()] if b_start else []
    b = [l for l in b if not l.startswith("###")]
    return a, b, lines[exit_at] if exit_at is not None else ""


def checks_capture(env):
    cap = env["capture"]
    a, b, exitline = capture_sections(cap)
    check("capture: the file is sectioned into A (the lesson) and B (the preserved probe)",
          bool(a) and bool(b) and exitline == "## exit code: 0", exitline)
    check("capture: the header records the notes' build line and a zero warning build",
          ("# Build: " + BUILD_LINE) in cap and "zero warnings, zero errors" in cap)
    check("capture: the header records the machine and the capture script",
          "# Machine: " in cap and "# Captured by /tmp/run_lec16.sh on " in cap)
    check("capture: the header states the ASLR and allocation-counter caveats",
          "Addresses change between runs (ASLR)" in cap
          and "taken at the source" in cap.replace("counters are taken at the source", "taken at the source"))
    fresh = env["runs"][0] if env["runs"] else ""
    fa = [l for l in fresh.splitlines() if l.strip()]
    ma = [mask_line(l) for l in fa]
    ra = [mask_line(l) for l in a]
    check("capture: the recorded section A equals a FRESH run line for line once the "
          "volatile slots are masked (%d lines)" % len(ra), ma == ra,
          next(("line %d differs: recorded %r vs fresh %r" % (i, x, y)
                for i, (x, y) in enumerate(zip(ra, ma)) if x != y),
               "line count %d vs %d" % (len(ra), len(ma))))
    check("capture: no invariant number was changed by the masking",
          mask_line("  span across those frames = 336 bytes, so 48 bytes per recursive call")
          == "  span across those frames = 336 bytes, so 48 bytes per recursive call")
    # section B is a preserved earlier capture, byte for byte
    if env["prep"] is not None:
        pb = [l for l in env["prep"].splitlines() if l.strip()]
        check("capture: section B is the preserved 09_recursion_frames.cpp capture, byte for byte",
              b == pb, "%d lines vs %d; first difference %r" % (
                  len(b), len(pb),
                  next((x for x, y in zip(b, pb) if x != y), "none")))
    # the masked slots, asserted on the capture itself rather than against a fresh run
    cd = env["cap_parsed"]
    check("capture: the recorded death depth is inside the same 1 percent band",
          cd.get("s8_died") and (262144 - cd["s8_died"]) / 262144 < 0.01,
          "recorded death depth %r" % cd.get("s8_died"))
    check("capture: the recorded difference and KiB follow from that recorded depth",
          cd.get("s8_diff") and cd["s8_diff"][0] == cd["s8_died"] - 262144
          and cd["s8_diff"][3] == cd["s8_died"] * 32 // 1024,
          "diff %r, died %r" % (cd.get("s8_diff"), cd.get("s8_died")))
    check("capture: the recorded timings are one sample each and stay in band",
          cd.get("s5_times") is not None and 200 <= cd["s5_times"][0] <= 200000
          and cd.get("fib32_naive") and 1.0 <= cd["fib32_naive"][1] <= 100000.0,
          "recorded timings %r %r" % (cd.get("s5_times"), cd.get("fib32_naive")))

# ======================================================================================
# the independent probe, and the notes and diagrams that quote the numbers
# ======================================================================================

def checks_probe(env):
    check("probe: the independent program compiles with zero warnings under the same flags",
          env["indep_ok"] and not env["indep_warnings"],
          env["indep_warnings"][0][:200] if env["indep_warnings"] else "did not build")
    indep = env["indep"]
    check("probe: the independent program ran and reported every block it owns",
          bool(indep) and len(indep.get("fact", {})) == 8 and len(indep.get("fib", {})) >= 3
          and indep.get("memo") and indep.get("tree") and indep.get("stack"),
          "parsed %s" % sorted(indep)[:5])
    # the one number the lesson never prints and diagram 03 claims
    if indep.get("fib"):
        eq("probe: the deepest fib(4) stack is 4 frames, not 5 (diagram 03 measures this)",
           indep["fib"][4][2], 4)
        eq("probe: the deepest fib(32) stack is 32 frames, not 33 (diagram 03 measures this)",
           indep["fib"][32][2], 32)
        check("probe: the fib depth equals n for the naive recurrence with the n < 2 base case",
              indep["fib"][32][2] == 32 and indep["fib"][4][2] == 4 and indep["fib"][5][2] == 5,
              "%r" % {k: v[2] for k, v in indep["fib"].items()})
        eq("probe: the fib(4) call tree has 9 calls, the number diagram 03 draws",
           indep["fib"][4][1], 9)
    if indep.get("memo"):
        eq("probe: the memoised call count is 2n - 1 at n = 32, so 63 and never 65",
           indep["memo"][1], 2 * 32 - 1)
    if indep.get("tree"):
        eq("probe: a complete binary tree of depth 9 has 1023 nodes and 24 byte nodes",
           (indep["tree"][0], indep["tree"][1]), (1023, 24))
    if indep.get("hanoi20"):
        eq("probe: 2^20 - 1 hanoi moves", indep["hanoi20"][0], 1048575)


def md_tables(text):
    """Every pipe table in the document, as lists of rows of stripped cells, header first."""
    tables, cur = [], []
    for line in text.splitlines():
        s = line.rstrip()
        if s.startswith("|") and s.endswith("|"):
            cur.append([c.strip() for c in s.strip("|").split("|")])
        elif cur:
            tables.append(cur)
            cur = []
    if cur:
        tables.append(cur)
    return tables


def md_table(text, header0):
    """The first table whose header row's first cell is header0."""
    key = header0.strip().strip("`*_ ").lower()
    for rows in md_tables(text):
        if rows and rows[0] and rows[0][0].strip("`*_ ").lower() == key:
            return rows
    return None


def md_row(text, label, table=None):
    """The cells of the first row whose first cell is exactly label, optionally inside a table."""
    key = label.strip().strip("`*_ ").lower()
    for rows in ([table] if table else md_tables(text)):
        for cells in rows:
            if cells and cells[0].strip("`*_ ").lower() == key:
                return cells
    return None


mdrow = md_row


def cellnum(cells, idx):
    if not cells or idx >= len(cells):
        return None
    m = re.search(r"-?\d+(?:\.\d+)?", cells[idx].replace(",", ""))
    return float(m.group(0)) if m else None


# every closed form, percentage, ceiling and ratio the notes assert in prose, recomputed here
NOTES_ARITH = [
    ("N1: 6 x 48 = 288 bytes held at the deepest point",
     6 * 48, "6 frames \u00d7 48 bytes = 288 bytes held at the deepest point"),
    ("N2: 8 frames have 7 gaps of 48, which is 336", 7 * 48, "7 \u00d7 48 = 336"),
    ("N2: charging every live frame gives 8 x 48 = 384", 8 * 48, "8 \u00d7 48 = 384 bytes"),
    ("N3: ceil(2097152 / 48) = 43691 frames", -(-2097152 // 48), "2097152 \u00f7 48 \u2309 = 43691 frames"),
    ("N4: 2 x 2584 - 1 = 5167", 2 * 2584 - 1, "2 \u00d7 2584 - 1 = 5167"),
    ("N5: 17711 / 150049 = 0.1180, about 11.8 percent", round(17711 / 150049, 4), "17711 \u00f7 150049 = 0.1180"),
    ("N6: 7049155 / 63 = 111891.35", round(7049155 / 63, 2), "7049155 \u00f7 63 = 111891.35"),
    ("N6: (1 - 63/7049155) x 100 = 99.9991 percent",
     round((1 - 63 / 7049155) * 100, 4), "99.9991% of the calls removed"),
    ("N7: the memo calls for n = 32 are 2n - 1 = 63", 2 * 32 - 1,
     "2n - 1 calls; for n = 32 that is 33 entries and 63 calls"),
    ("N8: hanoi(9) is 2^9 - 1 = 511 moves and 511 calls", 2 ** 9 - 1, "2^9 - 1 = 511 moves and 511 calls"),
    ("N9: 1048575 / 31536000 = 0.0333 years", round(1048575 / 31536000, 4), "1048575 \u00f7 31536000 = 0.0333 years"),
    ("N9: that is about 12 days", round(1048575 / 86400), "about 12 days"),
    ("N10: 64 x 96 = 6144 bytes", 64 * 96, "64 \u00d7 96 = 6144 bytes"),
    ("N11: depth 3 gives 2^4 - 1 = 15 nodes", 2 ** 4 - 1, "depth 3 gives 2^4 - 1 = 15 nodes"),
    ("N12: 24552 / 1023 = 24 bytes per node", 24552 // 1023, "24552 \u00f7 1023 = 24 bytes per node"),
    ("N13: 1023 x 1024 / 2 = 523776", 1023 * 1024 // 2, "1023 \u00d7 1024 \u00f7 2 = 523776"),
    ("N13: 1023 x 1024 = 1047552", 1023 * 1024, "1023 \u00d7 1024 = 1047552"),
    ("N14: ceil(8388608 / 96) = 87382 calls", -(-8388608 // 96), "96 \u2309 = 87382 calls"),
    ("N15: 8388608 / 32 = 262144 calls", 8388608 // 32, "8388608 \u00f7 32 = 262144 calls"),
    ("N18: 13 x 479001600 = 6227020800", 13 * 479001600, "13 \u00d7 479001600 = 6227020800"),
    ("N18: 2147483647 is the signed 32 bit maximum", 2 ** 31 - 1, "2147483647"),
    ("N20: T(20) = 2^20 - 1 = 1048575", 2 ** 20 - 1, "T(20) = 2^20 - 1 = 1048575"),
    ("N22: 33 x 8 = 264 bytes for the memo table", 33 * 8, "33 \u00d7 8 = 264 bytes"),
    ("N22: 7049155 x 32 = 225572960 bytes", 7049155 * 32, "7049155 \u00d7 32 = 225572960 bytes"),
    ("N23: 500 x 48 = 24000 bytes", 500 * 48, "500 \u00d7 48 = 24000 bytes"),
    ("N24: 2^3 - 1 = 7 moves", 2 ** 3 - 1, "2^3 - 1 = 7"),
    ("N25: 48 - 32 = 16 bytes of stride difference", 48 - 32, "differ by 16 bytes"),
]


def checks_notes(env):
    notes = env["notes"]
    d = env["parsed"]
    cd = env["cap_parsed"]
    indep = env["indep"]
    check("notes: RECURSION-STUDY-NOTES.md exists and is non-trivial (%d bytes)" % len(notes.encode()),
          len(notes.encode()) >= 20000, "%d bytes" % len(notes.encode()))
    check("notes: no em dash anywhere (user absolute rule)", EMDASH not in notes)
    check("notes: no emoji", has_emoji(notes) is None, str(has_emoji(notes)))
    check("notes: the header documents the build, the machine, the capture and this verifier",
          BUILD_LINE in notes and "| Warnings from that build | 0 |" in notes
          and "Linux 7.0.0-31-generic x86_64, g++ 15.2.0, Python 3.14.4" in notes
          and "~/learning/cpp/exercises/lec16_recursion_output.txt" in notes
          and "python3 ~/learning/cpp/exercises/verify_recursion_lec16.py" in notes)
    for name in DIAGRAM_NAMES:
        check("notes: the header and the folder table both name diagrams/%s" % name,
              notes.count("diagrams/" + name) >= 2, "%d mentions" % notes.count("diagrams/" + name))
    # ---- section 3: the frame table and the factorial table
    eq("notes s3: linearSumRec is charged 48 bytes per call",
       cellnum(mdrow(notes, "`linearSumRec(n)`"), 2), 48.0)
    eq("notes s3: factorialRec is charged 48 bytes per call",
       cellnum(mdrow(notes, "`factorialRec(n)`"), 2), 48.0)
    eq("notes s3: hanoi is charged 96 bytes per call",
       cellnum(mdrow(notes, "`hanoi(n, from, to, via)`"), 2), 96.0)
    eq("notes s3: the runaway is charged 32 bytes per call",
       cellnum(mdrow(notes, "the runaway in section 9"), 2), 32.0)
    rows = [tuple(int(x) for x in m)
            for m in re.findall(r"(?m)^\| (\d+) \| (\d+) \| (\d+) \| (\d+) \|$", notes)]
    eq("notes s3: the factorial table quotes the same six rows the program printed",
       rows, [(1, 1, 1, 1), (2, 2, 2, 2), (3, 6, 3, 3), (4, 24, 4, 4), (5, 120, 5, 5), (6, 720, 6, 6)])
    check("notes s3: factorial(8) is quoted as 40320 with 8 calls and 8 x 48 = 384 bytes",
          "`factorial(8) = 40320`" in notes and "**8** calls" in notes and "8 \u00d7 48 = 384" in notes)
    check("notes s3: the span is quoted as 336 bytes, 7 gaps of 48 and not 8 frames of 48",
          "**336 bytes**" in notes and "7 gaps of 48" in notes and "not 8" in notes)
    eq("notes s3: the quoted 336 byte span is the measured span", 336.0,
       float(re.search(r"span \*\*(\d+) bytes\*\*", notes).group(1)) if re.search(r"span \*\*(\d+) bytes\*\*", notes) else None)
    # ---- section 4: fibonacci
    fibrows = [tuple(int(x) for x in m) for m in
               re.findall(r"(?m)^\| (\d+) \| (\d+) \| (\d+) \| (\d+) \| (\d+) \|$", notes)]
    eq("notes s4: the fib table quotes the three measured rows",
       fibrows, [(10, 55, 177, 21, 177), (17, 1597, 5167, 610, 5167), (24, 46368, 150049, 17711, 150049)])
    eq("notes s4: the naive fib(32) cell quotes the capture's 7049155 calls",
       cellnum(mdrow(notes, "Naive recursion"), 2), 7049155.0)
    msnotes = mdrow(notes, "Naive recursion")
    ms_cell = msnotes[3] if msnotes else ""
    ms_cap = cd.get("fib32_naive")[1] if cd.get("fib32_naive") else None
    check("notes s4: the millisecond cell is the capture's own number, not an earlier run's",
          ms_cap is not None and "%.3f ms" % ms_cap in ms_cell,
          "notes say %r, the capture holds %.3f ms" % (ms_cell, ms_cap if ms_cap else -1))
    eq("notes s4: the memoised row quotes the capture's 63 calls",
       cellnum(mdrow(notes, "Recursion with a memo table"), 2), 63.0)
    eq("notes s4: the fib(3) recompute count is quoted as 832040",
       cellnum(mdrow(notes, "fib(3) computed inside that single fib(32) call"), 1), 832040.0)
    eq("notes s4: the memo table entries are quoted as 33 = n + 1",
       cellnum(mdrow(notes, "Memo table entries"), 1), 33.0)
    eq("notes s4: the call count reduction is quoted as 111891x",
       cellnum(mdrow(notes, "Call count reduction"), 1), 111891.0)
    check("notes s4: the per-call row quotes the capture's ns pair and ratio verbatim",
          cd.get("fib32_percall") and cd.get("fib32_ratio")
          and ("%.1f ns against %.1f ns" % cd["fib32_percall"]) in notes
          and ("%.2f" % cd["fib32_ratio"][0]) in notes,
          "notes row %r" % (mdrow(notes, "Naive against memoised"),))
    check("notes s4: the memo timing evidence row is the checksum equation",
          "20000 runs with a fresh table, checksum 43566180000 = 20000 \u00d7 2178309" in notes)
    check("notes s4: the prose says 111891 of every 111892 calls are removed, which is 1 - 63/7049155",
          "111891 of every 111892" in notes and 111891 == 7049155 // 63 and 111892 == 7049155 // 63 + 1)
    # ---- section 5: hanoi
    hanrows = [tuple(int(x) for x in m) for m in
               re.findall(r"(?m)^\| (\d+) \| (\d+) \| (\d+) \| (\d+) \| (\d+) \| (\d+) \|$", notes)]
    eq("notes s5: the hanoi table quotes moves = calls = 2^n - 1 with frames = n",
       hanrows, [(n, 2 ** n - 1, 2 ** n - 1, 2 ** n - 1, 2 ** n - 1, n) for n in range(1, 7)])
    check("notes s5: calls for n = 6 and n = 20 are quoted as 63 and 1048575",
          "| Calls for n = 6 | 63, the same as the moves |" in notes
          and "2^20 - 1 = 1048575, the same as the moves" in notes)
    check("notes s5: the exponential/linear split is stated as O(2^n) work with O(n) stack",
          "| Stack cost | n frames, so O(n) |" in notes and "| Work | 2^n - 1 moves, so O(2^n) |" in notes)
    # ---- section 6: recursive against iterative
    for label, want in (("`factorial(12)`", "479001600"), ("sum 1..1000", "500500"), ("`fib(32)`", "2178309")):
        row = mdrow(notes, label)
        check("notes s6: %s is quoted as %s on both sides" % (label, want),
              row and row[1] == want and row[2] == want, "%r" % (row,))
    check("notes s6: the closed form row quotes 1000 x 1001 / 2 = 500500",
          "1000 \u00d7 1001 / 2 = 500500" in notes)
    walks = md_table(notes, "Walk")
    trow = md_row(notes, "Recursive `linearSumRec`", table=walks)
    orow = md_row(notes, "Iterative loop", table=walks)
    crow = md_row(notes, "Expected by the closed form", table=walks)
    eq("notes s6: the recursive walk row quotes the capture's 1054.1 ns and 250500000 checksum",
       (cellnum(trow, 1), cellnum(trow, 2)), (1054.1, 250500000.0))
    eq("notes s6: the loop row quotes the capture's 101.6 ns and 250500000 checksum",
       (cellnum(orow, 1), cellnum(orow, 2)), (101.6, 250500000.0))
    check("notes s6: the closed form row quotes 2000 x 500 x 501 / 2 = 250500000 and it is right",
          "2000 \u00d7 500 \u00d7 501 / 2 = 250500000" in notes and 2000 * 500 * 501 // 2 == 250500000)
    check("notes s6: the ratio quoted in the table is the same one the capture printed",
          cd.get("s5_ratio") and ("**%.2f\u00d7**" % cd["s5_ratio"]) in notes
          and abs(cd["s5_ratio"] - 1054.1 / 101.6) < 0.01,
          "capture ratio %r" % (cd.get("s5_ratio"),))
    check("notes s6: the two numbers the notes tell you never to trust as exact are named",
          "treat any single one as a sample" in notes and "the runaway depth both move between runs" in notes)
    check("notes s6: the recorded death depth is quoted as the capture's 261579 with the earlier "
          "run's 261640 named as such and both placed inside 0.3 percent of 262144",
          "died at depth 261579 there against 261640 on an earlier\nrun" in notes
          and "both within 0.3% of the 262144 ceiling" in notes
          and abs(261579 - 262144) / 262144 < 0.003 and abs(261640 - 262144) / 262144 < 0.003)
    check("notes s6: the checksum pair is named as the stable thing to quote",
          "Quote the\nchecksums (250500000, 3817763271)" in notes)
    check("notes s6: the spread paragraph names the three samples the same binary printed",
          "886.7, 1054.1 and 2657.0 ns" in notes and "about 3x" in notes)
    # ---- section 7: the heap tree
    nodes_cell = md_row(notes, "Nodes")
    check("notes s7: Nodes is quoted as the closed form 2^10 - 1 = 1023",
          bool(nodes_cell) and nodes_cell[1] == "2^10 - 1 = 1023" and 2 ** 10 - 1 == 1023,
          "%r" % (nodes_cell,))
    for label, want in (("Heap requests made by the recursive build", 1023.0),
                        ("Bytes requested", 24552.0), ("Bytes per node", 24.0),
                        ("`sizeof(TreeNode{int; TreeNode*; TreeNode*})`", 24.0),
                        ("Nodes visited by recursive inorder", 1023.0),
                        ("Nodes visited with an explicit stack", 1023.0),
                        ("Sum of all node values", 523776.0),
                        ("Free calls made by the recursive destroy", 1023.0),
                        ("Usable bytes for that 24 byte request", 24.0)):
        eq("notes s7: %s is quoted as %g" % (label, want), cellnum(mdrow(notes, label), 1), want)
    check("notes s7: the offsets are quoted as 8 and 16", "| 8, 16 |" in notes)
    check("notes s7: the closed form row quotes 1023 x 1024 / 2 = 523776",
          "1023 \u00d7 1024 / 2 = 523776" in notes)
    check("notes s7: the allocation counters are stated to be taken at the source, not by "
          "overriding operator new",
          "Counters are taken at the source" in notes and "is deliberately not overridden" in notes)
    # ---- section 8: the recursion against the loop table
    check("notes s8: the recursion-against-loop table quotes 48 to 96 bytes per call, the measured range",
          "measured 48 to 96 per call" in notes
          and [48, 96] == [d.get("s7_stride"), at(d, "shape_hanoi", 2)])
    # ---- section 9: the limit
    check("notes s9: the stack limit is quoted as 8192 KiB = 8388608 bytes",
          "| Stack limit | 8192 KiB | 8388608 bytes |" in notes)
    check("notes s9: the stride, ceiling and safe depth are quoted as 48, 174762 and 87381",
          "48 bytes | measured over 8 frames" in notes and "174762 calls" in notes
          and "depth 87381" in notes and 174762 // 2 == 87381)
    check("notes s9: the checksum row quotes 3817763271 with its closed form and says it matches",
          "3817763271" in notes and "87381 \u00d7 87382 / 2 = 3817763271" in notes and "matches" in notes)
    check("notes s9: the stack used and the time are quoted as 4095 KiB and 23 microseconds",
          "| Stack used by that run | 4095 KiB | 87381 \u00d7 48 |" in notes and "23 \u00b5s" in notes)
    rrow = mdrow(notes, "Runaway frame stride, measured inside the child")
    eq("notes s9: the runaway stride row quotes 32 bytes", cellnum(rrow, 1), 32.0)
    check("notes s9: the predicted ceiling is quoted as 8388608 / 32 = 262144 calls",
          "8388608 \u00f7 32 = 262144 calls" in notes)
    died_note = cellnum(mdrow(notes, "Measured death depth"), 1)
    eq("notes s9: the death depth row quotes the capture's own depth, not a stale run",
       died_note, float(cd.get("s8_died") or -1))
    check("notes s9: the error row quotes the capture's difference and its rounded percentage",
          bool(cd.get("s8_diff")) and ("%d calls, -0.216%%" % cd["s8_diff"][0]) in notes
          and round(abs(cd["s8_diff"][1]), 3) == 0.216,
          "capture diff %r" % (cd.get("s8_diff"),))
    check("notes s9: the prose makes the 0.216 percent argument and the two derived KiB figures",
          "A 0.216% error is the whole argument" in notes
          and "the safe side at 4095 KiB and the fatal side\nat 8174 KiB" in notes)
    check("notes s9: the child ended with code 42 and no signal, as the capture shows",
          "exited with code 42" in notes and cd.get("s8_sig") == 0)
    # ---- the answer key: every closed form recomputed here
    for label, value, needle in NOTES_ARITH:
        check("notes arithmetic: %s" % label, needle in notes,
              "the notes do not contain %r" % needle)
        if isinstance(value, float) and value == int(value):
            value = int(value)
        forms = [str(value)] if isinstance(value, int) else [
            "%g" % value, "%.4f" % value, "%.3f" % value, "%.2f" % value]
        check("notes arithmetic: %s, recomputed here as %s" % (label, value),
              any(f in needle for f in forms), "recomputed %r, none of %r in the quote" % (value, forms))
    check("notes arithmetic: N2 and the C2 answer both state 7 x 48 = 336, so one copy cannot hide",
          notes.count("7 \u00d7 48 = 336") >= 2 and notes.count("8 \u00d7 48 = 384") >= 2,
          "7x48=336 appears %d times, 8x48=384 appears %d times"
          % (notes.count("7 \u00d7 48 = 336"), notes.count("8 \u00d7 48 = 384")))
    check("notes arithmetic: N16 |261579 - 262144| / 262144 rounds to 0.216 percent",
          round(abs(261579 - 262144) / 262144 * 100, 3) == 0.216)
    check("notes arithmetic: N17 1054.1 / 101.6 = 10.38 and 2.11 / 0.20 ns per term",
          abs(1054.1 / 101.6 - 10.38) < 0.01 and round(1054.1 / 500, 2) == 2.11
          and round(101.6 / 500, 2) == 0.20)
    check("notes arithmetic: N22 225572960 bytes is about 215 MiB",
          "about\n215 MiB" in notes and round(225572960 / 1048576) == 215)
    check("notes arithmetic: N23 24000 bytes is about 23.4 KiB and 24000000 is about 22.9 MiB",
          round(24000 / 1024, 1) == 23.4 and round(24000000 / 1048576, 1) == 22.9)
    check("notes arithmetic: N7 states 2n - 1 calls and never 2n + 1",
          "2n - 1 calls" in notes and "2n + 1" not in notes)
    check("notes arithmetic: the memo answers are consistent with the 63 the capture printed",
          "for n = 32 that is 33 entries and 63 calls" in notes and 2 * 32 - 1 == 63)
    note("the notes' answer key was recomputed line by line: every closed form, percentage and "
         "ceiling in it agrees with the measured numbers. Three defects were on disk when this "
         "verifier was first run and are now fixed: N7 said 2n + 1 calls (65 for n = 32) against "
         "the measured 63 = 2n - 1; section 4's table carried 17.006 ms from an earlier run while "
         "its own per-call row quoted the capture's 16731224.0 ns = 16.731 ms; and diagram 03 "
         "claimed a 5 frame fib(4) stack and a 33 frame fib(32) stack where the independent probe "
         "measures 4 and 32.")


def checks_diagrams(env):
    svgs = env["svgs"]
    d = env["parsed"]
    cd = env["cap_parsed"]
    indep = env["indep"]
    for name in DIAGRAM_NAMES:
        raw = svgs.get(name)
        root = None
        if not raw:
            check("diagrams: %s is on disk" % name, False, "missing")
            continue
        try:
            root = ET.fromstring(raw)
            tag = root.tag.split("}")[-1]
            ok = tag == "svg" and root.get("width") == "840" and root.get("viewBox") is not None
            check("diagrams: %s parses as XML, 840 wide, with a viewBox" % name, ok,
                  "tag=%s width=%s" % (tag, root.get("width")))
            check("diagrams: %s declares role=img and a Lecture 16 aria-label" % name,
                  root.get("role") == "img" and "Lecture 16" in (root.get("aria-label") or ""),
                  root.get("aria-label"))
        except ET.ParseError as exc:
            check("diagrams: %s parses as XML" % name, False, str(exc))
        check("diagrams: %s is non-trivial, free of emoji and free of em dash" % name,
              len(raw) >= 3000 and has_emoji(raw) is None and EMDASH not in raw,
              "%d bytes, emoji %r" % (len(raw), has_emoji(raw)))
        text = " ".join((t.text or "") for t in root.iter() if t.tag.endswith("}text")) if root is not None else raw
        env["svg_text"][name] = text

    t1 = env["svg_text"].get(DIAGRAM_NAMES[0], "")
    if t1:
        check("svg01: titles itself as the call stack growing for factorial(4)",
              "the call stack growing" in t1 and "factorial(4): four live frames, 48 bytes per frame" in t1,
              t1[:200])
        check("svg01: depth 1 is labelled as the first frame and depth 4 as the deepest",
              "depth 1, the first frame" in t1 and "depth 4, 144 bytes lower" in t1)
        for k, lower in ((2, 48), (3, 96), (4, 144)):
            check("svg01: depth %d is drawn %d bytes lower, which is %d x 48" % (k, lower, k - 1),
                  ("depth %d, %d bytes lower" % (k, lower)) in t1 and lower == (k - 1) * 48)
        check("svg01: the four factorial frames are labelled n = 4 down to n = 1",
              all(("factorial(%d)   n = %d" % (i, i)) in t1 for i in (1, 2, 3, 4)))
        check("svg01: the span prose says 144 bytes is 3 gaps of 48",
              "144 bytes, which is 3 gaps of 48 bytes" in t1 and 144 == 3 * 48)
        check("svg01: the charge prose says 4 x 48 = 192 and 240 for five frames",
              "4 x 48 = 192" in t1 and "240" in t1 and 4 * 48 == 192 and 5 * 48 == 240)
        check("svg01: the measured-facts box quotes factorial(8) with 8 calls, a 336 byte span "
              "and 48 bytes per call, all three from the run",
              "factorial(8): 8 calls, 336 byte span across the frames, 48 bytes per call" in t1
              and d.get("probe_frames") == (8, 7) and d.get("span") == (336, 48))
        check("svg01: the measured-facts box quotes hanoi at 96 bytes per call, the measured stride",
              "hanoi(n, from, to, via): 96 bytes per call" in t1
              and d.get("shape_hanoi", (0, 0, 0))[2] == 96)
        check("svg01: the deepest frame is stated to return first, in exact reverse order",
              "the n = 1 frame is the deepest and is the first to return: exact reverse order" in t1)
        check("svg01: the picture says where its numbers came from",
              "10_recursion_algorithms.cpp, sections 1 and 2" in t1 and "lec16_recursion_output.txt" in t1)

    t2 = env["svg_text"].get(DIAGRAM_NAMES[1], "")
    if t2:
        check("svg02: titles itself as the call stack unwinding for factorial(4)",
              "the call stack unwinding" in t2 and "factorial(4): frames leave in reverse order" in t2)
        check("svg02: four steps are drawn", t2.count("step ") == 4, "%d steps" % t2.count("step "))
        check("svg02: the value handed back is 1, then 2 x 1 = 2, then 3 x 2 = 6, then 4 x 6 = 24",
              "returns 1" in t2 and "returns 2 x 1 = 2" in t2 and "returns 3 x 2 = 6" in t2
              and "returns 4 x 6 = 24" in t2 and 2 * 1 == 2 and 3 * 2 == 6 and 4 * 6 == 24)
        check("svg02: one frame disappears per step and six frames are marked gone",
              t2.count("gone") == 6, "%d gone" % t2.count("gone"))
        check("svg02: the summary quotes the measured factorial(4) row: 24 from 4 calls, 4 frames",
              "factorial(4) = 24, measured: 4 calls, 4 frames, one frame per value of n" in t2
              and (d.get("fact_rows") or [None] * 4)[3] == (4, 24, 4, 4))
        check("svg02: creation order f(4), f(3), f(2), f(1) and exact reverse release",
              "created as f(4), f(3), f(2), f(1) and released in the exact reverse order" in t2)
        check("svg02: only the return value leaves the stack",
              "nothing is copied out of the stack except the return value" in t2)
        check("svg02: the picture says where its numbers came from",
              "10_recursion_algorithms.cpp, sections 1 and 9" in t2 and "lec16_recursion_output.txt" in t2)

    t3 = env["svg_text"].get(DIAGRAM_NAMES[2], "")
    if t3:
        check("svg03: the tree is exponential and the stack is not, as the title says",
              "the call tree is exponential, the stack is not" in t3)
        check("svg03: the subtitle claims 9 calls in the tree, which is the measured fib(4) count",
              "fib(4): 9 calls in the tree" in t3
              and (indep.get("fib", {}).get(4, (0, 0, 0, 0, 0))[1] if indep.get("fib") else None) == 9)
        check("svg03: the subtitle claims the measured stack depth for fib(4), which is 4 frames",
              "only 4 frames deep at once" in t3
              and (indep.get("fib", {}).get(4, (0, 0, 0, 0, 0))[2] if indep.get("fib") else None) == 4,
              "the diagram says 5 frames deep, the probe measures 4")
        check("svg03: the stack column draws exactly four live frames, not five",
              t3.count("frame ") == 4 and "frame 5" not in t3 and "f(0) base" not in t3,
              "%d frame labels, frame 5 present: %s" % (t3.count("frame "), "frame 5" in t3))
        check("svg03: the stack column says 4 frames alive and 9 calls in the tree",
              "4 frames alive, 9 calls in the tree" in t3)
        check("svg03: the call tree is drawn with 9 nodes, one per call the probe counts",
              svgs.get(DIAGRAM_NAMES[2], "").count("<circle") == 9,
              "%d circles" % svgs.get(DIAGRAM_NAMES[2], "").count("<circle"))
        check("svg03: the two red f(2) nodes are named as the same subproblem solved twice",
              "the two red f(2) nodes are the same subproblem, solved twice" in t3)
        check("svg03: the measured table quotes the capture's naive call count 7049155",
              "7049155" in t3 and bool(cd.get("fib32_naive")) and int(cd["fib32_naive"][0]) == 7049155)
        check("svg03: the measured table quotes 63 memoised calls over a table of 33 entries",
              "63, using a table of 33 entries" in t3)
        check("svg03: the measured table quotes 832040 recomputes of fib(3)",
              "832040" in t3)
        check("svg03: the measured table quotes the measured fib(32) stack depth, which is 32 frames",
              "32 frames, one per value of n" in t3
              and (indep.get("fib", {}).get(32, (0, 0, 0, 0, 0))[2] if indep.get("fib") else None) == 32,
              "the diagram says 33 frames, the probe measures 32")
        check("svg03: the footer states the tree grows like Fib(n+1) while the stack grows like n",
              "The tree grows like Fib(n+1) in calls while the stack grows like n" in t3)
        check("svg03: the picture says where its numbers came from",
              "10_recursion_algorithms.cpp section 3" in t3 and "lec16_recursion_output" in t3)


def checks_self(env):
    me = Path(__file__).read_text(encoding="utf-8")
    check("verifier: it lives at the path the notes promise "
          "(~/learning/cpp/exercises/verify_recursion_lec16.py)",
          Path(__file__).resolve() == SELF.resolve(), str(Path(__file__).resolve()))
    check("verifier: no em dash in this file (user absolute rule)", EMDASH not in me)
    check("verifier: it masks ASLR addresses and timings rather than ignoring them",
          "0xADDR" in me and "<TIME>" in me and "<DEPTH>" in me)
    check("verifier: at least 150 checks ran", len(RESULTS) + 1 >= 150, "%d so far" % len(RESULTS))


def chk_python_ground_truth():
    """Independent arithmetic on this machine, so the notes' ceilings are not self-referential."""
    check("python: 8388608 / 48 = 174762 and 8388608 / 32 = 262144 and 8388608 / 96 rounds to 87382",
          8388608 // 48 == 174762 and 8388608 // 32 == 262144 and -(-8388608 // 96) == 87382)
    check("python: 87381 x 87382 / 2 = 3817763271", 87381 * 87382 // 2 == 3817763271)
    check("python: 2 * Fib(33) - 1 = 7049155, the naive fib(32) call count", 2 * fib(33) - 1 == 7049155)
    check("python: Fib(30) = 832040, the number of times fib(3) is recomputed inside fib(32)",
          fib(30) == 832040)
    check("python: 2 * Fib(18) - 1 = 5167 and 2 * Fib(11) - 1 = 177",
          2 * fib(18) - 1 == 5167 and 2 * fib(11) - 1 == 177)
    check("python: the fib(4) call tree is 2 * Fib(5) - 1 = 9 nodes", 2 * fib(5) - 1 == 9)
    check("python: hanoi(6) is 2^6 - 1 = 63 moves and hanoi(20) is 1048575", 2 ** 6 - 1 == 63 and 2 ** 20 - 1 == 1048575)
    check("python: hanoi(20) at a move per second is 0.03325 years and 12 days",
          round(1048575 / 3600 / 24 / 365, 5) == 0.03325 and round(1048575 / 86400) == 12)
    check("python: a depth 9 tree has 2^10 - 1 = 1023 nodes and their values sum to 523776",
          2 ** 10 - 1 == 1023 and sum(range(1, 1024)) == 523776)


def fib(n):
    a, b = 0, 1
    for _ in range(n):
        a, b = b, a + b
    return a


# ======================================================================================
# one full pass over every artifact, replayable against a mutated copy
# ======================================================================================

def run_check_set(env):
    checks_build(env)
    checks_run(env)
    checks_multirun(env)
    checks_probe(env)
    checks_capture(env)
    checks_notes(env)
    checks_diagrams(env)
    chk_python_ground_truth()
    checks_self(env)


def refresh(env):
    env["parsed"] = parse_program(env["runs"][0]) if env["runs"] else {}
    env["cap_parsed"] = parse_program(env["capture"])
    env["indep"] = parse_probe(env["probe_out"]) if env["indep_ok"] else {}
    return env


def read_env_file(path):
    return path.read_text(encoding="utf-8") if path.exists() else None


def make_env(tmpdir, runs=3):
    ok, warns, err, exe = build(LESSONS, PROGRAM.name, tmpdir, "lec16_prog")
    outs, run_ok = [], ok
    if ok:
        for _ in range(runs):
            try:
                cp = run_exe(exe)
            except subprocess.TimeoutExpired:
                run_ok = False
                break
            if cp.returncode != 0:
                run_ok = False
            outs.append(cp.stdout)
    pdir = Path(tmpdir) / "probe"
    pdir.mkdir(exist_ok=True)
    (pdir / "indep_probe.cpp").write_text(PROBE_SRC, encoding="utf-8")
    pok, pwarns, perr, pexe = build(pdir, "indep_probe.cpp", pdir, "indep_probe")
    pout = run_exe(pexe).stdout if pok else ""
    env = {
        "program": PROGRAM.read_text(encoding="utf-8"),
        "build": {"ok": ok, "warnings": warns, "stderr": err},
        "runs": outs,
        "run_ok": run_ok,
        "indep_ok": pok,
        "indep_warnings": pwarns,
        "indep_stderr": perr,
        "probe_out": pout,
        "capture": CAPTURE.read_text(encoding="utf-8"),
        "prep": read_env_file(PREP),
        "notes": NOTES.read_text(encoding="utf-8"),
        "svgs": {n: (DIAGRAMS / n).read_text(encoding="utf-8") for n in DIAGRAM_NAMES
                 if (DIAGRAMS / n).exists()},
        "svg_text": {},
    }
    return refresh(env)


# ======================================================================================
# the mutation battery: a verifier that cannot fail is not a verifier
# ======================================================================================
# Mutations that have to be REBUILT to mean anything: the fault is in the C++ and the
# verifier only sees it by compiling the mutant and reading what it prints.
REBUILD_MUTATIONS = [
    ("the factorial(8) frame count is inflated by one",
     "int factFrames = recordedFrames(g_factAddr);", "int factFrames = recordedFrames(g_factAddr) + 1;"),
    ("the hanoi check column becomes 2^n - 2, so its own check column must disagree",
     "long long closed = pow2ll(n) - 1;", "long long closed = pow2ll(n) - 2;"),
    ("the fib(3) counter counts fib(4) instead",
     "if (n == 3) g_fib3Hits++;", "if (n == 4) g_fib3Hits++;"),
    ("the memo table is one slot too large",
     "vector<long long> memo((size_t)FIB_N + 1, -1);", "vector<long long> memo((size_t)FIB_N + 2, -1);"),
    ("the tree loses its deepest level",
     "const int TREE_DEPTH = 9;", "const int TREE_DEPTH = 8;"),
    ("the timed walk runs 1999 reps instead of 2000",
     "const int REPS = 2000;", "const int REPS = 1999;"),
    ("the safe run becomes a quarter of the ceiling instead of half",
     "long long safeDepth = ceiling / 2;", "long long safeDepth = ceiling / 4;"),
    ("the child credits itself 16 more bytes per runaway frame",
     "report[1] = g_runawayStride;", "report[1] = g_runawayStride + 16;"),
    ("an unused local is left in main, so -Wall speaks",
     'int main() {\n    cout << "=== 1. factorial', 'int main() {\n    int zz = 5;\n    cout << "=== 1. factorial'),
    ("the frame probe stops recording after two frames, so the stride is not a wrapper",
     "static const int MAXPROBE = 64;", "static const int MAXPROBE = 2;"),
]

# Mutations of the TEXT the verifier reads: the capture, the notes, the diagrams, the probe
# output and the program source as a document. No rebuild, just a replay of every check.
TEXT_MUTATIONS = [
    ("capture", r"8388608 / 48 = 174762", "8388608 / 48 = 174763", "the capture's ceiling is off by one"),
    ("capture", r"24552 bytes requested", "24551 bytes requested", "the capture's byte total moves"),
    ("capture", r"reached depth \d+", "reached depth 999999", "the recorded death depth leaves the band"),
    ("capture", r"tree is exponential, the stack is not", "tree is exponential, the stack is not", "no-op guard"),
    ("notes", r"2n - 1 calls", "2n + 1 calls", "N7 is reverted to 2n + 1, the defect that was on disk"),
    ("notes", r"16\.731 ms", "17.006 ms", "the stale millisecond sample is put back"),
    ("notes", r"\u2309 = 87382 calls", "\u2309 = 87381 calls", "N14's ceiling loses its rounding up"),
    ("notes", r"\| four arguments \| 96 \|", "| four arguments | 64 |", "the notes' hanoi stride is wrong"),
    ("notes", r"checksum 43566180000 = 20000 \u00d7 2178309", "checksum 43566180000 = 20000 \u00d7 2178308",
     "the notes' memo checksum equation is wrong"),
    ("notes", r"7 \u00d7 48 = 336", "7 \u00d7 48 = 337", "N2's arithmetic is wrong"),
    ("notes", r"Quote the\nchecksums", "Quote the checksums", "the sampling warning loses its shape"),
    ("notes", r"died at depth 261579 there against 261640 on an earlier\s+run",
     "died at depth 261640 there against 261579 on an earlier run", "the notes' provenance is swapped"),
    ("svg01", r"48 bytes per call", "64 bytes per call", "diagram 01's stride is wrong"),
    ("svg01", r"4 x 48 = 192, and 240", "4 x 48 = 193, and 240",
     "diagram 01's charge arithmetic is wrong"),
    ("svg02", r"returns 4 x 6 = 24", "returns 4 x 6 = 23", "diagram 02's final value is wrong"),
    ("svg02", r"one frame per value of n", "one frame per two values of n", "diagram 02's frame count claim is wrong"),
    ("svg03", r"only 4 frames deep at once", "only 5 frames deep at once",
     "diagram 03's fib(4) depth is reverted to the defect that was on disk"),
    ("svg03", r"4 frames alive, 9 calls in the tree", "5 frames alive, 9 calls in the tree",
     "diagram 03's stack column is inflated"),
    ("svg03", r"32 frames, one per value of n", "33 frames, one per value of n",
     "diagram 03's fib(32) depth is reverted to the defect that was on disk"),
    ("svg03", r"7049155", "7049156", "diagram 03's naive call count is wrong"),
    ("svg03", r"63, using a table of 33 entries", "63, using a table of 34 entries", "diagram 03's table size is wrong"),
    ("probe", r"FIB n=4 v=3 calls=9 depth=4", "FIB n=4 v=3 calls=9 depth=5",
     "the independent measurement is flipped to agree with the old diagram"),
    ("prog", r"// Build: g\+\+ -Wall", "// build: g++ -Wall", "the documented build line is removed"),
    ("prog", r"#include <cstddef>", "void *operator new(unsigned long);\n#include <cstddef>",
     "an operator new overload is declared, which notes C6 forbids"),
]


def replay(env, what):
    """Run every check against this environment, collect the failures, restore the report."""
    global REPORTING, COLLECT
    saved_results = list(RESULTS)
    saved_reporting = REPORTING
    REPORTING = False
    COLLECT = []
    try:
        run_check_set(env)
        failed = [l for l, _ in COLLECT]
    except Exception as exc:  # a mutant that makes the verifier explode is caught too
        failed = ["the verifier raised %s: %s" % (type(exc).__name__, exc)]
    finally:
        COLLECT = None
        del RESULTS[:]
        RESULTS.extend(saved_results)
        REPORTING = saved_reporting
    return failed


def mutated_env(env, target, pattern, repl, tmpdir):
    """Return (applied, new_env): apply one text mutation and refresh everything derived from it."""
    env2 = dict(env)
    env2["svgs"] = dict(env["svgs"])
    env2["svg_text"] = {}
    if target == "capture":
        new, n = re.subn(pattern, lambda m: repl, env["capture"], count=1)
        if n == 0:
            return False, env2
        env2["capture"] = new
    elif target == "notes":
        new, n = re.subn(pattern, lambda m: repl, env["notes"], count=1)
        if n == 0:
            return False, env2
        env2["notes"] = new
    elif target == "probe":
        new, n = re.subn(pattern, lambda m: repl, env["probe_out"], count=1)
        if n == 0:
            return False, env2
        env2["probe_out"] = new
    elif target == "prog":
        new, n = re.subn(pattern, lambda m: repl, env["program"], count=1)
        if n == 0:
            return False, env2
        env2["program"] = new
    elif target.startswith("svg"):
        i = int(target[3:]) - 1
        name = DIAGRAM_NAMES[i]
        new, n = re.subn(pattern, lambda m: repl, env["svgs"][name], count=1)
        if n == 0:
            return False, env2
        env2["svgs"][name] = new
    else:
        return False, env2
    return True, refresh(env2)


def run_rebuild_mutation(env, what, old, new, tmpdir):
    src = env["program"]
    if old not in src:
        return False, ["the mutation did not apply (stale pattern)"]
    d = Path(tmpdir) / "mut"
    d.mkdir(exist_ok=True)
    (d / PROGRAM.name).write_text(src.replace(old, new, 1), encoding="utf-8")
    ok, warns, err, exe = build(d, PROGRAM.name, d, "mutant")
    outs, run_ok = [], ok
    if ok:
        for _ in range(3):          # the same three runs the clean program gets
            try:
                cp = run_exe(exe, timeout=120)
            except subprocess.TimeoutExpired:
                return True, ["the mutant hung, which the verifier counts as caught"]
            if cp.returncode != 0:
                run_ok = False
            outs.append(cp.stdout)
    env2 = dict(env)
    env2["program"] = (d / PROGRAM.name).read_text(encoding="utf-8")
    env2["build"] = {"ok": ok, "warnings": warns, "stderr": err}
    env2["runs"] = outs
    env2["run_ok"] = run_ok
    env2["svg_text"] = {}
    failed = replay(refresh(env2), what)
    if not ok:
        failed = failed + ["the mutant no longer compiles"]
    if warns:
        failed = failed + ["the mutant emits %d warning(s): %s"
                           % (len(warns), warns[0].split("warning:")[-1].strip()[:70])]
    return True, failed


def run_mutations(env):
    print("\n== MUTATION BATTERY: a verifier that cannot fail on a broken artifact is not a verifier ==")
    print("   %d faults are injected into COPIES of the C++ program (rebuilt and re-run), the"
          % len(REBUILD_MUTATIONS))
    print("   capture, the notes, the three diagrams, the probe output and the source text, then every")
    print("   check is replayed against the mutant and must fail by name.")
    caught, total, missed = 0, 0, []
    with tempfile.TemporaryDirectory(prefix="lec16mut_") as td:
        for what, old, new in REBUILD_MUTATIONS:
            total += 1
            sub = Path(td) / ("m%d" % total)
            sub.mkdir(parents=True)
            applied, failed = run_rebuild_mutation(env, what, old, new, sub)
            ok = applied and bool(failed)
            caught += ok
            if not ok:
                missed.append(what)
            print("  [%s] rebuild  %s" % ("CAUGHT" if ok else "MISSED", what))
            if failed:
                print("             -> %s" % failed[0][:150])
        for i, (target, pattern, repl, what) in enumerate(TEXT_MUTATIONS):
            if what.startswith("no-op"):
                continue
            total += 1
            sub = Path(td) / ("t%d" % i)
            sub.mkdir(parents=True)
            applied, env2 = mutated_env(env, target, pattern, repl, sub)
            if not applied:
                print("  [MISSED] %-8s %s   -> the pattern no longer matches (stale test)" % (target, what))
                missed.append(what)
                continue
            failed = replay(env2, what)
            ok = bool(failed)
            caught += ok
            if not ok:
                missed.append(what)
            print("  [%s] %-8s %s" % ("CAUGHT" if ok else "MISSED", target, what))
            if failed:
                print("             -> %s" % failed[0][:150])
    print("== MUTATION RESULT: %d/%d injected faults caught, every one by a NAMED check ==" % (caught, total))
    if missed:
        print("   MISSED: %s" % "; ".join(missed))
    print("   known blind spots, stated rather than hidden:")
    print("   - the volatile slots (ASLR addresses, timings, the guard-page death depth, the child")
    print("     pid) are masked in the capture freshness comparison, so a wrong digit that appears")
    print("     ONLY in a timing slot survives that comparison. The band checks test those directly,")
    print("     and the notes are only allowed to quote the capture's own sample.")
    print("   - the guard-page death depth is not reproducible to the call, so it is asserted within")
    print("     1 percent of 262144 and against its own difference and KiB arithmetic, never for")
    print("     equality with the capture.")
    print("   - notes C6's claim that GCC 15 emits -Wmismatched-new-delete for a naive operator new")
    print("     pair was NOT reproduced by this verifier on g++ 15.2.0; what it does reproduce is")
    print("     -Wsized-deallocation when only one delete overload is defined, and the source-level")
    print("     rule that the lesson does not override operator new at all.")
    return caught == total


def main():
    global REPORTING
    args = sys.argv[1:]
    verbose = "--verbose" in args
    do_mutate = "--no-mutate" not in args
    if verbose:
        globals()["_VERBOSE"] = True
    required = [PROGRAM, CAPTURE, PREP, NOTES] + [DIAGRAMS / n for n in DIAGRAM_NAMES]
    for path in required:
        if not path.exists():
            print("missing required file: %s" % path)
            return 2

    with tempfile.TemporaryDirectory(prefix="lec16verify_") as td:
        env = make_env(td)
        run_check_set(env)
        if do_mutate:
            mut_ok = run_mutations(env)
        else:
            mut_ok = True

    passed = sum(1 for _, st, _ in RESULTS if st == "PASS")
    failed = [(l, dd) for l, st, dd in RESULTS if st == "FAIL"]
    for label, detail in failed:
        print("  [FAIL] %s%s" % (label, "   " + detail if detail else ""))
    if REMARKS:
        print("\n== REMARKS (measured, not asserted) ==")
        for r in REMARKS:
            print("   %s" % r)
    print("\n== VERIFY: %d PASS, %d FAIL (%d checks) ==" % (passed, len(failed), len(RESULTS)))
    status = 1 if failed else 0
    if failed:
        print("   the artifact set above does NOT match its own measurements")
    if do_mutate and not mut_ok:
        status = 1
        print("   the mutation battery did NOT catch every injected fault, so this verifier is unproven")
    return status


if __name__ == "__main__":
    sys.exit(main())
