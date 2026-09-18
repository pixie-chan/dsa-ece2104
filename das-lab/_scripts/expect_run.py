#!/usr/bin/env python3
"""Runs a program inside a pseudo-terminal so that the typed input is echoed
back into the transcript, exactly as it looks in a real terminal session.

Usage: python3 _scripts/expect_run.py <program> [input line ...]

Each input line is sent only after the program has stopped writing, so the
echoed input lands right after its prompt. A single argument containing "|"
is split on those bars, which is the easy way to pass several lines:

    expect_run.py ./pq "1|10 3|4|3|6"

Output is printed on stdout with CRLF turned into LF and trailing blanks
removed.
"""

import os
import pty
import select
import subprocess
import sys
import time

QUIET = 0.10          # seconds of silence that mean "the program is waiting for input"
HARD_TIMEOUT = 20.0   # give up after this many seconds


def run(command: list[str], inputs: list[str]) -> str:
    master, slave = pty.openpty()
    proc = subprocess.Popen(command, stdin=slave, stdout=slave, stderr=slave,
                            close_fds=True)
    os.close(slave)

    pending = list(inputs)
    data = b""
    started = time.monotonic()

    while True:
        if time.monotonic() - started > HARD_TIMEOUT:
            proc.kill()
            break

        readable, _, _ = select.select([master], [], [], QUIET)

        if readable:
            try:
                chunk = os.read(master, 4096)
            except OSError:      # the child closed the pty: it is done
                break
            if not chunk:
                break
            data += chunk
            continue

        # Nothing was written for QUIET seconds.
        if pending:
            os.write(master, (pending.pop(0) + "\n").encode())
        elif proc.poll() is not None:
            break
        elif time.monotonic() - started > HARD_TIMEOUT - QUIET:
            proc.kill()
            break

    proc.wait()
    os.close(master)

    text = data.decode(errors="replace").replace("\r\n", "\n").replace("\r", "\n")
    return "\n".join(line.rstrip() for line in text.splitlines()) + "\n"


def main() -> int:
    if len(sys.argv) < 2:
        print(__doc__)
        return 2
    inputs = sys.argv[2:]
    if len(inputs) == 1 and "|" in inputs[0]:
        inputs = inputs[0].split("|")
    sys.stdout.write(run(sys.argv[1:2], inputs))
    return 0


if __name__ == "__main__":
    sys.exit(main())
