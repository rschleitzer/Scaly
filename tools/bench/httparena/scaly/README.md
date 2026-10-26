# scaly

[Scaly](https://scaly.io)'s standard `http` package, used the way an
application uses it. Scaly is a compiled language whose memory lives in
regions the compiler infers: there is no garbage collector and no borrow
checker, and what a request handler allocates is freed when it returns.

The whole entry is [`arena.scaly`](arena.scaly) and
[`db.scaly`](db.scaly); [`train.scaly`](train.scaly) is the profile run of
its build.

## Profiles

| profile | how |
|---|---|
| baseline, limited-conn, latency-* | the router's `/baseline11`; query parameters and the body as the package hands them over |
| pipelined | the parser is resumable at any byte; every complete request in a read is answered, and the responses leave in one write |
| async | `Scheduler.sleep` suspends the connection's fiber; no thread waits |
| json-comp | the dataset is parsed once into records with the `json` package; each answer is serialized per request by its `JsonWriter` and sent with `send_json`; the router's response compression (`app.compress`) gzips it per request where the client accepts gzip, through the `compress` package (libdeflate, level 1 as nginx's default) |
| json-tls, 8gbit, static-tls | the same routes over TLS 1.3 on `:8081`, from the `tls` and `https` packages (OpenSSL) |
| static-* | the package's static handler: `.br`/`.gz` siblings chosen by `Accept-Encoding`, and a file replaced on disk is served anew |
| baseline-h2, static-h2 | the package's own HTTP/2 over TLS on `:8443` |
| baseline-h2c, json-h2c | HTTP/2 by prior knowledge on a dedicated `:8082` (`start_h2c`), which refuses HTTP/1.1 with 505; `:8080` answers both |
| baseline-h3, static-h3 | the `h3` package: HTTP/3 over QUIC (ngtcp2) on udp `:8443`, the same router |
| async-db | the `pg` package's pool: a prepared statement per pooled connection, rows read from the connection's receive buffer; over HTTP/2 the route runs beside the other streams |
| unary-grpc | gRPC framing and protobuf varints from the package |
| echo-ws* | the package's WebSocket upgrade; every message echoed |

## Runtime

One scheduler per core (the CPUs the process may run on), each with its own
io_uring and its own listener on the port (`SO_REUSEPORT`): the kernel
spreads the connections, and each scheduler accepts and serves its own,
handing nothing to another thread. A connection is a fiber with a region of
its own; each request is handled in a region that dies with the handler.
Nothing is tuned beyond the defaults.

## Build

The Dockerfile installs the released Scaly toolchain, as anyone does
(`curl -fsSL https://scaly.io/install.sh | sh`), and builds with
`scaly build`: the program, the standard library and every package as one
module. [`build.sh`](build.sh) makes it profile-guided — an instrumented
build, a training run under the request shapes of the profiles (against a
Postgres seeded as the board's sidecar, for async-db), the final
build with that profile and `-mcpu=native` — which is the
[production build](https://scaly.io/lang/tutorial.htm#tut-production) the
documentation describes. `SCALY_PGO=0 ./build.sh` builds with `--release`
alone.
