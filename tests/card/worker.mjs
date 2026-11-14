// A program of tests/card/run.sh, built for wasm32 with --gpu, run in this
// worker with the page's card as the home of its CardVectors. Posted back:
// its exit status, what it wrote, and how often it went to the card.
import { MemoryFiles, Wasi } from './wasi.mjs';
import { CardHost } from './card-host.mjs';

self.onmessage = async ({ data: { wasm } }) => {
  try {
    const module = await WebAssembly.compileStreaming(fetch(wasm));
    const card = await CardHost.open(new URL('./card-worker.mjs', import.meta.url));
    if (!card) {
      self.postMessage({ card: false, why: CardHost.why });
      return;
    }
    const wasi = new Wasi({ args: ['program'], files: new MemoryFiles(), card });
    let status;
    try {
      status = String(wasi.start(await WebAssembly.instantiate(module, wasi.imports())));
    } catch (e) {
      status = 'ended: ' + (e && e.message || e);
    }
    self.postMessage({ card: true, adapter: card.adapter, status, stdout: wasi.stdout_text(), stderr: wasi.stderr_text(), runs: card.runs, flushes: card.flushes });
  } catch (e) {
    self.postMessage({ card: true, failed: String(e && e.stack || e) });
  }
};
