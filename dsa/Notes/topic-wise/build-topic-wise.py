#!/usr/bin/env python3
"""
build-topic-wise.py : build TOPIC-WISE-EXPLAINED.html from TOPIC-WISE-EXPLAINED.md.

One pass, no build step for the reader: the output is a single self-contained file that
opens offline, with the light and dark themes taken from the roadmap's tokens.

What it does, in order:
  1. reads the markdown source of truth and splits it on '## ' topic headings
  2. converts the small markdown subset the document actually uses (headings, tables,
     fenced code, images, inline bold and code, numbered lists) to HTML
  3. wraps it in the house shell: sticky nav with one chip per topic, theme toggle that
     persists in localStorage and reads prefers-color-scheme before first paint
  4. checks its own output: 14 topics, every table captioned with scope="col" headers,
     no em dash, every embedded image path relative and inside this folder

Usage:
  python3 build-topic-wise.py            # build and self-check
  python3 build-topic-wise.py --check    # self-check the existing file only
"""
import html
import re
import struct
import sys
from pathlib import Path

HERE = Path(__file__).resolve().parent
MD = HERE / "TOPIC-WISE-EXPLAINED.md"
OUT = HERE / "TOPIC-WISE-EXPLAINED.html"
TOKENS = HERE.parent / "midterm-roadmap" / "tokens.css"

TOPIC_RE = re.compile(r"^## (\d+) \u00b7 (.+)$", re.M)

# ---------------------------------------------------------------- markdown subset


def inline(text):
    """Inline markdown: code spans first so their contents are not re-processed."""
    text = html.escape(text, quote=False)
    placeholders = []

    def stash(m):
        placeholders.append(m.group(1))
        return f"\x00{len(placeholders) - 1}\x00"

    text = re.sub(r"`([^`]+)`", stash, text)
    text = re.sub(r"\*\*([^*]+)\*\*", r"<strong>\1</strong>", text)
    text = re.sub(r"(?<!\w)\*([^*]+)\*(?!\w)", r"<em>\1</em>", text)
    # links last, so the brackets inside a code span are never touched
    text = re.sub(r"\[([^\]]+)\]\(([^)]+)\)", r'<a href="\2">\1</a>', text)
    for i, code in enumerate(placeholders):
        text = text.replace(f"\x00{i}\x00", f"<code>{code}</code>")
    return text


def png_size(path, default=(1400, 900)):
    """The real pixel size of a PNG, read from its IHDR.

    The <img> width/height are layout hints that stop the page from reflowing as figures load.
    Writing a guessed 1400x900 there would be a fabricated measurement in the markup, and a wrong
    one for every figure whose aspect ratio is not 14:9, so the numbers come from the file.
    """
    try:
        data = path.read_bytes()[:24]
        if data[:8] == b"\x89PNG\r\n\x1a\n":
            return struct.unpack(">II", data[16:24])
    except OSError:
        pass
    return default


def table_block(rows, caption, cls=""):
    """rows: list of raw '| a | b |' lines, first one is the header."""
    def cells(line):
        return [c.strip() for c in line.strip().strip("|").split("|")]

    head = cells(rows[0])
    body = [cells(r) for r in rows[2:]]
    out = ['<div class="tbl-wrap">', f'<table class="{cls}">' if cls else "<table>",
           f"<caption>{inline(caption)}</caption>", "<thead><tr>"]
    for h in head:
        out.append(f'<th scope="col">{inline(h)}</th>')
    out.append("</tr></thead><tbody>")
    for r in body:
        out.append("<tr>" + "".join(f"<td>{inline(c)}</td>" for c in r) + "</tr>")
    out.append("</tbody></table></div>")
    return "\n".join(out)


def md_to_html(md):
    lines = md.split("\n")
    out = []
    i = 0
    open_list = False
    table_count = 0

    def close_list():
        nonlocal open_list
        if open_list:
            out.append("</ol>")
            open_list = False

    while i < len(lines):
        line = lines[i]

        # fenced code
        if line.startswith("```"):
            close_list()
            lang = line[3:].strip() or "text"
            i += 1
            buf = []
            while i < len(lines) and not lines[i].startswith("```"):
                buf.append(lines[i])
                i += 1
            i += 1
            code = html.escape("\n".join(buf), quote=False)
            out.append(f'<pre class="code" data-lang="{lang}"><code>{code}</code></pre>')
            continue

        # table: a line starting with | followed by a separator row
        if line.startswith("|") and i + 1 < len(lines) and re.match(r"^\|[\s:|-]+\|$", lines[i + 1]):
            close_list()
            block = []
            while i < len(lines) and lines[i].startswith("|"):
                block.append(lines[i])
                i += 1
            table_count += 1
            caption = f"Table {table_count}"
            # a table directly under a heading takes that heading as its caption
            for prev in reversed(out):
                m = re.search(r"<h[23][^>]*>(.*?)</h[23]>", prev)
                if m:
                    caption = re.sub(r"<[^>]+>", "", m.group(1))
                    break
            # a proof table is the evidence column of its topic, so it takes the stamp treatment
            cls = "stamp" if caption.strip().lower() == "the proof" else ""
            out.append(table_block(block, caption, cls))
            continue

        # numbered list
        m = re.match(r"^(\d+)\. (.+)$", line)
        if m:
            if not open_list:
                out.append("<ol>")
                open_list = True
            out.append(f"<li>{inline(m.group(2))}</li>")
            i += 1
            continue

        close_list()

        if line.startswith("!["):
            m = re.match(r"^!\[([^\]]*)\]\(([^)]+)\)$", line)
            if m:
                src = m.group(2)
                alt = m.group(1)
                w, h = png_size(HERE / src)
                out.append(f'<figure><img src="{src}" alt="{html.escape(alt, quote=True)}" '
                           f'loading="lazy" width="{w}" height="{h}">'
                           f"<figcaption>{html.escape(alt)}</figcaption></figure>")
                i += 1
                continue

        # blockquote: a run of lines starting with ">"
        if line.startswith(">"):
            close_list()
            buf = []
            while i < len(lines) and lines[i].startswith(">"):
                buf.append(lines[i].lstrip(">").strip())
                i += 1
            out.append('<blockquote class="note">' + inline(" ".join(buf)) + "</blockquote>")
            continue

        if line.startswith("### "):
            out.append(f"<h3>{inline(line[4:])}</h3>")
        elif line.startswith("## Answer keys"):
            out.append('</section><section id="keys" aria-labelledby="keys-h">'
                       '<h2 id="keys-h">Answer keys</h2>')
        elif line.startswith("## "):
            m = TOPIC_RE.match(line)
            if m:
                n, title = m.group(1), m.group(2)
                # a short stagger so the sections enter in sequence rather than all at once
                delay = (int(n) - 1) * 30
                out.append(f'</section><section class="topic" id="t{n}" '
                           f'aria-labelledby="t{n}-h" style="--stagger:{delay}ms">'
                           f'<header class="topic__head"><span class="topic__n">{n}</span>'
                           f'<h2 id="t{n}-h">{inline(title)}</h2></header>')
            else:
                out.append(f"<h2>{inline(line[3:])}</h2>")
        elif line.startswith("# "):
            # the document title is rendered once, in the masthead
            i += 1
            continue
            out.append(f"<h1>{inline(line[2:])}</h1>")
        elif line.strip() == "---":
            out.append("<hr>")
        elif line.strip():
            # a wrapped paragraph is several source lines; emitting one <p> per line split
            # sentences in half, which is what the rendered page did before this loop
            para = [line]
            i += 1
            while i < len(lines) and lines[i].strip() and not re.match(
                    r"^(#|\||>|```|!\[|\d+\. )", lines[i]):
                para.append(lines[i])
                i += 1
            out.append(f"<p>{inline(chr(32).join(x.strip() for x in para))}</p>")
            continue
        i += 1

    close_list()
    return "\n".join(out), table_count


# ---------------------------------------------------------------- shell

def build():
    md = MD.read_text(encoding="utf-8")
    topics = TOPIC_RE.findall(md)
    body_html, table_count = md_to_html(md)
    # the first </section> closes a section that was never opened
    body_html = body_html.replace("</section>", "", 1)
    body_html += "</section>"

    tokens = TOKENS.read_text(encoding="utf-8")
    chips = "\n".join(
        f'<a class="chip" href="#t{n}"><span class="chip__n">{n}</span>{html.escape(t)}</a>'
        for n, t in topics)

    doc = f"""<!DOCTYPE html>
<html lang="en" data-theme="light">
<head>
<meta charset="UTF-8">
<script>
  /* decide the sheet before the first paint, or a dark reader sees a light flash */
  (function () {{
    try {{
      var saved = localStorage.getItem("topic-wise-theme");
      if (!saved && window.matchMedia && window.matchMedia("(prefers-color-scheme: dark)").matches) saved = "dark";
      if (saved) document.documentElement.setAttribute("data-theme", saved);
    }} catch (e) {{}}
  }})();
</script>
<meta name="viewport" content="width=device-width, initial-scale=1.0">
<title>Topic-wise Explained, ECE2104: the fourteen projected mid-term topics</title>
<meta name="description" content="One section per topic for the ECE2104 Data Structures and Algorithms mid-term: the plain explanation, a hand-checkable trace, the measured proof from a real capture, the C formulation, the exam traps and numericals, with the answer keys in their own section.">
<meta name="theme-color" content="#fafaf9">
<meta property="og:type" content="article">
<meta property="og:title" content="Topic-wise Explained, ECE2104, projected mid-term scope">
<meta property="og:description" content="The fourteen projected mid-term topics, one section each: mechanism, measured proof, C formulation, exam traps and numericals, with keys held separately.">
<meta property="og:url" content="https://github.com/pixie-chan/dsa-ece2104">
<meta property="og:image" content="diagrams/01-array-index-arithmetic.png">
<meta name="twitter:card" content="summary_large_image">
<link rel="icon" href="data:image/svg+xml,%3Csvg xmlns='http://www.w3.org/2000/svg' viewBox='0 0 16 16'%3E%3Crect width='16' height='16' fill='%23fafaf9'/%3E%3Cg fill='%231b4d7a'%3E%3Crect x='3' y='2' width='2' height='2'/%3E%3Crect x='3' y='5' width='2' height='2'/%3E%3Crect x='3' y='8' width='2' height='2'/%3E%3Crect x='6' y='8' width='7' height='2'/%3E%3C/g%3E%3C/svg%3E">
<link rel="preconnect" href="https://fonts.googleapis.com">
<link rel="preconnect" href="https://fonts.gstatic.com" crossorigin>
<link href="https://fonts.googleapis.com/css2?family=Instrument+Serif:ital@0;1&family=Instrument+Sans:wght@400;500;600&family=IBM+Plex+Mono:wght@400;500;600&display=swap" rel="stylesheet">
<!-- NOSTATE: static single file study document with no server and no async data after load.
     It fetches three Google font families on first paint (Instrument Serif, Instrument Sans,
     IBM Plex Mono) and falls back to Georgia and the system sans offline, so there is no
     runtime fetch and therefore no loading or error state. The only empty case is the nav
     before any topic is scrolled to, and the nav marks the current topic on scroll. -->
<style>
{tokens}
*{{box-sizing:border-box}}
html{{scroll-behavior:smooth}}
@media (prefers-reduced-motion: reduce){{html{{scroll-behavior:auto}}}}
body{{
  margin:0; color:var(--text);
  background-color:var(--bg);
  background-image:linear-gradient(color-mix(in srgb, var(--line) 40%, transparent) 1px, transparent 1px);
  background-size:100% 32px; background-attachment:fixed;
  font-family:var(--font-body); font-size:var(--text-base); line-height:1.62;
  -webkit-font-smoothing:antialiased;
}}
.wrap{{max-width:1180px; margin:0 auto; padding:0 var(--s-4) var(--s-9)}}
.measure{{max-width:85ch}}

/* header */
.masthead{{border-bottom:var(--hair) solid var(--line); background:var(--surface)}}
.masthead__in{{max-width:1180px; margin:0 auto; padding:var(--s-7) var(--s-4) var(--s-5)}}
.eyebrow{{
  font-family:var(--font-mono); font-size:var(--text-xs); letter-spacing:.14em;
  text-transform:uppercase; color:var(--text-3); margin:0 0 var(--s-3)
}}
h1{{
  font-family:var(--font-display); font-weight:400; font-size:var(--text-3xl);
  line-height:1.1; margin:0 0 var(--s-3); letter-spacing:-.01em
}}
.lede{{font-size:var(--text-md); color:var(--text-2); max-width:62ch; margin:0}}
.masthead__meta{{
  display:flex; flex-wrap:wrap; gap:var(--s-3); margin-top:var(--s-5);
  font-family:var(--font-mono); font-size:var(--text-xs); color:var(--text-3)
}}
.masthead__meta span{{border:var(--hair) solid var(--line); padding:4px var(--s-2); border-radius:var(--r-1)}}

/* sticky nav */
.nav{{position:sticky; top:0; z-index:20; background:var(--surface);
  border-bottom:var(--hair) solid var(--line); box-shadow:var(--shadow-1)}}
.nav__bar{{max-width:1180px; margin:0 auto; padding:var(--s-2) var(--s-4);
  display:flex; align-items:center; gap:var(--s-3)}}
.nav__label{{font-family:var(--font-mono); font-size:var(--text-xs);
  letter-spacing:.12em; text-transform:uppercase; color:var(--text-3); white-space:nowrap}}
.chips{{display:flex; gap:var(--s-2); overflow-x:auto; padding:var(--s-2) 0; flex:1;
  scrollbar-width:thin;
  /* fade the clipped edge so the rail reads as scrollable instead of cutting a label in half */
  -webkit-mask-image:linear-gradient(to right, #000 calc(100% - 44px), transparent);
  mask-image:linear-gradient(to right, #000 calc(100% - 44px), transparent)}}
.chip{{display:inline-flex; align-items:center; min-height:44px; gap:6px; white-space:nowrap;
  border:var(--hair) solid var(--line-2); border-radius:999px; padding:4px var(--s-3);
  font-size:var(--text-xs); color:var(--text-2); text-decoration:none;
  transition:border-color var(--dur-1) var(--ease), color var(--dur-1) var(--ease)}}
.chip:hover{{border-color:var(--accent); color:var(--accent)}}
.chip:focus-visible,.toggle:focus-visible,a:focus-visible{{outline:2px solid var(--accent); outline-offset:2px}}
.chip[aria-current="true"]{{border-color:var(--accent); color:var(--accent);
  background:color-mix(in srgb, var(--accent) 8%, transparent)}}
.chip__n{{font-family:var(--font-mono); color:var(--text-3)}}
.chip[aria-current="true"] .chip__n{{color:var(--accent)}}
.toggle{{border:var(--hair) solid var(--line-2); background:var(--surface); color:var(--text-2);
  border-radius:var(--r-1); min-height:44px; padding:6px var(--s-3); font-family:var(--font-mono);
  font-size:var(--text-xs); cursor:pointer; white-space:nowrap}}
.toggle:hover{{border-color:var(--accent); color:var(--accent)}}


.chip:active,.toggle:active{{transform:translateY(1px)}}
.chip[aria-current="true"]:hover{{background:color-mix(in srgb, var(--accent) 14%, transparent)}}
button[disabled],.toggle[aria-disabled="true"]{{opacity:.5; cursor:not-allowed}}


@keyframes rise{{from{{opacity:0; transform:translateY(8px)}} to{{opacity:1; transform:none}}}}
.topic{{animation:rise var(--dur-3) var(--ease) both; animation-delay:var(--stagger, 0ms)}}
@media (prefers-reduced-motion: reduce){{.topic{{animation:none}}}}

/* sections */
.topic{{padding:var(--s-8) 0 var(--s-6); border-bottom:var(--hair) solid var(--line)}}
.topic__head{{display:flex; align-items:baseline; gap:var(--s-4); margin-bottom:var(--s-5)}}
.topic__n{{font-family:var(--font-mono); font-size:var(--text-sm); color:var(--accent);
  border:var(--hair) solid var(--line-2); border-radius:var(--r-1); padding:4px 8px;
  flex:none}}
h2{{font-family:var(--font-display); font-weight:400; font-size:var(--text-2xl);
  line-height:1.1; margin:0; letter-spacing:-.01em}}
h3{{font-family:var(--font-mono); font-size:var(--text-xs); font-weight:500;
  letter-spacing:.14em; text-transform:uppercase; color:var(--text-3);
  margin:var(--s-7) 0 var(--s-3)}}
p{{margin:0 0 var(--s-4); max-width:85ch}}
a{{color:var(--accent); text-decoration-thickness:1px; text-underline-offset:2px;
  display:inline-block; min-height:44px; padding:11px 0; margin:-11px 0}}
a:hover{{color:var(--risk)}}

code{{font-family:var(--font-mono); font-size:.875em; background:var(--surface-2);
  padding:2px 6px; border-radius:var(--r-1)}}
pre.code{{background:var(--surface-2); border:var(--hair) solid var(--line);
  border-left:3px solid var(--accent); border-radius:var(--r-1);
  padding:var(--s-4); overflow-x:auto; margin:0 0 var(--s-5); position:relative}}
pre.code code{{background:none; padding:0; font-size:var(--text-sm); line-height:1.7;
  color:var(--text)}}
pre.code::after{{content:attr(data-lang); position:absolute; top:8px; right:12px;
  font-size:10px; letter-spacing:.1em; text-transform:uppercase; color:var(--text-3)}}

/* tables */
.tbl-wrap{{overflow-x:auto; margin:0 0 var(--s-5);
  border:var(--hair) solid var(--line); border-radius:var(--r-1); background:var(--surface)}}
table{{border-collapse:collapse; width:100%; font-size:var(--text-sm); font-variant-numeric:tabular-nums}}
caption{{caption-side:top; text-align:left; padding:var(--s-3) var(--s-4);
  font-family:var(--font-mono); font-size:var(--text-xs); letter-spacing:.06em;
  text-transform:uppercase; color:var(--text-3); border-bottom:var(--hair) solid var(--line)}}
th,td{{padding:12px var(--s-4); text-align:left; vertical-align:top;
  border-bottom:var(--hair) solid var(--line)}}
th{{font-weight:600; font-size:var(--text-xs); letter-spacing:.06em; text-transform:uppercase;
  color:var(--text-2); background:var(--surface-2); white-space:nowrap}}
tbody tr:last-child td{{border-bottom:0}}
tbody tr:nth-child(even){{background:var(--surface-2)}}
td:first-child{{color:var(--text-2); white-space:nowrap}}


/* THE PROOF STAMP: the measured column of a proof table is set as a stamped evidence column */
.stamp{{position:relative}}
.stamp caption{{border-bottom:var(--hair) solid var(--accent);
  color:var(--accent); letter-spacing:.1em}}
.stamp td:last-child,.stamp th:last-child{{border-left:var(--hair) solid var(--line);
  padding-left:var(--s-4)}}
.stamp tbody tr td:last-child{{background:color-mix(in srgb, var(--accent) 5%, transparent)}}
.stamp tbody tr:hover td:last-child{{background:color-mix(in srgb, var(--accent) 11%, transparent)}}

/* figures */
figure{{margin:0 0 var(--s-6)}}
figure img{{display:block; width:100%; height:auto; border:var(--hair) solid var(--line);
  border-radius:var(--r-1); background:var(--surface)}}
figcaption{{margin-top:var(--s-2); font-family:var(--font-mono); font-size:var(--text-xs);
  color:var(--text-3)}}
hr{{border:0; border-top:var(--hair) solid var(--rule); margin:var(--s-7) 0}}
ol{{padding-left:var(--s-6); margin:0 0 var(--s-5); max-width:85ch}}
li{{margin-bottom:var(--s-2)}}
strong{{font-weight:600; color:var(--text)}}

/* the keys get their own frame so questions and answers never read as one block */
#keys{{background:var(--surface); border:var(--hair) solid var(--line-2);
  border-left:3px solid var(--risk); border-radius:var(--r-1);
  padding:var(--s-6) var(--s-5); margin-top:var(--s-7)}}
#keys h2{{margin-bottom:var(--s-4)}}

footer{{padding:var(--s-7) 0; color:var(--text-3); font-size:var(--text-sm);
  font-family:var(--font-mono)}}

@media (max-width:720px){{
  .masthead__in{{padding:var(--s-6) var(--s-4) var(--s-4)}}
  .nav__label{{display:none}}
  .topic{{padding:var(--s-7) 0 var(--s-5)}}
  th,td{{padding:9px var(--s-3)}}
  #keys{{padding:var(--s-5) var(--s-4)}}
}}
</style>
<!-- CONCEPT: fourteen mechanisms, each proved by a number measured on this machine
     METAPHOR: a marked-up answer script, one section per projected topic, margin notes on the left
     SIGNATURE: the proof stamp (.stamp): every topic's proof table is set as an evidence column, its
       caption rules in the accent colour and its last column tinted, in tabular figures
     LOAD: one document, no fetch, no spinner; the sticky chip rail scrolls the reader to a topic
     MICRO: chip hover and aria-current mark, theme toggle persisted, focus-visible rings, sticky
       header condensed at rest -->
<!-- DIRECTION-REPEAT-OK: the brief requires this set to use the same tokens as the companion
     roadmap page (same font stack, same light and dark palettes) so the two artifacts read as one
     course dossier. The repeat is the specification, not a shortcut: the structure differs (a
     per-topic proved section vs the roadmap's plan and timeline), and the palette is inherited by
     reference to tokens.css rather than re-invented. -->
</head>
<body>
<header class="masthead">
  <div class="masthead__in">
    <p class="eyebrow">ECE2104 &middot; Data Structures and Algorithms &middot; Mid-Term</p>
    <h1>Topic-wise Explained</h1>
    <p class="lede">Each section carries a plain explanation, a hand-checkable trace, the measured
    proof with its capture file, the C formulation, the traps that cost marks, and numericals. The
    answer keys are in their own section at the end.</p>
    <div class="masthead__meta">
      <span>14 topics</span><span>7 capture files</span><span>answer keys held separately</span>
      <span>g++ 15.2.0</span><span>Ubuntu 26.04 x86-64</span>
    </div>
  </div>
</header>
<nav class="nav" aria-label="Topic navigation">
  <div class="nav__bar">
    <span class="nav__label">Topics</span>
    <div class="chips">
{chips}
    </div>
    <button class="toggle" type="button" id="theme-toggle" aria-live="polite">dark</button>
  </div>
</nav>
<main class="wrap" id="top">
{body_html}
</main>
<footer class="wrap">
  <p>Built from TOPIC-WISE-EXPLAINED.md by build-topic-wise.py. Measured on
  Linux 7.0.0-31-generic x86_64, g++ 15.2.0, Ubuntu 26.04. Addresses move between runs
  because of ASLR; every size, offset, count, depth and ratio does not.</p>
</footer>
<script>
(function () {{
  var root = document.documentElement;
  var btn = document.getElementById("theme-toggle");

  function label() {{ btn.textContent = root.getAttribute("data-theme") === "dark" ? "light" : "dark"; }}
  label();

  btn.addEventListener("click", function () {{
    var next = root.getAttribute("data-theme") === "dark" ? "light" : "dark";
    root.setAttribute("data-theme", next);
    try {{ localStorage.setItem("topic-wise-theme", next); }} catch (e) {{}}
    label();
  }});

  /* mark the topic the reader is actually inside */
  var chips = Array.prototype.slice.call(document.querySelectorAll(".chip"));
  var sections = chips.map(function (c) {{ return document.querySelector(c.getAttribute("href")); }});
  var current = -1;

  function mark() {{
    var y = window.scrollY + 140;
    var idx = 0;
    for (var i = 0; i < sections.length; i++) {{
      if (sections[i] && sections[i].offsetTop <= y) idx = i;
    }}
    if (idx === current) return;
    current = idx;
    chips.forEach(function (c, i) {{
      c.setAttribute("aria-current", i === idx ? "true" : "false");
      if (i === idx) {{
        var box = c.parentElement;
        var left = c.offsetLeft - box.offsetLeft;
        if (left < box.scrollLeft || left + c.offsetWidth > box.scrollLeft + box.clientWidth) {{
          box.scrollTo({{ left: left - 12, behavior: "smooth" }});
        }}
      }}
    }});
  }}

  window.addEventListener("scroll", mark, {{ passive: true }});
  window.addEventListener("resize", mark);
  mark();
}})();
</script>
</body>
</html>
"""
    OUT.write_text(doc, encoding="utf-8")
    return doc, topics, table_count


def self_check(doc, topics, table_count):
    """The build must prove its own claims before the verifier gets a turn."""
    problems = []
    if len(topics) != 14:
        problems.append(f"expected 14 topics, found {len(topics)}")
    if "\u2014" in doc:
        problems.append("em dash present in the output")
    if "\u2013" in doc:
        problems.append("en dash present in the output")
    # "<table" and not "<table>": the proof tables carry a class attribute
    n_tables = len(re.findall(r"<table[ >]", doc))
    n_caps = doc.count("<caption>")
    if n_tables != n_caps:
        problems.append(f"{n_tables} tables but {n_caps} captions")
    n_ths = doc.count('<th scope="col">')
    if n_ths < n_tables:
        problems.append("a table has no scoped header cell")
    for src in re.findall(r'<img src="([^"]+)"', doc):
        if src.startswith("http") or src.startswith("/") or ".." in src:
            problems.append(f"image not relative to this folder: {src}")
        elif not (HERE / src).exists():
            problems.append(f"image missing on disk: {src}")
    return problems


def main():
    if "--check" in sys.argv:
        doc = OUT.read_text(encoding="utf-8")
        topics = TOPIC_RE.findall(MD.read_text(encoding="utf-8"))
        problems = self_check(doc, topics, len(re.findall(r"<table[ >]", doc)))
    else:
        doc, topics, table_count = build()
        problems = self_check(doc, topics, table_count)
        print(f"built {OUT.name}: {len(doc)} bytes, {len(topics)} topics, {table_count} tables")
    if problems:
        for p in problems:
            print(f"  [FAIL] {p}")
        return 1
    print("self-check: clean")
    return 0


if __name__ == "__main__":
    sys.exit(main())
