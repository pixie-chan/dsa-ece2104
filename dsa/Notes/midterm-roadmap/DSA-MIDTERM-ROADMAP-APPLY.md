# Apply list: DSA-MIDTERM-ROADMAP.html

The do-now list. Twelve patches, ordered by value, each with the literal edit and a check. Every item here is now applied, including the three Batch 2 leftovers (B4, B5, B9) closed at revision `19f350175c8da8ba`. Deeper evidence for every item, plus the record of what is already fixed, is in `DSA-MIDTERM-ROADMAP-PATCHES.md`; the measurements behind each claim are in `DSA-MIDTERM-ROADMAP-REVIEW.md`.

- **Target:** `~/Documents/MUJ-SEM-3/DSA/Notes/midterm-roadmap/DSA-MIDTERM-ROADMAP.html`
- **Current revision:** `sha256 cbc321cd4a017fb8`, 231128 bytes, mtime 2026-09-19 00:09:38 IST. Everything below is applied at this revision, and the health battery passes 15 of 15 rows.
- **Pre-edit snapshots:** `versions/v5-pre-og/` holds the page at `19f350175c8da8ba`, before the share-card pass, and `versions/v4-pre-leftovers/` holds the page and both documents at `2131627215d8df38`, before the leftovers.
- **Pinned revision this list was validated against:** `sha256 2131627215d8df38`, 230493 bytes, mtime 2026-09-18 23:29:35 IST
- **Careful with line numbers.** Revision `10634400` reformatted the file from 2632 lines to 1121 by compacting whitespace, so the line numbers written when this list was drafted (against `7fc0de2d` and `c13596a0`) are stale. Every literal snippet in this document was re-validated against `10634400` and still matches exactly once, except the nine-image prefix in B6 which matches nine times by design. Locate every edit by its snippet.

## Every patch here was test-applied and verified

A1 to A11 were applied to a scratch copy at `~/.cache/roadmap-qa/try.html` (the real page was not touched) and the full battery was run against it. All three scripts still parse, and every functional row flips to PASS:

```
page errors               (only the scratch copy's missing relative diagram fails to load)
spine logger exported     PASS
navigator render handshake PASS
meter after 5 ticks       PASS
aria-valuenow             PASS
slab counter + fill       PASS
slab jump link            PASS
navigator scroll spy      PASS
navigator chip + stepper  PASS
trail numbering           PASS
spine section ranges      PASS
timeline tick meets rule  PASS
rail wheel chaining       PASS
mobile 375 overflow       PASS
OS dark preference        PASS
```

Plus, against the same copy: the numbering interleave is `1, 2, 3, 4` (it is `1, 2, 1, 2` before A2), the print handler's `double` count is 2 instead of 12 (A7), and the wheel gain over the bar equals the gain over the content at 1200px each (A11). The probes pointed at the scratch copy are `~/.cache/roadmap-qa/health_try.mjs`, `verify14_try.mjs`, `verify4_try.mjs` and `wheel_try.mjs`.

The single console error in that run is `diagrams/03-concept-map.svg -> net::ERR_FILE_NOT_FOUND`, an artifact of the scratch copy living outside the project folder, not a page defect.

## Batch 1 is applied, and it holds

At revision `c13596a0952abdae` (2632 lines, 237504 bytes, mtime 23:00:39) all of A1 to A11 are present in the live file: every "want gone" marker is gone and every replacement is in place. The full health battery passes 15 of 15 rows, including the five that failed before: navigator handshake, spine ranges, timeline gap, rail wheel chaining and the OS dark preference.

In the same window the page also gained a large content expansion, which is what Batch 2 below deals with.

## Rules

1. Hash first: `sha256sum ~/Documents/MUJ-SEM-3/DSA/Notes/midterm-roadmap/DSA-MIDTERM-ROADMAP.html`. If it is not `7fc0de2dea029f63`, find each edit by its literal snippet, not by line number.
2. One patch at a time, then its check. Do not reformat, rename or reorder anything else.
3. After the edits, all three inline scripts must still parse:

```
awk 'BEGIN{n=0} /<script>/{n++; f=sprintf("%s/s%d.js", ENVIRON["HOME"]"/.cache/roadmap-qa", n); next} /<\/script>/{f=""; next} f!=""{print > f}' ~/Documents/MUJ-SEM-3/DSA/Notes/midterm-roadmap/DSA-MIDTERM-ROADMAP.html; for f in ~/.cache/roadmap-qa/s*.js; do node --check "$f" || echo "FAIL $f"; done
```

4. Full behavioural check in one command, before and after: `node ~/.cache/roadmap-qa/health.mjs`
5. Keep the page's voice in any copy change: plain sentences, no em dashes, nothing unmeasured.

## Order and summary

| # | Was | Fix | Effort |
|---|---|---|---|
| A1 | P14 | Mobile: the spine pins over 60 percent of a phone screen | 1 line added |
| A2 | P1 | Navigator trail numbering splits into two counters | 2 line edits |
| A3 | P3 | Section 7 never highlights the spine | 1 edit |
| A4 | P5 | Rail wheel freeze | delete 1 clause |
| A5 | P6 | OS dark preference overridden and persisted | 1 line |
| A6 | P8 | Twelve `SHOW ANSWER` labels in print | 1 selector |
| A7 | P7 | Printing twice leaves every answer open | 1 block |
| A8 | P9 | Timeline ticks stop 21px short of the rule | 1 line |
| A9 | P10 | 144 against 192 in the same list | 1 line |
| A10 | P11 | Cleanup: dead selector, dead id, invalid head script, unused tokens | 4 small edits |
| A11 | P13 | Chip strip taxes vertical wheel | delete 1 block |
| A12 | P4 | Share-card URLs were relative | FIXED, absolute and live-verified |

---

## A1. Mobile: the spine pins over the content (P14, high)

**Problem:** line 95 has `@media (max-width:1040px){ ... .rail{position:static} ... }`, but the base rule at lines 154 to 155 comes later with equal specificity, so it wins. Measured at 375x812: computed `position` is `sticky`, the rail is 487px tall, and at scrollY 3000 the element at viewport `(180, 60)`, `(180, 200)` and `(180, 400)` is the rail. About 60 percent of a phone screen is covered by the spine for the whole document.

**Edit.** Immediately after line 155, add:

```css
@media (max-width:1040px){ .rail{position:static; max-height:none; overflow:visible} }
```

Do not remove the desktop clamp on lines 154 to 155; it is what makes the rail's controls reachable.

**Check:** at 375px, `getComputedStyle(document.querySelector(".rail")).position` is `static`, and at scrollY 3000 the element at `(180, 200)` is page content.

---

## A2. Navigator trail numbering (P1, degraded)

**Problem:** the export at line 1147 is dead code, because `render` belongs to the first IIFE. `typeof` keeps it from throwing, but the value is always `null`, so the navigator falls back to its own counter. Measured on a fresh load: chip, next, then two ticks, and the trail reads `1, 2, 1, 2`.

**Edit 1.** Delete these two lines (1146 and 1147):

```js
  /* two things the section navigator appended below needs from this closure */
  window.__rmRender = typeof render === "function" ? render : null;
```

**Edit 2.** Export from the scope that owns the function. At line 990 the standalone call, with its context:

```js
  });
  render();

  /* the spine follows the reader */
```

becomes:

```js
  });
  render();
  window.__rmRender = render;

  /* the spine follows the reader */
```

Order matters: the first IIFE runs before patch A, so patch A must not assign to `window.__rmRender` at all.

**Check:** `node ~/.cache/roadmap-qa/verify14.mjs` prints `rmRender: "function"` and an interleave of `1, 2, 3, 4`. `node ~/.cache/roadmap-qa/health.mjs` shows the handshake row as PASS.

---

## A3. Section 7 range (P3, medium)

**Problem:** lines 998 to 999 require exactly two values, so `<section id="s7" data-lectures="16">` (line 758) never enters `RANGES` and the recursion section highlights no ticks. Measured: `s7` = none, `s3` = 5,6,7,8,9.

**Edit.** Replace lines 998 to 999:

```js
      var parts = (marked[m].getAttribute("data-lectures") || "").split(/\s+/);
      if (parts.length === 2) RANGES[marked[m].id] = [parseInt(parts[0], 10), parseInt(parts[1], 10)];
```

with:

```js
      var parts = (marked[m].getAttribute("data-lectures") || "").trim().split(/\s+/);
      var lo = parseInt(parts[0], 10);
      if (isNaN(lo)) continue;
      var hi = parts.length > 1 ? parseInt(parts[1], 10) : lo;
      RANGES[marked[m].id] = [lo, isNaN(hi) ? lo : hi];
```

**Check:** scroll to `#s7`, tick 16 carries `data-current="true"`; `health.mjs` shows the ranges row as PASS.

---

## A4. Rail wheel freeze (P5, medium)

**Problem:** `overscroll-behavior:contain` on the rail blocks scroll chaining. Measured: page frozen at 1200 with the rail at its maximum and two further wheel events doing nothing, while the same events over the content scroll normally. The rail is 322px of viewport width.

**Edit.** Line 155, delete the clause, keeping everything else:

```css
  max-height:calc(100dvh - var(--s-6)); overflow-y:auto; overscroll-behavior:contain; padding-right:var(--s-2)}
```

becomes:

```css
  max-height:calc(100dvh - var(--s-6)); overflow-y:auto; padding-right:var(--s-2)}
```

**Check:** with the cursor over the rail, scroll past the rail's internal maximum and confirm the page keeps moving. `health.mjs` shows the chaining row as PASS.

---

## A5. OS dark preference (P6, medium)

**Problem:** the head script resolves `prefers-color-scheme: dark` before the first paint, then line 931 overrides it whenever storage is empty. Measured in a clean profile with OS dark: after load the attribute is `light` and storage reads `"light"`.

**Edit.** Line 931:

```js
  applyTheme(savedTheme === "dark" ? "dark" : "light");
```

becomes:

```js
  applyTheme(document.documentElement.getAttribute("data-theme") === "dark" ? "dark" : "light");
```

**Check:** clean profile with `prefers-color-scheme: dark` keeps `dark` after load; `health.mjs` shows the dark preference row as PASS.

---

## A6. `SHOW ANSWER` in print (P8, low)

**Problem:** line 283 was written to force the old reveal, and its `!important` also matches `<summary>`, beating `details.ans > summary{display:none}` on line 281. Every printed answer carries an orphan label, twelve in total.

**Edit.** Line 283:

```css
  details.ans > *{display:block !important}
```

becomes:

```css
  details.ans > *:not(summary){display:block !important}
```

**Check:**

```
cd /tmp && ~/.cache/ms-playwright/chromium-1234/chrome-linux64/chrome --headless --disable-gpu --no-sandbox --print-to-pdf=/tmp/p.pdf --no-pdf-header-footer file://$HOME/Documents/MUJ-SEM-3/DSA/Notes/midterm-roadmap/DSA-MIDTERM-ROADMAP.html && pdftotext /tmp/p.pdf - | tr '\n' ' ' | tr -s ' ' | grep -o -i "allocator bookkeeping\|SHOW ANSWER" | sort | uniq -c
```

Expected: `1 allocator bookkeeping`, `0 SHOW ANSWER`. Normalise whitespace as shown, `pdftotext` wraps long lines and a wrapped phrase looks missing.

---

## A7. Double print (P7, low)

**Problem:** `beforeprint` snapshots the open state; if it fires twice before `afterprint`, the second snapshot records the all-open state. Measured: `beforeprint`, `beforeprint`, `afterprint` leaves 12 of 12 answers open.

**Edit.** Replace lines 1040 to 1048 with:

```js
  window.addEventListener("beforeprint", function () {
    if (window.__rmRestoreOpen) return;
    var all = document.querySelectorAll("details.ans"), restore = [];
    for (var i = 0; i < all.length; i++) { restore.push(all[i].open); all[i].open = true; }
    window.__rmRestoreOpen = restore;
  });
  window.addEventListener("afterprint", function () {
    var all = document.querySelectorAll("details.ans"), r = window.__rmRestoreOpen || [];
    for (var i = 0; i < all.length; i++) all[i].open = r[i] || false;
    window.__rmRestoreOpen = null;
  });
```

**Check:** `node ~/.cache/roadmap-qa/verify4.mjs`, the `double` field must equal `beforePrintOpen`.

---

## A8. Timeline ticks (P9, low)

**Problem:** measured at 1460px, the rule occupies 64 to 66px, the axis labels 96 to 115px, the slabs start at 117px, and each tick runs 87 to 117px, so every tick hangs 21px below the rule and runs through the label text.

**Edit.** Line 131:

```css
.slab::before{content:""; position:absolute; top:-30px; left:12px; width:2px; height:30px; background:var(--line-2)}
```

becomes:

```css
.slab::before{content:""; position:absolute; top:-51px; left:12px; width:2px; height:51px; background:var(--line-2)}
```

Optional, since the tick then passes behind the labels: give the labels a chip on lines 128 to 129, `.blocks__axis span{... background:var(--bg)}`.

**Check:** `health.mjs` shows the timeline row as PASS with a 0px gap.

---

## A9. The 144 against 192 sentence (P10, content)

**Problem:** line 785 says the probe measured 144 bytes across four nested frames; line 786 says `4 x 48 = 192`. The source gives the first frame at `0x7ffc71522ad4` and the innermost at `0x7ffc71522a44`, a difference of `0x90` = 144 = exactly three strides of 48, so 144 is an address span, not the size of four frames.

**Edit.** Replace line 786 with:

```html
            <li>Question 11 charges five frames, not four. The 144 bytes above is the address span from the outermost to the innermost of the four recursive frames, which is three 48 byte strides; charging every live frame gives 4 x 48 = 192 for the recursion, and 5 x 48 = 240 once <code>main</code> is counted.</li>
```

**Check:** `python3 -c "print(0x7ffc71522ad4-0x7ffc71522a44)"` prints 144.

---

## A10. Cleanup (P11, low)

- **Line 275**, drop the dead selector: `.slab,.pill,.meter__fill,.mapnode{...}` becomes `.slab,.pill,.meter__fill{...}`. No element in the file carries `.mapnode`.
- **Line 378**, `<span id="ready-count">2 of 5 blocks</span>` is never referenced by any script. Either wire it to the CO2 count or remove the id so it stops reading like a live hook.
- **Lines 3 to 12**, the theme boot script sits between `<html>` and `<head>`, which is invalid markup. The parser recovers by creating an implicit head (verified: `documentElement.children` is `HEAD,BODY`), so behaviour is correct, but move the whole `<script>` block to be the first child inside `<head>` (after line 13). Behaviour is identical: it still runs before the stylesheet applies.
- **Unused tokens**, declared and never read: `--dur-1`, `--rule`, `--shadow-2`, `--w-reg`, `--w-med`, `--w-semi`. Optional delete. If you touch the `:root` block at all, regenerate the export and gate it:

```
python3 ~/scripts/export-tokens.py DSA-MIDTERM-ROADMAP.html && python3 ~/scripts/export-tokens.py DSA-MIDTERM-ROADMAP.html --check
```

The check must print `clean`.

---

## A11. Chip strip wheel (P13, judgment)

**Problem:** measured at 1460px with the bar pinned, six wheel events over the bar move the page 400px against 1200px over the content, and it never settles: the handler walks the strip to its 161px maximum and the scroll-spy's `reveal()` pulls it back to 99 to keep the active chip visible, re-arming the handler. Trace: `161, 160, 99, 161, 160, 99`.

**Edit.** Delete lines 1255 to 1264, the comment and the whole listener:

```js
  /* a vertical wheel over the strip walks it sideways while it still has room,
     so the chips past the right edge are reachable without a trackpad gesture */
  strip.addEventListener("wheel", function (ev) {
    ...
  }, { passive: false });
```

The chips stay reachable by tab, by prev/next, and by their own anchors. If sideways access is wanted, a shift+wheel or a drag gesture costs the reader nothing.

**Check:** `node ~/.cache/roadmap-qa/wheel.mjs`, the two gains should be equal.

---

## A12. Share-card URLs (P4, fixed at revision cbc321cd4a017fb8)

The defect was not only "wrong once published". `og:image` was the bare string `og-card.png`, and a relative URL in a share card never resolves: every scraper (Slack, X, WhatsApp, Discord) fetches metadata with no document base, so the card was broken everywhere, including locally. `og:url` named the repo root rather than the page.

Fixed by making both absolute against the host that actually serves the file today, plus the missing Twitter mirrors:

| Tag | Was | Now |
|---|---|---|
| `og:url` | `https://github.com/pixie-chan/dsa-ece2104` | `https://github.com/pixie-chan/dsa-ece2104/blob/main/dsa/Notes/midterm-roadmap/DSA-MIDTERM-ROADMAP.html` |
| `og:image` | `og-card.png` | `https://raw.githubusercontent.com/pixie-chan/dsa-ece2104/main/dsa/Notes/midterm-roadmap/og-card.png` |
| `twitter:image` | absent | the same absolute card URL |
| `twitter:title` | absent | the `og:title` wording |
| `twitter:description` | absent | 168 characters, trimmed to fit the card limit |

All three absolute URLs were checked with `curl -sI` and return 200, the card returns `image/png`, and `og-card.png` measures exactly 1200 x 630, matching `og:image:width` and `og:image:height`.

**One line to change if the page ever gets a real host.** The durable answer is a GitHub Pages site, which would make `og:url` the rendered page instead of a source view. It is not enabled on this repo (the Pages API returns 404), so nothing here points at a URL that does not exist. If Pages is turned on at `/`, swap the two URLs to `https://pixie-chan.github.io/dsa-ece2104/...` and this section becomes history again.

---

## Acceptance check after all patches

`node ~/.cache/roadmap-qa/health.mjs` should print every row as PASS:

```
page errors, spine logger exported, navigator render handshake,
meter after 5 ticks, aria-valuenow, slab counter + fill, slab jump link,
navigator scroll spy, navigator chip + stepper, trail numbering,
spine section ranges, timeline tick meets rule, rail wheel chaining,
mobile 375 overflow, OS dark preference
```

Then, in order: `node ~/.cache/roadmap-qa/verify14.mjs` (trail `1, 2, 3, 4`), `node ~/.cache/roadmap-qa/wheel.mjs` (equal gains), `node ~/.cache/roadmap-qa/sweep.mjs` (no overflow at any of the ten widths), `node ~/.cache/roadmap-qa/verify4.mjs` (`double` equals `beforePrintOpen`), and the print one-liner in A6.

## Batch 2: the expanded page, revision c13596a0

The page grew from 1317 to 2632 lines and from 90KB to 237KB. It now carries the projected MTE scope: 19 sections, 18 nav chips, 69 tables, 10 figures, a 40 question mock paper, a second answer key, and a new Block F on the timeline. Batch 1 survived intact. Six new items, all measured on this revision.

| # | Status | One line |
|---|---|---|
| B1 | FIXED | Seven axis labels match seven slabs, verified pairwise from 1241px up |
| B2 | FIXED | Block F now reads `15 lectures, past the spine` with no fill bar, and `SLABS` carries the `f` entry |
| B3 | FIXED | The chips read `topic plan`, `inventory`, `honesty`; no placeholder labels remain |
| B4 | FIXED | The description now ends `... each with its own answer key, and the gaps that are still open.`, no clipped join |
| B5 | FIXED | 69 of 69 tables wrapped and captioned; every one of the 234 `<th>` already carried `scope="col"` |
| B6 | FIXED | All ten figures now sit in `diagrams/` beside the page and every file resolves |
| B7 | FIXED | Zero mangled struct literals remain; `sizeof(Node{int data; Node* next;})` is back |
| B8 | FIXED | Rail 1431px to 464px, uniform 347px slabs, no overflow at any of eleven widths |
| B9 | FIXED | `min-width:44px` merged into the single `.slab__link` rule; one declaration, computed value still 44px |

Status measured at revision `2131627215d8df38`. The sections below are kept as the measurement record; where one disagrees with this table, the table is current.

### Batch 2 closed out, revision 19f350175c8da8ba

Applied 2026-09-18 23:56 IST to the live page, after snapshotting it to `versions/v4-pre-leftovers/`. Three items, four literal edits, nothing else touched:

| # | Edit |
|---|---|
| B4 | line 17: `...each with its own answer key.re still open.` becomes `...each with its own answer key, and the gaps that are still open.` |
| B5 | line 700: `<h3>The four gaps that only practice closes</h3><table>` becomes `</h3><div class="tbl-wrap"><table>`, and that table's closing `</table>` becomes `</table></div>` |
| B9 | line 292: `.slab__link{min-width:44px} .slab__link{position:absolute; ...}` becomes one rule, `.slab__link{min-width:44px; position:absolute; ...}` |

**B5 needed only the wrap.** The `<th scope>` half was a phantom: the census command used to find it, `grep -o '<th' file \| wc -l`, also matches `<thead>`, and this file has 43 of those. Counted properly, all 234 `<th>` elements already carried `scope="col"` (227 plain, 7 with `class="n"`), which is why the reported "about seven headers" was exactly the `<thead>` opening tags. Corrected commands are at the end of this section.

Verified against the live page at `sha256 19f350175c8da8ba`, 230529 bytes, mtime 23:56:34:

```
node ~/.cache/roadmap-qa/leftovers.mjs   ALL LEFTOVER CHECKS PASS (8 rows, 0 page errors)
node ~/.cache/roadmap-qa/health.mjs      15 of 15 PASS
node ~/.cache/roadmap-qa/verify14.mjs    trail 1 to 10, s7Spine 16, errs []
node ~/.cache/roadmap-qa/wheel.mjs       page 2500 to 2900 over the pinned bar, strip stays at 0
node ~/.cache/roadmap-qa/verify4.mjs     beforePrintOpen 2, double 2 (equal), openAfterPdf 0
node ~/.cache/roadmap-qa/sweep.mjs       ok at all ten widths, 320px to 1920px
print                                    83 pages, all four gap rows present, SHOW ANSWER 0
python3 ~/scripts/export-tokens.py DSA-MIDTERM-ROADMAP.html --check    clean, rc 0
three inline scripts                     node --check clean
```

`leftovers.mjs` is the new probe and it reads the DOM rather than the source: 69 tables with 69 `.tbl-wrap` and 0 loose, the description as whole sentences with no `word.word` join, exactly one `.slab__link` rule in the CSSOM whose computed `min-width` is still 44px, link box 356 x 235 at 390px, and no document overflow at 390px.

**Probe pitfall, it cost one false FAIL.** Current Chromium gives every `CSSStyleRule` a `cssRules` property that exists but is empty. A CSSOM sweep guarded with `if (rule.cssRules) return [...rule.cssRules].forEach(walk)` therefore recurses into nothing and reports zero rules, which reads exactly like "the declaration was deleted". Guard on `rule.cssRules && rule.cssRules.length`.

### Corrected table checks

The `<th>` commands quoted in the B5 sections above over-count by 43. Use these instead:

```
grep -o '<table' DSA-MIDTERM-ROADMAP.html | wc -l                 # 69
grep -o 'class="tbl-wrap"' DSA-MIDTERM-ROADMAP.html | wc -l       # 69
grep -o '<th[^>]*>' DSA-MIDTERM-ROADMAP.html | sort | uniq -c     # every line carries scope="col"
```

The third command is a census rather than a count, so it shows which variant each header uses and cannot be fooled by `<thead>`.

### B4b, the meta description (applied at revision 19f350175c8da8ba)

The text quoted below is the state before the fix. The tail was clipped off with the front of "The gaps are still open", leaving `key.re still open.`, so the whole content was replaced with a clean version that keeps the original front half and closes the sentence as `... each with its own answer key, and the gaps that are still open.`

It ended:

```
... a forty question mock paper and a twenty four problem numeric set, each with its own answer key.re still open.
```

`key.re still open.` is the tail of "The gaps are still open" with the front of the phrase clipped. Replace the whole content with a clean version, for example:

```html
<meta name="description" content="Mid-term roadmap for ECE2104 Data Structures and Algorithms: the fourteen topics projected in class, where each sits in the handout plan, the sorting, tree, analysis and formulation modules measured for the topics the handout files under End Term, a forty question mock paper, a twenty four problem numeric set, and the gaps that are still open.">
```

**Check:** the description reads as whole sentences and contains no `key.re` style joins.

### B5 residue, two small passes (applied at revision 19f350175c8da8ba)

- The table titled "The four gaps that only practice closes" in the mock paper section is now wrapped like the other 68.
- **The `<th>` pass needed nothing, and the command that asked for it was wrong.** `grep -o '<th ' file | wc -l` counts `<thead>` opening tags too, and this file has 43 of them. Every one of the 234 `<th>` elements already carried `scope="col"`, so the "about seven headers" here were the `<thead>` tags themselves. Use the census command in the closure section above.

### B1. Timeline labels no longer line up with the slabs

**Measured at 1460px**, offsets inside `.blocks__rail`:

```
labels: 1 to 4@0  5 to 9@215  10@483  11 to 15@539  16@807  exam@862
slabs:  A@0       B@215       C@483   D@539         E@807   F@862    exam@1096
```

Seven slabs are placed into a six column grid, so the seventh falls into an implicit column and the row reads: Block F under the label `exam`, and the real exam slab past the end with nothing above it. The widths still scale for the teaching blocks (about 53px per lecture), and the exam slab is 300px of auto width.

**Edit.** Give Block F its own labelled column and keep the exam last.

Line 389:

```html
        <div class="blocks__axis" aria-hidden="true"><span>1 to 4</span><span>5 to 9</span><span>10</span><span>11 to 15</span><span>16</span><span>exam</span></div>
```

becomes:

```html
        <div class="blocks__axis" aria-hidden="true"><span>1 to 4</span><span>5 to 9</span><span>10</span><span>11 to 15</span><span>16</span><span>17 to 31</span><span>exam</span></div>
```

Line 126, and this one line also fixes B8, so do them together:

```css
  grid-template-columns:minmax(0,4fr) minmax(0,5fr) minmax(0,1fr) minmax(0,5fr) minmax(0,1fr) 232px; grid-template-rows:auto auto}
```

becomes:

```css
  grid-template-columns:minmax(0,4fr) minmax(0,5fr) 132px minmax(0,5fr) 132px 232px 232px; grid-template-rows:auto auto}
```

Measured effect of that exact value, applied live in the browser: the rail drops from 1431px to 479px and the slabs from 1314px to 361px, with slab widths 187, 234, 132, 234, 132, 232, 232.

Line 153, the print rule:

```css
@media print{ .blocks__rail{grid-template-columns:repeat(6, 1fr)} .slab{min-height:auto} }
```

becomes `repeat(7, 1fr)`. The `.blocks__note` copy that ends "the single recursion lecture, then the exam" should also mention the projected block, since it is now one of the slabs.

**Check:** re-measure label lefts against slab lefts; they must match pairwise, and the `exam` label must sit above the exam slab.

### B1 and B8, FIXED at revision 993b491f, kept as a record

The timeline was rebuilt in one pass. The axis now carries seven labels (`1 to 4`, `5 to 9`, `10`, `11 to 15`, `16`, `17 to 31`, `exam`) and the grid carries seven columns with minimum widths, so the labels and the slabs align pairwise. Measured at 1460px: labels at 0, 160, 340, 510, 690, 860, 1182 and slabs at exactly the same offsets.

The height problem went with it. The rail fell from 1431px to 464px and the slabs are a uniform 347px instead of 1314px. Verified across eleven widths from 320px to 1920px: no overflow inside the rail and no document overflow at any of them, with the layout dropping to two columns at 1240px and one at 620px, and the exam slab spanning the full row in the stacked layouts. The print rule now reads `repeat(7, 1fr)`.

### B2. Block F's counter can never move, and its numbers disagree

The slab markup at line 435 carries a state row:

```html
          <span class="slab__state" aria-hidden="true"><span class="slab__tag slab__tag--part">in scope by the list</span><span class="slab__count" id="rm-count-f">0 of 9 ticked</span><span class="slab__bar"><span class="slab__fill" id="rm-fill-f"></span></span></span>
```

but the fill script's `SLABS` table (line 2597 onward) has entries for `a`, `b`, `c`, `d`, `e` and `exam` only, so nothing ever paints `rm-count-f` or `rm-fill-f`. Measured after clicking tick-all: `rm-count-a` reads `4 of 4 ticked`, `rm-count-f` still reads `0 of 9 ticked` and `rm-fill-f` has no width at all.

The two numbers also disagree by construction: the visible count says 9 topics, while patch A writes the accessible name from `data-lectures="17-31"`, which is 15 lectures, so the link reads `Block F, projected MTE scope, lectures 17 to 31, 0 of 15 ticked off`. And since the spine only has ticks 1 to 16, that count can never change.

**Edit, pick one:**

1. Simplest and honest: delete the state row on line 435 for Block F. Its progress is not a spine question.
2. If Block F should report something, report the state of the `mte-scope` material rather than spine ticks, and drop `data-lectures="17-31"` from the slab so the accessible name stops promising a count.
3. If you keep it as ticks, add `{ id: "f", lo: 17, hi: 31 },` to `SLABS` and add ticks 17 to 31 to the spine, which is a much larger change.

**Check:** after tick-all, every visible count on the timeline must agree with the meter or be explicitly a material state, and no `rm-count-*` element may be left unpainted.

### B3. Placeholder nav labels

Lines 506, 515 and 516 read `s5`, `s8`, `s9` while the other fifteen chips carry prose. Match the section headings, as the earlier labels did: `the plan`, `inventory`, `honesty`.

**Check:** no chip label matches `/^s\d+$/`.

### B4. Meta description fragment

Line 17 ends `... and a timed mock paper. Roughly sequence, twelve numerical problems with a separate answer key, and the gaps that are still open.` The words "Roughly sequence" are the remains of the old "a seven session study sequence" phrase. Replace the whole content with one clean sentence, for example:

```html
<meta name="description" content="Mid-term roadmap for ECE2104 Data Structures and Algorithms: the fourteen topics projected in class, where each sits in the handout, the modules measured for the ones the handout files under End Term, a timed mock paper, twelve numerical problems with a separate key, and the gaps still open.">
```

**Check:** no sentence fragment, and the description still names what the page contains.

### B5. Table hygiene in the new sections

- **Line 1835**, the table "The four gaps that only practice closes" in the mock paper section is the only one of 69 not wrapped in `.tbl-wrap`, so it can push the layout sideways on a narrow screen. Wrap it like its neighbours.
- **133 of 277 `<th>` elements have no `scope="col"`.** Superseded and false: see the B5 residue item above. All 234 real `<th>` elements carry it; the 277 came from counting `<thead>` as well. A mechanical pass over the new sections would have added nothing.
- **58 of 69 tables have no `<caption>`.** The originals all had one. Captions are also what a screen reader announces as the table name.

**Check:** `grep -o '<table' file | wc -l` equals `grep -o 'class="tbl-wrap"' file | wc -l` (69 each), and `grep -o '<th[^>]*>' file | sort | uniq -c` shows only variants containing `scope="col"`. The older form of this check, `grep -o '<th' file | wc -l`, over-counts by the number of `<thead>` tags and reports a phantom defect.

### B6. Nine figures now live outside this folder

The page references `../mte-scope/diagrams/01-tree-terminology.png` through `09-sorting-family.png`, nine files that live in `Notes/mte-scope/diagrams/`. They load correctly today (verified in the browser: all ten images report `naturalWidth` around 1400, no failed requests). The fragility is that the dossier is no longer self-contained: move the HTML, or share the `midterm-roadmap` folder on its own, and nine figures break silently.

**Edit, pick one:**

1. Copy the nine PNGs into `midterm-roadmap/diagrams/` and change the nine `src="../mte-scope/diagrams/` prefixes to `src="diagrams/`. The page then travels as one folder again.
2. Keep the shared folder, and say so in the footer next to the rebuild line, naming `Notes/mte-scope/render-diagrams.sh` as the source of those nine figures.

**Check:** every `src` in the file resolves relative to the HTML's own folder, or the footer names the dependency.

### B7. Four struct literals lost their field names (content, high)

The trees and formulation sections show C++ that cannot compile, while the same file elsewhere shows the correct form. All four, verbatim:

```
sizeof(Node{int; Node*})                            twice
sizeof(TreeNode{int; TreeNode*; TreeNode*})          twice, one of them followed by " = 24"
```

The identifiers `data`, `next`, `left` and `right` were dropped somewhere between writing and building the page. Compare the correct fragments that survive in the same document: `<code>{int data; Node* next;}</code>` and `<code>sizeof(Node{int data; Node* next;})</code>`.

This matters more than a typo, because the whole premise of these sections is that the sizes were compiled and measured. `lec05_run_output.txt` records `Node{int data; Node* next;} sizeof=16`, and 24 is right for `TreeNode{int data; TreeNode* left; TreeNode* right;}`, so the numbers are correct and only the declarations are broken.

**Edit.** Two global replacements:

```html
<code>sizeof(Node{int; Node*})</code>
```
becomes
```html
<code>sizeof(Node{int data; Node* next;})</code>
```

and

```html
sizeof(TreeNode{int; TreeNode*; TreeNode*})
```
becomes
```html
sizeof(TreeNode{int data; TreeNode* left; TreeNode* right;})
```

**Check:** `grep -c 'int; ' file` returns 0, and every struct literal in the page compiles as written.

### B8, FIXED at revision 993b491f, kept as a record

The single-lecture columns were given minimum widths instead of a bare `1fr`, and the padding and line height were trimmed, so the two slabs that used to wrap to 1231px and 1314px now sit at 347px like the rest. Measured: rail 1431px to 464px, slabs 1314px to 347px, and the intrinsic heights are uniform enough that no slab carries dead space. `node ~/.cache/roadmap-qa/tall3.mjs` re-measures it.

### B9. A duplicated `.slab__link` declaration (applied at revision 19f350175c8da8ba)

The 23:07 save added `.slab__link{min-width:44px}` on the same line as the existing rule, so the file had two `.slab__link` declarations where one would do. They are now one rule, `min-width:44px` first so the declaration order is unchanged, and the CSSOM reports exactly one `.slab__link` entry whose computed `min-width` is still 44px. The text below is the state before the merge.

```css
.slab__link{min-width:44px} .slab__link{position:absolute; inset:0; z-index:1; display:block; color:inherit; text-decoration:none}
```

It is also redundant: the link is `inset:0` inside a slab that is at least 53px wide, so it already meets the 44px target. Harmless, but merge it into the second rule rather than leaving a duplicate.

### Note, not a defect

Printing is now 81 pages, up from 18, because the mock paper and the numeric set are part of the document. Everything still prints correctly: answer bodies present, zero `SHOW ANSWER` labels. If a shorter print is wanted, `@media print{ #mock, #newprob { display:none } }` would drop the two largest new sections from paper while keeping them on screen.

## Do not revert

Verified working at this revision, so leave alone: the print reveal (`::details-content` plus the `beforeprint` handler), the rail clamp `max-height:calc(100dvh - var(--s-6))` with `overflow-y:auto`, `color-scheme` in both `:root` blocks, the `theme-color` update inside `applyTheme()`, the head boot script's placement before first paint, the tick `aria-label` read from the tick's own span with `.tick__n` hidden, `aria-valuenow`, the trail cap of 40, `tick-reset` clearing the trail, the slab jump links and their dynamic accessible names, the sticky section navigator markup, `tabindex="-1"` on `#s1`, `scope="col"` on every table header, and the generated `tokens.css` with its `--check` gate.
