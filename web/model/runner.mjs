// The model's program worker (web/model/index.html): model.wasm, built for
// wasm32-wasip1-threads, runs here once and keeps running -- it draws the
// picture anew whenever the page moves c, and waits otherwise. Its threads
// run in the page's workers (the page is their coordinator: threads.mjs).
//
//   { type: 'run', env }   -> { type: 'watch', memory, address }, once the
//                             program says where its board lies
//                             { type: 'done', status, stderr } where it ends
//
// The page reads the board straight out of the program's memory, which its
// threads share, and writes c into the board's head (model.scaly says what
// lies where).
import { MemoryFiles, Wasi } from './wasi.mjs';
import { ThreadHost, threadMemory, PAGE_MEMORY } from './threads.mjs';

self.onmessage = async ({ data }) => {
  if (data.type !== 'run') return;
  let status;
  const wasi = { stderr_text: () => '' };
  try {
    const bytes = new Uint8Array(await (await fetch('model.wasm')).arrayBuffer());
    const module = await WebAssembly.compile(bytes);
    const memory = threadMemory(bytes, { most: PAGE_MEMORY });
    const decoder = new TextDecoder();
    let line = '';
    const stdout = chunk => {
      line += decoder.decode(chunk, { stream: true });
      for (let nl; (nl = line.indexOf('\n')) >= 0; line = line.slice(nl + 1)) {
        const at = /^board at (\d+)$/.exec(line.slice(0, nl));
        if (at && memory) self.postMessage({ type: 'watch', memory, address: Number(at[1]) });
      }
    };
    const host = new Wasi({
      args: ['model'], env: data.env, files: new MemoryFiles(), stdout,
      threads: memory ? new ThreadHost({ memory, post: m => self.postMessage(m) }) : null,
    });
    wasi.stderr_text = () => host.stderr_text();
    status = String(host.start(await WebAssembly.instantiate(module, host.imports(module))));
  } catch (e) {
    status = 'ended: ' + (e && e.message || e);
  }
  self.postMessage({ type: 'done', status, stderr: wasi.stderr_text() });
};
