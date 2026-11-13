// One node of the render farm (web/cluster/index.html): render.wasm, built
// for wasm32-wasip1-threads with --gpu, as one machine of a cluster runs it --
// a memory of its own, its threads in workers of their own (the page is their
// coordinator: threads.mjs), its sockets on the page's network (net.mjs),
// which every node shares, and, where it has one, the page's graphics card.
//
//   { bytes, module, env, net: { buffer, node }, kept, most, card }
//       -> { type: 'memory', kept }       where it made a new memory
//          { type: 'scaly-gpu', run, transfer }   a run for the card
//          { type: 'out', fd, text }      what it writes, as it comes
//          { type: 'exit', status, cards } its end; how often the card ran
//
// `kept` is the memory this node's place had in the run before, which it
// takes again (threads.mjs, runMemory): a browser gives the address space of
// a memory back late, and a page that rendered again and again would run out
// of it. `most` is what the memory may grow to, in pages of 64 KiB: the page
// gives a node more the more cores it has -- each of its threads keeps
// pages of its own. `card`, where the node has one, is the first answer of
// the page's card worker (gpu-worker.mjs, { card, why }): the node's card
// runs go to the page, which passes them on to that worker -- one card for
// every node that has one, its kernels compiled once. The card is FORCED
// (gpu-host.mjs, mode 'on'): such a node renders its tiles there. The page
// ends this worker to end the node -- as a machine is switched off.
import { Wasi } from './wasi.mjs';
import { Net } from './net.mjs';
import { ThreadHost, runMemory } from './threads.mjs';
import { GpuHost } from './gpu-host.mjs';

self.onmessage = async ({ data: { bytes, module, env, net, kept, most, card } }) => {
  const decoder = new TextDecoder();
  const out = fd => bytes => self.postMessage({ type: 'out', fd, text: decoder.decode(bytes) });
  let status, gpu = null;
  try {
    if (card) {
      const relay = {
        hello: card,
        postMessage: (run, transfer = []) => self.postMessage({ type: 'scaly-gpu', run, transfer }, transfer),
      };
      gpu = await GpuHost.open(relay, { mode: 'on' }).catch(() => null);
    }
    const got = runMemory(bytes, kept, { most });
    if (got !== kept) self.postMessage({ type: 'memory', kept: got });
    const wasi = new Wasi({
      args: ['render'], env, net: new Net(net.buffer, net.node), gpu,
      stdout: out(1), stderr: out(2),
      threads: new ThreadHost({ memory: got.memory, post: m => self.postMessage(m) }),
    });
    status = wasi.start(await WebAssembly.instantiate(module, wasi.imports(module)));
  } catch (e) {
    self.postMessage({ type: 'out', fd: 2, text: `[the node stopped: ${e && e.message || e}]\n` });
    status = 134;
  }
  const cards = gpu ? gpu.report().reduce((all, site) => all + site.card, 0) : 0;
  self.postMessage({ type: 'exit', status, cards });
};
