# Measuring the HttpArena entry on AWS bare metal

HttpArena measures on a Threadripper PRO 3995WX (Zen 2, one socket, 64 cores,
128 threads): the server on 32 cores and their siblings, the load generators
on the rest. AWS has no Zen 2 bare metal; the nearest is **c6a.metal** (Zen 3,
two sockets of 48 cores, 192 threads), where setup.sh rebuilds the same split
from the machine's topology -- the server on one socket, the load generators
on the other.

Within a smaller vCPU quota, `TYPE=c6a.16xlarge ./launch.sh` takes the same
Zen 3 generation virtualized with 64 vCPUs on one socket: setup.sh halves it,
32 logical CPUs each for the server and the load generators -- half the
board's size, so it finds failures and ranks the entries, but says nothing
about 64 cores.

The host costs while it runs (about 8 USD an hour on demand in Frankfurt) and
ends itself after the hours launch.sh is given.

```sh
./launch.sh 8                     # start it, wait for ssh; ends itself after 8 h
./prepare.sh <httparena-commit>   # entry + HttpArena on it, the CPU split, measure.sh started
./status.sh                       # progress, and when it ends itself
./finish.sh ~/bench/2026-10-22    # fetch the results, terminate the host
./compare.py ~/bench/2026-10-22   # per profile: Scaly's rank among the frameworks measured
```

`prepare.sh <commit> scaly bun` measures only those entries; by default
measure.sh runs the Scaly entry and the framework entries the default board
shows on top (standard mode, flagship or emerging). It validates the Scaly
entry first (`validate-scaly.log`), then runs HttpArena's full
`benchmark.sh` for each entry: every profile it subscribes to, three runs,
the best kept. The load generators run as HttpArena's Docker images
(`LOADGEN_DOCKER=true`), the same for every entry.

What differs from the board, and why the numbers compare entries WITHIN one
run, not with the published ones: Zen 3 instead of Zen 2, two sockets (the
server and the load generators talk across them), the load generators in
Docker.

The key pair (`~/.ssh/scaly-bench.pem`) and the security group `scaly-bench`
(ssh from the launching address only) stay between runs.
