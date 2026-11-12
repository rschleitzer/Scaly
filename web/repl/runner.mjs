// The playground's program worker (playground.html): a program the page
// BUILT for wasm32-wasip1-threads with --gpu (`scaly build` in the page,
// worker.mjs and playground.json) runs here, once per message -- a new
// instance over a new memory each time --, its threads in the page's
// workers (the page is their coordinator: threads.mjs) and its card in a
// worker of its own (gpu-host.mjs, gpu-worker.mjs), opened once and kept, so
// that a kernel is compiled once.
//
//   { type: 'hello' }                    -> { type: 'ready', card, why }
//   { type: 'load', bytes }              -> { type: 'loaded', threads }
//   { type: 'run', args, env, gpu }      -> { type: 'done', status, stdout, stderr, ms,
//                                              pictures, drawings, cards, compiled }
//
// `gpu` is the card's mode for the run: 'off', 'on' or 'auto'. `ms` is how
// long the program ran, `cards` how many loops the card took, `compiled`
// how much of `ms` compiling the card's kernels took (a kernel is compiled
// once, at its first run, and kept). `pictures` and
// `drawings` are what worker.mjs hands back for a program `scaly run` ran:
// the `.<w>x<h>.rgba` files and the SVG files it wrote.
import { MemoryFiles, Wasi } from './wasi.mjs';
import { ThreadHost, threadMemory, threaded, PAGE_MEMORY } from './threads.mjs';
import { GpuHost } from './gpu-host.mjs';

let program = null;     // { bytes, module }
let card;               // the GpuHost, or null where the page has no card

self.onmessage = async ({ data }) => {
  try {
    await answer(data);
  } catch (e) {
    // what went wrong is said, never left unanswered: the page waits for it
    const why = 'ended: ' + (e && e.message || e);
    if (data.type === 'run') self.postMessage({ type: 'done', status: why, stdout: '', stderr: '', ms: 0, pictures: [], drawings: [], cards: 0, compiled: 0 });
    else if (data.type === 'load') self.postMessage({ type: 'loaded', failed: why });
    else self.postMessage({ type: 'ready', card: false, why });
  }
};

async function answer(data) {
  if (card === undefined)
    card = await GpuHost.open(new URL('./gpu-worker.mjs', import.meta.url), { mode: 'off' }).catch(() => null);
  if (data.type === 'hello') {
    self.postMessage({ type: 'ready', card: card !== null, why: card ? '' : (GpuHost.why || 'the card did not open') });
    return;
  }
  if (data.type === 'load') {
    program = { bytes: data.bytes, module: await WebAssembly.compile(data.bytes) };
    self.postMessage({ type: 'loaded', threads: threaded(program.bytes) });
    return;
  }
  const memory = threadMemory(program.bytes, { most: PAGE_MEMORY });
  if (card) card.mode = data.gpu;
  const sum = what => card ? card.report().reduce((all, site) => all + site[what], 0) : 0;
  const before = sum('card'), compiledBefore = sum('compile_ms');
  const files = new MemoryFiles();
  for (const dir of ['/tmp', '/work']) files.dirs.add(dir);
  const wasi = new Wasi({
    args: ['main', ...(data.args ?? [])], env: data.env, cwd: '/work', files, gpu: card,
    threads: memory ? new ThreadHost({ memory, post: m => self.postMessage(m) }) : null,
  });
  const t0 = performance.now();
  let status;
  try {
    status = String(wasi.start(await WebAssembly.instantiate(program.module, wasi.imports(program.module))));
  } catch (e) {
    status = 'ended: ' + (e && e.message || e);
  }
  const ms = performance.now() - t0;
  const pictures = [], drawings = [];
  for (const path of new Set(files.written)) {
    const bytes = path.startsWith('/work/') ? files.read(path) : null;
    if (!bytes) continue;
    const size = /\.(\d+)x(\d+)\.rgba$/.exec(path);
    if (size && bytes.length === Number(size[1]) * Number(size[2]) * 4)
      pictures.push({ name: path.slice(6), width: Number(size[1]), height: Number(size[2]), bytes: bytes.slice() });
    else if (path.endsWith('.svg'))
      drawings.push({ name: path.slice(6), text: new TextDecoder().decode(bytes) });
  }
  const cards = sum('card') - before;
  // of the run: compiling the card's kernels, which only a kernel's first run pays
  const compiled = sum('compile_ms') - compiledBefore;
  self.postMessage({ type: 'done', status, stdout: wasi.stdout_text(), stderr: wasi.stderr_text(), ms,
                     pictures, drawings, cards, compiled }, pictures.map(p => p.bytes.buffer));
}
