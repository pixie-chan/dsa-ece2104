#!/usr/bin/env bash
# Render the mid-term roadmap diagrams to PNG (same chromium trick as intro-memory).
set -u
DIAG="$HOME/Documents/MUJ-SEM-3/DSA/Notes/midterm-roadmap/diagrams"
CFG=/tmp/kiki-mmdc-puppeteer.json
cat > "$CFG" <<'JSON'
{ "executablePath": "/home/zen/.cache/ms-playwright/chromium-1234/chrome-linux64/chrome",
  "args": ["--no-sandbox", "--disable-gpu", "--disable-dev-shm-usage"] }
JSON
export PUPPETEER_CONFIG="$CFG"
cd "$DIAG" || exit 1
for f in *.mmd; do
  out="${f%.mmd}.png"
  echo "--- $f -> $out"
  npx -y @mermaid-js/mermaid-cli -i "$f" -o "$out" -b white -w 1800 -p "$CFG" 2>&1 | tail -2
done
ls -la "$DIAG"/*.png
