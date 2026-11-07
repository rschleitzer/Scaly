# The compiler as C++

Scaly's compiler is written in Scaly and is built from the seed: the compiler
compiled to LLVM IR, committed in `seed/`. Whoever does not trust that IR can
build a compiler from the C++ in this directory instead, and let it emit the
seed again. If the result is the committed seed byte for byte, the seed is
what the sources say, whatever the IR might have hidden: this is David A.
Wheeler's *diverse double-compiling*, with a C++ compiler of your choice as
the second, independent compiler.

```
trust/check.sh                 # build from the C++, emit the seed, compare
CXX=g++ CC=gcc trust/check.sh  # the same without any of LLVM's toolchain
trust/check-gcc.sh             # that, in an AlmaLinux 9 container without clang
trust/build.sh [out-dir]       # only the build: <out-dir>/scalyc
```

`check.sh` prints `OK` when the compiler built from the C++ emits all seven
files of `seed/` exactly as they are committed. The release workflow runs
`check-gcc.sh` first and makes no release of a version that does not pass it.

## What is here

| file | what it is |
|---|---|
| `libscaly.cpp` | the standard library (`packages/scaly`) |
| `scalyc.cpp` | the compiler (`packages/scalyc`) |
| `main.cpp` | the compiler's entry |
| `scaly_types.h` | the standard library's records, included by the other two |
| `emit.sh` | writes the four files above from the sources |
| `build.sh`, `check.sh` | build the compiler from them; compare its output with `seed/` |
| `check-gcc.sh` | `check.sh` with GCC in a container, as the release runs it |

The C++ is written by the compiler's second back end, `scalyc --emit-cpp`
(`packages/scalyc/<version>/scalyc/compiler/CppEmitter.scaly`). It translates
the same plan the LLVM back end reads, after every decision the compiler makes
about types, overloads, integer widths and where a value lives; the C++ shows
those decisions written out instead of leaving them to the C++ compiler:

- one function per routine, under a name read off its symbol
  (`Planner_lookup_method`), with the symbol as its link name
  (`asm("...")`); a call names its callee, so no C++ overloading is involved;
- a generic routine is a template where all its instances agree on the text
  (`template <class T> static T* Vector_get(Vector<T>&, uint64_t)`), and an
  instance that does not is written out on its own. An instance counts as the
  template's only if the template, with its arguments put back in as text, is
  that instance's own text character for character;
- no headers: the integer types are the C++ compiler's own
  (`__INT64_TYPE__`), and the C library is declared where it is called.

These files are generated and are never edited by hand. They are written again
whenever the seed is, from the same tree, so the two always stand for the same
sources.

## What the check proves, and what stays trusted

Proven: the compiler built from this C++ by your C++ compiler produces the
committed seed. A seed that carried anything the sources do not say would
have to be produced by the C++ too, where it can be read.

Still trusted:

- **your C++ compiler**, which is the point: pick one that has nothing to do
  with Scaly. The check passes with Apple clang and with GCC 11.5 (AlmaLinux 9).
- **libLLVM 21**, which the compiler calls to write the IR. Both builds use
  it, so both would agree on anything it might add.
- **the C and assembly files of the runtime** (under
  `packages/scaly/<version>/scaly/`: `fiber/eio.c`, `fiber/fcontext_*.S`,
  `time/ctime.c`, `memory/panic.c`) and the compiler's
  `compiler/wasm_eh.cpp`, which both builds compile as they are.
- **this C++ itself**: of the order of the Scaly sources of the compiler and
  the standard library in length, readable, and the thing to read.

The C++ is built with `-fwrapv -fno-strict-aliasing -ffp-contract=off`:
Scaly's integer arithmetic wraps, a record may be read as another of the same
layout, and floating-point expressions are evaluated as written.
