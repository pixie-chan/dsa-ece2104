# Code review: DSA-MIDTERM-ROADMAP.html

Handoff document. Written to be pasted straight into another agent, no context needed.

## Target

- **File under review:** `/home/zen/Documents/MUJ-SEM-3/DSA/Notes/midterm-roadmap/DSA-MIDTERM-ROADMAP.html`
- **Revision reviewed:** 907 lines, 66786 bytes, `sha256 b7ecab3cef4e0daba1c6684e2642a079cf1b1418232c491346605762aaa51bc3`, mtime 2026-09-18 16:24:47 IST
- **Line numbers below refer to that revision.** If the file has changed, re-check the line refs and re-run the repro commands before trusting a finding.
- **What the file is:** a single-file static dossier (inline CSS, inline JS, no build step) for the ECE2104 DSA mid-term: a hero, a "block timeline" signature graphic, a sticky lecture-spine nav with a readiness meter and localStorage persistence, ten content sections, twelve reveal-on-click numerical answers, and a duplicate answer key.

## How the findings were produced

- Read the full file, then ran it headlessly in Playwright Chromium (build `chromium-1234`) at 1460x900 and 375x812.
- Printed it with `--print-to-pdf` and tested text presence in the PDF with `pdftotext` plus control phrases.
- Measured live geometry with `getBoundingClientRect()` at three scroll positions.
- Cross-checked every numeric claim in the page against `~/learning/cpp/exercises/lec05_run_output.txt` and `~/learning/cpp/exercises/lec16_recursion_output.txt`.
- Probe scripts and screenshots live in `/home/zen/.cache/roadmap-qa/` (`measure.mjs`, `rail.mjs`, `print.pdf`, `top.png`, `baseline.html`).

Repro commands, each one line:

```
node /home/zen/.cache/roadmap-qa/measure.mjs
node /home/zen/.cache/roadmap-qa/rail.mjs
```

## 1. Real bugs, verified with output

### 1.1 Print drops all twelve answers

`--print-to-pdf` produces 18 pages in which `SHOW ANSWER` appears 12 times and the answer bodies appear 0 times. Control test: `pdftotext print.pdf - | grep -ci "SHOW ANSWER"` gives 12, while phrases that exist only inside the `<details>` bodies give 0 (`allocator bookkeeping`, `four walks from the head`, `deepest nesting three frames`). Phrases in always-visible text give non-zero (`Twelve problems` 2, `answer key` 1).

Cause: the rules at lines 262 to 263 (`details.ans{display:block}` and `details.ans > *{display:block !important}`) do not defeat Chromium's hiding of a closed `<details>`, which now lives on the `::details-content` pseudo-element.

Fix options, cheapest first:

1. Let the duplicate key section carry print and drop the inline stubs: `@media print { details.ans { display: none } }`. The key section at lines 751 to 773 already prints all twelve answers.
2. Add `@media print { details.ans::details-content { content-visibility: visible } }` (Chromium 131+) and a `window.addEventListener("beforeprint", ...)` that sets `open = true` on every `details.ans`, restoring state on `afterprint`. This one also works in Firefox and Safari.

### 1.2 The sticky rail's controls are unreachable

Measured: `.rail` is 1139px tall inside a 900px viewport (`.rail__ticks` alone is 704px). Mid-scroll the rail pins at `top: 16`, and the controls sit at `#tick-all` y=986, `#theme-btn` y=1038, `#trail` y=1142. They only enter the viewport on the final screen of a 13043px document, so "tick all done", "reset" and the theme switch are effectively dead during reading. Even the meter is only just visible (bar at y=824 to 838).

Fix: `max-height: calc(100dvh - 32px); overflow-y: auto; overscroll-behavior: contain` on `.rail` (line 138), or scroll only `.rail__ticks` while keeping `.meter` and `.rail__btns` pinned.

### 1.3 The signature timeline's ticks do not touch its rule

Measured inside `.blocks__rail`: the `::before` rule occupies 38 to 40px, the axis labels 64 to 83px, slabs start at 85px, and each `.slab::before` connector runs 67 to 85px. So every hanging tick stops 27px short of the rule and instead runs alongside the axis label text, breaking the intended "marks hanging off a ruled line" reading. Relevant lines: 107 to 133.

Fix: move the rule to just above the slabs (around `top: 82px`) and let the connector span the full drop, or lift the labels above the rule on a background chip.

Related note: the caption at lines 349 to 351 claims the widths are lecture counts drawn to scale. That holds for the five teaching slabs (measured 289/361/72/361/72 for 4/5/1/5/1 lectures, a constant 72.2px per lecture), but the exam slab is a fixed 232px, and the print rule at line 135 then overrides everything to `repeat(6, 1fr)`.

### 1.4 Theme meta and `color-scheme` never follow the toggle

After clicking the theme button: `meta[name="theme-color"]` still reports `#fafaf9`, and computed `color-scheme` is `normal` in both themes (body background correctly becomes `rgb(20, 18, 15)`). Result: light scrollbar and light native controls on the dark sheet.

Fix: set `color-scheme: light` in the light `:root` and `color-scheme: dark` in `[data-theme="dark"]`, and update the meta content inside `applyTheme()` (line 805).

### 1.5 Theme flash on load

Line 2 hardcodes `data-theme="light"`, and the stored theme is only applied at the end of the body (lines 812 to 814), so a dark-sheet reader gets a light first paint. Fix: a three-line inline script in `<head>`, immediately after the opening `<html>` tag, that reads localStorage and sets `data-theme` before the first paint.

## 2. Logic and polish

- **Dead code.** The `Enter` key logger at lines 898 to 903 can never fire: the page contains 0 `input`, `select` and `textarea` elements (measured). `#ready-count` (line 303) is never referenced by the script. Eight custom properties are declared and never used: `--dur-1`, `--hair`, `--rule`, `--s-1`, `--shadow-2`, `--w-reg`, `--w-med`, `--w-semi`.
- **Session trail.** `tick-reset` (line 854) clears the tick state but not the trail, and the trail grows unbounded. Clear it on reset, or cap it at about 8 entries.
- **Accessible names.** `render()` sets `aria-label` to "lecture 5, ticked off", which replaces the visible text, so a screen reader never hears the topic ("DS and memory"). Include the topic in the label or drop the custom label and rely on `aria-pressed` alone.
- **Meter semantics.** `.meter__bar` is `role="img"` with a frozen `aria-label`. Prefer `role="progressbar"` with `aria-valuemin="0"`, `aria-valuemax="16"` and `aria-valuenow` updated in `render()`.
- **Double announcements.** `#meter-text` (line 378) and `#trail` (line 390) are both `aria-live="polite"` and both update on every click. Keep one.
- **Skip link.** It targets `#s1`, which is not focusable. Add `tabindex="-1"` to the target section.
- **`RANGES` (line 860)** duplicates section knowledge that already exists in the markup. `data-lectures="5 9"` on each section would remove a place to forget. The clearing behaviour is correct as written: a section with no range does reset every tick to `data-current="false"`.
- **Micro.** Use `Object.create(null)` for the `done` map parsed from localStorage; cache the four `$()` lookups inside `render()`.

## 3. Drift and maintenance

- **Token drift.** `tokens.css` and the inline `:root` have diverged: `--text-2xl` is 2.25rem vs 2.5rem, `--text-3xl` is `clamp(2.5rem, 5vw, 3.5rem)` vs `clamp(2.75rem, 4.4vw, 4rem)`, `--shadow-2` is `10px 24px -18px rgba(...,.4)` vs `12px 28px -20px rgba(...,.45)`. Pick one source of truth, ideally generate the inline block from the token file.
- **Diagram rebuild.** `render-diagrams.sh` writes only `.png` (`-o "${f%.mmd}.png"`), but the page displays `diagrams/03-concept-map.svg` (line 497). The rebuild instruction in the footer (line 779) therefore does not rebuild what the page shows. Add an SVG pass or reference the PNG.
- **Fonts.** The page fetches three Google font families (lines 14 to 16) while the comment at line 17 claims "no fetches", and the dossier is meant to open offline. Offline, Instrument Serif collapses to Georgia. Self-host the woff2 files or correct the comment.
- **Social meta.** `og:image` is the relative `og-card.svg`; crawlers need an absolute URL and most platforms do not render SVG at all. Export the 1200x630 card to PNG and add `og:url`, `og:image:width`, `og:image:height`, `twitter:card`.
- **Table and print detail.** `<th>` elements lack `scope`; slab and pill fills will print white without `print-color-adjust: exact`.

## 4. Content issues, which matter more than the CSS here

- **One call, two byte counts.** Section 7 (line 675) says "144 bytes across four nested frames", while question 11 (line 637) says five frames, "about 240 bytes", for the same `factorial(4)` call. The cited source says exactly: `span between first and innermost frame = 144 bytes, about 48 bytes per frame` (lec16 output, lines 8 and 9). So 144 is three 48-byte strides across four live frames, and 240 is five frames charged including `main`. Both are defensible, but on one page a student reads them as a contradiction. One reconciling sentence fixes it.
- **Markdown twin drift.** `DSA-MIDTERM-ROADMAP.md:93` says "solve the ten numerical problems in section 6" while section 6 lists twelve and the key has twelve rows. The HTML footer (line 780) calls the MD "the same content".
- **Metric wording.** "Material ready: 2 of 5 blocks" with the sub-caption "memory and lists shipped and verified" silently skips block A (C++), which section 8 counts as complete.

## 5. What was checked and is already correct

- No console errors, no page errors, no failed requests on load.
- No body-level horizontal overflow at 375x812 (`document.scrollWidth` is 375); the lecture tick strip scrolls inside its own box.
- Meter math: three ticks gives `width: 19%`, `3 of 16 lectures ticked`, CO1 `1 / 10`, CO2 `2 / 6`. Correct.
- localStorage round-trip works under `file://` (`{"5":1,"11":1,"12":1}`).
- Tick click toggles `aria-pressed` and writes the trail correctly.
- Reduced-motion block present, focus-visible rings on every control, touch targets at 44px minimum.
- Every measured constant in the twelve numericals matches `lec05_run_output.txt`: node size 16 with offsets 0 and 8, allocator stride 32 bytes, base `0x7ffe7c3131b0`, and `0x101C` for the address arithmetic.

## 6. Suggested order of work

1. Print bug (1.1)
2. Sticky rail reachability (1.2)
3. Timeline rule and connectors (1.3)
4. Theme flash (1.5)
5. `color-scheme` and theme-color meta (1.4)
6. The 144 versus 240 reconciliation (4)
7. Everything in sections 2 and 3

---

# Round 2 verification, revision ce93d4f0

Second pass, run at 2026-09-18 16:36 IST against `sha256 ce93d4f01cd69a88` (68173 bytes, mtime 16:35:44). This supersedes the line numbers in the sections above where the file has changed.

## Fixed and verified

- **Print (1.1) now works.** The PDF is 16 pages (was 18) and every inline answer prints: `allocator bookkeeping`, `four walks from the head` and `deepest nesting three frames` each appear once. Both mechanisms work: the CSS `details.ans::details-content{content-visibility:visible}` rule, and the new `beforeprint` handler. Handler tested directly: with 2 answers open, `beforeprint` opens 12, `afterprint` restores 2. Heads up: `page.pdf()` in Chromium does fire `beforeprint`, so both paths are live at once.
- **Rail reachability (1.2) fixed.** `.rail` height dropped from 1139px to 868px (`max-height: calc(100dvh - 32px)`), the rail scrolls internally (271px of internal scroll), and the buttons become visible once it does.
- **`color-scheme` (1.4) fixed.** Computed `color-scheme` is `light` in the light sheet and `dark` in the dark sheet, and `meta[theme-color]` now tracks the toggle (`rgb(20, 18, 15)` in dark).
- **Theme boot script runs before the first paint**, so the light-flash issue (1.5) is closed in practice.

## New issues introduced by those fixes, all verified

- **Wheel trap on the rail.** With `overscroll-behavior: contain` on `.rail`, once the rail's internal scroll is exhausted the page stops scrolling while the cursor is over the rail column. Wheel trace: page 400, 800, 1200 (cursor still over the hero), then rail 0 to 270, then two further wheel events move nothing at all. Moving the cursor over the main column resumes page scrolling (1600, 2000, 2400). That is 322px of viewport width able to freeze the page. Fix: drop `overscroll-behavior:contain`, or set `overscroll-behavior-y: auto`.
- **The `prefers-color-scheme` branch in the new head script is dead.** Clean profile with `prefers-color-scheme: dark` and empty storage: after load the attribute is `light` and storage reads `"light"`. Line 831 (`applyTheme(savedTheme === "dark" ? "dark" : "light")`) still forces light when storage is empty, so a dark-OS reader gets a dark first paint, then a flip to light, and "light" is persisted. Fix: `applyTheme(document.documentElement.getAttribute("data-theme") === "dark" ? "dark" : "light")`.
- **Double print leaves every answer open.** `beforeprint` twice before `afterprint` overwrites the restore snapshot with the all-open state, so 12 answers stay open afterwards. Fix: only snapshot when `window.__rmRestoreOpen` is unset, and clear it in `afterprint`.
- **`SHOW ANSWER` still prints 12 times.** The legacy `details.ans > *{display:block !important}` beats the new `details.ans > summary{display:none}`. Cosmetic, one-line fix: `details.ans > *:not(summary)`.
- **The head script sits between `<html>` and `<head>`**, which is invalid markup. The parser recovers by creating an implicit head (`documentElement.children` is `HEAD,BODY`, and the script lands in the head), so it works, but move it to be the first child inside `<head>` for valid markup.

## Timeline (1.3) is only partly fixed

Measured at 1460px: rule occupies 64 to 66px, axis labels 96 to 115px, slabs start at 117px, and each `.slab::before` tick runs 87 to 117px. So the ticks now stop 21px short of the rule (was 27px) and still run through the axis label row. To actually connect them: `top: -51px; height: 51px` on `.slab::before`. The labels then sit on top of the ticks, so either give `.blocks__axis span` a `background: var(--bg)` chip or move the ticks to the slab's right edge.

## Still open from round 1, re-checked at this revision

- 8 unused custom properties (`--dur-1 --hair --rule --s-1 --shadow-2 --w-reg --w-med --w-semi`).
- Dead `Enter` key handler (the page has no input elements); `#ready-count` still never touched by the script.
- `RANGES` still hardcoded; 0 `data-lectures` attributes.
- Tick `aria-label` still drops the topic name.
- `tick-reset` still does not clear the trail; the trail is still uncapped.
- `og:image` still the relative `og-card.svg`.
- Section 7 still says "144 bytes across four nested frames" with no reconciliation against question 11's 240 bytes.
- `DSA-MIDTERM-ROADMAP.md:93` still says "the ten numerical problems".
- Token drift unchanged (`--text-2xl` 2.5rem in the page against 2.25rem in `tokens.css`).

---

# Round 3 verification, revision 1d8f05d1 (BLOCKER)

Checked at 2026-09-18 16:38 IST against `sha256 1d8f05d194c3a529` (68856 bytes, mtime 16:36:48).

## Blocker: the lecture spine is dead on load

Line 850 in `render()` now reads `(LABELS[lec] || "")`, but no `LABELS` object exists anywhere in the file (the only occurrence of the string is that line). Every call to `render()` throws `ReferenceError: LABELS is not defined`.

Measured on load: 1 page error, `#meter-fill` style width empty, `#meter-text` still "nothing ticked yet", all sixteen ticks still `data-state="todo"`, `#tick-5` `aria-label` is `null`. Clicking lecture 5 logs "lecture 5 ticked off" to the trail and writes localStorage, but the tick never turns `done` and the meter never moves. Two page errors total.

Fix, preferred: do not add a second copy of the sixteen topics. Read the topic from the tick's own label span, which is already in the DOM:

```js
var topic = ticks[i].querySelector("span:nth-child(2)").textContent.trim();
ticks[i].setAttribute("aria-label", "lecture " + lec + ", " + topic + (isDone ? ", ticked off" : ", not ticked off yet"));
```

With `LABELS` stubbed to an empty map, the rest of the spine is correct: 2 ticks gives `13%` and `2 of 16 lectures ticked`, tick-all gives `100%`, `10 / 10` and `6 / 6`, and zero page errors.

## Second defect in the same block

`aria-valuenow` is never written by the script. With the stub in place it stayed `"0"` at 2 of 16 and still `"0"` at 16 of 16, so a screen reader reports an empty progress bar forever. One line in `render()`:

```js
if ($("meter-bar")) $("meter-bar").setAttribute("aria-valuenow", n);
```

## The new reconciliation sentence is arithmetically wrong

The added bullet says "4 x 48 = 192 bytes of recursion", which contradicts the measured 144 in the bullet directly above it. From `lec16_recursion_output.txt`: first frame `0x7ffc71522ad4`, innermost `0x7ffc71522a44`. That difference is `0x90` = 144 = exactly 3 strides of 48. So 144 is the address span from the outermost to the innermost of the four recursive frames (four frames, three gaps), not the size of four frames. Correct wording: the span is 144 bytes of addresses; charging each live frame gives 4 x 48 = 192 for the recursion, and 5 x 48 = 240 once `main` is included. As written, the page now shows three mutually inconsistent numbers.

## Verified good in this revision

- `print-color-adjust: exact` added for the printed slabs, pills and meter fill.
- "Material ready" caption now names blocks A and B correctly.
- `.meter__bar` converted to `role="progressbar"` with min, max and a proper label.
- The trail's `aria-live` removed, leaving the meter text as the single live region.
- `tabindex="-1"` on `#s1`, so the skip link moves focus.
- `scope="col"` on every table header.
- Dead selector: `.mapnode` in the new print rule matches no element in this file (no element carries that class); it is a leftover from the lecture 5 concept-map page.

## Still open

Unused tokens, dead `Enter` handler, `#ready-count`, hardcoded `RANGES`, `tick-reset` not clearing the trail, relative `og:image`, the MD's "the ten numerical problems", and the `--text-2xl` drift.

---

# Round 4 verification, revision 5b9bee69 (round 3 blocker resolved)

Checked at 2026-09-18 16:39 IST against `sha256 5b9bee69539c8dfe` (68961 bytes, mtime 16:38:00).

## Fixed

- **The LABELS blocker is gone.** No page errors on load or on click. Verified live: 2 ticks gives `13%` and `2 of 16 lectures ticked`, tick-all gives `100%`, `10 / 10`, `6 / 6`. The topic is now read from the tick's own span, so no duplicated map can go stale.
- Accessible names carry the topic: `lecture 5, DS and memory, not ticked off yet`, and `.tick__n` is `aria-hidden="true"` so the number is not read twice.
- Session trail is capped at 40 entries (verified: 45 further clicks leave exactly 40 children, oldest trimmed).
- The dead `Enter` key handler is removed.

## Still open, in the order worth doing

1. `aria-valuenow` never moves. Measured `"0"` at 2 of 16 and still `"0"` at 16 of 16. Needs `if ($("meter-bar")) $("meter-bar").setAttribute("aria-valuenow", n);` in `render()`.
2. The "4 x 48 = 192" sentence still contradicts the measured 144 in the bullet above it, see round 3 for the correct phrasing.
3. Timeline ticks still stop 21px short of the rule and run through the axis labels (`padding-top: 96px`, rule `top: 64px`, tick `top: -30px; height: 30px`). Use `top: -51px; height: 51px`.
4. Wheel trap: `overscroll-behavior: contain` on `.rail` still freezes page scrolling once the rail's internal scroll is exhausted.
5. `prefers-color-scheme` branch in the head script is still inert (line 834 forces and persists light when storage is empty).
6. Printing twice in a row still leaves all twelve answers open (2 references to `__rmRestoreOpen`, no guard).
7. `SHOW ANSWER` still prints 12 times (`details.ans > *{display:block !important}` beats the new summary rule).
8. `.mapnode` in the print rule matches no element (leftover from the lecture 5 page).
9. Unused tokens, `#ready-count`, hardcoded `RANGES`, `tick-reset` not clearing the trail, relative `og:image`, the MD's "the ten numerical problems", and the `--text-2xl` drift.

---

# Round 5 verification, revision 20fd1281

Checked at 2026-09-18 16:40 IST against `sha256 20fd1281c10c1bdc` (69965 bytes, mtime 16:38:40).

## Fixed and verified

- Social meta added: `og:url`, `og:image`, `og:image:width`, `og:image:height`, `twitter:card`. `og:url` resolves: `https://github.com/pixie-chan/dsa-ece2104` returns HTTP 200.
- The `NOSTATE` comment (line 17) now admits the three Google font fetches and the offline fallback, so the comment matches the code.
- Section ranges moved into the markup via `data-lectures`, and `RANGES` is built from the DOM. Verified highlight bands: `s2` = 1 to 16, `s3` = 5 to 9, `s5` = 11 to 16, `s6` = 5 to 9, and sections without the attribute clear the highlight.
- `tick-reset` now clears the trail and restarts the counter. Verified: two ticks then reset leaves one line (the reset message itself, numbered 1) and the meter back to "nothing ticked yet".

## New defect: section 7 is no longer highlighted

The parser requires exactly two values:

```js
var parts = (marked[m].getAttribute("data-lectures") || "").split(/\s+/);
if (parts.length === 2) RANGES[marked[m].id] = [parseInt(parts[0], 10), parseInt(parts[1], 10)];
```

`<section id="s7" data-lectures="16">` has one value, so it is skipped. Measured while scrolling: `s7` produces `none` current ticks, where the old hardcoded map gave `[16]`. Fix:

```js
var lo = parseInt(parts[0], 10);
var hi = parts.length > 1 ? parseInt(parts[1], 10) : lo;
if (!isNaN(lo)) RANGES[marked[m].id] = [lo, isNaN(hi) ? lo : hi];
```

## Missing asset: og-card.png does not exist

`<meta property="og:image" content="og-card.png">` points at a file that is not in the folder (only `og-card.svg` is). Two notes:

- Working export, verified on this machine: `inkscape og-card.svg -w 1200 -h 630 -o og-card.png` produces a 1200x630 PNG (108 KB). The card uses Georgia and generic `monospace`, so the render is stable without the Instrument fonts installed.
- For a real crawler the image must also be an absolute URL on the host where the page is published, and `og:url` (the repo root) is not the page URL if the HTML ends up in a subfolder such as `Notes/midterm-roadmap/`.

Optional content thought: the most prominent line on that card after the title is `GAP: stacks, queues and the circular wrap have no material yet`. That is honest, and it is also what a social preview will show.

## Still open

`aria-valuenow` never updated, the "4 x 48 = 192" sentence, the 21px timeline gap, the rail wheel trap, the inert `prefers-color-scheme` branch, unguarded double print, `SHOW ANSWER` printing, the dead `.mapnode` selector, unused tokens, `#ready-count`, the MD's "the ten numerical problems", and the `--text-2xl` drift.

---

# Round 6 verification, revision 11b6deca

Checked at 2026-09-18 16:42 IST against `sha256 11b6deca2fefa1f8` (mtime 16:39:18).

## Fixed

`aria-valuenow` is now written by `render()`. Verified live across a full cycle: load `0`, two ticks `2` with 13 percent, tick-all `16` with 100 percent, reset back to `0`, and zero page errors.

## Note on the watch process

One save (16:39:18) briefly went unreviewed because the watcher auto-baselined whatever hash was live when it was armed. The watcher now baselines from `~/.cache/roadmap-qa/reviewed.sha`, which is updated only after a revision has been verified, so a save landing between a review and a re-arm can no longer be swallowed.

## Still open

Section 7's single-value `data-lectures="16"` is still skipped by the parser, `og-card.png` is still missing, the `aria-valuenow` item is done, and the rest of the round 4 and 5 lists stand: the "4 x 48 = 192" sentence, the 21px timeline gap, the rail wheel trap, the inert `prefers-color-scheme` branch, unguarded double print, `SHOW ANSWER` printing, dead `.mapnode`, unused tokens, `#ready-count`, the MD's "ten", and the `--text-2xl` drift.

---

# Round 7 verification, revision 5856374a (BLOCKER: patch A aborts the script)

Checked at 2026-09-18 16:46 IST against `sha256 5856374a34243e4a` (85686 bytes, mtime 16:44:38). This revision adds the slab jump links and the sticky section navigator.

## Blocker: one out-of-scope line kills the whole section navigator

Patch A ends with:

```js
window.__rmHooks.push(function () { syncAll(); });
/* two things the section navigator appended below needs from this closure */
window.__rmRender = render;
```

`render` is declared with `function render()` inside the FIRST IIFE of the same `<script>` block, so it is not visible here. That line throws `ReferenceError: render is not defined`, and because all three IIFEs live in one script element, the throw aborts the remainder of the block: **the entire section navigator IIFE never runs.**

Proof, measured in the browser:

- One page error: `render is not defined`.
- `window.__rmRender` is `undefined`, and `window.__rmHooks.length` is 1 (patch A's hook only). The navigator's own hook was never pushed, which is only possible if its IIFE never executed.
- `#jump-now` stays `01 / 11` after scrolling to y=6000, and the active chip stays `#s1`.
- Chip clicks do not write to the session trail; `prev` and `next` do nothing at all.
- Everything defined before the throw still works: the slab jump links (native anchor plus flash, `aria-current`, dynamic `.vh` text, one trail line), the lecture spine, the meter, and `aria-valuenow`.

Fix: move the export into the scope that owns the function. Inside the first IIFE, right after its `render();` call, add `window.__rmRender = render;`, then delete the out-of-scope line in patch A. Simpler still: the navigator only needs the page's logger, and `window.__rmLog` is already exported from inside that IIFE, so the navigator can call `window.__rmLog(...)` directly and drop the `__rmRender` handshake entirely.

## Second defect in the same block: every hook runs twice

`render()` contains the hook loop twice:

```js
for (var hk = 0; hk < (window.__rmHooks || []).length; hk++) { try { window.__rmHooks[hk](log); } catch (e) {} }
if (window.__rmHooks) { for (var h = 0; h < window.__rmHooks.length; h++) { try { window.__rmHooks[h](log); } catch (e) {} } }
```

Confirmed by counting both loop variables in the served DOM: 2. Any hook that is not idempotent, and any future hook that logs, will fire twice per render. Delete one loop.

## Verified working in this revision

- Slab jump links: clicking Block B jumped to `#s3` (scrollY 3367), applied `is-jump`, marked the slab `data-current="true"`, and logged `slab jump: Block B, Memory and lists to #s3 (0 of 5 lectures ticked)`.
- The dynamic accessible name reads `Block B, Memory and lists, lectures 5 to 9, 0 of 5 ticked off: jump to section 03, the chain`.
- Sticky bar is 57px tall at 1460, 900 and 375px widths, and after an anchor jump the section head sits at y=65 against a bar bottom of 57, so the heading is not covered. That clearance is only 8px and exists because sections carry 48px of top padding, worth knowing before the bar grows.
- Mobile: no body-level horizontal overflow at 375 (`scrollWidth` 375); the strip scrolls internally (769px of chip overflow).
- Print: the nav and the slab links are hidden (0 occurrences of the bar text in the PDF), and the answers still print (`allocator bookkeeping` present, `SHOW ANSWER` still 12 times).

## Still open

The section 7 spine range bug is unchanged (`data-lectures="16"` still skipped, measured `s7` = none while `s3` = 5 to 9), plus the earlier list: `og-card.png` missing, the "4 x 48 = 192" sentence, the 21px timeline gap, the rail wheel trap, the inert `prefers-color-scheme` branch, unguarded double print, `SHOW ANSWER` printing, dead `.mapnode`, unused tokens, `#ready-count`, the MD's "ten", and the `--text-2xl` drift.

Two notes for when the navigator is alive again:

- Its wheel handler on the chip strip will hijack vertical wheel while the strip still has room. Measured overflow: 161px at 1460, 769px at 375, so it will be active almost always. That is the same trap class as the rail's `overscroll-behavior: contain`, by design this time; consider dropping it since the chips are reachable by tab and by the prev/next buttons.
- `data-lectures` now has two formats in one file: `"1-4"` on the slabs and `"5 9"` on the sections. Pick one.

---

# Round 8 verification, revision 4fdfb930

Checked at 2026-09-18 16:50 IST against `sha256 4fdfb9301b98832a` (85555 bytes, 1251 lines, mtime 16:49:19).

## Fixed

The duplicate hook loop is gone from `render()` (P2 on the patch sheet). Only one hook loop remains, so `syncAll()` runs once per render.

## Still broken, and it is the headline item

P1 on the patch sheet, `window.__rmRender = render;`, is untouched, so the `ReferenceError: render is not defined` throw still aborts the script block and the whole section navigator still never runs. Re-measured at this revision: 1 page error, `window.__rmHooks.length` is 1, `window.__rmRender` is `undefined`, `#jump-now` stays `01 / 11` after a 6000px scroll, the active chip stays `#s1`, and `prev` and `next` do nothing.

## Patch sheet

A standalone, agent-ready fix list now exists beside this log: `DSA-MIDTERM-ROADMAP-PATCHES.md`. It pins a revision, gives every open item an ID, its literal location, the measured evidence, the exact edit and a verification step, and it lists what is already fixed so an agent does not revert it. This log stays as the chronological evidence trail.

---

# Round 9 verification, revision c7c60d9c (BLOCKER: the main script does not parse)

Checked at 2026-09-18 16:52 IST against `sha256 c7c60d9ca1292d51` (90308 bytes, 1321 lines, mtime 16:50:50). This revision adds a state row to each timeline slab and a third `<script>` that paints it.

## The page is inert

The new hook loop was inserted into `render()` ABOVE the loop it was meant to replace, and the old loop's closing brace was left in place. Line 969 therefore closes `render()`, lines 970 to 973 become loose statements inside the IIFE, and line 974 closes the IIFE early. The trailing `})();` no longer matches, so the entire main script block fails to compile.

Proof:

- `node --check` on the extracted block (`~/.cache/roadmap-qa/s2.js`) reports `SyntaxError: Unexpected token 'function'` at line 975, `function save()`.
- Browser: one page error, `Unexpected token 'function'`.
- `window.__rmLog` and `window.__rmRender` are both `undefined`, and `window.__rmHooks.length` is 1, the new third script only.
- Clicking a tick does nothing: `data-state` stays `todo`, the meter stays `nothing ticked yet`, the trail stays empty.
- The new slab counters paint `0 of 4`, `0 of 5`, `0 of 1`, `0 of 5`, `0 of 1`, `0 of 16`, reading static HTML that nothing updates.

So the spine, meter, trail, slab jump links and section navigator are all dead at this revision. The page looks complete only because the static markup is.

## Second blocker waiting behind the first

The hook contract changed from `(log)` to `(done, n, ticks.length)`, but the navigator still registers `function (pageLog) { own = pageLog; }` at line 1273. Once the parse error is fixed, `own` becomes the `done` map and the next `say(msg)` throws `TypeError: own is not a function` inside a click handler, breaking chip clicks and both stepper buttons.

## Fix order

1. Delete lines 970 to 974 in the HTML (the orphan comment, the old loop and the extra brace).
2. Pass the logger as a fourth hook argument, or make the navigator use `window.__rmLog` directly and drop the handshake.
3. Fix `window.__rmRender = render;` (line 1151) by exporting `render` from the scope that owns it, right after the `render();` call at line 994, or drop the handshake with step 2.

Full details, exact snippets and verification commands are in `DSA-MIDTERM-ROADMAP-PATCHES.md`, which now leads with P0 and P0b.

---

# Round 10 verification, revision e5b32c74

Checked at 2026-09-18 16:54 IST against `sha256 e5b32c74af252811` (90107 bytes, 1317 lines, mtime 16:52:54).

## Fixed, and the page is alive again

The repair collapsed the two hook loops into one and deleted the orphaned block, so `render()` now ends with a single guarded loop:

```js
    var hks = window.__rmHooks || [];
    for (var h = 0; h < hks.length; h++) {
      try { hks[h](log, done, n, ticks.length); } catch (e) {}
    }
```

- All three inline scripts pass `node --check`.
- No page error from a parse failure any more; `window.__rmLog` is a function.
- Clicking ticks 5 to 9 moves the meter to `5 of 16 lectures ticked, 31 percent`, sets `aria-valuenow` to 5, writes five trail lines, and repaints the new slab row: `rm-count-b` reads `5 of 5 ticked`, `rm-fill-b` is 100 percent, the slab is `data-cleared="true"`, and its accessible name becomes `Block B, Memory and lists, lectures 5 to 9, every lecture in this block is ticked off: jump to section 03, the chain`.
- P0b is fixed by the same edit: the hook call passes `log` first, so the navigator's `function (pageLog) { own = pageLog; }` contract holds, and `done`, `n`, `total` still reach the slab painter.

## Still the one blocker: P1

`window.__rmRender = render;` at line 1147 is still out of scope, so the block still throws `render is not defined` after patch A finishes. Measured with P0 fixed: one page error, `window.__rmRender` `undefined`, `#jump-now` stuck at `01 / 11` after a 6000px scroll, the active chip stuck at `#s1`, chip clicks writing nothing to the trail, and `prev` / `next` doing nothing. The section navigator has still never run in any revision since it was added.

## Also still open

P3 (section 7 range: measured `s7` = none, `s3` = 5 to 9), P4 (`og-card.png` missing), P5, P6, P7 (re-measured: `beforeprint` twice leaves 12 answers open), P8, P9, P10, P11, P12 and the P13 judgment items. Print still works: answers present, `SHOW ANSWER` still 12 times.
