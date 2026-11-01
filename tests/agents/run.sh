#!/bin/bash
# tests/agents/run.sh [scaly] -- every program of docs/website/AGENTS.md runs.
#
# AGENTS.md teaches a coding assistant the language by example, so an example
# that no longer compiles teaches it wrong. Each ```scaly block is a whole
# program: it is run with `scaly run` in a scratch directory and must end with
# rc 0, or with the code a line `; exits N` in it names. Where an ```output
# block follows it, the program's standard output must be exactly that.
#   scaly   the tool to run them with (default: `scaly` on PATH -- the file
#           describes the published toolchain)
HERE="$(cd "$(dirname "$0")" && pwd)"
ROOT="$(cd "$HERE/../.." && pwd)"
SCALY="${1:-scaly}"
command -v "$SCALY" > /dev/null 2>&1 || { echo "agents: no scaly at '$SCALY'"; exit 1; }
TMP="$(mktemp -d)"
trap 'rm -rf "$TMP"' EXIT

python3 - "$ROOT/docs/website/AGENTS.md" "$TMP" <<'PY' || exit 1
import re, sys
text = open(sys.argv[1], encoding='utf-8').read()
blocks = re.findall(r'^```(\w+)\n(.*?)^```\n', text, re.M | re.S)
n = 0
for i, (kind, body) in enumerate(blocks):
    if kind != 'scaly':
        continue
    n += 1
    open(f'{sys.argv[2]}/b{n:02}.scaly', 'w').write(body)
    if i + 1 < len(blocks) and blocks[i + 1][0] == 'output':
        open(f'{sys.argv[2]}/b{n:02}.expected', 'w').write(blocks[i + 1][1])
if n == 0:
    sys.exit('agents: no scaly blocks found')
PY

pass=0; fail=0
cd "$TMP" || exit 1
for f in b*.scaly; do
  b="${f%.scaly}"
  want_rc=$(sed -n 's/^; exits \([0-9]*\).*/\1/p' "$f" | head -1)
  want_rc="${want_rc:-0}"
  "$SCALY" run "$f" > "$b.out" 2> "$b.err"
  rc=$?
  why=""
  [ "$rc" = "$want_rc" ] || why="rc $rc, want $want_rc"
  if [ -z "$why" ] && [ -f "$b.expected" ] && ! cmp -s "$b.out" "$b.expected"; then
    why="output differs"
  fi
  if [ -z "$why" ]; then
    pass=$((pass+1))
  else
    fail=$((fail+1))
    echo "FAIL $b ($(head -1 "$f")): $why"
    head -5 "$b.err" | sed 's/^/    /'
    [ -f "$b.expected" ] && diff "$b.expected" "$b.out" | head -8 | sed 's/^/    /'
  fi
done
echo "agents: $pass PASS, $fail FAIL"
[ "$fail" = 0 ]
