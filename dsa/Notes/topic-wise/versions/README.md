# Snapshot index

One byte copy of the shipped pair, taken after the UI gate returned exit 0 and the verifier returned
250/250 with 11/11 mutations caught. This folder did not exist before the build, so there was nothing
older to preserve; the copy here is the approved first revision, and any later edit snapshots first.

| Folder | Taken (IST) | sha256 (first 8) | Bytes | Why it exists |
|---|---|---|---|---|
| `v1-approved` | 2026-09-19 00:34 | `54b85306` | 125410 | the first complete revision: 14 topics, 243 to 250 checks green, gate exit 0 |

## Full digests

```
54b85306191a63ca9497533cb05b4cf78d49fbaf322de13a24eb3c5b8736eae3  v1-approved/TOPIC-WISE-EXPLAINED-v1-approved.html
734c931218932a16707218d1869bbd47c2c7831838bf3f9cfffe28106db7c4bd  v1-approved/TOPIC-WISE-EXPLAINED-v1-approved.md
```

## What this revision is

`TOPIC-WISE-EXPLAINED.md` is the source of truth and `TOPIC-WISE-EXPLAINED.html` is built from it, so
the snapshot carries both: a reader can diff the source and the rendered page against their own copies.

## The exact build command

```
cd ~/Documents/MUJ-SEM-3/DSA/Notes/topic-wise && python3 build-topic-wise.py
```

That reads `TOPIC-WISE-EXPLAINED.md`, inlines `../midterm-roadmap/tokens.css`, and writes the single
self-contained `TOPIC-WISE-EXPLAINED.html`. The figures are re-rendered separately with:

```
cd ~/Documents/MUJ-SEM-3/DSA/Notes/topic-wise && bash render-diagrams.sh
```

## The verification behind the approval

```
python3 ~/learning/cpp/exercises/verify_topic_wise.py --mutate      # 250/250, 11/11 caught, exit 0
python3 ~/.hermes/scripts/ui-gate.py TOPIC-WISE-EXPLAINED.html --direction editorial \
        --project topic-wise-explained --creative --vision-rate 8   # 13 PASS, 6 WARN, 0 FAIL, exit 0
```

The gate log is kept at `../.qa/ui-gate-run.log` and the verifier transcript at
`~/learning/cpp/exercises/verify_topic_wise_run_output.txt`.

## One thing that looks like drift and is not

The embedded figures are relative paths, so opening a snapshot file in place shows empty figure boxes:
the images sit in `../diagrams/`. The live page renders them; the snapshots are for byte comparison.
