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

# A path as a native program wants it: Windows' spelling under Git Bash.
repro_native_path() {
  if command -v cygpath >/dev/null 2>&1; then cygpath -w "$1"; else printf '%s' "$1"; fi
}

# <zip> <dir> <entries...>: a .zip of the entries below <dir>, in byte
# order of their names, every entry with the commit's time and fixed modes
# and nothing else. Written by Python's zipfile, which writes only what it
# is told: Windows' tar.exe added a second time of its own to every entry
# (measured 2026-11-16: two packings of one directory differed there).
repro_zip() {
  local zip="$1" dir="$2"; shift 2
  local list; list="$(mktemp)"
  ( cd "$dir" && find "$@" | LC_ALL=C sort ) > "$list"
  local abs; abs="$(cd "$(dirname "$zip")" && pwd)/$(basename "$zip")"
  rm -f "$abs"
  local py; py="$(command -v python3 || command -v python)"
  [ -n "$py" ] || { echo "repro: no python for the zip" >&2; rm -f "$list"; return 1; }
  "$py" - "$(repro_native_path "$dir")" "$(repro_native_path "$list")" "$(repro_native_path "$abs")" "$(repro_epoch)" <<'PY'
import os, sys, time, zipfile
root, listing, out, epoch = sys.argv[1], sys.argv[2], sys.argv[3], int(sys.argv[4])
stamp = time.gmtime(max(epoch, 315532800))[:6]          # a zip counts from 1980
names = [l.rstrip('\r\n') for l in open(listing, encoding='utf-8') if l.strip()]
with zipfile.ZipFile(out, 'w') as z:
    for n in names:
        path = os.path.join(root, n)
        arc = n.replace('\\', '/')
        if os.path.isdir(path):
            info = zipfile.ZipInfo(arc.rstrip('/') + '/', stamp)
            info.external_attr = (0o40755 << 16) | 0x10
            info.create_system = 3
            z.writestr(info, b'')
        else:
            info = zipfile.ZipInfo(arc, stamp)
            mode = 0o755 if arc.endswith('.exe') else 0o644
            info.external_attr = (0o100000 | mode) << 16
            info.create_system = 3
            info.compress_type = zipfile.ZIP_DEFLATED
            with open(path, 'rb') as f:
                z.writestr(info, f.read(), compresslevel=9)
PY
  local rc=$?
  rm -f "$list"
  return $rc
}

# <object>: a COFF object's time stamp (bytes 4 to 7 of its header) set
# to 0. clang writes the clock there for Windows, and nothing reads it;
# llc and the assembler already write 0. Anything else -- a Mach-O or ELF
# object, LLVM bitcode, a big-object COFF file -- is left as it is.
repro_coff_untime() {
  local m; m="$(od -An -tx1 -N2 "$1" 2>/dev/null | tr -d ' \n')"
  case "$m" in
    6486|64aa|4c01) printf '\000\000\000\000' | dd of="$1" bs=1 seek=4 count=4 conv=notrunc 2>/dev/null ;;
  esac
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
  ( cd "$tmp" && "$ar" x "$inabs" && for m in $members; do repro_coff_untime "$m"; done \
      && rm -f "$outabs" && "$ar" rcsD "$outabs" $members )
  local rc=$?
  rm -rf "$tmp"
  return $rc
}

# A tool of the LLVM the build uses: beside its llvm-config, else on PATH.
repro_llvm_tool() {
  local t="$1"
  if [ -n "${LLVM_PREFIX:-}" ]; then
    [ -x "$LLVM_PREFIX/bin/$t" ] && { echo "$LLVM_PREFIX/bin/$t"; return; }
    [ -x "$LLVM_PREFIX/bin/$t.exe" ] && { echo "$LLVM_PREFIX/bin/$t.exe"; return; }
  fi
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
