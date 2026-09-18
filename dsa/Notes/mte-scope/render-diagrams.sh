#!/usr/bin/env bash
# Render every mermaid source in this note set to a 1400 px wide PNG.
#   bash ~/Documents/MUJ-SEM-3/DSA/Notes/mte-scope/render-diagrams.sh
#
# Why the two passes: mmdc squeezes any diagram whose natural width exceeds
# the viewport width, which shrinks the text. Pass one reads the natural
# viewBox width from an SVG render; pass two re-renders the PNG with
# puppeteer scale = 1400 / natural width, so the PNG lands on exactly
# 1400 px with every glyph drawn at that higher resolution (crisp, not upscaled).
set -u
DIAG="$HOME/Documents/MUJ-SEM-3/DSA/Notes/mte-scope/diagrams"
CFG=/tmp/kiki-mmdc-puppeteer.json
TMP=/tmp/mte-scope-svg
mkdir -p "$TMP"
cat > "$CFG" <<'JSON'
{ "executablePath": "/home/zen/.cache/ms-playwright/chromium-1234/chrome-linux64/chrome",
  "args": ["--no-sandbox", "--disable-gpu", "--disable-dev-shm-usage"] }
JSON
cd "$DIAG" || exit 1
MMDC="npx -y @mermaid-js/mermaid-cli"

for f in *.mmd; do
  base="${f%.mmd}"
  if ! $MMDC -i "$f" -o "$TMP/$base.svg" -p "$CFG" >/tmp/mmdc.log 2>&1; then
    echo "$f  SVG FAILED"; tail -3 /tmp/mmdc.log; continue
  fi
  read -r W H < <(python3 - "$TMP/$base.svg" <<'PY'
import re, sys
s = open(sys.argv[1], encoding="utf-8").read(4000)
m = re.search(r'viewBox="-?[\d.]+ -?[\d.]+ ([\d.]+) ([\d.]+)"', s)
print(int(float(m.group(1))) + 1, int(float(m.group(2))) + 1) if m else print(0, 0)
PY
)
  if [ "$W" -le 0 ]; then echo "$f  no viewBox found"; continue; fi
  SCALE=$(python3 -c "print(round(1400 / min($W, 1400), 4))")
  if $MMDC -i "$f" -o "$base.png" -b white -w 1400 -s "$SCALE" -p "$CFG" >/tmp/mmdc.log 2>&1; then
    printf '%-40s natural %sx%-6s scale %-8s -> %s\n' "$f" "$W" "$H" "$SCALE" "$base.png"
  else
    echo "$f  PNG FAILED"; tail -3 /tmp/mmdc.log
  fi
done

echo "== result =="
python3 - <<'PY'
import struct, glob, os
d = os.path.expanduser("~/Documents/MUJ-SEM-3/DSA/Notes/mte-scope/diagrams")
for p in sorted(glob.glob(d + "/*.png")):
    w, h = struct.unpack('>II', open(p, 'rb').read(33)[16:24])
    print(f"{os.path.getsize(p):>9} bytes  {w}x{h}  {os.path.basename(p)}")
PY
