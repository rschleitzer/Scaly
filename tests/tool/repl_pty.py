#!/usr/bin/env python3
# repl_pty.py <compiler> <scratch dir> -- the REPL at a TERMINAL.
#
# tests/tool/repl.session comes through a pipe, where lineedit.read_line reads
# the line as it comes: no raw mode, no history, no redraw. This drives the
# same binary through a pty, so the line editor itself runs -- under poison,
# with HOME in the scratch directory. It waits for what it expects instead of
# sleeping, and says which step it was waiting at.
#
#   second entry   the history is read back when a second line is remembered
#                  (the REPL died here on 2026-10-01: two locals named
#                  `history`, the editor's Array handed on as the session's
#                  StringBuilder)
#   up, up         an older line comes back and runs
#   left, delete   the cursor moves inside a line; an umlaut is one step
#   Ctrl-C         drops the line, the session goes on
#   the file       ~/.scaly/history holds the lines, a repeated one once
#   next session   starts with them
import os, pty, select, sys, time

BIN, SCRATCH = sys.argv[1], sys.argv[2]
HOME = os.path.join(SCRATCH, "home")
os.makedirs(HOME, exist_ok=True)
ENV = dict(os.environ, HOME=HOME, SCALY_POISON="1", TERM="xterm")
UP, LEFT = b"\x1b[A", b"\x1b[D"


class Session:
    def __init__(self):
        self.pid, self.fd = pty.fork()
        if self.pid == 0:
            os.execve(BIN, [BIN], ENV)
        self.seen = b""
        self.at = 0

    def expect(self, text, step):
        want = text.encode()
        deadline = time.time() + 30
        while True:
            i = self.seen.find(want, self.at)
            if i >= 0:
                self.at = i + len(want)
                return
            left = deadline - time.time()
            if left <= 0 or not select.select([self.fd], [], [], left)[0]:
                self.fail(step, "no %r" % text)
            try:
                chunk = os.read(self.fd, 4096)
            except OSError:
                chunk = b""
            if not chunk:
                self.fail(step, "the REPL ended before %r" % text)
            self.seen += chunk

    def send(self, keys):
        os.write(self.fd, keys if isinstance(keys, bytes) else keys.encode())

    def entry(self, keys, answer, step):
        self.expect("scaly> ", step)
        self.send(keys)
        self.send("\r")
        self.expect(answer, step)

    def end(self, step):
        self.expect("scaly> ", step)
        self.send(b"\x04")
        deadline = time.time() + 30
        while time.time() < deadline:
            pid, status = os.waitpid(self.pid, os.WNOHANG)
            if pid:
                if status != 0:
                    self.fail(step, "exit status %d" % status)
                return
            if select.select([self.fd], [], [], 0.05)[0]:
                try:
                    os.read(self.fd, 4096)
                except OSError:
                    pass
        self.fail(step, "still running after Ctrl-D")

    def fail(self, step, what):
        tail = self.seen[-200:].decode("utf-8", "replace").replace("\x1b", "^[").replace("\r", "")
        print("%s: %s; last output: %r" % (step, what, tail))
        try:
            os.kill(self.pid, 9)
        except OSError:
            pass
        sys.exit(1)


s = Session()
s.entry("let a 3", "a: int = 3", "first entry")
s.entry("a + 4", "it: int = 7", "second entry")
s.entry(UP + UP, "a: int = 3", "up, up")
# "a + 59", left over the 9, delete the 5: a + 9
s.entry(b"a + 59" + LEFT + b"\x7f", "it: int = 12", "left, delete")
# "a + 1\u00e4x": left over x and the umlaut (two bytes, one step), delete the 1,
# insert 0, to the end, delete x and the umlaut: a + 0
s.entry("a + 1\u00e4x".encode() + LEFT + LEFT + b"\x7f" + b"0" + b"\x05" + b"\x7f\x7f", "it: int = 3", "umlaut")
s.expect("scaly> ", "Ctrl-C")
s.send(b"a + 100\x03")
s.expect("^C", "Ctrl-C")
s.entry("a + 5", "it: int = 8", "after Ctrl-C")
s.end("Ctrl-D")

want = ["let a 3", "a + 4", "let a 3", "a + 9", "a + 0", "a + 5"]
path = os.path.join(HOME, ".scaly", "history")
try:
    got = open(path, encoding="utf-8").read().split("\n")[:-1]
except OSError as e:
    print("the file: %s" % e)
    sys.exit(1)
if got != want:
    print("the file: %r, expected %r" % (got, want))
    sys.exit(1)

s = Session()
s.expect("scaly> ", "next session")
s.send(UP)
s.expect("scaly> a + 5", "next session")
s.send(b"\x03")
s.end("next session Ctrl-D")
print("ok")
