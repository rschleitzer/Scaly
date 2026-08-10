#!/bin/bash
# The Windows substrate completeness check — every symbol a Win64 link of the
# Scaly runtime must resolve, and whether anything in this tree provides it.
#
# It began (2026-08-09) as a work LIST: cross-emit the runtime for the Windows
# triple, classify the undefined symbols by who owes them, and watch the count
# fall. With all five brocken built it became the check it is now — the useful
# question stopped being "what is left" and became "is anything unprovided".
# Both readings come from the same measurement.
#
# Why an instrument rather than a paragraph: a number in a document goes stale
# in silence. Run it.
#
# What it measures, and the constraint that makes the number large: the runtime
# archive is ONE object with ONE .text section (no per-function COMDATs), so a
# COFF linker cannot discard anything — a program that touches the runtime AT
# ALL must resolve every symbol below, not just the ones on its own call path.
# That is the same "the archive is one object" mechanic as the -lm rule in
# CLAUDE.md, and it is why "link something that uses the runtime" is not a
# single rung: it forces brocken 1, 2 and 3 at once.
#
# ★SCOPE, and why it is wider than the runtime root (2026-08-10): it used to
# emit ONLY packages/scaly/0.1.0/scaly.scaly while claiming "every symbol has a
# provider" — and so it never saw `setenv` and `socketpair`, which are declared
# `extern` in TEST sources (tests/fiber/{taskpool_default,trace_balance,echo,
# echo_main}.scaly), not in packages/. An instrument whose scope is narrower
# than its claim is exactly the trap it exists to prevent. The undefined sets of
# the runtime root AND of every program the Windows corpus links are therefore
# UNIONED here; symbols the runtime object itself defines are subtracted, since
# for a program object those resolve at the archive.
#
# Usage: tools/win-undef.sh [compiler] [triple]
cd "$(dirname "$0")/.." || exit 1
SC=${1:-scalyc/build/scalyc}
TRIPLE=${2:-x86_64-pc-windows-msvc}
OBJ=/tmp/win_undef_$$.o

# shellcheck disable=SC1091
source tools/llvm-env.sh > /dev/null 2>&1
NM="${LLVM18:-/opt/homebrew/opt/llvm@18}/bin/llvm-nm"
command -v llvm-nm > /dev/null 2>&1 && NM=$(command -v llvm-nm)

"$SC" -c --target "$TRIPLE" --no-prelude --no-tests -o "$OBJ" \
      packages/scaly/0.1.0/scaly.scaly > /dev/null 2>&1 \
  || { echo "win-undef: FAIL — cross-emit for $TRIPLE failed"; exit 1; }

ALL=$("$NM" -u "$OBJ" | sed 's/^ *U //' | sort -u)

# What the runtime object DEFINES. A program's reference to Page.allocate is
# not owed to anyone — it is resolved by the archive at the program link, so it
# must not appear as missing when the test programs are folded in below.
RT_DEFINED=$("$NM" --defined-only --extern-only "$OBJ" \
             | sed 's/^[0-9a-f]* [A-Za-z] //' | sort -u)
rm -f "$OBJ"

# The programs the Windows corpus links — emitted BY the corpus script rather
# than by a second copy of its selection rule here. Two scripts deciding
# separately which tests are eligible is how a scope drifts apart again; asking
# the corpus makes "what win-undef checks" and "what CI links" the same set by
# construction. It also keeps `scalyc_test_exit.scaly` out for the right reason
# — the corpus skips it (its ground truth is a reference binary, not its own
# source), so its `_ZN6scalyc4testEv` is not a symbol any Windows link owes.
POUT=/tmp/win_undef_progs_$$
EMITLOG=$(tests/win32/corpus.sh emit "$SC" "$POUT" 2>&1)
nprog=$(printf '%s\n' "$EMITLOG" | sed -n 's/^corpus emit: \([0-9]*\) objects.*/\1/p')
nemitfail=$(printf '%s\n' "$EMITLOG" | sed -n 's/.*, \([0-9]*\) cross-emit failures.*/\1/p')
nprog=${nprog:-0}; nemitfail=${nemitfail:-0}
for o in "$POUT"/*.o; do
  [ -f "$o" ] || continue
  ALL="$ALL
$("$NM" -u "$o" | sed 's/^ *U //')"
done
rm -rf "$POUT"
ALL=$(printf '%s\n' "$ALL" | grep . | sort -u)
ALL=$(printf '%s\n' "$ALL" | while read -r s
      do printf '%s\n' "$RT_DEFINED" | grep -qx "$s" || echo "$s"; done)

# Present in the MSVC CRT under this exact name, or under an underscore alias
# the CRT also exports — no work beyond linking.
CRT='^(abort|atexit|exit|fclose|fopen|fread|free|fwrite|getenv|malloc|memcmp|memcpy|memset|puts|rewind|strcmp|strlen|strdup|write|access|mkdir|rmdir|unlink|expf|logf|powf|sqrtf|tanhf|_fltused|_tls_index)$'
HAVE=$(printf '%s\n' "$ALL" | grep -E "$CRT")

# What our own Windows sources DEFINE. Read by grep rather than by compiling
# them, because the host that runs this cannot: posixcompat.c and eio_win.c
# need the Windows SDK. That makes this an advisory answer, not a linker's —
# but it answers "is anything unprovided" NOW, before the archive plumbing
# exists, and a symbol missing here is missing either way.
WIN_C="packages/scaly/0.1.0/scaly/fiber/eio_win.c
packages/scaly/0.1.0/scaly/win32/posixcompat.c
packages/scaly/0.1.0/scaly/time/ctime.c"
WIN_S="packages/scaly/0.1.0/scaly/fiber/fcontext_x86_64_win.S"

# A definition is a non-indented line naming a function, whose body opens
# either on the SAME line or on the next non-blank one. Both shapes are
# required, not just Allman: eio_win.c writes its two tcp_listen wrappers as
# one-liners, and a detector that missed them reported them MISSING — a false
# alarm that would send the next reader implementing something that exists.
# Prototypes (ending in ';') are excluded, as are control-flow keywords, which
# is what keeps `if (...)` at column 0 out.
defs_of_c() {
  awk '
    /^[A-Za-z_][A-Za-z0-9_ \t*]*\(/ {
      if ($0 ~ /;[ \t]*$/) next
      if ($0 ~ /^(if|for|while|switch|return|typedef)\b/) next
      cand = $0
      ok = 0
      if ($0 ~ /\{/) ok = 1
      else if ($0 ~ /\)[ \t]*$/) {
        if ((getline nx) > 0) {
          while (nx ~ /^[ \t]*$/) { if ((getline nx) <= 0) break }
          if (nx ~ /^[ \t]*\{/) ok = 1
        }
      }
      if (ok) { sub(/\(.*/, "", cand); sub(/.*[ \t*]/, "", cand); print cand }
    }' "$1"
}

PROVIDED=$( { for f in $WIN_C; do [ -f "$f" ] && defs_of_c "$f"; done
              for f in $WIN_S; do [ -f "$f" ] && sed -n 's/^\.globl[ \t]*//p' "$f"; done
            } | sort -u )

# Emitted into every PROGRAM module by the compiler itself (Emitter.scaly's
# build-stamp function), so no shim owes it and it resolves at the program
# link. Listing it as missing would send the next reader hunting.
EMITTED='^(scaly_build_stamp)$'

OWED=$(printf '%s\n' "$ALL" | grep -vE "$CRT" | grep -vE "$EMITTED")
MISSING=$(printf '%s\n' "$OWED" | while read -r s
          do [ -n "$s" ] && { printf '%s\n' "$PROVIDED" | grep -qx "$s" || echo "$s"; }; done)
COVERED=$(printf '%s\n' "$OWED" | while read -r s
          do [ -n "$s" ] && { printf '%s\n' "$PROVIDED" | grep -qx "$s" && echo "$s"; }; done)

n() { printf '%s\n' "$1" | grep -c . ; }

echo "win-undef: $(n "$ALL") undefined symbols for $TRIPLE"
echo "  (runtime root + $nprog corpus programs; symbols the runtime defines are resolved by the archive)"
if [ "$nemitfail" -gt 0 ]; then
  echo "  WARNING — $nemitfail corpus program(s) did not cross-emit and were NOT scanned"
  printf '%s\n' "$EMITLOG" | sed 's/^/    /'
fi
echo
echo "  provided by our Windows sources:   $(n "$COVERED")"
echo "  provided by the MSVC CRT:          $(n "$HAVE")"
echo "  emitted per program (build stamp): 1"
echo
if [ -n "$MISSING" ]; then
  echo "  MISSING — nothing in this tree defines these:  $(n "$MISSING")"
  printf '%s\n' "$MISSING" | sed 's/^/       /'
  echo
  echo "win-undef: INCOMPLETE"
  exit 1
fi
echo "win-undef: every symbol has a provider"
echo "  (advisory — the definitions are read by grep, not by a linker;"
echo "   rung 3 of the roadmap is what proves it for real)"
