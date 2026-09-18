# v2-pre-timeline snapshot

Taken before the block-timeline relayout on 2026-09-18 23:12 IST, because the shipped v2
revision (the projected-MTE-list rewrite) renders the timeline as a wall.

| Field | Value |
|---|---|
| Artifact | DSA-MIDTERM-ROADMAP-v2-pre-timeline.html |
| Bytes | 224788 |
| Lines | 1121 |
| sha256 | 9641bcb875092473bc040d5c2a8aca8541d9ee4ebbc1dddf380060c63eb957c3 |
| Copy proof | `sha256sum` of the copy and of the live file printed the same digest |
| Rebuild note | hand-authored single-file HTML, this snapshot is a byte copy |

## Why it was superseded

Measured in headless Chromium at 1440x1000 (`.qa/before-timeline-1440x1000.png`):

| Slab | Column width | Height |
|---|---|---|
| Block A, Foundation | 204 px | 1407 px |
| Block B, Memory and lists | 255 px | 1407 px |
| Block C, class test 1 | 51 px | 1407 px |
| Block D, Stacks and queues | 255 px | 1407 px |
| Block E, Recursion | 51 px | 1407 px |
| Block F, projected MTE scope | 232 px | 1407 px |
| Exam | 300 px (implicit 7th track) | 1407 px |

Three faults, all machine-measured:

1. `grid-template-columns` declared six tracks for seven slabs, so the exam slab landed in an
   implicit seventh track and the axis row carried six labels: the label `exam` sat over Block F.
2. The two single-lecture slabs were sized `minmax(0,1fr)` against the 4fr and 5fr teaching
   blocks, so Block C and Block E rendered 44 to 53 px wide and their body copy wrapped to
   19 to 49 lines of one-word-per-line text.
3. `align-items:stretch` then stretched every other slab to the height of those two slivers,
   so a seven-card timeline occupied a 1361 x 1407 px box (2319 px tall at 1280 viewport).
