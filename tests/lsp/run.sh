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
LIBDIR=""
if command -v brew >/dev/null 2>&1; then
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

# ---- scalyls build backend (cpp single-binary | selfhosted 4-root) --------
LSO=""        # dir holding the shared .o objects (selfhosted only)
if [ "$MODE" = selfhosted ]; then
    set +u; source tools/llvm-env.sh >/dev/null; set -u
    [ "${llvm_env_ok:-0}" = "1" ] || { echo "FAIL  llvm-env (need LLVM 20 for selfhosted)"; exit 1; }
    echo "scalyls build: selfhosted 4-root via $SCALYC"
    LSO="$(mktemp -d)"
    trap 'rm -rf "$LSO"' EXIT
    ( ulimit -s 65520
      "$SCALYC" -S --no-tests -o "$LSO/scalyc.ll"       packages/scalyc/0.1.0/scalyc.scaly
      "$SCALYC" -S --no-tests -o "$LSO/scaly.ll"        packages/scaly/0.1.0/scaly.scaly
      "$SCALYC" -S --no-tests -o "$LSO/scalyls.ll"      packages/scalyls/0.1.0/scalyls.scaly
      "$SCALYC" -S --no-tests -o "$LSO/scalyls_main.ll" packages/scalyls/0.1.0/main.scaly
    ) || { echo "FAIL  selfhosted scalyls emission"; exit 1; }
    for f in scalyc scaly scalyls scalyls_main; do
        "$LLC" -relocation-model=pic -filetype=obj "$LSO/$f.ll" -o "$LSO/$f.o" \
            || { echo "FAIL  llc $f"; exit 1; }
    done
    # scaly.o references the fiber context-switch primitives (vendored asm)
    # and the evented-I/O backend shim (kqueue/epoll C); ctime.o is the
    # civil-time shim every scalyc-family link carries (scaly/time/ctime.c).
    tools/fcontext.sh "$LSO/fcontext.o" || { echo "FAIL  fcontext assembly"; exit 1; }
    tools/eio.sh "$LSO/eio.o" || { echo "FAIL  eio shim compile"; exit 1; }
    tools/ctime.sh "$LSO/ctime.o" || { echo "FAIL  ctime shim compile"; exit 1; }
fi

# Build a scalyls CONSUMER program (json_test / echo): $1=src $2=out-binary.
lsp_build_prog() {
    if [ "$MODE" = selfhosted ]; then
        ( ulimit -s 65520; "$SCALYC" -S --no-tests -o "$LSO/prog.ll" "$1" ) || return 1
        "$LLC" -relocation-model=pic -filetype=obj "$LSO/prog.ll" -o "$LSO/prog.o" || return 1
        clang "$LSO/prog.o" "$LSO/scalyls.o" "$LSO/scalyc.o" "$LSO/scaly.o" "$LSO/fcontext.o" "$LSO/eio.o" "$LSO/ctime.o" "${LINK[@]}" -o "$2" 2>/dev/null
    else
        "$SCALYC" -o "$2" "$1" "${LINK[@]}" 2>/dev/null
    fi
}

# Build the scalyls SERVER (main.scaly): $1=out-binary.
lsp_build_server() {
    if [ "$MODE" = selfhosted ]; then
        clang "$LSO/scalyls_main.o" "$LSO/scalyls.o" "$LSO/scalyc.o" "$LSO/scaly.o" "$LSO/fcontext.o" "$LSO/eio.o" "$LSO/ctime.o" "${LINK[@]}" -o "$1" 2>/dev/null
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

frames = []
send({"jsonrpc":"2.0","id":1,"method":"initialize","params":{}})
frames.append(next_frame())
send({"jsonrpc":"2.0","method":"initialized","params":{}})
send({"jsonrpc":"2.0","method":"textDocument/didOpen","params":{"textDocument":{
        "uri":"file:///tmp/ok.scaly","languageId":"scaly","version":1,
        "text":"function answer() returns int\n{\n    return 42\n}\n"}}})
frames.append(next_frame())
send({"jsonrpc":"2.0","method":"textDocument/didChange","params":{
        "textDocument":{"uri":"file:///tmp/ok.scaly","version":2},
        "contentChanges":[{"text":"function f() returns int\n{\n    return nope()\n}\n"}]}})
frames.append(next_frame())
send({"jsonrpc":"2.0","method":"textDocument/didClose","params":{
        "textDocument":{"uri":"file:///tmp/ok.scaly"}}})
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
check(frames[3]["params"]["uri"] == "file:///tmp/ok.scaly", "didClose clears the right uri")
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
uri = "file:///tmp/lsp_budget_test.scaly"
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
path = "/tmp/lsp_symbols_test.scaly"
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
# everything the line says. Inside the BODY it stays `kind name`: there the
# semantic hover answers with a type when it can, and the enclosing declaration
# is all its fallback owes. Both halves are asserted (ids 2/3/7/8 header, 9 body).
python3 - <<'PY'
import sys, json, subprocess
src = ("function add(a: int, b: int) returns int\n{\n    return a + b\n}\n\n"
       "mutable counter: int 0\n\n"
       "define Point\n(\n    x: int\n)\n{\n"
       "    function get_x(this: Point) returns int\n    {\n        return x\n    }\n}\n"
       # Line 18: a GENERIC routine — nothing in the tree declares one, so this
       # fixture is the only cover for the `[T]` half of the rendering.
       "\nfunction pick[T](a: T, b: T) returns T\n{\n    return a\n}\n")
path = "/tmp/lsp_hover_test.scaly"
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
check(val(9) == "method get_x", "hover in a method BODY -> 'method get_x'")
check(val(10) == "function pick[T](a: T, b: T) returns T",
      "hover on a generic routine -> signature with its generics")
check(val(11) == "generic T", "hover on a generic parameter -> 'generic T'")
sys.exit(1 if failures else 0)
PY
rc=$?
if [ $rc -eq 0 ]; then ok "lsp hover"; else bad "lsp hover"; fi

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
path = "/tmp/lsp_def_test.scaly"
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
python3 - <<'PY'
import sys, json, subprocess
src = ("function add(a: int, b: int) returns int\n{\n    return a + b\n}\n\n"
       "function main() returns int\n{\n    return add(add(1, 2), 3)\n}\n")
path = "/tmp/lsp_ref_test.scaly"
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
path = "/tmp/lsp_scan_test.scaly"
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

# ---- documentHighlight: occurrences in the current document (no uri) ----
# Same lexical scan as references, but each result is a range-only
# DocumentHighlight (the editor knows the current document).
python3 - <<'PY'
import sys, json, subprocess
src = ("function add(a: int, b: int) returns int\n{\n    return a + b\n}\n\n"
       "function main() returns int\n{\n    return add(add(1, 2), 3)\n}\n")
path = "/tmp/lsp_hl_test.scaly"
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
path = "/tmp/lsp_completion_test.scaly"
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
uri = "file:///tmp/lsp_detail_test.scaly"
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
uri = "file:///tmp/lsp_callh_test.scaly"
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
uri = "file:///tmp/lsp_typedef_test.scaly"
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
uri = "file:///tmp/lsp_selrange_test.scaly"
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
path = "/tmp/lsp_incr_test.scaly"
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
path = "/tmp/lsp_docstore_test.scaly"
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
path = "/tmp/lsp_rename_test.scaly"
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
path = "/tmp/lsp_scope_test.scaly"
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
path = "/tmp/lsp_ctxcompl_test.scaly"
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
# two checks are the guards that matter: the repair must NOT reach diagnostics
# (the parse error IS that answer), and completion must still see the RAW line —
# the receiver being typed lives on the very line the repair blanks.
python3 - <<'PY'
import sys, json, subprocess, os, select, time
src = ("define Point\n(\n    x: int\n)\n{\n"
       "    function get_x(this: Point) returns int\n    {\n"
       "        set this.\n"                     # line 7: unfinished
       "        return x\n    }\n}\n")
path = "/tmp/lsp_midedit_test.scaly"
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
      "diagnostics still REPORT the parse error (not repaired away)")
sys.exit(1 if failures else 0)
PY
rc=$?
if [ $rc -eq 0 ]; then ok "lsp mid-edit parse repair"; else bad "lsp mid-edit parse repair"; fi

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
path = "/tmp/lsp_burst_test.scaly"
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
check(caps.get("inlayHintProvider") is True, "initialize advertises inlayHintProvider")
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

uri = "file:///tmp/sig.scaly"
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

uri = "file:///tmp/xpkg.scaly"
doc = ("function f()\n"               # 0
       "{\n"                          # 1
       "    var sb StringBuilder$()\n"# 2
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

# 1. cross-package (doc outside packages/, so the hit can only come from there)
r = define_at("file:///tmp/moddef1.scaly",
              "function f()\n{\n    var a Array[int]()\n}\n", 2, 12)
check(rel(r) == "packages/scaly/0.1.0/scaly/containers/Array.scaly",
      "definition: Array resolves to `define Array[T]`, not `module Array`")
r = define_at("file:///tmp/moddef2.scaly",
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
r = define_at("file:///tmp/moddef3.scaly",
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

probes = [
    (line_of("define Vector[T]"),        "Vector",  "Vector"),
    (line_of("length: size_t"),          "length",  "Vector"),
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
# containers/Vector.scaly line 69, `let own_page Page.get(this as pointer[void])`:
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
# Asserted EXACTLY: a substring check would pass `pointer` for `pointer[Page]`,
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

bind = line_of("let own_page Page.get(this as pointer[void])")
alloc = line_of("set data: own_page.allocate(len * sizeof T, alignof T) as pointer[T]")
cond = line_of("if len > 0")

# (line, token, occurrence index, expected type)
probes = [
    (bind,  "own_page", 0, "pointer[Page]"),       # untyped binding NAME
    (bind,  "Page",     0, "pointer[Page]"),       # the call it is bound to
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

uri = "file:///tmp/xpkgmem.scaly"
doc = ("function f()\n"               # 0
       "{\n"                          # 1
       "    var sb StringBuilder$()\n"# 2
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
path = "/tmp/lsp_fold_test.scaly"
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
path = "/tmp/lsp_sem_hover.scaly"
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

# ---- semantic hover fallback: parse-error input still answers (no crash) ----
# A broken document fails the pipeline; the server must still return a valid
# response (null) and stay alive to answer the next request.
python3 - <<'PY'
import sys, json, subprocess
bad = "/tmp/lsp_sem_bad.scaly"
open(bad, "w").write("function broken(a: int \n{\n    return a\n")  # missing ')'
good = "/tmp/lsp_sem_good.scaly"
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
path = "/tmp/lsp_sem_completion.scaly"
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

echo "-----"
echo "PASS: $pass  FAIL: $fail"
[ $fail -eq 0 ]
