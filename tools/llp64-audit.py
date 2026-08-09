#!/usr/bin/env python3
"""LLP64-Audit der C-Shims — kein bare `long` in einer exportierten Signatur.

Warum das ein eigener Prüfer ist und nicht eine Zeile in abi-audit.py:
abi-audit.py vergleicht Scaly-Deklarationen gegen ECHTE C-Header und kann
deshalb nur prüfen, was auf DIESEM Host deklariert ist. Die LLP64-Frage ist
eine andere: sie fragt, ob eine Signatur ihre Breite auf einem Target ändert,
das wir hier gar nicht übersetzen.

Der Mechanismus, in einem Satz: der committete Seed liefert EIN scaly.ll für
jedes Target, also kann eine Scaly-extern-Deklaration nicht target-abhängig
sein — sie sagt einmal `i64` bzw. `size_t`, für alle. C's `long` ist aber
64 Bit auf LP64 (mac/linux) und 32 Bit auf LLP64 (Win64). Eine exportierte
Shim-Funktion mit `long` widerspricht damit ihrer eigenen Deklaration auf
genau einem Target — und zwar lautlos.

Die RÜCKGABE ist die gefährliche Hälfte (dieselbe Klasse, für die
tests/abi/run.sh gebaut wurde): eine 32-Bit-Rückgabe, als i64 gelesen, lässt
die oberen 32 Bit unbestimmt, das kippt das VORZEICHEN, und jede
`if r < 0`-Fehlerprüfung darauf wird zum Münzwurf. Parameter sind die
gutartige Richtung, werden aber mitgemeldet: `unsigned long count` gegen ein
deklariertes `size_t` ist auf Win64 schlicht eine andere Funktion.

Geprüft werden NUR exportierte (nicht-`static`) Definitionen, denn nur die
sind die ABI-Grenze. Ein `long` in einem lokalen Zwischenwert ist in Ordnung
und teils sogar richtig (sysconf() gibt `long` zurück).

Behebung immer auf der C-SEITE: `long long` für Ergebnisse, `size_t` für
Größen/Zählungen — beide 64 Bit auf jedem Target, das wir ausliefern. Die
Scaly-Deklaration bleibt unangetastet, es gibt also keine Emissionsänderung
und keinen Seed-Refresh.

Usage: tools/llp64-audit.py [--quiet] [datei.c ...]
       ohne Dateien: alle .c unter packages/
"""

import os
import re
import sys

# `long long` zuerst neutralisieren, sonst schlägt \blong\b darauf an.
LONGLONG = re.compile(r"\blong\s+long\b")
BARE_LONG = re.compile(r"\blong\b")
BLOCK_COMMENT = re.compile(r"/\*.*?\*/", re.S)
LINE_COMMENT = re.compile(r"//[^\n]*")


def strip_comments(text):
    """Kommentare durch Leerzeilen ersetzen, damit die Zeilennummern stimmen."""
    text = BLOCK_COMMENT.sub(lambda m: "\n" * m.group(0).count("\n"), text)
    return LINE_COMMENT.sub("", text)


def has_bare_long(fragment):
    return bool(BARE_LONG.search(LONGLONG.sub("longlong", fragment)))


def find_definitions(lines):
    """Top-level-Funktionsdefinitionen im Allman-Stil der Shims.

    Eine Definition ist eine Zeile ohne Einrückung, die eine Parameterliste
    schliesst und deren naechste nicht-leere Zeile mit '{' beginnt. Damit
    fallen Prototypen (enden auf ';') und Funktionszeiger-Variablen heraus.
    """
    for i, line in enumerate(lines):
        if not line or line[0].isspace():
            continue
        stripped = line.strip()
        if "(" not in stripped or not stripped.endswith(")"):
            continue
        nxt = next((l.strip() for l in lines[i + 1:] if l.strip()), "")
        if not nxt.startswith("{"):
            continue
        yield i + 1, stripped


def split_signature(sig):
    """(Rueckgabetyp+Name, Parameterliste) — an der ERSTEN offenen Klammer."""
    depth = 0
    for i, ch in enumerate(sig):
        if ch == "(":
            if depth == 0:
                return sig[:i], sig[i + 1:-1]
            depth += 1
        elif ch == ")":
            depth -= 1
    return sig, ""


def audit(path):
    with open(path, "r", encoding="utf-8", errors="replace") as fh:
        lines = strip_comments(fh.read()).split("\n")

    findings = []
    for lineno, sig in find_definitions(lines):
        head, params = split_signature(sig)
        if re.match(r"^\s*static\b", head):
            continue
        where = []
        if has_bare_long(head):
            where.append("RUECKGABE")
        if has_bare_long(params):
            where.append("PARAMETER")
        if where:
            findings.append((path, lineno, "+".join(where), sig))
    return findings


def main():
    args = [a for a in sys.argv[1:] if a != "--quiet"]
    quiet = "--quiet" in sys.argv[1:]

    if args:
        files = args
    else:
        files = []
        for root, _, names in os.walk("packages"):
            files.extend(os.path.join(root, n) for n in sorted(names)
                         if n.endswith(".c"))
        files.sort()

    findings = []
    for path in files:
        findings.extend(audit(path))

    if not quiet:
        print(f"Geprüfte C-Shims: {len(files)}")
        for path in files:
            print(f"  {path}")
        print()

    for path, lineno, where, sig in findings:
        print(f"{path}:{lineno}: {where}: {sig}")

    print(f"LLP64-BEFUNDE: {len(findings)}")
    return 1 if findings else 0


if __name__ == "__main__":
    sys.exit(main())
