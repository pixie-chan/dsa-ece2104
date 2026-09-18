# v5-pre-og snapshot

The page as it stood before the share-card pass, 2026-09-19 00:09 IST. Taken before the fix, not
after: A12 made the social card URLs absolute.

| Field | Value |
|---|---|
| Artifact | DSA-MIDTERM-ROADMAP.html (byte copy of the live file, verified by sha256sum) |
| Bytes | 230529 |
| sha256 | 19f350175c8da8bab37902a645dd241d9dc3c0fcb1a3fdee9a8c8cff905665d8 |
| What changed after it | five head tags: `og:url`, `og:image`, plus the missing `twitter:image`, `twitter:title` and `twitter:description` |
| Record | `DSA-MIDTERM-ROADMAP-REVIEW.md` round 22, `DSA-MIDTERM-ROADMAP-APPLY.md` A12 |
| Test result at this revision | health battery 15 of 15, leftovers probe 8 of 8, token gate clean |

## Why it is here

At this revision `og:image` was the bare string `og-card.png`. A relative URL in a share card never
resolves, because every scraper fetches metadata without a document base, so the card was broken on
every platform rather than only after publication. `og:url` also named the repo root instead of the
page. Both are absolute now, against the host that serves the file today, and every one of those URLs
returns 200.

## Note on the diagram references

The figure tags inside this file point at `diagrams/`, which sits beside the live page one folder up.
Opening this snapshot in place shows empty figure boxes, as it does for every snapshot in this folder.
The images themselves are intact in the live tree and in `versions/v1-approved/`.
