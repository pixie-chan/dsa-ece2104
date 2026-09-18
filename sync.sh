#!/usr/bin/env bash
# Mirror the live course trees into this repo, then commit and push.
# POSIX friendly on purpose: `sh sync.sh` works as well as `bash sync.sh`.
#
#   sh ~/projects/dsa-ece2104/sync.sh
#   cd ~/projects/dsa-ece2104 && git add -A && git commit -m "sync: <what changed>" && git push
#
set -eu
REPO="$HOME/projects/dsa-ece2104"
SRC_DSA="$HOME/Documents/MUJ-SEM-3/DSA"
SRC_LAB="$HOME/Documents/MUJ-SEM-3/DAS-LAB"
SRC_CPP="$HOME/learning/cpp"
SRC_DSA_LEARN="$HOME/learning/dsa"

# one flag string, reused for every rsync
X="--exclude .git/ --exclude __pycache__/ --exclude *.o --exclude *.out --exclude *.pdf"
X="$X --exclude .qa/ui-gate-*.png --exclude .qa/review-*.png --exclude .qa/zoom-*.png"
X="$X --exclude .DS_Store --exclude compile_commands.json --exclude Handouts/"

mkdir -p "$REPO/dsa" "$REPO/das-lab" "$REPO/learning/cpp" "$REPO/learning/dsa"

echo "== DSA notes and guides =="
rsync -a --delete $X "$SRC_DSA/" "$REPO/dsa/"

echo "== DAS lab (without _build) =="
rsync -a --delete $X --exclude _build/ "$SRC_LAB/" "$REPO/das-lab/"

echo "== C++ lessons and verifiers =="
rsync -a --delete $X "$SRC_CPP/" "$REPO/learning/cpp/"

echo "== DSA learning extras =="
rsync -a --delete $X "$SRC_DSA_LEARN/" "$REPO/learning/dsa/"

echo "== removing any compiled binaries that slipped through =="
find "$REPO" -type f -exec file {} + 2>/dev/null \
  | grep -E 'ELF .*executable|ELF .*shared object' \
  | cut -d: -f1 | while read -r f; do
      echo "  removed binary: ${f#$REPO/}"; rm -f "$f"
    done

cd "$REPO"
echo
echo "== status =="
git status --short 2>/dev/null | head -30 || echo "(no git repo yet)"
echo
echo "Working tree size: $(du -sh --exclude=.git . | cut -f1)"
echo "Files: $(find . -type f -not -path './.git/*' | wc -l)"
echo
echo "Next: git add -A && git commit -m \"sync: <what changed>\" && git push"
