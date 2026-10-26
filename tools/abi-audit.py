#!/usr/bin/env python3
"""Check every `extern` declaration in this tree against the real C prototype.

Nothing in the compiler verifies an extern declaration. A mismatch produces
wrong values at runtime and never a diagnostic, so this reads the actual headers
and compares WIDTH CLASSES parameter by parameter, plus the result.

The result is the dangerous half. A C function returning `int` leaves the upper
32 bits of the return register unspecified, so reading it as Scaly `int` (64
bits) can flip the SIGN. Proven with a three-line shim:

    int neg_int(void) { return -1; }

declared `returns int` answered 4294967295 (`> 0`); declared `returns i32`
answered -1. Every `if rc < 0` error check on such a function was a coin flip
decided by the callee's codegen. Parameters are the benign half — a 32-bit
callee reads the low half of a register the caller wrote in full — but they are
reported too, since "benign today" is not a property worth relying on.

What this tool does NOT silently skip: a prototype it cannot find, cannot parse,
or whose arity disagrees is COUNTED AND NAMED, never booked as clean. A checker
that skips quietly is the same failure class it exists to catch (this tool had
that bug: `LLVM_C_EXTERN_C_BEGIN int` parsed as no known type and 82 wrong
result types hid behind it).

Platform note that decides several cases: some libc typedefs differ per OS
(`mode_t` 16-bit on macOS vs 32 on glibc, `nfds_t` 32 vs 64). ONE seed serves
every target, so a width cannot be conditional — require the WIDER one, because
the caller then writes the whole register and a narrower callee reads the
correct low half.

WebAssembly (`--target wasm32`, 2026-10-19). The C side is ILP32 there
(`long`, `size_t`, `ssize_t`, `intptr_t`, `ptrdiff_t`, `clock_t` and `nfds_t`
are 32 bits; `off_t` and `time_t` stay 64) while Scaly's `int` and `size_t`
stay 64, and `c_size` is the type of the C side's pointer width. And there the
PARAMETERS are not benign: wasm-ld checks every signature, so a width that
differs in a parameter fails the link or traps -- in this mode a parameter
finding counts like a result finding. The wider-of-two rule for typedefs that
differ per libc does not apply either: the width is the one wasi-libc has.

Usage:
    tools/abi-audit.py [--target wasm32] [--headers <dir>]... [--sources <file.c>]... [--quiet] <file.scaly>...

`--sources` names one of our own C shims: its DEFINITIONS are the prototypes
of the externs that call into it (they have no header).

Exits 1 when there is at least one finding, so it can gate (tests/abi/run.sh).
"""
import re
import subprocess
import sys
from pathlib import Path

DEFAULT_HEADER_DIRS = [
    "/opt/homebrew/opt/llvm@21/include/llvm-c",
    "/usr/lib/llvm-21/include/llvm-c",
    "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include",
    "/usr/include",
]

C_WIDTH = {
    "void": 0,
    "int": 32, "unsigned": 32, "unsigned int": 32, "signed int": 32, "signed": 32,
    "long": 64, "unsigned long": 64, "long int": 64, "unsigned long int": 64,
    "long long": 64, "unsigned long long": 64, "long long int": 64,
    "short": 16, "unsigned short": 16, "short int": 16,
    "char": 8, "unsigned char": 8, "signed char": 8,
    "float": 32, "double": 64, "long double": 64,
    "size_t": 64, "ssize_t": 64, "off_t": 64, "time_t": 64, "clock_t": 64,
    "intptr_t": 64, "uintptr_t": 64, "ptrdiff_t": 64,
    "int8_t": 8, "uint8_t": 8, "int16_t": 16, "uint16_t": 16,
    "int32_t": 32, "uint32_t": 32, "int64_t": 64, "uint64_t": 64,
    "pid_t": 32, "uid_t": 32, "gid_t": 32, "socklen_t": 32, "useconds_t": 32,
    "sa_family_t": 8, "in_port_t": 16, "in_addr_t": 32, "wchar_t": 32,
    "id_t": 32, "key_t": 32, "dev_t": 32, "ino_t": 64, "nlink_t": 16,
    "blkcnt_t": 64, "blksize_t": 32, "rlim_t": 64, "suseconds_t": 32,
    "sig_atomic_t": 32, "mode_t": 16, "nfds_t": 32,
    # llvm-c
    "LLVMBool": 32, "LLVMOpcode": 32, "LLVMTypeKind": 32, "LLVMValueKind": 32,
    "LLVMIntPredicate": 32, "LLVMRealPredicate": 32, "LLVMLinkage": 32,
    "LLVMVisibility": 32, "LLVMCallConv": 32, "LLVMAtomicOrdering": 32,
    "LLVMAtomicRMWBinOp": 32, "LLVMDWARFTypeEncoding": 32,
    "LLVMCodeGenOptLevel": 32, "LLVMRelocMode": 32, "LLVMCodeModel": 32,
    "LLVMCodeGenFileType": 32, "LLVMVerifierFailureAction": 32,
    "LLVMDIFlags": 32, "LLVMDWARFSourceLanguage": 32, "LLVMDWARFEmissionKind": 32,
    "LLVMMetadataKind": 32, "LLVMInlineAsmDialect": 32, "LLVMThreadLocalMode": 32,
    "LLVMUnnamedAddr": 32, "LLVMModuleFlagBehavior": 32, "LLVMAttributeIndex": 32,
    "LLVMBinaryType": 32, "LLVMDWARFMacinfoRecordType": 32,
    "LLVMByteOrdering": 32, "LLVMGEPNoWrapFlags": 32,
}
# Typedefs whose width differs on glibc; the value is the width to REQUIRE.
PLATFORM_WIDER = {"mode_t": 32, "nfds_t": 64}

# The C side on wasm32 (wasi-libc's bits/alltypes.h): what differs from above.
WASM32_C = {
    "long": 32, "unsigned long": 32, "long int": 32, "unsigned long int": 32,
    "size_t": 32, "ssize_t": 32, "intptr_t": 32, "uintptr_t": 32, "ptrdiff_t": 32,
    "clock_t": 32, "nfds_t": 32, "mode_t": 32, "long double": 128,
}

S_WIDTH = {
    "void": 0, "c_size": 64,
    "int": 64, "uint": 64, "i64": 64, "u64": 64, "size_t": 64, "size": 64,
    "i32": 32, "u32": 32, "i16": 16, "u16": 16, "i8": 8, "u8": 8,
    "char": 8, "bool": 8, "double": 64, "f64": 64, "float": 32, "f32": 32,
}

DECL = re.compile(
    r"^\s*(?:function|procedure)\s+([A-Za-z_][A-Za-z0-9_]*)\s*\((.*?)\)"
    r"(?:\s+returns\s+(.+?))?\s+extern\s*$"
)
KEYWORDS = {"return", "if", "while", "for", "switch", "sizeof", "else", "typedef"}


def scaly_params(sig):
    out, depth, cur = [], 0, ""
    for ch in sig:
        if ch == "[":
            depth += 1
        elif ch == "]":
            depth -= 1
        if ch == "," and depth == 0:
            out.append(cur)
            cur = ""
        else:
            cur += ch
    if cur.strip():
        out.append(cur)
    res = []
    for p in out:
        if ":" in p:
            n, t = p.split(":", 1)
            res.append((n.strip(), t.strip()))
        else:
            res.append(("", p.strip()))
    return res


def clean_c(text):
    text = re.sub(r"/\*.*?\*/", " ", text, flags=re.S)
    text = re.sub(r"//[^\n]*", " ", text)
    # Preprocessor lines (with their continuations) are no part of a
    # prototype: `#endif` before `int access(...)` read as the return type
    # `endif int` (wasi-libc's headers are full of them).
    text = re.sub(r"\\\n", " ", text)
    text = re.sub(r"^[ \t]*#.*$", " ", text, flags=re.M)
    # These sit immediately before a declaration and would otherwise be swept
    # into the captured RETURN type, which then matches no known C type — and
    # an unparsed return used to be skipped silently.
    text = re.sub(r"\bLLVM_C_EXTERN_C_(BEGIN|END)\b", " ", text)
    # LLVM 21 puts its export macro in front of every C API prototype; read as
    # part of the result type it made 275 results "cannot be judged" -- the
    # result check, the dangerous half, was blind to the whole LLVM-C binding
    # (found 2026-10-26: LLVMGetBufferSize answered size_t, 64 bits in Scaly)
    text = re.sub(r"\bLLVM_C_ABI\b", " ", text)
    text = re.sub(r'extern\s*"C"', " ", text)
    text = re.sub(r"__(DARWIN_ALIAS|DARWIN_ALIAS_C|DARWIN_ALIAS_I|DARWIN_INODE64|"
                  r"DARWIN_1050|DARWIN_EXTSN|DARWIN_EXTSN_C)\s*\([^)]*\)", " ", text)
    text = re.sub(r"__attribute__\s*\(\(.*?\)\)", " ", text)
    text = re.sub(r"__(THROW|THROWNL|nonnull|wur|attribute_pure__|nothrow__)\b", " ", text)
    text = re.sub(r"\b(__restrict|restrict|volatile|_Nullable|_Nonnull|"
                  r"__unused|__deprecated|const|extern|_Noreturn)\b", " ", text)
    text = re.sub(r"__API_\w+\s*\([^)]*\)", " ", text)
    text = re.sub(r"__OSX_AVAILABLE\w*\s*\([^)]*\)", " ", text)
    return re.sub(r"\s+", " ", text)


PROTO = re.compile(
    r"([A-Za-z_][A-Za-z0-9_]*(?:\s+[A-Za-z_][A-Za-z0-9_]*)*\s*\**)\s*"
    r"\b([A-Za-z_][A-Za-z0-9_]*)\s*\(([^;{)]*)\)\s*;"
)
# A DEFINITION in one of our own C shims (`--sources`): the same shape,
# ended by its body instead of a semicolon.
PROTO_DEF = re.compile(
    r"([A-Za-z_][A-Za-z0-9_]*(?:\s+[A-Za-z_][A-Za-z0-9_]*)*\s*\**)\s*"
    r"\b([A-Za-z_][A-Za-z0-9_]*)\s*\(([^;{)]*)\)\s*\{"
)


def load_prototypes(dirs, want, sources=()):
    """{name: (ret_c, [param_c...], raw, parsed_ok)} — first prototype found."""
    out = {}
    files = []
    for d in dirs:
        if Path(d).is_dir():
            files += [(d, f, PROTO) for f in sorted(Path(d).rglob("*.h"))]
    for f in sources:
        files.append((str(Path(f).parent), Path(f), PROTO_DEF))
    for d, path, proto in files:
        if path:
            # C++ trees under a C include root are not C prototypes. On Linux
            # /usr/include holds libstdc++ (c++/15/bits/fs_ops.h declares
            # `void rename(const path&, const path&)`) and LLVM's C++ API
            # (llvm-21/llvm/ADT/APInt.h: `APInt trunc(unsigned)`), and both
            # sort before stdio.h — measured 2026-10-01 as one RESULT and one
            # PARAM finding against libc functions declared correctly.
            if any(p == "c++" or re.fullmatch(r"llvm(-\d+)?", p)
                   for p in path.relative_to(d).parts[:-1]):
                continue
            try:
                flat = clean_c(path.read_text(errors="replace"))
            except OSError:
                continue
            for m in proto.finditer(flat):
                name = m.group(2)
                if name not in want or name in out:
                    continue
                ret = m.group(1).strip()
                if not ret or ret.split()[0] in KEYWORDS:
                    continue
                args, ok = [], True
                for a in m.group(3).split(","):
                    a = a.strip()
                    if not a or a == "void":
                        continue
                    if a == "...":
                        args.append("...")
                        continue
                    if "*" in a or "[" in a or "(" in a:
                        args.append("ptr")
                        continue
                    # Every llvm-c handle typedef (`LLVMValueRef`, ...) is a
                    # pointer to an opaque struct. Naming them individually
                    # would be a maintenance list; the suffix is the contract.
                    if a.startswith("LLVM") and a.endswith("Ref"):
                        args.append("ptr")
                        continue
                    toks = a.split()
                    if len(toks) > 1 and re.fullmatch(r"[A-Za-z_][A-Za-z0-9_]*", toks[-1]) \
                            and " ".join(toks[:-1]) in C_WIDTH:
                        args.append(" ".join(toks[:-1]))
                    elif a in C_WIDTH:
                        args.append(a)
                    else:
                        ok = False
                        args.append(a)
                out[name] = (ret, args, " ".join(m.group(0).split())[:130], ok)
    return out


WASM32 = False


def width_c(t):
    if WASM32:
        return WASM32_C.get(t, C_WIDTH.get(t))
    return PLATFORM_WIDER.get(t, C_WIDTH.get(t))


def main():
    global WASM32
    args = sys.argv[1:]
    if "--target" in args:
        k = args.index("--target")
        if args[k + 1] != "wasm32":
            print(f"abi-audit: unknown target {args[k + 1]}")
            return 2
        WASM32 = True
        S_WIDTH["c_size"] = 32
        args = args[:k] + args[k + 2:]
    dirs = []
    sources = []
    while "--sources" in args:
        k = args.index("--sources")
        sources.append(args[k + 1])
        args = args[:k] + args[k + 2:]
    while "--headers" in args:
        k = args.index("--headers")
        dirs.append(args[k + 1])
        args = args[:k] + args[k + 2:]
    quiet = "--quiet" in args
    paths = [a for a in args if not a.startswith("--")]
    if not dirs:
        dirs = DEFAULT_HEADER_DIRS

    decls = {}
    for path in paths:
        for lineno, line in enumerate(open(path), 1):
            m = DECL.match(line.split(";")[0])
            if not m:
                continue
            decls.setdefault(m.group(1), []).append(
                (path, lineno, m.group(2), (m.group(3) or "void").strip()))

    protos = load_prototypes(dirs, set(decls), sources)
    findings, checked, unmatched, unparsed = [], 0, [], []
    unknown_params = []

    for name, entries in sorted(decls.items()):
        if name not in protos:
            unmatched.append(name)
            continue
        ret_c, args_c, raw, ok = protos[name]
        if "..." in args_c:
            unparsed.append((name, f"variadic: {raw}"))
            continue
        # EVERY declaration of the name, not just the first: a symbol declared
        # in three packages used to be width-checked once, so a mismatch in the
        # other two hid behind it. (The consistency check would catch the
        # divergence, but this tool must not depend on that to be complete.)
        issues = []
        arity_bad = False
        for path, lineno, sig, sret_i in entries:
            sp_i = scaly_params(sig)
            if len(sp_i) != len(args_c):
                unparsed.append((name, f"{path}:{lineno} arity scaly {len(sp_i)} vs C {len(args_c)}: {raw}"))
                arity_bad = True
                break
        if arity_bad:
            continue
        path, lineno, sig, sret = entries[0]
        sp = scaly_params(sig)
        base = ret_c.replace("*", "").strip()
        seen_issue = set()
        for path_i, lineno_i, sig_i, sret_i in entries:
            if "*" not in ret_c:
                cw, sw = width_c(base), S_WIDTH.get(sret_i)
                if cw is None:
                    unparsed.append((name, f"return type '{base}' cannot be judged: {raw}"))
                elif sw is None:
                    pass
                elif cw != sw:
                    msg = f"RESULT: C {base}={cw}b vs scaly {sret_i}={sw}b"
                    if msg not in seen_issue:
                        seen_issue.add(msg)
                        issues.append(msg)
                else:
                    checked += 1
            for (pn, st), ct in zip(scaly_params(sig_i), args_c):
                if ct == "ptr" or st.startswith("pointer") or st.startswith("ref"):
                    continue
                cw, sw = width_c(ct), S_WIDTH.get(st)
                if cw is None:
                    unknown_params.append((name, ct))
                    continue
                if sw is None:
                    continue
                if cw != sw:
                    msg = f"{pn or '?'}: C {ct}={cw}b vs scaly {st}={sw}b"
                    if msg not in seen_issue:
                        seen_issue.add(msg)
                        issues.append(msg)
                else:
                    checked += 1
        if issues:
            findings.append((name, entries, issues, raw))

    n_result = sum(1 for _, _, iss, _ in findings for i in iss if i.startswith("RESULT"))
    n_param = sum(1 for _, _, iss, _ in findings for i in iss if not i.startswith("RESULT"))
    print(f"positions checked (parameters + result): {checked}")
    print(f"FINDINGS: {len(findings)}")
    # Reported apart because they are not equally dangerous. A wrong RESULT
    # width can flip the sign of a returned value (the upper half of the return
    # register is unspecified) -- that is a bug, and it gates. A wrong PARAMETER
    # width is ABI-safe in the other direction: the caller writes the whole
    # register and a 32-bit callee reads exactly the low half it is entitled to.
    # Those are pinned by count in tests/abi/run.sh instead, so a NEW one still
    # gets caught.
    print(f"RESULT FINDINGS: {n_result}")
    print(f"PARAM FINDINGS: {n_param}")
    for name, entries, issues, raw in findings:
        print(f"\n### {name}\n    C: {raw}")
        for path, lineno, sig, sret in entries:
            print(f"    {path}:{lineno}  ({sig}) returns {sret}")
        for i in issues:
            print(f"    -> {i}")
    if not quiet:
        print(f"\nno prototype found in the headers ({len(unmatched)}) — "
              f"NOT booked as clean:")
        print("    " + " ".join(sorted(unmatched)))
        if unknown_params:
            u = {}
            for n, t in unknown_params:
                u[t] = u.get(t, 0) + 1
            print(f"\nparameter types without a known width ({len(unknown_params)} positions) — "
                  f"not booked: {dict(sorted(u.items(), key=lambda kv: -kv[1]))}")
        print(f"\nnot parsed safely ({len(unparsed)}) — not booked either:")
        for n, r in sorted(unparsed):
            print(f"    {n}: {r}")
    if WASM32:
        return 1 if n_result or n_param else 0
    return 1 if n_result else 0


if __name__ == "__main__":
    sys.exit(main())
