#!/bin/bash
# The check of trust/check.sh with GCC alone, in a container of AlmaLinux 9:
# g++ builds the compiler from the C++, gcc the runtime's C and assembly
# files, and no clang is installed, so nothing of LLVM's toolchain takes part
# but libLLVM itself. The release refuses a version this does not pass.
#
#   trust/check-gcc.sh
#
# Outside the container (any machine with docker) it hands the tree -- the
# files git knows and the new ones it does not ignore -- to a container of
# $SCALY_TRUST_IMAGE and runs itself there.
set -e
cd "$(dirname "$0")/.."
IMAGE="${SCALY_TRUST_IMAGE:-almalinux:9}"

if [ "${SCALY_IN_TRUST:-0}" != 1 ]; then
  command -v docker >/dev/null 2>&1 || { echo "trust/check-gcc.sh: FAIL — no docker"; exit 1; }
  git ls-files -z --cached --others --exclude-standard \
    | xargs -0 sh -c 'for f; do [ -e "$f" ] && printf "%s\0" "$f"; done' _ \
    | tar --null -T - -cf - \
    | docker run --rm -i -e SCALY_IN_TRUST=1 "$IMAGE" \
        bash -c 'set -e; mkdir /src; cd /src; tar -xf -; trust/check-gcc.sh'
  exit
fi

# --- in the container ---------------------------------------------------------
say() { echo "trust/check-gcc.sh: $*"; }
say "$(grep PRETTY_NAME /etc/os-release | cut -d= -f2 | tr -d '"')"
dnf -q -y install dnf-plugins-core > /dev/null
dnf config-manager --set-enabled crb
dnf -q -y install gcc-c++ llvm-devel diffutils which findutils file make python3 perl-Digest-SHA > /dev/null
if rpm -q clang > /dev/null 2>&1 || command -v clang > /dev/null 2>&1; then
  say "FAIL — clang is installed in the container"; exit 1
fi
say "$(g++ --version | head -1), LLVM $(llvm-config --version)"
CXX=g++ CC=gcc trust/check.sh /tmp/trust
