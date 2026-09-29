// jsonbench — the Rust side of tools/bench/json/run.sh: the method of
// bench.scaly on the same file. Five rounds of `iterations` parses into each
// library's DOM (dropped at once), and five of as many writes of a tree parsed
// once; the best round counts. Throughput is the FILE's size per second in
// both directions. simd-json parses in place, so its parse includes the copy
// of the input it needs (as serde-rs/json-benchmark counts it).
//
//   jsonbench <file> [iterations]

use std::hint::black_box;
use std::time::Instant;

const ROUNDS: usize = 5;

fn best<F: FnMut()>(iterations: usize, mut f: F) -> f64 {
    let mut best = f64::MAX;
    for _ in 0..ROUNDS {
        let t0 = Instant::now();
        for _ in 0..iterations {
            f();
        }
        let dt = t0.elapsed().as_secs_f64();
        if dt < best {
            best = dt;
        }
    }
    best
}

fn mbps(bytes: usize, iterations: usize, secs: f64) -> u64 {
    ((bytes * iterations) as f64 / secs / 1e6) as u64
}

fn main() {
    let args: Vec<String> = std::env::args().collect();
    if args.len() < 2 {
        eprintln!("usage: jsonbench <file> [iterations]");
        std::process::exit(2);
    }
    let iterations: usize = args.get(2).and_then(|s| s.parse().ok()).unwrap_or(20);
    let data = std::fs::read(&args[1]).expect("read");
    let n = data.len();

    // serde_json: Value
    let p = best(iterations, || {
        let v: serde_json::Value = serde_json::from_slice(&data).unwrap();
        black_box(v);
    });
    let tree: serde_json::Value = serde_json::from_slice(&data).unwrap();
    let w = best(iterations, || {
        black_box(serde_json::to_vec(&tree).unwrap());
    });
    println!("serde_json parse {} MB/s  write {} MB/s", mbps(n, iterations, p), mbps(n, iterations, w));

    // simd-json: OwnedValue (the copy it parses in place is counted)
    let p = best(iterations, || {
        let mut copy = data.clone();
        let v = simd_json::to_owned_value(&mut copy).unwrap();
        black_box(v);
    });
    let mut copy = data.clone();
    let tree = simd_json::to_owned_value(&mut copy).unwrap();
    let w = best(iterations, || {
        black_box(simd_json::to_vec(&tree).unwrap());
    });
    println!("simd-json  parse {} MB/s  write {} MB/s", mbps(n, iterations, p), mbps(n, iterations, w));

    // sonic-rs: Value
    let p = best(iterations, || {
        let v: sonic_rs::Value = sonic_rs::from_slice(&data).unwrap();
        black_box(v);
    });
    let tree: sonic_rs::Value = sonic_rs::from_slice(&data).unwrap();
    let w = best(iterations, || {
        black_box(sonic_rs::to_vec(&tree).unwrap());
    });
    println!("sonic-rs   parse {} MB/s  write {} MB/s", mbps(n, iterations, p), mbps(n, iterations, w));
}
