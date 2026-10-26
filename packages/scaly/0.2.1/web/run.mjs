// Run a WASI module on the command line through the page's host (wasi.mjs),
// as a page would run it: the arguments after the module are its argv, the
// working directory this one, standard input read whole first. A file the
// program opens is read from the disk into the MemoryFiles when it is first
// named, a directory it lists is listed from the disk, and every directory it
// made, file it wrote and file it removed goes back to the disk at the end --
// the one difference from a page, which fills the MemoryFiles itself.
// Standard output and error are written as they come; the exit code is the
// program's.
//
//   node run.mjs [--argv0 name] program.wasm [args...]
//
// --argv0 names the program in its argv[0] (and so in its messages) where the
// module's path would be: a wrapper script passes its own name, `$0`.
import { mkdirSync, readdirSync, readFileSync, statSync, unlinkSync, writeFileSync } from 'node:fs';
import process from 'node:process';
import { isatty } from 'node:tty';
import { MemoryFiles, Wasi } from './wasi.mjs';

let rest = process.argv.slice(2);
let argv0 = null;
if (rest[0] === '--argv0') {
  argv0 = rest[1];
  rest = rest.slice(2);
}
const [module_path, ...args] = rest;
// Only a regular file is read: a FIFO would keep the run waiting for a writer
// (every .NET process keeps two in the temporary directory, for its debugger),
// and a device has no end.
const files = new MemoryFiles({}, {
  fallback(path) {
    try {
      const s = statSync(path);
      return s.isDirectory() ? 'dir' : s.isFile() ? readFileSync(path) : null;
    } catch {
      return null;
    }
  },
  fallback_kind(path) {
    try {
      const s = statSync(path);
      return s.isDirectory() ? 'dir' : s.isFile() ? 'file' : null;
    } catch {
      return null;
    }
  },
  fallback_list(path) {
    try {
      return readdirSync(path);
    } catch {
      return [];
    }
  },
});
// a pipe on standard output is non-blocking under node, and a write past its
// buffer would fail with EAGAIN. Standard input is read whole when the
// program first reads it -- never before: a pipe nobody closes would keep a
// program waiting that does not read it at all.
for (const stream of [process.stdout, process.stderr]) stream._handle?.setBlocking?.(true);
const stdin = () => {
  if (isatty(0)) return new Uint8Array();
  try { return readFileSync(0); } catch { return new Uint8Array(); }
};
const wasi = new Wasi({
  args: [argv0 ?? module_path, ...args], env: process.env, cwd: process.cwd(), files, stdin,
  stdout: chunk => writeFileSync(1, chunk),
  stderr: chunk => writeFileSync(2, chunk),
});
const module = await WebAssembly.compile(readFileSync(module_path));
const status = wasi.start(await WebAssembly.instantiate(module, wasi.imports()));
for (const path of files.made) mkdirSync(path, { recursive: true });
for (const path of files.removed) {
  try { unlinkSync(path); } catch { }
}
for (const path of files.written) writeFileSync(path, files.read(path));
process.exit(status);
