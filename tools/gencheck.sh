#!/bin/bash
# Prueft, ob jede GENERIERTE Datei noch das ist, was ihr Generator erzeugt.
#
# ★★★DIE KLASSE, DIE DIESES GATE FAENGT: eine Konversion wird in der
# GENERIERTEN DATEI gemacht und nicht im Generator.  Bis zur naechsten
# Regeneration faellt das nicht auf, dann dreht sie still zurueck.  Zweimal
# gefunden am 2026-09-04:
#   * tscaly/Keywords.scaly  -- kw_eq auf Slice[char] konvertiert, Generator
#     erzeugte weiter (pointer[const_char], int): 88 Stellen.
#   * dazzle/CharNames.scaly + Sdata.scaly -- pointer[Interpreter] -> ref und
#     der Literal-Cast-Sweep (` as u32`): 1830 Diff-Zeilen.
#
# ★Die openjade/opensp-Quellen fehlen auf dem Entwicklungsrechner, also kann
# chartablegen nicht gegen die echte Tabelle laufen.  Die Quelle wird deshalb
# AUS DER AUSGABE REKONSTRUIERT -- das prueft weiterhin alles, was nicht die
# Eintragsdaten selbst ist (Signaturen, Rahmen, Zahlenformat), und genau dort
# sassen beide Funde.
set -u
cd "$(dirname "$0")/.."
TMP=$(mktemp -d); trap 'rm -rf "$TMP"' EXIT
fail=0

# --- 1. (entfallen 2026-10-05) ---------------------------------------------
# Die Generatoren, die ihre Zieldatei direkt schreiben, waren die von tscaly
# (packages/tscaly/tools/gen*.py). tscaly ist ein eigenes Repository
# (github.com/rschleitzer/tscaly); der Lauf dort: jeden Generator starten und
# die erzeugten Dateien vorher und nachher per Prüfsumme vergleichen.

# --- 2. chartablegen: Quelle aus der Ausgabe rekonstruieren ---------------
python3 - "$TMP" <<'PY'
import re, sys
tmp = sys.argv[1]
# Der Dateiname der Quelle landet IM KOPF der Ausgabe, also muss die
# rekonstruierte Datei genau so heissen wie die echte Referenztabelle.
for concept, call, src in (('CharNames', 'def_char', 'charNames.h'),
                           ('Sdata', 'def_sdata_char', 'sdata.h')):
    f = f'packages/dazzle/0.1.0/dazzle/{concept}.scaly'
    pat = re.compile(rf'interp\.{call}\("([^"]+)",\s*0x([0-9A-Fa-f]+)\)')
    rows = [(m.group(2), m.group(1)) for m in pat.finditer(open(f, encoding='utf8').read())]
    with open(f'{tmp}/{src}', 'w') as o:
        for code, name in rows:
            o.write('  { 0x%s, "%s" },\n' % (code, name))
PY
for pair in "CharNames:charNames.h" "Sdata:sdata.h"; do
    c=${pair%%:*}; src=${pair#*:}
    python3 tools/chartablegen.py "$c" "$TMP/$src" > "$TMP/$c.scaly" 2>/dev/null
    if ! cmp -s "packages/dazzle/0.1.0/dazzle/$c.scaly" "$TMP/$c.scaly"; then
        echo "DIVERGENT  chartablegen.py -> $c.scaly"
        diff "packages/dazzle/0.1.0/dazzle/$c.scaly" "$TMP/$c.scaly" | head -6 | sed 's/^/           /'
        fail=1
    fi
done

[ $fail = 0 ] && echo "gencheck: OK — jede geprüfte generierte Datei ist Generatorausgabe"
exit $fail
