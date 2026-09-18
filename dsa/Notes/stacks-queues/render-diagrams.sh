#!/usr/bin/env bash
# Render every picture this note set uses, into the diagrams folder.
#   sh ~/Documents/MUJ-SEM-3/DSA/Notes/stacks-queues/render-diagrams.sh
set -u
DIAG="$HOME/Documents/MUJ-SEM-3/DSA/Notes/stacks-queues/diagrams"
CFG=/tmp/kiki-mmdc-puppeteer.json
cat > "$CFG" <<'JSON'
{ "executablePath": "/home/zen/.cache/ms-playwright/chromium-1234/chrome-linux64/chrome",
  "args": ["--no-sandbox", "--disable-gpu", "--disable-dev-shm-usage"] }
JSON
cd "$DIAG" || exit 1
echo "== mermaid sources -> png"
for f in *.mmd; do
  out="${f%.mmd}.png"
  printf '   %-46s' "$f -> $out"
  if npx -y @mermaid-js/mermaid-cli -i "$f" -o "$out" -b white -w 1800 -p "$CFG" >/tmp/mmdc.log 2>&1; then
    echo "ok"
  else
    echo "FAILED (tail /tmp/mmdc.log)"; tail -3 /tmp/mmdc.log
  fi
done
echo "== hand-authored svg -> png"
for f in *.svg; do
  out="${f%.svg}.png"
  printf '   %-46s' "$f -> $out"
  if inkscape --export-type=png --export-dpi=110 --export-filename="$out" "$f" >/dev/null 2>&1; then
    echo "ok"
  else
    echo "FAILED"
  fi
done
echo "== result"
ls -la "$DIAG"/*.png | awk '{print "   " $5 " bytes  " $9}'
