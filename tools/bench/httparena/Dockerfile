# The Scaly entry for HttpArena: tools/bench/http/arena.scaly
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
# Everything is one whole-program module (`scaly build --release`: runtime,
# packages and program linked in process, -O2, hidden definitions) generated for
# the build host's CPU, as the Rust entries do with -C target-cpu=native — so
# the image is built on the machine that runs it. Without pgo/arena.profdata
# in the context (SCALY_PGO=0) the arena is built the same way minus the
# profile.
#
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
# the compiler and the tool (no language server) and /tmp/libscaly.a, from the seed
RUN SCALYC_SEED_NO_SCALYLS=1 tools/build-from-seed.sh
# The arena is built by the one tool: `scaly build --release` takes every
# package as bitcode out of its cache and links, optimises and emits the whole
# program as one module (until 2026-10-01: ten IR emissions merged by
# tools/link-lto.sh). The packages say which C libraries they use (`extern
# ssl, crypto`); the tool finds them in $SCALY_LIBRARY_PATH, and since those
# directories hold STATIC libraries only -- libdeflate.a is copied beside them
# for that -- the runtime image needs nothing beyond the base system.
ENV SCALY_LIBRARY_PATH=/opt/ngtcp2/lib:/opt/openssl/lib:/opt/static/lib
RUN mkdir -p /opt/static/lib \
 && cp "$(find /usr/lib -name libdeflate.a | head -1)" /opt/static/lib/

# --pgo-train links with clang-21, whose driver adds the profile runtime of
# the LLVM that instrumented the module.
FROM build AS pgo-train
RUN scalyc/build/scaly build tools/bench/http/arena.scaly --pgo-train -mcpu=native -o /arena_instr \
 && scalyc/build/scaly build tools/bench/http/train.scaly --release -o /train

FROM build AS optimized
COPY pgo/ /pgo/
RUN PGO="--release" \
 && if [ -f /pgo/arena.profdata ]; then PGO="--pgo /pgo/arena.profdata"; fi \
 && scalyc/build/scaly build tools/bench/http/arena.scaly $PGO -mcpu=native -o /arena

FROM ubuntu:26.04
COPY --from=optimized /arena /arena
EXPOSE 8080 8081 8082 8443 8443/udp
CMD ["/arena", "8080"]
