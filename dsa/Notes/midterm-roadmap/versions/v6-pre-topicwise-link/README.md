# v6-pre-topicwise-link

A byte copy of `../DSA-MIDTERM-ROADMAP.html` taken immediately BEFORE one change: the cross-link
from the roadmap to the topic-wise explanation set was added.

| Field | Value |
|---|---|
| sha256 | `cbc321cd4a017fb8f053947c42fdca0376c326e2056e6ae8042b882b3d49fb48` |
| Bytes | 231128 |
| Taken (IST) | 2026-09-18 23:59 |
| Live file at the time | byte-identical to this copy |

## What this revision was

The verified roadmap at the end of the Batch 2 leftovers round: fourteen projected topics, the scope
conflict section, the sorting, tree, analysis and formulation modules, the mock paper and the numeric
set, all green under its own gate history. Nothing about it was wrong. It simply had no outbound link
to the topic-wise explanation set that was built beside it.

## Why the snapshot exists

The topic-wise set links INTO this page (`#s5`, the topic plan section), so a reader arriving here
had no way back. Adding the return link meant editing a verified artifact, and the rule for that is
to snapshot first so the pre-edit bytes are recoverable by hash rather than by memory.

## The build command

The page is hand-authored HTML with one token stylesheet exported from itself. There is no build
step, so "the build command" is the verification pair that was run against it:

```
python3 ~/learning/cpp/exercises/verify_mte_guide.py
python3 ~/.hermes/scripts/ui-gate.py DSA-MIDTERM-ROADMAP.html --direction editorial --project dsa-midterm-roadmap --creative --vision-rate 8
```

## The change that followed

One anchor was added inside the topic plan section (`id="s5"`): a link to
`../topic-wise/TOPIC-WISE-EXPLAINED.html`, labelled with the fourteen projected topics. No existing
markup, id or style was altered, so the snapshot and the live file differ by that anchor alone.
