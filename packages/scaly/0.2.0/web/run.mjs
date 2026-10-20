// Run a WASI module on the command line through the page's host (wasi.mjs),
// as a page would run it: the arguments after the module are its argv, the
// working directory this one, standard input read whole first. A file the
// program opens is read from the disk into the MemoryFiles when it is first
// named, and every file it wrote goes back to the disk at the end -- the one
// difference from a page, which fills the MemoryFiles itself. Standard output
// and error are written as they come; the exit code is the program's.
//
//   node run.mjs program.wasm [args...]
import { readFileSync, statSync, writeFileSync } from 'node:fs';
import process from 'node:process';
import { isatty } from 'node:tty';
import { MemoryFiles, Wasi } from './wasi.mjs';

const [module_path, ...args] = process.argv.slice(2);
const files = new MemoryFiles({}, {
  fallback(path) {
    try {
      return statSync(path).isDirectory() ? 'dir' : readFileSync(path);
    } catch {
      return null;
    }
  },
});
// a pipe on standard output is non-blocking under node, and a write past its
// buffer would fail with EAGAIN; standard input is read before node opens a
// stream on it (which would make it non-blocking too)
for (const stream of [process.stdout, process.stderr]) stream._handle?.setBlocking?.(true);
let stdin = new Uint8Array();
if (!isatty(0)) {
  try { stdin = readFileSync(0); } catch { }
}
const wasi = new Wasi({
  args: [module_path, ...args], env: process.env, cwd: process.cwd(), files, stdin,
  stdout: chunk => writeFileSync(1, chunk),
  stderr: chunk => writeFileSync(2, chunk),
});
const module = await WebAssembly.compile(readFileSync(module_path));
const status = wasi.start(await WebAssembly.instantiate(module, wasi.imports()));
for (const path of files.written) writeFileSync(path, files.read(path));
process.exit(status);
