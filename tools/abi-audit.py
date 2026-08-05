#!/usr/bin/env python3
"""Audit the tree's extern C declarations against the real C prototypes.

The tree declares LLVM-C entry points with Scaly `int` parameters, and Scaly's
`int` is 64 bits. The C API declares those same parameters `unsigned` — 32 bits.
Nothing checked, so the call sites papered over it with `as i32` casts (244 of
them). This finds every such mismatch by reading the headers.

Only the width class is compared, because that is what the ABI turns on: a
32-bit C type against a 64-bit Scaly type is a finding; signedness differences
(unsigned vs i32) are reported separately and are ABI-compatible.

Usage: tools/abi-audit.py [--headers <dir>]... <scaly-file> [<scaly-file> ...]
"""
import re
import sys
from pathlib import Path

# Header roots to read prototypes from. Override with --headers <dir>
# (repeatable) so this runs on any platform; the default is where LLVM 18 lives
# on a Homebrew box.
HEADER_DIRS = [
    "/opt/homebrew/opt/llvm@18/include/llvm-c",
]

# C type -> width class. Only the types the LLVM-C API actually uses.
C_WIDTH = {
    "unsigned": 32, "unsigned int": 32, "int": 32, "uint32_t": 32,
    "int32_t": 32, "LLVMBool": 32, "LLVMOpcode": 32, "LLVMTypeKind": 32,
    "LLVMValueKind": 32, "LLVMIntPredicate": 32, "LLVMRealPredicate": 32,
    "LLVMLinkage": 32, "LLVMVisibility": 32, "LLVMCallConv": 32,
    "LLVMAtomicOrdering": 32, "LLVMAtomicRMWBinOp": 32, "LLVMDWARFTypeEncoding": 32,
    "LLVMCodeGenOptLevel": 32, "LLVMRelocMode": 32, "LLVMCodeModel": 32,
    "LLVMCodeGenFileType": 32, "LLVMVerifierFailureAction": 32,
    "LLVMDIFlags": 32, "LLVMDWARFSourceLanguage": 32, "LLVMDWARFEmissionKind": 32,
    "LLVMMetadataKind": 32, "LLVMInlineAsmDialect": 32, "LLVMThreadLocalMode": 32,
    "LLVMUnnamedAddr": 32, "LLVMModuleFlagBehavior": 32,
    "unsigned long long": 64, "long long": 64, "uint64_t": 64, "int64_t": 64,
    "size_t": 64, "unsigned long": 64, "long": 64, "double": 64, "float": 32,
    "LLVMBinaryType": 32, "LLVMDWARFMacinfoRecordType": 32,
    "LLVMByteOrdering": 32, "LLVMGEPNoWrapFlags": 32,
    "LLVMAttributeIndex": 32, "uint8_t": 8, "char": 8, "int8_t": 8,
    "uint16_t": 16, "int16_t": 16,
}

# Scaly type -> width class.
S_WIDTH = {
    "int": 64, "uint": 64, "i64": 64, "u64": 64, "size_t": 64, "size": 64,
    "i32": 32, "u32": 32, "i16": 16, "u16": 16, "i8": 8, "u8": 8, "char": 8,
    "bool": 8, "double": 64, "f64": 64, "float": 32, "f32": 32,
}

DECL = re.compile(
    r"^\s*(?:function|procedure)\s+([A-Za-z_][A-Za-z0-9_]*)\s*\((.*?)\)"
    r"(?:\s+returns\s+([A-Za-z_][A-Za-z0-9_]*(?:\[[^\]]*\])?))?\s+extern\s*$"
)


def scaly_params(sig):
    """[(name, type)] for a Scaly parameter list, bracket-aware."""
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


def load_prototypes():
    """{name: [c_param_type, ...]} from the LLVM-C headers."""
    text = ""
    for d in HEADER_DIRS:
        for h in sorted(Path(d).rglob("*.h")):
            text += h.read_text(errors="replace") + "\n"
    # Collapse to one logical line per declaration.
    text = re.sub(r"/\*.*?\*/", " ", text, flags=re.S)
    text = re.sub(r"//[^\n]*", " ", text)
    flat = re.sub(r"\s+", " ", text)
    protos = {}
    for m in re.finditer(r"([A-Za-z_][A-Za-z0-9_ *]*?)\b(LLVM[A-Za-z0-9_]*)\s*\(([^;{)]*)\)\s*;", flat):
        name, args = m.group(2), m.group(3)
        if name in protos:
            continue
        params = []
        for a in args.split(","):
            a = a.strip()
            if not a or a == "void":
                continue
            # Drop the parameter name and any array/pointer decoration.
            if "*" in a:
                params.append("ptr")
                continue
            a = re.sub(r"\[\s*\]\s*$", "", a).strip()
            toks = a.split()
            if len(toks) > 1 and re.fullmatch(r"[A-Za-z_][A-Za-z0-9_]*", toks[-1]):
                cand = " ".join(toks[:-1])
            else:
                cand = a
            params.append(cand.replace("const ", "").strip())
        protos[name] = params
    return protos


def main():
    args = sys.argv[1:]
    dirs = []
    while "--headers" in args:
        k = args.index("--headers")
        dirs.append(args[k + 1])
        args = args[:k] + args[k + 2:]
    if dirs:
        HEADER_DIRS[:] = dirs
    protos = load_prototypes()
    findings, ok, unknown = [], 0, []
    for path in args:
        for lineno, line in enumerate(open(path), 1):
            m = DECL.match(line.split(";")[0])
            if not m:
                continue
            name, sig = m.group(1), m.group(2)
            if name not in protos:
                continue
            sp, cp = scaly_params(sig), protos[name]
            if len(sp) != len(cp):
                findings.append((path, lineno, name, "arity", f"scaly {len(sp)} vs C {len(cp)}"))
                continue
            for i, ((pn, st), ct) in enumerate(zip(sp, cp)):
                if ct == "ptr" or st.startswith("pointer") or st.startswith("ref"):
                    continue
                cw, sw = C_WIDTH.get(ct), S_WIDTH.get(st)
                if cw is None:
                    unknown.append((name, ct))
                    continue
                if sw is None:
                    continue
                if cw != sw:
                    findings.append((path, lineno, name, f"param {i} ({pn})",
                                     f"C {ct}={cw}b vs scaly {st}={sw}b"))
                else:
                    ok += 1
    print(f"geprüfte Parameter mit passender Breite: {ok}")
    print(f"BEFUNDE: {len(findings)}\n")
    bad_fns = {}
    for path, lineno, name, where, detail in findings:
        bad_fns.setdefault((path, lineno, name), []).append(f"{where}: {detail}")
    for (path, lineno, name), items in sorted(bad_fns.items(), key=lambda kv: kv[0][2]):
        print(f"{path}:{lineno} {name}")
        for it in items:
            print(f"    {it}")
    if unknown:
        u = {}
        for n, t in unknown:
            u[t] = u.get(t, 0) + 1
        print("\nunbekannte C-Typen (nicht bewertet):", dict(sorted(u.items(), key=lambda kv: -kv[1])))


if __name__ == "__main__":
    main()
