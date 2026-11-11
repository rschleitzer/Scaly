// The REPL's worker: the compiler built for WebAssembly (tools/wasm/repl.sh
// build) runs here, off the page's thread, for two reasons -- it reads its
// standard input as it comes and must wait for the next line, and the module
// of every entry is compiled synchronously (side.mjs), which a page allows in
// a worker only.
//
// The page hands over, once: the compiled module, the standard library's
// files (paths under /scaly, SCALY_HOME) and a SharedArrayBuffer through
// which it hands on what is typed -- an Int32 state (0 nothing there, 1 bytes
// there, 2 the end of the input), an Int32 count, then the bytes -- or no
// buffer, and the standard input is empty (the reference's Run buttons,
// whose page is not cross-origin isolated). Without more it is the REPL; the tour (tour.html) hands `args` too, ['scaly',
// 'run', 'main.scaly'], and the program as one of the `files`, under /work.
// Back come messages: `out`/`err` with bytes as they are written, `wants`
// when the program waits for input, `exit` with its status, `crash` with a
// trap's text.
//
// With `gpu` in what the page hands over -- 'on', 'off' or 'auto', the mode
// of the standard library's gpu-host.mjs -- this worker opens the page's
// graphics card first (a second worker, gpu-worker.mjs) and gives it to the
// program: one run with `scaly run --gpu` then sends the loops that are
// kernels there. `card` comes back before anything else, with whether the
// page has one (`has`), and with `exit` the host's `report`, per loop how
// often it went where. Without a card every loop stays on the processor.
import { MemoryFiles, Wasi } from './wasi.mjs';
import { SideModules } from './side.mjs';
import { GpuHost } from './gpu-host.mjs';

const encoder = new TextEncoder();

onmessage = async ({ data }) => {
  const { module, files: tree, stdin, args = ['scaly'], gpu: mode = null } = data;
  let gpu = null;
  if (mode) {
    gpu = await GpuHost.open(new URL('./gpu-worker.mjs', import.meta.url), { mode }).catch(() => null);
    postMessage({ type: 'card', has: gpu !== null });
  }
  const state = stdin ? new Int32Array(stdin, 0, 2) : null;
  const bytes = stdin ? new Uint8Array(stdin, 8) : null;
  const files = new MemoryFiles(tree);
  for (const dir of ['/tmp', '/work', '/home']) files.dirs.add(dir);
  const wasi = new Wasi({
    args, env: { SCALY_HOME: '/scaly', HOME: '/home' }, cwd: '/work', files,
    stdin_more() {
      if (!state) return new Uint8Array();
      postMessage({ type: 'wants' });
      Atomics.wait(state, 0, 0);
      if (state[0] === 2) return new Uint8Array();
      const line = bytes.slice(0, state[1]);
      Atomics.store(state, 0, 0);
      return line;
    },
    stdout: chunk => postMessage({ type: 'out', bytes: chunk }),
    stderr: chunk => postMessage({ type: 'err', bytes: chunk }),
    gpu,
  });
  const side = new SideModules(path => files.read(path),
                               text => postMessage({ type: 'err', bytes: encoder.encode(text + '\n') }));
  try {
    const repl = new WebAssembly.Instance(module, { ...wasi.imports(), ...side.imports() });
    side.attach(repl);
    const status = wasi.start(repl);
    postMessage({ type: 'exit', status, report: gpu ? gpu.report() : null });
  } catch (e) {
    postMessage({ type: 'crash', text: String(e) });
  }
};
