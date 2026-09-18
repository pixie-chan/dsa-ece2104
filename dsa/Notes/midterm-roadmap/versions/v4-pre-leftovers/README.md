# v4-pre-leftovers snapshot

The page and both handoff documents as they stood before the Batch 2 leftovers were applied,
2026-09-18 23:56 IST. Taken before the fix, not after.

| Field | Value |
|---|---|
| Artifact | DSA-MIDTERM-ROADMAP.html, plus the APPLY and REVIEW documents as they read at that moment |
| Bytes | 230493 (page) |
| sha256 | 2131627215d8df3870a4d41fbe217e056824e755b04ed58e1efd6985dc61626b |
| What changed after it | B4 the clipped meta description tail, B5 the one unwrapped table, B9 the duplicated `.slab__link` declaration |
| Record | `DSA-MIDTERM-ROADMAP-REVIEW.md` round 21, and the "Batch 2 closed out" section of the apply list |
| Test result at this revision | health battery 15 of 15 PASS, no page errors, no overflow at 375px |

This is the same revision the previous snapshot folder holds (`v3-timeline-relayout`), which is not a
mistake: v3 recorded the timeline relayout, and the two later rounds changed only four lines between
them, so the byte copy is identical. The next snapshot, `v5-pre-og`, is where the bytes move again.

## Note on the diagram references

The figure tags inside the page point at `diagrams/`, which sits beside the live page one folder up.
Opening this snapshot in place shows empty figure boxes, as it does for every snapshot in this folder.
The images themselves are intact in the live tree and in `versions/v1-approved/`.
