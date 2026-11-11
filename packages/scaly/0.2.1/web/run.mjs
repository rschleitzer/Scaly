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
//
// A module built for wasm32-wasip1-threads runs as in a page (threads.mjs):
// this process's main thread makes the workers and does nothing else, the
// program runs in a worker of its own -- this file again --, each thread in
// one of thread.mjs. The program is told the cores of this machine
// (SCALY_CPUS); SCALY_WORKERS caps them, as natively.
import { mkdirSync, readdirSync, readFileSync, statSync, unlinkSync, writeFileSync } from 'node:fs';
import { availableParallelism } from 'node:os';
import process from 'node:process';
import { isatty } from 'node:tty';
import { isMainThread, parentPort, Worker, workerData } from 'node:worker_threads';
import { MemoryFiles, Wasi } from './wasi.mjs';

for (const stream of [process.stdout, process.stderr]) stream._handle?.setBlocking?.(true);
const program_args = isMainThread ? process.argv.slice(2) : workerData.args;
const bytes = readFileSync(program_args[program_args[0] === '--argv0' ? 2 : 0]);
// a module built for threads imports its memory; threads.mjs is loaded only
// then (a page that serves this file beside wasi.mjs alone runs every other one)
const threaded = WebAssembly.Module.imports(new WebAssembly.Module(bytes)).some(i => i.kind === 'memory');
const { ThreadHost, Threads, threadMemory } = threaded ? await import('./threads.mjs') : {};
const env = { ...process.env, SCALY_CPUS: String(availableParallelism()) };

if (isMainThread && threaded) {
  // the coordinator: the program in a worker, the threads in theirs
  const end = status => {
    threads.end();
    process.exit(status);
  };
  const threads = await Threads.create({
    out: bytes => writeFileSync(1, bytes),
    err: bytes => writeFileSync(2, bytes),
    exit: end,
    env,
  });
  const program = new Worker(new URL(import.meta.url), { workerData: { args: program_args } });
  program.on('message', data => {
    if (threads.handle(data)) return;
    if (data && data.type === 'exit') end(data.status);
  });
  program.on('error', e => {
    writeFileSync(2, `[the program stopped: ${e && e.message || e}]\n`);
    end(134);
  });
} else {
  await run();
}

async function run() {
const memory = threaded ? threadMemory(bytes) : null;
let rest = program_args;
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
const stdin = () => {
  if (isatty(0)) return new Uint8Array();
  try { return readFileSync(0); } catch { return new Uint8Array(); }
};
const wasi = new Wasi({
  args: [argv0 ?? module_path, ...args], env: memory ? env : process.env, cwd: process.cwd(), files, stdin,
  stdout: chunk => writeFileSync(1, chunk),
  stderr: chunk => writeFileSync(2, chunk),
  threads: memory ? new ThreadHost({ memory, post: m => parentPort.postMessage(m) }) : null,
});
const module = await WebAssembly.compile(bytes);
const status = wasi.start(await WebAssembly.instantiate(module, wasi.imports(module)));
for (const path of files.made) mkdirSync(path, { recursive: true });
for (const path of files.removed) {
  try { unlinkSync(path); } catch { }
}
for (const path of files.written) writeFileSync(path, files.read(path));
if (isMainThread) process.exit(status);
parentPort.postMessage({ type: 'exit', status });
}
