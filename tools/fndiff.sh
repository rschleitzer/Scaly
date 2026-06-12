#!/bin/bash
# Per-function semantic diff between two .ll files.
# Normalizes @.str.N / @N global numbering, then diffs each define body.
# Usage: tools/fndiff.sh <a.ll> <b.ll>
s1=$1; s2=$2
norm() { sed -E 's/@\.str\.[0-9]+/@STR/g; s/@([0-9]+)/@G/g; s/@trace\.name\.[0-9]+/@TRACE/g'; }
grep '^define' $s1 | sed -E 's/^define[^@]*@([^(]+)\(.*/\1/' | sort > /tmp/fns_s1.txt
grep '^define' $s2 | sed -E 's/^define[^@]*@([^(]+)\(.*/\1/' | sort > /tmp/fns_s2.txt
comm -3 /tmp/fns_s1.txt /tmp/fns_s2.txt > /tmp/fns_only.txt
echo "=== defines only in one file: $(wc -l < /tmp/fns_only.txt)"
head -10 /tmp/fns_only.txt
# split bodies
mkdir -p /tmp/fd1 /tmp/fd2; rm -f /tmp/fd1/* /tmp/fd2/*
awk '/^define/{m=$0; sub(/^define[^@]*@/,"",m); sub(/\(.*/,"",m); f="/tmp/fd1/" m; inside=1} inside{print > f} /^}/{inside=0}' $s1
awk '/^define/{m=$0; sub(/^define[^@]*@/,"",m); sub(/\(.*/,"",m); f="/tmp/fd2/" m; inside=1} inside{print > f} /^}/{inside=0}' $s2
ndiff=0
for f in /tmp/fd1/*; do
  b=$(basename $f)
  [ -f /tmp/fd2/$b ] || continue
  if ! diff <(norm < $f) <(norm < /tmp/fd2/$b) > /dev/null 2>&1; then
    n=$(diff <(norm < $f) <(norm < /tmp/fd2/$b) | grep -c '^[<>]')
    echo "DIFF $b ($n lines)"
    ndiff=$((ndiff+1))
  fi
done
echo "=== total differing functions: $ndiff"
