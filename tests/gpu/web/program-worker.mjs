// One program of tests/gpu/web/run.sh, built for wasm32 with --gpu, run in
// this worker three times over ONE host of the page's card: with the card
// off, with it tried at every loop's head, and left to the gate. Posted
// back: per mode the exit status (or how the run ended), what the program
// wrote, how many loops the card ran and what the host said.
import { MemoryFiles, Wasi } from './wasi.mjs';
import { GpuHost } from './gpu-host.mjs';

self.onmessage = async event => {
  const { wasm } = event.data;
  try {
    const module = await WebAssembly.compileStreaming(fetch(wasm));
    const said = [];
    const gpu = await GpuHost.open(new URL('./gpu-worker.mjs', import.meta.url), { trace: line => said.push(line) });
    if (!gpu) {
      self.postMessage({ card: false });
      return;
    }
    const runs = [];
    for (const mode of ['off', 'on', 'auto']) {
      gpu.mode = mode;
      said.length = 0;
      const before = gpu.report().reduce((sum, site) => sum + site.card, 0);
      const files = new MemoryFiles();
      files.dirs.add('/tmp');
      const wasi = new Wasi({ args: ['program'], files, gpu });
      let status;
      try {
        status = String(wasi.start(await WebAssembly.instantiate(module, wasi.imports())));
      } catch (e) {
        status = 'ended: ' + (e && e.message || e);
      }
      const cards = gpu.report().reduce((sum, site) => sum + site.card, 0) - before;
      runs.push({ mode, status, stdout: wasi.stdout_text(), cards, said: said.slice() });
    }
    self.postMessage({ card: true, runs, report: gpu.report() });
  } catch (e) {
    self.postMessage({ card: true, failed: String(e && e.stack || e) });
  }
};
