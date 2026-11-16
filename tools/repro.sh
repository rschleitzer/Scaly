# tools/repro.sh -- sourced by the scripts that make what is handed out
# (tools/make-bindist.sh, make-bindist-windows.sh, make-dist.sh): what makes
# an archive the same bytes when it is made again from the same commit, and
# what a rebuild needs to know to make it so (tools/verify-archive.sh).
#
# Built twice from one commit (2026-11-16, darwin-arm64), the three programs
# were the same bytes -- with the same PGO profile. A profile made twice is
# not: the training run counts the probes of a hash table keyed by
# addresses, and where the heap lies changes from run to run. So the
# profile is an INPUT of a release, published beside the archive like the
# seed beside the sources; it steers inlining and layout, never what a
# program does. What differed besides were time stamps: the gzip header,
# tar's entries, the members of libscaly.a.

# The time every entry of an archive carries: the commit's, so that two
# packings of one commit agree (SOURCE_DATE_EPOCH, where set, wins).
repro_epoch() {
  if [ -n "${SOURCE_DATE_EPOCH:-}" ]; then echo "$SOURCE_DATE_EPOCH"; return; fi
  git log -1 --format=%ct 2>/dev/null || echo 0
}

# A time in seconds as touch -t wants it, in UTC.
repro_touch_time() {
  if date -u -r 0 +%Y >/dev/null 2>&1; then
    TZ=UTC date -u -r "$1" +%Y%m%d%H%M.%S
  else
    TZ=UTC date -u -d "@$1" +%Y%m%d%H%M.%S
  fi
}

# Every file and directory below <dir> under <entries...>: one time, and
# the modes of a program (755) or of data (644) -- what umask and the
# order of making them left behind is not part of what is handed out.
repro_normalize() {
  local dir="$1"; shift
  local t; t="$(repro_touch_time "$(repro_epoch)")"
  ( cd "$dir" && find "$@" -type d -exec chmod 755 {} + )
  ( cd "$dir" && find "$@" -type f -exec chmod u+rw,go+r,go-w {} + )
  ( cd "$dir" && find "$@" -exec env TZ=UTC touch -h -t "$t" {} + )
}

# <tarball> <dir> <entries...>: a .tar.gz of the entries below <dir>, in
# byte order of their names, owned by 0, with one time and no gzip name or
# time. GNU tar (Linux) and bsdtar (macOS) say the same in their own words.
repro_tar_gz() {
  local tarball="$1" dir="$2"; shift 2
  repro_normalize "$dir" "$@"
  local list; list="$(mktemp)"
  ( cd "$dir" && find "$@" | LC_ALL=C sort ) > "$list"
  local abs; abs="$(cd "$(dirname "$tarball")" && pwd)/$(basename "$tarball")"
  if tar --version 2>/dev/null | grep -q 'GNU tar'; then
    ( cd "$dir" && tar --no-recursion --owner=0 --group=0 --numeric-owner --format=gnu -cf - -T "$list" ) | gzip -n -9 > "$abs"
  else
    ( cd "$dir" && COPYFILE_DISABLE=1 tar --no-xattrs --no-mac-metadata --no-acls --no-fflags -n \
        --uid 0 --gid 0 --uname root --gname root --format=ustar -cf - -T "$list" ) | gzip -n -9 > "$abs"
  fi
  local rc=$?
  rm -f "$list"
  return $rc
}

# <zip> <dir> <entries...>: a .zip of the entries below <dir>, by Windows'
# own tar (bsdtar: the suffix makes it a zip; Git Bash's tar cannot), in
# byte order of their names, with one time. A zip keeps that time as DOS
# time, in the zone of the machine that packs it: TZ=UTC0 for the C runtime,
# so that a rebuild elsewhere writes the same bytes.
repro_zip() {
  local zip="$1" dir="$2"; shift 2
  repro_normalize "$dir" "$@"
  local list; list="$(mktemp)"
  ( cd "$dir" && find "$@" | LC_ALL=C sort ) > "$list"
  local abs; abs="$(cd "$(dirname "$zip")" && pwd)/$(basename "$zip")"
  rm -f "$abs"
  local wtar; wtar="$(cygpath -u "${SYSTEMROOT:-C:\\Windows}")/System32/tar.exe"
  ( cd "$dir" && TZ=UTC0 "$wtar" -a -c -n --uid 0 --gid 0 --uname root --gname root \
      -f "$(cygpath -w "$abs")" -T "$(cygpath -w "$list")" )
  local rc=$?
  rm -f "$list"
  return $rc
}

# <out.a> <in.a>: the archive again, its members in the same order, with
# llvm-ar's deterministic header (time, owner and mode zero) -- the system's
# ar writes the time each member was made.
repro_ar() {
  local out="$1" in="$2" ar="${3:-}"
  [ -n "$ar" ] || ar="$(repro_llvm_tool llvm-ar)"
  [ -n "$ar" ] || { echo "repro: no llvm-ar" >&2; return 1; }
  local tmp; tmp="$(mktemp -d)"
  local members; members="$("$ar" t "$in")" || { rm -rf "$tmp"; return 1; }
  local inabs; inabs="$(cd "$(dirname "$in")" && pwd)/$(basename "$in")"
  local outabs; outabs="$(cd "$(dirname "$out")" && pwd)/$(basename "$out")"
  ( cd "$tmp" && "$ar" x "$inabs" && rm -f "$outabs" && "$ar" rcsD "$outabs" $members )
  local rc=$?
  rm -rf "$tmp"
  return $rc
}

# A tool of the LLVM the build uses: beside its llvm-config, else on PATH.
repro_llvm_tool() {
  local t="$1"
  if [ -n "${LLVM_PREFIX:-}" ] && [ -x "$LLVM_PREFIX/bin/$t" ]; then echo "$LLVM_PREFIX/bin/$t"; return; fi
  command -v "$t" 2>/dev/null || command -v "$t-${LLVM_MAJOR:-21}" 2>/dev/null || true
}

# The SHA-256 of a file, wherever the box keeps its tool.
repro_sha256() {
  if command -v sha256sum >/dev/null 2>&1; then sha256sum "$1" | cut -d' ' -f1
  else shasum -a 256 "$1" | cut -d' ' -f1; fi
}

# <file> <version> <system> <profile or none> <dir> <entries...>: what a
# rebuild must match -- the commit, the tools that made the programs, the
# profile they were optimised with -- and the SHA-256 of every file handed
# out, to compare a rebuild with where the archive's own bytes differ.
repro_buildinfo() {
  local file="$1" version="$2" system="$3" profile="$4" dir="$5"; shift 5
  {
    echo "version $version"
    echo "system $system"
    # a tree with changes is not its commit: said, so that nobody rebuilds
    # the commit and wonders
    # (in a container without the repository the host says it:
    # SCALY_BUILD_COMMIT)
    if [ -n "${SCALY_BUILD_COMMIT:-}" ]; then
      echo "commit $SCALY_BUILD_COMMIT"
    else
      local dirty=""
      [ -n "$(git status --porcelain --untracked-files=no 2>/dev/null)" ] && dirty=" dirty"
      echo "commit $(git rev-parse HEAD 2>/dev/null || echo unknown)$dirty"
    fi
    echo "epoch $(repro_epoch)"
    local cfg; cfg="$(repro_llvm_tool llvm-config)"
    [ -n "$cfg" ] && echo "llvm $("$cfg" --version)"
    echo "clang $(${CLANG:-clang} --version 2>/dev/null | head -1)"
    case "$(uname -s)" in
      Darwin)
        echo "ld $(ld -v 2>&1 | head -1)"
        echo "sdk $(xcrun --show-sdk-version 2>/dev/null)"
        command -v brew >/dev/null 2>&1 && echo "brew $(brew list --versions llvm@21 zstd 2>/dev/null | tr '\n' ' ')"
        ;;
      Linux)
        local lld; lld="$(repro_llvm_tool ld.lld)"
        [ -n "$lld" ] && echo "ld $("$lld" --version | head -1)"
        command -v rpm >/dev/null 2>&1 && echo "rpm $(rpm -q clang lld llvm llvm-static llvm-devel compiler-rt libstdc++-static zlib-static glibc-devel gcc 2>/dev/null | grep -v 'not installed' | tr '\n' ' ')"
        [ -f /etc/os-release ] && echo "os $(. /etc/os-release && echo "$PRETTY_NAME")"
        [ -n "${SCALY_BUILD_IMAGE:-}" ] && echo "image $SCALY_BUILD_IMAGE"
        ;;
      MINGW*|MSYS*|CYGWIN*)
        # read off LIB: set by tools/win-env.sh, or by the developer prompt
        echo "msvc $(printf '%s' "${LIB:-}" | grep -o 'MSVC.[0-9][0-9.]*' | head -1 | sed 's/^MSVC.//')"
        echo "sdk $(printf '%s' "${LIB:-}" | grep -o '[\\/]Lib[\\/]10\.[0-9.]*' | head -1 | sed 's/^.Lib.//')"
        ;;
    esac
    if [ "$profile" = none ]; then echo "profile none"
    else echo "profile $(repro_sha256 "$profile")"; fi
    ( cd "$dir" && find "$@" -type f | LC_ALL=C sort ) | while read -r f; do
      echo "file $(repro_sha256 "$dir/$f") $f"
    done
  } > "$file"
}
