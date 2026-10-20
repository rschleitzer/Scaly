// Run a WASI module under node: the arguments after the module are its argv,
// the file system is the host's from `/`, the working directory this one (PWD,
// which the runtime adopts: eio_wasm.c), and the process ends with the
// module's exit code.
//
// The module writes the descriptors themselves, through WASI, and does not
// expect a short write: node sets a pipe on standard output non-blocking, so
// everything past the pipe's 64 KB was lost to wasi-libc's stdio. They are
// made blocking again first.
import { readFile } from 'node:fs/promises';
import { WASI } from 'node:wasi';
import process, { argv, cwd, env, exit } from 'node:process';

for (const stream of [process.stdout, process.stderr]) stream._handle?.setBlocking?.(true);
const wasi = new WASI({ version: 'preview1', args: argv.slice(2), env: { ...env, PWD: cwd() }, preopens: { '/': '/' }, returnOnExit: true });
const module = await WebAssembly.compile(await readFile(argv[2]));
const instance = await WebAssembly.instantiate(module, wasi.getImportObject());
exit(wasi.start(instance));
