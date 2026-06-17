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

check(len(frames) == 4, "frame count == 4 (init, didOpen diag, didChange diag, shutdown)")
check(frames[0].get("id") == 1 and "capabilities" in frames[0].get("result", {}), "initialize -> capabilities")
check(frames[0]["result"]["capabilities"].get("textDocumentSync") == 1, "textDocumentSync == 1 (full)")
check(frames[1].get("method") == "textDocument/publishDiagnostics", "didOpen -> publishDiagnostics")
check(frames[1]["params"]["diagnostics"] == [], "valid doc -> no diagnostics")
diags = frames[2]["params"]["diagnostics"]
check(len(diags) == 1, "didChange (bad call) -> 1 diagnostic")
check("nope" in diags[0]["message"], "diagnostic message names the missing fn")
check(diags[0]["range"]["start"]["line"] == 2, "diagnostic on 0-based line 2")
check(frames[3].get("id") == 2 and frames[3].get("result") is None, "shutdown -> null result")
sys.exit(1 if failures else 0)
PY
rc=$?
if [ $rc -eq 0 ]; then ok "lsp server lifecycle+diagnostics"; else bad "lsp server lifecycle+diagnostics"; fi

echo "-----"
echo "PASS: $pass  FAIL: $fail"
[ $fail -eq 0 ]
