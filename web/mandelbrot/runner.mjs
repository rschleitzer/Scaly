// The showcase's program worker (web/mandelbrot/index.html): mandelbrot.wasm,
// built for wasm32-wasip1-threads with --gpu, runs here once per message --
// a new instance over a new memory each time --, its threads in the page's
// workers (the page is their coordinator: threads.mjs) and its card in a
// worker of its own (gpu-host.mjs, gpu-worker.mjs), opened once and kept, so
// that a kernel is compiled once.
//
//   { type: 'hello', gpu }               -> { type: 'ready', card, why }
//   { type: 'load', bytes }              -> { type: 'loaded', threads }
//   { type: 'run', args, env, gpu }      -> { type: 'watch', memory, address, width, height }, then
//                                           { type: 'done', status, stdout, stderr, picture, cards, compiled }
//
// `gpu`, where given, is a MessagePort to the page's own card worker
// (gpu-worker.mjs): the page ends this worker to break a run off and makes
// another, and the card, its kernels compiled, stays. Without it this
// worker opens a card of its own.
//
// `load` puts another module in the place of mandelbrot.wasm: the reader's
// own version, built in the page (index.html). `threads` says whether it
// was built for them.
//
// `gpu` is the card's mode for the run: 'off', 'on' or 'auto'. `picture`
// holds the bytes the program wrote (mandelbrot.<w>x<h>.rgba), `cards` how
// many loops the card took in this run.
//
// `watch` comes while the program runs, where it says where its picture
// lies (a line `picture at <address>` before its loop) and its memory is
// shared: the page then reads the picture as the threads and the card write
// it.
import { MemoryFiles, Wasi } from './wasi.mjs';
import { ThreadHost, threadMemory, threaded, PAGE_MEMORY } from './threads.mjs';
import { GpuHost } from './gpu-host.mjs';

let program = null;     // { bytes, module }
let card;               // the GpuHost, or null where the page has no card

async function prepare(port) {
  if (!program) {
    const bytes = new Uint8Array(await (await fetch('mandelbrot.wasm')).arrayBuffer());
    program = { bytes, module: await WebAssembly.compile(bytes) };
  }
  if (card === undefined)
    card = await GpuHost.open(port ?? new URL('./gpu-worker.mjs', import.meta.url), { mode: 'off' }).catch(() => null);
}

self.onmessage = async ({ data }) => {
  try {
    await answer(data);
  } catch (e) {
    // what went wrong is said, never left unanswered: the page waits for it
    const why = 'ended: ' + (e && e.message || e);
    if (data.type === 'run') self.postMessage({ type: 'done', status: why, stdout: '', stderr: '', picture: null, cards: 0, compiled: 0 });
    else if (data.type === 'load') self.postMessage({ type: 'loaded', failed: why });
    else self.postMessage({ type: 'ready', failed: why });
  }
};

async function answer(data) {
  if (data.type === 'load') {
    program = { bytes: data.bytes, module: await WebAssembly.compile(data.bytes) };
    self.postMessage({ type: 'loaded', threads: threaded(program.bytes) });
    return;
  }
  await prepare(data.gpu);
  if (data.type === 'hello') {
    self.postMessage({ type: 'ready', card: card !== null, why: card ? '' : (GpuHost.why || 'the card did not open'),
                       threads: threaded(program.bytes) });
    return;
  }
  const memory = threadMemory(program.bytes, { most: PAGE_MEMORY });
  if (card) card.mode = data.gpu;
  const sum = what => card ? card.report().reduce((all, site) => all + site[what], 0) : 0;
  const before = sum('card'), compiledBefore = sum('compile_ms');
  const files = new MemoryFiles();
  // what the program writes, read as it comes: the line that says where its
  // picture lies is handed to the page at once
  const written = [];
  const decoder = new TextDecoder();
  let line = '';
  const stdout = bytes => {
    written.push(bytes);
    line += decoder.decode(bytes, { stream: true });
    for (let nl; (nl = line.indexOf('\n')) >= 0; line = line.slice(nl + 1)) {
      const at = /^picture at (\d+)$/.exec(line.slice(0, nl));
      if (at && memory)
        self.postMessage({ type: 'watch', memory, address: Number(at[1]),
                           width: Number(data.args[0]), height: Number(data.args[1]) });
    }
  };
  const wasi = new Wasi({
    args: ['mandelbrot', ...data.args], env: data.env, files, gpu: card, stdout,
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
  const cards = sum('card') - before;
  // of the run: compiling the card's kernels, which only a kernel's first run pays
  const compiled = sum('compile_ms') - compiledBefore;
  const all = new Uint8Array(written.reduce((n, b) => n + b.length, 0));
  written.reduce((at, b) => (all.set(b, at), at + b.length), 0);
  const text = new TextDecoder().decode(all);
  self.postMessage({ type: 'done', status, stdout: text, stderr: wasi.stderr_text(), picture, width, height, cards, compiled },
                   picture ? [picture.buffer] : []);
}
