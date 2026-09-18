#!/usr/bin/env python3
"""Expands the include markers inside the lab records.

    <!--CODE:Exp3_Recursion/exp3_1_factorial.c-->
    <!--TEXT:_build/out/exp3_1_factorial.txt-->

is expanded into a fenced code block holding the current content of that file, so
the records can never drift away from the real sources and the real transcripts.
Running it twice is safe: the marker line stays and the block below it is
replaced.

Usage: python3 _scripts/inline_code.py
"""

import pathlib
import re
import sys

BASE = pathlib.Path(__file__).resolve().parent.parent
RECORDS = BASE / "Records"

PATTERN = re.compile(r"<!--(CODE|TEXT):(?P<path>[^>]+?)-->\n(?:```[a-z]*\n.*?```\n)?", re.S)


def expand(md_path: pathlib.Path) -> int:
    """Inlines every marker of one record file, returns how many were expanded."""
    text = md_path.read_text()
    count = 0

    def replace(match: re.Match) -> str:
        nonlocal count
        kind, rel = match.group(1), match.group("path")
        source = BASE / rel
        if not source.exists():
            sys.exit(f"{md_path.name}: referenced file does not exist: {rel}")
        body = source.read_text()
        if not body.endswith("\n"):
            body += "\n"
        count += 1
        fence = ""
        if kind == "CODE":
            fence = "cpp" if rel.endswith(".cpp") else "c"
        return f"<!--{kind}:{rel}-->\n```{fence}\n{body}```\n"

    md_path.write_text(PATTERN.sub(replace, text))
    return count


def main() -> int:
    files = sorted(RECORDS.glob("EXPERIMENT_*.md"))
    if not files:
        sys.exit(f"no records found in {RECORDS}")
    for md in files:
        print(f"{md.name}: {expand(md)} block(s) inlined")
    return 0


if __name__ == "__main__":
    sys.exit(main())
