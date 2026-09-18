#!/bin/sh
# Builds every DAS-LAB experiment program into _build/.
# Usage: sh build.sh          (from the DAS-LAB folder)
set -e

CXX=${CXX:-g++}
CXXFLAGS="-Wall -Wextra -Wpedantic -std=c++17 -O2"
OUT=_build
LOG="$OUT/.warnings.tmp"

mkdir -p "$OUT"

warned=0
for src in Exp0_Array_Operations/*.cpp Exp1_Merge_Sort/*.cpp Exp2_Quick_Sort/*.cpp \
           Exp3_Recursion/*.cpp Exp4_Priority_Queue/*.cpp; do
    [ -f "$src" ] || continue
    name=$(basename "$src" .cpp)

    if $CXX $CXXFLAGS -o "$OUT/$name" "$src" 2> "$LOG"; then
        if [ -s "$LOG" ]; then
            echo "$src -> $OUT/$name  [warnings]"
            sed 's/^/    /' "$LOG"
            warned=$((warned + 1))
        else
            echo "$src -> $OUT/$name  [clean]"
        fi
    else
        echo "$src -> FAILED"
        sed 's/^/    /' "$LOG"
        exit 1
    fi
done

rm -f "$LOG"
echo "Build finished: $warned file(s) compiled with warnings."
