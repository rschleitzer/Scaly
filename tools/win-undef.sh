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
# That is the same "the archive is one object" mechanic as the -lm rule,
# and it is why "link something that uses the runtime" is not a
# single rung: it forces brocken 1, 2 and 3 at once.
#
# ★SCOPE, and why it is wider than the runtime root (2026-08-10): it used to
# emit ONLY packages/scaly/0.1.1/scaly.scaly while claiming "every symbol has a
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
NM="${LLVM21:-/opt/homebrew/opt/llvm@21}/bin/llvm-nm"
command -v llvm-nm > /dev/null 2>&1 && NM=$(command -v llvm-nm)

"$SC" -c --target "$TRIPLE" --no-prelude --no-tests -o "$OBJ" \
      packages/scaly/0.1.1/scaly.scaly > /dev/null 2>&1 \
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

# ★And the programs the Windows job links from OUTSIDE the corpus (2026-08-10,
# rung 6). tests/fiber/bench/ is deliberately out of tests/fiber/run.sh's glob —
# and therefore out of the corpus's — because the 10k demo is one long program
# under load rather than a corpus entry; but CI links it just the same, so its
# externs are owed exactly like any other program's. Widened by DIRECTORY rather
# than by naming http10k: a list would go stale the day a second bench program
# lands, and a scope that silently stops short is the one failure this
# instrument exists to prevent. A cross-emit failure here is reported rather
# than skipped, for the same reason.
nbench=0
for f in tests/fiber/bench/*.scaly; do
  [ -f "$f" ] || continue
  b=$(basename "$f" .scaly)
  if "$SC" -c --target "$TRIPLE" -o "$POUT/bench__$b.o" "$f" > /dev/null 2>&1; then
    nbench=$((nbench+1))
    ALL="$ALL
$("$NM" -u "$POUT/bench__$b.o" | sed 's/^ *U //')"
  else
    nemitfail=$((nemitfail+1))
    EMITLOG="$EMITLOG
bench emit: cross-emit failure fiber/bench/$b"
  fi
done

# ★And the PRODUCTS (2026-08-10 rung 7, widened 2026-08-11 rung 9). The corpus
# and the bench directory are both TEST programs; these three are the binaries
# the stage exists for, and each owes symbols the test programs do not — `creat`
# was the onsgmls one, `dlopen`/`dlsym` the compiler's. A package object is a
# LIBRARY (--no-prelude), so what it defines is subtracted the same way the
# runtime's definitions are: at the link the objects resolve each other, and
# only what NEITHER provides is owed.
#
# ★The compiler is the entry the FOURTH widening added, and it is the one the
# instrument had the least excuse to miss: `scalyc` is the reason the other four
# targets have a seed at all, and stage 7's exit criterion is that seed
# reproducing HERE. Its flags mirror `tools/seed.sh` exactly (--no-tests, no
# --no-prelude) so that what this scans is what that script emits.
# ★dazzle joined in the same pass. Rung 8 linked it on Windows while nothing
# checked its symbol debt, which is how its 61 LLVM-C symbols had to be counted
# by hand; the LLVM class below is the answer to that.
#
# One row per binary: name, library root + its flags, program root + its flags.
# A row whose library field is empty has no package object of its own.
PRODUCTS=(
  "scalyc|packages/scalyc/0.1.0/scalyc.scaly|--no-tests|packages/scalyc/0.1.0/main.scaly|--no-tests"
)
ndropin=0
for row in "${PRODUCTS[@]}"; do
  IFS='|' read -r pname lib libflags prog progflags <<< "$row"
  ok=1
  if [ -n "$lib" ]; then
    # shellcheck disable=SC2086
    "$SC" -c --target "$TRIPLE" $libflags -o "$POUT/p_$pname.o" "$lib" > /dev/null 2>&1 || ok=0
  fi
  # shellcheck disable=SC2086
  [ "$ok" = 1 ] && { "$SC" -c --target "$TRIPLE" $progflags -o "$POUT/pm_$pname.o" "$prog" > /dev/null 2>&1 || ok=0; }
  if [ "$ok" = 1 ]; then
    ndropin=$((ndropin+1))
    # Only the LIBRARY object's definitions are subtracted, never the program's.
    # A program defines `main` and its own build stamp and nothing anyone else
    # links against, so folding those in could only ever mask a symbol some
    # OTHER program owes — and masking is the one direction this instrument
    # must not err in.
    if [ -f "$POUT/p_$pname.o" ]; then
      RT_DEFINED=$(printf '%s\n%s\n' "$RT_DEFINED" \
        "$("$NM" --defined-only --extern-only "$POUT/p_$pname.o" \
           | sed 's/^[0-9a-f]* [A-Za-z] //')" | sort -u)
    fi
    for o in "$POUT/p_$pname.o" "$POUT/pm_$pname.o"; do
      [ -f "$o" ] || continue
      ALL="$ALL
$("$NM" -u "$o" | sed 's/^ *U //')"
    done
  else
    nemitfail=$((nemitfail+1))
    EMITLOG="$EMITLOG
product emit: cross-emit failure $pname"
  fi
done
rm -rf "$POUT"
ALL=$(printf '%s\n' "$ALL" | grep . | sort -u)
ALL=$(printf '%s\n' "$ALL" | while read -r s
      do printf '%s\n' "$RT_DEFINED" | grep -qx "$s" || echo "$s"; done)

# Present in the MSVC CRT under this exact name, or under an underscore alias
# the CRT also exports — no work beyond linking.
# ★Several of these are POSIX SPELLINGS that MSVC ships only in `oldnames.lib`,
# which maps them to the underscore forms (write/access/mkdir/rmdir/unlink). It
# reaches the link through the `/defaultlib:oldnames.lib` directive the CRT
# headers put in our C shim objects — the Scaly-emitted objects carry no such
# directive — and rung 5's 90 green programs are the standing proof that it does.
# ★`creat` is deliberately NOT in this list even though the CRT has it: reaching
# `_creat` through oldnames ENDS THE PROCESS on a POSIX 0666 mode, so
# win32/posixcompat_windows.c defines `creat` itself and it must be attributed there.
# A name being present in the CRT is not the same as it being usable.
# ★`__chkstk` is not a library call anyone wrote: the compiler emits it to probe
# a stack frame larger than a page, and the CRT defines it.
# ★★★`setjmp` arrived with the panic catch point (2026-09-08) and it is the one
# entry on this list whose PRESENCE is not the whole question: the CRT declares
# and provides it, so it belongs here rather than in MISSING -- but whether a
# catch point WORKS on Windows is a different matter. It shows up here because
# a program with a `try` emits the call directly from the Emitter, not through
# a shim.
# ★★★`_setjmp` is the COFF spelling and it is what a `try` emits SINCE
# 2026-09-20: on Win64 the CRT's catch point takes the frame as
# a second argument, which is what made every catching program die, so the
# Emitter emits `i32 @_setjmp(ptr, ptr)` for COFF and plain `setjmp` elsewhere.
# **This list went INCOMPLETE on the very commit that FIXED the catch** -- the
# advisory named a symbol the CRT provides under a name it did not know. Both
# spellings stay listed: `setjmp` is still what a non-COFF emission names, and
# a list that tracks only today's spelling reports a repair as a regression.
# ★The second block arrived with dazzle and scalyc in scope (2026-08-11): the
# DOUBLE-precision math the DSSSL numeric primitives call (the `f` suffixed ones
# above are the tensor kernels'), plus `calloc` (dazzle/FrameMark.scaly's state
# block), `raise`, and the four the compiler itself adds — `atoll`, `strtod`,
# `system`, `memmove`. Nothing here needs a shim; they are listed because a
# provider class with no entry reads as an unprovided symbol.
CRT='^(abort|atexit|atoll|exit|fclose|fopen|fread|free|fwrite|getenv|malloc|memcmp|memcpy|memmove|memset|puts|rewind|strcmp|strerror|strlen|strdup|strtod|system|write|access|mkdir|rmdir|unlink|expf|logf|powf|sqrtf|tanhf|_fltused|_tls_index|__chkstk|setjmp|_setjmp)$|^(acos|asin|atan|atan2|calloc|ceil|cos|exp|floor|log|log10|pow|raise|sin|sqrt|tan)$'
HAVE=$(printf '%s\n' "$ALL" | grep -E "$CRT")

# ★A third provider class, added with the compiler (2026-08-11): the LLVM-C API.
# It is neither a shim nor the CRT but a LIBRARY the link must be given — on
# Windows `LLVM-C.lib`/`LLVM-C.dll` from an LLVM **20** install, and the
# version is not free: libLLVM is what prints the IR, so it has to match the
# major the seed was minted with — unlike `llc`, whose version may differ.
# Two binaries owe it, and only one of them obviously: scalyc calls it, and the
# dazzle package carries the JIT (`dazzle/Jit.scaly`) so its object owes these
# even though `--jit` is opt-in and nothing on this platform switches it on —
# the archive is ONE object with ONE .text, so a COFF linker can discard
# nothing. That fact was measured by hand at rung 8; here it is a column.
LLVMLIB='^LLVM'
HAVE_LLVM=$(printf '%s\n' "$ALL" | grep -E "$LLVMLIB")

# What our own Windows sources DEFINE. Read by grep rather than by compiling
# them, because the host that runs this cannot: posixcompat_windows.c and eio_windows.c
# need the Windows SDK. That makes this an advisory answer, not a linker's —
# but it answers "is anything unprovided" NOW, before the archive plumbing
# exists, and a symbol missing here is missing either way.
# ★panic.c stands here although it is NOT Windows-specific: this list is "what
# our own C sources define", and the panic shim is one of them (it is on every
# link line, tools/panic.sh). It was missing when the shim landed, so the six
# scaly_catch_*/scaly_panic_* symbols were reported MISSING and this tool said
# INCOMPLETE — an advisory that is red for a known reason hides the next symbol
# that is missing for a real one.
WIN_C="packages/scaly/0.1.1/scaly/fiber/eio_windows.c
packages/scaly/0.1.1/scaly/win32/posixcompat_windows.c
packages/scaly/0.1.1/scaly/time/ctime.c
packages/scaly/0.1.1/scaly/memory/panic.c"
WIN_S="packages/scaly/0.1.1/scaly/fiber/fcontext_x86_64_windows.S"

# A definition is a non-indented line naming a function, whose body opens
# either on the SAME line or on the next non-blank one. Both shapes are
# required, not just Allman: eio_windows.c writes its two tcp_listen wrappers as
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
      else {
        # Third shape: a PARAMETER LIST CONTINUED over lines. panic.c writes
        # scaly_catch_run that way, and without this it was reported MISSING
        # -- the same false alarm the one-liner shape above exists for.
        while ((getline nx) > 0) {
          if (++cont > 8) break
          if (nx ~ /;[ \t]*$/) break                 # a multi-line PROTOTYPE
          if (nx ~ /\{/) { ok = 1; break }
          if (nx ~ /\)[ \t]*$/) {
            if ((getline nx2) > 0) {
              while (nx2 ~ /^[ \t]*$/) { if ((getline nx2) <= 0) break }
              if (nx2 ~ /^[ \t]*\{/) ok = 1
            }
            break
          }
        }
        cont = 0
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

OWED=$(printf '%s\n' "$ALL" | grep -vE "$CRT" | grep -vE "$LLVMLIB" | grep -vE "$EMITTED")
MISSING=$(printf '%s\n' "$OWED" | while read -r s
          do [ -n "$s" ] && { printf '%s\n' "$PROVIDED" | grep -qx "$s" || echo "$s"; }; done)
COVERED=$(printf '%s\n' "$OWED" | while read -r s
          do [ -n "$s" ] && { printf '%s\n' "$PROVIDED" | grep -qx "$s" && echo "$s"; }; done)

n() { printf '%s\n' "$1" | grep -c . ; }

echo "win-undef: $(n "$ALL") undefined symbols for $TRIPLE"
echo "  (runtime root + $nprog corpus programs + $nbench bench programs"
echo "   + $ndropin products of ${#PRODUCTS[@]} (scalyc);"
echo "   symbols a package object defines are resolved at the link)"
if [ "$nemitfail" -gt 0 ]; then
  echo "  WARNING — $nemitfail corpus program(s) did not cross-emit and were NOT scanned"
  printf '%s\n' "$EMITLOG" | sed 's/^/    /'
fi
echo
echo "  provided by our Windows sources:   $(n "$COVERED")"
echo "  provided by the MSVC CRT:          $(n "$HAVE")"
echo "  provided by libLLVM 21 (LLVM-C):   $(n "$HAVE_LLVM")"
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
