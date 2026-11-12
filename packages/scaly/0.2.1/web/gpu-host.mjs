// The graphics card of a Scaly program that runs in a web page.
//
// A program built with `--gpu` for wasm32 carries the kernel of a parallel
// `for` in WGSL and asks its host, where such a loop begins, whether the
// card runs it (scaly/gpu/gpu_wasm.c: the import module "scaly_gpu"). This
// module is that host. The program asks and WAITS -- it is in the middle of
// a loop --, while WebGPU answers through promises; so the program runs in a
// worker, the card belongs to a second one (gpu-worker.mjs), and this module,
// on the program's side, posts a run there and sleeps on a shared word until
// the outputs are behind it.
//
//   // in the worker the program runs in
//   import { Wasi } from './wasi.mjs';
//   import { GpuHost } from './gpu-host.mjs';
//   const gpu = await GpuHost.open(new URL('./gpu-worker.mjs', import.meta.url));
//   const wasi = new Wasi({ args, files, gpu });        // null: no card, every loop on the processor
//   wasi.start(await WebAssembly.instantiate(module, wasi.imports()));
//   gpu.report()    // per loop: how often it went where, what the card's runs cost
//
// open() answers null where this cannot work: outside a worker (a page's
// main thread may not sleep), on a page that is not cross-origin isolated
// (no SharedArrayBuffer: the page must be served with
// Cross-Origin-Opener-Policy: same-origin and Cross-Origin-Embedder-Policy:
// require-corp), or where the browser has no WebGPU. Instead of a URL it
// takes a Worker the page made from gpu-worker.mjs, or a RELAY: an object
// with `postMessage(message, transfer)`, which the page passes on to such a
// worker of its own, and `hello`, the first answer that worker gave
// ({ card, why }). The card then outlives the program's worker. (A
// MessagePort between the two workers would be shorter -- but Safari
// delivered a message on one a second late, every other run.)
//
// WHO DECIDES is this module, as the native host does (gpu_darwin.c, whose
// account this follows line by line -- but for the cap on a run that stalled,
// in run(): a native card does not stall). `mode`:
//
//   'on'    the card is tried at every loop's head, and nowhere else
//   'off'   every loop stays on the processor
//   'auto'  (the default) the loop asks where the processor has measured a
//           round and is about to go on with the rest: there the card takes
//           the loop where it pays -- its time for all rounds, as measured
//           at this loop before, against the processor's for what is left.
//           A loop the card has not run is tried where the work left
//           outweighs a compilation and a call; that run is the measurement.
//           The processor's side is measured too: every run a gate kept
//           tells how long the rest took -- in any mode, a run on the cores
//           alone as well, since the processor is the same --, and that,
//           shared out over the cores the gate spoke of, is what the card is
//           weighed against from then on. (The first rounds alone, which the
//           gate measured, can be far cheaper than the rest: the top rows of
//           a Mandelbrot picture.)
//
// `mode` may be changed between two runs of a program, and during one.
// `trace`, a function, is told one line per run or refusal.
//
// All or nothing, as everywhere: the program's memory is written only where
// the card ran every round.

// What the card is believed to cost before it ran a loop: a kernel's
// compilation and one call -- the way to the other worker and back, the
// buffers there and here. Its rounds count as nothing until measured.
const COMPILE_NS = 30e6;
const CALL_NS = 1e6;
// the card must win by a quarter
const MARGIN = 1.25;
// a card that lost is asked again after this many runs, times how far behind
const AGAIN = 32;
// a run the card's worker does not answer in this time is given up, and the card with it
const PATIENCE_MS = 120000;
// where the outputs begin in a run's shared buffer (gpu-worker.mjs says the same)
const HEAD = 40;
const now = () => performance.timeOrigin + performance.now();

const decoder = new TextDecoder();

export class GpuHost {
  // Where it answers null, GpuHost.why says why.
  static async open(source, options = {}) {
    GpuHost.why = '';
    if (typeof SharedArrayBuffer === 'undefined' || typeof Atomics === 'undefined') {
      GpuHost.why = 'no SharedArrayBuffer (the page is not cross-origin isolated)';
      return null;
    }
    if (typeof WorkerGlobalScope === 'undefined' || !(self instanceof WorkerGlobalScope)) {
      GpuHost.why = 'not in a worker';
      return null;
    }
    let worker = source;
    if (typeof source === 'string' || source instanceof URL) {
      if (typeof Worker === 'undefined') { GpuHost.why = 'no worker can be made from a worker here'; return null; }
      worker = new Worker(source, { type: 'module' });
    }
    let has;
    if (worker.hello) {
      // a relay: the page's card worker has answered already
      GpuHost.why = worker.hello.why || '';
      has = worker.hello.card === true;
    } else {
      has = await new Promise(resolve => {
        worker.onmessage = event => {
          GpuHost.why = event.data && event.data.why || '';
          resolve(event.data && event.data.card === true);
        };
        worker.onerror = e => { GpuHost.why = `the card's worker failed: ${e && e.message || e}`; resolve(false); };
      });
      worker.onmessage = null;
    }
    if (!has) {
      worker.terminate?.();
      return null;
    }
    return new GpuHost(worker, options);
  }

  constructor(worker, { mode = 'auto', trace = null } = {}) {
    this.worker = worker;
    this.mode = mode;
    this.trace = trace;
    this.sites = new Map();
    this.lost = false;             // the card's worker did not answer
    // the shortest run seen: what a call costs whatever it computes
    this.least_ns = CALL_NS;
    // what compiling a kernel is believed to cost: the last one compiled
    this.compile_ns = COMPILE_NS;
  }

  // The functions of the import module, over the memory of the instance
  // that calls them (asked at each call: it is there only after the
  // instantiation).
  imports(memory) {
    return {
      forced: () => this.mode === 'on' && !this.lost ? 1 : 0,
      // addresses unsigned: past 2 GB a wasm i32 arrives negative
      run: request => this.run(memory(), request >>> 0),
      kept: (wgsl, where, rounds, ns) => this.kept(memory(), wgsl >>> 0, where >>> 0, rounds, ns),
    };
  }

  say(line) {
    if (this.trace) this.trace('gpu: ' + line);
  }

  site(memory, wgsl_at, where_at) {
    const wgsl = text_at(memory, wgsl_at);
    const where = text_at(memory, where_at);
    const key = where + '\n' + wgsl;
    let site = this.sites.get(key);
    if (!site) {
      site = {
        wgsl, where,
        refused: false,            // the card does not take this kernel
        stopped: false,            // a round met what the processor stops at
        measured: 0,               // card runs that count: 0, 1 (the first, which pays the compilation), 2
        fixed_ns: 0, round_ns: 0,  // the card's time: per call, per round
        cards: 0, card_rounds: 0, card_ns: 0,
        compile_ns: 0,             // of card_ns: compiling the kernel, once
        kept: 0, stops: 0,
        kept_ns: 0,                // the processor's work this loop brought to the gate and kept
        cpu_round_ns: 0,           // ONE core's time for a round, as it took (0: not told yet)
        gate_workers: 0,           // the cores the gate last spoke of: a kept run's time is theirs
        kept_since: 0,             // runs kept since the card last ran
        // of card_ns: the way to the card's worker, the browser's word on
        // errors, the results (the card's work in it), the way back
        there_ns: 0, errors_ns: 0, read_ns: 0, back_ns: 0,
      };
      this.sites.set(key, site);
    }
    return site;
  }

  // Does the card pay for this loop, as far as is known? (gpu_pays)
  pays(site, count, per1k, done, workers) {
    if (site.refused || site.stopped) return false;
    const left = count - done;
    const cores = workers > 0 ? workers : 1;
    let cpu_ns = left * (per1k / 1024) / cores;
    // what the processor took for a round the last times it was kept
    if (site.cpu_round_ns > 0) cpu_ns = left * site.cpu_round_ns / cores;
    if (site.measured === 1) return true;      // tried once: the run that measures comes now
    let gpu_ns;
    if (site.measured) {
      gpu_ns = site.fixed_ns + site.round_ns * count;
    } else {
      gpu_ns = this.compile_ns + CALL_NS;
      if (gpu_ns * MARGIN < cpu_ns + site.kept_ns) return true;
    }
    if (gpu_ns * MARGIN < cpu_ns) return true;
    site.kept_ns += cpu_ns;
    // lost, as measured -- and asked again after a number of runs that
    // grows with how clearly
    if (site.measured && cpu_ns > 0) {
      site.kept_since++;
      if (site.kept_since >= AGAIN * gpu_ns / cpu_ns) return true;
    }
    return false;
  }

  // A loop a gate kept is through: `rounds` of it took the processor `ns`,
  // on the cores the gate spoke of. In every mode: the processor is the
  // same, and the runs on the cores are what teaches the next 'auto' one.
  kept(memory, wgsl_at, where_at, rounds, ns) {
    if (!(rounds > 0) || !(ns > 0)) return;
    const site = this.site(memory, wgsl_at, where_at);
    const round = ns / rounds * Math.max(1, site.gate_workers);
    site.cpu_round_ns = site.cpu_round_ns > 0 ? (site.cpu_round_ns + round) / 2 : round;
  }

  // The request of scaly/gpu/gpu_wasm.c, at its address in the program's memory.
  run(memory, request) {
    const view = new DataView(memory.buffer);
    const u32 = at => view.getUint32(request + at, true);
    const i64 = at => Number(view.getBigInt64(request + at, true));
    const per1k = i64(40);
    const at_gate = per1k > 0;
    // the cores the gate speaks of, whatever it decides: a kept run's time
    // is shared by them (kept)
    if (at_gate && !this.lost) this.site(memory, u32(0), u32(4)).gate_workers = i64(56);
    // off: nowhere. on: at the head, which has asked already where this is
    // the gate. Otherwise: at the gate.
    if (this.lost || this.mode === 'off' || (this.mode === 'on') === at_gate) return 0;
    const site = this.site(memory, u32(0), u32(4));
    const count = i64(32);
    if (at_gate && !this.pays(site, count, per1k, i64(48), i64(56))) {
      site.kept++;
      this.say('processor (the card would not pay)');
      return 0;
    }

    const n_in = u32(24), n_out = u32(28);
    const buffer_at = i => view.getUint32(u32(12) + 4 * i, true);
    const elements = i => Number(view.getBigUint64(u32(16) + 16 * i, true));
    const words = copy_words(memory, u32(8), u32(20));
    const inputs = [], outputs = [];
    for (let i = 0; i < n_in; i++) inputs.push(copy_words(memory, buffer_at(i), elements(i)));
    for (let i = 0; i < n_out; i++) outputs.push(copy_words(memory, buffer_at(n_in + i), count));
    const data = HEAD + n_out * count * 4;
    const shared = new SharedArrayBuffer(data + 256);
    const state = new Int32Array(shared, 0, 2);

    const begun = performance.now(), posted = now();
    this.worker.postMessage({ wgsl: site.wgsl, count, words, inputs, outputs, shared },
                            [words.buffer, ...inputs.map(b => b.buffer), ...outputs.map(b => b.buffer)]);
    if (Atomics.wait(state, 0, 0, PATIENCE_MS) === 'timed-out') {
      this.lost = true;
      this.say('processor (the card did not answer)');
      return 0;
    }
    const all_ns = (performance.now() - begun) * 1e6, woken = now();

    if (state[0] !== 1) {
      const why = decoder.decode(new Uint8Array(shared, data, state[1]).slice());
      site.stops++;
      if (state[0] === 3) site.stopped = true;
      else if (why.startsWith('no kernel')) site.refused = true;
      this.say('processor (' + why + ')');
      return 0;
    }
    const mem = new Uint8Array(memory.buffer);
    for (let i = 0; i < n_out; i++)
      mem.set(new Uint8Array(shared, HEAD + i * count * 4, count * 4), buffer_at(n_in + i));
    const [arrived, through, errors_ms, read_ms] = new Float64Array(shared, 8, 4);
    site.there_ns += Math.max(0, arrived - posted) * 1e6;
    site.errors_ns += errors_ms * 1e6;
    site.read_ns += read_ms * 1e6;
    site.back_ns += Math.max(0, woken - through) * 1e6;

    // a part for the call, the rest by the round; halfway between this run
    // and the ones before. A kernel's first run pays what no later one
    // does: the second run is the first measurement, and stands alone.
    if (site.measured === 0) {
      this.compile_ns = Math.max(0, all_ns - this.least_ns);
    } else {
      if (all_ns < this.least_ns) this.least_ns = all_ns;
      let fixed = this.least_ns;
      let round = (all_ns - fixed) / count;
      // a run far slower than the ones before teaches at most twice what
      // they did: a browser's card can stall now and then (Safari, for a
      // second), and one such run would keep the loop off the card for
      // dozens of runs to come. A card that is slow for good still shows,
      // twice as slow with every run.
      if (site.measured >= 2 && site.round_ns > 0) round = Math.min(round, 2 * site.round_ns);
      if (site.measured >= 2) {
        fixed = (fixed + site.fixed_ns) / 2;
        round = (round + site.round_ns) / 2;
      }
      site.fixed_ns = fixed;
      site.round_ns = round;
    }
    if (site.measured < 2) site.measured++;
    site.cards++;
    site.kept_since = 0;
    site.card_rounds += count;
    site.card_ns += all_ns;
    site.compile_ns += state[1] * 1000;
    this.say('card (' + count + ' rounds)');
    return 1;
  }

  // Per loop the program brought here: how often it went where, and what
  // the card's runs cost.
  report() {
    return [...this.sites.values()].map(s => ({
      where: s.where, card: s.cards, card_rounds: s.card_rounds, card_ms: s.card_ns / 1e6,
      compile_ms: s.compile_ns / 1e6,
      there_ms: s.there_ns / 1e6, errors_ms: s.errors_ns / 1e6, read_ms: s.read_ns / 1e6, back_ms: s.back_ns / 1e6,
      processor: s.kept, given_back: s.stops,
    }));
  }
}

// the text that ends with a zero byte at an address
function text_at(memory, at) {
  const mem = new Uint8Array(memory.buffer);
  let end = at;
  while (mem[end] !== 0) end++;
  return decoder.decode(mem.slice(at, end));   // a copy: a shared memory is not decoded
}

// `count` 32-bit words at an address, as an array of its own (wherever the
// address lies: a typed array over the memory would want it divisible by four)
function copy_words(memory, at, count) {
  return new Uint32Array(new Uint8Array(memory.buffer, at, count * 4).slice().buffer);
}
