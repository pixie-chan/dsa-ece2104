#!/usr/bin/env python3
"""Builds the printable lab record book from the record markdown files.

Steps:
  1. Inlines every <!--CODE:...--> and <!--TEXT:...--> marker in Records/*.md
     (see _scripts/inline_code.py), so the records always show the current
     sources and the current transcripts.
  2. Renders all records into one self-contained HTML file.

Usage: python3 _scripts/make_records_html.py
Output: Records/DAS-LAB_Records.html
Then:   libreoffice --headless --convert-to pdf --outdir Records Records/DAS-LAB_Records.html
"""

import pathlib
import sys

import markdown

sys.path.insert(0, str(pathlib.Path(__file__).resolve().parent))
import inline_code  # noqa: E402  (needs the path tweak above)

BASE = pathlib.Path(__file__).resolve().parent.parent
RECORDS = BASE / "Records"

SOURCES = [
    RECORDS / "EXPERIMENT_0_RECORD.md",
    RECORDS / "EXPERIMENT_1_RECORD.md",
    RECORDS / "EXPERIMENT_2_RECORD.md",
    RECORDS / "EXPERIMENT_3_RECORD.md",
    RECORDS / "EXPERIMENT_4_RECORD.md",
]

CSS = """
@page { size: A4; margin: 18mm 16mm; }
body { font-family: "DejaVu Serif", Georgia, serif; font-size: 10.5pt; line-height: 1.5;
       color: #111; max-width: 190mm; margin: 0 auto; padding: 8mm; }
h1 { font-family: "DejaVu Sans", Arial, sans-serif; font-size: 17pt; color: #123c6b;
     border-bottom: 2px solid #123c6b; padding-bottom: 4px; margin-top: 0; }
h2 { font-family: "DejaVu Sans", Arial, sans-serif; font-size: 13pt; color: #123c6b;
     border-bottom: 1px solid #b9c8db; padding-bottom: 2px; margin-top: 18px; }
h3 { font-family: "DejaVu Sans", Arial, sans-serif; font-size: 11.5pt; color: #23486f;
     margin-top: 14px; }
code { font-family: "DejaVu Sans Mono", "Courier New", monospace; font-size: 9.5pt;
       background: #f2f4f7; padding: 0 2px; border-radius: 2px; }
pre { background: #f7f8fa; border: 1px solid #ccd4de; border-left: 3px solid #123c6b;
      border-radius: 3px; padding: 7px 9px; overflow-x: auto; page-break-inside: avoid; }
pre code { background: none; font-size: 8.6pt; line-height: 1.35; }
table { border-collapse: collapse; width: 100%; margin: 8px 0; font-size: 9.8pt;
        page-break-inside: avoid; }
th, td { border: 1px solid #b9c8db; padding: 4px 6px; text-align: left; vertical-align: top; }
th { background: #e8eef6; font-family: "DejaVu Sans", Arial, sans-serif; }
ul, ol { margin: 6px 0 6px 18px; }
li { margin: 2px 0; }
hr { border: 0; border-top: 1px dashed #b9c8db; margin: 14px 0; }
.page-break { page-break-before: always; }
strong { color: #000; }
"""


def render() -> pathlib.Path:
    # 1. pull the current sources and transcripts into the markdown
    for md in sorted(RECORDS.glob("EXPERIMENT_*.md")):
        count = inline_code.expand(md)
        print(f"  inlined {count:2d} block(s) into {md.name}")

    # 2. turn every record into HTML, one record per printed page block
    parts = []
    for i, src in enumerate(SOURCES):
        if not src.exists():
            sys.exit(f"missing record: {src}")
        if i:
            parts.append('<div class="page-break"></div>')
        parts.append(markdown.markdown(
            src.read_text(),
            extensions=["tables", "fenced_code", "sane_lists"],
        ))

    html = (
        "<!DOCTYPE html>\n<html lang=\"en\">\n<head>\n<meta charset=\"utf-8\">\n"
        "<title>DAS-LAB Lab Records: Experiments 0 to 4</title>\n"
        f"<style>{CSS}</style>\n</head>\n<body>\n"
        + "\n".join(parts)
        + "\n</body>\n</html>\n"
    )

    out = RECORDS / "DAS-LAB_Records.html"
    out.write_text(html)
    print(f"wrote {out}  ({len(html.splitlines())} lines)")
    return out


if __name__ == "__main__":
    render()
