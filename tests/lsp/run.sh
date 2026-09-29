#!/usr/bin/env bash
# Build + smoke-test the scalyls LSP server and its json/rpc unit tests.
#
# The scalyls package depends on scalyc (the diagnostics pipeline), so the
# whole compiler — including the LLVM-backed Emitter — links into every
# scalyls consumer. Hence every link here passes -lLLVM-20.
#
# Usage: tests/lsp/run.sh [scalyc-binary] [mode]
#   scalyc-binary  default /tmp/scalyc_stage2 (the canonical self-hosted stage)
#   mode           selfhosted (default) | cpp
#
# `selfhosted` mode emits the three shared package roots once (scalyc / scaly /
# scalyls) and reuses them, linking each scalyls consumer from four objects (the
# seed.sh "4-root" recipe). This is the canonical gate.
#
# `cpp` mode is DEAD: it drove the C++ stage-0, which is retired entirely
# (sources frozen under retired/scalyc0/, no build). The code path remains
# only as history; invoking it cannot work against the current tree.
set -euo pipefail

cd "$(dirname "$0")/../.."
SCALYC="${1:-/tmp/scalyc_stage2}"
MODE="${2:-selfhosted}"
export SCALYLS_MODE="$MODE"   # available to python blocks (no self-hosted gaps remain)

# Resolve the LLVM-20 lib dir (Homebrew / apt). The diagnostics pipeline
# pulls in the LLVM-backed Emitter, so every link below needs it.
LIBDIR="${LLVM20:+$LLVM20/lib}"   # the override tools/llvm-env.sh honors
if [ -z "$LIBDIR" ] && command -v brew >/dev/null 2>&1; then
    LIBDIR="$(brew --prefix llvm@20 2>/dev/null)/lib"
fi
[ -d "$LIBDIR" ] || LIBDIR="/usr/lib/llvm-20/lib"
[ -d "$LIBDIR" ] || LIBDIR="/opt/homebrew/opt/llvm@20/lib"
# -lm: the stdlib's tensor tape kernels call tanhf/expf/sqrtf/logf/powf, which
# live in a separate libm on Linux (libSystem on macOS). LINK is appended after
# the objects, as a left-to-right ELF linker requires.
LINK=(-L"$LIBDIR" -lLLVM-20 -lm)

pass=0
fail=0
ok()   { echo "PASS  $1"; pass=$((pass+1)); }
bad()  { echo "FAIL  $1"; fail=$((fail+1)); }
# The slowest checks (a server planning a whole package per request) run in
# the background beside the rest and are collected before the summary
# (2026-09-26): each has its own work directory under /tmp/lsp_ws and only
# reads /tmp/scalyls. Measured alone the suite spent 122 of its 156 test
# seconds in these six, one after the other. ★A background block may hold NO
# ok/bad of its own: those would count in the subshell and vanish.
BGDIR="$(mktemp -d)"
BG_PIDS=(); BG_NAMES=(); BG_OUTS=()
bg_add() { BG_PIDS+=("$1"); BG_NAMES+=("$2"); BG_OUTS+=("$3"); }
bg_collect() {
    local i
    for i in "${!BG_PIDS[@]}"; do
        if wait "${BG_PIDS[$i]}"; then cat "${BG_OUTS[$i]}"; ok "${BG_NAMES[$i]}"
        else cat "${BG_OUTS[$i]}"; bad "${BG_NAMES[$i]}"; fi
    done
}

# ---- scalyls build backend (cpp single-binary | selfhosted 4-root) --------
LSO=""        # dir holding the shared .o objects (selfhosted only)
if [ "$MODE" = selfhosted ]; then
    set +u; source tools/llvm-env.sh >/dev/null; set -u
    [ "${llvm_env_ok:-0}" = "1" ] || { echo "FAIL  llvm-env (need LLVM 20 for selfhosted)"; exit 1; }
    echo "scalyls build: selfhosted 4-root via $SCALYC"
    LSO="$(mktemp -d)"
    trap 'rm -rf "$LSO"' EXIT
    # The four roots are independent: emit and llc each one in its own job.
    # ★SCALYLS_PREBUILT=<server> (tools/bar.sh passes scalyc/build/scalyls,
    # which its build step made from the SAME fresh seed with opt -O2): the
    # server is that binary, and the objects only link the small test programs
    # (json_test, echo, format_test), so llc runs at -O0 -- 3 s instead of the
    # 63 s default codegen spends on scalyc.ll alone (2026-09-26).
    LLC_LEVEL=()
    [ -n "${SCALYLS_PREBUILT:-}" ] && LLC_LEVEL=(-O0)
    lsp_root() {
        ( ulimit -s 65520; "$SCALYC" -S --no-tests -o "$LSO/$1.ll" "$2" ) \
            || { echo "FAIL  selfhosted scalyls emission ($1)"; return 1; }
        "$LLC" ${LLC_LEVEL[@]+"${LLC_LEVEL[@]}"} -relocation-model=pic -filetype=obj "$LSO/$1.ll" -o "$LSO/$1.o" \
            || { echo "FAIL  llc $1"; return 1; }
    }
    pids=()
    lsp_root scalyc       packages/scalyc/0.1.0/scalyc.scaly & pids+=($!)
    lsp_root scaly        packages/scaly/0.1.0/scaly.scaly & pids+=($!)
    lsp_root scalyls      packages/scalyls/0.1.0/scalyls.scaly & pids+=($!)
    lsp_root json         packages/json/0.1.0/json.scaly & pids+=($!)
    lsp_root scalyls_main packages/scalyls/0.1.0/main.scaly & pids+=($!)
    for p in "${pids[@]}"; do wait "$p" || exit 1; done
    # scaly.o references the fiber context-switch primitives (vendored asm)
    # and the evented-I/O backend shim (kqueue/epoll C); ctime.o is the
    # civil-time shim every scalyc-family link carries (scaly/time/ctime.c).
    tools/fcontext.sh "$LSO/fcontext.o" || { echo "FAIL  fcontext assembly"; exit 1; }
    tools/eio.sh "$LSO/eio.o" || { echo "FAIL  eio shim compile"; exit 1; }
    tools/ctime.sh "$LSO/ctime.o" || { echo "FAIL  ctime shim compile"; exit 1; }
    tools/panic.sh "$LSO/panic.o" || { echo "FAIL  panic shim compile"; exit 1; }
fi

# Build a scalyls CONSUMER program (json_test / echo): $1=src $2=out-binary.
lsp_build_prog() {
    if [ "$MODE" = selfhosted ]; then
        ( ulimit -s 65520; "$SCALYC" -S --no-tests -o "$LSO/prog.ll" "$1" ) || return 1
        "$LLC" -relocation-model=pic -filetype=obj "$LSO/prog.ll" -o "$LSO/prog.o" || return 1
        clang "$LSO/prog.o" "$LSO/scalyls.o" "$LSO/json.o" "$LSO/scalyc.o" "$LSO/scaly.o" "$LSO/fcontext.o" "$LSO/eio.o" "$LSO/ctime.o" "$LSO/panic.o" "${LINK[@]}" -o "$2" 2>/dev/null
    else
        "$SCALYC" -o "$2" "$1" "${LINK[@]}" 2>/dev/null
    fi
}

# Build the scalyls SERVER (main.scaly): $1=out-binary.
lsp_build_server() {
    if [ -n "${SCALYLS_PREBUILT:-}" ]; then
        [ -x "$SCALYLS_PREBUILT" ] || return 1
        cp "$SCALYLS_PREBUILT" "$1"
    elif [ "$MODE" = selfhosted ]; then
        clang "$LSO/scalyls_main.o" "$LSO/scalyls.o" "$LSO/json.o" "$LSO/scalyc.o" "$LSO/scaly.o" "$LSO/fcontext.o" "$LSO/eio.o" "$LSO/ctime.o" "$LSO/panic.o" "${LINK[@]}" -o "$1" 2>/dev/null
    else
        "$SCALYC" -o "$1" packages/scalyls/0.1.0/main.scaly "${LINK[@]}" 2>/dev/null
    fi
}

# ---- json + rpc unit test -------------------------------------------------
lsp_build_prog tests/lsp/json_test.scaly /tmp/scalyls_json_test
if [ "$(/tmp/scalyls_json_test)" = "PASS" ]; then ok "json_test"; else bad "json_test"; fi

# ---- transport echo round-trip -------------------------------------------
lsp_build_prog tests/lsp/echo.scaly /tmp/scalyls_echo
echoed=$(printf 'Content-Length: 5\r\n\r\nhello' | /tmp/scalyls_echo)
if [ "$echoed" = $'Content-Length: 5\r\n\r\nhello' ]; then ok "echo transport"; else bad "echo transport"; fi

# ---- LSP server: lifecycle + diagnostics ----------------------------------
lsp_build_server /tmp/scalyls

# Driven INTERACTIVELY (one message, then read its answer) because that is what
# an editor does — and since server.process_one# defers an owed analysis while
# more client input is pending, a batch of all six messages would legitimately
# coalesce the didOpen and didChange analyses away and this test would be
# asserting the batch behaviour instead of the editor one. See the
# typing-burst group below, which asserts exactly that coalescing.
python3 - "$@" <<'PY'
import sys, json, subprocess, os, select, time
def frame(o):
    b = json.dumps(o).encode()
    return ("Content-Length: %d\r\n\r\n" % len(b)).encode() + b

p = subprocess.Popen(["/tmp/scalyls"], stdin=subprocess.PIPE, stdout=subprocess.PIPE, bufsize=0)
fd = p.stdout.fileno()
buf = bytearray()
def next_frame(timeout=20.0):
    deadline = time.time() + timeout
    while True:
        i = buf.find(b"\r\n\r\n")
        if i >= 0:
            n = int(bytes(buf[:i]).decode().split(":")[1].strip())
            if len(buf) >= i + 4 + n:
                body = bytes(buf[i+4:i+4+n]); del buf[:i+4+n]
                return json.loads(body)
        left = deadline - time.time()
        if left <= 0: return None
        if not select.select([fd], [], [], left)[0]: return None
        chunk = os.read(fd, 65536)
        if not chunk: return None
        buf.extend(chunk)
def send(o): p.stdin.write(frame(o))

import shutil as _sh; _ws = "/tmp/lsp_ws/basic"; _sh.rmtree(_ws, ignore_errors=True); os.makedirs(_ws)
_uri = "file://" + _ws + "/ok.scaly"

frames = []
send({"jsonrpc":"2.0","id":1,"method":"initialize","params":{}})
frames.append(next_frame())
send({"jsonrpc":"2.0","method":"initialized","params":{}})
send({"jsonrpc":"2.0","method":"textDocument/didOpen","params":{"textDocument":{
        "uri":_uri,"languageId":"scaly","version":1,
        "text":"function answer() returns int\n{\n    return 42\n}\n"}}})
frames.append(next_frame())
send({"jsonrpc":"2.0","method":"textDocument/didChange","params":{
        "textDocument":{"uri":_uri,"version":2},
        "contentChanges":[{"text":"function f() returns int\n{\n    return nope()\n}\n"}]}})
frames.append(next_frame())
send({"jsonrpc":"2.0","method":"textDocument/didClose","params":{
        "textDocument":{"uri":_uri}}})
frames.append(next_frame())
send({"jsonrpc":"2.0","id":2,"method":"shutdown"})
frames.append(next_frame())
send({"jsonrpc":"2.0","method":"exit"})

failures = 0
def check(cond, label):
    global failures
    print(("PASS  " if cond else "FAIL  ") + label)
    if not cond: failures += 1

check(all(f is not None for f in frames) and len(frames) == 5,
      "frame count == 5 (init, didOpen diag, didChange diag, didClose clear, shutdown)")
check(frames[0].get("id") == 1 and "capabilities" in frames[0].get("result", {}), "initialize -> capabilities")
check(frames[0]["result"]["capabilities"].get("textDocumentSync") == 2, "textDocumentSync == 2 (incremental)")
check(frames[1].get("method") == "textDocument/publishDiagnostics", "didOpen -> publishDiagnostics")
check(frames[1]["params"]["diagnostics"] == [], "valid doc -> no diagnostics")
diags = frames[2]["params"]["diagnostics"]
check(len(diags) == 1, "didChange (bad call) -> 1 diagnostic")
check("nope" in diags[0]["message"], "diagnostic message names the missing fn")
check(diags[0]["range"]["start"]["line"] == 2, "diagnostic on 0-based line 2")
check(frames[3].get("method") == "textDocument/publishDiagnostics", "didClose -> publishDiagnostics")
check(frames[3]["params"]["uri"] == _uri, "didClose clears the right uri")
check(frames[3]["params"]["diagnostics"] == [], "didClose -> empty diagnostics (cleared)")
check(frames[4].get("id") == 2 and frames[4].get("result") is None, "shutdown -> null result")
sys.exit(1 if failures else 0)
PY
rc=$?
if [ $rc -eq 0 ]; then ok "lsp server lifecycle+diagnostics"; else bad "lsp server lifecycle+diagnostics"; fi

# ---- crash isolation: kill the worker mid-session, server must recover ----
python3 - <<'PY'
import sys, json, subprocess, time, os
def frame(o):
    b=json.dumps(o).encode(); return ("Content-Length: %d\r\n\r\n"%len(b)).encode()+b
def readframe(f):
    hdr=b""
    while b"\r\n\r\n" not in hdr:
        c=f.read(1)
        if not c: return None
        hdr+=c
    n=int(hdr.decode().split(":")[1].strip()); return json.loads(f.read(n))
bad="function f() returns int\n{\n    return nope()\n}\n"
p=subprocess.Popen(["/tmp/scalyls"],stdin=subprocess.PIPE,stdout=subprocess.PIPE)
def send(o): p.stdin.write(frame(o)); p.stdin.flush()
try:
    send({"jsonrpc":"2.0","id":1,"method":"initialize","params":{}}); readframe(p.stdout)
    send({"jsonrpc":"2.0","method":"textDocument/didOpen","params":{"textDocument":{
        "uri":"file:///t.scaly","languageId":"scaly","version":1,"text":bad}}})
    readframe(p.stdout)                      # diag (worker now alive)
    time.sleep(0.2)
    kids=subprocess.run(["pgrep","-P",str(p.pid)],stdout=subprocess.PIPE).stdout.decode().split()
    for k in kids: os.kill(int(k), 9)        # simulate a compiler crash
    time.sleep(0.2)
    send({"jsonrpc":"2.0","method":"textDocument/didChange","params":{
        "textDocument":{"uri":"file:///t.scaly","version":2},"contentChanges":[{"text":bad}]}})
    d=readframe(p.stdout)                     # must STILL be a correct answer
    send({"jsonrpc":"2.0","id":2,"method":"shutdown"}); readframe(p.stdout)
    send({"jsonrpc":"2.0","method":"exit"}); p.wait(timeout=10)
    diags=(d or {}).get("params",{}).get("diagnostics",[])
    ok = len(diags)==1 and "nope" in diags[0]["message"]
    print(("PASS  " if ok else "FAIL  ")+"worker crash -> server recovers + retries")
    sys.exit(0 if ok else 1)
except Exception as e:
    print("FAIL  worker crash isolation ("+str(e)+")"); sys.exit(1)
PY
rc=$?
if [ $rc -eq 0 ]; then ok "lsp worker crash isolation"; else bad "lsp worker crash isolation"; fi

# ---- worker budget: a give-up must be LOUD, and per request kind ----
# Exceeding the budget kills the worker and answers with the fallback — a
# well-formed empty result the client cannot tell from "nothing here". That
# silence is why an inlayHint overrun ran on every scroll unnoticed (measured
# 2026-08-12: 8.01 / 9.01 / 8.00 s, empty every time, on a file in
# packages/opensp). Two things are gated here: the give-up says so on stderr,
# naming the kind and the budget, and the client still gets valid JSON.
# SCALYLS_BUDGET_MS is what makes this testable at all — a real overrun needs a
# workspace big enough to be machine-dependent (same reason the planner has
# SCALYC_STACK_BUDGET).
python3 - <<'PY'
import sys, json, subprocess, os
# 1500 call sites, so the request costs ~140 ms and a 1 ms budget is reliably
# exceeded. A four-line document does NOT work here: it answers inside the poll
# and the give-up never fires — the first draft of this test passed on the
# BROKEN binary for exactly that reason, which is the trap a negative control
# exists to catch.
body = "".join("    f(%d)\n" % i for i in range(1500))
doc  = "function f(a: int) returns int\n{\n" + body + "    0\n}\n"
last = doc.count("\n") - 1
import shutil as _sh; _ws = "/tmp/lsp_ws/budget_test"; _sh.rmtree(_ws, ignore_errors=True); os.makedirs(_ws)
uri = "file://" + _ws + "/lsp_budget_test.scaly"
def frame(o):
    b=json.dumps(o).encode(); return ("Content-Length: %d\r\n\r\n"%len(b)).encode()+b
inp  = frame({"jsonrpc":"2.0","id":1,"method":"initialize","params":{}})
inp += frame({"jsonrpc":"2.0","method":"initialized","params":{}})
inp += frame({"jsonrpc":"2.0","method":"textDocument/didOpen","params":{"textDocument":{
        "uri":uri,"languageId":"scaly","version":1,"text":doc}}})
inp += frame({"jsonrpc":"2.0","id":2,"method":"textDocument/inlayHint","params":{
        "textDocument":{"uri":uri},
        "range":{"start":{"line":0,"character":0},"end":{"line":last,"character":0}}}})
inp += frame({"jsonrpc":"2.0","id":9,"method":"shutdown"})
inp += frame({"jsonrpc":"2.0","method":"exit"})
r = subprocess.run(["/tmp/scalyls"], input=inp, stdout=subprocess.PIPE, stderr=subprocess.PIPE,
                   env=dict(os.environ, SCALYLS_BUDGET_MS="1"))
res, d = None, r.stdout
while d:
    i = d.find(b"\r\n\r\n")
    if i < 0: break
    n = int(d[:i].decode().split(":")[1].strip())
    f = json.loads(d[i+4:i+4+n]); d = d[i+4+n:]
    if f.get("id") == 2: res = f.get("result")
err = r.stderr.decode(errors="replace")
failures = 0
def check(cond, label):
    global failures
    print(("PASS  " if cond else "FAIL  ") + label)
    if not cond: failures += 1
check("exceeded its 1 ms budget" in err, "a give-up names the budget it exceeded on stderr")
check("request 'y'" in err, "and names the request KIND that overran")
check(res == [], "the client still gets a well-formed fallback, not a broken frame")
# Unset: the same request must NOT report an overrun, and must ANSWER — which
# also proves the test document is one the server can serve at all.
r2 = subprocess.run(["/tmp/scalyls"], input=inp, stdout=subprocess.PIPE, stderr=subprocess.PIPE,
                    env={k: v for k, v in os.environ.items() if k != "SCALYLS_BUDGET_MS"})
res2, d2 = None, r2.stdout
while d2:
    i = d2.find(b"\r\n\r\n")
    if i < 0: break
    n = int(d2[:i].decode().split(":")[1].strip())
    f = json.loads(d2[i+4:i+4+n]); d2 = d2[i+4+n:]
    if f.get("id") == 2: res2 = f.get("result")
check("budget" not in r2.stderr.decode(errors="replace"),
      "without the override a normal request reports nothing")
check(res2 is not None and len(res2) == 1500,
      "and answers in full — the empty result above was the give-up, not the input")
# A non-numeric override must be IGNORED, never read as 0 (that would disable
# the hang detector outright).
r3 = subprocess.run(["/tmp/scalyls"], input=inp, stdout=subprocess.PIPE, stderr=subprocess.PIPE,
                    env=dict(os.environ, SCALYLS_BUDGET_MS="soon"))
check("budget" not in r3.stderr.decode(errors="replace"),
      "a non-numeric SCALYLS_BUDGET_MS falls back to the table")
sys.exit(1 if failures else 0)
PY
rc=$?
if [ $rc -eq 0 ]; then ok "lsp worker budget"; else bad "lsp worker budget"; fi

# ---- documentSymbol: outline tree for a file on disk ----
# The request carries only a uri, so the worker re-reads the file; write a
# known one to disk first. Exercises functions, a module-level mutable, a
# struct with methods (children), and a union with variant children.
python3 - <<'PY'
import sys, json, subprocess
src = ("function add(a: int, b: int) returns int\n{\n    return a + b\n}\n\n"
       "mutable counter: int 0\n\n"
       "define Point\n(\n    x: int\n)\n{\n"
       "    function get_x(this: Point) returns int\n    {\n        return x\n    }\n}\n\n"
       "define Shape union (\n    Circle: int\n    Square: int\n)\n")
import os as _os, shutil as _sh; _ws = "/tmp/lsp_ws/symbols_test"; _sh.rmtree(_ws, ignore_errors=True); _os.makedirs(_ws)
path = _ws + "/lsp_symbols_test.scaly"
open(path, "w").write(src)
def frame(o):
    b=json.dumps(o).encode(); return ("Content-Length: %d\r\n\r\n"%len(b)).encode()+b
inp  = frame({"jsonrpc":"2.0","id":1,"method":"initialize","params":{}})
inp += frame({"jsonrpc":"2.0","method":"initialized","params":{}})
inp += frame({"jsonrpc":"2.0","id":2,"method":"textDocument/documentSymbol",
              "params":{"textDocument":{"uri":"file://"+path}}})
inp += frame({"jsonrpc":"2.0","id":3,"method":"shutdown"})
inp += frame({"jsonrpc":"2.0","method":"exit"})
out = subprocess.run(["/tmp/scalyls"], input=inp, stdout=subprocess.PIPE).stdout
frames, d = [], out
while d:
    i = d.find(b"\r\n\r\n")
    if i < 0: break
    n = int(d[:i].decode().split(":")[1].strip())
    frames.append(json.loads(d[i+4:i+4+n])); d = d[i+4+n:]
failures = 0
def check(cond, label):
    global failures
    print(("PASS  " if cond else "FAIL  ") + label)
    if not cond: failures += 1
caps = frames[0]["result"]["capabilities"]
check(caps.get("documentSymbolProvider") is True, "initialize advertises documentSymbolProvider")
r = next((f for f in frames if f.get("id") == 2), None)
syms = (r or {}).get("result")
check(isinstance(syms, list), "documentSymbol -> array result")
syms = syms or []
top = {s["name"]: s for s in syms}
check([s["name"] for s in syms] == ["add","counter","Point","Shape"], "top-level names + order")
check(top.get("add",{}).get("kind") == 12, "function -> Function(12)")
check(top.get("counter",{}).get("kind") == 13, "mutable -> Variable(13)")
check(top.get("Point",{}).get("kind") == 23, "define struct -> Struct(23)")
check([c["name"] for c in top.get("Point",{}).get("children",[])] == ["get_x"], "struct method child")
check(top.get("Point",{}).get("children",[{}])[0].get("kind") == 6, "method -> Method(6)")
check(top.get("Shape",{}).get("kind") == 10, "union -> Enum(10)")
check([c["name"] for c in top.get("Shape",{}).get("children",[])] == ["Circle","Square"], "union variant children")
check(top.get("Shape",{}).get("children",[{}])[0].get("kind") == 22, "variant -> EnumMember(22)")
sys.exit(1 if failures else 0)
PY
rc=$?
if [ $rc -eq 0 ]; then ok "lsp documentSymbol outline"; else bad "lsp documentSymbol outline"; fi

# ---- hover: symbol under the cursor for a file on disk ----
# Like documentSymbol, the request carries only a uri + position; the worker
# re-reads the file. Exercises a top-level function, a method inside a struct
# (innermost wins), a module-level mutable, and an empty result off any symbol.
#
# A DECLARATION HEADER answers with the signature, or with the constituent under
# the cursor (a parameter, the return type) — `method get_x` alone throws away
# everything the line says. Inside the BODY it becomes `in kind name`: there the
# semantic hover answers with a type when it can, and all its fallback owes is
# WHERE the cursor sits — the leading `in` is what keeps that from being read as
# an answer about the token under the cursor (symbols.enclosing_text#).
# Both halves are asserted (ids 2/3/7/8 header, 9 body).
python3 - <<'PY'
import sys, json, subprocess
src = ("function add(a: int, b: int) returns int\n{\n    return a + b\n}\n\n"
       "mutable counter: int 0\n\n"
       "define Point\n(\n    x: int\n)\n{\n"
       "    function get_x(this: Point) returns int\n    {\n        return x\n    }\n}\n"
       # Line 18: a GENERIC routine — nothing in the tree declares one, so this
       # fixture is the only cover for the `[T]` half of the rendering.
       "\nfunction pick[T](a: T, b: T) returns T\n{\n    return a\n}\n"
       # Line 28: a DEINIT, appended at the END so no line above it moves. It
       # is the one member whose hover has no name to add, so it is the only
       # cover for hover_deinit# — header `deinit`, body `in deinit`.
       "\ndefine Res\n(\n    n: int\n)\n{\n    deinit\n    {\n        let z 1\n    }\n}\n")
import os as _os, shutil as _sh; _ws = "/tmp/lsp_ws/hover_test"; _sh.rmtree(_ws, ignore_errors=True); _os.makedirs(_ws)
path = _ws + "/lsp_hover_test.scaly"
open(path, "w").write(src)
def frame(o):
    b=json.dumps(o).encode(); return ("Content-Length: %d\r\n\r\n"%len(b)).encode()+b
def hov(idn, line, char):
    return frame({"jsonrpc":"2.0","id":idn,"method":"textDocument/hover",
                  "params":{"textDocument":{"uri":"file://"+path},
                            "position":{"line":line,"character":char}}})
inp  = frame({"jsonrpc":"2.0","id":1,"method":"initialize","params":{}})
inp += frame({"jsonrpc":"2.0","method":"initialized","params":{}})
inp += hov(2, 0, 10)     # the name `add`
inp += hov(3, 12, 13)    # the name Point.get_x (innermost declaration wins)
inp += hov(4, 5, 10)     # inside `counter`
inp += hov(5, 100, 0)    # past EOF -> no symbol (declaration ranges run to
                         # the next token, so blank lines between decls still
                         # report the preceding one; past-EOF is the reliable
                         # null case)
inp += hov(7, 0, 13)     # a PARAMETER of `add`
inp += hov(8, 0, 30)     # the RETURN TYPE of `add`
inp += hov(9, 13, 4)     # get_x's `{` -> body, so the bare `method get_x`
inp += hov(10, 18, 10)   # the name `pick` -> a generic signature
inp += hov(11, 18, 14)   # its generic parameter `T`
inp += hov(12, 28, 4)    # the `deinit` KEYWORD -> the header answer
inp += hov(13, 29, 4)    # its body brace -> the `in` form
inp += frame({"jsonrpc":"2.0","id":6,"method":"shutdown"})
inp += frame({"jsonrpc":"2.0","method":"exit"})
out = subprocess.run(["/tmp/scalyls"], input=inp, stdout=subprocess.PIPE).stdout
frames, d = [], out
while d:
    i = d.find(b"\r\n\r\n")
    if i < 0: break
    n = int(d[:i].decode().split(":")[1].strip())
    frames.append(json.loads(d[i+4:i+4+n])); d = d[i+4+n:]
failures = 0
def check(cond, label):
    global failures
    print(("PASS  " if cond else "FAIL  ") + label)
    if not cond: failures += 1
caps = frames[0]["result"]["capabilities"]
check(caps.get("hoverProvider") is True, "initialize advertises hoverProvider")
def val(idn):
    f = next((x for x in frames if x.get("id") == idn), None)
    r = (f or {}).get("result")
    if r is None: return None
    return r.get("contents", {}).get("value")
check(val(2) == "function add(a: int, b: int) returns int",
      "hover on a function name -> its signature")
check(val(3) == "method get_x(this: Point) returns int",
      "hover on a method name (innermost) -> its signature")
check(val(4) == "mutable counter", "hover on mutable -> 'mutable counter'")
check(val(5) is None, "hover off any symbol -> null result")
check(val(7) == "parameter a: int", "hover on a parameter -> 'parameter a: int'")
check(val(8) == "returns int", "hover on the return type -> 'returns int'")
check(val(9) == "in method get_x", "hover in a method BODY -> 'in method get_x'")
check(val(10) == "function pick[T](a: T, b: T) returns T",
      "hover on a generic routine -> signature with its generics")
check(val(11) == "generic T", "hover on a generic parameter -> 'generic T'")
check(val(12) == "deinit", "hover on `deinit` -> 'deinit'")
check(val(13) == "in deinit", "hover in a deinit BODY -> 'in deinit'")
sys.exit(1 if failures else 0)
PY
rc=$?
if [ $rc -eq 0 ]; then ok "lsp hover"; else bad "lsp hover"; fi

# ---- hover: the DOC COMMENT above the declaration ------------------------
# In this tree the `;` block above a routine IS its documentation — it carries
# the reasoning the signature cannot. symbols.with_doc# appends it, fenced so
# that markdown does not reflow hand-wrapped structure into one paragraph.
#
# The interesting checks are the ones about where the block STOPS:
#
#   BLANK LINE   a file banner or an unrelated note above the doc is separated
#                by an empty line in this tree, and a walk that crossed one
#                would hand the reader the section header instead of the doc.
#   OWN LINE     a declaration that is not the first thing on its line does not
#                own the block above it. `a: b` on one line puts two routines
#                there, and the doc belongs to the first — which is what the
#                skip_blanks# test in doc_comment# decides, and the only reason
#                it is not dead code.
#   `;*` BLOCK   a block comment is not a doc line: its content runs past this
#                line, so a line-wise walk could report commented-out SOURCE as
#                documentation.
#
# And where it is NOT attached: a constituent (`parameter a: int`) is a
# statement about the parameter, and the in-body fallback about where the cursor
# is — repeating the whole block there would bury the semantic type hover.
python3 - <<'PY'
import sys, json, subprocess, os, shutil
ws = "/tmp/lsp_ws/hover_doc"
shutil.rmtree(ws, ignore_errors=True); os.makedirs(ws)
src = ("; A file banner that must NOT reach any hover.\n"        # 0
       "\n"                                                     # 1  blank ends it
       "; Add two numbers.\n"                                   # 2
       ";\n"                                                    # 3  paragraph break
       ";   a  the left one\n"                                  # 4  structure
       ";   b  the right one\n"                                 # 5
       "function add(a: int, b: int) returns int\n"             # 6
       "{\n    return a + b\n}\n"                               # 7-9
       "\n"                                                     # 10
       "; The live count.\n"                                    # 11
       "mutable counter: int 0\n"                               # 12
       "\n"                                                     # 13
       "; A point in the plane.\n"                              # 14
       "define Point\n(\n    x: int\n)\n{\n"                    # 15-19
       "    ; The x coordinate, read-only.\n"                   # 20
       "    function get_x(this: Point) returns int\n"          # 21
       "    {\n        return x\n    }\n}\n"                    # 22-25
       "\n"                                                     # 26
       "function undocumented() returns int\n"                  # 27
       "{\n    return 1\n}\n"                                   # 28-30
       "\n"                                                     # 31
       ";* a BLOCK comment, whose content runs past this line *;\n"  # 32
       "function blocky() returns int\n"                        # 33
       "{\n    return 2\n}\n"                                   # 34-36
       "\n"                                                     # 37
       "; the doc of first\n"                                   # 38
       "function first() returns int  1: function second() returns int  2\n")  # 39
path = ws + "/lsp_hover_doc.scaly"
open(path, "w").write(src)

def frame(o):
    b=json.dumps(o).encode(); return ("Content-Length: %d\r\n\r\n"%len(b)).encode()+b
def hov(idn, line, char):
    return frame({"jsonrpc":"2.0","id":idn,"method":"textDocument/hover",
                  "params":{"textDocument":{"uri":"file://"+path},
                            "position":{"line":line,"character":char}}})
inp  = frame({"jsonrpc":"2.0","id":1,"method":"initialize","params":{}})
inp += frame({"jsonrpc":"2.0","method":"initialized","params":{}})
inp += hov(2,  6, 10)   # the name `add`      -> signature + doc
inp += hov(3,  6, 15)   # a PARAMETER of add  -> no doc
inp += hov(4,  8, 10)   # inside add's BODY   -> no doc
inp += hov(5, 12, 10)   # `mutable counter`
inp += hov(6, 15, 8)    # `define Point`
inp += hov(7, 21, 15)   # the method name
inp += hov(8, 27, 10)   # an UNDOCUMENTED routine -> byte-identical to before
inp += hov(9, 33, 10)   # a routine under a `;*` block comment
inp += hov(10, 39, 10)  # `first`  -> owns the block above
inp += hov(11, 39, 43)  # `second` -> same line, does NOT
inp += frame({"jsonrpc":"2.0","id":99,"method":"shutdown"})
inp += frame({"jsonrpc":"2.0","method":"exit"})
out = subprocess.run(["/tmp/scalyls"], input=inp, stdout=subprocess.PIPE).stdout
frames, d = [], out
while d:
    i = d.find(b"\r\n\r\n")
    if i < 0: break
    n = int(d[:i].decode().split(":")[1].strip())
    frames.append(json.loads(d[i+4:i+4+n])); d = d[i+4+n:]

failures = 0
def check(cond, label):
    global failures
    print(("PASS  " if cond else "FAIL  ") + label)
    if not cond: failures += 1
def val(idn):
    f = next((x for x in frames if x.get("id") == idn), None)
    r = (f or {}).get("result")
    return None if r is None else r.get("contents", {}).get("value")

FENCE = "`" * 3
add = val(2) or ""
check(add.startswith("function add(a: int, b: int) returns int\n\n" + FENCE + "\n"),
      "the signature comes first, then the fenced doc block")
check("Add two numbers." in add, "the doc text is there")
check("\n\n  a  the left one\n  b  the right one\n" in add,
      "the block's own line structure survives (paragraph break + indent)")
check("file banner" not in add, "a block above a BLANK LINE is not part of it")
check(add.endswith("\n" + FENCE), "the fence is closed")

check(val(3) == "parameter a: int", "a constituent gets no doc")
check(val(4) == "in function add", "an offset inside the BODY gets no doc")
check((val(5) or "").startswith("mutable counter\n\n") and "The live count." in (val(5) or ""),
      "a mutable global carries its doc")
check((val(6) or "").startswith("struct Point\n\n") and "A point in the plane." in (val(6) or ""),
      "a concept carries its doc")
check("The x coordinate, read-only." in (val(7) or ""), "a method carries its doc")
check(val(8) == "function undocumented() returns int",
      "an undocumented routine is unchanged -- no fence, no blank line")
check(val(9) == "function blocky() returns int",
      "a `;*` BLOCK comment is not a doc comment")
check("the doc of first" in (val(10) or ""), "the routine that STARTS the line owns the block")
check(val(11) == "function second() returns int",
      "a second routine on the same line does not")
sys.exit(1 if failures else 0)
PY
rc=$?
if [ $rc -eq 0 ]; then ok "lsp hover doc comments"; else bad "lsp hover doc comments"; fi

# ---- definition: jump to the declaration named under the cursor ----
# Intra-file go-to-definition: extract the identifier at the cursor and find
# a declaration with that name. Exercises a top-level function call, a method
# (innermost in a struct body), a type reference, a union variant, and the
# no-match / off-symbol null cases.
python3 - <<'PY'
import sys, json, subprocess
src = ("function add(a: int, b: int) returns int\n{\n    return a + b\n}\n\n"
       "define Point\n(\n    x: int\n)\n{\n"
       "    function get_x(this: Point) returns int\n    {\n        return x\n    }\n}\n\n"
       "define Shape union (\n    Circle: int\n    Square: int\n)\n\n"
       "function use_it(p: Point) returns int\n{\n    return add(p.get_x(), 1)\n}\n")
import os as _os, shutil as _sh; _ws = "/tmp/lsp_ws/def_test"; _sh.rmtree(_ws, ignore_errors=True); _os.makedirs(_ws)
path = _ws + "/lsp_def_test.scaly"
open(path, "w").write(src)
def frame(o):
    b=json.dumps(o).encode(); return ("Content-Length: %d\r\n\r\n"%len(b)).encode()+b
def df(idn, line, char):
    return frame({"jsonrpc":"2.0","id":idn,"method":"textDocument/definition",
                  "params":{"textDocument":{"uri":"file://"+path},
                            "position":{"line":line,"character":char}}})
inp  = frame({"jsonrpc":"2.0","id":1,"method":"initialize","params":{}})
inp += frame({"jsonrpc":"2.0","method":"initialized","params":{}})
inp += df(2, 23, 12)    # `add` in `return add(...)` -> top-level add at line 0
inp += df(3, 23, 18)    # `get_x` in `p.get_x()`     -> method at line 10
inp += df(4, 21, 20)    # `Point` in `p: Point`      -> define Point at line 5
inp += df(5, 1, 1)      # `(` punctuation            -> null
inp += df(6, 100, 0)    # past EOF                    -> null
inp += frame({"jsonrpc":"2.0","id":9,"method":"shutdown"})
inp += frame({"jsonrpc":"2.0","method":"exit"})
out = subprocess.run(["/tmp/scalyls"], input=inp, stdout=subprocess.PIPE).stdout
frames, d = [], out
while d:
    i = d.find(b"\r\n\r\n")
    if i < 0: break
    n = int(d[:i].decode().split(":")[1].strip())
    frames.append(json.loads(d[i+4:i+4+n])); d = d[i+4+n:]
failures = 0
def check(cond, label):
    global failures
    print(("PASS  " if cond else "FAIL  ") + label)
    if not cond: failures += 1
caps = frames[0]["result"]["capabilities"]
check(caps.get("definitionProvider") is True, "initialize advertises definitionProvider")
def loc(idn):
    f = next((x for x in frames if x.get("id") == idn), None)
    return (f or {}).get("result")
def line_of(idn):
    r = loc(idn)
    return None if r is None else r["range"]["start"]["line"]
check(loc(2) is not None and loc(2)["uri"] == "file://"+path, "definition -> Location with the file uri")
check(line_of(2) == 0, "call site -> top-level function decl (line 0)")
check(line_of(3) == 10, "method call -> method decl (line 10)")
check(line_of(4) == 5, "type reference -> define decl (line 5)")
check(loc(5) is None, "punctuation under cursor -> null")
check(loc(6) is None, "past EOF -> null")
sys.exit(1 if failures else 0)
PY
rc=$?
if [ $rc -eq 0 ]; then ok "lsp definition"; else bad "lsp definition"; fi

# ---- references: all whole-token occurrences of the identifier ----
# Lexical Find-All-References: extract the identifier at the cursor and return
# a Location per whole-token occurrence. Exercises a multiply-called function,
# whole-token matching (the param `a` must NOT match inside `add`), and the
# no-identifier `[]` case.
#
# ★Every fixture block in this file writes into its OWN directory under
# /tmp/lsp_ws/, and for this test that is load-bearing. `references` is a
# WORKSPACE-WIDE query — scalyls searches the directory of the document, not
# just the document — so with all fixtures in a shared /tmp it answered 7
# instead of 3: hover, definition, symbols and rename each declare a
# `function add` of their own, and it found them all, correctly. Rename asserts
# the same "3 occurrences" and was NOT affected, because its TextEdits are
# scoped to one document.
#
# Measured 2026-08-14, the first time the bar ran this suite on Linux. ★The
# second lesson is why the isolation is applied to ALL blocks rather than to
# this one: the suite ABORTS at the first failing section, so the two failures
# here hid the other 44 tests entirely — and the very next one to run, the
# scope test's module-wide `alpha`, was the same defect again (the incremental
# test's fixture declares an `alpha` too). A shared scratch directory is not a
# tidiness question; it is a silent coupling between tests that only the
# workspace-wide assertions can feel.
python3 - <<'PY'
import sys, json, subprocess
src = ("function add(a: int, b: int) returns int\n{\n    return a + b\n}\n\n"
       "function main() returns int\n{\n    return add(add(1, 2), 3)\n}\n")
import os as _os, shutil as _sh; _ws = "/tmp/lsp_ws/ref_test"; _sh.rmtree(_ws, ignore_errors=True); _os.makedirs(_ws)
path = _ws + "/lsp_ref_test.scaly"
open(path, "w").write(src)
def frame(o):
    b=json.dumps(o).encode(); return ("Content-Length: %d\r\n\r\n"%len(b)).encode()+b
def rf(idn, line, char):
    return frame({"jsonrpc":"2.0","id":idn,"method":"textDocument/references",
                  "params":{"textDocument":{"uri":"file://"+path},
                            "position":{"line":line,"character":char},
                            "context":{"includeDeclaration":True}}})
inp  = frame({"jsonrpc":"2.0","id":1,"method":"initialize","params":{}})
inp += frame({"jsonrpc":"2.0","method":"initialized","params":{}})
inp += rf(2, 0, 10)    # `add` -> 3 occurrences (decl + the two nested calls)
inp += rf(3, 2, 11)    # `a` param -> 2 (decl + the use; NOT the `a` in `add`)
inp += rf(4, 1, 0)     # `{` -> []
inp += frame({"jsonrpc":"2.0","id":9,"method":"shutdown"})
inp += frame({"jsonrpc":"2.0","method":"exit"})
out = subprocess.run(["/tmp/scalyls"], input=inp, stdout=subprocess.PIPE).stdout
frames, d = [], out
while d:
    i = d.find(b"\r\n\r\n")
    if i < 0: break
    n = int(d[:i].decode().split(":")[1].strip())
    frames.append(json.loads(d[i+4:i+4+n])); d = d[i+4+n:]
failures = 0
def check(cond, label):
    global failures
    print(("PASS  " if cond else "FAIL  ") + label)
    if not cond: failures += 1
caps = frames[0]["result"]["capabilities"]
check(caps.get("referencesProvider") is True, "initialize advertises referencesProvider")
def res(idn):
    f = next((x for x in frames if x.get("id") == idn), None)
    return (f or {}).get("result")
def spans(idn):
    return [(x["uri"], x["range"]["start"]["line"], x["range"]["start"]["character"],
             x["range"]["end"]["character"]) for x in (res(idn) or [])]
add_spans = spans(2)
check(len(add_spans) == 3, "`add` -> 3 occurrences (decl + 2 calls)")
check(all(u == "file://"+path for (u,_,_,_) in add_spans), "each occurrence carries the file uri")
check((0,9,12) in [(l,s,e) for (_,l,s,e) in add_spans], "decl occurrence range covers the whole token")
a_spans = spans(3)
check(len(a_spans) == 2, "`a` -> 2 (whole-token: not the `a` inside `add`)")
check(res(4) == [], "no identifier under cursor -> []")
sys.exit(1 if failures else 0)
PY
rc=$?
if [ $rc -eq 0 ]; then ok "lsp references"; else bad "lsp references"; fi

# ---- references/rename: what the scan must and must not see ----
# The scan behind references and rename is lexical, so its whole value is in
# what it EXCLUDES. Four properties, each of which was once wrong:
#   * a `set NAME:` assignment target is a WRITE, not a binding — reading it as
#     one made the whole routine count as shadowing NAME, and every write site
#     of a `mutable` global silently vanished from references AND rename (F2
#     renamed the declaration and left the assignments behind = broken code)
#   * `for NAME in ...` DOES bind NAME, so an unrelated loop variable must not
#     be renamed along with a module-wide name
#   * `let NAME` shadows (Step 38, already covered above — pinned here too so
#     the two directions are read together)
#   * comments and string literals are never occurrences
# ★Its own directory under /tmp/lsp_ws/, and here that is load-bearing for the
# same reason it is in the references block above: `references` is answered
# from the document's DIRECTORY tree, and every assertion below counts EXACT
# occurrences (`[1, 5, 5, 6]`, "exactly the 4 real occurrences"), so one
# sibling naming `counter` turns a correct answer into a red test — measured:
# the same fixture with a `counter`-declaring sibling answers 8 locations
# across 2 files.
# ★★★It was missed when the other seventeen were isolated on 2026-08-14, and
# the reason it nevertheless stayed green on macOS turned out to be a DEFECT
# rather than luck: /tmp is a symlink to private/tmp, `find` does not follow a
# symlinked STARTING POINT, and scalyls' walk therefore listed nothing at all
# for any document sitting directly in /tmp. So this fixture was not immune,
# it was answered by a broken walk — and so was every other /tmp fixture on
# this host, which is why the Linux run was the first to see the coupling.
# Fixed with `find -H` (symbols.list_scaly_files); the gate for THAT is "lsp
# symlinked workspace root" below.
python3 - <<'PY'
import sys, json, subprocess
src = ("; counter is mentioned in this comment\n"          # 0
       "mutable counter: int 0\n"                          # 1
       "\n"
       "function bump() returns int\n"                     # 3
       "{\n"
       "    set counter: counter + 1\n"                    # 5
       "    counter\n"                                     # 6
       "}\n"
       "\n"
       "function noise() returns int\n"                    # 9
       "{\n"
       "    let s \"counter in a string\"\n"               # 11
       "    var total: int 0\n"
       "    for counter in 3\n"                            # 13
       "        set total: total + counter\n"              # 14
       "    total\n"
       "}\n"
       "\n"
       "function shadowed() returns int\n"                 # 18
       "{\n"
       "    let counter 7\n"                               # 20
       "    counter\n"                                     # 21
       "}\n")
import os as _os, shutil as _sh; _ws = "/tmp/lsp_ws/scan_test"; _sh.rmtree(_ws, ignore_errors=True); _os.makedirs(_ws)
path = _ws + "/lsp_scan_test.scaly"
open(path, "w").write(src)
def frame(o):
    b=json.dumps(o).encode(); return ("Content-Length: %d\r\n\r\n"%len(b)).encode()+b
uri = "file://"+path
inp  = frame({"jsonrpc":"2.0","id":1,"method":"initialize","params":{}})
inp += frame({"jsonrpc":"2.0","method":"initialized","params":{}})
inp += frame({"jsonrpc":"2.0","method":"textDocument/didOpen","params":{"textDocument":{
        "uri":uri,"languageId":"scaly","version":1,"text":src}}})
inp += frame({"jsonrpc":"2.0","id":2,"method":"textDocument/references","params":{
        "textDocument":{"uri":uri},"position":{"line":1,"character":8},
        "context":{"includeDeclaration":True}}})
inp += frame({"jsonrpc":"2.0","id":3,"method":"textDocument/rename","params":{
        "textDocument":{"uri":uri},"position":{"line":1,"character":8},"newName":"tally"}})
inp += frame({"jsonrpc":"2.0","id":9,"method":"shutdown"})
inp += frame({"jsonrpc":"2.0","method":"exit"})
out = subprocess.run(["/tmp/scalyls"], input=inp, stdout=subprocess.PIPE).stdout
frames, d = [], out
while d:
    i = d.find(b"\r\n\r\n")
    if i < 0: break
    n = int(d[:i].decode().split(":")[1].strip())
    frames.append(json.loads(d[i+4:i+4+n])); d = d[i+4+n:]
failures = 0
def check(cond, label):
    global failures
    print(("PASS  " if cond else "FAIL  ") + label)
    if not cond: failures += 1
def res(idn):
    f = next((x for x in frames if x.get("id") == idn), None)
    return None if f is None else f.get("result")
lines = sorted(l["range"]["start"]["line"] for l in (res(2) or []))
check(lines == [1, 5, 5, 6], "references -> decl + both `set` occurrences + the read")
check(0 not in lines, "the comment mentioning `counter` is not an occurrence")
check(11 not in lines, "`counter` inside a string literal is not an occurrence")
check(13 not in lines and 14 not in lines, "`for counter in` binds: the loop var is excluded")
check(20 not in lines and 21 not in lines, "`let counter` shadows: that routine is excluded")
edits = (res(3) or {}).get("changes", {}).get(uri, [])
check(len(edits) == 4, "rename edits exactly the 4 real occurrences")
check(all(e["newText"] == "tally" for e in edits), "each edit carries the new name")
sys.exit(1 if failures else 0)
PY
rc=$?
if [ $rc -eq 0 ]; then ok "lsp reference scan boundaries"; else bad "lsp reference scan boundaries"; fi

# ---- the walk root reached through a SYMLINK ----
# ★scalyls lists a directory with `find`, and `find` does not follow a symlink
# that is its own starting point — so before `-H` (symbols.list_scaly_files,
# 2026-08-14) a workspace behind a symlink answered every cross-file request as
# if it held ONE file: references and rename lost every other file,
# workspace/symbol went empty, the definition fan-out never left the buffer.
# Silent in all of them, because a shorter list is a well-formed result.
#
# The fixture is the defect in miniature: two files in `real/`, opened through
# `link/`. The assertion is that the SIBLING is found — which is exactly what
# fails without `-H`, and the reason this test exists rather than a comment.
# ★It is not an exotic setup: /tmp is a symlink on macOS, which is why every
# fixture in this file was walking a directory that listed nothing there.
python3 - <<'PY'
import sys, json, subprocess, os, shutil
ws = "/tmp/lsp_ws/symlink_test"
shutil.rmtree(ws, ignore_errors=True); os.makedirs(ws + "/real")
main = ("mutable counter: int 0\n"                      # 0
        "\n"
        "function bump() returns int\n"
        "{\n"
        "    set counter: counter + 1\n"                # 4
        "}\n")
sibling = ("function reader() returns int\n"
           "{\n"
           "    counter\n"                              # 2
           "}\n")
open(ws + "/real/main.scaly", "w").write(main)
open(ws + "/real/sibling.scaly", "w").write(sibling)
os.symlink("real", ws + "/link")
path = ws + "/link/main.scaly"                          # the path the editor sees
uri  = "file://" + path
def frame(o):
    b=json.dumps(o).encode(); return ("Content-Length: %d\r\n\r\n"%len(b)).encode()+b
inp  = frame({"jsonrpc":"2.0","id":1,"method":"initialize","params":{}})
inp += frame({"jsonrpc":"2.0","method":"initialized","params":{}})
inp += frame({"jsonrpc":"2.0","method":"textDocument/didOpen","params":{"textDocument":{
        "uri":uri,"languageId":"scaly","version":1,"text":main}}})
inp += frame({"jsonrpc":"2.0","id":2,"method":"textDocument/references","params":{
        "textDocument":{"uri":uri},"position":{"line":0,"character":8},
        "context":{"includeDeclaration":True}}})
inp += frame({"jsonrpc":"2.0","id":9,"method":"shutdown"})
inp += frame({"jsonrpc":"2.0","method":"exit"})
out = subprocess.run(["/tmp/scalyls"], input=inp, stdout=subprocess.PIPE).stdout
frames, d = [], out
while d:
    i = d.find(b"\r\n\r\n")
    if i < 0: break
    n = int(d[:i].decode().split(":")[1].strip())
    frames.append(json.loads(d[i+4:i+4+n])); d = d[i+4+n:]
failures = 0
def check(cond, label):
    global failures
    print(("PASS  " if cond else "FAIL  ") + label)
    if not cond: failures += 1
locs = next((x for x in frames if x.get("id") == 2), {}).get("result") or []
files = sorted(set(os.path.basename(l["uri"]) for l in locs))
check("sibling.scaly" in files,
      "the sibling behind the symlinked root is walked (find -H)")
check(files == ["main.scaly", "sibling.scaly"],
      "both files and no others: " + ",".join(files))
check(len(locs) == 4, "3 occurrences in the open buffer + 1 in the sibling (got %d)" % len(locs))
sys.exit(1 if failures else 0)
PY
rc=$?
if [ $rc -eq 0 ]; then ok "lsp symlinked workspace root"; else bad "lsp symlinked workspace root"; fi

# ---- every REQUEST is answered, every NOTIFICATION is not ----
# ★★★An unimplemented request used to get SILENCE (server.scaly's old tail
# comment even said the client tolerates it). It does not: Claude Code's LSP
# client maps its goToImplementation operation onto textDocument/implementation
# UNCONDITIONALLY — it never reads implementationProvider back out of our
# initialize result — and its transport has no request timeout, only a
# ContentModified retry. So silence does not degrade the feature, it hangs the
# caller forever: measured 2026-08-15 at 94 s in a real session before it was
# killed by hand, and at the protocol level five unimplemented methods in a row
# each ran the full 25 s probe budget with no answer.
#
# The two halves must be tested TOGETHER, because the obvious fix breaks the
# second: answering "everything left over" would also answer notifications, and
# a response to a message that carries no id is a stray frame that shifts every
# later id the client is waiting on. Hence the notification half is not padding
# — it is the half that constrains the fix.
python3 - <<'PY'
import sys, json, subprocess, os, shutil
ws = "/tmp/lsp_ws/unhandled_request"
shutil.rmtree(ws, ignore_errors=True); os.makedirs(ws)
src = ("function target() returns int  7\n"
       "function caller() returns int  target()\n")
path = ws + "/main.scaly"; uri = "file://" + path
open(path, "w").write(src)
def frame(o):
    b=json.dumps(o).encode(); return ("Content-Length: %d\r\n\r\n"%len(b)).encode()+b
pos = {"line": 1, "character": src.split("\n")[1].index("target()")}
inp  = frame({"jsonrpc":"2.0","id":1,"method":"initialize","params":{}})
inp += frame({"jsonrpc":"2.0","method":"initialized","params":{}})
inp += frame({"jsonrpc":"2.0","method":"textDocument/didOpen","params":{"textDocument":{
        "uri":uri,"languageId":"scaly","version":1,"text":src}}})
# requests the server does not implement — each MUST come back
inp += frame({"jsonrpc":"2.0","id":2,"method":"textDocument/implementation","params":{
        "textDocument":{"uri":uri},"position":pos}})
inp += frame({"jsonrpc":"2.0","id":3,"method":"textDocument/declaration","params":{
        "textDocument":{"uri":uri},"position":pos}})
inp += frame({"jsonrpc":"2.0","id":4,"method":"textDocument/documentLink","params":{
        "textDocument":{"uri":uri}}})
inp += frame({"jsonrpc":"2.0","id":5,"method":"nosuch/method","params":{}})
# notifications the server does not implement — each MUST NOT come back
inp += frame({"jsonrpc":"2.0","method":"$/setTrace","params":{"value":"off"}})
inp += frame({"jsonrpc":"2.0","method":"textDocument/didSave","params":{
        "textDocument":{"uri":uri}}})
inp += frame({"jsonrpc":"2.0","method":"$/cancelRequest","params":{"id":999}})
# an implemented request AFTER the unknown traffic: the stream is still in sync
inp += frame({"jsonrpc":"2.0","id":6,"method":"textDocument/definition","params":{
        "textDocument":{"uri":uri},"position":pos}})
inp += frame({"jsonrpc":"2.0","id":9,"method":"shutdown"})
inp += frame({"jsonrpc":"2.0","method":"exit"})
out = subprocess.run(["/tmp/scalyls"], input=inp, stdout=subprocess.PIPE).stdout
frames, d = [], out
while d:
    i = d.find(b"\r\n\r\n")
    if i < 0: break
    n = int(d[:i].decode().split(":")[1].strip())
    frames.append(json.loads(d[i+4:i+4+n])); d = d[i+4+n:]
failures = 0
def check(cond, label):
    global failures
    print(("PASS  " if cond else "FAIL  ") + label)
    if not cond: failures += 1
answered = {f["id"]: f for f in frames if "id" in f}
for idn, name in ((2, "textDocument/implementation"), (3, "textDocument/declaration"),
                  (4, "textDocument/documentLink"), (5, "nosuch/method")):
    check(idn in answered, "unimplemented request is ANSWERED: " + name)
    check(answered.get(idn, {}).get("result", "missing") is None,
          "  ... and its result is null: " + name)
check(len(answered) == 7,
      "exactly seven responses (initialize + 4 unknown + definition + shutdown), got %d"
      % len(answered))
check(all("id" in f or f.get("method") == "textDocument/publishDiagnostics" for f in frames),
      "no stray response was produced for a notification")
check(answered.get(6, {}).get("result") is not None,
      "the implemented request after the unknown traffic still answers")
sys.exit(1 if failures else 0)
PY
rc=$?
if [ $rc -eq 0 ]; then ok "lsp unhandled request answers null"; else bad "lsp unhandled request answers null"; fi

# ---- documentHighlight: occurrences in the current document (no uri) ----
# Same lexical scan as references, but each result is a range-only
# DocumentHighlight (the editor knows the current document).
python3 - <<'PY'
import sys, json, subprocess
src = ("function add(a: int, b: int) returns int\n{\n    return a + b\n}\n\n"
       "function main() returns int\n{\n    return add(add(1, 2), 3)\n}\n")
import os as _os, shutil as _sh; _ws = "/tmp/lsp_ws/hl_test"; _sh.rmtree(_ws, ignore_errors=True); _os.makedirs(_ws)
path = _ws + "/lsp_hl_test.scaly"
open(path, "w").write(src)
def frame(o):
    b=json.dumps(o).encode(); return ("Content-Length: %d\r\n\r\n"%len(b)).encode()+b
def hl(idn, line, char):
    return frame({"jsonrpc":"2.0","id":idn,"method":"textDocument/documentHighlight",
                  "params":{"textDocument":{"uri":"file://"+path},
                            "position":{"line":line,"character":char}}})
inp  = frame({"jsonrpc":"2.0","id":1,"method":"initialize","params":{}})
inp += frame({"jsonrpc":"2.0","method":"initialized","params":{}})
inp += hl(2, 0, 10)    # `add` -> 3 ranges
inp += hl(3, 1, 0)     # `{`   -> []
inp += frame({"jsonrpc":"2.0","id":9,"method":"shutdown"})
inp += frame({"jsonrpc":"2.0","method":"exit"})
out = subprocess.run(["/tmp/scalyls"], input=inp, stdout=subprocess.PIPE).stdout
frames, d = [], out
while d:
    i = d.find(b"\r\n\r\n")
    if i < 0: break
    n = int(d[:i].decode().split(":")[1].strip())
    frames.append(json.loads(d[i+4:i+4+n])); d = d[i+4+n:]
failures = 0
def check(cond, label):
    global failures
    print(("PASS  " if cond else "FAIL  ") + label)
    if not cond: failures += 1
caps = frames[0]["result"]["capabilities"]
check(caps.get("documentHighlightProvider") is True, "initialize advertises documentHighlightProvider")
def res(idn):
    f = next((x for x in frames if x.get("id") == idn), None)
    return (f or {}).get("result")
hi = res(2) or []
check(len(hi) == 3, "`add` -> 3 highlights")
check(all("uri" not in x and "range" in x for x in hi), "highlights are range-only (no uri)")
check(res(3) == [], "no identifier under cursor -> []")
sys.exit(1 if failures else 0)
PY
rc=$?
if [ $rc -eq 0 ]; then ok "lsp documentHighlight"; else bad "lsp documentHighlight"; fi

# ---- completion: every declared name (top-level + members) ----
# Parse-only flat CompletionItem[]: top-level functions/mutables/types plus
# struct methods and union variants, each with its CompletionItemKind. The
# cursor position is not used (the editor prefix-filters).
python3 - <<'PY'
import sys, json, subprocess
src = ("function add(a: int, b: int) returns int\n{\n    return a + b\n}\n\n"
       "mutable counter: int 0\n\n"
       "define Point\n(\n    x: int\n)\n{\n"
       "    function get_x(this: Point) returns int\n    {\n        return x\n    }\n}\n\n"
       "define Shape union (\n    Circle: int\n    Square: int\n)\n")
import os as _os, shutil as _sh; _ws = "/tmp/lsp_ws/completion_test"; _sh.rmtree(_ws, ignore_errors=True); _os.makedirs(_ws)
path = _ws + "/lsp_completion_test.scaly"
open(path, "w").write(src)
def frame(o):
    b=json.dumps(o).encode(); return ("Content-Length: %d\r\n\r\n"%len(b)).encode()+b
inp  = frame({"jsonrpc":"2.0","id":1,"method":"initialize","params":{}})
inp += frame({"jsonrpc":"2.0","method":"initialized","params":{}})
inp += frame({"jsonrpc":"2.0","id":2,"method":"textDocument/completion",
              "params":{"textDocument":{"uri":"file://"+path},
                        "position":{"line":2,"character":4}}})
inp += frame({"jsonrpc":"2.0","id":9,"method":"shutdown"})
inp += frame({"jsonrpc":"2.0","method":"exit"})
out = subprocess.run(["/tmp/scalyls"], input=inp, stdout=subprocess.PIPE).stdout
frames, d = [], out
while d:
    i = d.find(b"\r\n\r\n")
    if i < 0: break
    n = int(d[:i].decode().split(":")[1].strip())
    frames.append(json.loads(d[i+4:i+4+n])); d = d[i+4+n:]
failures = 0
def check(cond, label):
    global failures
    print(("PASS  " if cond else "FAIL  ") + label)
    if not cond: failures += 1
caps = frames[0]["result"]["capabilities"]
check(isinstance(caps.get("completionProvider"), dict), "initialize advertises completionProvider (object)")
# Without `.` in triggerCharacters an editor asks only while WORD characters are
# typed, so the member list never appeared at the dot — it took the first letter
# after it. This is the whole visible half of member completion.
check((caps.get("completionProvider") or {}).get("triggerCharacters") == ["."],
      "completionProvider declares `.` as a trigger character")
r = next((f for f in frames if f.get("id") == 2), None)
items = (r or {}).get("result")
check(isinstance(items, list), "completion -> array result")
items = items or []
by = {it["label"]: it["kind"] for it in items}
check([it["label"] for it in items] == ["add","counter","Point","get_x","Shape","Circle","Square"],
      "flat names: top-level + members, in source order")
check(by.get("add") == 3, "function -> Function(3)")
check(by.get("counter") == 6, "mutable -> Variable(6)")
check(by.get("Point") == 22, "define struct -> Struct(22)")
check(by.get("get_x") == 2, "struct method -> Method(2)")
check(by.get("Shape") == 13, "union -> Enum(13)")
check(by.get("Circle") == 20, "union variant -> EnumMember(20)")
sys.exit(1 if failures else 0)
PY
rc=$?
if [ $rc -eq 0 ]; then ok "lsp completion"; else bad "lsp completion"; fi

# ---- completion `detail`: the grey text beside the label ----
# A list of bare names cannot be read: `append` says nothing about whether it
# takes a String, a char or an int. detail carries the signature (minus the
# name, which IS the label) or the type. Two decisions pinned here:
#   * the routine WORD is kept — the CompletionItemKind icon is Function(3) for
#     both, so nothing else distinguishes a pure `function` from a `procedure`
#   * a leading `this` is DROPPED, so the detail agrees with the SignatureHelp
#     that pops up one keystroke later (hover keeps it — it renders the
#     declaration, not the call)
python3 - <<'PY'
import sys, json, subprocess
doc = ("mutable counter: int 0\n"
       "\n"
       "define Point\n(\n    x: int\n    y: double\n)\n{\n"
       "    init(x: int, y: double)\n    {\n    }\n"
       "    function distance(this, other: Point) returns double\n"
       "    {\n"
       "        this.\n"
       "        0.0\n"
       "    }\n"
       "    procedure move(this, dx: int)\n    {\n    }\n}\n"
       "\n"
       "function pick[T](a: T, b: T) returns T\n{\n    a\n}\n"
       "\n"
       "procedure emit(text: String)\n{\n}\n")
import os as _os, shutil as _sh; _ws = "/tmp/lsp_ws/detail_test"; _sh.rmtree(_ws, ignore_errors=True); _os.makedirs(_ws)
uri = "file://" + _ws + "/lsp_detail_test.scaly"
def frame(o):
    b=json.dumps(o).encode(); return ("Content-Length: %d\r\n\r\n"%len(b)).encode()+b
def items_at(line, ch):
    inp  = frame({"jsonrpc":"2.0","id":1,"method":"initialize","params":{}})
    inp += frame({"jsonrpc":"2.0","method":"initialized","params":{}})
    inp += frame({"jsonrpc":"2.0","method":"textDocument/didOpen","params":{"textDocument":{
            "uri":uri,"languageId":"scaly","version":1,"text":doc}}})
    inp += frame({"jsonrpc":"2.0","id":2,"method":"textDocument/completion","params":{
            "textDocument":{"uri":uri},"position":{"line":line,"character":ch}}})
    inp += frame({"jsonrpc":"2.0","id":9,"method":"shutdown"})
    inp += frame({"jsonrpc":"2.0","method":"exit"})
    out = subprocess.run(["/tmp/scalyls"], input=inp, stdout=subprocess.PIPE).stdout
    d = out
    while d:
        i = d.find(b"\r\n\r\n")
        if i < 0: break
        n = int(d[:i].decode().split(":")[1].strip())
        f = json.loads(d[i+4:i+4+n]); d = d[i+4+n:]
        if f.get("id") == 2: return f.get("result") or []
    return []
failures = 0
def check(cond, label):
    global failures
    print(("PASS  " if cond else "FAIL  ") + label)
    if not cond: failures += 1
flat = {it["label"]: it for it in items_at(24, 5)}
check(flat.get("pick", {}).get("detail") == "function[T](a: T, b: T) returns T",
      "function detail carries generics, params and return")
check(flat.get("emit", {}).get("detail") == "procedure(text: String)",
      "procedure detail says `procedure`, not `function`")
check(flat.get("counter", {}).get("detail") == "int",
      "a typed mutable gets its type as detail")
check(flat.get("Point", {}).get("detail") is None,
      "a type has no detail — the icon already says Struct")
members = {it["label"]: it for it in items_at(13, 13)}
# `init` rides along after `this.` — pre-existing and pinned, not endorsed: a
# Scaly value is constructed as `Point(x, y)`, never as `this.init(...)`, so
# this entry is noise. Pinned so that removing it is a visible decision.
check(set(members) == {"x", "y", "init", "distance", "move"},
      "`this.` -> the enclosing type's fields, init and methods")
check(members.get("init", {}).get("detail") == "(x: int, y: double)",
      "init's detail is its parameter list (it has no name of its own)")
check(members.get("x", {}).get("detail") == "int"
      and members.get("y", {}).get("detail") == "double",
      "a field gets its declared type as detail")
check(members.get("distance", {}).get("detail") == "function(other: Point) returns double",
      "a method's detail DROPS the `this` receiver")
check(members.get("move", {}).get("detail") == "procedure(dx: int)",
      "a mutating member reads as `procedure`")
sys.exit(1 if failures else 0)
PY
rc=$?
if [ $rc -eq 0 ]; then ok "lsp completion detail"; else bad "lsp completion detail"; fi

# ---- completionItem/resolve: the doc block, one entry at a time ------------
# The `;` block above a declaration is what hover shows, and it is the most
# valuable thing a completion entry could carry. It is NOT sent with the list:
# measured on packages/scalyls/0.1.0/scalyls/symbols.scaly, the flat list is 392
# entries / 85 KB, and resolving all of them takes it to 182 KB — 2.1x, per
# keystroke, for a reader who looks at one entry.
#
# So each entry carries `data` — the uri of the FILE IT WAS READ FROM — and the
# doc arrives on completionItem/resolve for the selected entry alone. `data`
# naming the file is what keeps resolve to one parse of one file: measured
# before this existed, the workspace search a name-only resolve would need
# costs 7.93 s cross-package and 1.12 s cross-file cold, against 0.026 s
# (median of 40) for the lookup this does.
#
# The checks that measure the EFFECT rather than the form:
#   * two documented routines in one file resolve to their OWN blocks, so an
#     implementation that always answers the first one fails
#   * a member read from ANOTHER file resolves against THAT file, while the open
#     document declares the same name with a different block — so using the
#     request's document instead of `data` is visible
#   * a name containing `"` and `}` still yields parseable JSON: the data
#     splice tracks string state and brace depth, and a scan that did not would
#     cut an entry in half
#
# 15 of the 19 fail on the binary built one commit earlier. The four that pass
# there are the WATCHDOGS and pass by carrying nothing: that the list has no
# `documentation` on it (which is the whole point and must stay true), that `s.`
# still lists Shape's members, and the two "and NOT the other file's text"
# companions — each guards a claim its red neighbour makes.
python3 - <<'PY'
import sys, json, subprocess, os, shutil
ws = "/tmp/lsp_ws/completion_resolve"
shutil.rmtree(ws, ignore_errors=True); os.makedirs(ws)

# The OTHER file: the concept whose members complete, with its own doc blocks.
other = ("; The shape module banner.\n"
         "\n"
         "define Shape\n(\n    side: int\n)\n{\n"
         "    ; The area, computed in the OTHER file.\n"
         "    function area(this: Shape) returns int\n    {\n        side * side\n    }\n"
         "    function undocumented_member(this: Shape) returns int\n    {\n        1\n    }\n}\n")
open(ws + "/helper.scaly", "w").write(other)

doc = ("; A file banner that must NOT reach any entry.\n"                 # 0
       "\n"                                                              # 1
       "; Add two numbers.\n"                                            # 2
       ";\n"                                                             # 3
       ";   a  the left one\n"                                           # 4
       "function add(a: int, b: int) returns int\n{\n    a + b\n}\n"     # 5-8
       "\n"                                                              # 9
       "; Subtract two numbers.\n"                                       # 10
       "function sub(a: int, b: int) returns int\n{\n    a - b\n}\n"     # 11-14
       "\n"                                                              # 15
       "function undocumented() returns int\n{\n    1\n}\n"              # 16-19
       "\n"                                                              # 20
       "; A different area, declared in the OPEN file.\n"                # 21
       "function area() returns int\n{\n    0\n}\n"                      # 22-25
       "\n"                                                              # 26
       "; A point in the plane.\n"                                       # 27
       "define Point\n(\n    x: int\n)\n{\n"                             # 28-32
       "    ; The x coordinate.\n"                                       # 33
       "    function get_x(this: Point) returns int\n    {\n        x\n    }\n}\n"   # 34-38
       "\n"                                                              # 39
       "; A name with a quote and a brace in it.\n"                      # 40
       "function 'we\"ird}x'() returns int\n{\n    1\n}\n"                # 41-44
       "\n"                                                              # 45
       "function use_it(s: Shape) returns int\n{\n"                      # 46-47
       "    s.\n"                                                        # 48
       "    0\n}\n")                                                     # 49-50
path = ws + "/main.scaly"
open(path, "w").write(doc)
uri = "file://" + path

def frame(o):
    b=json.dumps(o).encode(); return ("Content-Length: %d\r\n\r\n"%len(b)).encode()+b
def session(msgs):
    inp  = frame({"jsonrpc":"2.0","id":1,"method":"initialize","params":{"rootUri":"file://"+ws}})
    inp += frame({"jsonrpc":"2.0","method":"initialized","params":{}})
    inp += frame({"jsonrpc":"2.0","method":"textDocument/didOpen","params":{"textDocument":{
            "uri":uri,"languageId":"scaly","version":1,"text":doc}}})
    for m in msgs: inp += frame(m)
    inp += frame({"jsonrpc":"2.0","id":99,"method":"shutdown"})
    inp += frame({"jsonrpc":"2.0","method":"exit"})
    out = subprocess.run(["/tmp/scalyls"], input=inp, stdout=subprocess.PIPE).stdout
    frames, d = [], out
    while d:
        i = d.find(b"\r\n\r\n")
        if i < 0: break
        n = int(d[:i].decode().split(":")[1].strip())
        frames.append(json.loads(d[i+4:i+4+n])); d = d[i+4+n:]
    return frames
def result(frames, idn):
    f = next((x for x in frames if x.get("id") == idn), None)
    return None if f is None else f.get("result")

def completion(line, ch):
    return {"jsonrpc":"2.0","id":2,"method":"textDocument/completion","params":{
            "textDocument":{"uri":uri},"position":{"line":line,"character":ch}}}

failures = 0
def check(cond, label):
    global failures
    print(("PASS  " if cond else "FAIL  ") + label)
    if not cond: failures += 1

FENCE = "`" * 3

# --- the flat list of the open document, then a resolve for each entry -----
comp = completion(19, 0)
fr = session([comp])
caps = ((result(fr, 1) or {}).get("capabilities") or {}).get("completionProvider") or {}
check(caps.get("resolveProvider") is True,
      "the server advertises completionItem/resolve")

flat = result(fr, 2) or []
by = {it["label"]: it for it in flat}
check(len(flat) > 0 and all(it.get("data") == uri for it in flat),
      "every entry names the file it was read from in `data`")
check(all("documentation" not in it for it in flat),
      "the list itself carries NO documentation -- that is what resolve is for")
check(by.get('we"ird}x', {}).get("data") == uri,
      "a label holding a quote and a brace survives the `data` splice")

wanted = ["add", "sub", "undocumented", 'we"ird}x', "get_x", "Point"]
msgs = [comp] + [{"jsonrpc":"2.0","id":100+k,"method":"completionItem/resolve",
                  "params":by[n]} for k, n in enumerate(wanted) if n in by]
# an entry with no `data` at all (a client-invented one), and one whose label
# names nothing: both must come back unchanged rather than unanswered.
msgs.append({"jsonrpc":"2.0","id":200,"method":"completionItem/resolve",
             "params":{"label":"add","kind":3}})
msgs.append({"jsonrpc":"2.0","id":201,"method":"completionItem/resolve",
             "params":{"label":"nosuchname","kind":3,"data":uri}})
fr = session(msgs)
res = {}
for k, n in enumerate(wanted):
    if n in by: res[n] = result(fr, 100+k)
def value(n):
    d = (res.get(n) or {}).get("documentation")
    return d and d.get("value")

add = value("add") or ""
check((res.get("add") or {}).get("documentation", {}).get("kind") == "markdown",
      "the documentation is MarkupContent, kind markdown")
check(add.startswith(FENCE + "\n") and add.endswith("\n" + FENCE),
      "it is fenced, so markdown cannot reflow a hand-wrapped block")
check("Add two numbers." in add and "\n\n  a  the left one\n" in add,
      "the block's own line structure survives (paragraph break + indent)")
check("file banner" not in add,
      "a block above a BLANK LINE is not part of it")
got_add = res.get("add") or {}
check(got_add == dict(by["add"], documentation=got_add.get("documentation")),
      "the entry comes back with label/kind/detail/data untouched")
check(res.get("undocumented") == by["undocumented"],
      "an undocumented routine resolves to itself, with no documentation key")
check("The x coordinate." in (value("get_x") or ""),
      "a member nested in a `define` carries its own block")
check("A point in the plane." in (value("Point") or ""),
      "a concept carries its block")
# THE effect check: each entry gets ITS OWN block, not the first one found.
check("Subtract two numbers." in (value("sub") or "") and "Add two numbers." not in (value("sub") or ""),
      "a second documented routine resolves to ITS block, not the first one's")
check(result(fr, 200) == {"label":"add","kind":3},
      "an entry with no `data` is answered unchanged rather than guessed at")
check(result(fr, 201) == {"label":"nosuchname","kind":3,"data":uri},
      "a label that declares nothing resolves to itself")

# --- the cross-file half: `s.` lists Shape's members from helper.scaly -----
comp_x = completion(48, 6)
fr = session([comp_x])
members = {it["label"]: it for it in (result(fr, 2) or [])}
helper_uri = "file://" + ws + "/helper.scaly"
check(set(members) == {"side", "area", "undocumented_member"},
      "`s.` lists the members of Shape, which is declared in the other file")
check(all(it.get("data") == helper_uri for it in members.values()),
      "their `data` names helper.scaly, NOT the document the request came from")
msgs = [comp_x, {"jsonrpc":"2.0","id":300,"method":"completionItem/resolve","params":members["area"]}]
fr = session(msgs)
xdoc = ((result(fr, 300) or {}).get("documentation") or {}).get("value") or ""
check("The area, computed in the OTHER file." in xdoc,
      "the member resolves against the file it was read from")
check("declared in the OPEN file" not in xdoc,
      "and NOT against the open document, which declares `area` too")
sys.exit(1 if failures else 0)
PY
rc=$?
if [ $rc -eq 0 ]; then ok "lsp completionItem resolve"; else bad "lsp completionItem resolve"; fi

# ---- codeAction: the `use` quick fix ----
# The planner writes the fix INTO the diagnostic ("...; add: use a.b.C") and the
# client hands that diagnostic back with the codeAction request, so the server
# reads the answer rather than recomputing it. Two halves are gated: the real
# round trip through a planner-produced diagnostic — which is what pins the
# marker in Planner.use_vis_hint# to the reader in server.use_paths_from_
# diagnostics#, a pair that would otherwise drift apart silently — and the
# INSERT POSITION across the file shapes that occur, since a `use` above the
# file's own header comment is wrong in the shape every file in this tree has.
python3 - <<'PY'
import sys, json, subprocess, os, tempfile, threading, queue, shutil
home = os.getcwd()
d = tempfile.mkdtemp()
os.makedirs(d + "/packages/demo/0.1.0/demo")
os.symlink(home + "/packages/scaly", d + "/packages/scaly")
open(d + "/packages/demo/0.1.0/demo.scaly", "w").write(
    "define demo\n{\n    module widgets\n    module app\n}\n")
open(d + "/packages/demo/0.1.0/demo/widgets.scaly", "w").write(
    "define widgets\n{\n    define Widget\n    (\n        size: int\n    )\n}\n")
app = d + "/packages/demo/0.1.0/demo/app.scaly"
open(app, "w").write(
    "; app.scaly - uses Widget without a `use` for it\n"
    "\n"
    "define app\n{\n    function make() returns int\n    {\n"
    "        let w Widget(3)\n        w.size\n    }\n}\n")
uri = "file://" + app
def frame(o):
    b=json.dumps(o).encode(); return ("Content-Length: %d\r\n\r\n"%len(b)).encode()+b
failures = 0
def check(cond, label):
    global failures
    print(("PASS  " if cond else "FAIL  ") + label)
    if not cond: failures += 1

p = subprocess.Popen(["/tmp/scalyls"], stdin=subprocess.PIPE, stdout=subprocess.PIPE,
                     env=dict(os.environ, SCALY_HOME=d))
q = queue.Queue()
def reader():
    buf=b""
    while True:
        c=p.stdout.read(1)
        if not c: return
        buf+=c
        i=buf.find(b"\r\n\r\n")
        if i>=0:
            n=int(buf[:i].decode().split(":")[1].strip())
            while len(buf)<i+4+n:
                ch=p.stdout.read(i+4+n-len(buf))
                if not ch: return
                buf+=ch
            q.put(json.loads(buf[i+4:i+4+n])); buf=buf[i+4+n:]
threading.Thread(target=reader, daemon=True).start()
def send(o): p.stdin.write(frame(o)); p.stdin.flush()
def wait_for(pred):
    while True:
        f = q.get(timeout=90)
        if pred(f): return f
try:
    send({"jsonrpc":"2.0","id":1,"method":"initialize","params":{"rootUri":"file://"+d}})
    send({"jsonrpc":"2.0","method":"initialized","params":{}})
    caps = wait_for(lambda f: f.get("id") == 1)["result"]["capabilities"]
    check(caps.get("codeActionProvider") is True, "initialize advertises codeActionProvider")
    send({"jsonrpc":"2.0","method":"textDocument/didOpen","params":{"textDocument":{
            "uri":uri,"languageId":"scaly","version":1,"text":open(app).read()}}})
    diags = wait_for(lambda f: f.get("method") == "textDocument/publishDiagnostics"
                     )["params"]["diagnostics"]
    check(len(diags) == 1 and "add: use demo.widgets.Widget" in diags[0]["message"],
          "the planner's diagnostic carries the fix it wants")
    send({"jsonrpc":"2.0","id":5,"method":"textDocument/codeAction","params":{
            "textDocument":{"uri":uri},"range":diags[0]["range"],
            "context":{"diagnostics":diags}}})
    acts = wait_for(lambda f: f.get("id") == 5)["result"]
    check(len(acts) == 1 and acts[0]["title"] == "Add: use demo.widgets.Widget",
          "codeAction -> one quickfix naming the path")
    check(acts[0].get("kind") == "quickfix", "declared as a quickfix, so the lightbulb offers it")
    edits = acts[0]["edit"]["changes"][uri]
    check(len(edits) == 1 and edits[0]["newText"] == "use demo.widgets.Widget\n",
          "the edit inserts the use line")
    r = edits[0]["range"]
    check(r["start"] == r["end"], "a zero-width range — an insertion, not a replacement")
    check(r["start"]["line"] == 1,
          "inserted BELOW the file's header comment, not above it")
    # No diagnostics at the cursor: the client asks on every position, so the
    # answer has to be an empty array rather than an action with no fix in it.
    send({"jsonrpc":"2.0","id":6,"method":"textDocument/codeAction","params":{
            "textDocument":{"uri":uri},
            "range":{"start":{"line":0,"character":0},"end":{"line":0,"character":0}},
            "context":{"diagnostics":[]}}})
    check(wait_for(lambda f: f.get("id") == 6)["result"] == [],
          "no diagnostic at the cursor -> []")
    send({"jsonrpc":"2.0","id":9,"method":"shutdown"}); send({"jsonrpc":"2.0","method":"exit"})
    p.wait(timeout=20)
except Exception as e:
    # type name, not str(e): a queue.Empty — the shape a server that never
    # answers produces — stringifies to nothing at all.
    print("FAIL  codeAction round trip (" + type(e).__name__ + ": " + str(e) + ")")
    failures += 1
    p.kill()
finally:
    shutil.rmtree(d, ignore_errors=True)
sys.exit(1 if failures else 0)
PY
rc=$?
if [ $rc -eq 0 ]; then ok "lsp codeAction use quickfix"; else bad "lsp codeAction use quickfix"; fi

# ---- codeAction: quick fixes for the hard planner gates ------------------
# The 2026-08-15 gates each report a mistake with a mechanical repair, and each
# fix here is derived from the diagnostic's own MESSAGE — the planner has
# already decided what is wrong, and asking it a second time would let the two
# answers drift (symbols.code_actions_for# carries the argument).
#
# ★THE ASSERTION IS THAT THE FIX FIXES IT. Every case below applies the edit,
# writes the result back and re-analyses: the diagnostic must be GONE. A test
# that compares the produced text against an expected string only proves the
# server is self-consistent; re-running the compiler over the result is what
# makes the round trip — gate, message, action, range, edit — load-bearing end
# to end.
#
# The REFUSALS are the other half, and each would produce a wrong edit if it
# fired: a second `=` on the line (which comparison was meant?), a comment that
# the parentheses would swallow, a name occurring twice on the line (which one
# is the typo?), and a genuine dead expression, where parenthesizing `c + b`
# would produce `c(+ b)`.
{ SCALY_HOME="$(pwd)" python3 - <<'PY'
import sys, json, subprocess, os, select, time, shutil

def frame(o):
    b = json.dumps(o).encode()
    return ("Content-Length: %d\r\n\r\n" % len(b)).encode() + b

class Session:
    def __init__(self):
        self.p = subprocess.Popen(["/tmp/scalyls"], stdin=subprocess.PIPE,
                                  stdout=subprocess.PIPE, bufsize=0)
        self.buf = bytearray()
        self.send({"jsonrpc":"2.0","id":1,"method":"initialize","params":{}})
        self.next_frame()
        self.send({"jsonrpc":"2.0","method":"initialized","params":{}})

    def send(self, o):
        self.p.stdin.write(frame(o)); self.p.stdin.flush()

    def next_frame(self, timeout=60.0):
        fd = self.p.stdout.fileno()
        deadline = time.time() + timeout
        while True:
            i = self.buf.find(b"\r\n\r\n")
            if i >= 0:
                n = int(bytes(self.buf[:i]).decode().split(":")[1].strip())
                if len(self.buf) >= i + 4 + n:
                    body = bytes(self.buf[i+4:i+4+n]); del self.buf[:i+4+n]
                    return json.loads(body)
            left = deadline - time.time()
            if left <= 0: return None
            if not select.select([fd], [], [], left)[0]: return None
            chunk = os.read(fd, 65536)
            if not chunk: return None
            self.buf.extend(chunk)

    # The diagnostics published for `path` after an open or an edit.
    def analyse(self, path, text, version):
        uri = "file://" + path
        if version == 1:
            self.send({"jsonrpc":"2.0","method":"textDocument/didOpen","params":{
                "textDocument":{"uri":uri,"languageId":"scaly","version":1,"text":text}}})
        else:
            self.send({"jsonrpc":"2.0","method":"textDocument/didChange","params":{
                "textDocument":{"uri":uri,"version":version},
                "contentChanges":[{"text":text}]}})
        out = {}
        f = self.next_frame(60.0)
        while f is not None:
            if f.get("method") == "textDocument/publishDiagnostics":
                out[f["params"]["uri"]] = f["params"]["diagnostics"]
            f = self.next_frame(2.0)
        return out.get(uri, [])

    def actions(self, path, diag, idn):
        self.send({"jsonrpc":"2.0","id":idn,"method":"textDocument/codeAction","params":{
            "textDocument":{"uri":"file://"+path}, "range":diag["range"],
            "context":{"diagnostics":[diag]}}})
        while True:
            f = self.next_frame(60.0)
            if f is None: return []
            if f.get("id") == idn: return f.get("result") or []

    def close(self):
        self.send({"jsonrpc":"2.0","method":"exit"}); self.p.wait()

# A line of a possibly-missing result. A failing fix leaves `fixed` None, and
# an indexing crash there would take the whole suite down instead of reporting
# the one check that failed -- which is exactly what the negative control did.
def line_of(text, i):
    lines = (text or "").split("\n")
    if i >= len(lines): return None
    return lines[i]

def apply_edit(text, action):
    changes = list(action["edit"]["changes"].values())[0]
    lines = text.split("\n")
    for e in sorted(changes, key=lambda e: (-e["range"]["start"]["line"],
                                            -e["range"]["start"]["character"])):
        sl, sc = e["range"]["start"]["line"], e["range"]["start"]["character"]
        el, ec = e["range"]["end"]["line"], e["range"]["end"]["character"]
        if sl == el:
            lines[sl] = lines[sl][:sc] + e["newText"] + lines[el][ec:]
        else:
            lines[sl:el+1] = [lines[sl][:sc] + e["newText"] + lines[el][ec:]]
    return "\n".join(lines)

failures = 0
def check(cond, label):
    global failures
    print(("PASS  " if cond else "FAIL  ") + label)
    if not cond: failures += 1

ws = "/tmp/lsp_ws/codeaction_fixes"
shutil.rmtree(ws, ignore_errors=True); os.makedirs(ws)
s = Session()
counter = [0]

# Open `src`, find the diagnostic whose message contains `marker`, take its
# quick fixes, apply the first, and report (titles, line-after, diagnostic-gone).
def fix(name, src, marker):
    counter[0] += 1
    path = ws + "/" + name + ".scaly"
    open(path, "w").write(src)
    diags = s.analyse(path, src, 1)
    hit = [d for d in diags if marker in d["message"]]
    if not hit:
        return (None, None, None, [d["message"] for d in diags])
    d = hit[0]
    acts = s.actions(path, d, 500 + counter[0])
    if not acts:
        return ([], None, None, None)
    fixed = apply_edit(src, acts[0])
    open(path, "w").write(fixed)
    left = s.analyse(path, fixed, 2)
    gone = not [x for x in left if marker in x["message"]]
    return ([a["title"] for a in acts], fixed, gone, None)

# 1. `a = b` where `set a: b` was meant.
titles, fixed, gone, _ = fix("assign",
    "function demo() returns int\n{\n"
    "    var buffer u8[4]\n"
    "    set buffer[0]: 111\n"
    "    buffer[0] = 222\n"
    "    return 1\n}\n", "it does not assign")
check(titles == ["Assign: set buffer[0]: ..."], "`=` statement offers the set fix -- %s" % (titles,))
check(line_of(fixed, 4) == "    set buffer[0]: 222",
      "the edit writes `set buffer[0]: 222`, indentation kept")
check(gone is True, "and re-analysing the fixed file no longer reports it")

# 2. a parenless head that swallowed its argument's own call.
titles, fixed, gone, _ = fix("paren",
    "function helper(v: int) returns int\n    v + 1\n\n"
    "function demo() returns int\n{\n"
    "    print helper(2)\n"
    "    return 1\n}\n", "is discarded")
check(titles == ["Parenthesize the argument of print"],
      "swallowed call head offers the parenthesize fix -- %s" % (titles,))
check(line_of(fixed, 5) == "    print(helper(2))", "the edit parenthesizes the argument")
check(gone is True, "and the statement then resolves")

# 3-5. a misspelled name, in the three positions whose diagnostics differ.
SPELL = ("function twice(v: int) returns int\n    v + v\n\n"
         "function demo() returns int\n{\n"
         "    var counter 3\n"
         "    %s\n"
         "    return counter\n}\n")
for label, stmt, want in [
        ("in an operand",      "let b 1 + countr",  "    let b 1 + counter"),
        ("as a `set` target",  "set countr: 5",     "    set counter: 5"),
        ("as a call argument", "let c twice(countr)", "    let c twice(counter)")]:
    titles, fixed, gone, _ = fix("spell_" + label.split()[-1], SPELL % stmt, "countr")
    check(titles == ["Change to counter"], "a typo %s offers the near name -- %s" % (label, titles))
    check(line_of(fixed, 6) == want,
          "and replaces exactly the name %s" % label)

# 6-9. the refusals. Each would be a WRONG edit.
for label, src, marker in [
        ("two `=` on one line",
         "function demo() returns int\n{\n    var counter 3\n"
         "    set counter: 1\n    counter = 2: counter = 3\n    return counter\n}\n",
         "it does not assign"),
        ("a comment the parens would swallow",
         "function helper(v: int) returns int\n    v + 1\n\n"
         "function demo() returns int\n{\n    print helper(2)  ; a note\n    return 1\n}\n",
         "is discarded"),
        ("the name occurs twice on the line",
         "function demo() returns int\n{\n    var counter 3\n"
         "    let d countr + countr\n    return counter + d\n}\n",
         "countr"),
        ("a genuine dead expression",
         "function demo() returns int\n{\n    let a 1\n    let b 2\n"
         "    a + b\n    return a\n}\n",
         "is discarded")]:
    titles, _, _, msgs = fix("refuse_" + str(len(label)), src, marker)
    check(titles == [], "refused: %s -- %s" % (label, titles if titles is not None else msgs))

s.close()
sys.exit(1 if failures else 0)
PY
} > "$BGDIR/lsp_codeaction_gate_fixes.out" 2>&1 &
bg_add $! "lsp codeAction gate fixes" "$BGDIR/lsp_codeaction_gate_fixes.out"

# ---- callHierarchy: prepare / incoming / outgoing ----
# prepare must resolve from a CALL SITE, not just from a declaration — that is
# where a reader asks "who calls this?". incoming groups call sites by the
# routine that CONTAINS them (two sites in one caller = one entry with two
# ranges, not two entries). outgoing scans one body and dedupes by callee.
# The walk root is pointed at a directory that does not exist, so the whole
# assertion is about the OPEN BUFFER and cannot drift with the repo's contents.
python3 - <<'PY'
import sys, json, subprocess, os
doc = ("function helper(a: int) returns int\n{\n    a\n}\n\n"
       "function middle(b: int) returns int\n{\n    helper(b) + helper(b + 1)\n}\n\n"
       "function top() returns int\n{\n    middle(1) + helper(2)\n}\n")
import shutil as _sh; _ws = "/tmp/lsp_ws/callh_test"; _sh.rmtree(_ws, ignore_errors=True); os.makedirs(_ws)
uri = "file://" + _ws + "/lsp_callh_test.scaly"
def frame(o):
    b=json.dumps(o).encode(); return ("Content-Length: %d\r\n\r\n"%len(b)).encode()+b
def session(reqs):
    inp  = frame({"jsonrpc":"2.0","id":1,"method":"initialize","params":{
            "rootUri":"file:///tmp/lsp_callh_no_such_root"}})
    inp += frame({"jsonrpc":"2.0","method":"initialized","params":{}})
    inp += frame({"jsonrpc":"2.0","method":"textDocument/didOpen","params":{"textDocument":{
            "uri":uri,"languageId":"scaly","version":1,"text":doc}}})
    for r in reqs: inp += frame(r)
    inp += frame({"jsonrpc":"2.0","id":99,"method":"shutdown"})
    inp += frame({"jsonrpc":"2.0","method":"exit"})
    out = subprocess.run(["/tmp/scalyls"], input=inp, stdout=subprocess.PIPE).stdout
    got, d = {}, out
    while d:
        i = d.find(b"\r\n\r\n")
        if i < 0: break
        n = int(d[:i].decode().split(":")[1].strip())
        f = json.loads(d[i+4:i+4+n]); d = d[i+4+n:]
        if isinstance(f.get("id"), int): got[f["id"]] = f.get("result")
    return got
failures = 0
def check(cond, label):
    global failures
    print(("PASS  " if cond else "FAIL  ") + label)
    if not cond: failures += 1
def prep(line, ch, idn):
    return {"jsonrpc":"2.0","id":idn,"method":"textDocument/prepareCallHierarchy",
            "params":{"textDocument":{"uri":uri},"position":{"line":line,"character":ch}}}
g = session([prep(7, 6, 2), prep(0, 10, 3), prep(5, 10, 4)])
check(g[1]["capabilities"].get("callHierarchyProvider") is True,
      "initialize advertises callHierarchyProvider")
from_call = g.get(2) or []
check(len(from_call) == 1 and from_call[0]["name"] == "helper"
      and from_call[0]["range"]["start"]["line"] == 0,
      "prepare on a CALL SITE resolves to the declaration")
check(from_call and from_call[0]["selectionRange"]["start"]["line"] == 0,
      "the selectionRange points at the name")
helper_item = (g.get(3) or [None])[0]
middle_item = (g.get(4) or [None])[0]
if helper_item is None or middle_item is None:
    print("FAIL  prepare produced no item to follow up with"); sys.exit(1)
g2 = session([{"jsonrpc":"2.0","id":5,"method":"callHierarchy/incomingCalls","params":{"item":helper_item}},
              {"jsonrpc":"2.0","id":6,"method":"callHierarchy/outgoingCalls","params":{"item":middle_item}},
              {"jsonrpc":"2.0","id":7,"method":"callHierarchy/outgoingCalls","params":{"item":helper_item}}])
inc = {c["from"]["name"]: c for c in (g2.get(5) or [])}
check(set(inc) == {"middle", "top"}, "incoming: exactly the two routines that call helper")
check(len(inc.get("middle", {}).get("fromRanges", [])) == 2,
      "two call sites in one caller are ONE entry with two ranges")
check(len(inc.get("top", {}).get("fromRanges", [])) == 1, "and the other caller has one")
out = {c["to"]["name"]: c for c in (g2.get(6) or [])}
check(set(out) == {"helper"}, "outgoing: middle calls helper, deduped to one entry")
check(len(out.get("helper", {}).get("fromRanges", [])) == 2, "with both of its call sites")
check((g2.get(7) or []) == [], "a routine that calls nothing -> []")
sys.exit(1 if failures else 0)
PY
rc=$?
if [ $rc -eq 0 ]; then ok "lsp callHierarchy"; else bad "lsp callHierarchy"; fi

# ---- typeDefinition: go to the TYPE of the thing at the cursor ----
# Four shapes, and the order they are tried in is the point: the OPEN BUFFER
# before the workspace. Asking the workspace first got every case wrong in one
# run — a local `Point` answered with dazzle's, a local `make()` with the return
# type of some other `make` in the tree — because the document is the editor's
# copy and the file at that path may be stale or absent.
python3 - <<'PY'
import sys, json, subprocess, os
doc = ("define Point\n(\n    x: int\n)\n{\n}\n\n"
       "function make() returns Point\n{\n    Point(1)\n}\n\n"
       "function use_it() returns int\n{\n"
       "    let p Point(2)\n"          # 14  construction
       "    let q make()\n"            # 15  initialised by a CALL
       "    var sb StringBuilder()\n"  # 16  stdlib, cross-package
       "    p.x\n}\n")
# ★Own empty directory, like every other fixture: the document's own dir tree
# is the fallback walk root, so with the fixtures sharing /tmp this test
# resolved `Point` to a SIBLING fixture's `define Point` (it went red the
# moment `find -H` made that walk work at all on macOS).
import shutil as _sh; _ws = "/tmp/lsp_ws/typedef_test"; _sh.rmtree(_ws, ignore_errors=True); os.makedirs(_ws)
uri = "file://" + _ws + "/lsp_typedef_test.scaly"
# ★A SIBLING declaring a ROUTINE with the same name as the buffer's TYPE. It
# pins the narrowing that took case (9,4) from 28 s to 0.02 s: step 2 used to
# search every sibling and every package for a routine named `Point` before
# step 3 asked the buffer, so this file's return type won — and the walk could
# never change the answer for a name the buffer declares, only delay it. With
# the old order the check below reads `Other`, at line 0 of THIS file.
open(_ws + "/sibling.scaly", "w").write(
    "define Other\n(\n    v: int\n)\n{\n}\n\nfunction Point() returns Other\n{\n    Other(1)\n}\n")
def frame(o):
    b=json.dumps(o).encode(); return ("Content-Length: %d\r\n\r\n"%len(b)).encode()+b
cases = [(14, 8), (15, 8), (15, 10), (9, 4), (16, 8)]
inp  = frame({"jsonrpc":"2.0","id":1,"method":"initialize","params":{
        "rootUri":"file://"+os.getcwd()}})
inp += frame({"jsonrpc":"2.0","method":"initialized","params":{}})
inp += frame({"jsonrpc":"2.0","method":"textDocument/didOpen","params":{"textDocument":{
        "uri":uri,"languageId":"scaly","version":1,"text":doc}}})
for i, (l, c) in enumerate(cases):
    inp += frame({"jsonrpc":"2.0","id":10+i,"method":"textDocument/typeDefinition","params":{
            "textDocument":{"uri":uri},"position":{"line":l,"character":c}}})
inp += frame({"jsonrpc":"2.0","id":9,"method":"shutdown"})
inp += frame({"jsonrpc":"2.0","method":"exit"})
out = subprocess.run(["/tmp/scalyls"], input=inp, stdout=subprocess.PIPE,
                     env=dict(os.environ, SCALY_HOME=os.getcwd())).stdout
caps, res, d = None, {}, out
while d:
    i = d.find(b"\r\n\r\n")
    if i < 0: break
    n = int(d[:i].decode().split(":")[1].strip())
    f = json.loads(d[i+4:i+4+n]); d = d[i+4+n:]
    if f.get("id") == 1: caps = f["result"]["capabilities"]
    if isinstance(f.get("id"), int) and f["id"] >= 10: res[f["id"]] = f.get("result")
failures = 0
def check(cond, label):
    global failures
    print(("PASS  " if cond else "FAIL  ") + label)
    if not cond: failures += 1
def at(i):
    r = res.get(10+i)
    if not r: return None
    return (r["uri"], r["range"]["start"]["line"])
check(caps.get("typeDefinitionProvider") is True, "initialize advertises typeDefinitionProvider")
check(at(0) == (uri, 0), "a constructed binding -> its type in the SAME buffer")
check(at(1) == (uri, 0), "a binding initialised by a call -> the callee's return type")
check(at(2) == (uri, 0), "on the routine itself -> what it returns")
check(at(3) == (uri, 0), "on a type name -> that type (never nothing)")
check(at(3) is not None and at(3)[0] == uri,
      "a type declared HERE beats a same-named routine in a sibling -- the "
      "workspace walk is skipped, not merely outranked")
tgt = at(4)
check(tgt is not None and tgt[0].endswith("scaly/containers/StringBuilder.scaly"),
      "a stdlib type resolves cross-package")
sys.exit(1 if failures else 0)
PY
rc=$?
if [ $rc -eq 0 ]; then ok "lsp typeDefinition"; else bad "lsp typeDefinition"; fi

# ---- selectionRange: expand selection outward ----
# Shift+Alt+Right. Lexical (a bracket walk), because the editor asks while the
# buffer is being typed in and a parse-based answer would go silent exactly
# then. Two properties are gated: the chain nests strictly outward from the
# token, and a CALL is a step of its own — brackets alone would jump from
# `inner` straight past `inner(a, 2)` to the enclosing argument list.
python3 - <<'PY'
import sys, json, subprocess
doc = ("function outer(a: int) returns int\n"
       "{\n"
       "    let v compute(a + inner(a, 2), 7)\n"
       "    v\n"
       "}\n")
import os as _os, shutil as _sh; _ws = "/tmp/lsp_ws/selrange_test"; _sh.rmtree(_ws, ignore_errors=True); _os.makedirs(_ws)
uri = "file://" + _ws + "/lsp_selrange_test.scaly"
def frame(o):
    b=json.dumps(o).encode(); return ("Content-Length: %d\r\n\r\n"%len(b)).encode()+b
positions = [{"line":2,"character":28}, {"line":3,"character":4}]
inp  = frame({"jsonrpc":"2.0","id":1,"method":"initialize","params":{}})
inp += frame({"jsonrpc":"2.0","method":"initialized","params":{}})
inp += frame({"jsonrpc":"2.0","method":"textDocument/didOpen","params":{"textDocument":{
        "uri":uri,"languageId":"scaly","version":1,"text":doc}}})
inp += frame({"jsonrpc":"2.0","id":2,"method":"textDocument/selectionRange","params":{
        "textDocument":{"uri":uri},"positions":positions}})
inp += frame({"jsonrpc":"2.0","id":9,"method":"shutdown"})
inp += frame({"jsonrpc":"2.0","method":"exit"})
out = subprocess.run(["/tmp/scalyls"], input=inp, stdout=subprocess.PIPE).stdout
caps, res, d = None, None, out
while d:
    i = d.find(b"\r\n\r\n")
    if i < 0: break
    n = int(d[:i].decode().split(":")[1].strip())
    f = json.loads(d[i+4:i+4+n]); d = d[i+4+n:]
    if f.get("id") == 1: caps = f["result"]["capabilities"]
    if f.get("id") == 2: res = f.get("result")
failures = 0
def check(cond, label):
    global failures
    print(("PASS  " if cond else "FAIL  ") + label)
    if not cond: failures += 1
lines = doc.split("\n")
def texts(chain):
    out, node = [], chain
    while node:
        r = node["range"]; s, e = r["start"], r["end"]
        if s["line"] == e["line"]:
            out.append(lines[s["line"]][s["character"]:e["character"]])
        else:
            out.append("<multiline>")
        node = node.get("parent")
    return out
check(caps.get("selectionRangeProvider") is True, "initialize advertises selectionRangeProvider")
check(isinstance(res, list) and len(res) == 2, "one chain per requested position, in order")
# A server that does not answer leaves res None; report that as a failure
# rather than dying in a traceback three lines later.
if not isinstance(res, list) or len(res) != 2:
    print("FAIL  selectionRange returned nothing to inspect"); sys.exit(1)
c0 = texts(res[0])
check(c0[0] == "a", "innermost is the token under the cursor")
check(c0[1] == "(a, 2)", "then its enclosing argument list")
check(c0[2] == "inner(a, 2)", "then the CALL — not a bracket level, added on purpose")
check(c0[3] == "(a + inner(a, 2), 7)", "then the enclosing argument list")
check(c0[4] == "compute(a + inner(a, 2), 7)", "then the outer call")
check(c0[-1] == "<multiline>", "outermost is the whole file")
def spans(chain):
    out, node = [], chain
    while node:
        r = node["range"]
        out.append((r["start"]["line"], r["start"]["character"], r["end"]["line"], r["end"]["character"]))
        node = node.get("parent")
    return out
sp = spans(res[0])
ok = True
for a, b in zip(sp, sp[1:]):
    if not ((b[0], b[1]) <= (a[0], a[1]) and (a[2], a[3]) <= (b[2], b[3])): ok = False
check(ok, "every range strictly contains the one below it")
check(texts(res[1])[0] == "v", "the second position gets its own chain")
sys.exit(1 if failures else 0)
PY
rc=$?
if [ $rc -eq 0 ]; then ok "lsp selectionRange"; else bad "lsp selectionRange"; fi

# ---- incremental sync (textDocumentSync: 2): ranged edits ----
# The client sends deltas (a range + replacement text), not the whole doc.
# Apply two sequential ranged edits to the in-memory buffer and confirm a
# navigation request reflects the rebuilt document. Disk is never touched.
python3 - <<'PY'
import sys, json, subprocess
import os as _os, shutil as _sh; _ws = "/tmp/lsp_ws/incr_test"; _sh.rmtree(_ws, ignore_errors=True); _os.makedirs(_ws)
path = _ws + "/lsp_incr_test.scaly"
src  = "function alpha() returns int\n{\n    return 1\n}\n"
open(path, "w").write(src)             # disk has `alpha`; never rewritten
def frame(o):
    b=json.dumps(o).encode(); return ("Content-Length: %d\r\n\r\n"%len(b)).encode()+b
def change(version, edits):
    return frame({"jsonrpc":"2.0","method":"textDocument/didChange","params":{
        "textDocument":{"uri":"file://"+path,"version":version},
        "contentChanges":edits}})
inp  = frame({"jsonrpc":"2.0","id":1,"method":"initialize","params":{}})
inp += frame({"jsonrpc":"2.0","method":"initialized","params":{}})
inp += frame({"jsonrpc":"2.0","method":"textDocument/didOpen","params":{"textDocument":{
        "uri":"file://"+path,"languageId":"scaly","version":1,"text":src}}})
# rename `alpha` (line 0, chars 9..14) -> `beta_renamed`
inp += change(2, [{"range":{"start":{"line":0,"character":9},
                            "end":{"line":0,"character":14}}, "text":"beta_renamed"}])
inp += frame({"jsonrpc":"2.0","id":2,"method":"textDocument/documentSymbol",
              "params":{"textDocument":{"uri":"file://"+path}}})
# a second ranged edit applied ON TOP of the first: `1` -> `42`
inp += change(3, [{"range":{"start":{"line":2,"character":11},
                            "end":{"line":2,"character":12}}, "text":"42"}])
inp += frame({"jsonrpc":"2.0","id":3,"method":"textDocument/hover",
              "params":{"textDocument":{"uri":"file://"+path},"position":{"line":0,"character":11}}})
inp += frame({"jsonrpc":"2.0","id":9,"method":"shutdown"})
inp += frame({"jsonrpc":"2.0","method":"exit"})
out = subprocess.run(["/tmp/scalyls"], input=inp, stdout=subprocess.PIPE).stdout
frames, d = [], out
while d:
    i = d.find(b"\r\n\r\n")
    if i < 0: break
    n = int(d[:i].decode().split(":")[1].strip())
    frames.append(json.loads(d[i+4:i+4+n])); d = d[i+4+n:]
failures = 0
def check(cond, label):
    global failures
    print(("PASS  " if cond else "FAIL  ") + label)
    if not cond: failures += 1
sym = next((f for f in frames if f.get("id") == 2), None)
names = [s["name"] for s in ((sym or {}).get("result") or [])]
check(names == ["beta_renamed"], "ranged edit rebuilt the doc (alpha -> beta_renamed)")
hov = next((f for f in frames if f.get("id") == 3), None)
val = ((hov or {}).get("result") or {}).get("contents", {}).get("value")
check(val == "function beta_renamed() returns int",
      "second ranged edit applied on top (sequential)")
check(open(path).read() == src, "disk file untouched (edits are in-memory)")
sys.exit(1 if failures else 0)
PY
rc=$?
if [ $rc -eq 0 ]; then ok "lsp incremental sync"; else bad "lsp incremental sync"; fi

# ---- in-memory document store: unsaved edits are visible ----
# The store keeps the editor's latest text per uri so navigation requests
# (which carry only a uri) reflect UNSAVED edits instead of re-reading disk.
# Write one thing to disk, didOpen it, then didChange to DIFFERENT text that
# is NEVER written to disk. documentSymbol must report the edited (in-memory)
# outline. After didClose the entry is dropped and it falls back to disk.
python3 - <<'PY'
import sys, json, subprocess
import os as _os, shutil as _sh; _ws = "/tmp/lsp_ws/docstore_test"; _sh.rmtree(_ws, ignore_errors=True); _os.makedirs(_ws)
path = _ws + "/lsp_docstore_test.scaly"
disk   = "function on_disk() returns int\n{\n    return 1\n}\n"
edited = "function edited_only() returns int\n{\n    return 2\n}\n"
open(path, "w").write(disk)          # disk has `on_disk`; never rewritten
def frame(o):
    b=json.dumps(o).encode(); return ("Content-Length: %d\r\n\r\n"%len(b)).encode()+b
def docsym(idn):
    return frame({"jsonrpc":"2.0","id":idn,"method":"textDocument/documentSymbol",
                  "params":{"textDocument":{"uri":"file://"+path}}})
inp  = frame({"jsonrpc":"2.0","id":1,"method":"initialize","params":{}})
inp += frame({"jsonrpc":"2.0","method":"initialized","params":{}})
inp += frame({"jsonrpc":"2.0","method":"textDocument/didOpen","params":{"textDocument":{
        "uri":"file://"+path,"languageId":"scaly","version":1,"text":disk}}})
inp += frame({"jsonrpc":"2.0","method":"textDocument/didChange","params":{
        "textDocument":{"uri":"file://"+path,"version":2},
        "contentChanges":[{"text":edited}]}})   # unsaved edit, NOT on disk
inp += docsym(2)                                 # must reflect the edit
inp += frame({"jsonrpc":"2.0","method":"textDocument/didClose","params":{
        "textDocument":{"uri":"file://"+path}}})
inp += docsym(3)                                 # store dropped -> disk
inp += frame({"jsonrpc":"2.0","id":9,"method":"shutdown"})
inp += frame({"jsonrpc":"2.0","method":"exit"})
out = subprocess.run(["/tmp/scalyls"], input=inp, stdout=subprocess.PIPE).stdout
frames, d = [], out
while d:
    i = d.find(b"\r\n\r\n")
    if i < 0: break
    n = int(d[:i].decode().split(":")[1].strip())
    frames.append(json.loads(d[i+4:i+4+n])); d = d[i+4+n:]
failures = 0
def check(cond, label):
    global failures
    print(("PASS  " if cond else "FAIL  ") + label)
    if not cond: failures += 1
def names(idn):
    f = next((x for x in frames if x.get("id") == idn), None)
    return [s["name"] for s in ((f or {}).get("result") or [])]
check(names(2) == ["edited_only"], "documentSymbol reflects the unsaved edit (in-memory store)")
check(names(3) == ["on_disk"], "after didClose -> falls back to disk content")
sys.exit(1 if failures else 0)
PY
rc=$?
if [ $rc -eq 0 ]; then ok "lsp in-memory document store"; else bad "lsp in-memory document store"; fi

# ---- rename + prepareRename (lexical, single file) ----
# prepareRename returns the range of the identifier under the cursor;
# rename returns a WorkspaceEdit rewriting every whole-token occurrence of
# it to the new name. Exercises a multiply-called function (whole-token, so
# the param `a` is NOT renamed inside `add`), and the off-symbol null cases.
python3 - <<'PY'
import sys, json, subprocess
src = ("function add(a: int, b: int) returns int\n{\n    return a + b\n}\n\n"
       "function main() returns int\n{\n    return add(add(1, 2), 3)\n}\n")
import os as _os, shutil as _sh; _ws = "/tmp/lsp_ws/rename_test"; _sh.rmtree(_ws, ignore_errors=True); _os.makedirs(_ws)
path = _ws + "/lsp_rename_test.scaly"
open(path, "w").write(src)
def frame(o):
    b=json.dumps(o).encode(); return ("Content-Length: %d\r\n\r\n"%len(b)).encode()+b
def pr(idn, line, char):
    return frame({"jsonrpc":"2.0","id":idn,"method":"textDocument/prepareRename",
                  "params":{"textDocument":{"uri":"file://"+path},
                            "position":{"line":line,"character":char}}})
def rn(idn, line, char, name):
    return frame({"jsonrpc":"2.0","id":idn,"method":"textDocument/rename",
                  "params":{"textDocument":{"uri":"file://"+path},
                            "position":{"line":line,"character":char},"newName":name}})
inp  = frame({"jsonrpc":"2.0","id":1,"method":"initialize","params":{}})
inp += frame({"jsonrpc":"2.0","method":"initialized","params":{}})
inp += pr(2, 0, 10)         # prepareRename on `add` -> its range
inp += rn(3, 0, 10, "plus") # rename `add` -> `plus` (3 edits: decl + 2 calls)
inp += rn(4, 2, 11, "z")    # rename param `a` -> `z` (2 edits, NOT in `add`)
inp += pr(5, 1, 0)          # prepareRename on `{` -> null
inp += rn(6, 1, 0, "x")     # rename on `{` -> null
inp += frame({"jsonrpc":"2.0","id":9,"method":"shutdown"})
inp += frame({"jsonrpc":"2.0","method":"exit"})
out = subprocess.run(["/tmp/scalyls"], input=inp, stdout=subprocess.PIPE).stdout
frames, d = [], out
while d:
    i = d.find(b"\r\n\r\n")
    if i < 0: break
    n = int(d[:i].decode().split(":")[1].strip())
    frames.append(json.loads(d[i+4:i+4+n])); d = d[i+4+n:]
failures = 0
def check(cond, label):
    global failures
    print(("PASS  " if cond else "FAIL  ") + label)
    if not cond: failures += 1
caps = frames[0]["result"]["capabilities"]
rp = caps.get("renameProvider")
check(isinstance(rp, dict) and rp.get("prepareProvider") is True,
      "initialize advertises renameProvider with prepareProvider")
def res(idn):
    f = next((x for x in frames if x.get("id") == idn), None)
    return (f or {}).get("result")
# prepareRename -> the range of `add` (line 0, chars 9..12)
pre = res(2) or {}
check(pre.get("start",{}) == {"line":0,"character":9} and
      pre.get("end",{}) == {"line":0,"character":12}, "prepareRename -> identifier range")
# rename `add` -> WorkspaceEdit with 3 edits, all newText "plus"
edits = ((res(3) or {}).get("changes") or {}).get("file://"+path)
check(isinstance(edits, list) and len(edits) == 3, "rename `add` -> 3 TextEdits (decl + 2 calls)")
check(edits is not None and all(e["newText"] == "plus" for e in edits), "every edit carries the new name")
starts = sorted((e["range"]["start"]["line"], e["range"]["start"]["character"]) for e in (edits or []))
check(starts == [(0,9),(7,11),(7,15)], "edit ranges cover each whole-token occurrence")
# rename param `a` -> only 2 edits (whole-token: not the `a` inside `add`)
a_edits = ((res(4) or {}).get("changes") or {}).get("file://"+path)
check(isinstance(a_edits, list) and len(a_edits) == 2, "rename `a` -> 2 edits (not the `a` in `add`)")
check(res(5) is None, "prepareRename off any identifier -> null")
check(res(6) is None, "rename off any identifier -> null")
sys.exit(1 if failures else 0)
PY
rc=$?
if [ $rc -eq 0 ]; then ok "lsp rename"; else bad "lsp rename"; fi

# ---- scope-aware references / rename / prepareRename ----
# References and rename are lexical whole-token scans, but now (1) skip comments
# and string literals, and (2) confine a local/parameter to its declaring
# routine (a module/file-wide name still scans the whole file). prepareRename
# rejects a cursor on a keyword, in a comment, or in a string literal.
python3 - <<'PY'
import sys, json, subprocess
src = ("function alpha() returns int\n"                       # 0
       "{\n"                                                  # 1
       "    var x 1            ; mentions x in a comment\n"   # 2
       "    let y \"x inside a string literal\"\n"            # 3
       "    return x\n"                                       # 4
       "}\n"                                                  # 5
       "\n"                                                   # 6
       "function beta() returns int\n"                        # 7
       "{\n"                                                  # 8
       "    var x 2\n"                                        # 9
       "    return alpha() + x\n"                             # 10
       "}\n")                                                 # 11
import os as _os, shutil as _sh; _ws = "/tmp/lsp_ws/scope_test"; _sh.rmtree(_ws, ignore_errors=True); _os.makedirs(_ws)
path = _ws + "/lsp_scope_test.scaly"
open(path, "w").write(src)
def frame(o):
    b=json.dumps(o).encode(); return ("Content-Length: %d\r\n\r\n"%len(b)).encode()+b
def rf(idn, line, char):
    return frame({"jsonrpc":"2.0","id":idn,"method":"textDocument/references",
                  "params":{"textDocument":{"uri":"file://"+path},
                            "position":{"line":line,"character":char},
                            "context":{"includeDeclaration":True}}})
def pr(idn, line, char):
    return frame({"jsonrpc":"2.0","id":idn,"method":"textDocument/prepareRename",
                  "params":{"textDocument":{"uri":"file://"+path},
                            "position":{"line":line,"character":char}}})
def rn(idn, line, char, name):
    return frame({"jsonrpc":"2.0","id":idn,"method":"textDocument/rename",
                  "params":{"textDocument":{"uri":"file://"+path},
                            "position":{"line":line,"character":char},"newName":name}})
inp  = frame({"jsonrpc":"2.0","id":1,"method":"initialize","params":{}})
inp += frame({"jsonrpc":"2.0","method":"initialized","params":{}})
inp += rf(2, 2, 8)        # local x in alpha -> 2 (alpha only; not beta/comment/string)
inp += rf(3, 9, 8)        # local x in beta  -> 2 (beta only)
inp += rf(4, 0, 9)        # module-wide alpha -> 2 (decl + cross-routine call)
inp += rn(5, 2, 8, "q")   # rename local x in alpha -> 2 edits (alpha only)
inp += pr(6, 2, 28)       # prepareRename inside the comment -> null
inp += pr(7, 3, 14)       # prepareRename inside the string literal -> null
inp += pr(8, 0, 2)        # prepareRename on keyword `function` -> null
inp += pr(9, 2, 8)        # prepareRename on local x -> its range
inp += frame({"jsonrpc":"2.0","id":9,"method":"shutdown"})
inp += frame({"jsonrpc":"2.0","method":"exit"})
out = subprocess.run(["/tmp/scalyls"], input=inp, stdout=subprocess.PIPE).stdout
frames, d = [], out
while d:
    i = d.find(b"\r\n\r\n")
    if i < 0: break
    n = int(d[:i].decode().split(":")[1].strip())
    frames.append(json.loads(d[i+4:i+4+n])); d = d[i+4+n:]
failures = 0
def check(cond, label):
    global failures
    print(("PASS  " if cond else "FAIL  ") + label)
    if not cond: failures += 1
def res(idn):
    f = next((x for x in frames if x.get("id") == idn), None)
    return (f or {}).get("result")
def starts(idn):
    return sorted((x["range"]["start"]["line"], x["range"]["start"]["character"]) for x in (res(idn) or []))
# (1) local x in alpha: only alpha's two occurrences (decl line 2, use line 4);
#     NOT beta's x, NOT the `x` in the comment, NOT the `x` in the string.
check(starts(2) == [(2,8),(4,11)], "local x in alpha -> 2 refs (scoped; no comment/string/beta)")
# (2) local x in beta: only beta's two occurrences (decl line 9, use line 10).
check(starts(3) == [(9,8),(10,21)], "local x in beta -> 2 refs (not alpha's x)")
# (3) module-wide alpha still matches across routines (decl + the call in beta).
check(starts(4) == [(0,9),(10,11)], "module-wide alpha -> 2 refs across routines")
# rename a local renames only its own routine's occurrences.
ed = ((res(5) or {}).get("changes") or {}).get("file://"+path)
ed_starts = sorted((e["range"]["start"]["line"], e["range"]["start"]["character"]) for e in (ed or []))
check(ed_starts == [(2,8),(4,11)], "rename local x in alpha -> 2 edits (scoped to alpha)")
# prepareRename rejects comment / string / keyword.
check(res(6) is None, "prepareRename inside a comment -> null")
check(res(7) is None, "prepareRename inside a string literal -> null")
check(res(8) is None, "prepareRename on a keyword -> null")
check(res(9) == {"start":{"line":2,"character":8},"end":{"line":2,"character":9}},
      "prepareRename on a local identifier -> its range")
sys.exit(1 if failures else 0)
PY
rc=$?
if [ $rc -eq 0 ]; then ok "lsp scope-aware refs/rename"; else bad "lsp scope-aware refs/rename"; fi

# ---- cross-file references / rename ----
# A module/file-wide name (a function declared in one file, used in another) now
# resolves references AND renames across every .scaly file in the directory tree
# — each carrying its own uri. A local stays single-file. Uses a private mkdtemp
# dir so the dir-tree walk sees only these two files.
python3 - <<'PY'
import sys, json, subprocess, os, tempfile
d = tempfile.mkdtemp(prefix="lsp_xfile_")
lib = ("function compute(n: int) returns int\n"   # 0
       "{\n"                                       # 1
       "    var tmp 0          ; tmp here\n"       # 2
       "    return n + tmp\n"                      # 3
       "}\n")                                      # 4
main = ("function run() returns int\n"            # 0
        "{\n"                                      # 1
        "    var tmp 5\n"                          # 2
        "    return compute(tmp)\n"                # 3
        "}\n")                                     # 4
libp  = os.path.join(d, "lib.scaly")
mainp = os.path.join(d, "main.scaly")
open(libp, "w").write(lib); open(mainp, "w").write(main)
def frame(o):
    b=json.dumps(o).encode(); return ("Content-Length: %d\r\n\r\n"%len(b)).encode()+b
def rf(idn, p, line, char):
    return frame({"jsonrpc":"2.0","id":idn,"method":"textDocument/references",
                  "params":{"textDocument":{"uri":"file://"+p},
                            "position":{"line":line,"character":char},
                            "context":{"includeDeclaration":True}}})
def rn(idn, p, line, char, name):
    return frame({"jsonrpc":"2.0","id":idn,"method":"textDocument/rename",
                  "params":{"textDocument":{"uri":"file://"+p},
                            "position":{"line":line,"character":char},"newName":name}})
inp  = frame({"jsonrpc":"2.0","id":1,"method":"initialize","params":{}})
inp += frame({"jsonrpc":"2.0","method":"initialized","params":{}})
inp += rf(2, libp, 0, 9)         # references on `compute` (decl) -> decl (lib) + use (main)
inp += rf(3, libp, 2, 8)         # references on local `tmp` in lib -> lib only (2)
inp += rn(4, mainp, 3, 11, "calc")  # rename `compute` from main (used here) -> both files
inp += rn(5, mainp, 2, 8, "t2")     # rename local `tmp` in main -> main only
inp += frame({"jsonrpc":"2.0","id":9,"method":"shutdown"})
inp += frame({"jsonrpc":"2.0","method":"exit"})
out = subprocess.run(["/tmp/scalyls"], input=inp, stdout=subprocess.PIPE).stdout
frames, b = [], out
while b:
    i = b.find(b"\r\n\r\n")
    if i < 0: break
    n = int(b[:i].decode().split(":")[1].strip())
    frames.append(json.loads(b[i+4:i+4+n])); b = b[i+4+n:]
failures = 0
def check(cond, label):
    global failures
    print(("PASS  " if cond else "FAIL  ") + label)
    if not cond: failures += 1
def res(idn):
    f = next((x for x in frames if x.get("id") == idn), None)
    return (f or {}).get("result")
def files(idn):
    return sorted(set(os.path.basename(x["uri"]) for x in (res(idn) or [])))
# (1) references of a cross-file function span both files.
check(files(2) == ["lib.scaly","main.scaly"], "references of `compute` span lib + main")
check(len(res(2) or []) == 2, "`compute` -> 2 references (decl + cross-file use)")
# (2) a local does not leak across files.
check(files(3) == ["lib.scaly"], "local `tmp` references stay in lib.scaly")
check(len(res(3) or []) == 2, "local `tmp` -> 2 references (decl + use)")
# (3) rename of a cross-file name (invoked from the using file) edits both files.
ch = (res(4) or {}).get("changes") or {}
keys = sorted(os.path.basename(k) for k in ch.keys())
check(keys == ["lib.scaly","main.scaly"], "rename `compute` edits lib + main")
check(sum(len(v) for v in ch.values()) == 2, "rename `compute` -> 2 edits total")
# (4) renaming a local edits only its own file.
ch5 = (res(5) or {}).get("changes") or {}
check(sorted(os.path.basename(k) for k in ch5.keys()) == ["main.scaly"],
      "rename local `tmp` stays in main.scaly")
check(sum(len(v) for v in ch5.values()) == 2, "rename local `tmp` -> 2 edits")
sys.exit(1 if failures else 0)
PY
rc=$?
if [ $rc -eq 0 ]; then ok "lsp cross-file refs/rename"; else bad "lsp cross-file refs/rename"; fi

# ---- workspace-root references / rename (across the dir boundary) ----
# Step 36's walk was rooted at the EDITED file's directory tree, so a symbol
# declared in a SUBDIRECTORY file and used in a PARENT-directory file was
# missed. With the workspace root (captured from initialize.rootUri) threaded
# into the references/rename frames, the walk fans out from the real root and
# finds the cross-directory use. Without a root it falls back to the dir tree
# (and so misses it) — proving the root is what closes the gap.
python3 - <<'PY'
import sys, json, subprocess, os, tempfile
d = tempfile.mkdtemp(prefix="lsp_wsroot_")
sub = os.path.join(d, "sub"); os.makedirs(sub)
# compute() is DECLARED in the subdirectory, USED in the parent-dir main.scaly.
lib = ("function compute(n: int) returns int\n"   # 0  decl at char 9
       "{\n"                                       # 1
       "    var tmp 0\n"                           # 2
       "    return n + tmp\n"                      # 3
       "}\n")                                      # 4
main = ("function run() returns int\n"            # 0
        "{\n"                                      # 1
        "    var tmp 5\n"                          # 2
        "    return compute(tmp)\n"                # 3  use of compute at char 11
        "}\n")                                     # 4
libp  = os.path.join(sub, "lib.scaly")
mainp = os.path.join(d, "main.scaly")
open(libp, "w").write(lib); open(mainp, "w").write(main)
def frame(o):
    b=json.dumps(o).encode(); return ("Content-Length: %d\r\n\r\n"%len(b)).encode()+b
def rf(idn, p, line, char):
    return frame({"jsonrpc":"2.0","id":idn,"method":"textDocument/references",
                  "params":{"textDocument":{"uri":"file://"+p},
                            "position":{"line":line,"character":char},
                            "context":{"includeDeclaration":True}}})
def rn(idn, p, line, char, name):
    return frame({"jsonrpc":"2.0","id":idn,"method":"textDocument/rename",
                  "params":{"textDocument":{"uri":"file://"+p},
                            "position":{"line":line,"character":char},"newName":name}})
def session(initparams):
    inp  = frame({"jsonrpc":"2.0","id":1,"method":"initialize","params":initparams})
    inp += frame({"jsonrpc":"2.0","method":"initialized","params":{}})
    inp += rf(2, libp, 0, 9)            # references on `compute` (decl, in sub/)
    inp += rn(3, mainp, 3, 11, "calc")  # rename `compute` from the parent-dir use
    inp += frame({"jsonrpc":"2.0","id":9,"method":"shutdown"})
    inp += frame({"jsonrpc":"2.0","method":"exit"})
    out = subprocess.run(["/tmp/scalyls"], input=inp, stdout=subprocess.PIPE).stdout
    frames, b = [], out
    while b:
        i = b.find(b"\r\n\r\n")
        if i < 0: break
        n = int(b[:i].decode().split(":")[1].strip())
        frames.append(json.loads(b[i+4:i+4+n])); b = b[i+4+n:]
    return frames
def res(frames, idn):
    f = next((x for x in frames if x.get("id") == idn), None)
    return (f or {}).get("result")
def reffiles(frames, idn):
    return sorted(set(os.path.basename(x["uri"]) for x in (res(frames, idn) or [])))
def renkeys(frames, idn):
    ch = (res(frames, idn) or {}).get("changes") or {}
    return sorted(os.path.basename(k) for k in ch.keys())
failures = 0
def check(cond, label):
    global failures
    print(("PASS  " if cond else "FAIL  ") + label)
    if not cond: failures += 1

# (A) rootUri set -> the walk fans out from the root and crosses the dir boundary.
fa = session({"rootUri":"file://"+d})
check(reffiles(fa, 2) == ["lib.scaly","main.scaly"],
      "root set: references of `compute` span sub/lib + parent main")
check(len(res(fa, 2) or []) == 2, "root set: `compute` -> 2 references across dirs")
check(renkeys(fa, 3) == ["lib.scaly","main.scaly"],
      "root set: rename `compute` edits sub/lib + parent main")
check(sum(len(v) for v in ((res(fa,3) or {}).get('changes') or {}).values()) == 2,
      "root set: rename `compute` -> 2 edits total")

# (B) no rootUri -> dir-tree fallback (Step 36): the request from sub/lib.scaly
#     only walks sub/, so the parent-dir use is NOT seen.
fb = session({})
check(reffiles(fb, 2) == ["lib.scaly"],
      "no root: references of `compute` stay in the file's own dir tree (sub/)")
check(len(res(fb, 2) or []) == 1, "no root: `compute` -> 1 reference (decl only)")
sys.exit(1 if failures else 0)
PY
rc=$?
if [ $rc -eq 0 ]; then ok "lsp workspace-root refs/rename"; else bad "lsp workspace-root refs/rename"; fi

# ---- shadowed-local rename (cross-file scope safety, Step 38) ----
# A module-wide name is scanned across every file in the workspace. But another
# file may bind that name LOCALLY (a `let`/`var` local or a parameter), and
# every occurrence inside such a routine refers to the local, not the global.
# The cross-file scan now SKIPS any routine that binds the name locally, so a
# global rename never corrupts an unrelated same-named local. Detection is
# purely lexical (routine boundaries by brace matching, bindings by token scan)
# — no parse, so it stays cheap when fanned out over many files.
python3 - <<'PY'
import sys, json, subprocess, os, tempfile
d = tempfile.mkdtemp(prefix="lsp_shadow_")
# File A declares the module-wide function `compute` and uses it.
a = ("function compute(n: int) returns int\n"      # 0  decl, char 9
     "{\n"                                          # 1
     "    return n + 1\n"                           # 2
     "}\n"                                          # 3
     "function use_a(m: int) returns int\n"         # 4
     "{\n"                                          # 5
     "    return compute(m)\n"                      # 6  real use, char 11
     "}\n")                                         # 7
# File B has (1) an unrelated LOCAL `var compute` in one routine, (2) a routine
# whose PARAMETER is named `compute`, and (3) a real call to the global compute.
b = ("function with_local(x: int) returns int\n"   # 0
     "{\n"                                          # 1
     "    var compute 7\n"                          # 2  LOCAL decl, char 8 -> must NOT rename
     "    return compute + x\n"                     # 3  local use, char 11 -> must NOT rename
     "}\n"                                          # 4
     "function with_param(compute: int) returns int\n"  # 5  PARAM, char 20 -> must NOT rename
     "{\n"                                          # 6
     "    return compute * 2\n"                     # 7  param use, char 11 -> must NOT rename
     "}\n"                                          # 8
     "function with_call(z: int) returns int\n"     # 9
     "{\n"                                          # 10
     "    return compute(z)\n"                      # 11 real use, char 11 -> MUST rename
     "}\n")                                         # 12
ap = os.path.join(d, "a.scaly"); bp = os.path.join(d, "b.scaly")
open(ap,"w").write(a); open(bp,"w").write(b)
def frame(o):
    s=json.dumps(o).encode(); return ("Content-Length: %d\r\n\r\n"%len(s)).encode()+s
def rf(idn, p, line, char):
    return frame({"jsonrpc":"2.0","id":idn,"method":"textDocument/references",
                  "params":{"textDocument":{"uri":"file://"+p},
                            "position":{"line":line,"character":char},
                            "context":{"includeDeclaration":True}}})
def rn(idn, p, line, char, name):
    return frame({"jsonrpc":"2.0","id":idn,"method":"textDocument/rename",
                  "params":{"textDocument":{"uri":"file://"+p},
                            "position":{"line":line,"character":char},"newName":name}})
inp  = frame({"jsonrpc":"2.0","id":1,"method":"initialize","params":{"rootUri":"file://"+d}})
inp += frame({"jsonrpc":"2.0","method":"initialized","params":{}})
inp += rf(2, ap, 0, 9)               # references on `compute` (decl in A)
inp += rn(3, ap, 0, 9, "calc")       # rename `compute` (the global) from A
inp += rn(4, bp, 0, 9, "renamed_local")  # rename the LOCAL `with_local` fn -> sanity: a real local rename still works
inp += frame({"jsonrpc":"2.0","id":9,"method":"shutdown"})
inp += frame({"jsonrpc":"2.0","method":"exit"})
out = subprocess.run(["/tmp/scalyls"], input=inp, stdout=subprocess.PIPE, stderr=subprocess.DEVNULL).stdout
frames, raw = [], out
while raw:
    i = raw.find(b"\r\n\r\n")
    if i < 0: break
    n = int(raw[:i].decode().split(":")[1].strip())
    frames.append(json.loads(raw[i+4:i+4+n])); raw = raw[i+4+n:]
failures = 0
def check(cond, label):
    global failures
    print(("PASS  " if cond else "FAIL  ") + label)
    if not cond: failures += 1
def res(idn):
    f = next((x for x in frames if x.get("id") == idn), None)
    return (f or {}).get("result")
def reflocs(idn):
    return sorted((os.path.basename(x["uri"]), x["range"]["start"]["line"], x["range"]["start"]["character"])
                  for x in (res(idn) or []))

# (1) references of the global `compute`: decl + the two REAL uses (a.scaly:6,
#     b.scaly:11). The local var, its use, the param, and the param use are all
#     EXCLUDED.
locs = reflocs(2)
check(locs == [("a.scaly",0,9), ("a.scaly",6,11), ("b.scaly",11,11)],
      "references of global `compute` = decl + 2 real uses, no shadowed local/param")

# (2) rename the global `compute` -> edits ONLY the decl + the two real uses.
ch = (res(3) or {}).get("changes") or {}
edits = sorted((os.path.basename(u), e["range"]["start"]["line"], e["range"]["start"]["character"])
               for u,vs in ch.items() for e in vs)
check(edits == [("a.scaly",0,9), ("a.scaly",6,11), ("b.scaly",11,11)],
      "rename global `compute` edits decl + 2 real uses only (local/param untouched)")
# explicit: the shadowed sites are NOT in the edit set.
shadow_sites = {("b.scaly",2,8),("b.scaly",3,11),("b.scaly",5,20),("b.scaly",7,11)}
check(not (set(edits) & shadow_sites),
      "no edit touches the same-named local (b:2,3) or parameter (b:5,7)")

# (3) sanity: a genuine local-vs-global is independent — renaming the function
#     `with_local` (a real module-wide name) still edits its own decl.
ch4 = (res(4) or {}).get("changes") or {}
n4 = sum(len(v) for v in ch4.values())
check(n4 == 1, "control: rename function `with_local` -> 1 edit (its decl)")
sys.exit(1 if failures else 0)
PY
rc=$?
if [ $rc -eq 0 ]; then ok "lsp shadowed-local rename"; else bad "lsp shadowed-local rename"; fi

# ---- context-aware completion (member-after-`.`, lexical) ----
# When a `.` precedes the cursor and its receiver names a declared concept,
# completion returns only that concept's members. A struct receiver yields
# its methods; a union receiver yields its variants; a variable receiver is
# resolved to its declared type (here `p: Point` -> Point's members); no dot
# falls back to the flat all-names list.
python3 - <<'PY'
import sys, json, subprocess
src = ("function add(a: int, b: int) returns int\n{\n    return a + b\n}\n\n"
       "mutable counter: int 0\n\n"
       "define Point\n(\n    x: int\n)\n{\n"
       "    function get_x(this: Point) returns int\n    {\n        return x\n    }\n}\n\n"
       "define Shape union (\n    Circle: int\n    Square: int\n)\n\n"
       "function trigger(p: Point) returns int\n{\n    let q Point.get_x(p)\n"
       "    let s Shape.Circle\n    return q + p.get_x()\n}\n")
import os as _os, shutil as _sh; _ws = "/tmp/lsp_ws/ctxcompl_test"; _sh.rmtree(_ws, ignore_errors=True); _os.makedirs(_ws)
path = _ws + "/lsp_ctxcompl_test.scaly"
open(path, "w").write(src)
def linecol(idx):
    pre = src[:idx]; return pre.count("\n"), idx - (pre.rfind("\n")+1)
def after_dot(needle, recv_len):
    idx = src.index(needle) + recv_len            # offset of the dot
    l, c = linecol(idx); return l, c + 1          # cursor right after the dot
def frame(o):
    b=json.dumps(o).encode(); return ("Content-Length: %d\r\n\r\n"%len(b)).encode()+b
def comp(idn, line, char):
    return frame({"jsonrpc":"2.0","id":idn,"method":"textDocument/completion",
                  "params":{"textDocument":{"uri":"file://"+path},
                            "position":{"line":line,"character":char}}})
pl, pc = after_dot("Point.get_x(p)", 5)   # struct receiver
sl, sc = after_dot("Shape.Circle", 5)     # union receiver
vl, vc = after_dot("p.get_x()", 1)        # variable receiver (param p: Point)
inp  = frame({"jsonrpc":"2.0","id":1,"method":"initialize","params":{}})
inp += frame({"jsonrpc":"2.0","method":"initialized","params":{}})
inp += comp(2, pl, pc)
inp += comp(3, sl, sc)
inp += comp(4, vl, vc)
inp += comp(5, 2, 4)                       # no dot -> flat
inp += frame({"jsonrpc":"2.0","id":9,"method":"shutdown"})
inp += frame({"jsonrpc":"2.0","method":"exit"})
out = subprocess.run(["/tmp/scalyls"], input=inp, stdout=subprocess.PIPE).stdout
frames, d = [], out
while d:
    i = d.find(b"\r\n\r\n")
    if i < 0: break
    n = int(d[:i].decode().split(":")[1].strip())
    frames.append(json.loads(d[i+4:i+4+n])); d = d[i+4+n:]
failures = 0
def check(cond, label):
    global failures
    print(("PASS  " if cond else "FAIL  ") + label)
    if not cond: failures += 1
def res(idn):
    f = next((x for x in frames if x.get("id") == idn), None)
    return (f or {}).get("result")
def labels(idn):
    return [it["label"] for it in (res(idn) or [])]
def kinds(idn):
    return {it["label"]: it["kind"] for it in (res(idn) or [])}
check(labels(2) == ["x","get_x"], "`Point.` -> the struct's FIELDS then its members")
check(kinds(2).get("get_x") == 2, "member kept its CompletionItemKind (Method=2)")
check(kinds(2).get("x") == 5, "field carries CompletionItemKind Field=5")
check(labels(3) == ["Circle","Square"], "`Shape.` -> only the union's variants")
check(kinds(3).get("Circle") == 20, "variant kept its CompletionItemKind (EnumMember=20)")
flat = ["add","counter","Point","get_x","Shape","Circle","Square","trigger"]
check(labels(4) == ["x","get_x"], "variable receiver `p.` (p: Point) -> Point's members")
check(labels(5) == flat, "no dot -> flat all-names list (unchanged)")
sys.exit(1 if failures else 0)
PY
rc=$?
if [ $rc -eq 0 ]; then ok "lsp context-aware completion"; else bad "lsp context-aware completion"; fi

# ---- `this.` inside a PACKAGE MODULE (the case the planner cannot reach) ----
# `this` used to have no lexical resolution at all: it was answered only by the
# planner-backed case, and a file that BELONGS TO A PACKAGE does not plan on its
# own — every sibling module's name is absent, which is why diagnostics plan the
# package ROOT instead. So in this tree's own sources `this.` fell through to the
# FLAT all-names list, and paid the session's first plan (~6 s) to get there.
#
# Asserted on a real tree file (line found by CONTENT): the enclosing concept's
# fields AND methods must be there, and `fill_zeros_u32` — a FILE-LEVEL function
# of the same file, i.e. the tell-tale of the flat list — must not be.
SCALY_HOME="$(pwd)" python3 - <<'PY'
import sys, json, subprocess, os

def frame(o):
    b = json.dumps(o).encode()
    return ("Content-Length: %d\r\n\r\n" % len(b)).encode() + b

path = os.path.join(os.getcwd(), "packages/opensp/0.1.0/opensp/ContentState.scaly")
uri  = "file://" + path
doc  = open(path).read()
lines = doc.split("\n")
ln = next(i for i, l in enumerate(lines) if l.strip() == "if this.free_count = 0")
col = lines[ln].index("this.") + len("this.")

inp  = frame({"jsonrpc":"2.0","id":1,"method":"initialize","params":{}})
inp += frame({"jsonrpc":"2.0","method":"initialized","params":{}})
inp += frame({"jsonrpc":"2.0","method":"textDocument/didOpen","params":{"textDocument":{
        "uri":uri,"languageId":"scaly","version":1,"text":doc}}})
inp += frame({"jsonrpc":"2.0","id":2,"method":"textDocument/completion","params":{
        "textDocument":{"uri":uri},"position":{"line":ln,"character":col}}})
inp += frame({"jsonrpc":"2.0","id":9,"method":"shutdown"})
inp += frame({"jsonrpc":"2.0","method":"exit"})

out = subprocess.run(["/tmp/scalyls"], input=inp, stdout=subprocess.PIPE).stdout
frames, d = [], out
while d:
    i = d.find(b"\r\n\r\n")
    if i < 0: break
    n = int(d[:i].decode().split(":")[1].strip())
    frames.append(json.loads(d[i+4:i+4+n])); d = d[i+4+n:]

failures = 0
def check(cond, label):
    global failures
    print(("PASS  " if cond else "FAIL  ") + label)
    if not cond: failures += 1

r = next((f for f in frames if f.get("id") == 2), None)
items = (r or {}).get("result") or []
by = {it["label"]: it["kind"] for it in items}
check(by.get("free_count") == 5, "`this.` in a package module offers a FIELD (Field=5)")
check(by.get("acquire_element") == 2, "`this.` offers a METHOD of the same concept (Method=2)")
check("fill_zeros_u32" not in by, "`this.` is NOT the flat file list (no file-level function)")
sys.exit(1 if failures else 0)
PY
rc=$?
if [ $rc -eq 0 ]; then ok "lsp this-receiver in a package module"; else bad "lsp this-receiver in a package module"; fi

# ---- mid-edit: an unfinished line must not take the whole file down --------
# Every walk in symbols.scaly starts with parse_program# and answers ""/"[]" on
# Error, so ONE half-typed line killed every parse-based answer in the document —
# and typing is exactly when they are wanted. Measured with `set this.` typed
# into a method body: documentSymbol 0, foldingRange 0, hover null, definition
# null, completion 0, on healthy lines far from the edit too.
#
# symbols.repaired_source# blanks the line the parse error points at (bytes ->
# spaces, so every offset still indexes the same byte) and re-parses. The last
# two checks are the guards that matter: completion must still see the RAW line
# (the receiver being typed lives on the very line the repair blanks), and the
# repair must not cost the diagnostics their parse error nor add anything to it —
# this fixture has exactly one thing wrong with it, so exactly one is reported.
# The group below drives the other half: a file that is broken AND has a real
# semantic error elsewhere must report both.
python3 - <<'PY'
import sys, json, subprocess, os, select, time
src = ("define Point\n(\n    x: int\n)\n{\n"
       "    function get_x(this: Point) returns int\n    {\n"
       "        set this.\n"                     # line 7: unfinished
       "        return x\n    }\n}\n")
import os as _os, shutil as _sh; _ws = "/tmp/lsp_ws/midedit_test"; _sh.rmtree(_ws, ignore_errors=True); _os.makedirs(_ws)
path = _ws + "/lsp_midedit_test.scaly"
open(path, "w").write(src)
uri = "file://" + path
def frame(o):
    b=json.dumps(o).encode(); return ("Content-Length: %d\r\n\r\n"%len(b)).encode()+b
lines = src.split("\n")
bl = next(i for i, l in enumerate(lines) if l.strip() == "set this.")
bc = len(lines[bl])                                  # cursor right after the dot
gl = next(i for i, l in enumerate(lines) if "function get_x" in l)
gc = lines[gl].index("get_x") + 1

# Interactive, so the deferred diagnostics run is observed: server.process_one#
# holds an owed analysis back while more client input is pending, and `exit` ends
# the process — a single batch would therefore never show it.
p = subprocess.Popen(["/tmp/scalyls"], stdin=subprocess.PIPE, stdout=subprocess.PIPE, bufsize=0)
fd = p.stdout.fileno()
buf = bytearray()
def next_frame(timeout=20.0):
    deadline = time.time() + timeout
    while True:
        i = buf.find(b"\r\n\r\n")
        if i >= 0:
            n = int(bytes(buf[:i]).decode().split(":")[1].strip())
            if len(buf) >= i + 4 + n:
                body = bytes(buf[i+4:i+4+n]); del buf[:i+4+n]
                return json.loads(body)
        left = deadline - time.time()
        if left <= 0: return None
        if not select.select([fd], [], [], left)[0]: return None
        chunk = os.read(fd, 65536)
        if not chunk: return None
        buf.extend(chunk)
def send(o): p.stdin.write(frame(o))

frames = []
send({"jsonrpc":"2.0","id":1,"method":"initialize","params":{}})
frames.append(next_frame())
send({"jsonrpc":"2.0","method":"initialized","params":{}})
send({"jsonrpc":"2.0","method":"textDocument/didOpen","params":{"textDocument":{
        "uri":uri,"languageId":"scaly","version":1,"text":src}}})
frames.append(next_frame())                      # the analysis, once the queue drained
for idn, method, params in [
        (2, "textDocument/documentSymbol", {"textDocument":{"uri":uri}}),
        (3, "textDocument/foldingRange",   {"textDocument":{"uri":uri}}),
        (4, "textDocument/hover",          {"textDocument":{"uri":uri},
                                            "position":{"line":gl,"character":gc}}),
        (5, "textDocument/completion",     {"textDocument":{"uri":uri},
                                            "position":{"line":bl,"character":bc}})]:
    send({"jsonrpc":"2.0","id":idn,"method":method,"params":params})
    frames.append(next_frame())
send({"jsonrpc":"2.0","method":"exit"})

failures = 0
def check(cond, label):
    global failures
    print(("PASS  " if cond else "FAIL  ") + label)
    if not cond: failures += 1
def res(idn):
    f = next((x for x in frames if x.get("id") == idn), None)
    return (f or {}).get("result")

syms = res(2) or []
check([s["name"] for s in syms] == ["Point"], "documentSymbol survives an unfinished line")
check(len(res(3) or []) > 0, "foldingRange survives an unfinished line")
hv = res(4)
check(hv is not None and hv.get("contents", {}).get("value") == "method get_x(this: Point) returns int",
      "hover survives an unfinished line")
labels = [it["label"] for it in (res(5) or [])]
check(labels == ["x","get_x"], "completion at `set this.|` -> the enclosing concept's members")
diags = next((f["params"]["diagnostics"] for f in frames
              if f.get("method") == "textDocument/publishDiagnostics"), None)
check(diags is not None and len(diags) == 1,
      "diagnostics REPORT the parse error and invent nothing beside it")
sys.exit(1 if failures else 0)
PY
rc=$?
if [ $rc -eq 0 ]; then ok "lsp mid-edit parse repair"; else bad "lsp mid-edit parse repair"; fi

# ---- a parse error must not hide the file's SEMANTICS ----------------------
# A parse error used to END the report: diagnostics.publish_diagnostics# emitted
# the one parser message and returned, so no modeler/planner ran. Mid-edit is the
# normal state of a buffer, which made every type error in the document invisible
# until it parsed again — the single biggest gap in the live feel, and measured
# 2026-08-12 as exactly 1 diagnostic for a file with a broken line AND a missing
# function in it.
#
# The semantic half now runs on two routes, and the tests below are one each,
# because only one of them involves a guess:
#
#   * PACKAGE MEMBER — planned through its root, which the modeler reads from
#     DISK, so the broken buffer never enters that analysis. No repair, nothing
#     to filter. Every file of this tree takes this route, which is why the
#     fixture is a real (tiny) two-file package rather than a lone document.
#   * STANDALONE — the buffer is the only source, so it is analysed through
#     symbols.repaired_source#'s line blanking. THAT is a guess.
#
# The third check is the one that matters most, and it caught a real defect in
# the first draft of the filter: blanking a line can DELETE A DECLARATION, and
# then every use of that name reports `function not found` — about a name the
# reader can see on screen. An invented diagnostic is worse than a missing one.
# symbols.trust_repaired_diagnostic# drops those; the draft additionally required
# the name to be GONE from the repaired text, which re-admitted the whole class
# (a blanked declaration's name survives at its use sites) and is why that
# refinement is refuted in the function's own comment.
SCALY_HOME="$(pwd)" python3 - <<'PY'
import sys, json, subprocess, os, select, time, shutil

def frame(o):
    b = json.dumps(o).encode()
    return ("Content-Length: %d\r\n\r\n" % len(b)).encode() + b

# Open one document and return the diagnostics of the analysis it triggers.
# Interactive (not one batch) because server.process_one# defers the owed
# analysis while client input is pending, and `exit` would end the process.
def diagnose(uri, text):
    p = subprocess.Popen(["/tmp/scalyls"], stdin=subprocess.PIPE,
                         stdout=subprocess.PIPE, bufsize=0)
    fd = p.stdout.fileno()
    buf = bytearray()
    def next_frame(timeout=60.0):
        deadline = time.time() + timeout
        while True:
            i = buf.find(b"\r\n\r\n")
            if i >= 0:
                n = int(bytes(buf[:i]).decode().split(":")[1].strip())
                if len(buf) >= i + 4 + n:
                    body = bytes(buf[i+4:i+4+n]); del buf[:i+4+n]
                    return json.loads(body)
            left = deadline - time.time()
            if left <= 0: return None
            if not select.select([fd], [], [], left)[0]: return None
            chunk = os.read(fd, 65536)
            if not chunk: return None
            buf.extend(chunk)
    p.stdin.write(frame({"jsonrpc":"2.0","id":1,"method":"initialize","params":{}}))
    next_frame()
    p.stdin.write(frame({"jsonrpc":"2.0","method":"initialized","params":{}}))
    p.stdin.write(frame({"jsonrpc":"2.0","method":"textDocument/didOpen","params":{
            "textDocument":{"uri":uri,"languageId":"scaly","version":1,"text":text}}}))
    f = next_frame()
    p.stdin.write(frame({"jsonrpc":"2.0","method":"exit"}))
    p.wait()
    return (f or {}).get("params", {}).get("diagnostics")

failures = 0
def check(cond, label):
    global failures
    print(("PASS  " if cond else "FAIL  ") + label)
    if not cond: failures += 1

def at(diags, line):
    return [d["message"] for d in diags if d["range"]["start"]["line"] == line]

# ---- standalone: parse error in the middle, missing function after it ----
src = ("function helper() returns int\n"
       "{\n"
       "    return 7\n"
       "}\n"
       "\n"
       "function broken() returns int\n"
       "{\n"
       "    set this.\n"                       # line 7: unfinished
       "    return 1\n"
       "}\n"
       "\n"
       "function later() returns int\n"
       "{\n"
       "    return nope()\n"                   # line 13: semantic error
       "}\n")
ws = "/tmp/lsp_ws/pasterror_standalone"
shutil.rmtree(ws, ignore_errors=True); os.makedirs(ws)
path = ws + "/lsp_pasterror_standalone.scaly"
open(path, "w").write(src)
d = diagnose("file://" + path, src) or []
check(any("expected" in m for m in at(d, 7)),
      "standalone: the parse error is still reported, on its own line")
check(any("nope" in m for m in at(d, 13)),
      "standalone: and the planner diagnostic PAST it is reported too")

# ---- package member: the semantic half comes from DISK ----
root_dir = "/tmp/lsp_ws/pasterror_pkg"
shutil.rmtree(root_dir, ignore_errors=True)
os.makedirs(root_dir + "/pkgroot")
open(root_dir + "/pkgroot.scaly", "w").write("define pkgroot\n{\n    module member\n}\n")
member = root_dir + "/pkgroot/member.scaly"
disk = ("define member\n{\n"
        "    function calls_missing() returns int\n"
        "    {\n"
        "        return no_such_function()\n"   # line 4 of the SAVED file
        "    }\n"
        "    function healthy() returns int\n"
        "    {\n"
        "        return 3\n"
        "    }\n}\n")
open(member, "w").write(disk)
lines = disk.split("\n")
lines.insert(8, "        set this.")            # buffer-only broken line, line 8
d = diagnose("file://" + member, "\n".join(lines)) or []
check(any("expected" in m for m in at(d, 8)),
      "package member: the parse error comes from the live BUFFER")
check(any("no_such_function" in m for m in at(d, 4)),
      "package member: the semantic diagnostic comes from the package root on DISK")

# ---- the guard: a blanked DECLARATION must not invent a diagnostic ----
src = ("function helper(\n"                     # line 0: broken header
       "{\n"
       "    return 1\n"
       "}\n"
       "\n"
       "function g() returns int\n"
       "{\n"
       "    return helper()\n"                  # line 7: helper IS declared above
       "}\n")
ws = "/tmp/lsp_ws/pasterror_invented"
shutil.rmtree(ws, ignore_errors=True); os.makedirs(ws)
path = ws + "/lsp_pasterror_invented.scaly"
open(path, "w").write(src)
d = diagnose("file://" + path, src) or []
# since 2026-09-24 an expected-token error names the token the parse stopped
# at: here the `{` on line 1, where the header's `)` was due
check(len(at(d, 0)) + len(at(d, 1)) == 1, "invented-guard: the broken header is reported")
check(at(d, 7) == [],
      "invented-guard: the call to the name the repair BLANKED is not reported")
sys.exit(1 if failures else 0)
PY
rc=$?
if [ $rc -eq 0 ]; then ok "lsp diagnostics past a parse error"; else bad "lsp diagnostics past a parse error"; fi

# ---- diagnostics for the WHOLE package root, not just the open file -------
# A package member is planned through its ROOT, so one analysis answers for
# every module of that root. The answer for every file but the open one used to
# be computed and discarded; scalyls.diagnostics' header has the reasoning and
# server.write_notifications# the transport. What is checked here:
#
#   FAN-OUT     a diagnostic in a file that is NOT open is reported.
#   COORDINATES it is reported at ITS OWN line. This is the sharp one: the
#               standalone (program-root) route had no file filter at all, so a
#               module's byte offset was rendered against the OPEN buffer and
#               the squiggle landed under an unrelated line of a file that is
#               perfectly fine. Measured before the fix on this very fixture:
#               the module's error appeared on line 8 of the root.
#   CLEARING    a clean sibling is reported with an EMPTY array, which is what
#               removes a squiggle the user has just fixed. No server-side state
#               backs this, so it is the mechanism, not a nicety.
#   BOUND       nothing outside the root's own subtree is published into. The
#               modeler attaches the PRELUDE as a sub-module of the main module,
#               so an unbounded walk reports into packages/scaly/.../prelude.scaly
#               — another root's file, whose diagnostics this analysis has no
#               business clearing. The first version did exactly that.
#   BOTH ROUTES the package-member route (planned via package_root#) and the
#               program-root route (planned directly) reach the fan-out through
#               different code and are checked separately.
{ SCALY_HOME="$(pwd)" python3 - <<'PY'
import sys, json, subprocess, os, select, time, shutil

def frame(o):
    b = json.dumps(o).encode()
    return ("Content-Length: %d\r\n\r\n" % len(b)).encode() + b

# A server session that collects EVERY publishDiagnostics notification, keyed by
# uri. One analysis now answers with a list of them, so a helper that reads a
# single frame (diagnose# above) cannot see this feature at all.
class Session:
    def __init__(self):
        self.p = subprocess.Popen(["/tmp/scalyls"], stdin=subprocess.PIPE,
                                  stdout=subprocess.PIPE, bufsize=0)
        self.buf = bytearray()
        self.send({"jsonrpc":"2.0","id":1,"method":"initialize","params":{}})
        self.next_frame()
        self.send({"jsonrpc":"2.0","method":"initialized","params":{}})

    def send(self, o):
        self.p.stdin.write(frame(o)); self.p.stdin.flush()

    def next_frame(self, timeout=60.0):
        fd = self.p.stdout.fileno()
        deadline = time.time() + timeout
        while True:
            i = self.buf.find(b"\r\n\r\n")
            if i >= 0:
                n = int(bytes(self.buf[:i]).decode().split(":")[1].strip())
                if len(self.buf) >= i + 4 + n:
                    body = bytes(self.buf[i+4:i+4+n]); del self.buf[:i+4+n]
                    return json.loads(body)
            left = deadline - time.time()
            if left <= 0: return None
            if not select.select([fd], [], [], left)[0]: return None
            chunk = os.read(fd, 65536)
            if not chunk: return None
            self.buf.extend(chunk)

    # Every notification of the analysis an edit triggers: read until the first
    # one arrives (the analysis is slow), then until the stream goes quiet (the
    # rest arrive together, measured well under a millisecond apart).
    def collect(self):
        out = {}
        f = self.next_frame(60.0)
        while f is not None:
            if f.get("method") == "textDocument/publishDiagnostics":
                pr = f["params"]
                out[pr["uri"]] = pr["diagnostics"]
            f = self.next_frame(2.0)
        return out

    def open(self, path):
        self.send({"jsonrpc":"2.0","method":"textDocument/didOpen","params":{
            "textDocument":{"uri":"file://"+path,"languageId":"scaly",
                            "version":1,"text":open(path).read()}}})
        return self.collect()

    def touch(self, path, version):
        self.send({"jsonrpc":"2.0","method":"textDocument/didChange","params":{
            "textDocument":{"uri":"file://"+path,"version":version},
            "contentChanges":[{"text":open(path).read()}]}})
        return self.collect()

    def close(self):
        self.send({"jsonrpc":"2.0","method":"exit"}); self.p.wait()

failures = 0
def check(cond, label):
    global failures
    print(("PASS  " if cond else "FAIL  ") + label)
    if not cond: failures += 1

# A program root with two modules, one of them broken. The call is what makes
# the module reachable: the planner is DEMAND-DRIVEN, and a routine nothing
# calls is never planned in a sibling exactly as in the open file.
ws = "/tmp/lsp_ws/root_diagnostics"
shutil.rmtree(ws, ignore_errors=True); os.makedirs(ws + "/app")
open(ws + "/app.scaly", "w").write(
    "define app\n{\n    module good\n    module bad\n}\n"
    "\n"
    "use app.bad.faulty\n"
    "\n"
    "let v faulty.boom()\n")
BROKEN = ("define faulty\n{\n"
          "    function boom() returns int\n"
          "    {\n"
          "        let x no_such_name_here + 1\n"      # line 4 of bad.scaly
          "        x\n"
          "    }\n}\n")
open(ws + "/app/bad.scaly", "w").write(BROKEN)
open(ws + "/app/good.scaly", "w").write(
    "define fine\n{\n    function ok() returns int\n        7\n}\n")

ROOT = "file://" + ws + "/app.scaly"
BAD  = "file://" + ws + "/app/bad.scaly"
GOOD = "file://" + ws + "/app/good.scaly"

s = Session()
d = s.open(ws + "/app.scaly")
check(BAD in d and any("no_such_name_here" in x["message"] for x in d.get(BAD, [])),
      "program root: a diagnostic in a module that is NOT open is reported")
lines_bad = [x["range"]["start"]["line"] for x in d.get(BAD, [])]
check(lines_bad == [4],
      "program root: at the MODULE's own line, not an offset into the open buffer"
      + ("" if lines_bad == [4] else " -- got %s" % lines_bad))
check(d.get(ROOT) == [],
      "program root: the open file itself stays clean")
check(GOOD in d and d[GOOD] == [],
      "program root: a CLEAN module is reported with an empty array (that is what clears)")
outside = [u for u in d if not u.startswith("file://" + ws + "/")]
check(not outside,
      "program root: nothing outside the root's subtree is published into"
      + ("" if not outside else " -- %s" % outside[:2]))

# Same session: fix the module on disk and re-analyse. The empty array is the
# only thing that takes the squiggle away.
open(ws + "/app/bad.scaly", "w").write(BROKEN.replace("no_such_name_here + 1", "2 + 1"))
d = s.touch(ws + "/app.scaly", 2)
check(d.get(BAD) == [],
      "the fixed module is CLEARED in the same session")
open(ws + "/app/bad.scaly", "w").write(BROKEN)
s.close()

# The other route: opening a MEMBER plans through package_root# instead.
s = Session()
d = s.open(ws + "/app/good.scaly")
check(BAD in d and any("no_such_name_here" in x["message"] for x in d.get(BAD, [])),
      "package member: the sibling module's diagnostic is reported")
check([x["range"]["start"]["line"] for x in d.get(BAD, [])] == [4],
      "package member: at the sibling's own line")
check(d.get(GOOD) == [], "package member: the open file itself stays clean")
s.close()

# A sibling that is itself OPEN must not be published over: its own analysis
# takes parse errors from the LIVE BUFFER, a sibling notification only from
# disk, so the fan-out would erase the diagnostic under the user's cursor.
ws2 = "/tmp/lsp_ws/root_diagnostics_open"
shutil.rmtree(ws2, ignore_errors=True); os.makedirs(ws2 + "/app")
open(ws2 + "/app.scaly", "w").write(
    "define app\n{\n    module good\n}\n\nuse app.good.fine\n\nlet v fine.ok()\n")
open(ws2 + "/app/good.scaly", "w").write(
    "define fine\n{\n    function ok() returns int\n        7\n}\n")
OPEN_SIB = "file://" + ws2 + "/app/good.scaly"

s = Session()
s.open(ws2 + "/app.scaly")
# good.scaly is opened with an unfinished line that is NEVER saved.
buf = open(ws2 + "/app/good.scaly").read().replace("        7\n", "        7\n    set this.\n")
s.send({"jsonrpc":"2.0","method":"textDocument/didOpen","params":{
    "textDocument":{"uri":OPEN_SIB,"languageId":"scaly","version":1,"text":buf}}})
d = s.collect()
check(any("expected" in x["message"] for x in d.get(OPEN_SIB, [])),
      "open sibling: its own analysis reports the buffer-only parse error")
d = s.touch(ws2 + "/app.scaly", 2)
check(OPEN_SIB not in d,
      "open sibling: editing another file of the root does not publish over it")
s.close()
sys.exit(1 if failures else 0)
PY
} > "$BGDIR/lsp_package_root_diagnostics.out" 2>&1 &
bg_add $! "lsp package-root diagnostics" "$BGDIR/lsp_package_root_diagnostics.out"

# ---- formatter: THE TREE IS THE CORPUS ------------------------------------
# A formatter is only worth having if it agrees with the code that already
# exists, so the gate is not a fixture: it is every packages/**/*.scaly. Two
# claims are checked over all of them.
#
#   IDEMPOTENCE  format(format(x)) == format(x), everywhere. A formatter that
#                oscillates makes every save a diff.
#   HOUSE FORM   the formatter reproduces the committed file BYTE FOR BYTE,
#                except for a named allow-list. That list is the point of this
#                test: it makes the tree's remaining deviations explicit and
#                bounded, so a newly added file that does not follow the house
#                form turns the gate red instead of quietly widening it.
#
# The allow-list was established 2026-08-12 by running the formatter over the
# whole tree: 162 of 172 files came back byte-identical and all ten others were
# read. Four have since been brought into the house form instead of being
# excused here -- FunctionIndex.scaly (a block whose body sat at the enclosing
# level) and one trailing-whitespace line in Array.scaly, both DEFECTS the
# formatter found rather than caused, then Emitter.scaly and Modeler.scaly
# (19 372 lines, the outermost-define convention). Every one of them is a
# compiler or stdlib source, so each reformat was checked the only way that
# settles it: scalyc.ll and scaly.ll re-emit BYTE-IDENTICALLY to the committed
# seed. That -- with `git diff -w` empty and the line count unchanged -- is what
# proves a 19 372-line diff was whitespace and needed no new fixed point.
#
# The six that remain are conventions or losses no indent model can avoid:
#
#   * the outermost `define` body at column 0, still in parser.scaly, hashing
#     and StringIterator. parser.scaly is GENERATED (byte-exact output of
#     codegen/parser-scaly.scm), so reformatting it would be undone by ./mkp and
#     would break the codegen byte-identity gate -- it can only change by
#     changing the generator.
#   * chained single-condition `if`s written at one indent (Planner).
#   * comments aligned to a trailing comment's column (fiber, Plan), which no
#     indent-based formatter can keep.
#
# Shrinking this list is a source change, never a formatter change: if a file
# is reformatted into the house form, its line here must go -- which the last
# check below enforces, so the list cannot rot into a list of excuses.
lsp_build_prog tests/lsp/format_test.scaly /tmp/scalyls_format_test
{ python3 - <<'PY'
import sys, glob, os, subprocess

BIN = "/tmp/scalyls_format_test"
ALLOWED = {
    "packages/scalyc/0.1.0/scalyc/compiler/parser.scaly",
    "packages/scalyc/0.1.0/scalyc/compiler/Planner.scaly",
    "packages/scaly/0.1.0/scaly/containers/hashing.scaly",
    "packages/scaly/0.1.0/scaly/containers/StringIterator.scaly",
    "packages/scaly/0.1.0/scaly/fiber.scaly",
    "packages/scalyc/0.1.0/scalyc/compiler/Plan.scaly",
}

failures = 0
def check(cond, label):
    global failures
    print(("PASS  " if cond else "FAIL  ") + label)
    if not cond: failures += 1

# the generated package interfaces are machine output (tools/interfaces.sh),
# not source a formatter owns: their lines stand where interface/positions
# says, not where the text alone would put them
files = sorted(f for f in glob.glob("packages/**/*.scaly", recursive=True)
               if "/interface/" not in f)
not_idem, unexpected, still_clean = [], [], []
for f in files:
    env = dict(os.environ, SCALYLS_FMT_FILE=f)
    got = subprocess.run([BIN], env=env, stdout=subprocess.PIPE).stdout
    env["SCALYLS_FMT_MODE"] = "i"
    if subprocess.run([BIN], env=env, stdout=subprocess.PIPE).stdout.strip() != b"IDEMPOTENT":
        not_idem.append(f)
    same = got == open(f, "rb").read()
    if not same and f not in ALLOWED:
        unexpected.append(f)
    if same and f in ALLOWED:
        still_clean.append(f)

check(len(files) > 150, "the corpus is the whole tree (%d files)" % len(files))
check(not not_idem, "format(format(x)) == format(x) on every file"
      + ("" if not not_idem else " -- %s" % not_idem[:3]))
check(not unexpected, "every file outside the allow-list is reproduced BYTE-IDENTICALLY"
      + ("" if not unexpected else " -- %s" % unexpected[:5]))
check(not still_clean, "no allow-list entry is stale (a fixed file must be removed from it)"
      + ("" if not still_clean else " -- %s" % still_clean))
sys.exit(1 if failures else 0)
PY
} > "$BGDIR/lsp_formatter_corpus.out" 2>&1 &
bg_add $! "lsp formatter corpus" "$BGDIR/lsp_formatter_corpus.out"

# ---- formatter over the LSP: capability, edits, and what it must NOT do ----
# The last check is the load-bearing one. In Scaly a line break is SEMANTIC --
# LFs separate constructs -- so a formatter that joins or splits lines changes
# the meaning of the program. The line count is therefore an invariant of the
# run, and format_edits# depends on it to pair source and result line by line.
python3 - <<'PY'
import sys, json, subprocess, os, select, time

src = ("define Point\n"
       "(\n"
       "x: int\n"
       ")\n"
       "{\n"
       "        function get_x(this: Point) returns int\n"
       "    {\n"
       "  if x > 0\n"
       "            return x\n"
       "        0\n"
       "        }\n"
       "}\n")
want = ("define Point\n"
        "(\n"
        "    x: int\n"
        ")\n"
        "{\n"
        "    function get_x(this: Point) returns int\n"
        "    {\n"
        "        if x > 0\n"
        "            return x\n"
        "        0\n"
        "    }\n"
        "}\n")
import shutil as _sh; _ws = "/tmp/lsp_ws/format_doc"; _sh.rmtree(_ws, ignore_errors=True); os.makedirs(_ws)
path = _ws + "/lsp_format_doc.scaly"
open(path, "w").write(src)
uri = "file://" + path

def frame(o):
    b = json.dumps(o).encode()
    return ("Content-Length: %d\r\n\r\n" % len(b)).encode() + b

p = subprocess.Popen(["/tmp/scalyls"], stdin=subprocess.PIPE, stdout=subprocess.PIPE, bufsize=0)
fd = p.stdout.fileno()
buf = bytearray()
def next_frame(timeout=30.0):
    deadline = time.time() + timeout
    while True:
        i = buf.find(b"\r\n\r\n")
        if i >= 0:
            n = int(bytes(buf[:i]).decode().split(":")[1].strip())
            if len(buf) >= i + 4 + n:
                body = bytes(buf[i+4:i+4+n]); del buf[:i+4+n]
                return json.loads(body)
        left = deadline - time.time()
        if left <= 0: return None
        if not select.select([fd], [], [], left)[0]: return None
        chunk = os.read(fd, 65536)
        if not chunk: return None
        buf.extend(chunk)

frames = []
p.stdin.write(frame({"jsonrpc":"2.0","id":1,"method":"initialize","params":{}}))
frames.append(next_frame())
p.stdin.write(frame({"jsonrpc":"2.0","method":"initialized","params":{}}))
p.stdin.write(frame({"jsonrpc":"2.0","method":"textDocument/didOpen","params":{"textDocument":{
        "uri":uri,"languageId":"scaly","version":1,"text":src}}}))
next_frame()
p.stdin.write(frame({"jsonrpc":"2.0","id":2,"method":"textDocument/formatting","params":{
        "textDocument":{"uri":uri},
        "options":{"tabSize":4,"insertSpaces":True}}}))
frames.append(next_frame())
p.stdin.write(frame({"jsonrpc":"2.0","method":"exit"}))

failures = 0
def check(cond, label):
    global failures
    print(("PASS  " if cond else "FAIL  ") + label)
    if not cond: failures += 1

caps = (frames[0] or {}).get("result", {}).get("capabilities", {})
check(caps.get("documentFormattingProvider") is True,
      "initialize advertises documentFormattingProvider")

r = next((f for f in frames[1:] if f and f.get("id") == 2), None)
edits = (r or {}).get("result")
check(isinstance(edits, list) and len(edits) > 0,
      "textDocument/formatting answers a non-empty TextEdit[]")

# Apply the edits the way a client does: per line, later edits first so an
# earlier one cannot shift a later one's offsets.
lines = src.split("\n")
for e in sorted(edits or [], key=lambda e: (e["range"]["start"]["line"],
                                            e["range"]["start"]["character"]),
                reverse=True):
    ln = e["range"]["start"]["line"]
    a, b = e["range"]["start"]["character"], e["range"]["end"]["character"]
    check_same_line = e["range"]["end"]["line"] == ln
    if not check_same_line:
        failures += 1
        print("FAIL  every edit stays within ONE line")
        break
    lines[ln] = lines[ln][:a] + e["newText"] + lines[ln][b:]
got = "\n".join(lines)
check(got == want, "applying the edits yields the house form")
check(len(got.split("\n")) == len(src.split("\n")),
      "the line COUNT is unchanged -- an LF in Scaly separates constructs")
sys.exit(1 if failures else 0)
PY
rc=$?
if [ $rc -eq 0 ]; then ok "lsp formatting"; else bad "lsp formatting"; fi

# ---- typing burst: diagnostics must not queue ahead of the answers ---------
# didOpen/didChange used to ANALYSE on the spot, and the editor sends one
# didChange PER KEYSTROKE. The server is single-threaded, so every keystroke put
# a whole modeler+planner pass over the package root (~1 s on a real file) in
# front of everything that followed: typing `set this.` measured 19 analysis runs
# and the completion answer 10.83 s later — long after the editor had cancelled
# it. That is what "no suggestions at all" looked like from the outside.
#
# The handlers now only MARK the buffer (docstore.mark_dirty#) and
# server.process_one# runs the analysis once the client's input queue has
# drained. Both halves are asserted, and WITHOUT timing: the burst plus the
# completion request are written in ONE go, so the server always has input
# pending and must answer the request with NO analysis in front of it; then the
# client goes quiet and the single owed analysis must arrive.
python3 - <<'PY'
import sys, json, subprocess, os, select, time
src = ("define Point\n(\n    x: int\n)\n{\n"
       "    function get_x(this: Point) returns int\n    {\n"
       "        return x\n    }\n}\n")
import os as _os, shutil as _sh; _ws = "/tmp/lsp_ws/burst_test"; _sh.rmtree(_ws, ignore_errors=True); _os.makedirs(_ws)
path = _ws + "/lsp_burst_test.scaly"
open(path, "w").write(src)
uri = "file://" + path
def frame(o):
    b=json.dumps(o).encode(); return ("Content-Length: %d\r\n\r\n"%len(b)).encode()+b
lines = src.split("\n")
# type "\n        set this." at the start of the `return x` line
ln = next(i for i, l in enumerate(lines) if l.strip() == "return x")
typed = "\n        set this."

p = subprocess.Popen(["/tmp/scalyls"], stdin=subprocess.PIPE, stdout=subprocess.PIPE, bufsize=0)
fd = p.stdout.fileno()
buf = bytearray()
def next_frame(timeout=20.0):
    deadline = time.time() + timeout
    while True:
        i = buf.find(b"\r\n\r\n")
        if i >= 0:
            n = int(bytes(buf[:i]).decode().split(":")[1].strip())
            if len(buf) >= i + 4 + n:
                body = bytes(buf[i+4:i+4+n]); del buf[:i+4+n]
                return json.loads(body)
        left = deadline - time.time()
        if left <= 0: return None
        if not select.select([fd], [], [], left)[0]: return None
        chunk = os.read(fd, 65536)
        if not chunk: return None
        buf.extend(chunk)

failures = 0
def check(cond, label):
    global failures
    print(("PASS  " if cond else "FAIL  ") + label)
    if not cond: failures += 1

p.stdin.write(frame({"jsonrpc":"2.0","id":1,"method":"initialize","params":{}}))
next_frame()
p.stdin.write(frame({"jsonrpc":"2.0","method":"initialized","params":{}}))
p.stdin.write(frame({"jsonrpc":"2.0","method":"textDocument/didOpen","params":{"textDocument":{
        "uri":uri,"languageId":"scaly","version":1,"text":src}}}))
opened = next_frame()
check(opened is not None and opened.get("method") == "textDocument/publishDiagnostics"
      and opened["params"]["diagnostics"] == [],
      "didOpen analysis runs (clean file, no diagnostics)")

# ONE write: the keystrokes AND the completion request, so the server never sees
# an empty input queue until it has answered.
burst = b""
ver, line, col = 2, ln, 0
for ch in typed:
    burst += frame({"jsonrpc":"2.0","method":"textDocument/didChange","params":{
            "textDocument":{"uri":uri,"version":ver},
            "contentChanges":[{"range":{"start":{"line":line,"character":col},
                                        "end":{"line":line,"character":col}},"text":ch}]}})
    ver += 1
    if ch == "\n": line, col = line + 1, 0
    else: col += 1
burst += frame({"jsonrpc":"2.0","id":2,"method":"textDocument/completion","params":{
        "textDocument":{"uri":uri},"position":{"line":line,"character":col},
        "context":{"triggerKind":2,"triggerCharacter":"."}}})
p.stdin.write(burst)

runs_before = 0
comp = None
while comp is None:
    f = next_frame()
    if f is None: break
    if f.get("id") == 2: comp = f
    elif f.get("method") == "textDocument/publishDiagnostics": runs_before += 1
labels = [it["label"] for it in ((comp or {}).get("result") or [])]
check(labels == ["x","get_x"], "completion answers inside a typing burst")
check(runs_before == 0,
      "%d keystrokes put NO analysis in front of the answer (got %d)"
      % (len(typed), runs_before))

# The client goes quiet: the one owed analysis must now arrive, and it must still
# report the unfinished line (the parse repair is for the read-only requests, not
# for diagnostics).
tail = next_frame()
check(tail is not None and tail.get("method") == "textDocument/publishDiagnostics",
      "the owed analysis runs once the queue drains")
check(tail is not None and len(tail["params"]["diagnostics"]) == 1,
      "and it reports the unfinished line (1 diagnostic)")
p.stdin.write(frame({"jsonrpc":"2.0","method":"exit"}))
sys.exit(1 if failures else 0)
PY
rc=$?
if [ $rc -eq 0 ]; then ok "lsp typing-burst coalescing"; else bad "lsp typing-burst coalescing"; fi

# ---- cross-file go-to-definition (workspace) ----
# When the cursor identifier is not declared in the current file, the worker
# enumerates sibling .scaly files (via `find`) and returns the declaration
# from whichever file defines it. Two files in a temp dir: caller.scaly uses
# `helper_fn` and the type `Widget`, both defined only in helper.scaly.
python3 - <<'PY'
import sys, json, subprocess, os, tempfile
d = tempfile.mkdtemp(prefix="lsp_xdef_")
helper = ("function helper_fn(n: int) returns int\n{\n    return n + 1\n}\n\n"
          "define Widget\n(\n    w: int\n)\n{\n"
          "    function area(this: Widget) returns int\n    {\n        return w\n    }\n}\n")
caller = ("function caller(x: int) returns int\n{\n    let v helper_fn(x)\n    return v\n}\n\n"
          "function make(w: Widget) returns int\n{\n    return 0\n}\n")
hp = os.path.join(d, "helper.scaly"); open(hp, "w").write(helper)
cp = os.path.join(d, "caller.scaly"); open(cp, "w").write(caller)
curi = "file://"+cp; huri = "file://"+hp
def frame(o):
    b=json.dumps(o).encode(); return ("Content-Length: %d\r\n\r\n"%len(b)).encode()+b
def df(idn, line, char):
    return frame({"jsonrpc":"2.0","id":idn,"method":"textDocument/definition",
                  "params":{"textDocument":{"uri":curi},"position":{"line":line,"character":char}}})
hc = caller.split("\n")[2].index("helper_fn") + 1   # `helper_fn` call site
wc = caller.split("\n")[6].index("Widget") + 1      # `Widget` type reference
inp  = frame({"jsonrpc":"2.0","id":1,"method":"initialize","params":{}})
inp += frame({"jsonrpc":"2.0","method":"initialized","params":{}})
inp += df(2, 2, hc)     # helper_fn -> cross-file helper.scaly:0
inp += df(3, 6, wc)     # Widget    -> cross-file helper.scaly:5
inp += df(4, 0, 10)     # caller    -> intra-file caller.scaly:0
inp += df(5, 1, 0)      # `{`       -> null (no identifier anywhere)
inp += frame({"jsonrpc":"2.0","id":9,"method":"shutdown"})
inp += frame({"jsonrpc":"2.0","method":"exit"})
out = subprocess.run(["/tmp/scalyls"], input=inp, stdout=subprocess.PIPE).stdout
frames, dd = [], out
while dd:
    i = dd.find(b"\r\n\r\n")
    if i < 0: break
    n = int(dd[:i].decode().split(":")[1].strip())
    frames.append(json.loads(dd[i+4:i+4+n])); dd = dd[i+4+n:]
failures = 0
def check(cond, label):
    global failures
    print(("PASS  " if cond else "FAIL  ") + label)
    if not cond: failures += 1
def loc(idn):
    f = next((x for x in frames if x.get("id") == idn), None)
    return (f or {}).get("result")
def at(idn):
    r = loc(idn)
    return None if r is None else (r["uri"], r["range"]["start"]["line"])
check(at(2) == (huri, 0), "cross-file: call site -> function in sibling file (helper.scaly:0)")
check(at(3) == (huri, 5), "cross-file: type reference -> define in sibling file (helper.scaly:5)")
check(at(4) == (curi, 0), "intra-file still wins (caller.scaly:0)")
check(loc(5) is None, "no identifier under cursor -> null")
sys.exit(1 if failures else 0)
PY
rc=$?
if [ $rc -eq 0 ]; then ok "lsp cross-file definition"; else bad "lsp cross-file definition"; fi

# ---- definition + outgoingCalls scoped by the RECEIVER's type ----
# A method name is not unique — this tree declares `append` twenty-six times —
# and matching by name alone answered whichever file the walk reached first.
# The receiver is right there in the source, so its concept decides who may
# answer. Every assertion below is FALSE on the pre-fix binary: the free
# `append` / `helper` of the document itself sit ahead of everything (the
# intra-file pass runs first), so all four member jumps landed in main.scaly.
#
# The decoys are deliberately in the DOCUMENT rather than in a sibling file:
# `find` lists a directory in readdir order, so a sibling decoy would win or
# lose by inode luck, and a gate that passes for that reason gates nothing.
python3 - <<'PY'
import sys, json, subprocess, os, shutil
ws = "/tmp/lsp_ws/def_scope"; shutil.rmtree(ws, ignore_errors=True); os.makedirs(ws)
def concept(name):
    return ("define %s\n(\n    n: int\n)\n{\n"
            "    function append(this: %s, x: int) returns int\n    {\n        return x\n    }\n}\n"
            % (name, name))
tools = "define tools\n{\n    function append(x: int) returns int\n    {\n        return x\n    }\n}\n"
main = ("function append(x: int) returns int\n{\n    return x\n}\n\n"          # decoy 1
        "function helper(x: int) returns int\n{\n    return x\n}\n\n"          # decoy 2
        "define Holder\n(\n    v: int\n)\n{\n"
        "    function run(this: Holder, x: int) returns int\n    {\n        return this.helper(x)\n    }\n\n"
        "    function helper(this: Holder, x: int) returns int\n    {\n        return x\n    }\n}\n\n"
        "function use_it(x: int) returns int\n{\n    var b Bag()\n    var s Sink()\n"
        "    let p b.append(x)\n    let q s.append(x)\n    let r tools.append(x)\n    return p + q + r\n}\n")
for nm, txt in (("bag.scaly", concept("Bag")), ("sink.scaly", concept("Sink")),
                ("tools.scaly", tools), ("main.scaly", main)):
    open(os.path.join(ws, nm), "w").write(txt)
uri  = "file://" + ws + "/main.scaly"
buri = "file://" + ws + "/bag.scaly"
suri = "file://" + ws + "/sink.scaly"
turi = "file://" + ws + "/tools.scaly"
L = main.split("\n")
def pos(needle, occurrence=0, off=0):
    """(line, character) of the `occurrence`-th line containing `needle`."""
    hits = [i for i, l in enumerate(L) if needle in l]
    ln = hits[occurrence]
    return ln, L[ln].index(needle) + off
def frame(o):
    b = json.dumps(o).encode(); return ("Content-Length: %d\r\n\r\n" % len(b)).encode() + b
def session(reqs):
    inp  = frame({"jsonrpc":"2.0","id":1,"method":"initialize","params":{"rootUri":"file://"+ws}})
    inp += frame({"jsonrpc":"2.0","method":"initialized","params":{}})
    inp += frame({"jsonrpc":"2.0","method":"textDocument/didOpen","params":{"textDocument":{
            "uri":uri,"languageId":"scaly","version":1,"text":main}}})
    for r in reqs: inp += frame(r)
    inp += frame({"jsonrpc":"2.0","id":99,"method":"shutdown"})
    inp += frame({"jsonrpc":"2.0","method":"exit"})
    out = subprocess.run(["/tmp/scalyls"], input=inp, stdout=subprocess.PIPE).stdout
    got, d = {}, out
    while d:
        i = d.find(b"\r\n\r\n")
        if i < 0: break
        n = int(d[:i].decode().split(":")[1].strip())
        f = json.loads(d[i+4:i+4+n]); d = d[i+4+n:]
        if isinstance(f.get("id"), int): got[f["id"]] = f.get("result")
    return got
def df(idn, ln, ch):
    return {"jsonrpc":"2.0","id":idn,"method":"textDocument/definition",
            "params":{"textDocument":{"uri":uri},"position":{"line":ln,"character":ch}}}
bl, bc = pos("b.append", 0, 2)          # the member word, not the receiver
sl, sc = pos("s.append", 0, 2)
tl, tc = pos("tools.append", 0, 6)
hl, hc = pos("this.helper", 0, 5)
ul, uc = pos("function use_it", 0, 9)
g = session([df(2, bl, bc), df(3, sl, sc), df(4, tl, tc), df(5, hl, hc), df(6, ul, uc)])
failures = 0
def check(cond, label):
    global failures
    print(("PASS  " if cond else "FAIL  ") + label)
    if not cond: failures += 1
def at(idn):
    r = g.get(idn)
    return None if r is None else (r["uri"], r["range"]["start"]["line"])
holder_helper = [i for i, l in enumerate(L) if "function helper(this: Holder" in l][0]
use_it_line    = [i for i, l in enumerate(L) if "function use_it" in l][0]
check(at(2) == (buri, 5), "b.append -> Bag's own file, not the document's free `append`")
check(at(3) == (suri, 5), "s.append -> Sink's file: the SAME name, a different receiver")
check(at(4) == (turi, 2), "tools.append -> the namespace the receiver names")
check(at(5) == (uri, holder_helper),
      "this.helper -> the enclosing concept's member, not the free one above it")
check(at(6) == (uri, use_it_line), "a cursor that is not a member is unchanged")

# The call hierarchy resolves callees the same way and had the same defect.
# outgoing dedupes by NAME, so the three `append` calls collapse into the entry
# of the FIRST one — b.append, i.e. Bag's file.
prep = session([{"jsonrpc":"2.0","id":2,"method":"textDocument/prepareCallHierarchy",
                 "params":{"textDocument":{"uri":uri},"position":{"line":ul,"character":uc}}}])
item = (prep.get(2) or [None])[0]
if item is None:
    print("FAIL  prepareCallHierarchy produced no item for use_it"); sys.exit(1)
g2 = session([{"jsonrpc":"2.0","id":3,"method":"callHierarchy/outgoingCalls","params":{"item":item}}])
outs = {c["to"]["name"]: c["to"]["uri"] for c in (g2.get(3) or [])}
check(outs.get("append") == buri, "outgoingCalls: the callee comes from the receiver's file too")
sys.exit(1 if failures else 0)
PY
rc=$?
if [ $rc -eq 0 ]; then ok "lsp definition scoped by the receiver type"; else bad "lsp definition scoped by the receiver type"; fi

# ---- hover on a PROPERTY declaration ----
# A hover on a property's declaration line fell through to the concept and
# answered `struct Box` — the same class the routine-declaration block in
# symbols.scaly argues against, one level in: a plausible-looking answer to a
# different question. Hovering the SAME name in a use or in a `set` always
# answered `int`, so the declaration was the one place its type was hidden.
# hover_definition# asked the class BODY and never the structure.
#
# The first three checks are FALSE on the pre-fix binary (all three report
# `struct Box`); the last four are the negative controls and pass on both.
# The `struct Box` control at the class's closing brace is the one that keeps
# the walk BOUNDED: every part test is `offset >= start`, so with the body-start
# limit removed that offset reports `property secret: int` instead (verified by
# building exactly that variant).
python3 - <<'PY'
import sys, json, subprocess, os, shutil
BIN = "/tmp/scalyls"
ws = "/tmp/lsp_ws/hover_property"; shutil.rmtree(ws, ignore_errors=True); os.makedirs(ws)
src = ("; A file banner that must NOT reach any hover.\n"                  # 0
       "\n"                                                               # 1
       "define Box\n"                                                     # 2
       "(\n"                                                              # 3
       "    ; The width, in points.\n"                                    # 4
       "    ; A second doc line.\n"                                       # 5
       "    width: int\n"                                                 # 6
       "    height: size_t\n"                                             # 7
       "    private secret: int\n"                                        # 8
       ")\n"                                                              # 9
       "{\n"                                                              # 10
       "    init()\n"                                                     # 11
       "    {\n"                                                          # 12
       "        set width: 3\n"                                           # 13
       "        set height: 4\n"                                          # 14
       "        set secret: 5\n"                                          # 15
       "    }\n"                                                          # 16
       "\n"                                                               # 17
       "    function area(this) returns int\n"                            # 18
       "    {\n"                                                          # 19
       "        return width\n"                                           # 20
       "    }\n"                                                          # 21
       "}\n")                                                             # 22
path = ws + "/box.scaly"; open(path, "w").write(src)
uri = "file://" + path
L = src.split("\n")
def pos(needle, off=0):
    ln = [i for i, l in enumerate(L) if needle in l][0]
    return ln, L[ln].index(needle) + off
def frame(o):
    b = json.dumps(o).encode(); return ("Content-Length: %d\r\n\r\n" % len(b)).encode() + b
def session(reqs):
    inp  = frame({"jsonrpc":"2.0","id":1,"method":"initialize","params":{"rootUri":"file://"+ws}})
    inp += frame({"jsonrpc":"2.0","method":"initialized","params":{}})
    inp += frame({"jsonrpc":"2.0","method":"textDocument/didOpen","params":{"textDocument":{
            "uri":uri,"languageId":"scaly","version":1,"text":src}}})
    for r in reqs: inp += frame(r)
    inp += frame({"jsonrpc":"2.0","id":99,"method":"shutdown"})
    inp += frame({"jsonrpc":"2.0","method":"exit"})
    out = subprocess.run([BIN], input=inp, stdout=subprocess.PIPE).stdout
    got, d = {}, out
    while d:
        i = d.find(b"\r\n\r\n")
        if i < 0: break
        n = int(d[:i].decode().split(":")[1].strip())
        f = json.loads(d[i+4:i+4+n]); d = d[i+4+n:]
        if isinstance(f.get("id"), int): got[f["id"]] = f.get("result")
    return got
def hv(idn, ln, ch):
    return {"jsonrpc":"2.0","id":idn,"method":"textDocument/hover",
            "params":{"textDocument":{"uri":uri},"position":{"line":ln,"character":ch}}}
wl, wc = pos("    width: int", 4)
hl, hc = pos("    height: size_t", 4)
sl, sc = pos("    private secret", 12)
cl, cc = pos("define Box", 7)
ul, uc = pos("return width", 7)
bl, bc = 22, 0                                  # the class's closing brace
refs = {"jsonrpc":"2.0","id":8,"method":"textDocument/references",
        "params":{"textDocument":{"uri":uri},"position":{"line":wl,"character":wc},
                  "context":{"includeDeclaration":True}}}
g = session([hv(2,wl,wc), hv(3,hl,hc), hv(4,sl,sc), hv(5,cl,cc), hv(6,ul,uc), hv(7,bl,bc), refs])
def val(idn):
    r = g.get(idn)
    return None if r is None else r["contents"]["value"]
failures = 0
def check(cond, label):
    global failures
    print(("PASS  " if cond else "FAIL  ") + label)
    if not cond: failures += 1
check(val(2) == "property width: int\n\n```\nThe width, in points.\nA second doc line.\n```",
      "hover on a property DECLARATION -> the property, with its doc block")
check(val(3) == "property height: size_t",
      "the SECOND property answers its own name and type")
check(val(4) == "property secret: int",
      "a `private` field is a property too")
check(val(5) == "struct Box", "the concept name still answers the concept")
check(val(6) == "int", "hover on a USE is unchanged (the semantic type)")
check(val(7) == "struct Box",
      "an offset PAST the body answers the concept, not the last property")
loc = g.get(8) or []
check(sorted((r["range"]["start"]["line"], r["range"]["start"]["character"]) for r in loc)
      == [(wl, wc), (13, 12), (20, 15)],
      "findReferences on the declaration is unchanged (3 hits)")
sys.exit(1 if failures else 0)
PY
rc=$?
if [ $rc -eq 0 ]; then ok "lsp hover on a property declaration"; else bad "lsp hover on a property declaration"; fi

# ---- definition on a PROPERTY ----
# The other half of the same gap: def_body# walks a class BODY, so a property
# had no declaration to jump to at all and goToDefinition on a field answered
# nothing. Wired at BOTH sites that route a class to def_body — the intra-file
# walk (def_definition#) and the receiver-scoped one (md_definition#) — which
# is what the `b.height` check separates: a free `height` sits at line 0 and
# wins the name-wide walk, so only the scoped route can answer the property.
# The first three checks are FALSE on the pre-fix binary (null, null, and the
# free function); the last two are the controls.
#
# LIMIT, deliberate: a receiver whose concept lives in ANOTHER FILE still
# misses. That path answers from the cached workspace blob (def_in_file#),
# which indexes declarations and not structure parts — extending it is a
# separate claim, with workspace/symbol's output attached to it.
python3 - <<'PY'
import sys, json, subprocess, os, shutil
BIN = "/tmp/scalyls"
ws = "/tmp/lsp_ws/def_property"; shutil.rmtree(ws, ignore_errors=True); os.makedirs(ws)
src = ("function height(x: int) returns int\n"         # 0  DECOY: a free function named like a property
       "{\n    return x\n}\n"                          # 1-3
       "\n"                                            # 4
       "define Box\n"                                  # 5
       "(\n"                                           # 6
       "    width: int\n"                              # 7
       "    height: int\n"                             # 8
       ")\n"                                           # 9
       "{\n"                                           # 10
       "    init()\n"                                  # 11
       "    {\n"                                       # 12
       "        set width: 3\n"                        # 13
       "        set height: 4\n"                       # 14
       "    }\n"                                       # 15
       "\n"                                            # 16
       "    function area(this) returns int\n"         # 17
       "    {\n"                                       # 18
       "        return width\n"                        # 19
       "    }\n"                                       # 20
       "}\n"                                           # 21
       "\n"                                            # 22
       "function use_it() returns int\n"               # 23
       "{\n"                                           # 24
       "    var b Box()\n"                             # 25
       "    return b.height\n"                         # 26
       "}\n")                                          # 27
path = ws + "/box.scaly"; open(path, "w").write(src)
uri = "file://" + path
def frame(o):
    b = json.dumps(o).encode(); return ("Content-Length: %d\r\n\r\n" % len(b)).encode() + b
inp  = frame({"jsonrpc":"2.0","id":1,"method":"initialize","params":{"rootUri":"file://"+ws}})
inp += frame({"jsonrpc":"2.0","method":"initialized","params":{}})
inp += frame({"jsonrpc":"2.0","method":"textDocument/didOpen","params":{"textDocument":{
        "uri":uri,"languageId":"scaly","version":1,"text":src}}})
probes = [(2, 19, 15), (3, 13, 12), (4, 26, 13), (5, 17, 13), (6, 23, 9)]
for idn, ln, ch in probes:
    inp += frame({"jsonrpc":"2.0","id":idn,"method":"textDocument/definition",
                  "params":{"textDocument":{"uri":uri},"position":{"line":ln,"character":ch}}})
inp += frame({"jsonrpc":"2.0","id":99,"method":"shutdown"})
inp += frame({"jsonrpc":"2.0","method":"exit"})
out = subprocess.run([BIN], input=inp, stdout=subprocess.PIPE).stdout
got, d = {}, out
while d:
    i = d.find(b"\r\n\r\n")
    if i < 0: break
    n = int(d[:i].decode().split(":")[1].strip())
    f = json.loads(d[i+4:i+4+n]); d = d[i+4+n:]
    if isinstance(f.get("id"), int): got[f["id"]] = f.get("result")
def at(idn):
    r = got.get(idn)
    return None if r is None else (r["range"]["start"]["line"], r["range"]["start"]["character"])
failures = 0
def check(cond, label):
    global failures
    print(("PASS  " if cond else "FAIL  ") + label)
    if not cond: failures += 1
check(at(2) == (7, 4),  "definition on a property USE -> its declaration")
check(at(3) == (7, 4),  "definition on a `set` target -> the property it names")
check(at(4) == (8, 4),
      "b.height -> the receiver's property, not the free function of that name")
check(at(5) == (17, 4), "a method still resolves to the method")
check(at(6) == (23, 0), "a non-member cursor is unchanged")
sys.exit(1 if failures else 0)
PY
rc=$?
if [ $rc -eq 0 ]; then ok "lsp definition on a property"; else bad "lsp definition on a property"; fi

# ---- hover names the chain LINK the cursor is on ----
# Mode 0 reports a member chain's FINAL type and hover asked it at every
# offset, so all twelve columns of `t.mid.leaf.v` answered `int`: hovering `t`
# said the Top was an int. Three of four names carried another name's type — a
# plausible answer to a different question, which is the class this tree keeps
# calling out.
#
# The plan cannot say which link a cursor is on: a PlannedMemberAccess carries
# a NAME and no span, and the operand's span is NOT the chain's text —
# reconstructing the tokens backwards from `loc.end` came out shifted by 1, 5,
# 3 and 1 in four statement contexts, which is why that route was abandoned.
# The question is decided on the SOURCE instead (semantic.chain_position#) and
# chooses WHICH question to put to the walk. A middle link is the one position
# no mode reports, and it answers where the cursor IS rather than a type.
#
# Checks 1-3 are FALSE on the pre-fix binary (all three report `int`);
# 4-6 are the controls and pass on both — the last link, the METHOD link that
# must keep answering what the call yields, and an ordinary binding.
python3 - <<'PY'
import sys, json, subprocess, os, shutil
BIN = "/tmp/scalyls"
ws = "/tmp/lsp_ws/hover_chain"; shutil.rmtree(ws, ignore_errors=True); os.makedirs(ws)
src = ("define Leaf\n(\n    v: int\n)\n{\n    init()\n    {\n        set v: 1\n    }\n}\n"          # 0-9
       "\ndefine Mid\n(\n    leaf: Leaf\n)\n{\n    init()\n    {\n        set leaf: Leaf()\n    }\n}\n"   # 10-20
       "\ndefine Top\n(\n    mid: Mid\n)\n{\n    init()\n    {\n        set mid: Mid()\n    }\n"     # 21-30
       "    function depth(this) returns int\n    {\n        return 3\n    }\n}\n"                   # 31-35
       "\nfunction f(t: Top) returns int\n"                                                          # 36-37
       "{\n"                                                                                         # 38
       "    var n: size_t 7\n"                                                                       # 39
       "    let a t.mid.leaf.v\n"                                                                    # 40
       "    let b t.depth()\n"                                                                       # 41
       "    return a + b\n"                                                                          # 42
       "}\n")                                                                                        # 43
path = ws + "/a.scaly"; open(path, "w").write(src)
uri = "file://" + path
L = src.split("\n")
def frame(o):
    b = json.dumps(o).encode(); return ("Content-Length: %d\r\n\r\n" % len(b)).encode() + b
chain_ln = [i for i, l in enumerate(L) if "let a t.mid.leaf.v" in l][0]
base = L[chain_ln].index("t.mid.leaf.v")
call_ln = [i for i, l in enumerate(L) if "let b t.depth()" in l][0]
probes = [
    (2, chain_ln, base + 0,  "receiver `t`"),
    (3, chain_ln, base + 2,  "middle link `mid`"),
    (4, chain_ln, base + 7,  "middle link `leaf`"),
    (5, chain_ln, base + 11, "last link `v`"),
    (6, call_ln,  L[call_ln].index("depth"), "method link `depth`"),
    (7, 39,       L[39].index("n") if "n" in L[39] else 8, "a plain binding name"),
]
inp  = frame({"jsonrpc":"2.0","id":1,"method":"initialize","params":{"rootUri":"file://"+ws}})
inp += frame({"jsonrpc":"2.0","method":"initialized","params":{}})
inp += frame({"jsonrpc":"2.0","method":"textDocument/didOpen","params":{"textDocument":{
        "uri":uri,"languageId":"scaly","version":1,"text":src}}})
for idn, ln, ch, _ in probes:
    inp += frame({"jsonrpc":"2.0","id":idn,"method":"textDocument/hover",
                  "params":{"textDocument":{"uri":uri},"position":{"line":ln,"character":ch}}})
inp += frame({"jsonrpc":"2.0","id":99,"method":"shutdown"})
inp += frame({"jsonrpc":"2.0","method":"exit"})
out = subprocess.run([BIN], input=inp, stdout=subprocess.PIPE).stdout
got, d = {}, out
while d:
    i = d.find(b"\r\n\r\n")
    if i < 0: break
    n = int(d[:i].decode().split(":")[1].strip())
    fr = json.loads(d[i+4:i+4+n]); d = d[i+4+n:]
    if isinstance(fr.get("id"), int): got[fr["id"]] = fr.get("result")
def val(idn):
    r = got.get(idn)
    return None if r is None else r["contents"]["value"].split("\n")[0]
failures = 0
def check(cond, label):
    global failures
    print(("PASS  " if cond else "FAIL  ") + label)
    if not cond: failures += 1
check(val(2) == "Top",  "the RECEIVER of a chain reports its own type, not the chain's last link")
check(val(3) == "in function f", "a MIDDLE link answers where the cursor is, never another link's type")
check(val(4) == "in function f", "the second middle link likewise")
check(val(5) == "int",  "the LAST link reports its own type (unchanged)")
check(val(6) == "int",  "a METHOD link still reports what the call yields (unchanged)")
check(val(7) == "size_t", "a plain binding is untouched")
sys.exit(1 if failures else 0)
PY
rc=$?
if [ $rc -eq 0 ]; then ok "lsp hover names the chain link"; else bad "lsp hover names the chain link"; fi

# ---- cross-FILE definition on a property ----
# def_structure# closed the intra-file half; a receiver whose concept lives in
# ANOTHER file still missed, because that path answers from the cached
# workspace blob (def_in_file#), which indexed declarations and body members
# but not structure parts. Measured before the fix on this tree: five field
# accesses in symbols.scaly — `def.concept_`, `c.structure`, `fld.property`,
# `routine.throws_`, `routine.implementation`, every one of them a record from
# scalyc/compiler/Syntax.scaly — all answered null.
#
# The decoy is a free `height` in the DOCUMENT, so only the receiver-scoped
# route can reach the property in the other file. The third check is the
# consequence, asserted rather than left implicit: this blob IS the workspace
# symbol table, so the property now appears there too.
# Checks 1 and 3 are FALSE on the pre-fix binary; check 2 is the control.
python3 - <<'PY'
import sys, json, subprocess, os, shutil
BIN = "/tmp/scalyls"
ws = "/tmp/lsp_ws/def_property_xfile"; shutil.rmtree(ws, ignore_errors=True); os.makedirs(ws)
box = ("define Box\n"                                  # 0
       "(\n"                                           # 1
       "    width: int\n"                              # 2
       "    height: int\n"                             # 3
       ")\n"                                           # 4
       "{\n"                                           # 5
       "    init()\n"                                  # 6
       "    {\n"                                       # 7
       "        set width: 3\n"                        # 8
       "        set height: 4\n"                       # 9
       "    }\n"                                       # 10
       "\n"                                            # 11
       "    function area(this) returns int\n"         # 12
       "    {\n"                                       # 13
       "        return width\n"                        # 14
       "    }\n"                                       # 15
       "}\n")                                          # 16
main = ("function height(x: int) returns int\n"        # 0  DECOY, and it sits in the OTHER file
        "{\n    return x\n}\n"                         # 1-3
        "\n"                                           # 4
        "function use_it() returns int\n"              # 5
        "{\n"                                          # 6
        "    var b Box()\n"                            # 7
        "    return b.height + b.area()\n"             # 8
        "}\n")                                         # 9
open(os.path.join(ws, "box.scaly"),  "w").write(box)
open(os.path.join(ws, "main.scaly"), "w").write(main)
buri = "file://" + ws + "/box.scaly"
uri  = "file://" + ws + "/main.scaly"
def frame(o):
    b = json.dumps(o).encode(); return ("Content-Length: %d\r\n\r\n" % len(b)).encode() + b
inp  = frame({"jsonrpc":"2.0","id":1,"method":"initialize","params":{"rootUri":"file://"+ws}})
inp += frame({"jsonrpc":"2.0","method":"initialized","params":{}})
inp += frame({"jsonrpc":"2.0","method":"textDocument/didOpen","params":{"textDocument":{
        "uri":uri,"languageId":"scaly","version":1,"text":main}}})
inp += frame({"jsonrpc":"2.0","id":2,"method":"textDocument/definition","params":{
        "textDocument":{"uri":uri},"position":{"line":8,"character":13}}})     # b.height
inp += frame({"jsonrpc":"2.0","id":3,"method":"textDocument/definition","params":{
        "textDocument":{"uri":uri},"position":{"line":8,"character":25}}})     # b.area()
inp += frame({"jsonrpc":"2.0","id":4,"method":"workspace/symbol","params":{"query":"width"}})
inp += frame({"jsonrpc":"2.0","id":99,"method":"shutdown"})
inp += frame({"jsonrpc":"2.0","method":"exit"})
out = subprocess.run([BIN], input=inp, stdout=subprocess.PIPE).stdout
got, d = {}, out
while d:
    i = d.find(b"\r\n\r\n")
    if i < 0: break
    n = int(d[:i].decode().split(":")[1].strip())
    fr = json.loads(d[i+4:i+4+n]); d = d[i+4+n:]
    if isinstance(fr.get("id"), int): got[fr["id"]] = fr.get("result")
def at(idn):
    r = got.get(idn)
    return None if r is None else (r["uri"], r["range"]["start"]["line"], r["range"]["start"]["character"])
failures = 0
def check(cond, label):
    global failures
    print(("PASS  " if cond else "FAIL  ") + label)
    if not cond: failures += 1
check(at(2) == (buri, 3, 4),
      "b.height with Box in ANOTHER file -> the property there, not the decoy beside the call")
check(at(3) == (buri, 12, 4), "a method through the same receiver is unchanged")
syms = got.get(4) or []
hits = [(s["name"], s["kind"], os.path.basename(s["location"]["uri"]),
         s["location"]["range"]["start"]["line"]) for s in syms]
check(("width", 8, "box.scaly", 2) in hits,
      "workspace/symbol lists the property as Field(8) at its NAME token")
sys.exit(1 if failures else 0)
PY
rc=$?
if [ $rc -eq 0 ]; then ok "lsp cross-file definition on a property"; else bad "lsp cross-file definition on a property"; fi

# ---- workspace/symbol (parse-based, all files) ----
# Query the whole workspace for declarations whose name matches a substring
# (case-insensitive). The root comes from initialize's rootUri. Two files in
# a temp dir contribute functions, a struct + method, a mutable, and a union
# variant; a case-insensitive substring query gathers them from both files.
python3 - <<'PY'
import sys, json, subprocess, os, tempfile
d = tempfile.mkdtemp(prefix="lsp_wsym_")
a = ("function alpha_one(n: int) returns int\n{\n    return n\n}\n\n"
     "define Gadget\n(\n    g: int\n)\n{\n"
     "    function alpha_method(this: Gadget) returns int\n    {\n        return g\n    }\n}\n")
b = ("mutable alpha_counter: int 0\n\n"
     "define Shape union (\n    AlphaVariant: int\n    Beta: int\n)\n")
ap = os.path.join(d, "a.scaly"); open(ap, "w").write(a)
bp = os.path.join(d, "b.scaly"); open(bp, "w").write(b)
auri = "file://"+ap; buri = "file://"+bp
def frame(o):
    s=json.dumps(o).encode(); return ("Content-Length: %d\r\n\r\n"%len(s)).encode()+s
def wsym(idn, q):
    return frame({"jsonrpc":"2.0","id":idn,"method":"workspace/symbol","params":{"query":q}})
inp  = frame({"jsonrpc":"2.0","id":1,"method":"initialize","params":{"rootUri":"file://"+d}})
inp += frame({"jsonrpc":"2.0","method":"initialized","params":{}})
inp += wsym(2, "alpha")     # case-insensitive substring across both files
inp += wsym(3, "Gadget")    # a type name
inp += wsym(4, "zzz")       # no match
inp += frame({"jsonrpc":"2.0","id":9,"method":"shutdown"})
inp += frame({"jsonrpc":"2.0","method":"exit"})
out = subprocess.run(["/tmp/scalyls"], input=inp, stdout=subprocess.PIPE).stdout
frames, dd = [], out
while dd:
    i = dd.find(b"\r\n\r\n")
    if i < 0: break
    n = int(dd[:i].decode().split(":")[1].strip())
    frames.append(json.loads(dd[i+4:i+4+n])); dd = dd[i+4+n:]
failures = 0
def check(cond, label):
    global failures
    print(("PASS  " if cond else "FAIL  ") + label)
    if not cond: failures += 1
caps = frames[0]["result"]["capabilities"]
check(caps.get("workspaceSymbolProvider") is True, "initialize advertises workspaceSymbolProvider")
def res(idn):
    f = next((x for x in frames if x.get("id") == idn), None)
    return (f or {}).get("result")
def by_name(idn):
    return {s["name"]: (s["kind"], s["location"]["uri"]) for s in (res(idn) or [])}
al = by_name(2)
check(set(al.keys()) == {"alpha_one","alpha_method","alpha_counter","AlphaVariant"},
      "'alpha' matches names across both files (case-insensitive substring)")
check(al.get("alpha_one") == (12, auri), "function -> Function(12) in a.scaly")
check(al.get("alpha_method") == (6, auri), "struct method -> Method(6) in a.scaly")
check(al.get("alpha_counter") == (13, buri), "mutable -> Variable(13) in b.scaly")
check(al.get("AlphaVariant") == (22, buri), "union variant -> EnumMember(22) in b.scaly")
g = by_name(3)
check(set(g.keys()) == {"Gadget"} and g["Gadget"][0] == 23, "'Gadget' -> the struct only (Struct=23)")
check(res(4) == [], "no match -> []")
sys.exit(1 if failures else 0)
PY
rc=$?
if [ $rc -eq 0 ]; then ok "lsp workspace/symbol"; else bad "lsp workspace/symbol"; fi

# ---- persistent workspace symbol index (cache reuse + invalidation) ----
# The worker caches each file's parsed symbol blob keyed by content hash, so
# repeated workspace/symbol queries reuse the parse and only changed files are
# re-parsed. This test interleaves I/O (Popen) to change a file ON DISK between
# two queries in ONE session and asserts the index reflects the new content
# (content-hash invalidation), then that a third identical query is stable.
python3 - <<'PY'
import sys, json, subprocess, os, tempfile
d = tempfile.mkdtemp(prefix="lsp_idx_")
fp = os.path.join(d, "idx.scaly")
open(fp, "w").write("function index_alpha() returns int\n{\n    return 1\n}\n")
def frame(o):
    s = json.dumps(o).encode(); return ("Content-Length: %d\r\n\r\n" % len(s)).encode() + s
p = subprocess.Popen(["/tmp/scalyls"], stdin=subprocess.PIPE, stdout=subprocess.PIPE)
def send(o):
    p.stdin.write(frame(o)); p.stdin.flush()
def read_frame():
    hdr = b""
    while b"\r\n\r\n" not in hdr:
        ch = p.stdout.read(1)
        if not ch: return None
        hdr += ch
    n = int(hdr.split(b"\r\n")[0].split(b":")[1].strip())
    return json.loads(p.stdout.read(n))
def result_for(idn):                              # read frames until id `idn`
    while True:
        f = read_frame()
        if f is None: return None
        if f.get("id") == idn: return f.get("result")
def names(r): return sorted(s["name"] for s in (r or []))

send({"jsonrpc":"2.0","id":1,"method":"initialize","params":{"rootUri":"file://"+d}})
result_for(1)
send({"jsonrpc":"2.0","method":"initialized","params":{}})
send({"jsonrpc":"2.0","id":2,"method":"workspace/symbol","params":{"query":"index_"}})
q1 = names(result_for(2))
# Change the file on disk -> different content hash -> must re-parse.
open(fp, "w").write("function index_beta() returns int\n{\n    return 2\n}\n")
send({"jsonrpc":"2.0","id":3,"method":"workspace/symbol","params":{"query":"index_"}})
q2 = names(result_for(3))
send({"jsonrpc":"2.0","id":4,"method":"workspace/symbol","params":{"query":"index_"}})
q3 = names(result_for(4))
send({"jsonrpc":"2.0","id":9,"method":"shutdown"})
result_for(9)
send({"jsonrpc":"2.0","method":"exit"})
p.wait()

failures = 0
def check(cond, label):
    global failures
    print(("PASS  " if cond else "FAIL  ") + label)
    if not cond: failures += 1
check(q1 == ["index_alpha"], "first query indexes index_alpha")
check(q2 == ["index_beta"],  "disk change picked up (content-hash invalidation)")
check(q3 == ["index_beta"],  "repeated query stable (cache reuse)")
sys.exit(1 if failures else 0)
PY
rc=$?
if [ $rc -eq 0 ]; then ok "lsp workspace symbol index"; else bad "lsp workspace symbol index"; fi

# ---- cross-file definition via the persistent symindex (Step 29) ----
# Cross-file go-to-definition now queries the SAME content-hash-cached symbol
# blobs workspace/symbol / semanticTokens build, instead of re-parsing each
# sibling per request. This interleaves I/O (Popen) to prove the cache across
# requests in ONE session: a def-on-miss warms the index; a disk change to the
# target file between two identical queries is picked up via content-hash
# invalidation (the decl moves down); a repeated query is stable (cache reuse).
python3 - <<'PY'
import sys, json, subprocess, os, tempfile
d = tempfile.mkdtemp(prefix="lsp_xdefcache_")
hp = os.path.join(d, "helper.scaly")
cp = os.path.join(d, "caller.scaly")
helper_v1 = ("function xfn_target(n: int) returns int\n{\n    return n + 1\n}\n")
helper_v2 = ("function filler() returns int\n{\n    return 0\n}\n\n"
             "function xfn_target(n: int) returns int\n{\n    return n + 1\n}\n")
caller = ("function caller(x: int) returns int\n{\n    let v xfn_target(x)\n    return v\n}\n")
open(hp, "w").write(helper_v1)
open(cp, "w").write(caller)
curi = "file://"+cp; huri = "file://"+hp
xc = caller.split("\n")[2].index("xfn_target") + 1   # `xfn_target` call site col
def frame(o):
    s = json.dumps(o).encode(); return ("Content-Length: %d\r\n\r\n" % len(s)).encode() + s
p = subprocess.Popen(["/tmp/scalyls"], stdin=subprocess.PIPE, stdout=subprocess.PIPE)
def send(o):
    p.stdin.write(frame(o)); p.stdin.flush()
def read_frame():
    hdr = b""
    while b"\r\n\r\n" not in hdr:
        ch = p.stdout.read(1)
        if not ch: return None
        hdr += ch
    n = int(hdr.split(b"\r\n")[0].split(b":")[1].strip())
    return json.loads(p.stdout.read(n))
def result_for(idn):
    while True:
        f = read_frame()
        if f is None: return None
        if f.get("id") == idn: return f.get("result")
def df(idn):
    return {"jsonrpc":"2.0","id":idn,"method":"textDocument/definition",
            "params":{"textDocument":{"uri":curi},"position":{"line":2,"character":xc}}}
def at(r):
    return None if r is None else (r["uri"], r["range"]["start"]["line"])

send({"jsonrpc":"2.0","id":1,"method":"initialize","params":{"rootUri":"file://"+d}})
result_for(1)
send({"jsonrpc":"2.0","method":"initialized","params":{}})
send(df(2)); q1 = at(result_for(2))            # def-on-miss warms the index
open(hp, "w").write(helper_v2)                 # decl moves to line 5 on disk
send(df(3)); q2 = at(result_for(3))            # content-hash invalidation
send(df(4)); q3 = at(result_for(4))            # cache reuse, stable
send({"jsonrpc":"2.0","id":9,"method":"shutdown"})
result_for(9)
send({"jsonrpc":"2.0","method":"exit"})
p.wait()

failures = 0
def check(cond, label):
    global failures
    print(("PASS  " if cond else "FAIL  ") + label)
    if not cond: failures += 1
check(q1 == (huri, 0), "cross-file def resolves (helper.scaly:0), warming the index")
check(q2 == (huri, 5), "disk change picked up (content-hash invalidation, decl now :5)")
check(q3 == (huri, 5), "repeated query stable (cache reuse)")
sys.exit(1 if failures else 0)
PY
rc=$?
if [ $rc -eq 0 ]; then ok "lsp cross-file definition cache"; else bad "lsp cross-file definition cache"; fi

# ---- cross-file signatureHelp + member completion via symindex (Step 29) ----
# The Step-29 follow-up: cross-file signatureHelp and member completion now read
# the SAME content-hash-cached ws blobs (each routine record carries an
# "S"-tagged param-label detail; each `define` record an "M"-tagged member
# fragment) instead of re-parsing each sibling per request. This interleaves I/O
# to prove the cache + invalidation for BOTH: a cross-file sig and member-list
# query warm the index; a disk edit to the target file (adds a param + a method)
# is picked up on the second identical query (content-hash invalidation).
python3 - <<'PY'
import sys, json, subprocess, os, tempfile
d = tempfile.mkdtemp(prefix="lsp_xsigmem_")
hp = os.path.join(d, "helper.scaly")
cp = os.path.join(d, "caller.scaly")
widget = ("define XWidget\n(\n    w: int\n)\n{\n"
          "    function area(this: XWidget) returns int\n    {\n        return w\n    }\n")
helper_v1 = ("function xfn_target(alpha: int) returns int\n{\n    return alpha\n}\n\n"
             + widget + "}\n")
helper_v2 = ("function xfn_target(alpha: int, beta: int) returns int\n{\n    return alpha\n}\n\n"
             + widget
             + "    function perimeter(this: XWidget) returns int\n    {\n        return w\n    }\n}\n")
caller = ("function caller(g: XWidget) returns int\n{\n    let v xfn_target(0)\n"
          "    XWidget.x\n    return v\n}\n")
open(hp, "w").write(helper_v1)
open(cp, "w").write(caller)
curi = "file://"+cp
sig_col = caller.split("\n")[2].index("(") + 1        # inside xfn_target(|0)
mem_col = caller.split("\n")[3].index(".") + 1        # after XWidget.|
def frame(o):
    s = json.dumps(o).encode(); return ("Content-Length: %d\r\n\r\n" % len(s)).encode() + s
p = subprocess.Popen(["/tmp/scalyls"], stdin=subprocess.PIPE, stdout=subprocess.PIPE)
def send(o):
    p.stdin.write(frame(o)); p.stdin.flush()
def read_frame():
    hdr = b""
    while b"\r\n\r\n" not in hdr:
        ch = p.stdout.read(1)
        if not ch: return None
        hdr += ch
    n = int(hdr.split(b"\r\n")[0].split(b":")[1].strip())
    return json.loads(p.stdout.read(n))
def result_for(idn):
    while True:
        f = read_frame()
        if f is None: return None
        if f.get("id") == idn: return f.get("result")
def sig(idn):
    return {"jsonrpc":"2.0","id":idn,"method":"textDocument/signatureHelp",
            "params":{"textDocument":{"uri":curi},"position":{"line":2,"character":sig_col}}}
def comp(idn):
    return {"jsonrpc":"2.0","id":idn,"method":"textDocument/completion",
            "params":{"textDocument":{"uri":curi},"position":{"line":3,"character":mem_col}}}
def sig_label(r): return (r or {}).get("signatures",[{}])[0].get("label")
def comp_labels(r): return [it["label"] for it in r] if isinstance(r, list) else None

send({"jsonrpc":"2.0","id":1,"method":"initialize","params":{"rootUri":"file://"+d}})
result_for(1)
send({"jsonrpc":"2.0","method":"initialized","params":{}})
send(sig(2));  s1 = sig_label(result_for(2))          # warm index
send(comp(3)); m1 = comp_labels(result_for(3))
open(hp, "w").write(helper_v2)                         # +param, +method on disk
send(sig(4));  s2 = sig_label(result_for(4))          # invalidation
send(comp(5)); m2 = comp_labels(result_for(5))
send({"jsonrpc":"2.0","id":9,"method":"shutdown"})
result_for(9)
send({"jsonrpc":"2.0","method":"exit"})
p.wait()

failures = 0
def check(cond, label):
    global failures
    print(("PASS  " if cond else "FAIL  ") + label)
    if not cond: failures += 1
check(s1 == "xfn_target(alpha: int)", "cross-file sig resolves from cache, warming the index")
check(m1 is not None and "area" in m1 and "perimeter" not in m1,
      "cross-file members resolve from cache (area, no perimeter yet)")
check(s2 == "xfn_target(alpha: int, beta: int)",
      "sig disk change picked up (content-hash invalidation: +beta)")
check(m2 is not None and "area" in m2 and "perimeter" in m2,
      "members disk change picked up (content-hash invalidation: +perimeter)")
sys.exit(1 if failures else 0)
PY
rc=$?
if [ $rc -eq 0 ]; then ok "lsp cross-file signatureHelp+completion cache"; else bad "lsp cross-file signatureHelp+completion cache"; fi

# ---- inlayHint: parameter-name hints at call sites ----
# At each call site within the requested range, label each argument with the
# callee's parameter name (resolved via the signatureHelp machinery — intra-file
# + symindex cross-file, leading `this` dropped). Definitions (function/
# procedure/operator/define) are NOT hinted. One file: a top-level function, a
# method `put(this, slot, item)`, and a caller that calls both.
python3 - <<'PY'
import sys, json, subprocess, os, tempfile
d = tempfile.mkdtemp(prefix="lsp_inlay_")
cp = os.path.join(d, "main.scaly")
main = ("function helper(aa: int) returns int\n{\n    return aa\n}\n"
        "define Bag\n(\n    n: int\n)\n{\n    procedure put(this: Bag, slot: int, item: int)\n    {\n        return\n    }\n}\n"
        "function caller(b: Bag) returns int\n{\n    let r helper(7)\n    b.put(3, 4)\n    return r\n}\n")
open(cp, "w").write(main)
curi = "file://"+cp
def frame(o):
    b = json.dumps(o).encode(); return ("Content-Length: %d\r\n\r\n" % len(b)).encode() + b
def ih(idn, sl, el):
    return frame({"jsonrpc":"2.0","id":idn,"method":"textDocument/inlayHint","params":{
        "textDocument":{"uri":curi},"range":{"start":{"line":sl,"character":0},"end":{"line":el,"character":0}}}})
inp  = frame({"jsonrpc":"2.0","id":1,"method":"initialize","params":{"rootUri":"file://"+d}})
inp += frame({"jsonrpc":"2.0","method":"initialized","params":{}})
inp += ih(2, 0, 30)        # whole document
inp += ih(3, 17, 18)       # narrow range: only the `b.put(3, 4)` line (17)
inp += frame({"jsonrpc":"2.0","id":9,"method":"shutdown"})
inp += frame({"jsonrpc":"2.0","method":"exit"})
out = subprocess.run(["/tmp/scalyls"], input=inp, stdout=subprocess.PIPE).stdout
frames, dd = [], out
while dd:
    i = dd.find(b"\r\n\r\n")
    if i < 0: break
    n = int(dd[:i].decode().split(":")[1].strip())
    frames.append(json.loads(dd[i+4:i+4+n])); dd = dd[i+4+n:]
failures = 0
def check(cond, label):
    global failures
    print(("PASS  " if cond else "FAIL  ") + label)
    if not cond: failures += 1
def result(idn): return next((x for x in frames if x.get("id") == idn), {}).get("result")
caps = (result(1) or {}).get("capabilities", {})
# Not advertised since 2026-09-28 (text woven into the code read as noise);
# the handler still answers a client that asks, which is what this block tests.
check("inlayHintProvider" not in caps, "initialize does not advertise inlayHintProvider")
whole = result(2)
pairs = [(h["label"], h["position"]["line"], h.get("kind")) for h in (whole or [])]
labels = [p[0] for p in pairs]
check(("aa:", 16, 2) in pairs, "intra-file call helper(7) -> hint `aa:` on the call line (kind 2)")
check(("slot:", 17, 2) in pairs, "method call b.put(3,4) -> `slot:` (this dropped)")
check(("item:", 17, 2) in pairs, "method call b.put(3,4) -> `item:`")
check("this:" not in labels, "the dropped `this` is never shown as a hint")
check(len(pairs) == 3, "definitions (function/procedure params) are NOT hinted (exactly 3 call hints)")
check(all(h.get("paddingRight") is True for h in (whole or [])), "hints set paddingRight")
narrow = result(3)
nlabels = sorted(h["label"] for h in (narrow or []))
check(nlabels == ["item:", "slot:"], "narrow range hints only the in-range call (excludes helper on line 16)")
sys.exit(1 if failures else 0)
PY
rc=$?
if [ $rc -eq 0 ]; then ok "lsp inlayHint"; else bad "lsp inlayHint"; fi

# ---- inlayHint: TYPE hints on untyped let/var bindings ----
# A `let NAME EXPR` / `var NAME EXPR` with no `: T` annotation gets an inferred
# `: Type` hint (kind 1, paddingLeft) after the name: a constructor `Foo#(`/`$(`
# -> Foo; a call `foo(..)` -> foo's return type (cross-file resolved); a bare var
# -> its type. Annotated bindings, literals, and primitive-typed results get NO
# hint. Type hints (kind 1) and the param hints (kind 2) coexist on the same line.
# Two files in a temp dir: Point + make_point/make_count live in types.scaly.
python3 - <<'PY'
import sys, json, subprocess, os, tempfile
d = tempfile.mkdtemp(prefix="lsp_ihtype_")
types = ("define Point\n(\n    x: int\n    y: int\n)\n{\n"
         "    function px(this: Point) returns int\n    {\n        return x\n    }\n}\n"
         "function make_point(v: int) returns Point\n{\n    return Point#(v, v)\n}\n"
         "function make_count() returns int\n{\n    return 0\n}\n")
main = ("function run(g: Point) returns int\n{\n"          # 0,1
        "    let p Point#(1, 2)\n"                          # 2  ctor -> : Point
        "    let r make_point(5)\n"                         # 3  call -> : Point (+ param v:)
        "    let c make_count()\n"                          # 4  call -> int (primitive) -> NO
        "    let n 5\n"                                      # 5  literal -> NO
        "    let s p\n"                                      # 6  bare var p -> : Point
        "    let w: int 7\n"                                 # 7  annotated -> NO
        "    return p.px() + r.px() + c + w + n\n}\n")       # 8,9
tp = os.path.join(d, "types.scaly"); open(tp, "w").write(types)
mp = os.path.join(d, "main.scaly");  open(mp, "w").write(main)
muri = "file://"+mp
def frame(o):
    b = json.dumps(o).encode(); return ("Content-Length: %d\r\n\r\n" % len(b)).encode() + b
def ih(idn, sl, el):
    return frame({"jsonrpc":"2.0","id":idn,"method":"textDocument/inlayHint","params":{
        "textDocument":{"uri":muri},"range":{"start":{"line":sl,"character":0},"end":{"line":el,"character":0}}}})
inp  = frame({"jsonrpc":"2.0","id":1,"method":"initialize","params":{"rootUri":"file://"+d}})
inp += frame({"jsonrpc":"2.0","method":"initialized","params":{}})
inp += ih(2, 0, 30)
inp += frame({"jsonrpc":"2.0","id":9,"method":"shutdown"})
inp += frame({"jsonrpc":"2.0","method":"exit"})
out = subprocess.run(["/tmp/scalyls"], input=inp, stdout=subprocess.PIPE,
    env=dict(os.environ, SCALY_HOME=d)).stdout
frames, dd = [], out
while dd:
    i = dd.find(b"\r\n\r\n")
    if i < 0: break
    n = int(dd[:i].decode().split(":")[1].strip())
    frames.append(json.loads(dd[i+4:i+4+n])); dd = dd[i+4+n:]
failures = 0
def check(cond, label):
    global failures
    print(("PASS  " if cond else "FAIL  ") + label)
    if not cond: failures += 1
def result(idn): return next((x for x in frames if x.get("id") == idn), {}).get("result")
whole = result(2) or []
tlines = [h["position"]["line"] for h in whole if h.get("kind") == 1]
def thint(line): return next((h for h in whole if h.get("kind")==1 and h["position"]["line"]==line), None)
check(thint(2) is not None and thint(2)["label"] == ": Point", "ctor `let p Point#()` -> `: Point` (kind 1)")
check(thint(3) is not None and thint(3)["label"] == ": Point", "call `let r make_point()` -> `: Point` (cross-file return type)")
check(thint(6) is not None and thint(6)["label"] == ": Point", "bare var `let s p` -> `: Point`")
check(4 not in tlines, "call returning primitive (`let c make_count()`) -> NO type hint")
check(5 not in tlines, "literal `let n 5` -> NO type hint")
check(7 not in tlines, "annotated `let w: int 7` -> NO type hint")
check(all(h.get("paddingLeft") is True for h in whole if h.get("kind")==1), "type hints set paddingLeft")
phints = [(h["label"], h["position"]["line"]) for h in whole if h.get("kind") == 2]
check(("v:", 3) in phints, "param hint `v:` on make_point(5) coexists with the type hint on `r`")
check(all(h.get("paddingRight") is True for h in whole if h.get("kind")==2), "param hints set paddingRight")
check(all(h.get("kind") in (1, 2) for h in whole), "every hint is kind 1 (type) or 2 (param)")
sys.exit(1 if failures else 0)
PY
rc=$?
if [ $rc -eq 0 ]; then ok "lsp inlayHint types"; else bad "lsp inlayHint types"; fi

# ---- cross-file type-hint inference via the symindex (Step 32) ----
# A `let r foo()` whose callee `foo` is declared in a SIBLING file now resolves
# its return type from the SAME content-hash-cached ws blobs (each routine
# record carries a 4th rettype field) instead of re-parsing the sibling per
# request. This interleaves I/O to prove the cache + invalidation: a cross-file
# return-type binding warms the index (-> `: Alpha`); a disk edit changing the
# sibling's return type (Alpha -> Beta) is picked up on the next identical query
# (content-hash invalidation); a repeated query is stable from the cache.
python3 - <<'PY'
import sys, json, subprocess, os, tempfile
d = tempfile.mkdtemp(prefix="lsp_ihtypecache_")
tp = os.path.join(d, "types.scaly")
mp = os.path.join(d, "main.scaly")
alpha = ("define Alpha\n(\n    a: int\n)\n{\n"
         "    function ax(this: Alpha) returns int\n    {\n        return a\n    }\n}\n")
beta = ("define Beta\n(\n    b: int\n)\n{\n"
        "    function bx(this: Beta) returns int\n    {\n        return b\n    }\n}\n")
types_v1 = alpha + beta + "function make_thing(v: int) returns Alpha\n{\n    return Alpha#(v)\n}\n"
types_v2 = alpha + beta + "function make_thing(v: int) returns Beta\n{\n    return Beta#(v)\n}\n"
main = ("function run(g: int) returns int\n{\n"          # 0,1
        "    let r make_thing(5)\n"                       # 2  call -> cross-file return type
        "    return r.ax()\n}\n")                         # 3
open(tp, "w").write(types_v1)
open(mp, "w").write(main)
muri = "file://"+mp
def frame(o):
    s = json.dumps(o).encode(); return ("Content-Length: %d\r\n\r\n" % len(s)).encode() + s
p = subprocess.Popen(["/tmp/scalyls"], stdin=subprocess.PIPE, stdout=subprocess.PIPE)
def send(o):
    p.stdin.write(frame(o)); p.stdin.flush()
def read_frame():
    hdr = b""
    while b"\r\n\r\n" not in hdr:
        ch = p.stdout.read(1)
        if not ch: return None
        hdr += ch
    n = int(hdr.split(b"\r\n")[0].split(b":")[1].strip())
    return json.loads(p.stdout.read(n))
def result_for(idn):
    while True:
        f = read_frame()
        if f is None: return None
        if f.get("id") == idn: return f.get("result")
def ih(idn):
    return {"jsonrpc":"2.0","id":idn,"method":"textDocument/inlayHint","params":{
        "textDocument":{"uri":muri},"range":{"start":{"line":2,"character":0},"end":{"line":3,"character":0}}}}
def type_hint(r):
    for h in (r or []):
        if h.get("kind") == 1 and h["position"]["line"] == 2:
            return h["label"]
    return None
send({"jsonrpc":"2.0","id":1,"method":"initialize","params":{"rootUri":"file://"+d}})
result_for(1)
send({"jsonrpc":"2.0","method":"initialized","params":{}})
send(ih(2)); t1 = type_hint(result_for(2))            # warm index -> : Alpha
open(tp, "w").write(types_v2)                          # change return type on disk
send(ih(3)); t2 = type_hint(result_for(3))            # invalidation -> : Beta
send(ih(4)); t3 = type_hint(result_for(4))            # cache reuse -> : Beta (stable)
send({"jsonrpc":"2.0","id":9,"method":"shutdown"})
result_for(9)
send({"jsonrpc":"2.0","method":"exit"})
p.wait()
failures = 0
def check(cond, label):
    global failures
    print(("PASS  " if cond else "FAIL  ") + label)
    if not cond: failures += 1
check(t1 == ": Alpha", "cross-file type-hint resolves from cache, warming the index (: Alpha)")
check(t2 == ": Beta", "return-type disk change picked up (content-hash invalidation: : Beta)")
check(t3 == ": Beta", "repeated query stable from cache (: Beta)")
sys.exit(1 if failures else 0)
PY
rc=$?
if [ $rc -eq 0 ]; then ok "lsp cross-file type-hint cache"; else bad "lsp cross-file type-hint cache"; fi

# ---- cross-file chain-segment resolution via the symindex (Step 33) ----
# A field-access chain `r.origin` whose intermediate type lives in a SIBLING
# file now resolves the segment type from the SAME content-hash-cached ws blobs
# (each `define` record carries a 5th membertypes field = its 0x1F-joined
# member-name/type pairs) instead of re-parsing the sibling per segment per
# request. Interleaved I/O proves cache + invalidation: `let p r.origin` (r:
# Rect param, Rect.origin: Point in the sibling) -> `: Point`; a disk edit
# changing Rect.origin's type Point -> Coord is picked up (content-hash
# invalidation); a repeated query is stable from the cache.
python3 - <<'PY'
import sys, json, subprocess, os, tempfile
d = tempfile.mkdtemp(prefix="lsp_chaincache_")
tp = os.path.join(d, "types.scaly"); mp = os.path.join(d, "main.scaly")
point = ("define Point\n(\n    x: int\n    y: int\n)\n{\n    function px(this: Point) returns int\n    {\n        return x\n    }\n}\n")
coord = ("define Coord\n(\n    u: int\n)\n{\n    function cu(this: Coord) returns int\n    {\n        return u\n    }\n}\n")
rect_v1 = ("define Rect\n(\n    origin: Point\n    w: int\n)\n{\n    function area(this: Rect) returns int\n    {\n        return w\n    }\n}\n")
rect_v2 = ("define Rect\n(\n    origin: Coord\n    w: int\n)\n{\n    function area(this: Rect) returns int\n    {\n        return w\n    }\n}\n")
types_v1 = point + coord + rect_v1
types_v2 = point + coord + rect_v2
main = ("function run(r: Rect) returns int\n{\n"   # 0,1
        "    let p r.origin\n"                     # 2 chain r.origin
        "    return p.px()\n}\n")                  # 3
open(tp, "w").write(types_v1); open(mp, "w").write(main)
muri = "file://"+mp
def frame(o):
    s = json.dumps(o).encode(); return ("Content-Length: %d\r\n\r\n" % len(s)).encode() + s
p = subprocess.Popen(["/tmp/scalyls"], stdin=subprocess.PIPE, stdout=subprocess.PIPE)
def send(o):
    p.stdin.write(frame(o)); p.stdin.flush()
def read_frame():
    hdr = b""
    while b"\r\n\r\n" not in hdr:
        ch = p.stdout.read(1)
        if not ch: return None
        hdr += ch
    n = int(hdr.split(b"\r\n")[0].split(b":")[1].strip())
    return json.loads(p.stdout.read(n))
def result_for(idn):
    while True:
        f = read_frame()
        if f is None: return None
        if f.get("id") == idn: return f.get("result")
def ih(idn):
    return {"jsonrpc":"2.0","id":idn,"method":"textDocument/inlayHint","params":{
        "textDocument":{"uri":muri},"range":{"start":{"line":2,"character":0},"end":{"line":3,"character":0}}}}
def th(r):
    for h in (r or []):
        if h.get("kind") == 1 and h["position"]["line"] == 2:
            return h["label"]
    return None
send({"jsonrpc":"2.0","id":1,"method":"initialize","params":{"rootUri":"file://"+d}})
result_for(1)
send({"jsonrpc":"2.0","method":"initialized","params":{}})
send(ih(2)); t1 = th(result_for(2))          # warm index -> : Point
open(tp, "w").write(types_v2)                 # Rect.origin: Point -> Coord on disk
send(ih(3)); t2 = th(result_for(3))          # invalidation -> : Coord
send(ih(4)); t3 = th(result_for(4))          # cache reuse -> : Coord (stable)
send({"jsonrpc":"2.0","id":9,"method":"shutdown"})
result_for(9)
send({"jsonrpc":"2.0","method":"exit"})
p.wait()
failures = 0
def check(cond, label):
    global failures
    print(("PASS  " if cond else "FAIL  ") + label)
    if not cond: failures += 1
check(t1 == ": Point", "cross-file chain `r.origin` resolves the field type from cache (: Point)")
# A mid-session SAME-LENGTH disk edit of a sibling file IS now picked up by the
# chain-segment content-hash cache under BOTH compilers. The former self-hosted
# gap was a content-INDEPENDENT (length-only) hash: hashing.hash used a legacy
# `hash // value` FNV step where `//` is an UNKNOWN operator the self-hosted
# emitter dropped entirely (the XOR of the byte was a no-op), so same-length
# files hashed identically and symindex.valid() spuriously stayed true. Fixed
# by computing the FNV XOR via the identity a^b == (a|b)-(a&b) (`^` is the
# lifetime sigil and cannot be written as a bitwise op). See the
# scalyls-self-host-blocker memory.
check(t2 == ": Coord", "chain segment-type disk change picked up (content-hash invalidation: : Coord)")
check(t3 == ": Coord", "repeated chain query stable from cache (: Coord)")
sys.exit(1 if failures else 0)
PY
rc=$?
if [ $rc -eq 0 ]; then ok "lsp cross-file chain-segment cache"; else bad "lsp cross-file chain-segment cache"; fi

# ---- type-aware completion (variable receivers, lexical) ----
# A variable receiver `v.` resolves `v`'s declared type lexically (constructor
# `var v T#(...)`, annotated `v: T`, parameter `(v: T)`, or `this: T`) and then
# lists that type's members — looking in the current file first, then sibling
# .scaly files (cross-file). A primitive / unresolvable type falls back to the
# flat all-names list. Two files in a temp dir: Widget is defined only in
# types.scaly; main.scaly uses it via a local, a parameter, and an int.
python3 - <<'PY'
import sys, json, subprocess, os, tempfile
d = tempfile.mkdtemp(prefix="lsp_tcompl_")
types = ("define Widget\n(\n    w: int\n)\n{\n"
         "    function area(this: Widget) returns int\n    {\n        return w\n    }\n"
         "    procedure grow(this: Widget)\n    {\n        return\n    }\n"
         "    function inspect(this: Widget) returns int\n    {\n        return this.area()\n    }\n}\n")
main = ("function run(g: Widget) returns int\n{\n"
        "    var local Widget#(3)\n"
        "    var count: int 0\n"
        "    let a local.area()\n"
        "    let b g.area()\n"
        "    let c count.area()\n"
        "    return a + b\n}\n")
tp = os.path.join(d, "types.scaly"); open(tp, "w").write(types)
mp = os.path.join(d, "main.scaly");  open(mp, "w").write(main)
turi = "file://"+tp; muri = "file://"+mp
def frame(o):
    b=json.dumps(o).encode(); return ("Content-Length: %d\r\n\r\n"%len(b)).encode()+b
def comp(idn, uri, line, char):
    return frame({"jsonrpc":"2.0","id":idn,"method":"textDocument/completion",
                  "params":{"textDocument":{"uri":uri},"position":{"line":line,"character":char}}})
def afterdot(src, needle, recv_len):
    idx = src.index(needle) + recv_len            # offset of the dot
    pre = src[:idx]; return pre.count("\n"), (idx - (pre.rfind("\n")+1)) + 1
ll, lc = afterdot(main, "local.area()", 5)        # ctor form, cross-file
gl, gc = afterdot(main, "g.area()", 1)            # param form, cross-file
nl, nc = afterdot(main, "count.area()", 5)        # int -> no members -> flat
hl, hc = afterdot(types, "this.area()", 4)        # this-receiver, intra-file
inp  = frame({"jsonrpc":"2.0","id":1,"method":"initialize","params":{}})
inp += frame({"jsonrpc":"2.0","method":"initialized","params":{}})
inp += comp(2, muri, ll, lc)
inp += comp(3, muri, gl, gc)
inp += comp(4, muri, nl, nc)
inp += comp(5, turi, hl, hc)
inp += frame({"jsonrpc":"2.0","id":9,"method":"shutdown"})
inp += frame({"jsonrpc":"2.0","method":"exit"})
out = subprocess.run(["/tmp/scalyls"], input=inp, stdout=subprocess.PIPE).stdout
frames, dd = [], out
while dd:
    i = dd.find(b"\r\n\r\n")
    if i < 0: break
    n = int(dd[:i].decode().split(":")[1].strip())
    frames.append(json.loads(dd[i+4:i+4+n])); dd = dd[i+4+n:]
failures = 0
def check(cond, label):
    global failures
    print(("PASS  " if cond else "FAIL  ") + label)
    if not cond: failures += 1
def res(idn):
    f = next((x for x in frames if x.get("id") == idn), None)
    return (f or {}).get("result")
def labels(idn):
    return [it["label"] for it in (res(idn) or [])]
def kinds(idn):
    return {it["label"]: it["kind"] for it in (res(idn) or [])}
members = ["w","area","grow","inspect"]   # the field `w` comes first (declaration order)
check(labels(2) == members, "ctor form `var local Widget#(..)` -> Widget members (cross-file)")
check(kinds(2).get("area") == 2, "cross-file member kept its kind (Method=2)")
check(labels(3) == members, "param form `g: Widget` -> Widget members (cross-file)")
check(labels(5) == members, "`this.` inside a method -> enclosing type's members (intra-file)")
check(labels(4) == ["run"], "int receiver `count.` -> flat fallback (no member type)")
sys.exit(1 if failures else 0)
PY
rc=$?
if [ $rc -eq 0 ]; then ok "lsp type-aware completion"; else bad "lsp type-aware completion"; fi

# ---- LSP server: signatureHelp -------------------------------------------
python3 - <<'PY'
import sys, json, subprocess
def frame(o):
    b = json.dumps(o).encode()
    return ("Content-Length: %d\r\n\r\n" % len(b)).encode() + b

import os as _os, shutil as _sh; _ws = "/tmp/lsp_ws/sig"; _sh.rmtree(_ws, ignore_errors=True); _os.makedirs(_ws)
uri = "file://" + _ws + "/sig.scaly"
doc = (
    "function foo(buf: pointer[Page], items: Vector[char], opt: ref[String]?) returns int\n"  # 0
    "{\n"                                                                                       # 1
    "    return 0\n"                                                                            # 2
    "}\n"                                                                                       # 3
    "function noargs() returns int\n"                                                           # 4
    "{\n"                                                                                       # 5
    "    return 1\n"                                                                            # 6
    "}\n"                                                                                       # 7
    "define Box\n"                                                                              # 8
    "(\n"                                                                                       # 9
    "    v: int\n"                                                                              # 10
    ")\n"                                                                                       # 11
    "{\n"                                                                                       # 12
    "    function put(this: Box, key: String, value: int) returns int\n"                       # 13
    "    {\n"                                                                                   # 14
    "        return value\n"                                                                    # 15
    "    }\n"                                                                                   # 16
    "}\n"                                                                                       # 17
    "function run()\n"                                                                          # 18
    "{\n"                                                                                       # 19
    "    foo(a, b, c)\n"                                                                        # 20
    "    noargs()\n"                                                                            # 21
    "    box.put(k, v)\n"                                                                       # 22
    "}\n")                                                                                      # 23

def sig(idn, ln, ch):
    return frame({"jsonrpc":"2.0","id":idn,"method":"textDocument/signatureHelp","params":{
        "textDocument":{"uri":uri},"position":{"line":ln,"character":ch}}})

inp  = frame({"jsonrpc":"2.0","id":1,"method":"initialize","params":{}})
inp += frame({"jsonrpc":"2.0","method":"initialized","params":{}})
inp += frame({"jsonrpc":"2.0","method":"textDocument/didOpen","params":{"textDocument":{
        "uri":uri,"languageId":"scaly","version":1,"text":doc}}})
inp += sig(2, 20, 9)    # foo(a,b,c): cursor on a  -> active 0
inp += sig(3, 20, 15)   # foo(a,b,c): cursor on c  -> active 2
inp += sig(4, 21, 11)   # noargs() inside          -> 0 params
inp += sig(5, 22, 14)   # box.put(k,v): cursor on v -> active 1, `this` dropped
inp += sig(6, 18, 11)   # not inside any call       -> null
inp += frame({"jsonrpc":"2.0","id":9,"method":"shutdown"})
inp += frame({"jsonrpc":"2.0","method":"exit"})

out = subprocess.run(["/tmp/scalyls"], input=inp, stdout=subprocess.PIPE).stdout
frames, d = [], out
while d:
    i = d.find(b"\r\n\r\n")
    if i < 0: break
    n = int(d[:i].decode().split(":")[1].strip())
    frames.append(json.loads(d[i+4:i+4+n])); d = d[i+4+n:]

failures = 0
def check(cond, label):
    global failures
    print(("PASS  " if cond else "FAIL  ") + label)
    if not cond: failures += 1
def res(idn):
    f = next((x for x in frames if x.get("id") == idn), None)
    return (f or {}).get("result")

caps = frames[0]["result"]["capabilities"]
check("signatureHelpProvider" in caps, "advertises signatureHelpProvider")
check(caps["signatureHelpProvider"]["triggerCharacters"] == ["(", ","], "trigger chars ( and ,")

r2 = res(2)
check(r2["signatures"][0]["label"] == "foo(buf: pointer[Page], items: Vector[char], opt: ref[String]?)", "full signature label with complex types")
check([p["label"] for p in r2["signatures"][0]["parameters"]] == ["buf: pointer[Page]","items: Vector[char]","opt: ref[String]?"], "parameter labels reconstructed")
check(r2["activeParameter"] == 0, "first arg -> activeParameter 0")
check(res(3)["activeParameter"] == 2, "third arg -> activeParameter 2")

r4 = res(4)
check(r4["signatures"][0]["label"] == "noargs()" and r4["signatures"][0]["parameters"] == [], "no-arg call -> empty parameter list")

r5 = res(5)
check(r5["signatures"][0]["label"] == "put(key: String, value: int)", "method call drops the `this` receiver param")
check(r5["activeParameter"] == 1, "second written arg -> activeParameter 1 (value)")

check(res(6) is None, "cursor not inside a call -> result null")
sys.exit(1 if failures else 0)
PY
rc=$?
if [ $rc -eq 0 ]; then ok "lsp signatureHelp"; else bad "lsp signatureHelp"; fi

# ---- LSP server: cross-package definition + signatureHelp ----------------
# SCALY_HOME points at the repo so the worker scans packages/scaly (the stdlib)
# when a symbol is not found in the current file's directory tree. The open
# document lives in /tmp (NOT under packages/), so a hit can only come from the
# cross-package search.
SCALY_HOME="$(pwd)" python3 - <<'PY'
import sys, json, subprocess, os
def frame(o):
    b = json.dumps(o).encode()
    return ("Content-Length: %d\r\n\r\n" % len(b)).encode() + b

import os as _os, shutil as _sh; _ws = "/tmp/lsp_ws/xpkg"; _sh.rmtree(_ws, ignore_errors=True); _os.makedirs(_ws)
uri = "file://" + _ws + "/xpkg.scaly"
doc = ("function f()\n"               # 0
       "{\n"                          # 1
       "    var sb StringBuilder()\n" # 2
       "    sb.append(c)\n"           # 3
       "}\n")                         # 4

inp  = frame({"jsonrpc":"2.0","id":1,"method":"initialize","params":{}})
inp += frame({"jsonrpc":"2.0","method":"initialized","params":{}})
inp += frame({"jsonrpc":"2.0","method":"textDocument/didOpen","params":{"textDocument":{
        "uri":uri,"languageId":"scaly","version":1,"text":doc}}})
inp += frame({"jsonrpc":"2.0","id":2,"method":"textDocument/definition","params":{
        "textDocument":{"uri":uri},"position":{"line":2,"character":15}}})
inp += frame({"jsonrpc":"2.0","id":3,"method":"textDocument/signatureHelp","params":{
        "textDocument":{"uri":uri},"position":{"line":3,"character":14}}})
inp += frame({"jsonrpc":"2.0","id":9,"method":"shutdown"})
inp += frame({"jsonrpc":"2.0","method":"exit"})

out = subprocess.run(["/tmp/scalyls"], input=inp, stdout=subprocess.PIPE,
                     env={**os.environ, "SCALY_HOME": os.getcwd()}).stdout
frames, d = [], out
while d:
    i = d.find(b"\r\n\r\n")
    if i < 0: break
    n = int(d[:i].decode().split(":")[1].strip())
    frames.append(json.loads(d[i+4:i+4+n])); d = d[i+4+n:]

failures = 0
def check(cond, label):
    global failures
    print(("PASS  " if cond else "FAIL  ") + label)
    if not cond: failures += 1
def res(idn):
    f = next((x for x in frames if x.get("id") == idn), None)
    return (f or {}).get("result")

r2 = res(2)
check(r2 is not None and "packages/scaly" in r2.get("uri",""),
      "definition: StringBuilder resolves into packages/scaly (cross-package)")
r3 = res(3)
check(r3 is not None and r3["signatures"][0]["label"] == "append(character: char)",
      "signatureHelp: StringBuilder.append resolves cross-package (this dropped)")
sys.exit(1 if failures else 0)
PY
rc=$?
if [ $rc -eq 0 ]; then ok "lsp cross-package definition+signatureHelp"; else bad "lsp cross-package definition+signatureHelp"; fi

# ---- LSP server: `module NAME` must not shadow `define NAME` --------------
# A `module NAME` statement is a LOAD DIRECTIVE, not a definition of NAME: the
# concept lives in the file the module names. packages/scaly/0.1.0/scaly/
# containers.scaly declares twenty of them and `find` lists it ahead of the
# whole containers/ subdirectory holding the concepts, so a definition search
# used to stop at `module Array` (containers.scaly:11) and never reach
# `define Array[T]` (containers/Array.scaly). Three properties:
#   1. cross-package: Array/StringBuilder land in the concept's OWN file;
#   2. intra-file: containers.scaly declares `module Vector` AND uses
#      Vector[int] — the use must still resolve to containers/Vector.scaly,
#      and so must the cursor sitting on the `module Vector` line itself;
#   3. a name that is ONLY ever a module (`runtime`, no `define runtime`
#      anywhere) still answers the `module` line — the fallback is load-bearing,
#      losing it would turn a working jump into null.
# The uri is asserted EXACTLY, not just "packages/scaly" — the old, wrong
# answer was inside packages/scaly too.
SCALY_HOME="$(pwd)" python3 - <<'PY'
import sys, json, subprocess, os
def frame(o):
    b = json.dumps(o).encode()
    return ("Content-Length: %d\r\n\r\n" % len(b)).encode() + b

def define_at(uri, doc, line, character):
    inp  = frame({"jsonrpc":"2.0","id":1,"method":"initialize","params":{}})
    inp += frame({"jsonrpc":"2.0","method":"initialized","params":{}})
    inp += frame({"jsonrpc":"2.0","method":"textDocument/didOpen","params":{"textDocument":{
            "uri":uri,"languageId":"scaly","version":1,"text":doc}}})
    inp += frame({"jsonrpc":"2.0","id":2,"method":"textDocument/definition","params":{
            "textDocument":{"uri":uri},"position":{"line":line,"character":character}}})
    inp += frame({"jsonrpc":"2.0","id":9,"method":"shutdown"})
    inp += frame({"jsonrpc":"2.0","method":"exit"})
    out = subprocess.run(["/tmp/scalyls"], input=inp, stdout=subprocess.PIPE).stdout
    d = out
    while d:
        i = d.find(b"\r\n\r\n")
        if i < 0: break
        n = int(d[:i].decode().split(":")[1].strip())
        f = json.loads(d[i+4:i+4+n]); d = d[i+4+n:]
        if f.get("id") == 2: return f.get("result")
    return None

failures = 0
def check(cond, label):
    global failures
    print(("PASS  " if cond else "FAIL  ") + label)
    if not cond: failures += 1

home = os.getcwd()
def rel(r):
    if not r: return None
    return r["uri"].replace("file://" + home + "/", "")

# Own empty directory per document: the doc's dir tree is the fallback walk
# root, so a shared scratch dir would let a sibling fixture answer instead of
# packages/ — which is the whole claim of each check below.
import shutil as _sh
_ws = "/tmp/lsp_ws/moddef"; _sh.rmtree(_ws, ignore_errors=True); os.makedirs(_ws)
def _u(name): return "file://" + _ws + "/" + name

# 1. cross-package (doc outside packages/, so the hit can only come from there)
r = define_at(_u("moddef1.scaly"),
              "function f()\n{\n    var a Array[int]()\n}\n", 2, 12)
check(rel(r) == "packages/scaly/0.1.0/scaly/containers/Array.scaly",
      "definition: Array resolves to `define Array[T]`, not `module Array`")
r = define_at(_u("moddef2.scaly"),
              "function f()\n{\n    var s StringBuilder()\n}\n", 2, 12)
check(rel(r) == "packages/scaly/0.1.0/scaly/containers/StringBuilder.scaly",
      "definition: StringBuilder resolves to its own file, not `module StringBuilder`")

# 2. intra-file: the declaring file both lists the module and uses the concept
cpath = os.path.join(home, "packages/scaly/0.1.0/scaly/containers.scaly")
cdoc  = open(cpath).read()
clines = cdoc.split("\n")
use_line = next(i for i, l in enumerate(clines) if "var vector Vector[int](2)" in l)
mod_line = next(i for i, l in enumerate(clines) if l.strip() == "module Vector")
r = define_at("file://" + cpath, cdoc, use_line, clines[use_line].index("Vector[") + 2)
check(rel(r) == "packages/scaly/0.1.0/scaly/containers/Vector.scaly",
      "definition: a Vector[int] USE inside containers.scaly skips its own `module Vector`")
r = define_at("file://" + cpath, cdoc, mod_line, clines[mod_line].index("Vector") + 2)
check(rel(r) == "packages/scaly/0.1.0/scaly/containers/Vector.scaly",
      "definition: the cursor ON `module Vector` jumps INTO the module's file")

# 3. module-only name: the `module` line is the only answer there is
r = define_at(_u("moddef3.scaly"),
              "function f()\n{\n    let r runtime\n}\n", 2, 12)
check(rel(r) == "packages/scaly/0.1.0/scaly/memory.scaly",
      "definition: `runtime` (module with no same-named define) still answers the module line")

sys.exit(1 if failures else 0)
PY
rc=$?
if [ $rc -eq 0 ]; then ok "lsp module-vs-define definition"; else bad "lsp module-vs-define definition"; fi

# ---- LSP server: a hover must not report ANOTHER FILE's type --------------
# Model.Span is a bare (start, end) byte pair with no file in it, and the plan
# built for a document also holds every routine the planner instantiated on its
# behalf — the prelude's, and every method of every generic the document's types
# pull in. Their spans index THOSE files and collide with the document's own
# offsets, so a walk comparing numbers alone reports one of them. Measured on
# packages/scaly/0.1.0/scaly/containers/Vector.scaly: EVERY hover in the file
# answered `bool`, the return type of Slice[T].equals, whose span 1328..2073 in
# containers/Slice.scaly covers most of `define Vector[T]`. The file is nothing
# but generic templates, so its own bodies plan to empty stubs and no local node
# competed. PlannedFunction.file cannot decide this — an instantiation carries
# the REQUESTING file — so semantic.is_routine_start# joins on the span START,
# which Modeler.build_function# copies verbatim from the routine's own syntax.
#
# The file is a real tree file, so the lines are found by CONTENT, not number.
# Both directions are asserted: nothing may come from another file (1-3), and
# the semantic layer must still ANSWER inside this file (4-5) — deleting the
# walk would satisfy the first three on its own.
SCALY_HOME="$(pwd)" python3 - <<'PY'
import sys, json, subprocess, os

def frame(o):
    b = json.dumps(o).encode()
    return ("Content-Length: %d\r\n\r\n" % len(b)).encode() + b

path = os.path.join(os.getcwd(), "packages/scaly/0.1.0/scaly/containers/Vector.scaly")
uri  = "file://" + path
doc  = open(path).read()
lines = doc.split("\n")

def line_of(needle):
    return next(i for i, l in enumerate(lines) if l.strip() == needle)

# Row 2 hovers a PROPERTY DECLARATION. Its expectation used to be `Vector`,
# because a property declaration fell through to the enclosing concept; since
# the structure walk landed it answers the property itself, and the claim of
# this test — an answer that can only have come from THIS file — is what the
# new string is checked for, not the concept name it used as a proxy.
probes = [
    (line_of("define Vector[T]"),        "Vector",  "Vector"),
    (line_of("length: size_t"),          "length",  "property length: size_t"),
    (line_of("define VectorIterator[T]"), "VectorIterator", "VectorIterator"),
    (line_of("set length: len"),         "len",     "size_t"),
    (line_of("set position: position + 1"), "position", "size_t"),
]

inp  = frame({"jsonrpc":"2.0","id":1,"method":"initialize","params":{}})
inp += frame({"jsonrpc":"2.0","method":"initialized","params":{}})
inp += frame({"jsonrpc":"2.0","method":"textDocument/didOpen","params":{"textDocument":{
        "uri":uri,"languageId":"scaly","version":1,"text":doc}}})
for k, (ln, word, _) in enumerate(probes):
    inp += frame({"jsonrpc":"2.0","id":100+k,"method":"textDocument/hover","params":{
            "textDocument":{"uri":uri},
            "position":{"line":ln,"character":lines[ln].index(word)+1}}})
inp += frame({"jsonrpc":"2.0","id":9,"method":"shutdown"})
inp += frame({"jsonrpc":"2.0","method":"exit"})

out = subprocess.run(["/tmp/scalyls"], input=inp, stdout=subprocess.PIPE).stdout
res, d = {}, out
while d:
    i = d.find(b"\r\n\r\n")
    if i < 0: break
    n = int(d[:i].decode().split(":")[1].strip())
    f = json.loads(d[i+4:i+4+n]); d = d[i+4+n:]
    if isinstance(f.get("id"), int) and f["id"] >= 100: res[f["id"]] = f.get("result")

failures = 0
def check(cond, label):
    global failures
    print(("PASS  " if cond else "FAIL  ") + label)
    if not cond: failures += 1

for k, (ln, word, expect) in enumerate(probes):
    r = res.get(100+k)
    v = (r or {}).get("contents", {}).get("value") if r else None
    check(v is not None and expect in v,
          "hover on `%s` (line %d) reports %r, got %r" % (word, ln+1, expect, v))

sys.exit(1 if failures else 0)
PY
rc=$?
if [ $rc -eq 0 ]; then ok "lsp hover stays inside the document"; else bad "lsp hover stays inside the document"; fi

# ---- LSP server: hover resolves PER TOKEN, with generic arguments ---------
# Three defects that made a whole line report one wrong type, all on
# containers/Vector.scaly line 69, `let own_page Page.get(this)`:
#
#  1. type_name# returned PlannedType.name, the BARE name — the resolved
#     arguments sit beside it in .generics — so a `pointer[Page]` printed as
#     `pointer` and a `Vector[int]` as `Vector`. Planner.get_readable_name# is
#     the compiler's own renderer and recurses into nested arguments.
#  2. walk_operand# pruned the descent unless the operand's own span contained
#     the offset, on the premise that children nest inside their parent. Planner
#     operands are NOT a containment hierarchy: an `As` operand spans only its
#     trailing `as <type>`, a Call operand stops after the callee name, and the
#     arguments lie outside both. The walk therefore never got below block/if
#     level and answered the enclosing block's type for every token of the line.
#  3. That block type is not `void` (an init body carries `pointer[void]`), so
#     the "skip void wrappers" guard never fired — a Block is now suppressed
#     structurally, and a cursor on an UNTYPED binding's name (item_type is null,
#     and no initializer operand covers the name) answers with the type its
#     initializer produces.
#
# Asserted EXACTLY: a substring check would pass `ref` for `ref[Page]`,
# which is the bug. Lines are found by CONTENT, the file is a real tree file.
SCALY_HOME="$(pwd)" python3 - <<'PY'
import sys, json, subprocess, os

def frame(o):
    b = json.dumps(o).encode()
    return ("Content-Length: %d\r\n\r\n" % len(b)).encode() + b

path = os.path.join(os.getcwd(), "packages/scaly/0.1.0/scaly/containers/Vector.scaly")
uri  = "file://" + path
doc  = open(path).read()
lines = doc.split("\n")

def line_of(needle):
    return next(i for i, l in enumerate(lines) if l.strip() == needle)

bind = line_of("let own_page Page.get(this)")
alloc = line_of("set data: own_page.allocate(len * sizeof T, alignof T) as pointer[T]")
cond = line_of("if len > 0")

# (line, token, occurrence index, expected type)
probes = [
    (bind,  "own_page", 0, "ref[Page]"),           # untyped binding NAME (Page.get answers ref[Page] since 2026-09-05)
    (bind,  "Page",     0, "ref[Page]"),           # the call it is bound to
    (bind,  "this",     0, "pointer[Vector[T]]"),  # nested generic argument
    (alloc, "len",      0, "size_t"),              # an argument INSIDE the call
    (alloc, "data",     0, "pointer[T]"),          # the assignment target
    # An OPERATOR call's span covers its LEFT operand's text alone, so the
    # operand and the operator tie on width. Preferring the outer one — right
    # for `recv.method()`, where the receiver carries the call's span — reported
    # the comparison's `bool` here. PlannedCall.is_operator breaks the tie.
    (cond,  "len",      0, "size_t"),
]

def col_of(ln, token, nth):
    i, l = -1, lines[ln]
    for _ in range(nth + 1):
        i = l.index(token, i + 1)
    return i + 1

inp  = frame({"jsonrpc":"2.0","id":1,"method":"initialize","params":{}})
inp += frame({"jsonrpc":"2.0","method":"initialized","params":{}})
inp += frame({"jsonrpc":"2.0","method":"textDocument/didOpen","params":{"textDocument":{
        "uri":uri,"languageId":"scaly","version":1,"text":doc}}})
for k, (ln, token, nth, _) in enumerate(probes):
    inp += frame({"jsonrpc":"2.0","id":100+k,"method":"textDocument/hover","params":{
            "textDocument":{"uri":uri},
            "position":{"line":ln,"character":col_of(ln, token, nth)}}})
inp += frame({"jsonrpc":"2.0","id":9,"method":"shutdown"})
inp += frame({"jsonrpc":"2.0","method":"exit"})

out = subprocess.run(["/tmp/scalyls"], input=inp, stdout=subprocess.PIPE).stdout
res, d = {}, out
while d:
    i = d.find(b"\r\n\r\n")
    if i < 0: break
    n = int(d[:i].decode().split(":")[1].strip())
    f = json.loads(d[i+4:i+4+n]); d = d[i+4+n:]
    if isinstance(f.get("id"), int) and f["id"] >= 100: res[f["id"]] = f.get("result")

failures = 0
def check(cond, label):
    global failures
    print(("PASS  " if cond else "FAIL  ") + label)
    if not cond: failures += 1

for k, (ln, token, nth, expect) in enumerate(probes):
    r = res.get(100+k)
    v = (r or {}).get("contents", {}).get("value") if r else None
    check(v == expect,
          "hover on `%s` (line %d) is exactly %r, got %r" % (token, ln+1, expect, v))

sys.exit(1 if failures else 0)
PY
rc=$?
if [ $rc -eq 0 ]; then ok "lsp hover per token + generic arguments"; else bad "lsp hover per token + generic arguments"; fi

# ---- LSP server: cross-package member completion -------------------------
# SCALY_HOME points at the repo so member completion scans packages/scaly when
# the receiver's type/concept is not in the current file's dir tree. The doc is
# in /tmp (NOT under packages/), so any member hit comes from the cross-package
# search. `sb.` is a variable receiver (Case 2: resolve type StringBuilder);
# `String.` is a literal concept-name receiver (Case 3: workspace/package).
SCALY_HOME="$(pwd)" python3 - <<'PY'
import sys, json, subprocess, os
def frame(o):
    b = json.dumps(o).encode()
    return ("Content-Length: %d\r\n\r\n" % len(b)).encode() + b

import os as _os, shutil as _sh; _ws = "/tmp/lsp_ws/xpkgmem"; _sh.rmtree(_ws, ignore_errors=True); _os.makedirs(_ws)
uri = "file://" + _ws + "/xpkgmem.scaly"
doc = ("function f()\n"               # 0
       "{\n"                          # 1
       "    var sb StringBuilder()\n" # 2
       "    sb.x\n"                    # 3
       "    String.x\n"               # 4
       "}\n")                         # 5

inp  = frame({"jsonrpc":"2.0","id":1,"method":"initialize","params":{}})
inp += frame({"jsonrpc":"2.0","method":"initialized","params":{}})
inp += frame({"jsonrpc":"2.0","method":"textDocument/didOpen","params":{"textDocument":{
        "uri":uri,"languageId":"scaly","version":1,"text":doc}}})
inp += frame({"jsonrpc":"2.0","id":2,"method":"textDocument/completion","params":{
        "textDocument":{"uri":uri},"position":{"line":3,"character":7}}})   # sb.
inp += frame({"jsonrpc":"2.0","id":3,"method":"textDocument/completion","params":{
        "textDocument":{"uri":uri},"position":{"line":4,"character":11}}})  # String.
inp += frame({"jsonrpc":"2.0","id":9,"method":"shutdown"})
inp += frame({"jsonrpc":"2.0","method":"exit"})

out = subprocess.run(["/tmp/scalyls"], input=inp, stdout=subprocess.PIPE,
                     env={**os.environ, "SCALY_HOME": os.getcwd()}).stdout
frames, d = [], out
while d:
    i = d.find(b"\r\n\r\n")
    if i < 0: break
    n = int(d[:i].decode().split(":")[1].strip())
    frames.append(json.loads(d[i+4:i+4+n])); d = d[i+4+n:]

failures = 0
def check(cond, label):
    global failures
    print(("PASS  " if cond else "FAIL  ") + label)
    if not cond: failures += 1
def labels(idn):
    f = next((x for x in frames if x.get("id") == idn), None)
    r = (f or {}).get("result")
    return [it["label"] for it in r] if isinstance(r, list) else None

l2 = labels(2)
check(l2 is not None and "append" in l2 and "to_string" in l2 and "f" not in l2,
      "sb. -> StringBuilder members cross-package (variable receiver, not flat)")
l3 = labels(3)
check(l3 is not None and "substring" in l3 and "equals" in l3 and "f" not in l3,
      "String. -> String members cross-package (concept-name receiver, not flat)")
sys.exit(1 if failures else 0)
PY
rc=$?
if [ $rc -eq 0 ]; then ok "lsp cross-package member completion"; else bad "lsp cross-package member completion"; fi

# ---- real type-aware completion (field-access chains + call results) ----
# Beyond the single-identifier variable receiver (Step 17), the resolver now
# follows the declared type GRAPH: a field-access chain `r.origin.` resolves
# r:Rect -> field origin:Point -> Point's members, and a call-result binding
# `let p make()` resolves to make()'s return type Point. Both list ONLY Point's
# own field and member (x, px), proving real type resolution rather than the flat
# all-names list.
python3 - <<'PY'
import sys, json, subprocess, os, tempfile
d = tempfile.mkdtemp(prefix="lsp_realtc_")
src = (
"define Point\n(\n    x: int\n)\n{\n"
"    function px(this: Point) returns int\n    {\n        return x\n    }\n}\n"
"define Rect\n(\n    origin: Point\n)\n{\n"
"    function area(this: Rect) returns int\n    {\n        return 0\n    }\n}\n"
"function make() returns Point\n{\n    return Point(1)\n}\n"
"function run(r: Rect) returns int\n{\n"
"    r.origin.z\n"          # chain: r:Rect -> origin:Point -> Point members
"    let p make()\n"
"    p.z\n"                 # call-result: p = make() : Point
"    return 0\n}\n")
fp = os.path.join(d, "m.scaly"); open(fp, "w").write(src)
uri = "file://"+fp
def frame(o):
    b = json.dumps(o).encode(); return ("Content-Length: %d\r\n\r\n" % len(b)).encode() + b
def afterdot(needle, recvlen):
    idx = src.index(needle) + recvlen; pre = src[:idx]; return pre.count("\n"), idx - (pre.rfind("\n")+1) + 1
cl, cc = afterdot("r.origin.z", 8)    # cursor after "r.origin."
pl, pc = afterdot("p.z", 1)           # cursor after "p."
inp  = frame({"jsonrpc":"2.0","id":1,"method":"initialize","params":{}})
inp += frame({"jsonrpc":"2.0","method":"initialized","params":{}})
inp += frame({"jsonrpc":"2.0","method":"textDocument/didOpen","params":{"textDocument":{
        "uri":uri,"languageId":"scaly","version":1,"text":src}}})
inp += frame({"jsonrpc":"2.0","id":2,"method":"textDocument/completion","params":{
        "textDocument":{"uri":uri},"position":{"line":cl,"character":cc}}})
inp += frame({"jsonrpc":"2.0","id":3,"method":"textDocument/completion","params":{
        "textDocument":{"uri":uri},"position":{"line":pl,"character":pc}}})
inp += frame({"jsonrpc":"2.0","id":9,"method":"shutdown"})
inp += frame({"jsonrpc":"2.0","method":"exit"})
out = subprocess.run(["/tmp/scalyls"], input=inp, stdout=subprocess.PIPE).stdout
frames, dd = [], out
while dd:
    i = dd.find(b"\r\n\r\n")
    if i < 0: break
    n = int(dd[:i].decode().split(":")[1].strip())
    frames.append(json.loads(dd[i+4:i+4+n])); dd = dd[i+4+n:]
failures = 0
def check(cond, label):
    global failures
    print(("PASS  " if cond else "FAIL  ") + label)
    if not cond: failures += 1
def labels(idn):
    r = next((x for x in frames if x.get("id") == idn), {}).get("result")
    return [it["label"] for it in r] if isinstance(r, list) else None
l2 = labels(2)
check(l2 == ["x","px"], "field-access chain r.origin. -> Point members only (not flat)")
l3 = labels(3)
check(l3 == ["x","px"], "call-result `let p make()` p. -> make()'s return type Point")
sys.exit(1 if failures else 0)
PY
rc=$?
if [ $rc -eq 0 ]; then ok "lsp real type-aware completion"; else bad "lsp real type-aware completion"; fi

# ---- foldingRange: collapsible regions for a file on disk ----
# Parse-only: every multi-line declaration with a body yields one FoldingRange
# {startLine,endLine}. The request carries only a uri, so the worker re-reads
# the file. Exercises a top-level function, a struct with a nested method
# (nested fold), a union (folds to the closing paren), a single-line mutable
# (NOT folded), and confirms endLine lands on the closing brace (not the next
# token's line, despite the syntax-node end-offset overshoot).
python3 - <<'PY'
import sys, json, subprocess
src = ("function add(a: int, b: int) returns int\n{\n    return a + b\n}\n\n"   # 0..3
       "mutable counter: int 0\n\n"                                            # 5 (single-line)
       "define Point\n(\n    x: int\n)\n{\n"                                    # 7.. open at 11
       "    function get_x(this: Point) returns int\n    {\n        return x\n    }\n}\n\n"  # 12..16
       "define Shape union (\n    Circle: int\n    Square: int\n)\n")          # 18..21
import os as _os, shutil as _sh; _ws = "/tmp/lsp_ws/fold_test"; _sh.rmtree(_ws, ignore_errors=True); _os.makedirs(_ws)
path = _ws + "/lsp_fold_test.scaly"
open(path, "w").write(src)
def frame(o):
    b=json.dumps(o).encode(); return ("Content-Length: %d\r\n\r\n"%len(b)).encode()+b
inp  = frame({"jsonrpc":"2.0","id":1,"method":"initialize","params":{}})
inp += frame({"jsonrpc":"2.0","method":"initialized","params":{}})
inp += frame({"jsonrpc":"2.0","id":2,"method":"textDocument/foldingRange",
              "params":{"textDocument":{"uri":"file://"+path}}})
inp += frame({"jsonrpc":"2.0","id":9,"method":"shutdown"})
inp += frame({"jsonrpc":"2.0","method":"exit"})
out = subprocess.run(["/tmp/scalyls"], input=inp, stdout=subprocess.PIPE).stdout
frames, d = [], out
while d:
    i = d.find(b"\r\n\r\n")
    if i < 0: break
    n = int(d[:i].decode().split(":")[1].strip())
    frames.append(json.loads(d[i+4:i+4+n])); d = d[i+4+n:]
failures = 0
def check(cond, label):
    global failures
    print(("PASS  " if cond else "FAIL  ") + label)
    if not cond: failures += 1
caps = frames[0]["result"]["capabilities"]
check(caps.get("foldingRangeProvider") is True, "initialize advertises foldingRangeProvider")
r = next((f for f in frames if f.get("id") == 2), None)
ranges = (r or {}).get("result")
check(isinstance(ranges, list), "foldingRange -> array result")
ranges = ranges or []
pairs = [(x["startLine"], x["endLine"]) for x in ranges]
check((0,3) in pairs, "function folds from signature to closing brace (0..3)")
check((7,16) in pairs, "struct folds whole body to closing brace (7..16)")
check((12,15) in pairs, "nested method folds inside the struct (12..15)")
check((18,21) in pairs, "union folds to its closing paren (18..21)")
check(all(s != 5 for (s,e) in pairs), "single-line mutable is not folded")
check(all(e > s for (s,e) in pairs), "every range spans more than one line")
sys.exit(1 if failures else 0)
PY
rc=$?
if [ $rc -eq 0 ]; then ok "lsp foldingRange"; else bad "lsp foldingRange"; fi

# ---- codeLens: the self-scaling verdict of every `for` loop ----
# Since milestone 4.4 a `for` whose body passes the TaskPlanner classification
# is emitted through the adaptive parallel driver and a blocked one runs
# sequentially, with NO diagnostic either way — so the lens is the only editor
# surface for the single most consequential thing the compiler decides about a
# loop (scalyls/tasklens.scaly).
#
# The groups below are each other's controls:
#   * a standalone document       -> the buffer IS the planned program
#   * a package member            -> planned through its ROOT (from disk), so
#                                    the verdicts and the buffer come from
#                                    different texts and must be PAIRED
#   * comment/string `for`s       -> the noncode skip is load-bearing: without
#                                    it this file has FIVE sites and the one
#                                    real verdict lands on the wrong line
#   * an undeclared module file   -> the reachable "no verdict" case
#   * a file with no `for` at all -> []
# The last group is the sharpest: the same verdicts are computed a SECOND time
# by a different program (`scalyc --plan --task-plan`) and compared line by
# line. A cross-producer check is what makes "the lens agrees with the
# compiler" an assertion rather than a hope.
( ulimit -s 65520; "$SCALYC" --plan --task-plan packages/scaly/0.1.0/scaly.scaly ) \
    > /tmp/lsp_taskplan.txt 2>/dev/null || true
python3 - <<'PY'
import sys, json, subprocess, os, shutil
failures = 0
def check(cond, label):
    global failures
    print(("PASS  " if cond else "FAIL  ") + label)
    if not cond: failures += 1
def frame(o):
    b=json.dumps(o).encode(); return ("Content-Length: %d\r\n\r\n"%len(b)).encode()+b

# One server run: didOpen `path` with `buffer` (defaults to the file), then ask
# for its lenses. Answers (capabilities, lens list).
def lenses(path, buffer=None):
    if buffer is None: buffer = open(path).read()
    uri = "file://" + os.path.abspath(path)
    inp  = frame({"jsonrpc":"2.0","id":1,"method":"initialize","params":{}})
    inp += frame({"jsonrpc":"2.0","method":"initialized","params":{}})
    inp += frame({"jsonrpc":"2.0","method":"textDocument/didOpen","params":{
                  "textDocument":{"uri":uri,"languageId":"scaly","version":1,"text":buffer}}})
    inp += frame({"jsonrpc":"2.0","id":2,"method":"textDocument/codeLens",
                  "params":{"textDocument":{"uri":uri}}})
    inp += frame({"jsonrpc":"2.0","id":9,"method":"shutdown"})
    inp += frame({"jsonrpc":"2.0","method":"exit"})
    out = subprocess.run(["/tmp/scalyls"], input=inp, stdout=subprocess.PIPE).stdout
    frames, d = [], out
    while d:
        i = d.find(b"\r\n\r\n")
        if i < 0: break
        n = int(d[:i].decode().split(":")[1].strip())
        frames.append(json.loads(d[i+4:i+4+n])); d = d[i+4+n:]
    caps = frames[0]["result"]["capabilities"] if frames else {}
    r = next((f for f in frames if f.get("id") == 2), None)
    res = (r or {}).get("result")
    if not isinstance(res, list): res = None
    return caps, res
# (1-based line, title) per lens, in reply order — restricted to the `for`-loop
# family this group is about. The plan-free families (Run, generated banner,
# package/module) live in the next group; a program fixture legitimately carries
# a Run lens too, and asserting an exact row list here would make each group's
# expectations depend on the other's behaviour.
def rows(ls):
    out = []
    for l in (ls or []):
        t = l["command"]["title"]
        if t.startswith("runs ") or t.startswith("no verdict"):
            out.append((l["range"]["start"]["line"] + 1, t))
    return out
def verdict_lenses(ls):
    return [l for l in (ls or [])
            if l["command"]["title"].startswith("runs ")
            or l["command"]["title"].startswith("no verdict")]

ws = "/tmp/lsp_ws/codelens"
shutil.rmtree(ws, ignore_errors=True)
os.makedirs(ws)

# ---- a standalone document: buffer == planned program --------------------
doc = ws + "/doc.scaly"
open(doc, "w").write(
    "function double_all(n: int, out: Vector[int])\n"
    "{\n"
    "    for i in n\n"                                  # 3: disjoint Vector slot
    "        out.put(i, i * 2)\n"
    "}\n"
    "\n"
    "function total(n: int) returns int\n"
    "{\n"
    "    var t 0\n"
    "    for x in n\n"                                  # 10: writes t (the LAST x --
    "        set t: x\n"                                #   a sum would be a reduction)
    "    t\n"
    "}\n"
    "\n"
    "var results Vector[int](4)\n"
    "double_all(4, results)\n"
    "if total(4) <> 3\n"
    "    exit 1\n"
    "print \"PASS\"\n")
caps, ls = lenses(doc)
check(caps.get("codeLensProvider") == {"resolveProvider": False},
      "initialize advertises codeLensProvider as CodeLensOptions")
check(ls is not None, "codeLens -> array result")
check(rows(ls) == [(3, "runs in parallel"),
                   (10, "runs sequentially: writes loop-external t")],
      "standalone document: one lens per `for`, verdict verbatim")
# The lens sits ON the keyword, so the editor draws it above that line.
vl = verdict_lenses(ls)
first = (vl or [{}])[0]
check(first.get("range", {}).get("start", {}).get("character") == 4
      and first["range"]["end"]["character"] == 7,
      "range covers the `for` keyword itself")
check(len(vl) > 0 and all(l["command"]["command"] == "" for l in vl),
      "every verdict lens is plain text (empty command id, nothing to click)")

# ---- the noncode skip is load-bearing -----------------------------------
# Four decoy `for`s: a line comment, a `;* *;` block spanning two lines, and a
# string literal. Without symbols.skip_noncode# the scan finds five sites, the
# real loop's verdict is paired with the FIRST of them, and every lens is
# wrong while the count still looks plausible.
nc = ws + "/noncode.scaly"
open(nc, "w").write(
    "; A comment mentioning for i in n must never get a lens.\n"
    ";* for x in y\n"
    "   for z in w *;\n"
    "function only_one(n: int) returns int\n"
    "{\n"
    "    let text_for \"for a in b\"\n"
    "    var t 0\n"
    "    for i in n\n"                                  # 8: the only real one
    "        set t: i\n"
    "    t\n"
    "}\n"
    "\n"
    "if only_one(3) <> 2\n"
    "    exit 1\n"
    "print \"PASS\"\n")
caps, ls = lenses(nc)
check(rows(ls) == [(8, "runs sequentially: writes loop-external t")],
      "`for` in a comment / block comment / string literal gets no lens")

# ---- a package member: verdicts from DISK, positions from the BUFFER ----
pkg = ws + "/pkg/0.1.0"
os.makedirs(pkg + "/pkg")
open(pkg + "/pkg.scaly", "w").write("define pkg\n{\n    module a\n}\n")
body = ("define a\n"
        "{\n"
        "    function sum_a(n: int) returns int\n"
        "    {\n"
        "        var t 0\n"
        "        for i in n\n"                           # 6
        "            set t: i\n"
        "        t\n"
        "    }\n"
        "}\n")
member = pkg + "/pkg/a.scaly"
open(member, "w").write(body)
caps, ls = lenses(member)
check(rows(ls) == [(6, "runs sequentially: writes loop-external t")],
      "package member: planned through its root, so the sibling names resolve")
# Unsaved edit that shifts lines without changing the `for` structure: the
# verdict still comes from the saved text, the POSITION follows the buffer.
caps, ls = lenses(member, "; one\n; two\n" + body)
check(rows(ls) == [(8, "runs sequentially: writes loop-external t")],
      "unsaved edit above the loop: the lens follows the buffer, not the disk")
# Unsaved edit that ADDS a `for`: no pairing is honest, so the lenses vanish
# until the file is saved (a visibly absent lens beats a plausible wrong one).
caps, ls = lenses(member, body.replace("        var t 0\n",
                                       "        var t 0\n        for q in n\n            set t: t + q\n"))
check(verdict_lenses(ls) == [], "a buffer whose `for` structure differs from disk answers no verdict")

# ---- the reachable "no verdict": a file the root does not declare -------
undeclared = pkg + "/pkg/b.scaly"
open(undeclared, "w").write(body.replace("define a", "define b").replace("sum_a", "sum_b"))
caps, ls = lenses(undeclared)
check(rows(ls) == [(6, "no verdict: not planned in this root")],
      "a file that is not a declared module of its root has no verdict")

# ---- nothing to say -----------------------------------------------------
plain = ws + "/plain.scaly"
open(plain, "w").write("function add(a: int, b: int) returns int\n{\n    return a + b\n}\n")
caps, ls = lenses(plain)
check(verdict_lenses(ls) == [], "a file without a `for` loop answers no verdict")

# ---- cross-producer: the lens must agree with `--task-plan` -------------
# tensor.scaly is this tree's densest `for` file (the tape kernels) and lives
# in the scaly package, so the LSP has to plan its ROOT to answer at all.
cli = {}
for ln in open("/tmp/lsp_taskplan.txt"):
    ln = ln.rstrip("\n")
    if not ln.startswith("task-plan: ") or " for-loops," in ln: continue
    f, line, col, verdict = ln[len("task-plan: "):].split(":", 3)
    if f.endswith("scaly/tensor.scaly"): cli[int(line)] = verdict.strip()
check(len(cli) > 10, "the --task-plan control run produced tensor.scaly entries")
caps, ls = lenses("packages/scaly/0.1.0/scaly/tensor.scaly")
lens_map = {}
for (line, title) in rows(ls):
    lens_map[line] = title
check(sorted(lens_map) == sorted(cli),
      "codeLens covers exactly the loops --task-plan reports (%d)" % len(cli))
def expected(v):
    if v == "parallel": return "runs in parallel"
    return "runs sequentially: " + v[len("blocked: "):]
mismatch = [l for l in cli if lens_map.get(l) != expected(cli[l])]
check(not mismatch, "every verdict matches the compiler's own report")
if mismatch:
    for l in mismatch[:5]:
        print("      line %d: lens %r vs cli %r" % (l, lens_map.get(l), cli[l]))
# Both outcomes must be present, or the comparison above could be vacuous.
check(any(v == "parallel" for v in cli.values()) and any(v != "parallel" for v in cli.values()),
      "the corpus file carries both a parallel and a blocked verdict")
sys.exit(1 if failures else 0)
PY
rc=$?
if [ $rc -eq 0 ]; then ok "lsp codeLens"; else bad "lsp codeLens"; fi

# ---- codeLens: the three plan-free families ----
# scalyls/codelens.scaly owns the lenses that need no planner: the GENERATED
# banner, PACKAGE/MODULE resolution, and Run. Two of them make a claim about the
# FILE SYSTEM, so most of this group checks the claim against the file system
# rather than against a hardcoded expectation — an invariant that cannot rot as
# the tree moves:
#   * every generator the banner names must exist
#   * every "opens X" target must exist, every "missing module file: X" must not
# The rest pins the two rules that were WRONG in the first draft and are the
# reason this group exists at all:
#   * the module search directory is the file's OWN for a top-level `module`, and
#     <dir>/<NS> only inside `define NS { … }` (Modeler.build_module# vs
#     Modeler.handle_namespace#) — the first draft used the second rule for both
#   * a package lens with no absolute base must NOT be clickable (a relative path
#     handed to vscode.Uri.file resolves against nothing)
{ python3 - <<'PY'
import sys, json, subprocess, os, shutil
failures = 0
def check(cond, label):
    global failures
    print(("PASS  " if cond else "FAIL  ") + label)
    if not cond: failures += 1
def frame(o):
    b=json.dumps(o).encode(); return ("Content-Length: %d\r\n\r\n"%len(b)).encode()+b

# `no_home=True` runs the server WITHOUT SCALY_HOME, so the resolution-base
# fallback is exercised deterministically instead of depending on the shell that
# started the suite.
def lenses(path, no_home=False):
    uri = "file://" + os.path.abspath(path)
    inp  = frame({"jsonrpc":"2.0","id":1,"method":"initialize","params":{}})
    inp += frame({"jsonrpc":"2.0","method":"initialized","params":{}})
    inp += frame({"jsonrpc":"2.0","method":"textDocument/didOpen","params":{
                  "textDocument":{"uri":uri,"languageId":"scaly","version":1,"text":open(path).read()}}})
    inp += frame({"jsonrpc":"2.0","id":2,"method":"textDocument/codeLens",
                  "params":{"textDocument":{"uri":uri}}})
    inp += frame({"jsonrpc":"2.0","id":9,"method":"shutdown"})
    inp += frame({"jsonrpc":"2.0","method":"exit"})
    env = dict(os.environ)
    if no_home: env.pop("SCALY_HOME", None)
    out = subprocess.run(["/tmp/scalyls"], input=inp, stdout=subprocess.PIPE, env=env).stdout
    frames, d = [], out
    while d:
        i = d.find(b"\r\n\r\n")
        if i < 0: break
        n = int(d[:i].decode().split(":")[1].strip())
        frames.append(json.loads(d[i+4:i+4+n])); d = d[i+4+n:]
    r = next((f for f in frames if f.get("id") == 2), None)
    res = (r or {}).get("result")
    return res if isinstance(res, list) else []
def rows(ls):   # (1-based line, title, command id, single argument or None)
    out = []
    for l in ls:
        c = l["command"]
        a = c.get("arguments")
        out.append((l["range"]["start"]["line"] + 1, c["title"], c["command"],
                    a[0] if a else None))
    return out

# ---- GENERATED banner over the real tree --------------------------------
# The table in codelens.scaly mirrors ./mkp; these are three of mkp's four
# output classes, and the fourth (docs/*.xml) is not a .scaly file.
gen_cases = [
    ("packages/scalyc/0.1.0/scalyc/compiler/parser.scaly", "codegen/parser-scaly.scm"),
    ("packages/scalyc/0.1.0/scalyc/compiler/Syntax.scaly", "codegen/syntax-scaly.scm"),
    ("packages/scalyls/0.1.0/scalyls/grammar.scaly",       "codegen/highlight-scaly.scm"),
    ("tests/selfhosted/controlflow__break-in-for.scaly",   "tests/controlflow.sgm"),
    ("packages/opensp/0.1.0/opensp/ParserMessages.scaly",  "tools/msggen.py"),
]
for (f, want) in gen_cases:
    banner = [r for r in rows(lenses(f)) if r[1].startswith("generated from ")]
    hit = len(banner) == 1 and banner[0][1] == ("generated from %s - edit the generator, then ./mkp" % want)
    check(hit, "banner on %s names %s" % (os.path.basename(f), want))
    if hit:
        target = banner[0][3]
        check(banner[0][2] == "scaly.openPath" and target is not None
              and os.path.isabs(target) and os.path.exists(target),
              "  its generator is an absolute path that exists")
        check(banner[0][0] == 1, "  and it sits at the top of the file")
# The literate tests say in their own first line where they came from, so that
# claim and the table are two independent producers of the same fact.
first = open("tests/selfhosted/controlflow__break-in-for.scaly").readline()
check("tests/controlflow.sgm" in first,
      "the generated file's own header agrees with the table")
# A hand-written file gets no banner.
check(not [r for r in rows(lenses("packages/scalyls/0.1.0/scalyls/docstore.scaly"))
           if r[1].startswith("generated from ")],
      "a hand-written file gets no banner")

# ---- Run --------------------------------------------------------------
run = [r for r in rows(lenses("tests/aot/hello.scaly")) if r[1].startswith("Run ")]
check(len(run) == 1 and run[0][1] == "Run (scalyc --jit)" and run[0][2] == "scaly.runFile"
      and run[0][3] == os.path.abspath("tests/aot/hello.scaly"),
      "a program (top-level statements) gets a Run lens carrying its own path")
check(not [r for r in rows(lenses("packages/scaly/0.1.0/scaly.scaly")) if r[1].startswith("Run ")],
      "a library root (no top-level statements) gets no Run lens")

# ---- claims about the file system must be TRUE --------------------------
# Over real tree files: whatever the lens says about a module file, the file
# system must agree. This is the assertion that cannot rot.
sweep = ["packages/scalyls/0.1.0/scalyls.scaly",
         "packages/scaly/0.1.0/scaly.scaly",
         "packages/scalyc/0.1.0/scalyc/compiler.scaly",
         "packages/scalyc/0.1.0/scalyc/compiler/parser.scaly"]
opens = 0
missing = 0
wrong = []
for f in sweep:
    for (line, title, cmd, arg) in rows(lenses(f)):
        if title.startswith("opens "):
            opens += 1
            if arg is None or not os.path.exists(arg): wrong.append((f, line, title, arg))
        if title.startswith("missing module file: "):
            missing += 1
            # A "missing" lens carries no command, so the claim is checked by
            # rebuilding the path the way the lens did: relative to the file's
            # own directory.
            rel = title[len("missing module file: "):]
            if os.path.exists(os.path.join(os.path.dirname(os.path.abspath(f)), rel)):
                wrong.append((f, line, title, None))
        if title.startswith("package root: "):
            if arg is None or not os.path.exists(arg): wrong.append((f, line, title, arg))
check(opens > 20, "the sweep resolved a real number of module files (%d)" % opens)
check(not wrong, "every module/package claim matches the file system")
for w in wrong[:5]: print("      ", w)
# This tree resolves EVERY declared module, so the sweep is expected to contain no
# missing-module claim at all — and it is the lens that made that true: it reported
# a dead `module lexer` inside the generated parser.scaly (resolving to
# compiler/parser/lexer.scaly, which never existed, silently stubbed by the
# Modeler), and codegen/parser-scaly.scm stopped emitting it. So "claims are true"
# is proved on the OTHER outcome by the fixture group below, which declares a
# module with no file on purpose; asserting a wart here would pin the tree to it.
check(missing == 0, "no module of this tree fails to resolve (%d)" % missing)

# ---- the two rules the first draft got wrong ---------------------------
ws = "/tmp/lsp_ws/codelens_paths"
shutil.rmtree(ws, ignore_errors=True)
os.makedirs(ws + "/pkg/0.1.0/pkg")
open(ws + "/pkg/0.1.0/pkg.scaly", "w").write(
    "package scaly 0.1.0\n\ndefine pkg\n{\n    module a\n    module gone\n}\n")
# a.scaly declares a TOP-LEVEL module: the Modeler looks for it beside a.scaly,
# NOT under a/.
open(ws + "/pkg/0.1.0/pkg/a.scaly", "w").write("module helper\n\ndefine a\n{\n}\n")
open(ws + "/pkg/0.1.0/pkg/helper.scaly", "w").write("define helper\n{\n}\n")
root_rows = rows(lenses(ws + "/pkg/0.1.0/pkg.scaly", no_home=True))
titles = [t for (_, t, _, _) in root_rows]
check("opens pkg/a.scaly" in titles,
      "a module inside `define NS` resolves under NS/ (the namespace rule)")
check("missing module file: pkg/gone.scaly" in titles,
      "a declared module with no file says so")
member_rows = rows(lenses(ws + "/pkg/0.1.0/pkg/a.scaly", no_home=True))
mt = [t for (_, t, _, _) in member_rows]
check("opens helper.scaly" in mt,
      "a TOP-LEVEL module resolves beside its own file (the sibling rule)")
check("missing module file: a/helper.scaly" not in mt,
      "  and is not looked up under a directory named after the file")
# No packages/ anywhere above the fixture, so there is no absolute base: the
# package lens must state the path and offer NO command.
pkg_rows = [r for r in root_rows if r[1].startswith("package")]
check(len(pkg_rows) == 1 and pkg_rows[0][1] == "package: packages/scaly/0.1.0/scaly.scaly"
      and pkg_rows[0][2] == "" and pkg_rows[0][3] is None,
      "with no resolution base the package lens is plain text, not a link")
# In the checkout the same declaration IS clickable and absolute.
tree_pkg = [r for r in rows(lenses("packages/scalyls/0.1.0/scalyls.scaly")) if r[1].startswith("package root: ")]
check(len(tree_pkg) == 3 and all(r[2] == "scaly.openPath" and os.path.isabs(r[3]) for r in tree_pkg),
      "in a checkout the package lens resolves absolutely and is clickable")
sys.exit(1 if failures else 0)
PY
} > "$BGDIR/lsp_codelens_plan_free.out" 2>&1 &
bg_add $! "lsp codeLens plan-free" "$BGDIR/lsp_codelens_plan_free.out"

# ---- codeLens: definitions emitted per routine (monomorphisation) ----
# scalyls/instlens.scaly counts, per `function` / `procedure` declaration, how
# many definitions the compiler emits for it — one per distinct mangled name, so
# a generic routine shows what it COSTS in copies — and reports a routine, or a
# whole file, that the demand-driven planner never planned.
#
# The gate is a cross-check against the compiler's own output, never against a
# number written into this file, and it has TWO halves because one of them cannot
# see the defect that matters:
#   * a self-contained fixture package, compiled to IR by $SCALYC right here
#   * the TREE: `Vector[T].get` against the committed seed/scaly.ll
# The first version of this lens counted plan ENTRIES rather than distinct mangled
# names and said 44 for Vector[T].get where the IR holds 20 — and the fixture does
# NOT reproduce that (measured: its plan has exactly one entry per definition, and
# a deliberately broken build passed the fixture half unchanged). Whatever makes
# the planner keep several entries for one instantiation, this fixture does not
# trigger it, so the tree half is the one that holds the dedup load-bearing.
python3 - "$SCALYC" <<'PYINST'
import sys, json, subprocess, os, shutil, re
scalyc = sys.argv[1]
failures = 0
def check(cond, label):
    global failures
    print(("PASS  " if cond else "FAIL  ") + label)
    if not cond: failures += 1
def frame(o):
    b=json.dumps(o).encode(); return ("Content-Length: %d\r\n\r\n"%len(b)).encode()+b
def lenses(path):
    uri = "file://" + os.path.abspath(path)
    inp  = frame({"jsonrpc":"2.0","id":1,"method":"initialize","params":{}})
    inp += frame({"jsonrpc":"2.0","method":"initialized","params":{}})
    inp += frame({"jsonrpc":"2.0","method":"textDocument/didOpen","params":{
                  "textDocument":{"uri":uri,"languageId":"scaly","version":1,"text":open(path).read()}}})
    inp += frame({"jsonrpc":"2.0","id":2,"method":"textDocument/codeLens",
                  "params":{"textDocument":{"uri":uri}}})
    inp += frame({"jsonrpc":"2.0","id":9,"method":"shutdown"})
    inp += frame({"jsonrpc":"2.0","method":"exit"})
    out = subprocess.run(["/tmp/scalyls"], input=inp, stdout=subprocess.PIPE).stdout
    frames, d = [], out
    while d:
        i = d.find(b"\r\n\r\n")
        if i < 0: break
        n = int(d[:i].decode().split(":")[1].strip())
        frames.append(json.loads(d[i+4:i+4+n])); d = d[i+4+n:]
    r = next((f for f in frames if f.get("id") == 2), None)
    res = (r or {}).get("result")
    return res if isinstance(res, list) else []
def titles(ls, needle):
    return [(l["range"]["start"]["line"] + 1, l["command"]["title"])
            for l in ls if needle in l["command"]["title"]]

ws = "/tmp/lsp_ws/codelens_inst"
shutil.rmtree(ws, ignore_errors=True)
os.makedirs(ws + "/pkg/0.1.0/pkg")
open(ws + "/pkg/0.1.0/pkg.scaly", "w").write(
    "define pkg\n{\n    module boxes\n    module uses\n}\n")
# A generic with ONE method, plus a non-generic routine as the control.
open(ws + "/pkg/0.1.0/pkg/boxes.scaly", "w").write(
    "define boxes\n"
    "{\n"
    "    define Box[T]\n"
    "    (\n"
    "        value: T\n"
    "    )\n"
    "    {\n"
    "        function get(this: Box[T]) returns T\n"
    "        {\n"
    "            value\n"
    "        }\n"
    "    }\n"
    "\n"
    "    function plain_helper(n: int) returns int\n"
    "    {\n"
    "        n + 1\n"
    "    }\n"
    "}\n")
# Two instantiations, from a sibling module.
open(ws + "/pkg/0.1.0/pkg/uses.scaly", "w").write(
    "use pkg.boxes.Box\n"
    "\n"
    "define uses\n"
    "{\n"
    "    function use_int(n: int) returns int\n"
    "    {\n"
    "        let b Box[int](n)\n"
    "        b.get()\n"
    "    }\n"
    "\n"
    "    function use_char(c: char) returns char\n"
    "    {\n"
    "        let b Box[char](c)\n"
    "        b.get()\n"
    "    }\n"
    "}\n")

rows = titles(lenses(ws + "/pkg/0.1.0/pkg/boxes.scaly"), "definitions emitted")
check(len(rows) == 1 and rows[0][0] == 8,
      "the generic method gets a lens, the non-generic routine does not")
m = re.match(r"^(\d+) definitions emitted: (.*)$", rows[0][1] if rows else "")
check(m is not None, "the title reads '<N> definitions emitted: <types>'")
lens_count = int(m.group(1)) if m else -1
listed = m.group(2).split(", ") if m else []
check("Box[int]" in listed and "Box[char]" in listed,
      "both instantiated types are named (%s)" % ", ".join(listed))
check("Box[T]" in listed,
      "  and the template form too, since it is emitted as well")

# The cross-check: compile the same package and count the definitions.
ir = ws + "/pkg.ll"
rc = subprocess.run([scalyc, "-S", "--no-prelude", "-o", ir, ws + "/pkg/0.1.0/pkg.scaly"],
                    stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL).returncode
check(rc == 0 and os.path.exists(ir), "the fixture package compiles to IR")
if os.path.exists(ir):
    # `Box::get` in Itanium: _ZN3Box(I<args>E)?3getE...
    defs = set(re.findall(r"^define [^@]*@(_ZN3Box(?:I.*?E)?3getE[^(]*)\(", open(ir).read(), re.M))
    check(len(defs) == lens_count,
          "the lens count equals the emitted definition count (%d vs %d)" % (lens_count, len(defs)))
    check(len(defs) > 2, "  and the fixture really produced several (%d)" % len(defs))

# ---- the TREE half: a real generic against the committed IR ----------------
# Vector.scaly is a member of packages/scaly, so answering means planning that
# whole root; seed/scaly.ll is that package as the seed compiler emitted it, so
# the two sides come from different programs.
vec = "packages/scaly/0.1.0/scaly/containers/Vector.scaly"
src = open(vec).read()
# The `get` method's declaration line (1-based), found rather than hardcoded.
want_line = 0
for (n, line) in enumerate(src.split("\n"), start=1):
    if line.strip().startswith("function get(this: Vector[T]"):
        want_line = n
        break
check(want_line > 0, "found Vector[T].get in the tree")
rows = titles(lenses(vec), "definitions emitted")
hit = [t for (l, t) in rows if l == want_line]
check(len(hit) == 1, "Vector[T].get carries a definitions-emitted lens")
mv = re.match(r"^(\d+) definitions emitted: (.*)$", hit[0] if hit else "")
check(mv is not None, "  with the same title shape")
if mv:
    # Itanium: _ZN6Vector(I<args>E)?3getE... — the optional args group matters,
    # since the bare declaration form carries none and IS emitted.
    ir_defs = set(re.findall(r"^define [^@]*@(_ZN6Vector(?:I.*?E)?3getE[^(]*)\(",
                             open("seed/scaly.ll").read(), re.M))
    check(int(mv.group(1)) == len(ir_defs),
          "the tree lens count equals the seed IR definition count (%s vs %d)"
          % (mv.group(1), len(ir_defs)))
    check(len(ir_defs) > 10, "  and this generic really has many (%d)" % len(ir_defs))
    check("Vector[int]" in mv.group(2), "  the type list names a concrete instantiation")

# A file the root does not include as a module: ONE file-level lens, not a wall.
open(ws + "/pkg/0.1.0/pkg/orphan.scaly", "w").write(
    "define orphan\n{\n"
    "    function a(n: int) returns int\n    {\n        n\n    }\n"
    "    function b(n: int) returns int\n    {\n        n\n    }\n"
    "    function c(n: int) returns int\n    {\n        n\n    }\n"
    "}\n")
orphan = titles(lenses(ws + "/pkg/0.1.0/pkg/orphan.scaly"), "planned in this root")
check(len(orphan) == 1 and orphan[0][0] == 1
      and orphan[0][1] == "no routine of this file is planned in this root",
      "a file outside the root's module tree says so ONCE, at the top")
sys.exit(1 if failures else 0)
PYINST
rc=$?
if [ $rc -eq 0 ]; then ok "lsp codeLens instantiations"; else bad "lsp codeLens instantiations"; fi

# ---- codeLens: mutable / shared module globals ----
# codelens.storage_lens# states two things a reader cannot see on the line:
# which THREAD sees the cell (`mutable` is thread-local, `shared` is
# process-global) and whether the declaration produces a global AT ALL — one
# whose annotation is not a TYPE is an rc-4 at the declaration since
# 2026-09-23 (it used to be DROPPED, silently, with rc 0).
#
# The gate is an IR cross-check for the same reason the placement gate is one:
# the emitted IR is the second producer of both facts, so the expectation is
# DERIVED from it per declaration rather than written down here —
#   @n = thread_local global ...  -> "thread-local: ..."
#   @n = global ...               -> "process-global: ..."
#   no @n at all                  -> "no type: ..." (and the compiler names it)
# It cannot rot as the emitter's choices move; it breaks only when the lens and
# the compiler disagree.
python3 - "$SCALYC" <<'PYSTOR'
import sys, json, subprocess, os, shutil, re
scalyc = sys.argv[1]
failures = 0
def check(cond, label):
    global failures
    print(("PASS  " if cond else "FAIL  ") + label)
    if not cond: failures += 1
def frame(o):
    b=json.dumps(o).encode(); return ("Content-Length: %d\r\n\r\n"%len(b)).encode()+b
def lenses(path):
    uri = "file://" + os.path.abspath(path)
    inp  = frame({"jsonrpc":"2.0","id":1,"method":"initialize","params":{}})
    inp += frame({"jsonrpc":"2.0","method":"initialized","params":{}})
    inp += frame({"jsonrpc":"2.0","method":"textDocument/didOpen","params":{
                  "textDocument":{"uri":uri,"languageId":"scaly","version":1,"text":open(path).read()}}})
    inp += frame({"jsonrpc":"2.0","id":2,"method":"textDocument/codeLens",
                  "params":{"textDocument":{"uri":uri}}})
    inp += frame({"jsonrpc":"2.0","id":9,"method":"shutdown"})
    inp += frame({"jsonrpc":"2.0","method":"exit"})
    out = subprocess.run(["/tmp/scalyls"], input=inp, stdout=subprocess.PIPE).stdout
    frames, d = [], out
    while d:
        i = d.find(b"\r\n\r\n")
        if i < 0: break
        n = int(d[:i].decode().split(":")[1].strip())
        frames.append(json.loads(d[i+4:i+4+n])); d = d[i+4+n:]
    r = next((f for f in frames if f.get("id") == 2), None)
    res = (r or {}).get("result")
    return res if isinstance(res, list) else []
# Only this family's lenses, keyed by 1-based line.
KINDS = ("thread-local", "process-global", "no type:")
def storage(ls):
    out = {}
    for l in ls:
        c = l["command"]
        if c["title"].startswith(KINDS):
            out[l["range"]["start"]["line"] + 1] = (c["title"], c["command"], c.get("arguments"))
    return out

ws = "/tmp/lsp_ws/codelens_storage"
shutil.rmtree(ws, ignore_errors=True)
os.makedirs(ws)
# Every state the family can answer, and the two spellings for each.
# `pair` / `spair` are the case that makes the condition more than a colon
# test: the colon IS there and the global is still dropped, because a
# BindingSpec has a Structure arm and only the Type arm models to a type.
src = ("mutable counter: int 7\n"                    # 1  thread-local
       "shared table: pointer[void] null\n"          # 2  process-global
       "mutable flag: bool false\n"                  # 3  thread-local
       "shared ticket: int 0\n"                      # 4  process-global
       "mutable nameless 9\n"                        # 5  dropped (no colon)
       "shared unnamed 11\n"                         # 6  dropped (no colon)
       "mutable pair: (x: int) 0\n"                  # 7  dropped (colon, no type)
       "shared spair: (y: int) 0\n"                  # 8  dropped (colon, no type)
       "\n"
       "define ns\n"
       "{\n"
       "    mutable inner: int 3\n"                  # 12 thread-local, in a namespace
       "    shared outer: int 4\n"                   # 13 process-global, in a namespace
       "}\n"
       "\n"
       "set counter: counter + 1\n")
path = ws + "/storage.scaly"
open(path, "w").write(src)

decls = [(i + 1, m.group(1), m.group(2))
         for i, line in enumerate(src.split("\n"))
         for m in [re.match(r"\s*(mutable|shared)\s+([A-Za-z_][A-Za-z0-9_]*)", line)] if m]
check(len(decls) == 10, "the fixture declares 10 globals (%d)" % len(decls))

rows = storage(lenses(path))
check(sorted(rows) == [d[0] for d in decls],
      "one lens per declaration, on its own line (%s)" % sorted(rows))

# ---- the compiler rejects the four untyped declarations, at their lines --
full = subprocess.run([scalyc, "-S", "--no-tests", "-o", ws + "/full.ll", path],
                      stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
msgs = full.stdout.decode()
untyped = [(5, "nameless"), (6, "unnamed"), (7, "pair"), (8, "spair")]
check(full.returncode != 0 and all(
          ("storage.scaly:%d:1: error: module global %s has no type" % (ln, nm)) in msgs
          for (ln, nm) in untyped),
      "the compiler reports every untyped declaration at its own line")

# ---- the IR decides what each label SHOULD be -------------------------
# over the fixture without the four untyped lines (blank, so lines keep)
good = ws + "/good.scaly"
open(good, "w").write("\n".join("" if i + 1 in dict(untyped) else l
                                 for i, l in enumerate(src.split("\n"))))
ir = ws + "/storage.ll"
rc = subprocess.run([scalyc, "-S", "--no-tests", "-o", ir, good],
                    stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL).returncode
check(rc == 0 and os.path.exists(ir),
      "the fixture without them compiles with rc 0")
if os.path.exists(ir):
    text = open(ir).read()
    def ir_state(name):
        # LLVM auto-renames a non-extern collision (@cell.1), so accept a suffix.
        m = re.search(r"^@%s(\.\d+)? = (thread_local )?global " % re.escape(name),
                      text, re.M)
        if not m: return "dropped"
        return "thread-local" if m.group(2) else "process-global"
    want = {
        "thread-local":   "thread-local: one cell per thread",
        "process-global": "process-global: the declarer owns the race discipline",
    }
    states = {}
    wrong = []
    for (line, kw, name) in decls:
        st = ir_state(name)
        states[st] = states.get(st, 0) + 1
        expect = want.get(st, "no type: the compiler rejects this - %s NAME: TYPE INIT" % kw)
        got = rows.get(line, ("<none>", None, None))[0]
        if got != expect: wrong.append((line, name, st, expect, got))
    # A comparison is only worth something if the fixture reaches every state;
    # otherwise an all-one-answer lens would pass it.
    check(states.get("thread-local", 0) == 3 and states.get("process-global", 0) == 3
          and states.get("dropped", 0) == 4,
          "the IR puts the fixture in all three states (%s)" % states)
    check(not wrong, "every lens says what the IR does")
    for w in wrong[:6]: print("      ", w)
    # Named on its own, because it is the assertion that separates this
    # implementation from a colon test: both `pair` lines carry a colon and
    # neither reaches the IR.
    check(ir_state("pair") == "dropped" and ir_state("spair") == "dropped"
          and rows.get(7, ("", ))[0].startswith("no type:")
          and rows.get(8, ("", ))[0].startswith("no type:"),
          "a colon is not enough: `: (x: int)` is no type and the lens says so")
    # The namespace pair is a correction to CLAUDE.md, which calls a namespace
    # `mutable` rejected. It is not rejected, so the lens must cover it.
    check(ir_state("inner") == "thread-local" and ir_state("outer") == "process-global",
          "a global declared inside `define ns` is emitted, not rejected")

# Plain text, never a link: there is nothing to open.
check(all(v[1] == "" and v[2] is None for v in rows.values()),
      "the storage lenses carry no command")

# ---- a STATEMENT-level `mutable` is not a module global ----------------
# parse_file# takes the declaration list first and stops at the first
# statement, so a `mutable` after one is a local binding. It emits no global,
# and a lens over it would name a cell that does not exist.
stmt = ws + "/stmt.scaly"
open(stmt, "w").write("let z 1\nmutable after: int 7\n")
sir = ws + "/stmt.ll"
subprocess.run([scalyc, "-S", "--no-tests", "-o", sir, stmt],
               stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
check(os.path.exists(sir) and not re.search(r"^@after ", open(sir).read(), re.M),
      "a `mutable` after a top-level statement emits no global")
check(not storage(lenses(stmt)), "  and gets no lens")

# ---- over the real tree: one lens per declaration, right keyword --------
# The fixture proves the three states; this proves COVERAGE on files nobody
# wrote for the test. The source is the second producer here (the IR is not
# available per member without building its root): every `mutable` / `shared`
# line must carry exactly one lens, and only a `shared` may say
# "process-global". Members of the scaly package, whose root is the cheapest
# to plan.
sweep = ["packages/scaly/0.1.0/scaly/memory/root_pages.scaly",
         "packages/scaly/0.1.0/scaly/memory/Page.scaly",
         "packages/scaly/0.1.0/scaly/fiber.scaly"]
total = 0
off = []
for f in sweep:
    want = [(i + 1, m.group(1))
            for i, line in enumerate(open(f).read().split("\n"))
            for m in [re.match(r"\s*(mutable|shared)\s+[A-Za-z_]", line)] if m]
    got = {ln: t for ln, (t, _, _) in storage(lenses(f)).items()}
    total += len(want)
    if len(got) != len(want): off.append((f, "count", len(want), len(got)))
    for (ln, kw) in want:
        if (kw == "shared") != got.get(ln, "").startswith("process-global"):
            off.append((f, ln, kw, got.get(ln)))
check(total > 25, "the sweep saw a real number of declarations (%d)" % total)
check(not off, "every tree declaration gets one lens naming its own keyword")
for o in off[:5]: print("      ", o)
sys.exit(1 if failures else 0)
PYSTOR
rc=$?
if [ $rc -eq 0 ]; then ok "lsp codeLens storage"; else bad "lsp codeLens storage"; fi

# ---- codeLens: a store through a pointer parameter with no page to pin on ----
# scalyls/storelens.scaly marks the field-insensitive gap in the escape checker:
# a `set` that writes a String/StringC-carrying value through a pointer
# parameter, inside a routine that has NO page parameter and therefore nothing
# to pin the buffer onto.
#
# The gate is a RUNTIME cross-check, which is available here and stronger than
# any IR reading: the marked fixture and its one-keyword-different twin are
# COMPILED AND RUN, and the dangling one must actually print recycled bytes
# while the pinned one prints its string. So the lens is judged against the
# defect itself, not against a description of it.
{ python3 - "$SCALYC" <<'PYSTORE'
import sys, json, subprocess, os, shutil, re
scalyc = sys.argv[1]
failures = 0
def check(cond, label):
    global failures
    print(("PASS  " if cond else "FAIL  ") + label)
    if not cond: failures += 1
def frame(o):
    b=json.dumps(o).encode(); return ("Content-Length: %d\r\n\r\n"%len(b)).encode()+b
def lenses(path):
    uri = "file://" + os.path.abspath(path)
    inp  = frame({"jsonrpc":"2.0","id":1,"method":"initialize","params":{}})
    inp += frame({"jsonrpc":"2.0","method":"initialized","params":{}})
    inp += frame({"jsonrpc":"2.0","method":"textDocument/didOpen","params":{
                  "textDocument":{"uri":uri,"languageId":"scaly","version":1,"text":open(path).read()}}})
    inp += frame({"jsonrpc":"2.0","id":2,"method":"textDocument/codeLens",
                  "params":{"textDocument":{"uri":uri}}})
    inp += frame({"jsonrpc":"2.0","id":9,"method":"shutdown"})
    inp += frame({"jsonrpc":"2.0","method":"exit"})
    out = subprocess.run(["/tmp/scalyls"], input=inp, stdout=subprocess.PIPE).stdout
    frames, d = [], out
    while d:
        i = d.find(b"\r\n\r\n")
        if i < 0: break
        n = int(d[:i].decode().split(":")[1].strip())
        frames.append(json.loads(d[i+4:i+4+n])); d = d[i+4+n:]
    r = next((f for f in frames if f.get("id") == 2), None)
    res = (r or {}).get("result")
    return res if isinstance(res, list) else []
def stores(ls):
    return {l["range"]["start"]["line"] + 1: (l["command"]["title"], l["command"]["command"])
            for l in ls if l["command"]["title"].startswith("writes ")}

ws = "/tmp/lsp_ws/storelens"
shutil.rmtree(ws, ignore_errors=True)
os.makedirs(ws)

# The fixture prints what the caller reads back. `churn` reuses the released
# page, so a dangling buffer shows as the recycled bytes rather than by luck.
BODY = ("define Holder (name: String)\n"
        "\n"
        "procedure fill(mutable out: pointer[Holder], n: int) returns bool\n"
        "{\n"
        "    let s String%s(\"hello\")\n"
        "    set *out: Holder(s)\n"                      # 6 - the store
        "    true\n"
        "}\n"
        "\n"
        "function churn(k: int) returns int\n"
        "{\n"
        "    var acc 0\n"
        "    var i 0\n"
        "    while i < k\n"
        "    {\n"
        "        let junk String(\"XXXXXXXXXXXXXXXX\")\n"
        "        set acc: acc + (junk.get_length() as int)\n"
        "        set i: i + 1\n"
        "    }\n"
        "    acc\n"
        "}\n"
        "\n"
        "var h Holder(String(\"\"))\n"
        "if fill(&h, 1)\n"
        "{\n"
        "    let noise churn(64)\n"
        "    if noise > 0\n"
        "        scaly.os.Console.print(h.name.to_c_string())\n"
        "}\n")
bad_src  = BODY % ""     # built on its own frame: no page to pin on
good_src = BODY % "#"    # built on the caller page -- `#` gives the routine
                         # that page (it was a declared `rp` until 2026-09-25)
open(ws + "/bad.scaly", "w").write(bad_src)
open(ws + "/good.scaly", "w").write(good_src)

# ---- the defect itself, at runtime ------------------------------------
def run(name):
    exe = ws + "/" + name
    rc = subprocess.run([scalyc, "-o", exe, exe + ".scaly"],
                        stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL).returncode
    if rc != 0 or not os.path.exists(exe): return None
    p = subprocess.run([exe], stdout=subprocess.PIPE)
    return p.stdout.decode(errors="replace").strip()
out_bad, out_good = run("bad"), run("good")
check(out_good == "hello",
      "with a page parameter the caller reads what was written (%r)" % out_good)
check(out_bad is not None and out_bad != "hello",
      "without one it reads recycled bytes - SILENTLY, rc 0 (%r)" % out_bad)

# ---- and the lens marks exactly the one that breaks -------------------
bad_rows, good_rows = stores(lenses(ws + "/bad.scaly")), stores(lenses(ws + "/good.scaly"))
check(sorted(bad_rows) == [6],
      "the lens marks the store that dangles, and only it (%s)" % sorted(bad_rows))
check(bad_rows.get(6, ("",))[0] ==
      "writes Holder (carries String) through 'out' - no page parameter here, so its buffer cannot be pinned",
      "  naming the parameter and the String it carries transitively")
check(bad_rows.get(6, ("", ""))[1] == "", "  plain text, no command")
check(not good_rows,
      "a routine WITH a page parameter is not marked (%s)" % sorted(good_rows))

# ---- the three narrowings, each proved to cut something ---------------
# Without them this family put 133 lenses on one real file, several of them on
# lines that hold no store at all. Each fixture below is a shape that must stay
# UNMARKED, and each is unmarked for a different reason.
quiet = ("define Holder (name: String)\n"
         "use scaly.memory.Page\n"
         "\n"
         "procedure scalar_out(mutable ok: pointer[bool], n: int) returns int\n"
         "{\n"
         "    set *ok: true\n"                                    # 6  no buffer in a bool
         "    n\n"
         "}\n"
         "\n"
         "procedure empty_ctor(mutable out: pointer[Holder], n: int) returns int\n"
         "{\n"
         "    set *out: Holder(String())\n"                       # 12 nothing allocated
         "    n\n"
         "}\n"
         "\n"
         "procedure pinned(mutable out: pointer[String], host: pointer[Page], n: int) returns int\n"
         "{\n"
         "    set *out: String^host(\"x\")\n"                     # 18 decided on the line
         "    n\n"
         "}\n"
         "\n"
         "function local_only(n: int) returns int\n"
         "{\n"
         "    var acc 0\n"
         "    set acc: acc + n\n"                                 # 25 no pointer parameter
         "    acc\n"
         "}\n"
         "\n"
         "var q Holder(String(\"\"))\n"
         "var flag false\n"
         "if scalar_out(&flag, 1) > 0\n"
         "    scaly.os.Console.print \"q\"\n")
open(ws + "/quiet.scaly", "w").write(quiet)
qrc = subprocess.run([scalyc, "-S", "--no-tests", "-o", ws + "/quiet.ll", ws + "/quiet.scaly"],
                     stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL).returncode
check(qrc == 0, "the quiet fixture compiles")
qrows = stores(lenses(ws + "/quiet.scaly"))
check(not qrows, "none of the four quiet shapes is marked (%s)" % sorted(qrows))

# ---- over the real tree: every mark is ATTRIBUTED to its own routine ----
# The assertion that cannot rot, and it took two attempts. The first version
# only asked whether the marked line holds a `set` and whether the enclosing
# routine lacks a page parameter — and stayed GREEN with the declared_here
# guard removed, i.e. with 41 offsets from OTHER FILES of the package drawn into
# this one. It could not fail: the anchoring maps every offset to the nearest
# PRECEDING `set` site, so a marked line is a `set` line by construction, and in
# a file this full of them a foreign offset lands in a page-less routine too.
#
# What separates a correct mark from a foreign one is ATTRIBUTION, so that is
# what is checked: the parameter the title names must be a parameter OF THE
# ENCLOSING ROUTINE, and it must appear in the store's own target. Both fail
# immediately for an offset that belongs to another file.
# ★ The sweep spans FOUR files, and the two extra ones are not decoration.
# With only Style.scaly and Parser.scaly the total sat at 11 against a `> 10`
# assertion, i.e. a margin of ONE — and the caretctor conversion (2026-08-29)
# took `set *p: T(...)` out of Parser.scaly 69 times, tipping it to exactly 10.
# A canary whose margin is one is a canary that reports the next refactor as a
# defect, so the answer is more corpus, never a lower threshold.
sweep = ["packages/dazzle/0.1.0/dazzle/Style.scaly",
         "packages/dazzle/0.1.0/dazzle/Primitive.scaly",
         "packages/opensp/0.1.0/opensp/Parser.scaly",
         "packages/opensp/0.1.0/opensp/ParserState.scaly"]
def routine_head(src, ln):
    j = ln - 1
    while j >= 0 and not re.match(r"\s*(function|procedure)\s", src[j]): j -= 1
    if j < 0: return None
    head = ""
    for k in range(j, min(j + 8, len(src))):
        head += src[k]
        if ")" in src[k]: break
    return head
total = 0
wrong = []
for f in sweep:
    src = open(f).read().split("\n")
    rows = stores(lenses(f))
    total += len(rows)
    for (ln, (title, _)) in rows.items():
        line = src[ln - 1]
        m = re.search(r"through '([A-Za-z_0-9]+)'", title)
        if not m:
            wrong.append((f, ln, "title names no parameter", title[:40])); continue
        param = m.group(1)
        if not re.match(r"\s*set\s", line):
            wrong.append((f, ln, "not a set", line.strip()[:50])); continue
        target = line.split(":")[0]
        if not re.search(r"\b%s\b" % re.escape(param), target):
            wrong.append((f, ln, "param %s not in the target" % param, target.strip()[:50])); continue
        head = routine_head(src, ln)
        if head is None:
            wrong.append((f, ln, "no enclosing routine", "")); continue
        params = head[head.find("(") + 1:]
        if param != "this" and not re.search(r"\b%s\s*:" % re.escape(param), params):
            wrong.append((f, ln, "param %s is not a parameter here" % param, head.strip()[:60])); continue
        if re.match(r"\s*(function|procedure)\s+[A-Za-z_0-9]+\(\s*rp\b", head):
            wrong.append((f, ln, "routine HAS a page parameter", head.strip()[:50]))
# a real number, not a zero. ★The threshold was lowered to `> 5` on 2026-09-25
# when the total read 10 (attributed then to R8 giving marked routines their
# page -- not re-established). The same day the server's silent truncation was
# found and closed (Style.scaly answered 0 of its 15 marks on a short stack;
# worker.serve_on_worker_stack#), and the sweep reads 25 since (Style 15, Parser
# 8, ParserState 2, Primitive 0). So the rule above holds again: more corpus,
# never a lower threshold.
check(total > 10, "the sweep marked a real number of stores (%d)" % total)
check(not wrong, "every mark is attributed to a parameter of its OWN routine")
for w in wrong[:5]: print("      ", w)
sys.exit(1 if failures else 0)
PYSTORE
} > "$BGDIR/lsp_codelens_unpinnable_store.out" 2>&1 &
bg_add $! "lsp codeLens unpinnable store" "$BGDIR/lsp_codelens_unpinnable_store.out"

# ---- the answer does not depend on the stack the CLIENT gave the server ----
# The worker plans the whole package a document belongs to, and the planner's
# nesting guard is sized from the stack it runs on. Before 2026-09-25 the
# request loop ran on the worker's main thread: at `ulimit -s 2048` a codeLens
# request on dazzle/Style.scaly tripped the guard in TeXFOTBuilder.scaly, the
# worker exited 17, and the client got a SUCCESSFUL, EMPTY answer (0 lenses for
# 15) with the reason on a stderr no editor shows. worker.serve_on_worker_stack#
# runs the loop on a thread of its own size now. The check asks the same
# question at 1 MB and at the suite's own stack and wants the same answer.
{ python3 - <<'PYSTACK'
import json, os, subprocess, sys
def frame(o):
    b=json.dumps(o).encode(); return ("Content-Length: %d\r\n\r\n"%len(b)).encode()+b
def lens_count(path, stack_kb):
    uri = "file://" + os.path.abspath(path)
    inp  = frame({"jsonrpc":"2.0","id":1,"method":"initialize","params":{}})
    inp += frame({"jsonrpc":"2.0","method":"initialized","params":{}})
    inp += frame({"jsonrpc":"2.0","method":"textDocument/didOpen","params":{
                  "textDocument":{"uri":uri,"languageId":"scaly","version":1,"text":open(path).read()}}})
    inp += frame({"jsonrpc":"2.0","id":2,"method":"textDocument/codeLens",
                  "params":{"textDocument":{"uri":uri}}})
    inp += frame({"jsonrpc":"2.0","id":9,"method":"shutdown"})
    inp += frame({"jsonrpc":"2.0","method":"exit"})
    cmd = ["/tmp/scalyls"] if stack_kb is None else ["sh", "-c", "ulimit -s %d && exec /tmp/scalyls" % stack_kb]
    out = subprocess.run(cmd, input=inp, stdout=subprocess.PIPE, stderr=subprocess.DEVNULL).stdout
    frames, d = [], out
    while d:
        i = d.find(b"\r\n\r\n")
        if i < 0: break
        n = int(d[:i].decode().split(":")[1].strip())
        frames.append(json.loads(d[i+4:i+4+n])); d = d[i+4+n:]
    r = next((f for f in frames if f.get("id") == 2), None)
    res = (r or {}).get("result")
    return len(res) if isinstance(res, list) else -1
f = "packages/dazzle/0.1.0/dazzle/Style.scaly"
big = lens_count(f, None)
small = lens_count(f, 1024)
ok = big > 0 and small == big
print(("PASS  " if ok else "FAIL  ") + "codeLens on %s: %d lenses at 1 MB, %d at the suite's stack" % (f, small, big))
sys.exit(0 if ok else 1)
PYSTACK
} > "$BGDIR/lsp_answer_independent_of_the_client_stack.out" 2>&1 &
bg_add $! "lsp answer independent of the client stack" "$BGDIR/lsp_answer_independent_of_the_client_stack.out"

# ---- inlayHint: where each construction is ALLOCATED ----
# scalyls/placehints.scaly labels every construction with the placement the
# EMITTER will give it: `stack`, `region` (this function's own frame),
# `region #` (the caller's page) or `region ^name`. Lifetimes are inferred, so
# this is the only place the decision is visible at all.
#
# The gate is an IR cross-check, because a placement hint that lies is worse than
# no hint: the fixture is compiled by $SCALYC and each label is checked against
# what the emitted IR actually does —
#   stack     -> an alloca of the struct, and NO Page::allocate
#   region    -> Page::allocate on the function's OWN frame (%frame = alloca)
#   region #  -> Page::allocate on the CALLER's frame (force_frame(ptr %0))
# That check is what caught the first version: `is_region_alloc` alone said
# `region #` for a construction the emitter puts on the STACK, because the
# function has no page parameter and the region path is abandoned when `_rp`
# cannot be resolved.
python3 - "$SCALYC" <<'PYPLACE'
import sys, json, subprocess, os, shutil, re
scalyc = sys.argv[1]
failures = 0
def check(cond, label):
    global failures
    print(("PASS  " if cond else "FAIL  ") + label)
    if not cond: failures += 1
# A real client sends raw UTF-8; json.dumps would escape it as \uXXXX, which is
# NOT byte-identical to the file on disk and would make every placement hint
# vanish (they are answered only for a buffer that matches the saved text).
def frame(o):
    b=json.dumps(o, ensure_ascii=False).encode()
    return ("Content-Length: %d\r\n\r\n"%len(b)).encode()+b
def hints(path, buffer=None, repeat=1):
    if buffer is None: buffer = open(path).read()
    uri = "file://" + os.path.abspath(path)
    nl = buffer.count("\n") + 1
    inp  = frame({"jsonrpc":"2.0","id":1,"method":"initialize","params":{}})
    inp += frame({"jsonrpc":"2.0","method":"initialized","params":{}})
    inp += frame({"jsonrpc":"2.0","method":"textDocument/didOpen","params":{
                  "textDocument":{"uri":uri,"languageId":"scaly","version":1,"text":buffer}}})
    for i in range(repeat):
        inp += frame({"jsonrpc":"2.0","id":2+i,"method":"textDocument/inlayHint","params":{
                      "textDocument":{"uri":uri},
                      "range":{"start":{"line":0,"character":0},"end":{"line":nl,"character":0}}}})
    inp += frame({"jsonrpc":"2.0","id":9,"method":"shutdown"})
    inp += frame({"jsonrpc":"2.0","method":"exit"})
    out = subprocess.run(["/tmp/scalyls"], input=inp, stdout=subprocess.PIPE).stdout
    frames, d = [], out
    while d:
        i = d.find(b"\r\n\r\n")
        if i < 0: break
        n = int(d[:i].decode().split(":")[1].strip())
        frames.append(json.loads(d[i+4:i+4+n])); d = d[i+4+n:]
    answers = []
    for i in range(repeat):
        r = next((f for f in frames if f.get("id") == 2+i), None)
        res = (r or {}).get("result") or []
        answers.append(res)
    return answers
# The same placement, answered in the HOVER (the server no longer advertises
# inlay hints): one hover per (line, character), 0-based, on the type name.
def hovers(path, positions):
    buffer = open(path).read()
    uri = "file://" + os.path.abspath(path)
    inp  = frame({"jsonrpc":"2.0","id":1,"method":"initialize","params":{}})
    inp += frame({"jsonrpc":"2.0","method":"initialized","params":{}})
    inp += frame({"jsonrpc":"2.0","method":"textDocument/didOpen","params":{
                  "textDocument":{"uri":uri,"languageId":"scaly","version":1,"text":buffer}}})
    for i, (l, c) in enumerate(positions):
        inp += frame({"jsonrpc":"2.0","id":20+i,"method":"textDocument/hover","params":{
                      "textDocument":{"uri":uri},"position":{"line":l,"character":c}}})
    inp += frame({"jsonrpc":"2.0","id":9,"method":"shutdown"})
    inp += frame({"jsonrpc":"2.0","method":"exit"})
    out = subprocess.run(["/tmp/scalyls"], input=inp, stdout=subprocess.PIPE).stdout
    frames, d = [], out
    while d:
        i = d.find(b"\r\n\r\n")
        if i < 0: break
        n = int(d[:i].decode().split(":")[1].strip())
        frames.append(json.loads(d[i+4:i+4+n])); d = d[i+4+n:]
    texts = []
    for i in range(len(positions)):
        r = next((f for f in frames if f.get("id") == 20+i), None)
        res = (r or {}).get("result") or {}
        texts.append((res.get("contents") or {}).get("value", ""))
    return texts
def hover_placement(text):
    m = re.search(r"Allocation: `([^`]*)`", text)
    return m.group(1) if m else None
def placement(res):
    out = []
    for h in res:
        t = h.get("label", "")
        if t == "stack" or t.startswith("region"):
            out.append((h["position"]["line"] + 1, t))
    return sorted(out)

ws = "/tmp/lsp_ws/placehints"
shutil.rmtree(ws, ignore_errors=True)
os.makedirs(ws)
src = ("define Point\n"
       "(\n"
       "    x: int\n"
       "    y: int\n"
       ")\n"
       "\n"
       "function on_stack(n: int) returns int\n"
       "{\n"
       "    let p Point(n, n)\n"                     # 9  — no page parameter
       "    p.x\n"
       "}\n"
       "\n"
       "function on_caller_sigil(n: int) returns int\n"
       "{\n"
       "    let p Point#(n, n)\n"                    # 15 — the `#` sigil
       "    p.x\n"
       "}\n"
       "\n"
       "function on_own_region(n: int) returns String\n"
       "{\n"
       "    var sb StringBuilder()\n"                # 21 — its own frame
       "    sb.append \"x\"\n"
       "    sb.to_string()\n"
       "}\n"
       "\n"
       "if on_stack(3) <> 3\n"
       "    exit 1\n"
       "print \"PASS\"\n")
path = ws + "/place.scaly"
open(path, "w").write(src)

rows = placement(hints(path)[0])
check(len(rows) == 3, "one hint per construction (%s)" % rows)

# ---- the IR decides what each label SHOULD be -------------------------
# Not a hardcoded expectation: the emitted IR is read per function and
# classified, then the lens is compared against that. So the assertion cannot rot
# as the emitter's choices move — it only breaks when the two disagree.
ir = ws + "/place.ll"
rc = subprocess.run([scalyc, "-S", "-o", ir, path],
                    stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL).returncode
check(rc == 0 and os.path.exists(ir), "the fixture compiles to IR")
if os.path.exists(ir):
    text = open(ir).read()
    def body(name):
        m = re.search(r"^define [^\n]*@[^\n]*" + name + r"[^\n]*\{\n(.*?)^\}", text, re.M | re.S)
        return m.group(1) if m else ""
    def classify(b):
        # No Page::allocate at all -> the construction is a plain stack alloca.
        #
        # Otherwise: WHICH frame did the page come from? Presence alone does not
        # answer that — a function with a caller frame AND its own frame contains
        # `scaly_force_frameP5Frame(ptr %0)` for the frame chain and
        # `…(ptr %frame)` for the allocation, so a substring test called the
        # own-region case `region #`. Follow the dataflow instead: read the page
        # argument of Page::allocate, then find which force_frame defined it.
        # (The mangled callee is `_Z17scaly_force_frameP5Frame`; a pattern of
        # "force_frame(ptr %0)" matches nothing.)
        if "_ZN4Page8allocateEmm" not in b: return "stack"
        m = re.search(r"@_ZN4Page8allocateEmm\(ptr (%[A-Za-z0-9_.]+)", b)
        if not m: return "?"
        page = m.group(1)
        d = re.search(re.escape(page) + r" = call ptr @_Z17scaly_force_frameP5Frame\(ptr (%[A-Za-z0-9_.]+)\)", b)
        if d:
            src = d.group(1)
        else:
            # Since 2026-09-27 the fast path is inline (Emitter.force_frame#):
            # the page is a phi whose first incoming value is the frame's first
            # word, `load ptr, ptr <frame>`.
            ph = re.search(re.escape(page) + r" = phi ptr \[ (%[A-Za-z0-9_.]+),", b)
            if not ph: return "?"
            ld = re.search(re.escape(ph.group(1)) + r" = load ptr, ptr (%[A-Za-z0-9_.]+)", b)
            if not ld: return "?"
            src = ld.group(1)
        if src == "%0": return "region #"
        if src.startswith("%frame"): return "region"
        return "?"
    sites = [(9, "on_stack"), (15, "on_caller_sigil"), (21, "on_own_region")]
    got = dict(rows)
    for (line, fname) in sites:
        b = body(fname)
        check(b != "", "found %s in the IR" % fname)
        want = classify(b)
        check(want in ("stack", "region", "region #"),
              "  the IR classifies %s as %s" % (fname, want))
        check(got.get(line) == want,
              "  the hint on line %d says %r and the IR says %r" % (line, got.get(line), want))
    # The hover on the construction's type name says the same, and the hover
    # on a name that is no construction head says nothing about allocation.
    heads = [(8, 10), (14, 10), (20, 11), (8, 8)]   # Point, Point#, StringBuilder, `p`
    texts = hovers(path, heads)
    for (line, fname), t in zip(sites, texts):
        check(hover_placement(t) == got.get(line),
              "  the hover on line %d says %r like the hint" % (line, hover_placement(t)))
    check(hover_placement(texts[3]) is None, "  a binding name's hover carries no allocation")

# ---- the saved-text rule ----------------------------------------------
# The placement comes from a plan the modeler builds from DISK, and a
# construction has no lexical anchor to pair saved offsets with buffer
# positions — so an unsaved buffer gets NO placement hints, while the lexical
# hints (parameter names, binding types) keep working.
edited = hints(path, buffer="; an unsaved line\n" + src)[0]
check(placement(edited) == [], "an unsaved buffer answers no placement hint")
check(len(edited) > 0, "  while the lexical hints are unaffected")

# ---- the cache -------------------------------------------------------
# Two requests for the same saved file must agree; the second is served from the
# blob cache (measured on the shipped opt -O2 build: 7.6 s cold, 0.3 s warm, and
# the cold half of that is the pre-existing lexical pass, not this one).
two = hints(path, repeat=2)
check(placement(two[0]) == placement(two[1]) and placement(two[0]) != [],
      "a repeated request answers identically (the cache is transparent)")

# ---- the tree: one hint per site, not one per instantiation ------------
# A generic body is planned once per instantiation, so containers/Vector.scaly's
# two constructions are reached ~20 times each. Without the dedup pass the
# answer held 89 hints for those two sites.
vec = placement(hints("packages/scaly/0.1.0/scaly/containers/Vector.scaly")[0])
check(len(vec) == len(set(vec)), "no duplicate hints for one site (%d hints)" % len(vec))
check(len(vec) > 0 and len(vec) < 6,
      "  and the count is the file's own construction count, not its instantiation count (%s)" % vec)
sys.exit(1 if failures else 0)
PYPLACE
rc=$?
if [ $rc -eq 0 ]; then ok "lsp inlayHint placement"; else bad "lsp inlayHint placement"; fi

# ---- semanticTokens/full: lexical token classification ----
# A single byte scan classifies every token into the legend (keyword/type/
# function/variable/operator/string/number/comment) and returns the LSP
# delta-encoded `data` array. Keywords come from scaly.sgm (grammar-derived
# grammar.is_keyword, generated by ./mkp). Exercises: keyword, the identifier
# after `function`/`define` (-> function / type), uppercase-initial -> type,
# operators, numbers, strings, a single-line comment, and a `;* ... *;`
# multi-line comment (split into one token per line).
python3 - <<'PY'
import sys, json, subprocess, os, tempfile
src = ("function add(a: int) returns int\n"      # 0
       "{\n"                                      # 1
       "    return a + 42   ; sum\n"              # 2
       "}\n"                                      # 3
       ";* block\n"                               # 4
       "   comment *;\n"                          # 5
       "define Point\n"                           # 6
       "(\n"                                      # 7
       "    x: int\n"                             # 8
       ")\n"                                      # 9
       "{\n"                                      # 10
       "    function get_x(pt: Point) returns int\n"  # 11 <- method param `pt`
       "    {\n"                                  # 12
       "        return pt.x\n"                    # 13 <- `pt` param, `x` after-dot
       "    }\n"                                  # 14
       "}\n"                                      # 15
       "function main(p: Point) returns int\n"    # 16 <- param `p`
       "{\n"                                      # 17
       "    let s \"hi\"\n"                       # 18
       "    return add(p.x)\n"                    # 19 <- `add` USE, `p` param, `x` after-dot
       "}\n")                                     # 20
# Isolated dir so the cross-file name harvest (dir-tree scan) sees only this
# file (skipped as the current file) -> no cross-file names leak in here.
d = tempfile.mkdtemp(prefix="lsp_sem_")
path = os.path.join(d, "m.scaly")
open(path, "w").write(src)
def frame(o):
    b=json.dumps(o).encode(); return ("Content-Length: %d\r\n\r\n"%len(b)).encode()+b
inp  = frame({"jsonrpc":"2.0","id":1,"method":"initialize","params":{}})
inp += frame({"jsonrpc":"2.0","method":"initialized","params":{}})
inp += frame({"jsonrpc":"2.0","id":2,"method":"textDocument/semanticTokens/full",
              "params":{"textDocument":{"uri":"file://"+path}}})
inp += frame({"jsonrpc":"2.0","id":9,"method":"shutdown"})
inp += frame({"jsonrpc":"2.0","method":"exit"})
out = subprocess.run(["/tmp/scalyls"], input=inp, stdout=subprocess.PIPE).stdout
frames, d = [], out
while d:
    i = d.find(b"\r\n\r\n")
    if i < 0: break
    n = int(d[:i].decode().split(":")[1].strip())
    frames.append(json.loads(d[i+4:i+4+n])); d = d[i+4+n:]
failures = 0
def check(cond, label):
    global failures
    print(("PASS  " if cond else "FAIL  ") + label)
    if not cond: failures += 1
caps = frames[0]["result"]["capabilities"]
stp = caps.get("semanticTokensProvider")
TYPES = ["keyword","type","function","variable","operator","string","number","comment","parameter","property"]
check(isinstance(stp, dict) and stp.get("legend",{}).get("tokenTypes") == TYPES,
      "advertises semanticTokensProvider with the token-type legend")
check(stp.get("full") is True, "semanticTokens full:true")
r = next((f for f in frames if f.get("id") == 2), None)
data = (r or {}).get("result",{}).get("data")
check(isinstance(data, list) and len(data) % 5 == 0, "data is a flat array of 5-int tuples")
data = data or []
# delta-decode -> (line, col, text, type-name)
lines = src.split("\n")
line = col = 0
toks = []
for k in range(0, len(data), 5):
    dl, dc, ln, ty, mod = data[k:k+5]
    if dl > 0: line += dl; col = dc
    else:      col += dc
    toks.append((line, col, lines[line][col:col+ln], TYPES[ty]))
def has(text, ty): return any(t[2] == text and t[3] == ty for t in toks)
check(has("function","keyword") and has("define","keyword") and has("returns","keyword")
      and has("let","keyword"), "keywords classified (grammar-derived)")
check(has("add","function"), "identifier after `function` -> function")
check(has("Point","type"), "identifier after `define` -> type")
check(has("int","type"), "primitive `int` -> type (semantic, lowercase)")
# `add` used at a call site (line 19) resolves to function, not variable -
# semantic resolution a regex/shape pass cannot do.
add_uses = [t for t in toks if t[0] == 19 and t[2] == "add"]
check(len(add_uses) == 1 and add_uses[0][3] == "function",
      "function used at a call site -> function (declared-kind resolution)")
check(has("+","operator"), "operator token")
check(has("42","number"), "number token")
check(has('"hi"',"string"), "string literal token")
check(has("; sum","comment"), "single-line comment token")
# multi-line comment: ;* block / comment *; -> one comment token per line
seg4 = [t for t in toks if t[0] == 4 and t[3] == "comment"]
seg5 = [t for t in toks if t[0] == 5 and t[3] == "comment"]
check(len(seg4) == 1 and len(seg5) == 1, "multi-line comment split into one token per line")
# parameter (scope-aware): `a` in add's signature AND its use in add's body.
a_decl = [t for t in toks if t[0] == 0 and t[2] == "a"]
a_use  = [t for t in toks if t[0] == 2 and t[2] == "a"]
check(len(a_decl) == 1 and a_decl[0][3] == "parameter", "param `a` at its declaration -> parameter")
check(len(a_use) == 1 and a_use[0][3] == "parameter", "param `a` used in its routine body -> parameter")
# scope-aware: parameter tokens appear ONLY where a param is in scope -
# `a` (add: 0,2), `pt` (get_x: 11,13), `p` (main: 16,19). No stray leakage.
params_outside = [t for t in toks if t[3] == "parameter" and t[0] not in (0, 2, 11, 13, 16, 19)]
check(params_outside == [], "parameter colouring is scoped to its own routine")
# property (a): field name `x` at its declaration in `define Point (x: int)`.
x_decl = [t for t in toks if t[0] == 8 and t[2] == "x"]
check(len(x_decl) == 1 and x_decl[0][3] == "property", "field `x` at its declaration -> property")
# parameter inside a method body: `pt` in get_x's signature AND `pt.x` body use.
pt_decl = [t for t in toks if t[0] == 11 and t[2] == "pt"]
pt_use  = [t for t in toks if t[0] == 13 and t[2] == "pt"]
check(len(pt_decl) == 1 and pt_decl[0][3] == "parameter", "method param `pt` at its declaration -> parameter")
check(len(pt_use) == 1 and pt_use[0][3] == "parameter", "method param `pt` used in its body -> parameter")
# parameter in main: `p` both in its signature and its body.
p_decl = [t for t in toks if t[0] == 16 and t[2] == "p"]
p_use  = [t for t in toks if t[0] == 19 and t[2] == "p"]
check(len(p_decl) == 1 and p_decl[0][3] == "parameter", "param `p` at its declaration -> parameter")
check(len(p_use) == 1 and p_use[0][3] == "parameter", "param `p` used in its routine body -> parameter")
# property (b): `x` after the `.` in `pt.x` and `p.x` -> property (member access).
x_acc13 = [t for t in toks if t[0] == 13 and t[2] == "x"]
x_acc19 = [t for t in toks if t[0] == 19 and t[2] == "x"]
check(len(x_acc13) == 1 and x_acc13[0][3] == "property", "after-dot member access `pt.x` -> property")
x_acc = x_acc19
check(len(x_acc) == 1 and x_acc[0][3] == "property", "after-dot member access `p.x` -> property")
sys.exit(1 if failures else 0)
PY
rc=$?
if [ $rc -eq 0 ]; then ok "lsp semanticTokens"; else bad "lsp semanticTokens"; fi

# ---- semanticTokens: the MODIFIERS ---------------------------------------
# The fifth integer of each token is a bitset over the legend's tokenModifiers,
# whose ORDER is therefore the wire format. Every check below reads the legend
# from `initialize` and derives the bit itself, so a reordered legend cannot
# make this pass by accident.
#
# What each modifier claims, and why it is worth a bit:
#   declaration     the name right after the keyword that declares it. Exact.
#   readonly        a `let` binding, at its declaration AND at its uses. This is
#                   the one that earns the feature in Scaly: `let` versus `var`
#                   is the whole mutability story, and the use site does not
#                   repeat it. A `var` must NOT carry it -- that pair is the
#                   test, a `let` alone would pass on a rule that just says yes.
#   static          a module-level `mutable` / `shared` global, declaration and
#                   uses.
#   defaultLibrary  a name from the package dependency tree.
#
# ★The defaultLibrary VETO has its own fixture, because the name tables are
# keyed on the bare name: a local, a parameter or this file's own declaration
# that shares a name with the package tree would inherit `defaultLibrary`
# without it. Measured on scalyls/nameset.scaly before the veto: a `let sp`, a
# parameter `size` and the file's own `malloc extern` all came back as library.
SCALY_HOME="$(pwd)" python3 - <<'PY'
import sys, json, subprocess, os, shutil

ws = "/tmp/lsp_ws/semtokens_mods"
shutil.rmtree(ws, ignore_errors=True); os.makedirs(ws)
src = ("mutable counter: int 0\n"                    # 0
       "\n"
       "define Point\n(\n    x: int\n)\n"            # 2..5
       "\n"
       "function demo(a: int) returns int\n"         # 7
       "{\n"
       "    let fixed 3\n"                           # 9
       "    var moving 4\n"                          # 10
       "    set moving: fixed + counter\n"           # 11
       "    let s String(\"hi\")\n"                  # 12
       "    return moving + fixed + s.get_length()\n"  # 13
       "}\n")
path = ws + "/lsp_semtok_mods.scaly"
open(path, "w").write(src)

def frame(o):
    b=json.dumps(o).encode(); return ("Content-Length: %d\r\n\r\n"%len(b)).encode()+b
inp  = frame({"jsonrpc":"2.0","id":1,"method":"initialize","params":{}})
inp += frame({"jsonrpc":"2.0","method":"initialized","params":{}})
inp += frame({"jsonrpc":"2.0","method":"textDocument/didOpen","params":{
    "textDocument":{"uri":"file://"+path,"languageId":"scaly","version":1,"text":src}}})
inp += frame({"jsonrpc":"2.0","id":2,"method":"textDocument/semanticTokens/full",
              "params":{"textDocument":{"uri":"file://"+path}}})
inp += frame({"jsonrpc":"2.0","id":9,"method":"shutdown"})
inp += frame({"jsonrpc":"2.0","method":"exit"})
out = subprocess.run(["/tmp/scalyls"], input=inp, stdout=subprocess.PIPE).stdout
frames, d = [], out
while d:
    i = d.find(b"\r\n\r\n")
    if i < 0: break
    n = int(d[:i].decode().split(":")[1].strip())
    frames.append(json.loads(d[i+4:i+4+n])); d = d[i+4+n:]

failures = 0
def check(cond, label):
    global failures
    print(("PASS  " if cond else "FAIL  ") + label)
    if not cond: failures += 1

legend = frames[0]["result"]["capabilities"]["semanticTokensProvider"]["legend"]
MODS = legend["tokenModifiers"]
check(MODS == ["declaration", "readonly", "static", "defaultLibrary"],
      "the legend advertises the four modifiers, in wire order -- %s" % (MODS,))

data = next(f for f in frames if f.get("id") == 2)["result"]["data"]
lines = src.split("\n")
toks = []
L = C = 0
for k in range(0, len(data), 5):
    dl, dc, ln, ty, mo = data[k:k+5]
    L += dl; C = dc if dl else C + dc
    toks.append((L, C, lines[L][C:C+ln],
                 set(MODS[b] for b in range(len(MODS)) if mo & (1 << b))))

def mods_at(line, text):
    hit = [t[3] for t in toks if t[0] == line and t[2] == text]
    return hit[0] if hit else None

check(mods_at(0, "counter") == {"declaration", "static"},
      "a module-level mutable declares and is static -- %s" % (mods_at(0, "counter"),))
check(mods_at(11, "counter") == {"static"},
      "and its USE is static without declaration -- %s" % (mods_at(11, "counter"),))
check(mods_at(2, "Point") == {"declaration"}, "a concept name declares")
check(mods_at(4, "x") == {"declaration"}, "a field declares")
check(mods_at(7, "demo") == {"declaration"}, "a routine name declares")
check(mods_at(9, "fixed") == {"declaration", "readonly"},
      "a `let` declares and is readonly -- %s" % (mods_at(9, "fixed"),))
check(mods_at(11, "fixed") == {"readonly"},
      "and its USE stays readonly -- %s" % (mods_at(11, "fixed"),))
check(mods_at(10, "moving") == {"declaration"},
      "a `var` declares and is NOT readonly -- %s" % (mods_at(10, "moving"),))
check(mods_at(11, "moving") == set(),
      "and its use carries nothing -- %s" % (mods_at(11, "moving"),))
check(mods_at(12, "String") == {"defaultLibrary"},
      "a stdlib type is defaultLibrary -- %s" % (mods_at(12, "String"),))
check(mods_at(13, "get_length") == {"defaultLibrary"},
      "so is a stdlib method -- %s" % (mods_at(13, "get_length"),))
check(mods_at(7, "a") == set(), "a parameter carries none of them -- %s" % (mods_at(7, "a"),))

# The veto: names of this file that a package tree also declares.
ws2 = "/tmp/lsp_ws/semtokens_veto"
shutil.rmtree(ws2, ignore_errors=True); os.makedirs(ws2)
src2 = ("function malloc(size: size_t) returns pointer[void] extern\n"   # 0
        "\n"
        "function use_it() returns int\n{\n"                             # 2,3
        "    let get_length 7\n"                                         # 4
        "    let p malloc(8)\n"                                          # 5
        "    return get_length\n}\n")                                    # 6
path2 = ws2 + "/lsp_semtok_veto.scaly"
open(path2, "w").write(src2)
inp  = frame({"jsonrpc":"2.0","id":1,"method":"initialize","params":{}})
inp += frame({"jsonrpc":"2.0","method":"initialized","params":{}})
inp += frame({"jsonrpc":"2.0","method":"textDocument/didOpen","params":{
    "textDocument":{"uri":"file://"+path2,"languageId":"scaly","version":1,"text":src2}}})
inp += frame({"jsonrpc":"2.0","id":2,"method":"textDocument/semanticTokens/full",
              "params":{"textDocument":{"uri":"file://"+path2}}})
inp += frame({"jsonrpc":"2.0","id":9,"method":"shutdown"})
inp += frame({"jsonrpc":"2.0","method":"exit"})
out = subprocess.run(["/tmp/scalyls"], input=inp, stdout=subprocess.PIPE).stdout
frames2, d = [], out
while d:
    i = d.find(b"\r\n\r\n")
    if i < 0: break
    n = int(d[:i].decode().split(":")[1].strip())
    frames2.append(json.loads(d[i+4:i+4+n])); d = d[i+4+n:]
data2 = next(f for f in frames2 if f.get("id") == 2)["result"]["data"]
lines2 = src2.split("\n")
toks = []
L = C = 0
for k in range(0, len(data2), 5):
    dl, dc, ln, ty, mo = data2[k:k+5]
    L += dl; C = dc if dl else C + dc
    toks.append((L, C, lines2[L][C:C+ln],
                 set(MODS[b] for b in range(len(MODS)) if mo & (1 << b))))
# An exact set, not a `not in`: a token the walk failed to find would answer
# None and pass a membership test vacuously.
check(mods_at(5, "malloc") == set(),
      "a name this file declares itself is not library -- %s" % (mods_at(5, "malloc"),))
check(mods_at(6, "get_length") == {"readonly"},
      "nor is a LOCAL that shares a stdlib name -- %s" % (mods_at(6, "get_length"),))
sys.exit(1 if failures else 0)
PY
rc=$?
if [ $rc -eq 0 ]; then ok "lsp semanticTokens modifiers"; else bad "lsp semanticTokens modifiers"; fi

# ---- semanticTokens: word operators, value words, and the caret ----
# Scaly has no operator token class: an operator IS an identifier, one made
# exclusively of operator characters (lexer.scaly:326,:338 return
# Token.Identifier from scan_operator). So the character class IS the
# definition, and it is generated into grammar.is_op_char by
# codegen/highlight-scaly.scm; the four WORD operators are the only exceptions
# to it and the only ones that need a name test.
#
# Three ways this used to be wrong, all silent, all fixed 2026-08-11:
#   * and/or/not/xor          -> `variable`, i.e. painted as bindings
#   * true/false/null/this    -> `variable`, i.e. painted as bindings
#   * '^'                     -> `operator`, though the lexer excludes it from
#                                the operator characters ON PURPOSE
#                                (lexer.scaly:330 — the caret is the lifetime
#                                sigil, so `&^this v` must tokenize as
#                                `& ^this v`). Both highlighter layers pulled it
#                                back in, so `&^` came out as ONE operator run.
# The caret assertion is a NEGATIVE one and that is the point: the sigil must
# produce no token at all, so a regression shows up as a token appearing rather
# than as a wrong colour.
python3 - <<'PY'
import sys, json, subprocess, os, tempfile
src = ("define Probe\n"                              # 0
       "{\n"                                        # 1
       "    function pick(this, a: bool, b: bool) returns bool\n"  # 2
       "    {\n"                                    # 3
       "        let t true\n"                       # 4
       "        let n null\n"                       # 5
       "        if a and b\n"                       # 6
       "            return not a\n"                 # 7
       "        if a or b\n"                        # 8
       "            return false\n"                 # 9
       "        let x 1 xor 2\n"                    # 10
       "        let r &^this a\n"                   # 11 <- the real shape from
       "        a\n"                                # 12    Planner.scaly:4635
       "    }\n"                                    # 13
       "}\n")                                       # 14
d = tempfile.mkdtemp(prefix="lsp_words_")
path = os.path.join(d, "w.scaly")
open(path, "w").write(src)
def frame(o):
    b=json.dumps(o).encode(); return ("Content-Length: %d\r\n\r\n"%len(b)).encode()+b
inp  = frame({"jsonrpc":"2.0","id":1,"method":"initialize","params":{}})
inp += frame({"jsonrpc":"2.0","method":"initialized","params":{}})
inp += frame({"jsonrpc":"2.0","id":2,"method":"textDocument/semanticTokens/full",
              "params":{"textDocument":{"uri":"file://"+path}}})
inp += frame({"jsonrpc":"2.0","id":9,"method":"shutdown"})
inp += frame({"jsonrpc":"2.0","method":"exit"})
out = subprocess.run(["/tmp/scalyls"], input=inp, stdout=subprocess.PIPE).stdout
frames, d = [], out
while d:
    i = d.find(b"\r\n\r\n")
    if i < 0: break
    n = int(d[:i].decode().split(":")[1].strip())
    frames.append(json.loads(d[i+4:i+4+n])); d = d[i+4+n:]
failures = 0
def check(cond, label):
    global failures
    print(("PASS  " if cond else "FAIL  ") + label)
    if not cond: failures += 1
TYPES = ["keyword","type","function","variable","operator","string","number","comment","parameter","property"]
r = next((f for f in frames if f.get("id") == 2), None)
data = (r or {}).get("result",{}).get("data") or []
lines = src.split("\n")
line = col = 0
toks = []
for k in range(0, len(data), 5):
    dl, dc, ln, ty, mod = data[k:k+5]
    if dl > 0: line += dl; col = dc
    else:      col += dc
    toks.append((line, col, lines[line][col:col+ln], TYPES[ty]))
def kinds(text): return sorted(set(t[3] for t in toks if t[2] == text))
# The word operators. `variable` here means they were read as bindings.
for w in ("and", "or", "not", "xor"):
    check(kinds(w) == ["operator"], "word operator `%s` -> operator (got %s)" % (w, kinds(w) or "no token"))
# The value words. `this` is asserted at BOTH its positions - the parameter on
# line 2 and the sigil use on line 11 - because param_in_scope would otherwise
# claim it as a parameter; `this` is never a user binding.
for w in ("true", "false", "null"):
    check(kinds(w) == ["keyword"], "value word `%s` -> keyword (got %s)" % (w, kinds(w) or "no token"))
this_toks = [t for t in toks if t[2] == "this"]
check(len(this_toks) == 2 and all(t[3] == "keyword" for t in this_toks),
      "`this` -> keyword at both the parameter and the sigil (got %s)" % [t[3] for t in this_toks])
# The caret produces NO token: it is punctuation (the lifetime sigil), never an
# operator character. A token whose TEXT contains '^' catches both the lone
# sigil and the `&^` run that the old character class merged into one operator.
carets = [t for t in toks if "^" in t[2]]
check(carets == [], "'^' produces no token - it is the sigil, not an operator (got %s)" % carets)
# The '&' on line 11 is still an operator, and it stands ALONE: length 1, so the
# run stopped at the caret exactly as the lexer's does.
amp = [t for t in toks if t[0] == 11 and t[3] == "operator"]
check(len(amp) == 1 and amp[0][2] == "&", "`&^` scans as a lone `&` operator (got %s)" % amp)
sys.exit(1 if failures else 0)
PY
rc=$?
if [ $rc -eq 0 ]; then ok "lsp semanticTokens word operators"; else bad "lsp semanticTokens word operators"; fi

# ---- semanticTokens: cross-file declared-kind resolution ----
# An identifier USE resolves to function/type by its DECLARATION even when the
# declaration lives in a SIBLING file (harvested via the symindex). Both probe
# names are LOWERCASE, so without cross-file resolution they would fall to the
# shape default (variable); getting function/type proves the cross-file walk.
python3 - <<'PY'
import sys, json, subprocess, os, tempfile
d = tempfile.mkdtemp(prefix="lsp_semx_")
lib = ("function compute(n: int) returns int\n{\n    return n + 1\n}\n\n"
       "define widget\n(\n    g: int\n)\n{\n}\n")
main = ("function run(x: int) returns int\n"   # 0
        "{\n"                                   # 1
        "    let w widget(x)\n"                 # 2  <- `widget` type (cross-file)
        "    return compute(x)\n"               # 3  <- `compute` function (cross-file)
        "}\n")                                  # 4
lp = os.path.join(d, "lib.scaly");  open(lp, "w").write(lib)
mp = os.path.join(d, "main.scaly"); open(mp, "w").write(main)
muri = "file://"+mp
def frame(o):
    b=json.dumps(o).encode(); return ("Content-Length: %d\r\n\r\n"%len(b)).encode()+b
inp  = frame({"jsonrpc":"2.0","id":1,"method":"initialize","params":{}})
inp += frame({"jsonrpc":"2.0","method":"initialized","params":{}})
inp += frame({"jsonrpc":"2.0","method":"textDocument/didOpen",
              "params":{"textDocument":{"uri":muri,"languageId":"scaly","version":1,"text":main}}})
inp += frame({"jsonrpc":"2.0","id":2,"method":"textDocument/semanticTokens/full",
              "params":{"textDocument":{"uri":muri}}})
inp += frame({"jsonrpc":"2.0","id":9,"method":"shutdown"})
inp += frame({"jsonrpc":"2.0","method":"exit"})
out = subprocess.run(["/tmp/scalyls"], input=inp, stdout=subprocess.PIPE).stdout
frames, dd = [], out
while dd:
    i = dd.find(b"\r\n\r\n")
    if i < 0: break
    n = int(dd[:i].decode().split(":")[1].strip())
    frames.append(json.loads(dd[i+4:i+4+n])); dd = dd[i+4+n:]
failures = 0
def check(cond, label):
    global failures
    print(("PASS  " if cond else "FAIL  ") + label)
    if not cond: failures += 1
r = next((f for f in frames if f.get("id") == 2), None)
data = (r or {}).get("result",{}).get("data") or []
TYPES = ["keyword","type","function","variable","operator","string","number","comment","parameter","property"]
lines = main.split("\n"); line = col = 0; toks = []
for k in range(0, len(data), 5):
    dl, dc, ln, ty, mod = data[k:k+5]
    if dl > 0: line += dl; col = dc
    else:      col += dc
    toks.append((line, col, lines[line][col:col+ln], TYPES[ty]))
w_uses = [t for t in toks if t[0] == 2 and t[2] == "widget"]
c_uses = [t for t in toks if t[0] == 3 and t[2] == "compute"]
check(len(w_uses) == 1 and w_uses[0][3] == "type",
      "cross-file type use `widget` -> type (lowercase, sibling file)")
check(len(c_uses) == 1 and c_uses[0][3] == "function",
      "cross-file function use `compute` -> function (lowercase, sibling file)")
# the local `w` must stay variable (no cross-file confusion).
w_local = [t for t in toks if t[0] == 2 and t[2] == "w"]
check(len(w_local) == 1 and w_local[0][3] == "variable", "local `w` stays variable")
sys.exit(1 if failures else 0)
PY
rc=$?
if [ $rc -eq 0 ]; then ok "lsp semanticTokens cross-file"; else bad "lsp semanticTokens cross-file"; fi

# ---- semanticTokens: cross-package declared-kind resolution ----
# Step 28: the hashed name set removed the size gate AND re-enabled the package
# tree scan, so a USE of a stdlib symbol now resolves to its declared kind. The
# doc lives in a /tmp dir NOT under packages/, so the only way `is_prime` (a
# LOWERCASE top-level function in packages/scaly/.../hashing.scaly) can colour
# as `function` is the cross-package harvest. SCALY_HOME points at the repo.
SCALY_HOME="$(pwd)" python3 - <<'PY'
import sys, json, subprocess, os, tempfile
d = tempfile.mkdtemp(prefix="lsp_semp_")
main = ("function run(n: size_t) returns bool\n"  # 0
        "{\n"                                     # 1
        "    return is_prime(n)\n"                # 2  <- `is_prime` fn (cross-package)
        "}\n")                                    # 3
mp = os.path.join(d, "main.scaly"); open(mp, "w").write(main)
muri = "file://"+mp
def frame(o):
    b=json.dumps(o).encode(); return ("Content-Length: %d\r\n\r\n"%len(b)).encode()+b
inp  = frame({"jsonrpc":"2.0","id":1,"method":"initialize","params":{}})
inp += frame({"jsonrpc":"2.0","method":"initialized","params":{}})
inp += frame({"jsonrpc":"2.0","method":"textDocument/didOpen",
              "params":{"textDocument":{"uri":muri,"languageId":"scaly","version":1,"text":main}}})
inp += frame({"jsonrpc":"2.0","id":2,"method":"textDocument/semanticTokens/full",
              "params":{"textDocument":{"uri":muri}}})
inp += frame({"jsonrpc":"2.0","id":9,"method":"shutdown"})
inp += frame({"jsonrpc":"2.0","method":"exit"})
out = subprocess.run(["/tmp/scalyls"], input=inp, stdout=subprocess.PIPE,
                     env={**os.environ, "SCALY_HOME": os.getcwd()}).stdout
frames, dd = [], out
while dd:
    i = dd.find(b"\r\n\r\n")
    if i < 0: break
    n = int(dd[:i].decode().split(":")[1].strip())
    frames.append(json.loads(dd[i+4:i+4+n])); dd = dd[i+4+n:]
failures = 0
def check(cond, label):
    global failures
    print(("PASS  " if cond else "FAIL  ") + label)
    if not cond: failures += 1
r = next((f for f in frames if f.get("id") == 2), None)
data = (r or {}).get("result",{}).get("data") or []
TYPES = ["keyword","type","function","variable","operator","string","number","comment","parameter","property"]
lines = main.split("\n"); line = col = 0; toks = []
for k in range(0, len(data), 5):
    dl, dc, ln, ty, mod = data[k:k+5]
    if dl > 0: line += dl; col = dc
    else:      col += dc
    toks.append((line, col, lines[line][col:col+ln], TYPES[ty]))
ip = [t for t in toks if t[0] == 2 and t[2] == "is_prime"]
check(len(ip) == 1 and ip[0][3] == "function",
      "cross-package fn use `is_prime` -> function (stdlib, packages/ tree)")
sys.exit(1 if failures else 0)
PY
rc=$?
if [ $rc -eq 0 ]; then ok "lsp semanticTokens cross-package"; else bad "lsp semanticTokens cross-package"; fi

# ---- semantic hover: planner-resolved types the lexical walk can't do ----
# Runs the full modeler+planner in the worker and maps the cursor offset to
# the innermost planned node's resolved type. Verifies the four lexical-
# impossible cases (generic element type, call result, pointer/ref inner,
# mid-chain call), the lexical FALLBACK on a declaration / whitespace, and
# that a parse-error document still gets a (null) answer without crashing.
python3 - <<'PY'
import sys, json, subprocess
src = ("define Box[T](value: T)\n"
       "{\n"
       "    function get(this: Box[T]) returns T\n"
       "    {\n"
       "        return value\n"
       "    }\n"
       "}\n"
       "\n"
       "define Inner(z: int)\n"
       "{\n"
       "    function gz(this: Inner) returns int\n"
       "    {\n"
       "        return z\n"
       "    }\n"
       "}\n"
       "\n"
       "define Outer(inner: Inner)\n"
       "{\n"
       "    function gi(this: Outer) returns Inner\n"
       "    {\n"
       "        return inner\n"
       "    }\n"
       "}\n"
       "\n"
       "function make() returns int\n"
       "{\n"
       "    return 7\n"
       "}\n"
       "\n"
       "function use_it(p: pointer[int], o: Outer) returns int\n"
       "{\n"
       "    var b Box[int](5)\n"
       "    let g b.get()\n"
       "    let r make()\n"
       "    let v *p\n"
       "    let c o.gi().z\n"
       "    return r\n"
       "}\n")
import os as _os, shutil as _sh; _ws = "/tmp/lsp_ws/sem_hover"; _sh.rmtree(_ws, ignore_errors=True); _os.makedirs(_ws)
path = _ws + "/lsp_sem_hover.scaly"
open(path, "w").write(src)
lines = src.split("\n")
def loc(unique, token):
    li = next(i for i, l in enumerate(lines) if unique in l)
    return li, lines[li].index(token)
def frame(o):
    b=json.dumps(o).encode(); return ("Content-Length: %d\r\n\r\n"%len(b)).encode()+b
def hov(idn, line, char):
    return frame({"jsonrpc":"2.0","id":idn,"method":"textDocument/hover",
                  "params":{"textDocument":{"uri":"file://"+path},
                            "position":{"line":line,"character":char}}})
# Case positions (located by substring so line/col stay correct on edits).
g_l, g_c   = loc("let g b.get()", "get")       # generic element type -> int
mk_l, mk_c = loc("let r make()",  "make")      # call result          -> int
pt_l, pt_c = loc("let v *p",      "*")         # pointer inner (*p)   -> int
ch_l, ch_c = loc("let c o.gi().z","gi")        # mid-chain a.f().b    -> int
de_l, de_c = loc("function make()","make")     # declaration name -> lexical
inp  = frame({"jsonrpc":"2.0","id":1,"method":"initialize","params":{}})
inp += frame({"jsonrpc":"2.0","method":"initialized","params":{}})
inp += hov(2, g_l, g_c)
inp += hov(3, mk_l, mk_c)
inp += hov(4, pt_l, pt_c)
inp += hov(5, ch_l, ch_c)
inp += hov(6, de_l, de_c)
inp += frame({"jsonrpc":"2.0","id":7,"method":"shutdown"})
inp += frame({"jsonrpc":"2.0","method":"exit"})
out = subprocess.run(["/tmp/scalyls"], input=inp, stdout=subprocess.PIPE).stdout
frames, d = [], out
while d:
    i = d.find(b"\r\n\r\n")
    if i < 0: break
    n = int(d[:i].decode().split(":")[1].strip())
    frames.append(json.loads(d[i+4:i+4+n])); d = d[i+4+n:]
failures = 0
def check(cond, label):
    global failures
    print(("PASS  " if cond else "FAIL  ") + label)
    if not cond: failures += 1
def val(idn):
    f = next((x for x in frames if x.get("id") == idn), None)
    r = (f or {}).get("result")
    if r is None: return None
    return r.get("contents", {}).get("value")
check(val(2) == "int", "generic element type b.get() -> int (lexical sees T)")
check(val(3) == "int", "call result make() -> int")
check(val(4) == "int", "pointer inner *p -> int")
check(val(5) == "int", "mid-chain o.gi().z -> int (lexical stops at '(')")
check(val(6) == "function make() returns int",
      "declaration name falls back to the lexical hover (its signature)")
sys.exit(1 if failures else 0)
PY
rc=$?
if [ $rc -eq 0 ]; then ok "lsp semantic hover"; else bad "lsp semantic hover"; fi

# ---- semantic hover through the PACKAGE ROOT + the honest fallback -------
# A file that is a MODULE of a package cannot be planned on its own: the
# one-module program the modeler builds has no sibling, so every call into one
# was unresolved and the lexical fallback answered the ENCLOSING ROUTINE for it.
# Measured 2026-08-15 through the LSP plugin on scalyls/server.scaly, where
# `rpc.read_message()` reported `function process_one` while goToDefinition at
# the same position resolved correctly — the two contradicted each other. Same
# gap, same fix as diagnostics.package_report# and codelens.planned_fragment#.
#
# Four things are asserted, and the first three FAIL on the pre-fix binary:
#   1. the sibling call resolves (the defect itself),
#   2. the binding bound to it resolves,
#   3. a genuine miss (a brace) says `in function use_it`, not `function use_it`
#      — the fallback names WHERE the cursor is, and may not be readable as an
#      answer about the token under it,
#   4. an UNSAVED buffer keeps the document-plan behaviour. That is not a
#      nicety: the modeler reads every module from DISK, so a root plan's spans
#      index the SAVED bytes while the offset came from the buffer — resolving
#      anyway would trade one silent wrong answer for another.
#
# ★ensure_ascii=False, unlike every other block here, and it is load-bearing:
# json.decode_escape# maps a \uXXXX escape above 0x7F to a single '?' byte, so
# a python-default didOpen of a file with `★` or `—` in it delivers a buffer
# SHORTER than the file (server.scaly: 60440 against 60526 bytes) and the
# saved-buffer test in (1)…(3) would silently take the unsaved path. A real
# client (JSON.stringify) sends raw UTF-8, which is what this models.
python3 - <<'PY'
import sys, json, subprocess, os, shutil

ws = "/tmp/lsp_ws/sem_root"
shutil.rmtree(ws, ignore_errors=True)
os.makedirs(ws + "/app")
# A minimal PACKAGE: the root names two modules, one calls the other.
open(ws + "/app.scaly", "w").write(
    "package scaly 0.1.0\n\ndefine app\n{\n    module util\n    module user\n}\n")
open(ws + "/app/util.scaly", "w").write(
    "define util\n"
    "{\n"
    "    function num() returns int\n"
    "    {\n"
    "        return 7\n"
    "    }\n"
    "\n"
    "    function txt() returns String\n"
    "    {\n"
    "        return String(\"x\")\n"
    "    }\n"
    "}\n")
# ★The two call lines are the SAME LENGTH on purpose — that is what makes the
# unsaved-buffer control below able to fail. See it.
src = ("define user\n"
       "{\n"
       "    function use_it() returns int\n"
       "    {\n"
       "        let a util.num()\n"
       "        let s util.txt()\n"
       "        return a\n"
       "    }\n"
       "}\n")
path = ws + "/app/user.scaly"
open(path, "w").write(src)
uri = "file://" + path
lines = src.split("\n")

def frame(o):
    b = json.dumps(o, ensure_ascii=False).encode()
    return ("Content-Length: %d\r\n\r\n" % len(b)).encode() + b
def hov(idn, u, line, char):
    return frame({"jsonrpc":"2.0","id":idn,"method":"textDocument/hover",
                  "params":{"textDocument":{"uri":u},
                            "position":{"line":line,"character":char}}})
def loc(unique, token):
    li = next(i for i, l in enumerate(lines) if unique in l)
    return li, lines[li].index(token)

call_l, call_c = loc("let a util.num()", "num")     # the SIBLING module's call
bind_l, bind_c = loc("let a util.num()", "a")       # the binding bound to it
brace_l = next(i for i, l in enumerate(lines) if l == "    {")  # body open brace

# The real tree at real scale: the very position the wish list names.
srv = os.path.join(os.getcwd(), "packages/scalyls/0.1.0/scalyls/server.scaly")
srv_uri = "file://" + srv
srv_doc = open(srv).read()
srv_lines = srv_doc.split("\n")
srv_l = next(i for i, l in enumerate(srv_lines) if l.strip() == "let body rpc.read_message()")
srv_c = srv_lines[srv_l].index("read_message")

inp  = frame({"jsonrpc":"2.0","id":1,"method":"initialize","params":{}})
inp += frame({"jsonrpc":"2.0","method":"initialized","params":{}})
inp += frame({"jsonrpc":"2.0","method":"textDocument/didOpen","params":{"textDocument":{
        "uri":uri,"languageId":"scaly","version":1,"text":src}}})
inp += hov(2, uri, call_l, call_c)
inp += hov(3, uri, bind_l, bind_c)
inp += hov(4, uri, brace_l, 4)
inp += frame({"jsonrpc":"2.0","method":"textDocument/didOpen","params":{"textDocument":{
        "uri":srv_uri,"languageId":"scaly","version":1,"text":srv_doc}}})
inp += hov(5, srv_uri, srv_l, srv_c)
# Now the SAME document as an unsaved buffer. The edit is placed and SIZED so
# that a missing guard has to answer WRONG rather than merely differently — it
# took two tries to get a control that can fail at all:
#   * INSIDE the body, below the routine header, so `use_it`'s own start is
#     unchanged and is_routine_start# still accepts it. A comment prepended to
#     the FILE moves every routine start, the identity check rejects the whole
#     plan, and the probe passes with the guard REMOVED — proving nothing.
#   * exactly as long as the distance from `num` to `txt`, so the buffer offset
#     of `num` IS the saved offset of `txt`. Drop the `disk = source` test in
#     resolve_through_root# and this probe answers `String`: the type of the
#     OTHER call, read out of a file the buffer no longer matches.
pad = src.index("txt") - src.index("num")
edited = "\n".join(lines[:call_l] + [";" + " " * (pad - 2)] + lines[call_l:])
inp += frame({"jsonrpc":"2.0","method":"textDocument/didChange","params":{
        "textDocument":{"uri":uri,"version":2},
        "contentChanges":[{"text":edited}]}})
inp += hov(6, uri, call_l + 1, call_c)
inp += frame({"jsonrpc":"2.0","id":9,"method":"shutdown"})
inp += frame({"jsonrpc":"2.0","method":"exit"})

out = subprocess.run(["/tmp/scalyls"], input=inp, stdout=subprocess.PIPE).stdout
frames, d = [], out
while d:
    i = d.find(b"\r\n\r\n")
    if i < 0: break
    n = int(d[:i].decode().split(":")[1].strip())
    frames.append(json.loads(d[i+4:i+4+n])); d = d[i+4+n:]
failures = 0
def check(cond, label):
    global failures
    print(("PASS  " if cond else "FAIL  ") + label)
    if not cond: failures += 1
def val(idn):
    f = next((x for x in frames if x.get("id") == idn), None)
    r = (f or {}).get("result")
    return None if r is None else r.get("contents", {}).get("value")

check(val(2) == "int", "a SIBLING module's call resolves through the package root")
check(val(3) == "int", "the binding bound to it resolves too")
check(val(4) == "in function use_it",
      "a genuine miss says WHERE the cursor is (`in function use_it`)")
check(val(5) == "String",
      "the tree's own case: rpc.read_message() in server.scaly -> String")
check(val(6) == "in function use_it",
      "an UNSAVED buffer keeps the document plan (offsets index the saved file)")
sys.exit(1 if failures else 0)
PY
rc=$?
if [ $rc -eq 0 ]; then ok "lsp semantic hover through the package root"; else bad "lsp semantic hover through the package root"; fi

# ---- \uXXXX escapes decode to their UTF-8 bytes ------------------------------
# json.decode_escape# used to answer ONE '?' byte for any \u escape above 0x7F,
# on the comment's own admission a deliberate simplification. The CONSEQUENCE is
# what makes it a defect: a client that escapes non-ASCII delivers a buffer
# SHORTER than the file (measured on server.scaly: 60440 against 60526 bytes
# over 43 `★`/`—`), and from the first hit onwards EVERY offset is wrong —
# diagnostics, hover, all of it. Exposition is small but real: VS Code and
# Claude Code send raw UTF-8 via JSON.stringify, but JSON permits the escapes
# and python's json.dumps emits them BY DEFAULT.
#
# So this whole block frames with ensure_ascii=True — the opposite of the block
# above, which needs raw UTF-8 and says so.
#
# The observable is the BYTE COUNT, not the content, and it is borrowed rather
# than invented: resolve_through_root# only plans the package root when the
# buffer EQUALS the file on disk, so a buffer one byte off silently loses every
# cross-module answer. One fixture per encoding width (2, 3 and 4 bytes, the
# last one a SURROGATE PAIR), each with its non-ASCII on an EARLIER LINE than
# the hovered token — deliberately not on the same line, because scalyls counts
# columns in bytes while the client sends UTF-16, and that separate, documented
# gap would make this test fail for the wrong reason.
#
# The fourth check is the control: the same document sent one byte SHORT must
# fall back. Without it, three green checks would prove only that hover works.
python3 - <<'PY'
import sys, json, subprocess, os, shutil

def frame(o):
    b = json.dumps(o, ensure_ascii=True).encode()   # ★ the point of this block
    return ("Content-Length: %d\r\n\r\n" % len(b)).encode() + b

def build(name, mark):
    ws = "/tmp/lsp_ws/" + name
    shutil.rmtree(ws, ignore_errors=True)
    os.makedirs(ws + "/app")
    open(ws + "/app.scaly", "w").write(
        "package scaly 0.1.0\n\ndefine app\n{\n    module util\n    module user\n}\n")
    open(ws + "/app/util.scaly", "w").write(
        "define util\n"
        "{\n"
        "    function num() returns int\n"
        "    {\n"
        "        return 7\n"
        "    }\n"
        "}\n")
    src = ("; " + mark + " a comment carrying the non-ASCII, on its OWN line\n"
           "define user\n"
           "{\n"
           "    function use_it() returns int\n"
           "    {\n"
           "        let a util.num()\n"
           "        return a\n"
           "    }\n"
           "}\n")
    path = ws + "/app/user.scaly"
    open(path, "w", encoding="utf-8").write(src)
    return path, src

# 2 bytes (U+00FC), 3 bytes (U+2605), 4 bytes as a SURROGATE PAIR (U+1F600).
cases = [("esc2", "üü"), ("esc3", "★—"), ("esc4", "\U0001F600")]

inp  = frame({"jsonrpc":"2.0","id":1,"method":"initialize","params":{}})
inp += frame({"jsonrpc":"2.0","method":"initialized","params":{}})
ids = {}
idn = 2
for name, mark in cases:
    path, src = build(name, mark)
    uri = "file://" + path
    lines = src.split("\n")
    l = next(i for i, x in enumerate(lines) if "util.num()" in x)
    c = lines[l].index("num")
    inp += frame({"jsonrpc":"2.0","method":"textDocument/didOpen","params":{"textDocument":{
            "uri":uri,"languageId":"scaly","version":1,"text":src}}})
    inp += frame({"jsonrpc":"2.0","id":idn,"method":"textDocument/hover",
                  "params":{"textDocument":{"uri":uri},
                            "position":{"line":l,"character":c}}})
    ids[name] = idn
    idn += 1

# The control: the third fixture again, one byte short of its file.
path, src = build("esc_short", "★")
uri = "file://" + path
lines = src.split("\n")
l = next(i for i, x in enumerate(lines) if "util.num()" in x)
c = lines[l].index("num")
inp += frame({"jsonrpc":"2.0","method":"textDocument/didOpen","params":{"textDocument":{
        "uri":uri,"languageId":"scaly","version":1,"text":src[:-1]}}})
inp += frame({"jsonrpc":"2.0","id":idn,"method":"textDocument/hover",
              "params":{"textDocument":{"uri":uri},
                        "position":{"line":l,"character":c}}})
ids["esc_short"] = idn

inp += frame({"jsonrpc":"2.0","id":99,"method":"shutdown"})
inp += frame({"jsonrpc":"2.0","method":"exit"})

out = subprocess.run(["/tmp/scalyls"], input=inp, stdout=subprocess.PIPE).stdout
frames, d = [], out
while d:
    i = d.find(b"\r\n\r\n")
    if i < 0: break
    n = int(d[:i].decode().split(":")[1].strip())
    frames.append(json.loads(d[i+4:i+4+n])); d = d[i+4+n:]
failures = 0
def check(cond, label):
    global failures
    print(("PASS  " if cond else "FAIL  ") + label)
    if not cond: failures += 1
def val(idn):
    f = next((x for x in frames if x.get("id") == idn), None)
    r = (f or {}).get("result")
    return None if r is None else r.get("contents", {}).get("value")

check(val(ids["esc2"]) == "int", "a 2-byte escape (\\u00fc) keeps the buffer byte-exact")
check(val(ids["esc3"]) == "int", "a 3-byte escape (\\u2605) keeps the buffer byte-exact")
check(val(ids["esc4"]) == "int", "a surrogate PAIR (\\ud83d\\ude00) decodes to its four bytes")
check(val(ids["esc_short"]) == "in function use_it",
      "the control: a buffer one byte short DOES fall back")
sys.exit(1 if failures else 0)
PY
rc=$?
if [ $rc -eq 0 ]; then ok "lsp unicode escapes decode to UTF-8"; else bad "lsp unicode escapes decode to UTF-8"; fi

# ---- semantic hover fallback: parse-error input still answers (no crash) ----
# A broken document fails the pipeline; the server must still return a valid
# response (null) and stay alive to answer the next request.
python3 - <<'PY'
import sys, json, subprocess
import os as _os, shutil as _sh; _ws = "/tmp/lsp_ws/sem_bad"; _sh.rmtree(_ws, ignore_errors=True); _os.makedirs(_ws)
bad = _ws + "/lsp_sem_bad.scaly"
open(bad, "w").write("function broken(a: int \n{\n    return a\n")  # missing ')'
good = _ws + "/lsp_sem_good.scaly"
open(good, "w").write("function fine(a: int) returns int\n{\n    return a\n}\n")
def frame(o):
    b=json.dumps(o).encode(); return ("Content-Length: %d\r\n\r\n"%len(b)).encode()+b
def hov(idn, p, line, char):
    return frame({"jsonrpc":"2.0","id":idn,"method":"textDocument/hover",
                  "params":{"textDocument":{"uri":"file://"+p},
                            "position":{"line":line,"character":char}}})
inp  = frame({"jsonrpc":"2.0","id":1,"method":"initialize","params":{}})
inp += frame({"jsonrpc":"2.0","method":"initialized","params":{}})
inp += hov(2, bad, 0, 12)                  # parse-error doc -> null, no crash
inp += hov(3, good, 0, 9)                  # server still alive -> lexical signature
inp += frame({"jsonrpc":"2.0","id":4,"method":"shutdown"})
inp += frame({"jsonrpc":"2.0","method":"exit"})
out = subprocess.run(["/tmp/scalyls"], input=inp, stdout=subprocess.PIPE).stdout
frames, d = [], out
while d:
    i = d.find(b"\r\n\r\n")
    if i < 0: break
    n = int(d[:i].decode().split(":")[1].strip())
    frames.append(json.loads(d[i+4:i+4+n])); d = d[i+4+n:]
failures = 0
def check(cond, label):
    global failures
    print(("PASS  " if cond else "FAIL  ") + label)
    if not cond: failures += 1
def get(idn): return next((x for x in frames if x.get("id") == idn), None)
f2 = get(2)
check(f2 is not None and f2.get("result") is None, "parse-error hover -> null (no crash)")
f3 = get(3)
v3 = (f3 or {}).get("result", {})
v3 = v3.get("contents", {}).get("value") if v3 else None
check(v3 == "function fine(a: int) returns int",
      "server alive after parse error -> lexical hover answers")
sys.exit(1 if failures else 0)
PY
rc=$?
if [ $rc -eq 0 ]; then ok "lsp semantic hover fallback"; else bad "lsp semantic hover fallback"; fi

# ---- semantic completion: planner-resolved variable receiver ----
# `let g make_w().unwrap()` then `g.` — the receiver g is bound from a method
# call on a CALL RESULT, which the lexical resolve_variable_type# cannot resolve
# (its chain capture stops at `(`). The semantic resolver reads g's use-site
# type (Point) and lists Point's members instead of the flat all-names list.
python3 - <<'PY'
import sys, json, subprocess
src = ("define Point(x: int)\n"
       "{\n"
       "    function px(this: Point) returns int { return x }\n"
       "}\n"
       "\n"
       "define Wrapper(p: Point)\n"
       "{\n"
       "    function unwrap(this: Wrapper) returns Point\n"
       "    {\n"
       "        return p\n"
       "    }\n"
       "}\n"
       "\n"
       "function make_w() returns Wrapper\n"
       "{\n"
       "    return Wrapper(Point(1))\n"
       "}\n"
       "\n"
       "function use_it() returns int\n"
       "{\n"
       "    let g make_w().unwrap()\n"
       "    let z g.px()\n"
       "    return z\n"
       "}\n")
import os as _os, shutil as _sh; _ws = "/tmp/lsp_ws/sem_completion"; _sh.rmtree(_ws, ignore_errors=True); _os.makedirs(_ws)
path = _ws + "/lsp_sem_completion.scaly"
open(path, "w").write(src)
uri = "file://" + path
lines = src.split("\n")
zl = next(i for i, l in enumerate(lines) if "let z g.px" in l)
zc = lines[zl].index("px")
def frame(o):
    b=json.dumps(o).encode(); return ("Content-Length: %d\r\n\r\n"%len(b)).encode()+b
def didopen():
    return frame({"jsonrpc":"2.0","method":"textDocument/didOpen",
                  "params":{"textDocument":{"uri":uri,"languageId":"scaly","version":1,"text":src}}})
def compl(idn, line, char):
    return frame({"jsonrpc":"2.0","id":idn,"method":"textDocument/completion",
                  "params":{"textDocument":{"uri":uri},"position":{"line":line,"character":char}}})
inp  = frame({"jsonrpc":"2.0","id":1,"method":"initialize","params":{}})
inp += frame({"jsonrpc":"2.0","method":"initialized","params":{}})
inp += didopen()
inp += compl(2, zl, zc)        # completion inside g.px -> Point's members
inp += frame({"jsonrpc":"2.0","id":3,"method":"shutdown"})
inp += frame({"jsonrpc":"2.0","method":"exit"})
out = subprocess.run(["/tmp/scalyls"], input=inp, stdout=subprocess.PIPE).stdout
frames, d = [], out
while d:
    i = d.find(b"\r\n\r\n")
    if i < 0: break
    n = int(d[:i].decode().split(":")[1].strip())
    frames.append(json.loads(d[i+4:i+4+n])); d = d[i+4+n:]
failures = 0
def check(cond, label):
    global failures
    print(("PASS  " if cond else "FAIL  ") + label)
    if not cond: failures += 1
f = next((x for x in frames if x.get("id") == 2), None)
items = (f or {}).get("result") or []
labels = [c.get("label") for c in items]
check("px" in labels, "semantic receiver g (= make_w().unwrap()) lists Point member px")
check("use_it" not in labels, "not the flat fallback (top-level use_it absent)")
sys.exit(1 if failures else 0)
PY
rc=$?
if [ $rc -eq 0 ]; then ok "lsp semantic completion"; else bad "lsp semantic completion"; fi

bg_collect
echo "-----"
echo "PASS: $pass  FAIL: $fail"
[ $fail -eq 0 ]
