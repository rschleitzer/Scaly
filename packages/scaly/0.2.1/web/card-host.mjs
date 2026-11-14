// Where a Scaly program's CardVectors live in a web page: the graphics card
// (scaly/card.scaly).
//
// The program runs in a worker and calls its host synchronously
// (scaly/card/card_wasm.c: the import module "scaly_card"); WebGPU answers
// through promises, so the card belongs to a second worker
// (card-worker.mjs). This module, on the program's side, is the host: it
// LISTS what the program asks -- a vector made, bytes written, a vector
// filled from a file, a kernel's run -- and waits for nothing, until the
// program reads a vector. Then the whole list goes to the card's worker in
// ONE message, which runs it as one command buffer (split only where a
// write must come between runs), and the program sleeps on a shared word
// until the bytes it read are behind it. A token of a language model is one
// way to the card and back, not one per loop.
//
//   // in the worker the program runs in
//   import { Wasi } from './wasi.mjs';
//   import { CardHost } from './card-host.mjs';
//   const card = await CardHost.open(new URL('./card-worker.mjs', import.meta.url), { files });
//   const wasi = new Wasi({ args, files: memoryFiles, card });   // card null: in memory
//
// open() answers null where this cannot work: outside a worker (a page's
// main thread may not sleep), without SharedArrayBuffer (the page must be
// cross-origin isolated), or where the browser gives the page no card --
// CardHost.why says which. Instead of a URL it takes a Worker made from
// card-worker.mjs. `files`: a Map of path -> ArrayBuffer or
// SharedArrayBuffer whose bytes CardVector.from_file fills a vector from on
// the card's side; a SharedArrayBuffer is shared, not copied. `trace`, a
// function, is told after every flush what it was: { ms, runs, loads, mb }.
// `adapter` says which card the page got ({ vendor, architecture,
// description, fallback }).

const decoder = new TextDecoder();
// how many bytes the answer of a read may hold beyond its head
const HEAD = 24;

export class CardHost {
  static async open(source, { files = new Map(), trace = null } = {}) {
    CardHost.why = '';
    if (typeof SharedArrayBuffer === 'undefined' || typeof Atomics === 'undefined') {
      CardHost.why = 'no SharedArrayBuffer (the page is not cross-origin isolated)';
      return null;
    }
    if (typeof WorkerGlobalScope === 'undefined' || !(self instanceof WorkerGlobalScope)) {
      CardHost.why = 'not in a worker (a page\'s main thread may not wait)';
      return null;
    }
    const worker = typeof source === 'string' || source instanceof URL ? new Worker(source, { type: 'module' }) : source;
    const hello = await new Promise(resolve => {
      worker.onmessage = ({ data }) => resolve(data);
      worker.onerror = e => resolve({ card: false, why: `the card's worker failed: ${e.message || e}` });
      worker.postMessage({ type: 'hello', files: [...files.entries()] });
    });
    if (!hello.card) {
      CardHost.why = hello.why || 'no card';
      return null;
    }
    return new CardHost(worker, new Set(files.keys()), hello, trace);
  }

  constructor(worker, paths, hello, trace) {
    this.worker = worker;
    this.paths = paths;
    this.limits = hello.limits;
    this.adapter = hello.adapter || {};
    this.trace = trace;
    this.next = 1;
    this.ops = [];
    this.transfer = [];
    this.texts = new Map();   // address of a constant C string -> its text
    this.flushes = 0;
    this.runs = 0;
  }

  text(memory, ptr) {
    let t = this.texts.get(ptr);
    if (t !== undefined) return t;
    const bytes = new Uint8Array(memory.buffer);
    let end = ptr;
    while (bytes[end] !== 0) end++;
    t = decoder.decode(bytes.slice(ptr, end));
    this.texts.set(ptr, t);
    return t;
  }

  copy(memory, ptr, bytes) {
    const c = new Uint8Array(new ArrayBuffer(bytes));
    c.set(new Uint8Array(memory.buffer, ptr, bytes));
    this.transfer.push(c.buffer);
    return c;
  }

  imports(getMemory) {
    const u = x => x >>> 0;
    return {
      available: () => 1,
      create: bytes => {
        const id = this.next++;
        this.ops.push({ op: 'create', id, bytes: u(bytes) });
        return id;
      },
      write: (id, at, ptr, bytes) => {
        this.ops.push({ op: 'write', id: u(id), at: u(at), data: this.copy(getMemory(), u(ptr), u(bytes)) });
      },
      load: (id, at, path, length, offset, bytes) => {
        const memory = getMemory();
        const p = decoder.decode(new Uint8Array(memory.buffer, u(path), u(length)).slice());
        if (!this.paths.has(p)) return 0;
        this.ops.push({ op: 'load', id: u(id), at: u(at), path: p, offset: u(offset), bytes: u(bytes) });
        return 1;
      },
      run: request => {
        const memory = getMemory();
        const v = new DataView(memory.buffer);
        const r = u(request);
        const wgsl = this.text(memory, v.getUint32(r, true));
        const where = this.text(memory, v.getUint32(r + 4, true));
        const words_at = v.getUint32(r + 8, true);
        const n_words = v.getUint32(r + 12, true);
        const n_in = v.getUint32(r + 16, true);
        const n_out = v.getUint32(r + 20, true);
        const count = v.getUint32(r + 24, true);
        const slots_at = v.getUint32(r + 28, true);
        const words = new Uint32Array(n_words);
        words.set(new Uint32Array(memory.buffer, words_at, n_words));
        this.transfer.push(words.buffer);
        const slots = [];
        for (let i = 0; i < n_in + n_out; i++) {
          const at = slots_at + 16 * i;
          const address = v.getUint32(at, true), length = v.getUint32(at + 4, true);
          const element = v.getUint32(at + 8, true), id = v.getUint32(at + 12, true);
          // an ordinary vector the loop reads goes with the run, copied now
          slots.push({ id, length, data: id ? null : this.copy(memory, address, length * element) });
        }
        this.ops.push({ op: 'run', wgsl, where, words, count, n_in, n_out, slots });
        this.runs++;
      },
      read: (id, at, ptr, bytes, why, capacity) => {
        const memory = getMemory();
        const n = u(bytes);
        const shared = new SharedArrayBuffer(HEAD + Math.max(n, u(capacity)));
        const state = new Int32Array(shared, 0, 6);
        const ops = this.ops, transfer = this.transfer;
        this.ops = [];
        this.transfer = [];
        this.flushes++;
        this.worker.postMessage({ type: 'flush', ops, read: { id: u(id), at: u(at), bytes: n }, shared }, transfer);
        Atomics.wait(state, 0, 0);
        if (this.trace) this.trace({ ms: state[2], runs: state[3], loads: state[4], mb: state[5] });
        if (state[0] === 1) {
          new Uint8Array(memory.buffer, u(ptr), n).set(new Uint8Array(shared, HEAD, n));
          return 0;
        }
        const length = Math.min(state[1], u(capacity));
        new Uint8Array(memory.buffer, u(why), length).set(new Uint8Array(shared, HEAD, length));
        return length || 1;
      },
    };
  }
}
