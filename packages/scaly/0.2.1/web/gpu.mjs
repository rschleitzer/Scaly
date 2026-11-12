// A kernel of a Scaly program on the graphics card of a web page: WebGPU.
//
// A program compiled with --gpu carries the kernel of a parallel `for` in
// WGSL (the compiler's GpuMsl, `scalyc --gpu-wgsl` prints it). This module
// runs one such kernel through the page's own WebGPU, the way the native
// host does through wgpu-native (scaly/gpu/gpu_darwin.c): the same bindings,
// the same words, the same answer.
//
//   import { Card } from './gpu.mjs';
//   const card = await Card.open();            // null where the page has no WebGPU
//   const answer = await card.run({ wgsl, count, words, inputs, outputs });
//   answer.ran       // every round ran and `outputs` hold the results
//   answer.stopped   // a round met what the processor would stop at
//   answer.why       // where it did not run, why
//
// `words` is a Uint32Array, the kernel's scalars as the program lays them
// out; `inputs` and `outputs` are arrays of Uint32Array, Int32Array or
// Float32Array, an output `count` elements long and holding what it held
// before (a round that writes nothing leaves it). Behind the words this
// module appends, as the native host does, two words per input -- its
// element count, low and high -- and then the count of rounds and how many
// rounds one row of the dispatch holds: a card under WebGPU runs whole groups
// of 64, in rows of at most 65535 groups.
//
// Bindings, all in group 0: 0 the words, 1 the fault word, 2.. the inputs,
// then the outputs. The kernel's entry is `k`.
//
// All or nothing: the outputs are written only where every round ran. A
// kernel is compiled once per text and kept.

const GROUP = 64;
const ROW = 65535;

export class Card {
  // The card, or null -- and then `Card.why` says why: where a page has no
  // card, that is what a reader can tell the author (an iPhone, a browser
  // whose WebGPU is off, one that has it on the page but not in a worker).
  static async open() {
    Card.why = '';
    if (typeof navigator === 'undefined' || !navigator.gpu) {
      Card.why = typeof WorkerGlobalScope !== 'undefined' && self instanceof WorkerGlobalScope
        ? 'no WebGPU in a worker (navigator.gpu)' : 'no WebGPU (navigator.gpu)';
      return null;
    }
    let adapter;
    try { adapter = await navigator.gpu.requestAdapter(); }
    catch (e) { Card.why = `requestAdapter failed: ${e && e.message || e}`; return null; }
    if (!adapter) { Card.why = 'WebGPU gave no adapter (requestAdapter answered null)'; return null; }
    // every storage buffer the card allows: the default is eight, and a
    // kernel binds two of its own beside the program's
    const most = adapter.limits.maxStorageBuffersPerShaderStage;
    let device;
    try { device = await adapter.requestDevice({ requiredLimits: { maxStorageBuffersPerShaderStage: most } }); }
    catch (e) { Card.why = `requestDevice failed: ${e && e.message || e}`; return null; }
    return new Card(device, most);
  }

  constructor(device, most) {
    this.device = device;
    this.most = most;          // storage buffers a kernel may bind
    this.kernels = new Map();  // text -> pipeline, or null where it does not compile
    this.said = [];            // what the card said of a kernel it did not take
  }

  // The compiled kernel of a text, or null. `compile_ms` is how long the
  // last call took to compile one: 0 where the text was compiled before. (A
  // browser compiles WGSL into its card's own language here -- Safari into
  // Metal, which takes it far longer than any run after.)
  async kernel(wgsl) {
    this.compile_ms = 0;
    if (this.kernels.has(wgsl)) return this.kernels.get(wgsl);
    const begun = performance.now();
    this.device.pushErrorScope('validation');
    let pipeline = null;
    try {
      const module = this.device.createShaderModule({ code: wgsl });
      pipeline = await this.device.createComputePipelineAsync({ layout: 'auto', compute: { module, entryPoint: 'k' } });
    } catch (e) {
      this.said.push(String(e && e.message || e));
      pipeline = null;
    }
    const error = await this.device.popErrorScope();
    if (error) { this.said.push(error.message); pipeline = null; }
    this.kernels.set(wgsl, pipeline);
    this.compile_ms = performance.now() - begun;
    return pipeline;
  }

  async run({ wgsl, count, words, inputs, outputs }) {
    const no = why => ({ ran: false, stopped: false, why });
    if (count <= 0) return no('no round');
    if (count >= 0x7fffffff) return no('more rounds than a kernel counts');
    if (inputs.length + outputs.length + 2 > this.most) return no('more buffers than the card binds');
    for (const b of [...inputs, ...outputs])
      if (b.BYTES_PER_ELEMENT !== 4) return no('an element webgpu does not have');
    for (const o of outputs)
      if (o.length < count) return no('an output shorter than the rounds');
    const pipeline = await this.kernel(wgsl);
    if (!pipeline) return no('no kernel');

    const groups = Math.ceil(count / GROUP);
    const across = Math.min(groups, ROW);
    const rows = Math.ceil(groups / across);
    const all = new Uint32Array(words.length + 2 * inputs.length + 2);
    all.set(words);
    inputs.forEach((b, i) => { all[words.length + 2 * i] = b.length; });
    all[all.length - 2] = count;
    all[all.length - 1] = across * GROUP;

    const device = this.device;
    const usage = GPUBufferUsage.STORAGE | GPUBufferUsage.COPY_DST | GPUBufferUsage.COPY_SRC;
    const held = [];
    const hold = (data, elements) => {
      const size = Math.max(4, elements * 4);
      const buffer = device.createBuffer({ size, usage });
      if (elements) device.queue.writeBuffer(buffer, 0, data.buffer, data.byteOffset, elements * 4);
      held.push(buffer);
      return buffer;
    };
    device.pushErrorScope('validation');
    hold(all, all.length);
    hold(new Uint32Array(1), 1);
    for (const b of inputs) hold(b, b.length);
    for (const o of outputs) hold(o, count);
    const back = 4 + outputs.length * count * 4;
    const reader = device.createBuffer({ size: back, usage: GPUBufferUsage.MAP_READ | GPUBufferUsage.COPY_DST });
    const group = device.createBindGroup({
      layout: pipeline.getBindGroupLayout(0),
      entries: held.map((buffer, binding) => ({ binding, resource: { buffer } })),
    });
    const encoder = device.createCommandEncoder();
    const pass = encoder.beginComputePass();
    pass.setPipeline(pipeline);
    pass.setBindGroup(0, group);
    pass.dispatchWorkgroups(across, rows, 1);
    pass.end();
    encoder.copyBufferToBuffer(held[1], 0, reader, 0, 4);
    outputs.forEach((o, i) => encoder.copyBufferToBuffer(held[2 + inputs.length + i], 0, reader, 4 + i * count * 4, count * 4));
    device.queue.submit([encoder.finish()]);
    const error = await device.popErrorScope();
    let answer;
    if (error) {
      this.said.push(error.message);
      answer = no('the card refused');
    } else {
      await reader.mapAsync(GPUMapMode.READ);
      const result = reader.getMappedRange();
      if (new Uint32Array(result, 0, 1)[0] !== 0) {
        answer = { ran: false, stopped: true, why: 'a round stopped' };
      } else {
        // all or nothing: every output is here
        outputs.forEach((o, i) => new Uint32Array(o.buffer, o.byteOffset, count).set(new Uint32Array(result, 4 + i * count * 4, count)));
        answer = { ran: true, stopped: false, why: '', compile_ms: this.compile_ms };
      }
      reader.unmap();
    }
    for (const b of held) b.destroy();
    reader.destroy();
    return answer;
  }
}
