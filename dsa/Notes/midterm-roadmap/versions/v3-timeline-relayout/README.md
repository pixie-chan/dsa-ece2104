# v3-timeline-relayout snapshot

The block timeline made wide and short, 2026-09-18 23:31 IST. Taken after the fix, not before:
the pre-fix byte copy lives in `.qa/pre-timeline-fix-*.html` (three hashes, see below).

| Field | Value |
|---|---|
| Artifact | DSA-MIDTERM-ROADMAP-v3-timeline-relayout.html (byte copy of the live file, verified by sha256sum) |
| Bytes | 230493 |
| sha256 | 2131627215d8df3870a4d41fbe217e056824e755b04ed58e1efd6985dc61626b |
| Rebuild note | hand-authored single-file HTML; the timeline change is owned by `~/scripts/roadmap-timeline-fix.py`, which is idempotent |
| Gate | ui-gate.py --direction editorial --project dsa-midterm-roadmap --creative --vision-rate 8, 14/22 PASS, G7 PERF the only FAIL (pre-existing: the 225 KB file is over the 200 KB single-file rail, plus a setTimeout without clearTimeout) |
| CDP | ui-cdp-qa.py at 1783x900 and 390x844, 0 FAILs, 0 JS errors, no horizontal overflow at any of ten widths |

## What was wrong, measured in headless Chromium

| Fault | Before (1440x1000) | After (1440x1000) |
|---|---|---|
| Track count | six `grid-template-columns` for seven slabs, exam in an implicit seventh | seven tracks, `exam` label aligned |
| Axis labels | six labels: `exam` sat over Block F | seven labels |
| Single-lecture slabs | Block C 51 px, Block E 51 px, body copy 21 and 28 one-word lines | 168 px each, 2 and 3 normal lines |
| Row height | 1407 px (all slabs stretched to the tallest sliver) | 319 px |
| Block A body | 3 lines | 4 lines |
| Row shape | 1361 x 1407 | 1361 x 436 |

## Also fixed

- `#rm-count-f` on Block F read `0 of 15 ticked` and could never move: the spine carries lectures
  1 to 16 and Block F covers 17 to 31, so no tick maps to it. It now reads `15 lectures, past the
  spine`, and the JS `paint()` pass skips it instead of overwriting the text on load.
- `.slab__count` had no `display:block`, so `0 of 4 ticked` wrapped to three lines inside the
  narrow columns.
- The four slabs' screen-reader text named section numbers that the v2 rewrite had renumbered
  (`section 03, the chain` -> 04, and three others).
- The timeline caption claimed widths were lecture counts "drawn to scale"; the two single-lecture
  slabs are now held at a readable floor, so the caption says that.

## Concurrent writer

A second agent session rewrote this file wholesale at 23:17 IST and silently dropped the first
round of hand edits (which is why the change is now a re-runnable script in `~/scripts/`). Snapshots
taken, each a byte copy verified by `sha256sum` against the live file at the time:

| Snapshot | sha256 (first 8) | Bytes |
|---|---|---|
| `.qa/pre-timeline-fix-b0d9012a.html` | b0d9012a | 228903 |
| `.qa/pre-timeline-fix-70035dbc.html` | 70035dbc | 229339 |
| `.qa/pre-timeline-fix-a50edb42.html` | a50edb42 | 229371 |
