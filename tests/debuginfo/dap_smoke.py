#!/usr/bin/env python3
"""Drive lldb-dap over stdio exactly the way the VS Code extension does.

Usage: dap_smoke.py <lldb-dap> <program> <source> <line> <formatter.py>

Why this exists: `editors/vscode/package.json` makes CLAIMS about lldb-dap's
launch schema — that the attributes are `program`/`args`/`cwd`/`stopOnEntry` and
that `initCommands` is honoured before the target is created. Those are not
things a TypeScript typecheck can verify, and a wrong key name fails at DEBUG
time in someone's editor, which is the worst place to find it. So the gate speaks
the protocol itself: set a source breakpoint, launch, and read a variable back
through the adapter.

It also proves the formatter injection end to end — the whole point of
resolveDebugConfiguration's initCommands is that a String reads as text in the
Variables pane, and only a real DAP session shows that.

Prints one line per finding; exits non-zero if any check failed.
"""
import json
import signal
import subprocess
import sys

# ★A hard wall-clock limit, because every read below is a BLOCKING readline on
# the adapter's stdout. Measured while building this: point it at a program that
# does not exist and lldb-dap answers the launch with an error and then simply
# says nothing more, so the wait for a `stopped` event never returns and the gate
# hangs instead of failing. A harness that can hang is the trap it exists to
# prevent — same class as a pipeline that swallows an exit code.
TIMEOUT_SECONDS = 60


class Dap:
    def __init__(self, adapter):
        self.p = subprocess.Popen(
            [adapter],
            stdin=subprocess.PIPE,
            stdout=subprocess.PIPE,
            stderr=subprocess.DEVNULL,
        )
        self.seq = 0
        self.events = []

    def send(self, command, **args):
        self.seq += 1
        body = {"seq": self.seq, "type": "request", "command": command}
        if args:
            body["arguments"] = args
        raw = json.dumps(body).encode()
        self.p.stdin.write(b"Content-Length: %d\r\n\r\n" % len(raw) + raw)
        self.p.stdin.flush()
        return self.seq

    def _read_message(self):
        length = None
        while True:
            line = self.p.stdout.readline()
            if not line:
                return None
            line = line.strip()
            if not line:
                break
            if line.lower().startswith(b"content-length:"):
                length = int(line.split(b":")[1])
        if length is None:
            return None
        return json.loads(self.p.stdout.read(length))

    def wait_response(self, seq, budget=400):
        """Responses and events interleave; keep the events, return the response
        whose request_seq matches."""
        for _ in range(budget):
            msg = self._read_message()
            if msg is None:
                return None
            if msg.get("type") == "event":
                self.events.append(msg)
            elif msg.get("type") == "response" and msg.get("request_seq") == seq:
                return msg
        return None

    def wait_event(self, name, budget=400):
        for e in self.events:
            if e.get("event") == name:
                return e
        for _ in range(budget):
            msg = self._read_message()
            if msg is None:
                return None
            if msg.get("type") == "event":
                self.events.append(msg)
                if msg.get("event") == name:
                    return msg
        return None


def report(failures):
    for f in failures:
        print("dap: FAIL " + f)
    if failures:
        return 1
    print("dap: OK")
    return 0


def main():
    if len(sys.argv) != 6:
        print("dap: usage: dap_smoke.py <lldb-dap> <program> <source> <line> <formatter>")
        return 2
    adapter, program, source, line_s, formatter = sys.argv[1:]
    line = int(line_s)
    failures = []

    def on_timeout(_sig, _frm):
        print("dap: FAIL adapter went silent (no response within %ds)"
              % TIMEOUT_SECONDS)
        sys.stdout.flush()
        # SIGKILL the whole process group's adapter child before leaving, or it
        # outlives the gate holding the debuggee.
        try:
            proc = getattr(main, "_proc", None)
            if proc is not None:
                proc.kill()
        except Exception:
            pass
        sys.exit(1)

    signal.signal(signal.SIGALRM, on_timeout)
    signal.alarm(TIMEOUT_SECONDS)

    def check(cond, what):
        if not cond:
            failures.append(what)

    d = Dap(adapter)
    main._proc = d.p
    r = d.wait_response(d.send("initialize", adapterID="scaly", clientID="gate",
                               pathFormat="path", linesStartAt1=True,
                               columnsStartAt1=True))
    check(r is not None and r.get("success"), "initialize failed")

    # These launch attributes are exactly what package.json's
    # configurationAttributes advertise. A wrong key shows up right here.
    r = d.wait_response(d.send(
        "launch", program=program, args=[], cwd=".", stopOnEntry=False,
        initCommands=["command script import " + formatter]))
    check(r is not None and r.get("success"),
          "launch failed: %s" % (r.get("message") if r else "no response"))

    r = d.wait_response(d.send("setBreakpoints",
                               source={"path": source},
                               breakpoints=[{"line": line}]))
    verified = bool(r and r.get("success") and r["body"].get("breakpoints")
                    and r["body"]["breakpoints"][0].get("verified"))
    check(verified, "breakpoint at %s:%d not verified (no line table?)" % (source, line))

    d.wait_response(d.send("configurationDone"))

    stopped = d.wait_event("stopped")
    check(stopped is not None, "never stopped at the breakpoint")
    if stopped is None:
        return report(failures)

    tid = stopped["body"].get("threadId", 1)
    r = d.wait_response(d.send("stackTrace", threadId=tid, startFrame=0, levels=1))
    frames = r["body"]["stackFrames"] if (r and r.get("success")) else []
    check(bool(frames), "no stack frames")
    if not frames:
        return report(failures)

    r = d.wait_response(d.send("scopes", frameId=frames[0]["id"]))
    scopes = r["body"]["scopes"] if (r and r.get("success")) else []
    check(bool(scopes), "no scopes")
    if not scopes:
        return report(failures)

    # Locals AND arguments are separate scopes in lldb-dap, and both classes are
    # what the -g work added — so every scope is walked.
    names = {}
    refs = {}
    for scope in scopes:
        rv = d.wait_response(d.send("variables",
                                    variablesReference=scope["variablesReference"]))
        if rv and rv.get("success"):
            for v in rv["body"]["variables"]:
                names[v["name"]] = v.get("value", "")
                refs[v["name"]] = v.get("variablesReference", 0)

    for want in ("out", "base", "name", "pick", "items"):
        check(want in names, "`%s` missing from the Variables view" % want)
    check(names.get("base") == "7",
          "`base` reads %r, expected 7" % names.get("base"))

    # A struct's top-level value in DAP is a SUMMARY (`Pick @ 0x...`); the
    # members are one expansion down, which is what a Variables pane does when
    # you click the arrow. So the union tag has to be read from the children.
    tag = None
    if refs.get("pick"):
        rv = d.wait_response(d.send("variables", variablesReference=refs["pick"]))
        if rv and rv.get("success"):
            for v in rv["body"]["variables"]:
                if v["name"] == "tag":
                    tag = v.get("value", "")
    check(tag is not None and "Chosen" in tag,
          "union tag reads %r — the DWARF enumeration is not naming the variant"
          % tag)
    # The formatter ran if the String is text rather than a struct holding a
    # pointer, and if the Vector reports its element count.
    check('"probe"' in names.get("name", ""),
          "String `name` reads %r — the initCommands formatter did not load"
          % names.get("name"))
    check("element" in names.get("items", ""),
          "`items` reads %r — the Vector formatter did not load"
          % names.get("items"))

    signal.alarm(0)
    d.send("disconnect", terminateDebuggee=True)
    try:
        d.p.wait(timeout=10)
    except subprocess.TimeoutExpired:
        d.p.kill()

    return report(failures)


if __name__ == "__main__":
    sys.exit(main())
