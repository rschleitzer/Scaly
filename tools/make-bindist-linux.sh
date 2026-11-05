#!/bin/bash
# Build the Linux archive (tools/make-bindist.sh) on the OLDEST system we
# promise, in a container: AlmaLinux 9. A Linux program runs on the
# C library it was linked against and newer ones, so the build host is the
# floor -- built on Ubuntu 26.04 the archive asked for glibc 2.43 and started
# on nothing older (Debian 13, Ubuntu 24.04, RHEL 9 and 10, Fedora 43 all fell
# back to building from the seed). Built here it starts on RHEL 9 and its
# rebuilds, Debian 12, Ubuntu 22.04, Fedora, openSUSE, Arch and newer.
# ★The floor is glibc 2.35, not 2.34: the unwinder of gcc-toolset's libgcc
# calls _dl_find_object, which RHEL 9 backported into its 2.34 under the
# version GLIBC_2.35. An upstream 2.34 lacks it -- only distributions past
# their end of life shipped one.
#
# AlmaLinux 9 carries LLVM 21.1.8 with its static libraries (llvm-static),
# the version the Windows legs take from LLVM's release. Its packages are
# built for x86-64-v2 (SSE4.2, POPCNT: 2009 and later), so on x86-64 that is
# the floor of the CPU as well. It has no libzstd.a: Zstandard is built here
# from its release. No libxml2.a either, and none is needed (build-from-seed.sh).
#
#   tools/make-bindist-linux.sh [version] [outdir]   (default: the VERSION file, dist)
#
# Outside the container (any machine with docker, the release runners
# included) it hands the tree -- the files git knows and the new ones it does
# not ignore -- to a container of $SCALY_FLOOR_IMAGE and runs itself there;
# the archive comes back into <outdir>. $SCALY_PGO_PROFILE goes along, as a
# path inside the tree.
set -e
cd "$(dirname "$0")/.."
VERSION="${1:-$(cat VERSION)}"
OUT="${2:-dist}"
IMAGE="${SCALY_FLOOR_IMAGE:-almalinux:9}"
GLIBC_FLOOR=2.35   # with RHEL 9's 2.34, which carries _dl_find_object (above)
ZSTD_VERSION=1.5.7
ZSTD_SHA256=eb33e51f49a15e023950cd7825ca74a4a2b43db8354825ac24fc1b7ee09e6fa3

if [ "${SCALY_IN_FLOOR:-0}" != 1 ]; then
  command -v docker >/dev/null 2>&1 || { echo "make-bindist-linux: FAIL — no docker"; exit 1; }
  mkdir -p "$OUT"
  OUT_ABS="$(cd "$OUT" && pwd)"
  git ls-files -z --cached --others --exclude-standard \
    | xargs -0 sh -c 'for f; do [ -e "$f" ] && printf "%s\0" "$f"; done' _ \
    | tar --null -T - -cf - \
    | docker run --rm -i -e SCALY_IN_FLOOR=1 -e SCALY_PGO_PROFILE -v "$OUT_ABS:/out" "$IMAGE" \
        bash -c 'set -e; mkdir /src; cd /src; tar -xf -; tools/make-bindist-linux.sh "$0" /out' "$VERSION"
  exit
fi

# --- in the container ---------------------------------------------------------
say() { echo "make-bindist-linux: $*"; }
say "$(grep PRETTY_NAME /etc/os-release | cut -d= -f2 | tr -d '"'), $(ldd --version | head -1)"
dnf -q -y install dnf-plugins-core > /dev/null
dnf config-manager --set-enabled crb
# clang brings gcc-toolset's libstdc++ (the C++ runtime LLVM is built against);
# compiler-rt the profile runtime tools/make-profile.sh needs
dnf -q -y install clang lld llvm-devel llvm-static compiler-rt libstdc++-static \
  zlib-static python3 diffutils which findutils file make > /dev/null

# Zstandard's static library, where clang -print-file-name finds it
if [ ! -f "$(clang -print-file-name=libzstd.a)" ]; then
  Z="$(mktemp -d)"
  curl -fsSL -o "$Z/zstd.tar.gz" "https://github.com/facebook/zstd/releases/download/v$ZSTD_VERSION/zstd-$ZSTD_VERSION.tar.gz"
  echo "$ZSTD_SHA256  $Z/zstd.tar.gz" | sha256sum -c --quiet
  tar -xzf "$Z/zstd.tar.gz" -C "$Z"
  make -s -C "$Z/zstd-$ZSTD_VERSION/lib" -j"$(nproc)" libzstd.a CC=clang CFLAGS="-O2 -fPIC" > /dev/null
  cp "$Z/zstd-$ZSTD_VERSION/lib/libzstd.a" /usr/lib64/libzstd.a
  rm -rf "$Z"
fi

tools/make-bindist.sh "$VERSION" "$OUT"

# The floor, checked: no symbol of the programs may ask for a newer glibc.
TARBALL="$OUT/scaly-$VERSION-$(uname -s | tr '[:upper:]' '[:lower:]')-$(uname -m).tar.gz"
X="$(mktemp -d)"
tar -xzf "$TARBALL" -C "$X"
newest=$(for p in "$X"/libexec/*; do llvm-readelf --dyn-syms -W "$p"; done \
  | grep -o 'GLIBC_[0-9][0-9.]*' | sed 's/GLIBC_//' | sort -V -u | tail -1)
rm -rf "$X"
[ "$(printf '%s\n%s\n' "$newest" "$GLIBC_FLOOR" | sort -V | tail -1)" = "$GLIBC_FLOOR" ] \
  || { say "FAIL — the programs ask for glibc $newest, the floor is $GLIBC_FLOOR"; exit 1; }
say "OK — the programs ask for glibc $newest at most"
