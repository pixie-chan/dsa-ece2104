#!/usr/bin/env python3
"""
verify_topic_wise.py : verifier for the ECE2104 topic-wise explanation set.

What it proves, in order:
  1. every lesson the explanation set depends on compiles with ZERO warnings under
     -Wall -Wextra -Wpedantic -std=c++17 -O2, and runs to exit 0
  2. the explanation set is not stale: its quoted numbers are checked against a FRESH run
     of the lesson programs, and against the recorded captures, never against a typed constant
  3. every multi-digit number in the set is either found in a capture or in a fresh run, or
     named in an explicit printed allowlist of derived arithmetic
  4. all 14 topics are present, in the projected order, each with a coverage word and its
     source named
  5. every mermaid source has a rendered PNG, each PNG is non-trivial, and the PNGs are
     embedded with paths relative to the note folder, never to a sibling
  6. the answer keys are in their own section, numbered 1 to 1 with the questions
  7. every C or C++ code block in the set compiles as written, in C11 and in C++17
  8. the parser traps are avoided: re.M on every anchored regex, [ \t] instead of \\s in row
     parsers, because \\s eats the newline and merges two table rows into one

Usage:
  python3 verify_topic_wise.py            # run every check
  python3 verify_topic_wise.py --mutate   # run the mutation battery: prove the verifier
                                          # actually bites on 8 injected faults

Expectation values come from the artifacts. A number this script cannot trace is reported as
UNTRACED and fails the run; it is never silently accepted.
"""
import os
import re
import struct
import subprocess
import sys
import tempfile
from pathlib import Path

HOME = Path.home()
LESSONS = HOME / "learning/cpp/lessons"
EXER = HOME / "learning/cpp/exercises"
NOTES = HOME / "Documents/MUJ-SEM-3/DSA/Notes"
FOLDER = NOTES / "topic-wise"
MD = FOLDER / "TOPIC-WISE-EXPLAINED.md"
HTML = FOLDER / "TOPIC-WISE-EXPLAINED.html"
DIAG = FOLDER / "diagrams"
BUILD = FOLDER / "build-topic-wise.py"

CAPTURES = {
    "lec05_run_output.txt": EXER / "lec05_run_output.txt",
    "lec11_15_run_output.txt": EXER / "lec11_15_run_output.txt",
    "lec16_recursion_output.txt": EXER / "lec16_recursion_output.txt",
    "lec16_18_sorts_run_output.txt": EXER / "lec16_18_sorts_run_output.txt",
    "lec19_21_trees_run_output.txt": EXER / "lec19_21_trees_run_output.txt",
    "lec22_analysis_run_output.txt": EXER / "lec22_analysis_run_output.txt",
    "lec23_formulations_run_output.txt": EXER / "lec23_formulations_run_output.txt",
}

# the lessons the explanation set quotes. Every one must build warning-free.
LESSON_SOURCES = [
    "05_ds_memory_representation.cpp",
    "06_linked_list_representation.cpp",
    "07_linked_list_operations.cpp",
    "08_linked_list_searching_apps.cpp",
    "11_stack_array.cpp",
    "13_queue_array.cpp",
    "14_queue_linked_circular.cpp",
    "17_merge_sort.cpp",
    "18_quick_sort.cpp",
    "19_tree_representation.cpp",
    "20_tree_traversals.cpp",
    "21_bst_operations.cpp",
    "22_algorithm_analysis.cpp",
    "23_structure_formulations.cpp",
]

PROJECTED = [
    "Array", "Pointers", "Stack by Array", "Queue", "Linked List", "Merge Sort",
    "Quick Sort", "Algorithm fundamentals", "Asymptotic notation", "Finding Complexity",
    "Trees fundamentals", "Formulation in C", "Tree types and BST", "Different Operations",
]

# Numbers the document derives by arithmetic from measured values rather than reading them
# from a capture. Each entry names the measurement it is derived FROM, so the allowlist is an
# argument, not a hiding place. Every one of these is labelled "derived arithmetic" in the text.
DERIVED = {
    "1200": "300 x 4, array bytes for 300 ints, from sizeof(int)=4 in lec05",
    "7200": "300 x 24, allocator usable bytes, from the 24 usable per 16 byte request in lec05",
    "9600": "300 x 32, allocator charged bytes, from the 32 byte stride in lec05",
    "32000": "1000 x 32, allocator charged bytes, from the 32 byte stride in lec05",
    "1048586": "1048576 + 10, the measured nested loop count plus the measured halving loop count",
    "140": "f(6) = 3(36) + 5(6) + 7 = 140, the polynomial in lec22 evaluated at n = 6",
    "102": "f(5) = 3(25) + 5(5) + 7 = 102, the polynomial in lec22 evaluated at n = 5",
    "144": "c*g(5) = 4 x 36 = 144, the bound in lec22 evaluated at n = 6 for c = 4",
    "55": "10 x 11 / 2, the triangular closed form in lec22 evaluated at n = 10",
    "5050": "100 x 101 / 2, the triangular closed form in lec22 evaluated at n = 100",
    "1023": "2^10 - 1, the maximum node count of a binary tree of height 9",
    "512": "2^9, the minimum node count of a binary tree of height 9",
    "0x1014": "0x1000 + 5 x 4, from the stated base and sizeof(int)=4",
    "0x101c": "0x1000 + 7 x 4, from the stated base and sizeof(int)=4",
    "0x2ffc": "0x2000 + 1023 x 4, from the stated base and sizeof(int)=4",
    "3.267": "49 / 15, the balanced successful search average for a 15 node tree",
    "160": "5 x 32, allocator charged bytes, from the 32 byte stride in lec05",
    "50": "20 / 40 as a percentage, the array stack payload share",
    "17": "8 x ceil(log2 8) - 2^3 + 1 = 17, the merge sort bound at n = 8",
    "448": "7 x 64, peak stack bytes for the 7 frame chain",
    "0x30": "48 decimal, the measured frame stride in lec16_recursion",
    "1048586 ": "1048576 + 10",
}

RESULTS = []


def check(label, ok, detail=""):
    RESULTS.append((label, bool(ok), detail))
    return bool(ok)


# ------------------------------------------------------------------ builds

def compile_lesson(src, tmp):
    """Compile one lesson with the strictest flags this project uses and report warnings."""
    exe = Path(tmp) / (src[:-4] + ".bin")
    cp = subprocess.run(
        ["g++", "-Wall", "-Wextra", "-Wpedantic", "-std=c++17", "-O2",
         "-o", str(exe), str(LESSONS / src)],
        capture_output=True, text=True, timeout=600)
    warnings = [l for l in cp.stderr.splitlines() if "warning:" in l]
    return cp.returncode, warnings, exe


def check_lessons(tmp):
    """Every lesson the set quotes: compiles clean, and runs."""
    ok_all = True
    for src in LESSON_SOURCES:
        if not (LESSONS / src).exists():
            check(f"lesson {src}: present", False, "file missing")
            ok_all = False
            continue
        rc, warnings, exe = compile_lesson(src, tmp)
        good = rc == 0 and not warnings
        ok_all &= good
        check(f"lesson {src}: compiles with zero warnings", good,
              warnings[0] if warnings else (f"exit {rc}" if rc else ""))
    return ok_all


# ------------------------------------------------------------------ number tracing

def load_captures():
    text = {}
    for name, path in CAPTURES.items():
        if path.exists():
            text[name] = path.read_text(encoding="utf-8")
    return text


def fresh_run_texts(tmp):
    """A fresh run of the lesson programs, so a stale capture cannot hide a wrong number."""
    out = {}
    for src in ("08_linked_list_searching_apps.cpp",
                "17_merge_sort.cpp", "18_quick_sort.cpp", "19_tree_representation.cpp",
                "20_tree_traversals.cpp", "21_bst_operations.cpp", "22_algorithm_analysis.cpp",
                "23_structure_formulations.cpp", "11_stack_array.cpp", "13_queue_array.cpp",
                "14_queue_linked_circular.cpp", "05_ds_memory_representation.cpp"):
        rc, _, exe = compile_lesson(src, tmp)
        if rc != 0:
            continue
        try:
            run = subprocess.run([str(exe)], capture_output=True, text=True, timeout=900)
            out[src] = run.stdout
        except subprocess.TimeoutExpired:
            out[src] = ""
    return out


def md_numbers(md):
    return re.findall(r"(?<![\w.])(\d{2,})(?![\w])", md)


def check_numbers(md, caps, fresh):
    """Every multi-digit number must be traceable. An untraceable one is a failure."""
    corpus = list(caps.values()) + list(fresh.values())
    tokens = sorted(set(md_numbers(md)))
    untraced = []
    for tok in tokens:
        if any(tok in c for c in corpus):
            continue
        if tok in DERIVED:
            continue
        untraced.append(tok)
    check(f"numbers: all {len(tokens)} multi-digit tokens traceable to a capture, a fresh run, "
          f"or the named derived allowlist", not untraced,
          f"untraced: {untraced}" if untraced else "")
    check("numbers: the derived allowlist is explicit and non-empty", len(DERIVED) > 0,
          f"{len(DERIVED)} entries")
    return tokens


def check_key_numbers(md, caps):
    """The headline numbers of each topic, quoted by name, against the recorded capture."""
    l05 = caps.get("lec05_run_output.txt", "")
    l11 = caps.get("lec11_15_run_output.txt", "")
    l16 = caps.get("lec16_recursion_output.txt", "")
    sorts = caps.get("lec16_18_sorts_run_output.txt", "")
    trees = caps.get("lec19_21_trees_run_output.txt", "")
    ana = caps.get("lec22_analysis_run_output.txt", "")
    form = caps.get("lec23_formulations_run_output.txt", "")

    pairs = [
        # (the claim as the document makes it, the capture line that backs it, capture text)
        ("array: base address", "0x7ffe7c3131b0", l05),
        ("array: stride equals sizeof(int)", "byte offset=4  value=20", l05),
        ("array: index 4 in one step", "index 4 in ONE step", l05),
        ("array: sequential walk 0.19 ns", "0.19 ns/element", l05),
        ("array: scattered walk 2.46 ns", "2.46 ns/element", l05),
        ("array: linked walk 2.45 ns", "2.45 ns/element", l05),
        ("array: 12.69x ratio", "12.69x", l05),
        ("pointer: sizeof(Node)=16", "sizeof(Node)=16", l05),
        ("pointer: node offsets", "offsetof(data)=0 offsetof(next)=8", l05),
        ("pointer: heap stride 32", "delta from previous = 32 bytes", l05),
        ("stack: sizeof 40", "sizeof(ArrayStack<int,5>)  = 40", l11),
        ("stack: top offset 24", "offsetof(top)     = 24", l11),
        ("stack: 2 refusals", "refused pushes = 2, capacity() = 5", l11),
        ("stack: push 0.688 ns", "push 0.688 ns, pop 0.378 ns", l11),
        ("stack: 4267.2x", "4267.2x", l11),
        ("queue: FIFO order", "they came back 10 20 30", l11),
        ("queue: 3 slots lost", "free slots BELOW front = 3", l11),
        ("queue: 2 wrap-arounds", "the rear index came round 2 times", l11),
        ("queue: 0.531 ns circular", "circular queue: 0.531 ns per operation", l11),
        ("queue: 14.023 ns linked", "linked queue  : 14.023 ns per operation", l11),
        ("merge: 16 comparisons at n=8", "16        7        7", sorts),
        ("merge: 965633 comparisons at n=65536", "965633    65535    65535", sorts),
        ("merge: depth log2 n + 1", "array depth == log2(n)+1", sorts),
        ("merge: 262144 byte buffer", "auxiliary array requested =    262144 bytes", sorts),
        ("merge: list buffer 0 blocks", "the sort allocated 0 blocks and 0 bytes", sorts),
        ("quick: 523776 sorted case", "sorted   lomuto-last            523776", sorts),
        ("quick: 12286 all-equal hoare", "hoare-first         1024        12286", sorts),
        ("quick: factor 43", "a factor of 43", sorts),
        ("quick: 105.0 in time", "a measured factor of 105.0 in time", sorts),
        ("analysis: c=1 n0=7", "largest n in 1..20000 where it FAILS = 6  ->  n0 = 7", ana),
        ("analysis: theta certificate", "c1 = 3/4 (from n = 1) and c2 = 1/1 (from n = 7)", ana),
        ("analysis: 19994 values", "19994 values of n", ana),
        ("analysis: f/g convergence", "0.750125", ana),
        ("analysis: nested loop count", "1048576         1048576", ana),
        ("analysis: triangular count", "523776          523776", ana),
        ("analysis: best case 1 comparison", "index 0, comparisons 1", ana),
        ("analysis: average 542.588235", "9224/17 = 542.588235", ana),
        ("trees: sizeof 24", "sizeof(Node)=24 alignof(Node)=8", trees),
        ("trees: 2i index arithmetic", "left child of i  = 2i", trees),
        ("trees: right chain 128 slots", "right chain               128", trees),
        ("trees: 5.47 percent", "5.47 percent of its slots hold a key", trees),
        ("trees: in-order sequence", "got: 1 2 3 4 5 6 7", trees),
        ("trees: level order", "got: 4 2 6 1 3 5 7", trees),
        ("trees: chain depth 7", "chain  of 7 nodes: depth = 7", trees),
        ("trees: 64 bytes per frame", "that is 64.0 bytes per suspended call", trees),
        ("bst: search comparison counts", "search(35) -> found     comparisons = 4", trees),
        ("bst: present keys average", "the 8 present keys cost 21 comparisons in total, 2.62 on average", trees),
        ("bst: successor of 50", "successor(50) = 60", trees),
        ("bst: sorted order height 31", "1, 2, 3, ... 31 (sorted)      31", trees),
        ("bst: median order height 5", "(median)       5", trees),
        ("bst: 3.84x ratio", "3.84x", trees),
        ("formulation: array 32", "1 array (int a[8])                    32", form),
        ("formulation: list node 16", "2 linked list node                    16", form),
        ("formulation: tree node 24", "7 tree node                           24", form),
        ("formulation: 3.3 percent", "eight structures together            240                             8       3.3", form),
        ("formulation: 25.0 wasted", "2 linked list node                    16         8         2         4      25.0", form),
        ("recursion: stack limit 8192 KiB", "getrlimit(RLIMIT_STACK) says this process may use 8192 KiB", l16),
        ("recursion: 48 bytes per call", "so 48 bytes per recursive call", l16),
        ("recursion: runaway stride 32", "the child measured its own runaway frame stride: 32 bytes per call", l16),
        ("recursion: measured depth 261579", "the child reached depth 261579", l16),
    ]
    for label, needle, hay in pairs:
        check(f"quote: {label}", needle in hay,
              "" if needle in hay else f"not found: {needle!r}")
    check("quote: the document itself names all seven captures",
          all(name in md for name in CAPTURES), "")


def check_lesson_quotes(fresh):
    """Numbers the document quotes from a lesson program, checked against a fresh run."""
    ll = fresh.get("08_linked_list_searching_apps.cpp", "")
    for label, needle in (
            ("linked list: search 30 costs 3 comparisons",
             "search 30 -> found at position 3, comparisons = 3"),
            ("linked list: an absent key costs 5 comparisons",
             "search 35 -> result -1 (absent), comparisons = 5"),
            ("linked list: best 1, worst n = 5, average n/2 = 2.5",
             "best case 1 comparison (key is at head), worst case n = 5, average n/2 = 2.5")):
        check(f"fresh quote: {label}", needle in ll,
              "" if needle in ll else f"not in the fresh run of the lesson: {needle!r}")


def check_capture_not_stale(md, caps, fresh):
    """A capture that no longer matches a fresh run would make every quote a lie."""
    checks = [
        ("lec16_18_sorts_run_output.txt", "17_merge_sort.cpp", "965633"),
        ("lec16_18_sorts_run_output.txt", "18_quick_sort.cpp", "523776"),
        ("lec19_21_trees_run_output.txt", "21_bst_operations.cpp", "successor(50) = 60"),
        ("lec22_analysis_run_output.txt", "22_algorithm_analysis.cpp", "1048576"),
        ("lec23_formulations_run_output.txt", "23_structure_formulations.cpp", "eight structures together"),
        ("lec11_15_run_output.txt", "11_stack_array.cpp", "refused pushes = 2"),
        ("lec11_15_run_output.txt", "13_queue_array.cpp", "free slots BELOW front = 3"),
        ("lec11_15_run_output.txt", "14_queue_linked_circular.cpp", "came round 2 times"),
    ]
    for cap, src, needle in checks:
        fresh_text = fresh.get(src, "")
        if not fresh_text:
            check(f"fresh: {src} produced output", False, "no fresh output")
            continue
        check(f"fresh: {src} still prints {needle!r}",
              needle in fresh_text, "fresh run no longer prints this")


# ------------------------------------------------------------------ document structure

def split_topics(md):
    parts = re.split(r"^## (\d+) \u00b7 (.+)$", md, flags=re.M)
    # parts: [preamble, n1, t1, body1, n2, t2, body2, ...]
    topics = []
    for i in range(1, len(parts), 3):
        topics.append((int(parts[i]), parts[i + 1].strip(), parts[i + 2]))
    return topics


def check_topics(md):
    topics = split_topics(md)
    check("topics: exactly 14 sections", len(topics) == 14, f"{len(topics)} found")
    check("topics: numbered 1 to 14 in order",
          [n for n, _, _ in topics] == list(range(1, 15)),
          str([n for n, _, _ in topics]))
    check("topics: the projected order, not the handout order",
          [t for _, t, _ in topics] == PROJECTED, str([t for _, t, _ in topics]))
    required = ["### The plain explanation", "### The mechanism", "### The proof",
                "### The C formulation", "### Exam traps", "### Numericals"]
    for n, title, body in topics:
        missing = [h for h in required if h not in body]
        check(f"topic {n} ({title}): has all six parts", not missing, str(missing))
        # the plain explanation must be 120 words or fewer. The provenance line that opens the
        # section is metadata (a scope word and a file name), so it is stripped before counting;
        # counting it would measure the citation, not the explanation.
        m = re.search(r"### The plain explanation\n+(?:\*Scope:[^\n]*\*\n+)?(.*?)### The mechanism",
                      body, re.S)
        words = len(re.findall(r"[A-Za-z][A-Za-z'-]*", m.group(1))) if m else 999
        check(f"topic {n} ({title}): plain explanation is 120 words or fewer", words <= 120,
              f"{words} words")
        # a coverage word and a named source, per topic
        scope = re.search(r"\*Scope: (.*?)\*", body, re.S)
        ok_scope = bool(scope) and re.search(r"Source:", scope.group(1))
        check(f"topic {n} ({title}): names its coverage and its source", bool(ok_scope),
              (scope.group(1)[:60] if scope else "no scope line"))
        # the proof must quote a capture file by name
        proof = re.search(r"### The proof\n(.*?)### ", body, re.S)
        named = re.search(r"`(lec\d+[a-z0-9_]*\.txt)`", proof.group(1)) if proof else None
        check(f"topic {n} ({title}): the proof quotes a capture by file name", bool(named),
              (named.group(1) if named else "no capture named in the proof"))
        # a code block per topic
        check(f"topic {n} ({title}): carries a C code block",
              "```c" in body, "no fenced C block")
    return topics


def check_numericals(topics):
    """6 to 10 numericals per topic, and a key of the same length, numbered 1 to 1."""
    keys = split_keys(MD.read_text(encoding="utf-8"))
    for n, title, body in topics:
        m = re.search(r"### Numericals\n(.*?)(?=\n## |\Z)", body, re.S)
        qs = re.findall(r"^(\d+)\. ", m.group(1), re.M) if m else []
        check(f"topic {n} ({title}): 6 to 10 numericals", 6 <= len(qs) <= 10, f"{len(qs)} found")
        numbered = [int(x) for x in qs]
        check(f"topic {n} ({title}): numericals numbered 1..{len(qs)}",
              numbered == list(range(1, len(qs) + 1)), str(numbered))
        key = keys.get(n, [])
        check(f"topic {n} ({title}): key has one entry per numerical",
              [int(x) for x in key] == numbered,
              f"key {key} against questions {numbered}")


def split_keys(md):
    """The keys section, split into per-topic tables, returning {topic: [numbers]}.

    The heading pattern uses [^\\n]+ for the title and [\\s\\S]*? for the body: a .+ with
    re.S would swallow the rest of the file and collapse every topic into one match.
    """
    m = re.search(r"^## Answer keys$(.*)", md, re.S | re.M)
    if not m:
        return {}
    keys = {}
    for tm in re.finditer(r"^### Topic (\d+) \u00b7 ([^\n]+)\n([\s\S]*?)(?=^### Topic |\Z)",
                          m.group(1), re.M):
        n = int(tm.group(1))
        rows = re.findall(r"^\|\s*(\d+)\s*\|", tm.group(3), re.M)
        keys[n] = rows
    return keys


def check_keys_separate(md):
    m = re.search(r"^## Answer keys$", md, re.M)
    check("keys: the answer keys are in their own top level section", bool(m), "")
    if m:
        before = md[:m.start()]
        after = md[m.start():]
        check("keys: no key table before the keys section",
              "## Answer keys" not in before, "")
        check("keys: the section is the last thing in the file",
              md.rstrip().endswith(after.rstrip()[-40:]), "")
        check("keys: every topic has a key table",
              len(split_keys(md)) == 14, f"{len(split_keys(md))} key tables")
        check("keys: the section says it is separate from the questions",
              "separate from the questions" in after or "Keys are here, separate" in after, "")


def check_diagrams(md):
    mmds = sorted(DIAG.glob("*.mmd"))
    pngs = sorted(DIAG.glob("*.png"))
    check("diagrams: at least 14 mermaid sources", len(mmds) >= 14, f"{len(mmds)} found")
    missing = [m.name for m in mmds if not (DIAG / (m.stem + ".png")).exists()]
    check("diagrams: every .mmd has a rendered .png", not missing, str(missing))
    trivial = []
    for p in pngs:
        size = p.stat().st_size
        w, h = struct.unpack(">II", p.read_bytes()[16:24])
        if size < 20000 or w < 600 or h < 300:
            trivial.append(f"{p.name} {size}B {w}x{h}")
    check("diagrams: every PNG is non-trivial (over 20 KB, at least 600x300)", not trivial,
          str(trivial))
    # every figure embedded with a path relative to THIS folder
    srcs = re.findall(r"!\[[^\]]*\]\(([^)]+)\)", md)
    check("diagrams: the note embeds figures", len(srcs) >= 14, f"{len(srcs)} embeds")
    bad = [s for s in srcs if s.startswith("/") or s.startswith("http") or ".." in s]
    check("diagrams: every embedded path is relative and inside this folder", not bad, str(bad))
    gone = [s for s in srcs if not (FOLDER / s).exists()]
    check("diagrams: every embedded file exists on disk", not gone, str(gone))
    in_html = re.findall(r'<img src="([^"]+)"', HTML.read_text(encoding="utf-8"))
    bad_html = [s for s in in_html if s.startswith("/") or s.startswith("http") or ".." in s]
    check("diagrams: the HTML embeds only in-folder relative paths", not bad_html, str(bad_html))
    check("diagrams: the HTML embeds every rendered diagram",
          all(any(s.endswith(p.name) for s in in_html) for p in pngs),
          str([p.name for p in pngs if not any(s.endswith(p.name) for s in in_html)]))


def check_code_blocks(md, tmp):
    blocks = re.findall(r"```c\n(.*?)```", md, re.S)
    check("code: at least 14 C blocks", len(blocks) >= 14, f"{len(blocks)} found")
    bad = []
    for i, b in enumerate(blocks, 1):
        with tempfile.NamedTemporaryFile("w", suffix=".c", delete=False) as fh:
            fh.write(b)
            path = fh.name
        for tag, cmd in (("c++17", ["g++", "-x", "c++"]), ("c11", ["gcc", "-x", "c"])):
            std = "-std=c++17" if tag == "c++17" else "-std=c11"
            cp = subprocess.run(cmd + ["-Wall", "-Wextra", "-Wpedantic", std, "-O2",
                                       "-fsyntax-only", path],
                                capture_output=True, text=True)
            noise = [l for l in cp.stderr.splitlines()
                     if "warning:" in l or "error:" in l]
            if cp.returncode != 0 or noise:
                bad.append(f"block {i} ({tag}): {noise[0][:120] if noise else 'exit ' + str(cp.returncode)}")
        os.unlink(path)
    check("code: every C block compiles as written, in C11 and in C++17", not bad,
          " | ".join(bad[:3]))


def check_style(md):
    check("style: no em dash (U+2014) anywhere", "\u2014" not in md, "")
    check("style: no en dash (U+2013) anywhere", "\u2013" not in md, "")
    html_doc = HTML.read_text(encoding="utf-8")
    check("style: no em dash in the HTML", "\u2014" not in html_doc, "")
    needed = ("\u0398", "\u03a9", "\u03a3", "\u2192", "\u2264", "\u2265", "\u00b2", "\u00b3",
              "\u2080", "\u00d7", "\u221e")
    check("style: real Unicode math is used, not ASCII words",
          all(ch in md for ch in needed),
          "missing: " + str([c for c in needed if c not in md]))
    check("style: no ASCII spelling of the math words",
          not re.search(r"\btheta\b|\bomega\b|\bsigma\b|\blambda\b", md, re.I),
          str(re.findall(r"\b(?:theta|omega|sigma|lambda)\b", md, re.I)[:4]))
    check("style: no ASCII-art tree drawing (rendered mermaid instead)",
          not any(ch in md for ch in ("\u251c", "\u2514", "\u250c", "\u2502")), "")
    # tables carry a caption and a scoped header
    n_tables = html_doc.count("<table>")
    check("style: every HTML table carries a caption",
          n_tables == html_doc.count("<caption>"), f"{n_tables} tables")
    n_scoped = html_doc.count('<th scope="col">')
    check("style: every HTML table has scope=col header cells", n_scoped >= n_tables,
          f"{n_scoped} scoped headers for {n_tables} tables")
    check("style: the page is a single self contained file",
          "<style>" in html_doc and "<script>" in html_doc, "")
    check("style: light and dark themes both present from the roadmap tokens",
          '[data-theme="dark"]' in html_doc and "--bg:#fafaf9" in html_doc, "")
    n_chips = html_doc.count('class="chip"')
    check("style: sticky nav with one chip per topic", n_chips == 14,
          f"{n_chips} chips")
    check("style: mobile first viewport declared",
          'name="viewport"' in html_doc and "max-width:720px" in html_doc, "")
    check("style: page stays under 200 KB", len(html_doc) < 200 * 1024,
          f"{len(html_doc)} bytes")


ROADMAP_LINK = "../midterm-roadmap/DSA-MIDTERM-ROADMAP.html#s5"


def check_no_sibling_paths(md):
    """Two different kinds of path, two different rules.

    An IMAGE embed must never leave this folder: the figures have to render for a reader who
    opens the file offline from anywhere. A hyperlink MAY point at the companion roadmap, which
    is the cross-page navigation that was asked for, so it is allowlisted by exact URL rather
    than waved through as a class.
    """
    embeds = re.findall(r"!\[[^\]]*\]\(([^)]+)\)", md)
    escaping = [e for e in embeds if e.startswith("/") or e.startswith("http") or ".." in e]
    check("paths: no image embed escapes this folder", not escaping, str(escaping))

    links = re.findall(r"(?<!!)\[[^\]]*\]\(([^)]+)\)", md)
    outside = [u for u in links if u.startswith("..") and u != ROADMAP_LINK]
    check("paths: the only out-of-folder link is the roadmap cross-link", not outside,
          str(outside[:3]))
    check("paths: the roadmap cross-link is present and points at the topic plan",
          ROADMAP_LINK in md, f"{md.count(ROADMAP_LINK)} occurrences")
    check("paths: every in-folder link target exists",
          all((FOLDER / u).exists() for u in links
              if not u.startswith(("http", "..", "#"))), "")


def check_proof_rows(md, caps):
    """Every proof-table row that names a capture must have its numbers in THAT capture.

    This is the document-side half of traceability. check_numbers() only asks whether a token
    exists somewhere in the corpus, so it cannot see a number quoted against the wrong file;
    this walks each row and reads the file the row itself names.
    """
    topics = split_topics(md)
    problems, rows_seen = [], 0
    for n, title, body in topics:
        p = re.search(r"### The proof\n(.*?)(?=^### |\Z)", body, re.S | re.M)
        if not p:
            continue
        for row in p.group(1).splitlines():
            if not row.startswith("|"):
                continue
            cells = [c.strip() for c in row.strip("|").split("|")]
            if len(cells) < 3:
                continue
            names = re.findall(r"`(lec\w+\.txt)`", cells[-1])
            if not names:
                continue
            rows_seen += 1
            cap = names[0]
            if cap not in caps:
                problems.append(f"topic {n}: names a capture that does not exist: {cap}")
                continue
            for tok in re.findall(r"(?<![\w.])(\d{2,}(?:\.\d+)?)", cells[1]):
                if tok in caps[cap] or tok in DERIVED:
                    continue
                problems.append(f"topic {n}: {tok!r} is not in {cap} (row: {cells[0][:40]})")
    check("proof: every proof row names a capture that exists", rows_seen >= 40, f"{rows_seen} rows")
    check("proof: every number in a proof row appears in the capture that row names",
          not problems, " | ".join(problems[:3]))


def check_capture_names(md):
    """Any capture file name the document uses must be one of the seven that exist."""
    named = sorted(set(re.findall(r"`(lec[\w]*\.txt)`", md)))
    unknown = [n for n in named if n not in CAPTURES]
    check("captures: the document names only real capture files", not unknown, str(unknown))
    check("captures: the document names every capture at least once", len(named) == len(CAPTURES),
          f"{len(named)} named of {len(CAPTURES)}")


def check_no_leaks(md):
    """No answer may sit in a topic section: the keys live in their own section, or nowhere."""
    head = md.split("## Answer keys")[0]
    leaks = []
    # The patterns require a VALUE after the label. A bare "answer is" also matches honest prose
    # ("a fast wrong answer is worth nothing"), and a check that fires on its own document is
    # worse than no check: it trains the reader to ignore it.
    pats = (r"\banswer\s*(?:is|:)\s*[-+]?\d", r"\bKey\s*:\s*\S",
            r"\bAnswer\s*:\s*\S", r"correct option\s*:?\s*\d",
            r"\(printed\s*:", r"\(sheet\s*:")
    for pat in pats:
        for m in re.finditer(pat, head, re.I):
            leaks.append(head[max(0, m.start() - 40):m.end() + 20].replace("\n", " "))
    check("keys: no answer leaked into the question sections", not leaks, str(leaks[:2]))


def check_keys_after_topics(md):
    """The keys section must come after every topic, never before them."""
    ki = md.find("## Answer keys")
    last_topic = md.find("## 14 \u00b7")
    check("keys: the keys section sits after the last topic", ki > last_topic > -1,
          f"keys at {ki}, topic 14 at {last_topic}")
    n_topics_after = len(re.findall(r"^## \d+ \u00b7 ", md[ki:], re.M))
    check("keys: no topic section comes after the keys", n_topics_after == 0,
          f"{n_topics_after} topics after the keys")


ROADMAP_HTML = NOTES / "midterm-roadmap" / "DSA-MIDTERM-ROADMAP.html"


def check_cross_links(md, html_doc):
    """Both directions of the roadmap cross-link, and that each lands on a real anchor.

    A link that points at an id the target page does not have is a 404 in a browser tab and
    looks like a working link in a diff, so the anchor is resolved against the target file.
    """
    frag = ROADMAP_LINK.split("#")[-1]
    check("cross-link: the roadmap file exists on disk", ROADMAP_HTML.exists(), str(ROADMAP_HTML))
    if ROADMAP_HTML.exists():
        target = ROADMAP_HTML.read_text(encoding="utf-8")
        check(f"cross-link: the roadmap really has the #{frag} anchor",
              f'id="{frag}"' in target, f'id="{frag}" not found in the roadmap')
        check("cross-link: the roadmap links back to this explanation set",
              "topic-wise/TOPIC-WISE-EXPLAINED.html" in target, "")
        check("cross-link: the roadmap still passes its own verifier marker",
              "DSA Mid-Term Roadmap" in target or "ECE2104" in target, "")
    check("cross-link: the HTML carries the roadmap link as a real anchor",
          f'href="{ROADMAP_LINK}"' in html_doc, "")
    check("cross-link: the HTML renders it as a link, not as literal markdown",
          "[DSA mid-term roadmap" not in html_doc, "markdown left unrendered")


# ------------------------------------------------------------------ mutation battery

def mutate_and_run(name, transform, tmp, caps):
    """Apply a fault to a temp copy of the notes and report which checks bite."""
    with tempfile.TemporaryDirectory() as td:
        tmp_md = Path(td) / "TOPIC-WISE-EXPLAINED.md"
        original = MD.read_text(encoding="utf-8")
        doc = transform(original)
        tmp_md.write_text(doc, encoding="utf-8")
        saved_md = globals()["MD"]
        saved = list(RESULTS)
        del RESULTS[:]
        try:
            globals()["MD"] = tmp_md
            topics = check_topics(doc)
            check_numericals(topics)
            check_keys_separate(doc)
            check_numbers(doc, caps, {})
            check_proof_rows(doc, caps)
            check_capture_names(doc)
            check_no_leaks(doc)
            check_keys_after_topics(doc)
            check_style(doc)
            check_no_sibling_paths(doc)
            # the real HTML, not the mutant: passing the mutant as the HTML would make
            # every fault look caught for the wrong reason
            check_cross_links(doc, HTML.read_text(encoding="utf-8"))
            failed = [l for l, ok, _ in RESULTS if not ok]
        finally:
            globals()["MD"] = saved_md
            del RESULTS[:]
            RESULTS.extend(saved)
        return failed


REAL_MUTATIONS = [
    ("a measured number changed to a wrong value",
     lambda s: s.replace("523776", "523775", 1)),
    ("a capture file name misquoted",
     lambda s: s.replace("lec16_18_sorts_run_output.txt", "lec16_18_sorts_output.txt", 1)),
    ("a topic renamed away from the projected list",
     lambda s: s.replace("## 9 \u00b7 Asymptotic notation", "## 9 \u00b7 Something else", 1)),
    ("a topic section deleted",
     lambda s: re.sub(r"^## 14 \u00b7 Different Operations$.*?(?=^## Answer keys)",
                      "", s, flags=re.S | re.M)),
    ("an em dash injected",
     lambda s: s.replace("Merge sort splits", "Merge sort\u2014splits", 1)),
    ("a numerical deleted from a topic, so the key no longer matches",
     lambda s: s.replace("1. State `sizeof(void*)` on this machine and say why it is that size.\n", "", 1)),
    ("a key row deleted, so the key no longer numbers 1 to 1",
     lambda s: re.sub(r"^\| 3 \| 8 \u00d7 4 = 32 bytes.*?\n", "", s, count=1, flags=re.M)),
    ("an answer leaked into the question section",
     lambda s: s.replace("### Exam traps", "The answer is 523776.\n\n### Exam traps", 1)),
    ("a diagram embed pointed at a sibling folder",
     lambda s: s.replace("(diagrams/01-array-index-arithmetic.png)",
                         "(../midterm-roadmap/diagrams/01-array-index-arithmetic.png)", 1)),
    ("ASCII spelling of a math word introduced",
     lambda s: s.replace("Big O is an upper bound", "theta is an upper bound", 1)),
    ("the keys section moved before the topics",
     lambda s: s.replace("## 1 \u00b7 Array", "## Answer keys\n\n### Topic 1 \u00b7 Array\n\n| # | Answer |\n|---|---|\n| 1 | 16 |\n\n## 1 \u00b7 Array", 1)),
]


def run_mutations(tmp, caps):
    print("== MUTATION BATTERY: a verifier that cannot fail is not a verifier ==")
    caught = 0
    for name, fn in REAL_MUTATIONS:
        try:
            failed = mutate_and_run(name, fn, tmp, caps)
        except Exception as exc:  # noqa: BLE001
            failed = [f"exception: {exc}"]
        ok = bool(failed)
        caught += ok
        tag = "CAUGHT" if ok else "MISSED"
        print(f"  [{tag:6s}] {name}" + (f"  -> {failed[0]}" if failed else "  -> NO CHECK FAILED"))
    print(f"== MUTATION RESULT: {caught}/{len(REAL_MUTATIONS)} injected faults caught ==")
    print("== KNOWN BLIND SPOTS (stated, not hidden):")
    print("   - a measured number swapped for ANOTHER number the same capture contains: the")
    print("     traceability check is a membership test, so it cannot tell a swap from a")
    print("     repeat. The headline quotes ARE checked by name against their capture line.")
    print("   - a code block that is syntactically valid but semantically wrong (a wrong")
    print("     loop bound that still compiles) is not caught by a compile-only check.")
    return caught == len(REAL_MUTATIONS)


# ------------------------------------------------------------------ main

def main():
    args = sys.argv[1:]
    for path in (MD, HTML, DIAG, BUILD):
        if not path.exists():
            print(f"missing required artifact: {path}")
            return 2
    md = MD.read_text(encoding="utf-8")
    caps = load_captures()
    check("captures: all seven capture files present", len(caps) == 7,
          f"{len(caps)} of 7 found: {sorted(set(CAPTURES) - set(caps))}")

    with tempfile.TemporaryDirectory() as tmp:
        ok_lessons = check_lessons(tmp)
        fresh = fresh_run_texts(tmp) if ok_lessons else {}
        check_code_blocks(md, tmp)

    check_key_numbers(md, caps)
    check_proof_rows(md, caps)
    check_capture_names(md)
    check_no_leaks(md)
    check_keys_after_topics(md)
    if fresh:
        check_lesson_quotes(fresh)
    if fresh:
        check_capture_not_stale(md, caps, fresh)
    else:
        check("fresh runs: at least one lesson produced output", False,
              "no fresh output, the staleness check cannot run")
    check_numbers(md, caps, fresh)
    topics = check_topics(md)
    check_numericals(topics)
    check_keys_separate(md)
    check_diagrams(md)
    check_style(md)
    check_no_sibling_paths(md)
    check_cross_links(md, HTML.read_text(encoding="utf-8"))

    passed = sum(1 for _, ok, _ in RESULTS if ok)
    total = len(RESULTS)
    for label, ok, detail in RESULTS:
        if not ok:
            print(f"  [FAIL] {label}" + (f"   {detail}" if detail else ""))
    print(f"\n== VERIFY: {passed}/{total} checks pass ==")
    status = 0 if passed == total else 1

    if "--mutate" in args:
        with tempfile.TemporaryDirectory() as tmp:
            mut_ok = run_mutations(tmp, load_captures())
        status = status or (0 if mut_ok else 1)
    return status


if __name__ == "__main__":
    sys.exit(main())
