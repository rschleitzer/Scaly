#!/bin/bash
# Print a node that STARTS, or nothing (exit 1).
#
# `command -v node` answers a program that may not run: a package manager
# that moves a library on leaves a node behind that aborts in the loader
# before its first line (Homebrew's node against a newer simdjson). A suite
# that asked only whether node is there then fails every run instead of
# skipping. Asked here: $NODE, the node on the PATH, then Homebrew's
# versioned ones, newest first -- each by running it.
#
#     NODE=$(tools/node.sh) || { echo "…: SKIP (no node that starts)"; exit 0; }
starts() {
  # in a shell of its own: a node that aborts says so through its parent
  bash -c '"$0" -e 0; exit $?' "$1" >/dev/null 2>&1
}
for cand in "${NODE:-}" "$(command -v node 2>/dev/null)" \
            $(ls -d /opt/homebrew/opt/node@*/bin/node /usr/local/opt/node@*/bin/node 2>/dev/null | sort -t@ -k2 -n -r); do
  [ -n "$cand" ] || continue
  if starts "$cand"; then
    echo "$cand"
    exit 0
  fi
done
exit 1
