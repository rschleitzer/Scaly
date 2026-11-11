// The threads of a Scaly program built for wasm32-wasip1-threads, on a
// page's or node's workers.
//
// Such a program (`scaly build --target wasm32-wasip1-threads`) runs its
// parallel loops on more than one core: a thread is a NEW INSTANCE of the
// module over the SAME memory (wasi-threads), started where the program asks
// for one (the import `wasi.thread-spawn`, which wasi-libc's pthread_create
// calls) and entered at its export `wasi_thread_start(tid, arg)`. The memory
// is the host's -- imported, shared, made here from what the module asks for.
//
// THREE KINDS OF PLACE take part, and one of them must never block:
//
//   the coordinator  the page, or node's main thread: it makes every
//                    worker. A thread waiting for its siblings blocks
//                    (Atomics.wait), and a worker a blocked thread made
//                    would never start -- so no thread makes one; it asks
//                    the coordinator, whose event loop is always free.
//                    `Threads` is its part.
//   the program      a worker the coordinator made: the program's main
//                    thread, with the files and the standard streams
//                    (wasi.mjs). Its Wasi takes `threads`, a ThreadHost.
//   a thread         a worker the coordinator made from thread.mjs: an
//                    instance of the module that runs one thread, and then
//                    the next one the coordinator gives it.
//
//   // the coordinator (a page; node: packages/scaly/<version>/web/run.mjs)
//   const threads = await Threads.create({ out, err, exit });
//   program.onmessage = ({ data }) => { if (threads.handle(data)) return; ... };
//   // the program's worker
//   const memory = threadMemory(bytes);                    // null: no threads in this module
//   const wasi = new Wasi({ ..., env: { SCALY_CPUS: n }, threads: new ThreadHost({ memory, post: m => postMessage(m) }) });
//   wasi.start(await WebAssembly.instantiate(module, wasi.imports()));
//
// SCALY_CPUS tells the program how many cores the host gives it -- WASI has
// no question for it, and without it every loop runs inline on one.
//
// A thread's output goes to the coordinator (`out`, `err`), as it is written.
// Where any thread ends the program (proc_exit: a fault no catch point
// took), the coordinator hears `exit` with the status and ends every worker:
// the other threads may be waiting for the one that ended.

// The limits of the memory a module imports (`env.memory`), read from its
// bytes -- the JavaScript API does not tell them: null where it imports none.
export function memoryImport(bytes) {
  const b = bytes instanceof Uint8Array ? bytes : new Uint8Array(bytes);
  let at = 8;
  const leb = () => {
    let v = 0, shift = 0, x;
    do { x = b[at++]; v += (x & 0x7f) * 2 ** shift; shift += 7; } while (x & 0x80);
    return v;
  };
  const name = () => {
    const n = leb();
    const s = new TextDecoder().decode(b.subarray(at, at + n));
    at += n;
    return s;
  };
  while (at < b.length) {
    const id = b[at++], size = leb(), end = at + size;
    if (id === 2) {
      for (let count = leb(); count > 0; count--) {
        const module = name(), field = name(), kind = b[at++];
        if (kind === 0) { leb(); continue; }                       // a function: its type
        if (kind === 1) { at++; const f = leb(); leb(); if (f & 1) leb(); continue; }   // a table
        if (kind === 3) { at += 2; continue; }                      // a global
        if (kind === 4) { at++; leb(); continue; }                  // a tag
        const flags = leb(), initial = leb(), maximum = flags & 1 ? leb() : undefined;
        if (module === 'env' && field === 'memory') return { initial, maximum, shared: (flags & 2) !== 0 };
      }
      return null;
    }
    at = end;
  }
  return null;
}

// The memory of a module built for threads, as it asks for it; null where
// the module imports none (a program built for wasm32-wasip1).
export function threadMemory(bytes) {
  const limits = memoryImport(bytes);
  if (!limits || !limits.shared) return null;
  return new WebAssembly.Memory({ initial: limits.initial, maximum: limits.maximum, shared: true });
}

// What one instance of the module -- the program's or a thread's -- imports
// for its threads: the memory, and the start of a new thread, which goes to
// the coordinator by `post`. `counter` numbers the threads across all of
// them (a SharedArrayBuffer of one word; made here where not handed in).
export class ThreadHost {
  constructor({ memory, post, counter = null }) {
    this.memory = memory;
    this.post = post;
    this.counter = counter ?? new SharedArrayBuffer(4);
    this.module = null;      // the module, once its first instance is made
  }

  imports(module) {
    this.module = module;
    return {
      env: { memory: this.memory },
      wasi: {
        'thread-spawn': arg => {
          // a positive id below 2^29, as wasi-threads asks; the thread starts
          // when the coordinator has a worker for it
          const tid = Atomics.add(new Int32Array(this.counter), 0, 1) + 1;
          if (tid >= 1 << 29) return -1;
          this.post({ type: 'scaly-thread', module: this.module, memory: this.memory, counter: this.counter, tid, arg });
          return tid;
        },
      },
    };
  }
}

// The coordinator's part: the workers of the threads, made where a thread is
// asked for and kept for the next one. `out` and `err` take a thread's
// bytes, `exit` the status where a thread ended the program. `env` is what a
// thread's environment holds (as the program's, which wasi-libc may read in
// a thread first). `script` is thread.mjs.
export class Threads {
  static async create({ out, err, exit, env = {}, script = new URL('./thread.mjs', import.meta.url) }) {
    let make;
    if (typeof Worker !== 'undefined') {
      make = () => {
        const w = new Worker(script, { type: 'module' });
        return { post: m => w.postMessage(m), listen: f => { w.onmessage = e => f(e.data); }, end: () => w.terminate() };
      };
    } else {
      const { Worker: NodeWorker } = await import('node:worker_threads');
      make = () => {
        const w = new NodeWorker(script);
        w.unref();     // a waiting thread does not keep node running
        return { post: m => w.postMessage(m), listen: f => w.on('message', f), end: () => w.terminate() };
      };
    }
    return new Threads(make, { out, err, exit, env });
  }

  constructor(make, { out, err, exit, env }) {
    this.make = make;
    this.out = out;
    this.err = err;
    this.exit = exit;
    this.env = env;
    this.idle = [];
    this.all = [];
    this.started = 0;
  }

  // A message from the program's worker or from a thread: true where it was
  // one of the threads'.
  handle(data) {
    if (!data || typeof data.type !== 'string' || !data.type.startsWith('scaly-thread')) return false;
    switch (data.type) {
      case 'scaly-thread': {
        let worker = this.idle.pop();
        if (!worker) {
          worker = this.make();
          worker.listen(m => {
            if (m && m.type === 'scaly-thread-done') this.idle.push(worker);
            else this.handle(m);
          });
          this.all.push(worker);
        }
        this.started++;
        const { module, memory, counter, tid, arg } = data;
        worker.post({ module, memory, counter, tid, arg, env: this.env });
        break;
      }
      case 'scaly-thread-out': this.out(data.bytes); break;
      case 'scaly-thread-err': this.err(data.bytes); break;
      case 'scaly-thread-exit': this.exit(data.status); break;
      case 'scaly-thread-crash':
        this.err(new TextEncoder().encode(`[a thread stopped: ${data.text}]\n`));
        this.exit(134);
        break;
    }
    return true;
  }

  // End every thread's worker: the program is over.
  end() {
    for (const w of this.all) w.end();
    this.all = [];
    this.idle = [];
  }
}
