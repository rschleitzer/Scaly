#!/bin/bash
# Assemble the full public-domain Thomas Mann training corpus from
# Project Gutenberg (plain-text UTF-8 editions; Mann's works are
# public domain in the EU since 2026-01-01, and these pre-1930
# publications are US public domain as well). Strips the PG
# header/footer boilerplate from each book and concatenates them to
# demo/build/mann_full.txt (~7 MB). The result is NOT committed —
# rerun this script to rebuild it.
#
# Usage: demo/fetch_corpus.sh
set -e
cd "$(dirname "$0")/.."
mkdir -p demo/build

# id: gutenberg.org ebook number
BOOKS="65661 65662 34811 35328 36766 23313 12108 12053 13810"
# 65661/65662 Der Zauberberg I+II, 34811 Buddenbrooks,
# 35328 Königliche Hoheit, 36766 Der kleine Herr Friedemann (Novellen),
# 23313 Tonio Kröger, 12108 Der Tod in Venedig,
# 12053 Gladius Dei / Schwere Stunde, 13810 Tristan

OUT=demo/build/mann_full.txt
: > "$OUT"
for id in $BOOKS; do
  f=demo/build/pg$id.txt
  if [ ! -s "$f" ]; then
    echo "fetching $id..."
    curl -sL "https://www.gutenberg.org/ebooks/$id.txt.utf-8" -o "$f"
  fi
  # keep only the text between the PG START/END markers, then unwrap
  # the hard-wrapped paragraphs (the model should learn language, not
  # the column-70 line breaks): paragraph mode joins each paragraph to
  # one line, single blank line between paragraphs, whitespace runs
  # collapsed.
  awk '/^\*\*\* START OF/{flag=1; next} /^\*\*\* END OF/{flag=0} flag' "$f" \
    | tr -d '\r' \
    | awk 'BEGIN{RS=""; ORS="\n\n"} {gsub(/[ \t]*\n[ \t]*/," "); gsub(/[ \t]+/," "); sub(/^ /,""); print}' >> "$OUT"
done
wc -c "$OUT"
# sanity: must be valid UTF-8
iconv -f UTF-8 -t UTF-8 "$OUT" > /dev/null && echo "corpus: UTF-8 valid"
