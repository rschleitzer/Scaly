# The Scaly entry: arena.scaly on the http package and its siblings, built by
# the released Scaly toolchain (the installer at a pinned version, ready-made
# programs -- no LLVM to build) as one whole-program module, optimised with a
# profile and for the CPU of the machine that builds it (the production build
# of https://scaly.io/lang/tutorial.htm#tut-production).
#
#   --target pgo-train   the instrumented arena and its training client;
#                        build.sh runs the training between the two builds
#   default target       the arena with that profile (pgo/arena.profdata in
#                        the context), or --release alone without one -- what
#                        the gateway entries' compose files build
#
FROM ubuntu:26.04 AS build
ENV DEBIAN_FRONTEND=noninteractive
# clang and lld link; llvm-21 brings llvm-profdata and libclang-rt-21-dev the
# profile runtime, both for the profile-guided build; libdeflate is the
# compress package's library
RUN apt-get update \
 && apt-get install -y --no-install-recommends clang-21 lld-21 llvm-21 libclang-rt-21-dev libdeflate-dev pkg-config perl make curl ca-certificates \
 && rm -rf /var/lib/apt/lists/* \
 && ln -sf /usr/bin/clang-21 /usr/local/bin/clang \
 && update-alternatives --install /usr/bin/ld ld /usr/bin/ld.lld-21 100
# OpenSSL 3.5 or later for the tls, https and h3 packages (TLS 1.3, and
# HTTP/3's TLS through ngtcp2's crypto helper), built static
RUN curl -fsSL https://github.com/openssl/openssl/releases/download/openssl-3.6.3/openssl-3.6.3.tar.gz | tar xz -C /tmp \
 && cd /tmp/openssl-3.6.3 \
 && CC=clang ./Configure --prefix=/opt/openssl --libdir=lib no-shared no-tests no-docs \
 && make -j"$(nproc)" \
 && make install_sw \
 && cd / && rm -rf /tmp/openssl-3.6.3
# ngtcp2 for HTTP/3 (the h3 package), built static
RUN curl -fsSL https://github.com/ngtcp2/ngtcp2/releases/download/v1.25.0/ngtcp2-1.25.0.tar.gz | tar xz -C /tmp \
 && cd /tmp/ngtcp2-1.25.0 \
 && CC=clang PKG_CONFIG_PATH=/opt/openssl/lib/pkgconfig ./configure --prefix=/opt/ngtcp2 --enable-lib-only --disable-shared --with-openssl \
 && make -j"$(nproc)" \
 && make install \
 && cd / && rm -rf /tmp/ngtcp2-1.25.0
# Scaly itself, as anyone installs it
ENV SCALY_VERSION=0.2.25
RUN curl -fsSL https://scaly.io/install.sh | SCALY_PREFIX=/opt/scaly SCALY_NO_MODIFY_PATH=1 sh
ENV PATH=/opt/scaly/bin:$PATH
# The packages name their C libraries (`extern ssl, crypto`); the tool finds
# them here, static only, so the runtime image needs nothing beyond the base
ENV SCALY_LIBRARY_PATH=/opt/ngtcp2/lib:/opt/openssl/lib:/opt/static/lib
RUN mkdir -p /opt/static/lib \
 && cp "$(find /usr/lib -name libdeflate.a | head -1)" /opt/static/lib/
WORKDIR /entry
COPY arena.scaly db.scaly stack.scaly train.scaly ./

FROM build AS pgo-train
RUN scaly build arena.scaly --pgo-train -mcpu=native -o /arena_instr \
 && scaly build train.scaly --release -o /train

# the profile comes through the context when build.sh made one; a plain
# build of this directory (a gateway entry's compose file) has none
FROM build AS optimized
RUN --mount=type=bind,target=/ctx \
    PGO="--release" \
 && if [ -f /ctx/pgo/arena.profdata ]; then PGO="--pgo /ctx/pgo/arena.profdata"; fi \
 && scaly build arena.scaly $PGO -mcpu=native -o /arena

FROM ubuntu:26.04
COPY --from=optimized /arena /arena
EXPOSE 8080 8081 8082 8443 8443/udp
CMD ["/arena"]
