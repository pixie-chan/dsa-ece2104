#!/usr/bin/env bash
# Render the Lecture 5 Mermaid diagrams to PNG. Uses the chromium already on this box,
# so puppeteer never downloads one.
set -u
DIAG="$HOME/Documents/MUJ-SEM-3/DSA/Notes/intro-memory/diagrams"
CFG=/tmp/kiki-intromem-puppeteer.json
cat > "$CFG" <<'CFGEOF'
{ "executablePath": "/home/zen/.cache/ms-playwright/chromium-1234/chrome-linux64/chrome",
  "args": ["--no-sandbox", "--disable-gpu", "--disable-dev-shm-usage"] }
CFGEOF
cd "$DIAG" || exit 1
for f in *.mmd; do
  out="${f%.mmd}.png"
  echo "--- $f -> $out"
  npx -y @mermaid-js/mermaid-cli -i "$f" -o "$out" -b white -w 1600 -p "$CFG" 2>&1 | tail -2
done
ls -la "$DIAG"/*.png
