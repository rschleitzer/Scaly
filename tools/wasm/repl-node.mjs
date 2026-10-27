// tools/wasm/repl-node.mjs <scaly.wasm> [args...] -- the tool built for
// WebAssembly on the command line, through the page's own host: the WASI of
// the standard library's web/wasi.mjs and the side-module loader of
// web/repl/side.mjs, which loads every entry the REPL links
// (Emitter.wasm_run) into the REPL's memory and table. The one difference
// from the page (web/repl/worker.mjs): a file the program opens is read from
// the disk when it is first named, and standard input is this process's,
// read as it comes. Nothing the program writes goes back to the disk.
//
// SCALY_WASM_TRACE=<file> lists there every path read from the disk -- what a
// page has to carry (web/repl/build.sh).
import { appendFileSync, readFileSync, readdirSync, readSync, statSync, writeFileSync } from 'node:fs';
import process, { argv, cwd, env, exit } from 'node:process';
import { MemoryFiles, Wasi } from '../../packages/scaly/0.2.1/web/wasi.mjs';
import { SideModules } from '../../web/repl/side.mjs';

for (const stream of [process.stdout, process.stderr]) stream._handle?.setBlocking?.(true);

const trace = env.SCALY_WASM_TRACE;
const kind_of = path => {
  try {
    const s = statSync(path);
    return s.isDirectory() ? 'dir' : s.isFile() ? 'file' : null;
  } catch {
    return null;
  }
};
const files = new MemoryFiles({}, {
  fallback(path) {
    const kind = kind_of(path);
    if (kind === 'file' && trace) appendFileSync(trace, path + '\n');
    return kind === 'dir' ? 'dir' : kind === 'file' ? readFileSync(path) : null;
  },
  fallback_kind: kind_of,
  fallback_list(path) {
    try {
      return readdirSync(path);
    } catch {
      return [];
    }
  },
});

// a read of a pipe or a terminal that is not ready yet is tried again
function more() {
  const buf = Buffer.alloc(65536);
  for (;;) {
    try {
      return new Uint8Array(buf.buffer, 0, readSync(0, buf, 0, buf.length));
    } catch (e) {
      if (e.code === 'EAGAIN') continue;
      if (e.code === 'EOF') return new Uint8Array();
      throw e;
    }
  }
}

const wasi = new Wasi({
  args: argv.slice(2), env: { ...env }, cwd: cwd(), files, stdin_more: more,
  stdout: chunk => writeFileSync(1, chunk),
  stderr: chunk => writeFileSync(2, chunk),
});
const side = new SideModules(path => files.read(path), text => writeFileSync(2, text + '\n'));
const module = await WebAssembly.compile(readFileSync(argv[2]));
const repl = await WebAssembly.instantiate(module, { ...wasi.imports(), ...side.imports() });
side.attach(repl);
exit(wasi.start(repl));
