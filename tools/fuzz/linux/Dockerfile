# tools/fuzz/linux/Dockerfile -- built by run.sh from a context it makes:
# the HttpArena entry's build stage (scaly-fuzz-base: OpenSSL, ngtcp2,
# libdeflate and the released toolchain, as the entry builds on the board),
# the packages the arena uses replaced by this tree's (as the AWS bench's
# variant.sh does), the arena built --release, and python3 for the fuzzer,
# which starts the arena itself.
FROM scaly-fuzz-base
RUN apt-get update \
 && apt-get install -y --no-install-recommends python3 procps \
 && rm -rf /var/lib/apt/lists/*
COPY packages/ /tmp/packages/
RUN P=/opt/scaly/toolchain/packages; \
    for p in /tmp/packages/*; do n=$(basename "$p"); rm -rf "$P/$n/0.2.0"; cp -R "$p" "$P/$n/0.2.0"; done; \
    rm -rf /tmp/packages
WORKDIR /entry
COPY entry/ ./
RUN scaly build arena.scaly --release -o /arena
COPY fuzz/ /fuzz/
