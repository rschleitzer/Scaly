// tools/wasm/repl-node.mjs <scaly.wasm> [args...] -- the tool built for
// WebAssembly, under node's WASI, as tests/wasm/run.mjs runs a module, with
// the one import the compiler's REPL asks of its host where there is no JIT
// (compiler/wasm_eh.cpp): `scaly.run(path, argc, argv)` starts the module the
// REPL linked for an entry as an instance of its own, on the same standard
// streams, and answers its exit code. The file system is the host's from `/`,
// the working directory this one.
import { readFile } from 'node:fs/promises';
import { readFileSync } from 'node:fs';
import { WASI } from 'node:wasi';
import process, { argv, cwd, env, exit } from 'node:process';

for (const stream of [process.stdout, process.stderr]) stream._handle?.setBlocking?.(true);

let memory = null;
function cString(at) {
  const bytes = new Uint8Array(memory.buffer);
  let end = at;
  while (bytes[end] !== 0) end++;
  return new TextDecoder().decode(bytes.subarray(at, end));
}

// an entry's module: compiled and started at once, its exit code the answer
function run(pathAt, argc, argvAt) {
  const path = cString(pathAt);
  const view = new DataView(memory.buffer);
  const args = [path];
  for (let i = 0; i < argc; i++) args.push(cString(view.getUint32(argvAt + 4 * i, true)));
  const wasi = new WASI({ version: 'preview1', args, env: { ...env, PWD: cwd() }, preopens: { '/': '/' }, returnOnExit: true });
  const module = new WebAssembly.Module(readFileSync(path));
  const instance = new WebAssembly.Instance(module, wasi.getImportObject());
  return wasi.start(instance);
}

const wasi = new WASI({ version: 'preview1', args: argv.slice(2), env: { ...env, PWD: cwd() }, preopens: { '/': '/' }, returnOnExit: true });
const module = await WebAssembly.compile(await readFile(argv[2]));
const instance = await WebAssembly.instantiate(module, { ...wasi.getImportObject(), scaly: { run } });
memory = instance.exports.memory;
exit(wasi.start(instance));
