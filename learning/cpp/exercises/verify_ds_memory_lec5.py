#!/usr/bin/env python3
"""
verify_ds_memory_lec5.py : verifier for the DSA Lecture 5 memory-representation lesson.

What it proves, in order:
  1. the capture program compiles with ZERO warnings under -Wall -Wextra
  2. its output is invariant where it must be (sizes, offsets, strides, frame deltas)
     and internally consistent where it cannot be (addresses move because of ASLR)
  3. the recorded capture file is not stale: its invariant lines equal a fresh run's
  4. the visual guide quotes the recorded capture, number by number, and nothing else
  5. the guide is structurally sound: 10 sheets, unique ids, no em dashes, no hardcoded
     colours in the markup, tokens present, JS parses, og card present
  6. the notes and the diagram set are complete and agree with the guide

Usage:
  python3 verify_ds_memory_lec5.py            # run every check
  python3 verify_ds_memory_lec5.py --mutate   # run the mutation battery: prove the
                                              # verifier actually bites on 8 injected faults
"""
import re
import subprocess
import sys
import shutil
import tempfile
from pathlib import Path

HOME = Path.home()
CPP = HOME / "learning/cpp/lessons/05_ds_memory_representation.cpp"
CAPTURE = HOME / "learning/cpp/exercises/lec05_run_output.txt"
GUIDE = HOME / "Documents/MUJ-SEM-3/DSA/Notes/intro-memory/INTRO-MEMORY-VISUAL-GUIDE.html"
NOTES = HOME / "Documents/MUJ-SEM-3/DSA/Notes/intro-memory/INTRO-MEMORY-STUDY-NOTES.md"
DIAG = HOME / "Documents/MUJ-SEM-3/DSA/Notes/intro-memory/diagrams"
OG = HOME / "Documents/MUJ-SEM-3/DSA/Notes/intro-memory/og-card.svg"
TOKENS = HOME / "Documents/MUJ-SEM-3/DSA/Notes/intro-memory/tokens.css"
REFS = HOME / "Documents/MUJ-SEM-3/DSA/Notes/intro-memory/.qa/refs/refs.json"

RESULTS = []


def check(label, ok, detail=""):
    RESULTS.append((label, bool(ok), detail))
    return bool(ok)


def compile_and_run(tmp):
    """Compile the lesson with the strictest sensible flags and run it."""
    exe = Path(tmp) / "lec05"
    cp = subprocess.run(
        ["g++", "-Wall", "-Wextra", "-std=c++17", "-O2", "-o", str(exe), str(CPP)],
        capture_output=True, text=True, timeout=300)
    warnings = [l for l in cp.stderr.splitlines() if "warning:" in l]
    check("compile: exit 0", cp.returncode == 0, cp.stderr[-200:] if cp.returncode else "")
    check("compile: zero warnings under -Wall -Wextra", not warnings,
          warnings[0] if warnings else "")
    if cp.returncode != 0:
        return "", ""
    run = subprocess.run([str(exe)], capture_output=True, text=True, timeout=600)
    check("run: exit 0", run.returncode == 0, run.stderr[-200:])
    return run.stdout, cp.stderr


def parse_capture(text):
    """Pull the machine-independent facts and the address tables out of a run."""
    facts = {}
    m = re.search(r"sizeof\(char\)=(\d+) sizeof\(short\)=(\d+) sizeof\(int\)=(\d+) sizeof\(long\)=(\d+) "
                  r"sizeof\(float\)=(\d+) sizeof\(double\)=(\d+) sizeof\(void\*\)=(\d+)", text)
    if m:
        facts["sizes"] = dict(zip(["char", "short", "int", "long", "float", "double", "void*"],
                                  (int(x) for x in m.groups())))
    m = re.search(r"alignof\(int\)=(\d+) alignof\(double\)=(\d+) alignof\(void\*\)=(\d+)", text)
    if m:
        facts["align"] = dict(zip(["int", "double", "void*"], (int(x) for x in m.groups())))
    m = re.search(r"sizeof\(Node\)=(\d+) alignof\(Node\)=(\d+)", text)
    if m:
        facts["node"] = {"size": int(m.group(1)), "align": int(m.group(2))}
    facts["array"] = re.findall(r"i=(\d+)\s+&a\[(\d+)\]=(\S+)\s+byte offset=(\d+)\s+value=(\d+)", text)
    facts["one_step"] = "index 4 in ONE step" in text and "== &a[4] ? yes" in text
    facts["raw_bytes"] = re.findall(r"a\[0\]\+\s*(\d+)\s+byte ([0-9a-f]{2})", text)
    facts["string"] = re.findall(r"s\[(\d)\] at (\S+)  hex=([0-9a-f]{2})", text)
    facts["frames"] = re.findall(r"frame depth (\d+)\s+&local = (\S+)", text)
    facts["heap"] = re.findall(r"node (\d+) at (\S+)(?:\s+delta from previous = (\d+) bytes)?", text)
    m = re.search(r"requested per node = (\d+) bytes, allocator actually hands out (\d+) usable bytes "
                  r"and charges a (\d+) byte stride", text)
    if m:
        facts["alloc"] = {"requested": int(m.group(1)), "usable": int(m.group(2)), "stride": int(m.group(3))}
    m = re.search(r"stack sits (\d+) GiB above the heap", text)
    if m:
        facts["gap_gib"] = int(m.group(1))
    facts["bench"] = {}
    for key, pat in (("seq", r"array walk  \(arr\[i\], contiguous[^)]*\):\s*([\d.]+) ns"),
                     ("rand", r"array walk  \(arr\[idx\], same array, scattered order\):\s*([\d.]+) ns"),
                     ("list", r"linked walk \(cur = cur->next[^)]*\):\s*([\d.]+) ns")):
        mm = re.search(pat, text)
        if mm:
            facts["bench"][key] = float(mm.group(1))
    facts["bench_ratio"] = re.findall(r"scattered array is ([\d.]+)x the sequential one and the list is ([\d.]+)x", text)
    facts["sums_agree"] = "agree? yes" in text
    facts["structs"] = {}
    for name, pat in (("P1", r"P1 \{int a; char b; char c;\}\s+sizeof=(\d+)\s+offsetof\(a\)=(\d+) offsetof\(b\)=(\d+) offsetof\(c\)=(\d+)"),
                      ("P2", r"P2 \{char a; int b; char c;\}\s+sizeof=(\d+)\s+offsetof\(a\)=(\d+) offsetof\(b\)=(\d+) offsetof\(c\)=(\d+)"),
                      ("P3", r"P3 \{char a; char b; int c;\}\s+sizeof=(\d+)\s+offsetof\(a\)=(\d+) offsetof\(b\)=(\d+) offsetof\(c\)=(\d+)")):
        mm = re.search(pat, text)
        if mm:
            facts["structs"][name] = {"size": int(mm.group(1)), "off": [int(mm.group(i)) for i in (2, 3, 4)]}
    mm = re.search(r"Node\{int data; Node\* next;\}\s+sizeof=(\d+)\s+offsetof\(data\)=(\d+) offsetof\(next\)=(\d+)", text)
    if mm:
        facts["structs"]["Node"] = {"size": int(mm.group(1)), "off": [int(mm.group(2)), int(mm.group(3))]}
    return facts


INVARIANT_KEYS = ["sizes", "align", "node", "one_step", "raw_bytes", "string", "alloc",
                  "structs", "bench", "bench_ratio", "sums_agree"]


def invariant_lines(text):
    """The subset of the run that must be identical on any correct run of this compiler."""
    out = []
    keep = ("sizeof(", "alignof(", "index 4 in ONE step", "a[0]+", "s[", "requested per node",
            "P1 {", "P2 {", "P3 {", "Node{", "sanity: sums")
    # wall-clock timings are NOT invariants: they move a few percent between runs and are
    # checked separately with bounds, so they are excluded from the line-for-line comparison.
    skip = ("ns/element", "scattered array is")
    addr_re = re.compile(r"0x[0-9a-fA-F]{4,}")
    for line in text.splitlines():
        if any(k in line for k in keep) and not any(k in line for k in skip):
            # addresses move between runs because of ASLR: compare the line with them masked
            out.append(addr_re.sub("0xADDR", line.rstrip()))
    return out


def check_invariants(fresh, recorded):
    """Every invariant fact, checked by name so a failure says which one broke."""
    f = parse_capture(fresh)
    check("invariant: sizeof(char)=1, short=2, int=4, long=8, float=4, double=8, void*=8",
          f["sizes"] == {"char": 1, "short": 2, "int": 4, "long": 8, "float": 4, "double": 8, "void*": 8},
          str(f.get("sizes")))
    check("invariant: alignof(int)=4, alignof(double)=8, alignof(void*)=8",
          f["align"] == {"int": 4, "double": 8, "void*": 8}, str(f.get("align")))
    check("invariant: sizeof(Node)=16, alignof(Node)=8", f["node"] == {"size": 16, "align": 8}, str(f.get("node")))

    arr = f["array"]
    check("invariant: five array elements printed", len(arr) == 5, str(len(arr)))
    check("invariant: array stride is exactly sizeof(int)=4 on every element",
          all(int(a[3]) == int(a[0]) * 4 for a in arr), str(arr))
    check("invariant: array values are 10,20,30,40,50 in order",
          [int(a[4]) for a in arr] == [10, 20, 30, 40, 50], str([a[4] for a in arr]))
    check("invariant: addresses increase by 4 bytes per index",
          all(int(arr[i][2], 16) - int(arr[i - 1][2], 16) == 4 for i in range(1, len(arr))), "")
    check("invariant: base + i*sizeof(T) equals &a[i] (the O(1) claim)",
          f["one_step"] is True, "")

    raw = dict((int(o), b) for o, b in f["raw_bytes"])
    check("invariant: 20 raw array bytes printed", len(raw) == 20, str(len(raw)))
    check("invariant: little-endian packing of a[0]=10 is 0a 00 00 00",
          [raw.get(i) for i in range(4)] == ["0a", "00", "00", "00"], str([raw.get(i) for i in range(4)]))
    check("invariant: little-endian packing of a[4]=50 is 32 00 00 00",
          [raw.get(i) for i in range(16, 20)] == ["32", "00", "00", "00"], str([raw.get(i) for i in range(16, 20)]))
    check("invariant: second byte of every int is 00 (values fit in one byte)",
          all(raw.get(i) == "00" for i in range(20) if i % 4 != 0), "")

    check("invariant: string bytes are 44 53 41 00 (DSA plus terminator)",
          [b for _, _, b in f["string"]] == ["44", "53", "41", "00"], str(f["string"]))
    check("invariant: sizeof(string literal) counts the terminator, 4 for DSA", len(f["string"]) == 4, "")

    check("invariant: four nested frames printed", len(f["frames"]) == 4, str(len(f["frames"])))
    addrs = [int(a, 16) for _, a in f["frames"]]
    check("invariant: the stack grows DOWN (each deeper frame has a lower address)",
          all(addrs[i] < addrs[i - 1] for i in range(1, len(addrs))), str([hex(a) for a in addrs]))
    check("invariant: each frame consumes 48 bytes",
          all(addrs[i - 1] - addrs[i] == 48 for i in range(1, len(addrs))), str([addrs[i - 1] - addrs[i] for i in range(1, len(addrs))]))

    heap = f["heap"]
    check("invariant: five heap nodes printed", len(heap) == 5, str(len(heap)))
    check("invariant: heap addresses advance by the measured 32 byte stride",
          all(int(heap[i][1], 16) - int(heap[i - 1][1], 16) == 32 for i in range(1, len(heap))), str([h[1] for h in heap]))
    check("invariant: allocator charges 32 bytes for a 16 byte request and reports 24 usable",
          f["alloc"] == {"requested": 16, "usable": 24, "stride": 32}, str(f.get("alloc")))

    st = f["structs"]
    check("invariant: P1 is 8 bytes with int@0, char@4, char@5",
          st.get("P1") == {"size": 8, "off": [0, 4, 5]}, str(st.get("P1")))
    check("invariant: P2 is 12 bytes with char@0, int@4, char@8 (padding after the char)",
          st.get("P2") == {"size": 12, "off": [0, 4, 8]}, str(st.get("P2")))
    check("invariant: P3 is 8 bytes with char@0, char@1, int@4",
          st.get("P3") == {"size": 8, "off": [0, 1, 4]}, str(st.get("P3")))
    check("invariant: Node is 16 bytes with data@0 and next@8 (4 bytes of padding)",
          st.get("Node") == {"size": 16, "off": [0, 8]}, str(st.get("Node")))
    check("invariant: reordering fields changes the footprint (P1 8 vs P2 12)", st["P1"]["size"] != st["P2"]["size"], "")

    b = f["bench"]
    check("invariant: all three timings printed", len(b) == 3, str(b))
    check("invariant: sequential array walk is the fastest of the three",
          b.get("seq", 9) < min(b.get("rand", 9), b.get("list", 9)), str(b))
    check("invariant: scattered array and linked walk are both at least 5x the sequential walk",
          b.get("rand", 0) / b.get("seq", 1) >= 5 and b.get("list", 0) / b.get("seq", 1) >= 5, str(b))
    check("invariant: the benchmark sums agree, so the three loops did the same work",
          f["sums_agree"] is True, "")
    check("invariant: heap sits below the stack in the address space",
          int(heap[-1][1], 16) < addrs[0], f"{heap[-1][1]} < {hex(addrs[0])}")
    check("invariant: the stack-to-heap gap is reported in GiB (a large unmapped span)",
          f.get("gap_gib", 0) > 1000, str(f.get("gap_gib")))


def check_capture_not_stale(fresh, recorded):
    fi, ri = invariant_lines(fresh), invariant_lines(recorded)
    check("capture file: every invariant line matches a fresh run",
          fi == ri,
          "first difference: " + str(next((f"fresh={a!r} recorded={b!r}" for a, b in zip(fi, ri) if a != b), "line count differs")))


def check_guide_matches_capture(recorded, guide):
    """The guide may only quote the recorded capture, and must quote all of it."""
    f = parse_capture(recorded)
    missing = []
    for i, (_, _, addr, _, value) in enumerate(f["array"]):
        if addr not in guide:
            missing.append(addr)
    for _, addr in f["frames"]:
        if addr not in guide:
            missing.append(addr)
    for _, addr, _ in f["heap"]:
        if addr not in guide:
            missing.append(addr)
    check("guide: every captured array address appears in the guide", not missing, str(missing[:6]))
    # and the reverse: the guide may not quote an address the capture never printed.
    # Any address the run printed is fair game (stack locals, the heap, the data segment),
    # so the allowlist is read from the capture text itself, not hand-written here.
    allowed = set(re.findall(r"0x[0-9a-f]{8,}", recorded))
    # the sheet 06 address-space map draws the two ends of a 48-bit user space:
    # NULL at the bottom and the top of the range. Those are conventions, not measurements.
    allowed |= {"0x0000000000000000", "0x7fffffffffff"}
    quoted = set(re.findall(r"0x[0-9a-f]{8,}", guide))
    invented = sorted(a for a in quoted if a not in allowed)
    check("guide: never quotes an address the capture did not print (nothing invented, map ends excepted)",
          not invented, str(invented[:6]))

    checks = [
        ("guide: base address of the array is quoted", f["array"][0][2] in guide),
        ("guide: heap node 0 address is quoted", f["heap"][0][1] in guide),
        ("guide: global g_global address is quoted", re.search(r"g_global\s*=\s*(0x[0-9a-f]+)", recorded).group(1) in guide),
        ("guide: the 41326 GiB stack-heap gap is quoted", f"{f['gap_gib']} GiB" in guide),
        ("guide: sizeof(Node)=16 and offsetof(next)=8 are stated", "16" in guide and "next@8" in guide),
        ("guide: the allocator stride is stated as the measured 32 bytes, and never another number",
         "allocator stride 32 bytes for a 16-byte request" in guide
         and "the lane continues on the same measured 32 byte stride" in guide
         and "stride measured: 32 bytes" in guide
         and "heapStride: 32" in guide
         and set(re.findall(r"(\d+) byte stride", guide)) == {"32"},
         str(sorted(set(re.findall(r"(\d+) byte stride", guide))))),
        ("guide: requested 16 vs usable 24 vs stride 32 are all stated",
         "16 bytes, which is what" in guide and "24, the allocator" in guide and "32 bytes, measured" in guide),
        ("guide: frame deltas of 48 bytes are stated", "48 bytes" in guide),
        ("guide: the measured timings are quoted", str(f["bench"]["seq"]) in guide and str(f["bench"]["rand"]) in guide and str(f["bench"]["list"]) in guide),
        ("guide: the little-endian byte example is quoted", "0a 00 00 00" in guide and "32 00 00 00" in guide),
        ("guide: the four struct sizes are all quoted", all(g in guide for g in ("sizeof=8", "sizeof=12", "sizeof=16"))),
        ("guide: the DSA string bytes are quoted", "44 53 41 00" in guide),
        ("guide: provenance names the compiler and the flags", "g++ 15.2.0" in guide and "-Wall -Wextra -std=c++17 -O2" in guide),
        ("guide: the ASLR caveat is stated in the title block and again in the footer",
         "change per run (ASLR)" in guide and "because of ASLR" in guide and guide.count("ASLR") >= 3,
         "count=%d" % guide.count("ASLR")),
        ("guide: the array addresses in the script match the capture element by element",
         all('addr: "%s"' % addr in guide for _, _, addr, _, _ in f["array"]),
         str([addr for _, _, addr, _, _ in f["array"] if 'addr: "%s"' % addr not in guide][:4])),
        ("guide: labels the modelled heap growth as modelled, not measured",
         "marked as modelled" in guide and "modelled, measured 32 byte stride" in guide
         and "captured address" in guide),
    ]
    for row in checks:
        if len(row) == 3:
            check(row[0], row[1], row[2])
        else:
            check(row[0], row[1])


def check_guide_structure(guide, html_doc):
    ids = re.findall(r'\sid="([^"]+)"', html_doc)
    dups = sorted({i for i in ids if ids.count(i) > 1})
    check("guide: no duplicate element ids", not dups, str(dups))
    sheets = re.findall(r'<section class="sheet[^"]*" id="([^"]+)"', html_doc)
    sheet_ids = [s for s in sheets if s.startswith("sheet-")]
    check("guide: exactly 10 numbered sheets present", len(sheet_ids) == 10, str(sheet_ids))
    check("guide: every sheet id is linked from the rail",
          all(f'href="#{s}"' in html_doc for s in sheet_ids), "")
    check("guide: doctype, lang and viewport declared",
          html_doc.lstrip().startswith("<!DOCTYPE html>") and 'lang="en"' in html_doc and "viewport" in html_doc)
    check("guide: no em dash anywhere (user absolute rule)", "\u2014" not in html_doc)
    check("guide: no em dash in the notes either", "\u2014" not in NOTES.read_text(encoding="utf-8"))
    body = html_doc.split("<body", 1)[1]
    hexes = sorted(set(re.findall(r"#[0-9a-fA-F]{3,8}\b", body)))
    check("guide: no hardcoded colour in the markup (tokens only)", not hexes, str(hexes))
    style = re.search(r"<style>([\s\S]*?)</style>", html_doc).group(1)
    # everything outside the token blocks must reference a token; the print rule is
    # allowed the two vanilla neutrals because paper ink is white and black by definition
    stripped = re.sub(r":root\{[\s\S]*?\}", "", style)
    stripped = re.sub(r'\[data-theme="dark"\]\{[\s\S]*?\}', "", stripped)
    css_hexes = sorted({h for h in re.findall(r"#[0-9a-fA-F]{3,8}\b", stripped)
                        if h.lower() not in ("#fff", "#ffffff", "#000", "#000000")})
    check("guide: no hardcoded colour in the stylesheet outside the token blocks",
          not css_hexes, str(css_hexes[:8]))
    check("guide: token block carries the audited palette",
          all(t in html_doc for t in ("--bg:#f4f6f8", "--accent:#1d4ed8", "--bg:#0e141b", "--accent:#60a5fa")))
    check("guide: reduced-motion block present", "prefers-reduced-motion" in html_doc)
    check("guide: print stylesheet present", "@media print" in html_doc)
    check("guide: NOSTATE comment present for the states gate", "<!-- NOSTATE:" in html_doc)
    check("guide: og card exists on disk", OG.exists() and "og-card.svg" in html_doc)
    check("guide: references the verifier it is checked by", "verify_ds_memory_lec5.py" in html_doc)
    check("guide: references the capture program", "05_ds_memory_representation.cpp" in html_doc)
    check("refs artifact exists from ui-refs.py", REFS.exists())
    check("tokens.css exists and is the same palette", TOKENS.exists() and "--accent:#1d4ed8" in TOKENS.read_text().replace(" ", ""))

    # every interactive control has a matching id in the script
    js = "\n".join(re.findall(r"<script>([\s\S]*?)</script>", html_doc))
    # a control is a button/input/select or a node the script writes into (readout values)
    controls = []
    for tag in re.findall(r"<([a-zA-Z][a-zA-Z0-9]*)\b[^>]*\bid=\"([^\"]+)\"[^>]*>", body):
        name, eid = tag
        if name in ("button", "input", "select", "output", "details"):
            controls.append(eid)
    def wired(cid):
        """A control counts as wired if the script names its id, OR the script
        queries a selector that matches it (the struct tabs and the
        classification map are delegated: one listener per attribute/class)."""
        if f'"{cid}"' in js or f"'{cid}'" in js:
            return True
        m = re.search(r"<[^>]*\bid=\"" + re.escape(cid) + r"\"[^>]*>", body)
        if not m:
            return False
        attrs = re.findall(r"\b(data-[a-z-]+)=", m.group(0))
        if any(f"[{a}]" in js for a in attrs):
            return True
        klass = re.search(r'\bclass="([^"]+)"', m.group(0))
        classes = klass.group(1).split() if klass else []
        return any(f".{c}" in js for c in classes)
    live = [c for c in controls if wired(c)]
    check("guide: at least 15 real controls exist", len(controls) >= 15, f"{len(controls)} found")
    check("guide: every real control is wired in the script (no dead button)",
          len(live) == len(controls), str([c for c in controls if c not in live]))
    # readout targets: every readout__v id must be written by the script
    ro = re.findall(r'<span class="readout__v[^"]*" id="([^"]+)"', body)
    ro += re.findall(r'<span class="readout__v[^"]*"[^>]*id="([^"]+)"', body)
    ro = sorted(set(ro))
    dead_ro = [c for c in ro if f'"{c}"' in js or f"'{c}'" in js or f"getElementById(\"{c}\")" in js]
    check("guide: every readout has a live target the script updates", len(dead_ro) == len(ro),
          str([c for c in ro if c not in dead_ro]))

    # numbers the guide claims about itself must match the disk
    pngs = sorted(DIAG.glob("*.png"))
    mmds = sorted(DIAG.glob("*.mmd"))
    check("diagrams: five .mmd sources", len(mmds) == 5, str([m.name for m in mmds]))
    check("diagrams: every .mmd has a rendered .png", all((DIAG / (m.stem + ".png")).exists() for m in mmds), "")
    notes = NOTES.read_text(encoding="utf-8")
    check("notes: reference every rendered diagram", all(f"diagrams/{p.name}" in notes for p in pngs), "")
    check("notes: carry the lecture map and the CO", "Lecture 5" in notes and "CO1" in notes)
    check("notes: keep the answer key in a separate section", "## Answer key" in notes)
    check("notes: quote the measured timings", all(str(v) in notes for v in ("0.19", "2.46", "2.45")))
    check("notes: no ASCII-art tree lines (user prefers rendered mermaid)", "├" not in notes and "└" not in notes)


def check_js(html_doc):
    js = "\n".join(re.findall(r"<script>([\s\S]*?)</script>", html_doc))
    with tempfile.NamedTemporaryFile("w", suffix=".js", delete=False) as fh:
        fh.write(js)
        path = fh.name
    r = subprocess.run(["node", "--check", path], capture_output=True, text=True)
    check("guide: inline JavaScript parses (node --check)", r.returncode == 0, r.stderr[-300:])


# ------------------------------------------------------------------ mutation battery

def mutate(name, transform):
    """Apply a fault to a temp copy of the guide and report whether the verifier bites."""
    with tempfile.TemporaryDirectory() as td:
        tmp = Path(td)
        guide = tmp / "guide.html"
        notes = tmp / "notes.md"
        original = GUIDE.read_text(encoding="utf-8")
        doc = transform(original)
        guide.write_text(doc, encoding="utf-8")
        notes.write_text(NOTES.read_text(encoding="utf-8"), encoding="utf-8")
        # run the guide-only checks against the mutant
        saved = list(RESULTS)
        del RESULTS[:]
        try:
            check_guide_matches_capture(CAPTURE.read_text(encoding="utf-8"), doc)
            check_guide_structure(doc, doc)
            check_js(doc)
            readme_doc = doc
            failed = [l for l, ok, _ in RESULTS if not ok]
        finally:
            del RESULTS[:]
            RESULTS.extend(saved)
        return failed


REAL_MUTATIONS = [
    ("wrong array address in the data block", lambda s: s.replace('addr: "0x7ffe7c3131bc"', 'addr: "0x7ffe7c3131b8"', 1)),
    ("wrong array address in the prose", lambda s: s.replace("0x7ffe7c3131bc", "0x7ffe7c3131b8", 1)),
    ("wrong allocator stride", lambda s: s.replace("32 byte stride", "24 byte stride", 1)),
    ("wrong struct size", lambda s: s.replace("sizeof=12", "sizeof=16", 1)),
    ("wrong timing quoted", lambda s: s.replace("2.46 ns", "1.46 ns", 1)),
    ("a sheet id renamed away", lambda s: s.replace('id="sheet-05"', 'id="sheet-05-gone"', 1)),
    ("em dash injected", lambda s: s.replace("one multiply", "one multiply\u2014really", 1)),
    ("hardcoded colour injected", lambda s: s.replace(".panel{background:var(--surface)", ".panel{background:#abcdef", 1)),
    ("ASLR caveat removed", lambda s: s.replace("ASLR", "nothing", 1)),
    ("modelled heap growth relabelled as measured", lambda s: s.replace("marked as modelled because the run only printed five", "taken straight from the run", 1)),
    ("javascript syntax broken", lambda s: s.replace("function hex(n) {", "function hex(n {", 1)),
]


def run_mutations():
    print("== MUTATION BATTERY: a verifier that cannot fail is not a verifier ==")
    caught = 0
    for name, fn in REAL_MUTATIONS:
        try:
            failed = mutate(name, fn)
        except Exception as exc:  # noqa: BLE001
            failed = [f"exception: {exc}"]
        ok = bool(failed)
        caught += ok
        print(f"  [{'CAUGHT' if ok else 'MISSED':6s}] {name}" + (f"  -> {failed[0]}" if failed else "  -> NO CHECK FAILED"))
    print(f"== MUTATION RESULT: {caught}/{len(REAL_MUTATIONS)} injected faults caught ==")
    print("== KNOWN BLIND SPOTS (stated, not hidden):")
    print("   - a prose address swapped for ANOTHER address the same run printed: membership")
    print("     checks cannot tell a swap from a repeat. The script's data block IS checked")
    print("     element by element, so the same mistake inside CAP.array is caught.")
    return caught == len(REAL_MUTATIONS)


def main():
    args = sys.argv[1:]
    for path in (CPP, CAPTURE, GUIDE, NOTES, TOKENS):
        if not path.exists():
            print(f"missing required file: {path}")
            return 2
    recorded = CAPTURE.read_text(encoding="utf-8")
    guide = GUIDE.read_text(encoding="utf-8")

    with tempfile.TemporaryDirectory() as td:
        fresh, _ = compile_and_run(td)
    if fresh:
        check_invariants(fresh, recorded)
        check_capture_not_stale(fresh, recorded)
    else:
        check("capture: a fresh run happened", False, "compile or run failed, nothing else can be verified")

    check_guide_matches_capture(recorded, guide)
    check_guide_structure(guide, guide)
    check_js(guide)

    passed = sum(1 for _, ok, _ in RESULTS if ok)
    total = len(RESULTS)
    for label, ok, detail in RESULTS:
        if not ok:
            print(f"  [FAIL] {label}" + (f"   {detail}" if detail else ""))
    print(f"\n== VERIFY: {passed}/{total} checks pass ==")
    status = 0 if passed == total else 1

    if "--mutate" in args:
        mut_ok = run_mutations()
        status = status or (0 if mut_ok else 1)
    return status


if __name__ == "__main__":
    sys.exit(main())
