# Snapshot index

Five byte copies of `../DSA-MIDTERM-ROADMAP.html`, one per accepted round of change. Each folder
carries its own README with the reason, the hash and the test result at that moment; this page is the
one-glance index.

| Folder | Taken (IST) | sha256 (first 8) | Bytes | Why it exists |
|---|---|---|---|---|
| `v1-approved` | 2026-09-18 22:37 | `7fc0de2d` | 90145 | the approved first shell, before the projected-MTE-list rewrite |
| `v2-pre-timeline` | 2026-09-18 23:12 | `9641bcb8` | 224788 | before the block-timeline relayout, because this revision rendered the timeline as a wall |
| `v3-timeline-relayout` | 2026-09-18 23:31 | `21316272` | 230493 | taken after the relayout: the wide, short timeline |
| `v4-pre-leftovers` | 2026-09-18 23:56 | `21316272` | 230493 | before the Batch 2 leftovers. Byte-identical to v3, because nothing changed the page in between; it also carries the APPLY and REVIEW documents as they read at that moment |
| `v5-pre-og` | 2026-09-19 00:09 | `19f35017` | 230529 | before the share-card URLs were made absolute, which is A12 |

Live page as of the last pass: `cbc321cd`, 231128 bytes.

## Full digests

```
7fc0de2dea029f63dc02e7b78563fb4b93f36805751def025bc83e48162a5484  v1-approved/DSA-MIDTERM-ROADMAP-v1-approved.html
9641bcb875092473bc040d5c2a8aca8541d9ee4ebbc1dddf380060c63eb957c3  v2-pre-timeline/DSA-MIDTERM-ROADMAP-v2-pre-timeline.html
2131627215d8df3870a4d41fbe217e056824e755b04ed58e1efd6985dc61626b  v3-timeline-relayout/DSA-MIDTERM-ROADMAP-v3-timeline-relayout.html
2131627215d8df3870a4d41fbe217e056824e755b04ed58e1efd6985dc61626b  v4-pre-leftovers/DSA-MIDTERM-ROADMAP.html
19f350175c8da8bab37902a645dd241d9dc3c0fcb1a3fdee9a8c8cff905665d8  v5-pre-og/DSA-MIDTERM-ROADMAP.html
```

## How a snapshot is taken

Copy the live file with `cp -p` so the mtime travels with it, then compare `sha256sum` on both so the
copy is proven rather than assumed. Write the README in the same pass, while the reason is still
measured, and name the round in `../DSA-MIDTERM-ROADMAP-REVIEW.md` that records the change.

## One thing that looks broken and is not

The figure tags inside each snapshot point at `diagrams/`, the folder that sits beside the live page
one level up. Opening a snapshot in place therefore shows empty figure boxes, for every folder here.
The images are intact in `../diagrams/` and in `v1-approved/`.
