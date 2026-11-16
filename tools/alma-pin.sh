# tools/alma-pin.sh -- sourced INSIDE an AlmaLinux container
# (tools/make-bindist-linux.sh, trust/check-gcc.sh): the packages of ONE
# point release, not of "9".
#
# "9" follows the newest point release, and that moved LLVM from 21 to 22
# (2026-11-16: 9's AppStream offered 22.1.8 only, 9.8's still 21.1.8) --
# the Linux archives and the trust check stopped building, and nothing in
# the tree had changed. A point release's repositories keep their packages;
# when the next one is final they move, unchanged, to vault.almalinux.org,
# the second address below. So the tools a Linux archive is made with can
# be had again, which is what tools/verify-archive.sh needs.
ALMA_RELEASE=9.8
ALMA_IMAGE="almalinux:$ALMA_RELEASE"
# LLVM as every other system builds Scaly with it
ALMA_LLVM=21.1.8

alma_pin() {
  local r
  {
    for r in BaseOS AppStream CRB; do
      echo "[pin-$r]"
      echo "name=AlmaLinux $ALMA_RELEASE $r"
      echo "baseurl=https://repo.almalinux.org/almalinux/$ALMA_RELEASE/$r/\$basearch/os/"
      echo "        https://vault.almalinux.org/$ALMA_RELEASE/$r/\$basearch/os/"
      echo "gpgcheck=1"
      echo "gpgkey=file:///etc/pki/rpm-gpg/RPM-GPG-KEY-AlmaLinux-9"
      echo "enabled=1"
      echo
    done
  } > /etc/yum.repos.d/scaly-pinned.repo
  # the image's own repositories follow "9": off
  sed -i 's/^enabled=1/enabled=0/' /etc/yum.repos.d/almalinux*.repo
  dnf -q clean all > /dev/null 2>&1 || true
}
