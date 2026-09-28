# The Scaly entry for HttpArena (ROADMAP-http.md): tools/bench/http/arena.scaly
# on the http package, built from the committed seed the way CI's Linux leg
# builds the compiler (.github/actions/llvm20), then copied alone into a
# runtime image. The build context is not the repository: build.sh stages the
# few directories the build reads.

FROM ubuntu:24.04 AS build
ENV DEBIAN_FRONTEND=noninteractive
RUN apt-get update \
 && apt-get install -y --no-install-recommends llvm-20 llvm-20-dev clang-20 lld-20 perl ca-certificates \
 && rm -rf /var/lib/apt/lists/*
# seed.sh and the compiler's own link invoke `clang`; libLLVM-20.so sits under
# the versioned prefix; GNU BFD ld cannot link libLLVM-20, so `ld` is lld.
ENV CLANG=clang-20
ENV LD_LIBRARY_PATH=/usr/lib/llvm-20/lib
RUN ln -sf /usr/bin/clang-20 /usr/local/bin/clang \
 && update-alternatives --install /usr/bin/ld ld /usr/bin/ld.lld-20 100
WORKDIR /scaly
COPY . .
# the compiler (no language server) and /tmp/libscaly.a, both from the seed
RUN SCALYC_SEED_NO_SCALYLS=1 tools/build-from-seed.sh
# the http package's archive, then the program at -O2 against it
RUN . tools/llvm-env.sh \
 && scalyc/build/scalyc -S --no-prelude -o /tmp/http.ll packages/http/0.1.0/http.scaly \
 && sed 's/^define linkonce_odr /define weak_odr /' /tmp/http.ll > /tmp/http_weak.ll \
 && "$OPT" -O2 /tmp/http_weak.ll -o /tmp/http_opt.bc \
 && "$LLC" -relocation-model=pic -O2 -filetype=obj /tmp/http_opt.bc -o /tmp/http.o \
 && ar rcs /tmp/libhttp.a /tmp/http.o \
 && scalyc/build/scalyc -O2 -o /arena tools/bench/http/arena.scaly /tmp/libhttp.a

FROM ubuntu:24.04
COPY --from=build /arena /arena
EXPOSE 8080
CMD ["/arena", "8080"]
