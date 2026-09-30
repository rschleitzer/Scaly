# The Scaly entry for HttpArena (ROADMAP-http.md): tools/bench/http/arena.scaly
# on the http package, built from the committed seed the way CI's Linux leg
# builds the compiler (.github/actions/llvm21), then copied alone into a
# runtime image. The build context is not the repository: build.sh stages the
# few directories the build reads, and runs the PGO training BETWEEN two
# builds of this file:
#
#   --target pgo-train   an instrumented arena and the training client
#   (build.sh)           trains it in a container that allows io_uring
#   default target       the arena optimized with that profile
#
# Everything is one whole-program module (tools/link-lto.sh: runtime, http
# package and program merged, opt -O2, hidden definitions) code-generated for
# the build host's CPU, as the Rust entries do with -C target-cpu=native — so
# the image is built on the machine that runs it. Without pgo/arena.profdata
# in the context (SCALY_PGO=0) the arena is built the same way minus the
# profile.
#
# ★The IR files go USERS FIRST, dependencies after: llvm-link takes a
# linkonce_odr definition only when something already linked references it,
# so a package listed before its user loses every body only the user calls
# (tls before https left TlsStream.read & co. undefined).

FROM ubuntu:26.04 AS build
ENV DEBIAN_FRONTEND=noninteractive
RUN apt-get update \
 && apt-get install -y --no-install-recommends llvm-21 llvm-21-dev clang-21 lld-21 libclang-rt-21-dev libdeflate-dev perl make curl ca-certificates \
 && rm -rf /var/lib/apt/lists/*
# OpenSSL 3.5 or later (Ubuntu 24.04 ships 3.0) for the tls, https and h3
# packages -- TLS 1.3, h2, and HTTP/3's TLS through its QUIC TLS interface,
# which ngtcp2's crypto helper drives -- built static.
RUN curl -fsSL https://github.com/openssl/openssl/releases/download/openssl-3.6.3/openssl-3.6.3.tar.gz | tar xz -C /tmp \
 && cd /tmp/openssl-3.6.3 \
 && CC=clang-21 ./Configure --prefix=/opt/openssl --libdir=lib no-shared no-tests no-docs \
 && make -j"$(nproc)" \
 && make install_sw \
 && cd / && rm -rf /tmp/openssl-3.6.3
# ngtcp2 for HTTP/3 (packages/h3): QUIC itself, OpenSSL only for its TLS
# 1.3 through ngtcp2's crypto helper (libngtcp2_crypto_ossl, OpenSSL 3.5 or
# later). Until 2026-09-30 OpenSSL's own QUIC server did it all, and kept
# every stream a client opened until the connection ended.
RUN apt-get update \
 && apt-get install -y --no-install-recommends pkg-config \
 && rm -rf /var/lib/apt/lists/* \
 && curl -fsSL https://github.com/ngtcp2/ngtcp2/releases/download/v1.25.0/ngtcp2-1.25.0.tar.gz | tar xz -C /tmp \
 && cd /tmp/ngtcp2-1.25.0 \
 && CC=clang-21 PKG_CONFIG_PATH=/opt/openssl/lib/pkgconfig ./configure --prefix=/opt/ngtcp2 --enable-lib-only --disable-shared --with-openssl \
 && make -j"$(nproc)" \
 && make install \
 && cd / && rm -rf /tmp/ngtcp2-1.25.0
# seed.sh and the compiler's own link invoke `clang`; libLLVM-20.so sits under
# the versioned prefix; GNU BFD ld cannot link libLLVM-20, so `ld` is lld.
ENV CLANG=clang-21
ENV LD_LIBRARY_PATH=/usr/lib/llvm-21/lib
RUN ln -sf /usr/bin/clang-21 /usr/local/bin/clang \
 && update-alternatives --install /usr/bin/ld ld /usr/bin/ld.lld-21 100
WORKDIR /scaly
COPY . .
# the compiler (no language server) and /tmp/libscaly.a, both from the seed
RUN SCALYC_SEED_NO_SCALYLS=1 tools/build-from-seed.sh
# the whole program's IR: runtime, the http, json and compress packages, the
# arena, the trainer. libdeflate (gzip for json-comp) and OpenSSL (the tls and
# https packages, json-tls) are linked STATICALLY, so the runtime image needs
# nothing beyond the base system.
RUN mkdir -p /ir \
 && scalyc/build/scalyc -S --no-prelude --no-tests -o /ir/rt.ll packages/scaly/0.1.0/scaly.scaly \
 && scalyc/build/scalyc -S --no-prelude -o /ir/http.ll packages/http/0.1.0/http.scaly \
 && scalyc/build/scalyc -S --no-prelude -o /ir/json.ll packages/json/0.1.0/json.scaly \
 && scalyc/build/scalyc -S --no-prelude -o /ir/compress.ll packages/compress/0.1.0/compress.scaly \
 && scalyc/build/scalyc -S --no-prelude -o /ir/tls.ll packages/tls/0.1.0/tls.scaly \
 && scalyc/build/scalyc -S --no-prelude -o /ir/https.ll packages/https/0.1.0/https.scaly \
 && scalyc/build/scalyc -S --no-prelude -o /ir/h3.ll packages/h3/0.1.0/h3.scaly \
 && clang-21 -O2 -c packages/h3/0.1.0/h3/ngtcp2_glue.c -I/opt/ngtcp2/include -I/opt/openssl/include -o /ir/ngtcp2_glue.o \
 && scalyc/build/scalyc -S --no-prelude -o /ir/pg.ll packages/pg/0.1.0/pg.scaly \
 && scalyc/build/scalyc -S --no-prelude -o /ir/redis.ll packages/redis/0.1.0/redis.scaly \
 && scalyc/build/scalyc -S -o /ir/arena.ll tools/bench/http/arena.scaly \
 && scalyc/build/scalyc -S -o /ir/train.ll tools/bench/http/train.scaly

# The profile runtime is a static archive; the clang driver would pull its
# writer in with -u __llvm_profile_runtime, and a hand-made link must say so —
# without it the instrumented arena counts and never writes.
FROM build AS pgo-train
RUN TRIPLE=$(clang-21 -print-target-triple) \
 && PROFRT=$(find /usr/lib/llvm-21/lib/clang -name 'libclang_rt.profile*.a' | head -1) \
 && LTO_OPT_FLAGS="-mtriple=$TRIPLE -mcpu=native --pgo-kind=pgo-instr-gen-pipeline" \
    LTO_LLC_FLAGS="-mcpu=native" LINK_EXTRA="-Wl,-u,__llvm_profile_runtime $PROFRT /ir/ngtcp2_glue.o /opt/ngtcp2/lib/libngtcp2_crypto_ossl.a /opt/ngtcp2/lib/libngtcp2.a -l:libdeflate.a /opt/openssl/lib/libssl.a /opt/openssl/lib/libcrypto.a" \
    tools/link-lto.sh /arena_instr /ir/arena.ll /ir/https.ll /ir/h3.ll /ir/http.ll /ir/pg.ll /ir/redis.ll /ir/tls.ll /ir/json.ll /ir/compress.ll /ir/rt.ll \
 && tools/link-lto.sh /train /ir/train.ll /ir/http.ll /ir/rt.ll

FROM build AS optimized
COPY pgo/ /pgo/
RUN TRIPLE=$(clang-21 -print-target-triple) \
 && PGO="" \
 && if [ -f /pgo/arena.profdata ]; then PGO="--pgo-kind=pgo-instr-use-pipeline --profile-file=/pgo/arena.profdata"; fi \
 && LTO_OPT_FLAGS="-mtriple=$TRIPLE -mcpu=native $PGO" LTO_LLC_FLAGS="-mcpu=native" LINK_EXTRA="/ir/ngtcp2_glue.o /opt/ngtcp2/lib/libngtcp2_crypto_ossl.a /opt/ngtcp2/lib/libngtcp2.a -l:libdeflate.a /opt/openssl/lib/libssl.a /opt/openssl/lib/libcrypto.a" \
    tools/link-lto.sh /arena /ir/arena.ll /ir/https.ll /ir/h3.ll /ir/http.ll /ir/pg.ll /ir/redis.ll /ir/tls.ll /ir/json.ll /ir/compress.ll /ir/rt.ll

FROM ubuntu:26.04
COPY --from=optimized /arena /arena
EXPOSE 8080 8081 8082 8443 8443/udp
CMD ["/arena", "8080"]
