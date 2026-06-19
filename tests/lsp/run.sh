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
check(labels(2) == ["get_x"], "`Point.` -> only the struct's members")
check(kinds(2).get("get_x") == 2, "member kept its CompletionItemKind (Method=2)")
check(labels(3) == ["Circle","Square"], "`Shape.` -> only the union's variants")
check(kinds(3).get("Circle") == 20, "variant kept its CompletionItemKind (EnumMember=20)")
flat = ["add","counter","Point","get_x","Shape","Circle","Square","trigger"]
check(labels(4) == ["get_x"], "variable receiver `p.` (p: Point) -> Point's members")
check(labels(5) == flat, "no dot -> flat all-names list (unchanged)")
sys.exit(1 if failures else 0)
PY
rc=$?
if [ $rc -eq 0 ]; then ok "lsp context-aware completion"; else bad "lsp context-aware completion"; fi

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
members = ["area","grow","inspect"]
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
# member (px), proving real type resolution rather than the flat all-names list.
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
check(l2 == ["px"], "field-access chain r.origin. -> Point members only (not flat)")
l3 = labels(3)
check(l3 == ["px"], "call-result `let p make()` p. -> make()'s return type Point")
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
import sys, json, subprocess
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
path = "/tmp/lsp_sem_test.scaly"
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

echo "-----"
echo "PASS: $pass  FAIL: $fail"
[ $fail -eq 0 ]
