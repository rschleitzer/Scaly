// The showcase's program worker (web/mandelbrot/index.html): mandelbrot.wasm,
// built for wasm32-wasip1-threads with --gpu, runs here once per message --
// a new instance over a new memory each time --, its threads in the page's
// workers (the page is their coordinator: threads.mjs) and its card in a
// worker of its own (gpu-host.mjs, gpu-worker.mjs), opened once and kept, so
// that a kernel is compiled once.
//
//   { type: 'hello' }                    -> { type: 'ready', card }
//   { type: 'load', bytes }              -> { type: 'loaded', threads }
//   { type: 'run', args, env, gpu }      -> { type: 'done', status, stdout, stderr, picture, cards }
//
// `load` puts another module in the place of mandelbrot.wasm: the reader's
// own version, built in the page (index.html). `threads` says whether it
// was built for them.
//
// `gpu` is the card's mode for the run: 'off', 'on' or 'auto'. `picture`
// holds the bytes the program wrote (mandelbrot.<w>x<h>.rgba), `cards` how
// many loops the card took in this run.
import { MemoryFiles, Wasi } from './wasi.mjs';
import { ThreadHost, threadMemory } from './threads.mjs';
import { GpuHost } from './gpu-host.mjs';

let program = null;     // { bytes, module }
let card;               // the GpuHost, or null where the page has no card

async function prepare() {
  if (!program) {
    const bytes = new Uint8Array(await (await fetch('mandelbrot.wasm')).arrayBuffer());
    program = { bytes, module: await WebAssembly.compile(bytes) };
  }
  if (card === undefined)
    card = await GpuHost.open(new URL('./gpu-worker.mjs', import.meta.url), { mode: 'off' }).catch(() => null);
}

self.onmessage = async ({ data }) => {
  if (data.type === 'load') {
    program = { bytes: data.bytes, module: await WebAssembly.compile(data.bytes) };
    self.postMessage({ type: 'loaded', threads: threadMemory(program.bytes) !== null });
    return;
  }
  await prepare();
  if (data.type === 'hello') {
    self.postMessage({ type: 'ready', card: card !== null, threads: threadMemory(program.bytes) !== null });
    return;
  }
  const memory = threadMemory(program.bytes);
  if (card) card.mode = data.gpu;
  const before = card ? card.report().reduce((sum, site) => sum + site.card, 0) : 0;
  const files = new MemoryFiles();
  const wasi = new Wasi({
    args: ['mandelbrot', ...data.args], env: data.env, files, gpu: card,
    threads: memory ? new ThreadHost({ memory, post: m => self.postMessage(m) }) : null,
  });
  let status;
  try {
    status = String(wasi.start(await WebAssembly.instantiate(program.module, wasi.imports(program.module))));
  } catch (e) {
    status = 'ended: ' + (e && e.message || e);
  }
  // the picture: the first file the program wrote whose name says its size
  // (mandelbrot.<w>x<h>.rgba) and that holds four bytes per point
  let picture = null, width = 0, height = 0;
  for (const path of files.written) {
    const size = /\.(\d+)x(\d+)\.rgba$/.exec(path);
    const bytes = size && files.read(path);
    if (bytes && bytes.length === Number(size[1]) * Number(size[2]) * 4) {
      picture = bytes.slice();
      width = Number(size[1]);
      height = Number(size[2]);
      break;
    }
  }
  const cards = card ? card.report().reduce((sum, site) => sum + site.card, 0) - before : 0;
  self.postMessage({ type: 'done', status, stdout: wasi.stdout_text(), stderr: wasi.stderr_text(), picture, width, height, cards },
                   picture ? [picture.buffer] : []);
};
