#!/usr/bin/env bash
# Build + smoke-test the scalyls LSP server and its json/rpc unit tests.
#
# The scalyls package depends on scalyc (the diagnostics pipeline), so the
# whole compiler — including the LLVM-backed Emitter — links into every
# scalyls consumer. Hence every link here passes -lLLVM-18.
#
# Usage: tests/lsp/run.sh [scalyc-binary]
set -euo pipefail

cd "$(dirname "$0")/../.."
SCALYC="${1:-./scalyc/build/scalyc}"

# Resolve the LLVM-18 lib dir (Homebrew / apt). The diagnostics pipeline
# pulls in the LLVM-backed Emitter, so every link below needs it.
LIBDIR=""
if command -v brew >/dev/null 2>&1; then
    LIBDIR="$(brew --prefix llvm@18 2>/dev/null)/lib"
fi
[ -d "$LIBDIR" ] || LIBDIR="/usr/lib/llvm-18/lib"
[ -d "$LIBDIR" ] || LIBDIR="/opt/homebrew/opt/llvm@18/lib"
LINK=(-L"$LIBDIR" -lLLVM-18)

pass=0
fail=0
ok()   { echo "PASS  $1"; pass=$((pass+1)); }
bad()  { echo "FAIL  $1"; fail=$((fail+1)); }

# ---- json + rpc unit test -------------------------------------------------
"$SCALYC" -o /tmp/scalyls_json_test tests/lsp/json_test.scaly "${LINK[@]}" 2>/dev/null
if [ "$(/tmp/scalyls_json_test)" = "PASS" ]; then ok "json_test"; else bad "json_test"; fi

# ---- transport echo round-trip -------------------------------------------
"$SCALYC" -o /tmp/scalyls_echo tests/lsp/echo.scaly "${LINK[@]}" 2>/dev/null
echoed=$(printf 'Content-Length: 5\r\n\r\nhello' | /tmp/scalyls_echo)
if [ "$echoed" = $'Content-Length: 5\r\n\r\nhello' ]; then ok "echo transport"; else bad "echo transport"; fi

# ---- LSP server: lifecycle + diagnostics ----------------------------------
"$SCALYC" -o /tmp/scalyls packages/scalyls/0.1.0/main.scaly "${LINK[@]}" 2>/dev/null

python3 - "$@" <<'PY'
import sys, json, subprocess
def frame(o):
    b = json.dumps(o).encode()
    return ("Content-Length: %d\r\n\r\n" % len(b)).encode() + b

inp  = frame({"jsonrpc":"2.0","id":1,"method":"initialize","params":{}})
inp += frame({"jsonrpc":"2.0","method":"initialized","params":{}})
inp += frame({"jsonrpc":"2.0","method":"textDocument/didOpen","params":{"textDocument":{
        "uri":"file:///tmp/ok.scaly","languageId":"scaly","version":1,
        "text":"function answer() returns int\n{\n    return 42\n}\n"}}})
inp += frame({"jsonrpc":"2.0","method":"textDocument/didChange","params":{
        "textDocument":{"uri":"file:///tmp/ok.scaly","version":2},
        "contentChanges":[{"text":"function f() returns int\n{\n    return nope()\n}\n"}]}})
inp += frame({"jsonrpc":"2.0","method":"textDocument/didClose","params":{
        "textDocument":{"uri":"file:///tmp/ok.scaly"}}})
inp += frame({"jsonrpc":"2.0","id":2,"method":"shutdown"})
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

check(len(frames) == 5, "frame count == 5 (init, didOpen diag, didChange diag, didClose clear, shutdown)")
check(frames[0].get("id") == 1 and "capabilities" in frames[0].get("result", {}), "initialize -> capabilities")
check(frames[0]["result"]["capabilities"].get("textDocumentSync") == 1, "textDocumentSync == 1 (full)")
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
python3 - <<'PY'
import sys, json, subprocess
src = ("function add(a: int, b: int) returns int\n{\n    return a + b\n}\n\n"
       "mutable counter: int 0\n\n"
       "define Point\n(\n    x: int\n)\n{\n"
       "    function get_x(this: Point) returns int\n    {\n        return x\n    }\n}\n")
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
inp += hov(2, 0, 10)     # inside `add`
inp += hov(3, 12, 18)    # inside Point.get_x (innermost)
inp += hov(4, 5, 10)     # inside `counter`
inp += hov(5, 100, 0)    # past EOF -> no symbol (declaration ranges run to
                         # the next token, so blank lines between decls still
                         # report the preceding one; past-EOF is the reliable
                         # null case)
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
check(val(2) == "function add", "hover on function -> 'function add'")
check(val(3) == "method get_x", "hover in method (innermost) -> 'method get_x'")
check(val(4) == "mutable counter", "hover on mutable -> 'mutable counter'")
check(val(5) is None, "hover off any symbol -> null result")
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

echo "-----"
echo "PASS: $pass  FAIL: $fail"
[ $fail -eq 0 ]
