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
check(val == "function beta_renamed", "second ranged edit applied on top (sequential)")
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

echo "-----"
echo "PASS: $pass  FAIL: $fail"
[ $fail -eq 0 ]
