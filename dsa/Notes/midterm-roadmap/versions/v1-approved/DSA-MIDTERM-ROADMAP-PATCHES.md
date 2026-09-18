# Patch sheet: DSA-MIDTERM-ROADMAP.html

Agent-ready fix list. Every item has the exact location, the measured evidence, the literal edit, and a way to verify. Nothing here has been applied; this document changes nothing.

- **Target file:** `~/Documents/MUJ-SEM-3/DSA/Notes/midterm-roadmap/DSA-MIDTERM-ROADMAP.html`
- **Pinned revision:** `sha256 7fc0de2dea029f63`, 1317 lines, 90145 bytes, mtime 2026-09-18 16:54:03 IST
- **Status at this revision:** P0, P0b, P2, P4 and P12 are fixed. P1 is degraded rather than broken: the navigator runs, but its trail numbering can diverge from the page's own counter for the first navigator action. P3, P5 to P11 and the P13 judgment items are still open. A full health table for this revision is in round 12 of the review log.
- **Companion section:** `DSA-MIDTERM-ROADMAP.md` (one content fix below)
- **Chronological evidence log:** `DSA-MIDTERM-ROADMAP-REVIEW.md` (rounds 1 to 9, with the measurement method for every number)

## Instructions for the agent

1. Re-hash the file first: `sha256sum ~/Documents/MUJ-SEM-3/DSA/Notes/midterm-roadmap/DSA-MIDTERM-ROADMAP.html`
2. If the hash is not `7fc0de2dea029f63`, do not trust the line numbers. Locate each edit by the quoted snippet instead.
4. Apply one patch, verify it with that patch's check, then move on. Do not restructure the file, do not rename classes or ids, and do not reformat untouched lines.
5. Keep the page's voice: plain declarative sentences, no em dashes, and no claim on the page that is not measured or quoted from the handout.
6. Do not undo anything in the "Already fixed, do not revert" section at the end.

## Verification one-liners

Parse check for every inline script, which is the fastest way to catch a brace error:

```
awk 'BEGIN{n=0} /<script>/{n++; f=sprintf("%s/s%d.js", ENVIRON["HOME"]"/.cache/roadmap-qa", n); next} /<\/script>/{f=""; next} f!=""{print > f}' ~/Documents/MUJ-SEM-3/DSA/Notes/midterm-roadmap/DSA-MIDTERM-ROADMAP.html; for f in ~/.cache/roadmap-qa/s*.js; do echo "== $f"; node --check "$f"; done
```

Behaviour probes:

```
node ~/.cache/roadmap-qa/verify12.mjs
node ~/.cache/roadmap-qa/verify11.mjs
node ~/.cache/roadmap-qa/verify10.mjs
```

`verify12.mjs` is the smoke test: it must report `rmLog: "function"`, zero page errors, and a tick click that changes both `data-state` and the meter text.

Print check without a browser dialog. Normalise whitespace before grepping, because `pdftotext` wraps long lines and a wrapped phrase reads as missing:

```
cd /tmp && ~/.cache/ms-playwright/chromium-1234/chrome-linux64/chrome --headless --disable-gpu --no-sandbox --print-to-pdf=/tmp/p.pdf --no-pdf-header-footer file://$HOME/Documents/MUJ-SEM-3/DSA/Notes/midterm-roadmap/DSA-MIDTERM-ROADMAP.html && pdftotext /tmp/p.pdf - | tr '\n' ' ' | tr -s ' ' | grep -o -i "allocator bookkeeping\|SHOW ANSWER" | sort | uniq -c
```

Expected at this revision: `1 allocator bookkeeping` and `12 SHOW ANSWER`. The first number proves the answer bodies print, the second is P8. Verified the same way with JavaScript disabled (`page.setJavaScriptEnabled(false)`), so the reveal is carried by the CSS, not only by the `beforeprint` handler.

## Patch index

| ID | Severity | One line |
|---|---|---|
| P0 | FIXED | The stray brace in `render()` was repaired at `e5b32c74`, all three scripts parse |
| P0b | FIXED | The hook call is now `hks[h](log, done, n, ticks.length)`, so the navigator's logger contract holds |
| P1 | DEGRADED | The throw is gone so the navigator finally runs, but the export is always `null`, so the trail shows two counter sequences |
| P2 | DONE | Duplicate hook loop removed; `render()` now has one guarded loop |
| P3 | Medium | `data-lectures="16"` on section 7 is skipped, so the spine never follows the recursion section |
| P4 | FIXED | `og-card.png` now exists and is 1200x630; only the absolute-URL note remains |
| P5 | Medium | `overscroll-behavior: contain` on the rail freezes page scrolling |
| P6 | Medium | The `prefers-color-scheme` branch is inert, a dark-OS reader is flipped to light and it is persisted |
| P7 | Low | Printing twice leaves all twelve answers open |
| P8 | Low | `SHOW ANSWER` prints twelve times, orphaned labels |
| P9 | Low | The timeline ticks stop 21px short of the rule |
| P10 | Content | "4 x 48 = 192" contradicts the measured 144 two lines above |
| P11 | Cleanup | Dead `.mapnode`, eight unused tokens, unused `#ready-count`, head script outside `<head>` |
| P12 | FIXED | The markdown twin now says "twelve" |
| P13 | Judgment | Five deliberate choices worth a second look, not defects |

---

## P0, FIXED at revision e5b32c74, kept as a record

**Resolved by** collapsing the two loops into one and deleting the orphaned block. `render()` now ends with:

```js
    /* one guarded loop: a failed hook must not break the meter */
    var hks = window.__rmHooks || [];
    for (var h = 0; h < hks.length; h++) {
      try { hks[h](log, done, n, ticks.length); } catch (e) {}
    }
  }
```

Verified after the repair: all three inline scripts pass `node --check`, no page error, `window.__rmLog` is a function, clicking five ticks moves the meter to `5 of 16 lectures ticked, 31 percent`, sets `aria-valuenow` to 5, paints `rm-count-b` to `5 of 5 ticked` with a 100 percent fill, and writes five trail lines. Nothing left to do here.

**What it was:** lines 965 to 974 held a new hook loop inserted above the loop it was meant to replace, so `render()` closed early and the IIFE closed at the old brace, leaving the trailing `})();` unmatched. The whole main script failed to compile: `node --check` reported `SyntaxError: Unexpected token 'function'` at `function save()`, the browser reported `Unexpected token 'function'`, `window.__rmLog` was `undefined`, ticks did nothing, and the slab counters sat at `0 of N`.

```js
    /* a failed hook must not break the meter, so each one is isolated */
    for (var h = 0; h < window.__rmHooks.length; h++) {
      try { window.__rmHooks[h](done, n, ticks.length); } catch (e) {}
    }
  }
    /* let anything registered in window.__rmHooks see the page's own logger */
    for (var hk = 0; hk < (window.__rmHooks || []).length; hk++) {
      try { window.__rmHooks[hk](log); } catch (e) {}
    }
  }
```

The new loop and its closing brace were inserted above the old loop instead of replacing it, so line 969 closes `render()` early, lines 970 to 973 become loose statements, and line 974 closes the IIFE. The trailing `})();` then has no opening brace to match, and the whole `<script>` block fails to compile.

**Measured on this revision**

- `node --check` on the extracted block: `SyntaxError: Unexpected token 'function'` pointing at line 975, `function save()`.
- Browser: one page error, `Unexpected token 'function'`.
- `window.__rmLog` and `window.__rmRender` are both `undefined`, and `window.__rmHooks.length` is 1 (only the new third script registered).
- Clicking a tick does nothing at all: `data-state` stays `todo`, the meter stays `nothing ticked yet`, and the trail stays empty.
- The slab counters paint `0 of 4`, `0 of 5`, `0 of 1`, `0 of 5`, `0 of 1`, `0 of 16` because they read the static HTML state that nothing updates.

So the spine, the meter, the trail, the slab jump links and the section navigator are all dead, and the page only looks alive because the static markup is complete.

**Edit.** Delete lines 970 to 974, the orphaned comment, the old loop and the extra brace. The result must read:

```js
    if ($("co1-count")) $("co1-count").textContent = co1 + " / 10";
    if ($("co2-count")) $("co2-count").textContent = co2 + " / 6";
    /* a failed hook must not break the meter, so each one is isolated */
    for (var h = 0; h < window.__rmHooks.length; h++) {
      try { window.__rmHooks[h](done, n, ticks.length); } catch (e) {}
    }
  }
  function save() { try { localStorage.setItem("dsa-roadmap-done", JSON.stringify(done)); } catch (e) {} }
```

**Verify:** the parse check above reports no error for `s2.js`, and `node ~/.cache/roadmap-qa/verify12.mjs` reports `rmLog: "function"`, no page errors, and a tick click that changes `data-state` to `done`.

---

## P0b, FIXED at revision e5b32c74, kept as a record

**Resolved by** the same repair: the call is now `hks[h](log, done, n, ticks.length)`, so the logger is the first argument again and the navigator's existing `function (pageLog) { own = pageLog; }` keeps working, while the slab painter still receives the counts. Verified: `window.__rmHooks.length` is 2 and the slab counters repaint from real tick state.

**What it was:** line 1273, in the section navigator.

```js
  window.__rmHooks.push(function (pageLog) { own = pageLog; });
```

For one revision the hook was called with `(done, n, ticks.length)`, which handed this hook the `done` map as `pageLog`, so `own` became an object and the next `say(msg)` would have thrown `TypeError: own is not a function` inside a click handler.

---

## P1, DEGRADED at revision 7fc0de2d: the navigator runs, but the trail has two counters

**Where:** line 1147, the last statements of the patch A IIFE, and the handshake at line 1270.

As of `7fc0de2d` the line reads:

```js
  window.__rmRender = typeof render === "function" ? render : null;
```

`typeof` on an undeclared name does not throw, so the ReferenceError is gone and the script block now runs to the end. That is real progress: the section navigator executes for the first time. But the guard can never be true, because `render` is still out of scope here, so `window.__rmRender` is always `null` and the handshake at line 1270 is always skipped. Measured at this revision: `rmRender: "null"`, `hooks: 3`, no page errors, `#jump-now` moves to `03 / 11` on scroll with the active chip on `#s3`, a chip click writes `1. jumped to 06 numericals`, and `next` writes `2. stepped on to 07 evidence`.

The cost is the trail numbering, and it is narrower than it first looks. The logger does still arrive, through the other route: `render()` calls every hook with `log` first, so the navigator's `function (pageLog) { own = pageLog; }` sets `own` the first time the page ticks a lecture. The divergence therefore only shows if the reader touches the navigator BEFORE ticking anything, because until then `own` is null and the fallback `unattached` counter starts from the current list length and counts independently of the page's `ACTIONS`. Measured on a fresh load, two navigator moves then two tick clicks:

```
1. jumped to 06 numericals
2. stepped on to 07 evidence
1. lecture 3 ticked off
2. lecture 4 ticked off
```

Two sequences in one column, both claiming to start at 1. In the same health check run, ticking first and navigating after gave a clean 1 to 10, which confirms the race rather than a permanent split.

**Edit, smallest correct version.** Delete the assignment in patch A entirely, so it stops clobbering the export with `null`:

```js
  window.__rmHooks = window.__rmHooks || [];
  window.__rmHooks.push(function () { syncAll(); });
})();
```

and export the function from the scope that owns it, in the first IIFE immediately after its existing `render();` call at line 990:

```js
  render();
  window.__rmRender = render;
```

Order matters: the first IIFE runs before patch A, so if patch A keeps assigning anything to `window.__rmRender` it will overwrite the good value.

**Edit, cleanest version.** Skip the handshake. `window.__rmLog` is already a function (verified), so the navigator's `say()` can call `window.__rmLog(msg)` directly, and `own`, `unattached`, `window.__rmRender = render` and the `if (window.__rmRender)` call at line 1270 can all go. That removes the whole class of problem, including the ordering trap above.

**Verify:** click two navigator controls, then click two ticks, and confirm the trail numbers run 1, 2, 3, 4 with no repeats. `node ~/.cache/roadmap-qa/verify14.mjs` prints the interleave test directly.

**Alternative:** take option 2 of P0b and delete this export entirely.

**Verify:** `node ~/.cache/roadmap-qa/verify11.mjs` must report `hookCount: 2`, `rmRender: "function"`, no page errors, and a `jumpNow` that changes after scrolling.

---

## P2, DONE in revision 4fdfb930, kept as a record

The duplicate hook loop that used to run every hook twice was removed. That removal is what introduced the brace error in P0, because the new loop was added above the old one rather than in place of it. Fix P0 by deleting the leftover block, and do not restore the duplicate loop.

---

## P3, Medium: section 7 never highlights the spine

**Where:** lines 998 to 999.

```js
      var parts = (marked[m].getAttribute("data-lectures") || "").split(/\s+/);
      if (parts.length === 2) RANGES[marked[m].id] = [parseInt(parts[0], 10), parseInt(parts[1], 10)];
```

`<section id="s7" data-lectures="16">` (line 758) carries one value, so it fails the `parts.length === 2` test and never enters `RANGES`. Measured while scrolling: `s7` leaves every tick at `data-current="false"`, while `s3` correctly lights ticks 5 to 9.

**Edit.**

```js
      var parts = (marked[m].getAttribute("data-lectures") || "").trim().split(/\s+/);
      var lo = parseInt(parts[0], 10);
      if (isNaN(lo)) continue;
      var hi = parts.length > 1 ? parseInt(parts[1], 10) : lo;
      RANGES[marked[m].id] = [lo, isNaN(hi) ? lo : hi];
```

**Verify:** scroll to `#s7` and confirm tick 16 reports `data-current="true"`.

---

## P4, FIXED at revision 7fc0de2d, kept as a record

`og-card.png` now exists beside the SVG and `identify` reports `PNG 1200x630` (76061 bytes), so the `og:image` reference at line 23 resolves.

**Still worth doing when the page is published:** `og:image` must be an absolute URL on the host that serves the page, and `og:url` (line 22) is the repo root (`https://github.com/pixie-chan/dsa-ece2104`, resolves 200), not the page URL if the HTML sits in a subfolder such as `Notes/midterm-roadmap/`.

---

## P5, Medium: the sticky rail freezes page scrolling

**Where:** line 155.

```css
  max-height:calc(100dvh - var(--s-6)); overflow-y:auto; overscroll-behavior:contain; padding-right:var(--s-2)}
```

Keep the `max-height` and `overflow`: they took the rail from 1139px to 868px and made the bottom controls reachable. `overscroll-behavior:contain` is the problem. Measured wheel trace with the cursor over the rail: page scrolled 400, 800, 1200, then the rail scrolled 0 to 270 (its maximum), then two further wheel events moved nothing at all, while moving the cursor over the main column resumed page scrolling. The rail is 322px of viewport width.

**Edit.** Drop `overscroll-behavior:contain`, or set `overscroll-behavior-y:auto`.

**Verify:** with the cursor over the rail, scroll past the rail's internal maximum and confirm the page keeps scrolling.

---

## P6, Medium: the OS dark preference is overridden and persisted

**Where:** line 931.

```js
  applyTheme(savedTheme === "dark" ? "dark" : "light");
```

The head script (lines 5 to 13) correctly resolves the sheet before the first paint, including `prefers-color-scheme: dark`, and then this line undoes it whenever storage is empty. Measured in a clean profile with `prefers-color-scheme: dark` and empty storage: after load the attribute is `light` and storage reads `"light"`, so a dark-OS reader gets a dark first paint, a flip to light, and light persisted.

**Edit.**

```js
  applyTheme(document.documentElement.getAttribute("data-theme") === "dark" ? "dark" : "light");
```

**Verify:** launch with `prefers-color-scheme: dark` and empty storage, confirm the attribute stays `dark` after load.

---

## P7, Low: printing twice leaves every answer open

**Where:** the print handlers, `window.__rmRestoreOpen = restore;` at line 1043.

`beforeprint` snapshots the open state and `afterprint` restores it. If `beforeprint` fires twice before `afterprint`, the second snapshot records the all-open state. Measured: `beforeprint`, `beforeprint`, `afterprint` leaves 12 of 12 answers open.

**Edit.** Guard the snapshot and clear it on restore:

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

**Verify:** `node ~/.cache/roadmap-qa/verify4.mjs`, the `double` field must equal `beforePrintOpen`.

---

## P8, Low: `SHOW ANSWER` prints twelve times

**Where:** line 283, inside `@media print`.

```css
  details.ans > *{display:block !important}
```

That legacy rule also matches `<summary>`, and `!important` beats the newer `details.ans > summary{display:none}` on line 281, so each printed answer is preceded by an orphan label. Measured in the printed PDF: `SHOW ANSWER` 12 times, with the answer body present each time.

**Edit.** `details.ans > *:not(summary){display:block !important}`, or delete line 283, since line 282 already reveals the content in Chromium.

**Verify:** reprint, `pdftotext` must find `SHOW ANSWER` 0 times while `allocator bookkeeping` is still present.

---

## P9, Low: the timeline ticks stop short of the rule

**Where:** lines 123, 125 and 131.

Measured at 1460px: the rule occupies 64 to 66px, the axis labels 96 to 115px, the slabs start at 117px, and each `.slab::before` tick runs 87 to 117px. Every tick hangs 21px below the rule and runs through the axis label text.

**Edit.** Line 131 becomes `top:-51px; height:51px`, so the tick spans from the rule's bottom edge to the slab. The tick then passes behind the labels, so either give the labels a chip, `.blocks__axis span{background:var(--bg)}`, or move the ticks to the slab's right edge.

**Verify:** re-measure the rule bottom and the tick top, the gap must be 0.

---

## P10, Content: 144 against 192 in the same list

**Where:** lines 785 and 786.

Line 785 says "The probe measured 144 bytes across four nested frames". Line 786 says "4 x 48 = 192 bytes of recursion". The source `~/learning/cpp/exercises/lec16_recursion_output.txt` gives the first frame at `0x7ffc71522ad4` and the innermost at `0x7ffc71522a44`. That difference is `0x90` = 144 = exactly 3 strides of 48, so 144 is an address span across four frames, not the size of four frames. As written the page shows two numbers a student will read as contradictory.

**Edit.** Replace line 786 with wording that separates span from charge:

```html
            <li>Question 11 charges five frames, not four. The 144 bytes above is the address span from the outermost to the innermost of the four recursive frames, which is three 48 byte strides; charging every live frame gives 4 x 48 = 192 for the recursion, and 5 x 48 = 240 once <code>main</code> is counted.</li>
```

**Verify:** `python3 -c "print(0x7ffc71522ad4-0x7ffc71522a44)"` prints 144.

---

## P11, Cleanup

- **Line 275**, `.mapnode` in the print rule matches no element in this file (0 elements carry that class). A leftover from the lecture 5 concept-map page.
- **Unused custom properties**, declared and never read at this revision: `--dur-1`, `--rule`, `--shadow-2`, `--w-reg`, `--w-med`, `--w-semi`. (`--hair` and `--s-1` are now used by the slab state styles.) Either delete them or use them; `tokens.css` in the same folder still declares the full set.
- **Line 378**, `id="ready-count"` is never referenced by the script. Either wire it to the CO2 progress or drop the id so it does not read like a live hook.
- **Lines 5 to 13**, the theme boot script sits between `<html>` and `<head>`, which is invalid markup. The parser recovers by creating an implicit head (verified: `documentElement.children` is `HEAD,BODY` and the script lands in the head), so behaviour is correct. Moving it to be the first child inside `<head>` is equivalent and valid.
- **Token drift: RESOLVED.** `tokens.css` was rewritten as a generated export of the page's own `:root` and `[data-theme="dark"]` blocks by `~/scripts/export-tokens.py`. Verified at this revision: `python3 ~/scripts/export-tokens.py DSA-MIDTERM-ROADMAP.html --check` prints `clean: tokens.css already matches DSA-MIDTERM-ROADMAP.html (46 properties)` and exits 0, and the two token sets are identical in both directions. Regenerate with the same script (no `--check`) after any palette change, and keep `--check` in the QA pass so this cannot drift again.

---

## P12, FIXED at revision 7fc0de2d, kept as a record

`DSA-MIDTERM-ROADMAP.md:93` now says "the twelve numerical problems", matching the twelve items in section 6 and the twelve rows in the answer key.

---

## P13, Judgment calls, not defects

- **The chip strip steals about one gesture of vertical wheel.** Measured with the bar pinned at the top: the first wheel event over the strip scrolls the strip 0 to 161 (its maximum) and the page stays put, then the next events chain to the page normally. So it costs roughly one wheel notch at 1460px, and about four at 375px where the overflow is 769px. Annoying, not a freeze, and unlike P5 the page is never stuck. The chips are also reachable by tab and by prev/next, so dropping the handler is still defensible.
- **The new slab state row hardcodes the block ranges a third time.** The second new script carries `SLABS = [{id:"a", lo:1, hi:4}, ...]` at line 1283, while the same ranges already exist as `data-lectures="1-4"` on the slab elements and as `data-lectures` on the sections. Deriving the table from the slab attribute would leave one source, and the hook now receives the tick state as arguments.
- **`aria-live="polite"` on `#jump-now`** announces the section counter while the reader scrolls, and it is the third live region alongside `#meter-text` and the trail. `aria-current` on the chips already carries the state.
- **`.slab__link` covers the whole slab**, so the block text can no longer be selected or copied. Acceptable for a jump card, worth knowing.
- **`data-lectures` has two formats in one file:** `"1-4"` on the slab elements and `"5 9"` on the section elements. `lecList()` accepts comma and dash, the range parser accepts spaces. Pick one convention before the next patch adds a third.

---

## Already fixed, do not revert

Measured working before the P0 brace error was introduced:

- Print: answers print (CSS `::details-content` plus the `beforeprint` handler), and the nav and slab links are hidden in print.
- Rail: `max-height: calc(100dvh - var(--s-6))` with internal scroll, height down from 1139px to 868px.
- `color-scheme: light` and `color-scheme: dark` in the two `:root` blocks, and `meta[name="theme-color"]` updated by `applyTheme()`.
- Theme boot script ahead of the first paint.
- Lecture spine: `aria-label` reads the topic from the tick's own span, `.tick__n` is `aria-hidden`, `aria-valuenow` tracks the count (verified 0, 2, 16, back to 0), the trail is capped at 40, and `tick-reset` clears the trail.
- Slab jump links: native anchor plus flash, `aria-current`, and a dynamically written accessible name that reports block progress.
- Sticky section navigator markup, chips as real anchors, `tabindex="-1"` on `#s1`, `scope="col"` on every table header, `data-lectures` on the sections for the spine ranges.
- The `NOSTATE` comment states the three Google font fetches and the offline fallback.
- Token drift is closed and now has a gate: `python3 ~/scripts/export-tokens.py DSA-MIDTERM-ROADMAP.html --check` must print `clean` and exit 0.

## Appendix: how these numbers were taken

- Inline scripts extracted with `awk` and parsed with `node --check`, which is how the P0 syntax error was isolated to `function save()`.
- Headless Chromium (Playwright build `chromium-1234`) at 1460x900, 900x800 and 375x812, with `pageerror` and console capture.
- Geometry from `getBoundingClientRect()` after explicit scrolls; wheel behaviour from real wheel events at a cursor position.
- Print from `page.pdf()` and from `--print-to-pdf`, then `pdftotext` plus control phrases known to be present, so a zero count means "missing" rather than "extraction failed".
- Content claims cross-checked against `~/learning/cpp/exercises/lec05_run_output.txt` and `lec16_recursion_output.txt`.
- Probe scripts and snapshots: `~/.cache/roadmap-qa/` (`verify4.mjs` to `verify12.mjs`, `baseline.html`, `reviewed.sha`, `s1.js` to `s3.js`).
