// One run of tests/gpu/web/run.sh's part 4: a program built for
// wasm32-wasip1-threads, in this worker, its threads in the page's
// (threads.mjs: the page is the coordinator and makes their workers), and,
// where `gpu` says so, the page's card beside them. Posted back: the exit
// status, what the program wrote, how long it ran and how many loops the
// card took.
import { MemoryFiles, Wasi } from './wasi.mjs';
import { ThreadHost, threadMemory } from './threads.mjs';
import { GpuHost } from './gpu-host.mjs';

self.onmessage = async ({ data }) => {
  const bytes = new Uint8Array(await (await fetch(data.wasm)).arrayBuffer());
  const module = await WebAssembly.compile(bytes);
  const memory = threadMemory(bytes);
  const gpu = data.gpu ? await GpuHost.open(new URL('./gpu-worker.mjs', import.meta.url), { mode: data.gpu }) : null;
  const files = new MemoryFiles();
  files.dirs.add('/tmp');
  const wasi = new Wasi({
    args: ['program'], env: data.env, files, gpu,
    threads: memory ? new ThreadHost({ memory, post: m => self.postMessage(m) }) : null,
  });
  const t0 = performance.now();
  let status;
  try {
    status = String(wasi.start(await WebAssembly.instantiate(module, wasi.imports(module))));
  } catch (e) {
    status = 'ended: ' + (e && e.message || e);
  }
  const ms = performance.now() - t0;
  const cards = gpu ? gpu.report().reduce((sum, site) => sum + site.card, 0) : 0;
  self.postMessage({ type: 'exit', status, stdout: wasi.stdout_text(), stderr: wasi.stderr_text(), ms, cards, threaded: memory !== null });
};
