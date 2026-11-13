// One node of the render farm (web/cluster/index.html): render.wasm, built
// for wasm32-wasip1-threads, as one machine of a cluster runs it -- a memory
// of its own, its threads in workers of their own (the page is their
// coordinator: threads.mjs), and its sockets on the page's network
// (net.mjs), which every node shares.
//
//   { bytes, module, env, net: { buffer, node }, kept, most }
//       -> { type: 'memory', kept }       where it made a new memory
//          { type: 'out', fd, text }      what it writes, as it comes
//          { type: 'exit', status }       its end
//
// `kept` is the memory this node's place had in the run before, which it
// takes again (threads.mjs, runMemory): a browser gives the address space of
// a memory back late, and a page that rendered again and again would run out
// of it. `most` is what the memory may grow to, in pages of 64 KiB: the page
// gives a node more the more cores it has -- each of its threads keeps
// pages of its own. The page ends this worker to end the node -- as a
// machine is switched off.
import { Wasi } from './wasi.mjs';
import { Net } from './net.mjs';
import { ThreadHost, runMemory } from './threads.mjs';

self.onmessage = async ({ data: { bytes, module, env, net, kept, most } }) => {
  const decoder = new TextDecoder();
  const out = fd => bytes => self.postMessage({ type: 'out', fd, text: decoder.decode(bytes) });
  let status;
  try {
    const got = runMemory(bytes, kept, { most });
    if (got !== kept) self.postMessage({ type: 'memory', kept: got });
    const wasi = new Wasi({
      args: ['render'], env, net: new Net(net.buffer, net.node),
      stdout: out(1), stderr: out(2),
      threads: new ThreadHost({ memory: got.memory, post: m => self.postMessage(m) }),
    });
    status = wasi.start(await WebAssembly.instantiate(module, wasi.imports(module)));
  } catch (e) {
    self.postMessage({ type: 'out', fd: 2, text: `[the node stopped: ${e && e.message || e}]\n` });
    status = 134;
  }
  self.postMessage({ type: 'exit', status });
};
