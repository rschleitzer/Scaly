// The REPL's worker: the compiler built for WebAssembly (tools/wasm/repl.sh
// build) runs here, off the page's thread, for two reasons -- it reads its
// standard input as it comes and must wait for the next line, and the module
// of every entry is compiled synchronously (side.mjs), which a page allows in
// a worker only.
//
// The page hands over, once: the compiled module, the standard library's
// files (paths under /scaly, SCALY_HOME) and a SharedArrayBuffer through
// which it hands on what is typed -- an Int32 state (0 nothing there, 1 bytes
// there, 2 the end of the input), an Int32 count, then the bytes. Back come
// messages: `out`/`err` with bytes as they are written, `wants` when the
// REPL waits for input, `exit` with its status, `crash` with a trap's text.
import { MemoryFiles, Wasi } from './wasi.mjs';
import { SideModules } from './side.mjs';

const encoder = new TextEncoder();

onmessage = ({ data }) => {
  const { module, files: tree, stdin } = data;
  const state = new Int32Array(stdin, 0, 2), bytes = new Uint8Array(stdin, 8);
  const files = new MemoryFiles(tree);
  for (const dir of ['/tmp', '/work', '/home']) files.dirs.add(dir);
  const wasi = new Wasi({
    args: ['scaly'], env: { SCALY_HOME: '/scaly', HOME: '/home' }, cwd: '/work', files,
    stdin_more() {
      postMessage({ type: 'wants' });
      Atomics.wait(state, 0, 0);
      if (state[0] === 2) return new Uint8Array();
      const line = bytes.slice(0, state[1]);
      Atomics.store(state, 0, 0);
      return line;
    },
    stdout: chunk => postMessage({ type: 'out', bytes: chunk }),
    stderr: chunk => postMessage({ type: 'err', bytes: chunk }),
  });
  const side = new SideModules(path => files.read(path),
                               text => postMessage({ type: 'err', bytes: encoder.encode(text + '\n') }));
  try {
    const repl = new WebAssembly.Instance(module, { ...wasi.imports(), ...side.imports() });
    side.attach(repl);
    postMessage({ type: 'exit', status: wasi.start(repl) });
  } catch (e) {
    postMessage({ type: 'crash', text: String(e) });
  }
};
