// The worker that owns a page's graphics card for a Scaly program's
// CardVectors (scaly/card.scaly; the program's side: card-host.mjs).
//
// It holds the card's buffers by the ids the program gave them, and each
// kernel's pipeline, compiled once per text. A FLUSH is the program's list,
// done in order:
//
//   create   a buffer of that many bytes, zero (at least four: WebGPU binds
//            nothing smaller)
//   write    bytes into a buffer
//   load     bytes of a file the page handed over (hello) into a buffer
//   run      a kernel over its vectors: words of its scalars (then, as the
//            per-loop host lays them out, two words per input -- its length,
//            low and high --, the count of rounds and the rounds of a row),
//            the fault word, the inputs, then the outputs
//
// Runs are recorded into one command encoder; a write or a load submits what
// was recorded before it, since the queue's writes would otherwise overtake
// the runs listed ahead of them. Then the asked range of the asked buffer is
// copied out, every run's fault word with it, and the program is woken: word
// 0 of `shared` 1 with the bytes behind the head, 2 with why it did not run
// (its length in word 1) -- a round that met an index beyond its vector, or
// what WebGPU said.
//
// The first message is { type: 'hello', files }; the answer says whether the
// page has a card: { card, why, limits }.

const GROUP = 64;
const ROW = 65535;
const HEAD = 8;
// a run's fault word, at its own offset in one buffer per flush
const FAULT_STRIDE = 256;

let device = null;
const files = new Map();
const buffers = new Map();
const pipelines = new Map();
const encoder = new TextEncoder();

async function hello(data) {
  for (const [path, buffer] of data.files || []) files.set(path, buffer);
  if (typeof navigator === 'undefined' || !navigator.gpu)
    return { card: false, why: 'no WebGPU in a worker (navigator.gpu)' };
  let adapter;
  try { adapter = await navigator.gpu.requestAdapter(); }
  catch (e) { return { card: false, why: `requestAdapter failed: ${e && e.message || e}` }; }
  if (!adapter) return { card: false, why: 'WebGPU gave no adapter (requestAdapter answered null)' };
  // the adapter's own limits: a language model's embedding is larger than
  // WebGPU's default largest buffer
  const most = adapter.limits;
  const limits = {
    maxBufferSize: most.maxBufferSize,
    maxStorageBufferBindingSize: most.maxStorageBufferBindingSize,
    maxStorageBuffersPerShaderStage: most.maxStorageBuffersPerShaderStage,
  };
  try { device = await adapter.requestDevice({ requiredLimits: limits }); }
  catch (e) { return { card: false, why: `requestDevice failed: ${e && e.message || e}` }; }
  return { card: true, why: '', limits };
}

function pipeline(wgsl) {
  let p = pipelines.get(wgsl);
  if (!p) {
    const module = device.createShaderModule({ code: wgsl });
    p = device.createComputePipeline({ layout: 'auto', compute: { module, entryPoint: 'k' } });
    pipelines.set(wgsl, p);
  }
  return p;
}

const STORAGE = () => GPUBufferUsage.STORAGE | GPUBufferUsage.COPY_SRC | GPUBufferUsage.COPY_DST;
const size4 = n => Math.max(4, (n + 3) & ~3);

function filled(bytes) {
  const b = device.createBuffer({ size: size4(bytes.byteLength), usage: STORAGE(), mappedAtCreation: true });
  new Uint8Array(b.getMappedRange()).set(bytes);
  b.unmap();
  return b;
}

async function flush({ ops, read, shared }) {
  const state = new Int32Array(shared, 0, 2);
  const answer = (code, why) => {
    if (code === 2) {
      const text = encoder.encode(why).subarray(0, shared.byteLength - HEAD);
      new Uint8Array(shared, HEAD, text.length).set(text);
      state[1] = text.length;
    }
    Atomics.store(state, 0, code);
    Atomics.notify(state, 0);
  };
  const runs = ops.filter(o => o.op === 'run');
  const faults = device.createBuffer({ size: Math.max(1, runs.length) * FAULT_STRIDE, usage: STORAGE() });
  const temporary = [faults];
  const sites = [];
  device.pushErrorScope('validation');
  device.pushErrorScope('out-of-memory');
  let recording = null;
  const submit = () => {
    if (recording) device.queue.submit([recording.finish()]);
    recording = null;
  };
  try {
    for (const o of ops) {
      if (o.op === 'create') {
        buffers.set(o.id, device.createBuffer({ size: size4(o.bytes), usage: STORAGE() }));
      } else if (o.op === 'write') {
        submit();
        device.queue.writeBuffer(buffers.get(o.id), o.at, o.data);
      } else if (o.op === 'load') {
        submit();
        const file = files.get(o.path);
        const target = buffers.get(o.id);
        // in pieces: a write takes no view of a shared buffer everywhere,
        // and a copy of 500 MB at once is more than a worker needs to hold
        const PIECE = 1 << 26;
        for (let done = 0; done < o.bytes; done += PIECE) {
          const n = Math.min(PIECE, o.bytes - done);
          device.queue.writeBuffer(target, o.at + done, new Uint8Array(file, o.offset + done, n).slice());
        }
      } else if (o.op === 'run') {
        const index = sites.length;
        sites.push(o.where);
        const groups = Math.ceil(o.count / GROUP);
        const across = Math.min(groups, ROW);
        const rows = Math.ceil(groups / across);
        const words = new Uint32Array(o.words.length + 2 * o.n_in + 2);
        words.set(o.words);
        for (let i = 0; i < o.n_in; i++) words[o.words.length + 2 * i] = o.slots[i].length;
        words[words.length - 2] = o.count;
        words[words.length - 1] = across * GROUP;
        const wordsBuffer = filled(new Uint8Array(words.buffer));
        temporary.push(wordsBuffer);
        const entries = [
          { binding: 0, resource: { buffer: wordsBuffer } },
          { binding: 1, resource: { buffer: faults, offset: index * FAULT_STRIDE, size: 4 } },
        ];
        o.slots.forEach((s, i) => {
          let b;
          if (s.id) b = buffers.get(s.id);
          else {
            b = filled(s.data);
            temporary.push(b);
          }
          entries.push({ binding: i + 2, resource: { buffer: b } });
        });
        const p = pipeline(o.wgsl);
        const group = device.createBindGroup({ layout: p.getBindGroupLayout(0), entries });
        recording ??= device.createCommandEncoder();
        const pass = recording.beginComputePass();
        pass.setPipeline(p);
        pass.setBindGroup(0, group);
        pass.dispatchWorkgroups(across, rows);
        pass.end();
      }
    }
    // the asked range, and every run's fault word
    recording ??= device.createCommandEncoder();
    const bytes = size4(read.bytes);
    const out = device.createBuffer({ size: bytes, usage: GPUBufferUsage.MAP_READ | GPUBufferUsage.COPY_DST });
    const flt = device.createBuffer({ size: Math.max(1, runs.length) * FAULT_STRIDE, usage: GPUBufferUsage.MAP_READ | GPUBufferUsage.COPY_DST });
    temporary.push(out, flt);
    recording.copyBufferToBuffer(buffers.get(read.id), read.at, out, 0, bytes);
    recording.copyBufferToBuffer(faults, 0, flt, 0, Math.max(1, runs.length) * FAULT_STRIDE);
    submit();
    const oom = await device.popErrorScope();
    const invalid = await device.popErrorScope();
    if (invalid || oom) {
      answer(2, `the card refused a run: ${(invalid || oom).message}`);
      return;
    }
    await Promise.all([out.mapAsync(GPUMapMode.READ), flt.mapAsync(GPUMapMode.READ)]);
    const fault = new Uint32Array(flt.getMappedRange());
    for (let i = 0; i < sites.length; i++) {
      if (fault[i * FAULT_STRIDE / 4] !== 0) {
        answer(2, `a round of the loop at ${sites[i]} met an index beyond its vector`);
        return;
      }
    }
    new Uint8Array(shared, HEAD, read.bytes).set(new Uint8Array(out.getMappedRange(), 0, read.bytes));
    answer(1, '');
  } catch (e) {
    answer(2, `the card's host failed: ${e && e.message || e}`);
  } finally {
    for (const b of temporary) b.destroy();
  }
}

// one flush after the other: a flush's buffers are made by those before it
let queue = Promise.resolve();
self.onmessage = ({ data }) => {
  if (data.type === 'hello') {
    hello(data).then(h => self.postMessage(h));
    return;
  }
  if (data.type === 'flush') queue = queue.then(() => flush(data));
};
