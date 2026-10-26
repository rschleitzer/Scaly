// tools/wasm/repl-node.mjs <scaly.wasm> [args...] -- the tool built for
// WebAssembly, under node's WASI, as tests/wasm/run.mjs runs a module, with
// the one import the compiler's REPL asks of its host where there is no JIT
// (compiler/wasm_eh.cpp): `scaly.run(path, argc, argv)` loads the side module
// the REPL linked for an entry (Emitter.wasm_run) into the REPL's own memory
// and table, resolves what it imports against the REPL's exports, calls its
// `__main_argc_argv(argc, argv)` and answers that -- the entry's values stay
// in the one memory, on the REPL's pages, as under the native JIT. The file
// system is the host's from `/`, the working directory this one.
import { readFile } from 'node:fs/promises';
import { readFileSync } from 'node:fs';
import { WASI } from 'node:wasi';
import process, { argv, cwd, env, exit } from 'node:process';

for (const stream of [process.stdout, process.stderr]) stream._handle?.setBlocking?.(true);

let repl = null;
function cString(at) {
  const bytes = new Uint8Array(repl.exports.memory.buffer);
  let end = at;
  while (bytes[end] !== 0) end++;
  return new TextDecoder().decode(bytes.subarray(at, end));
}

function leb(bytes, at) {
  let v = 0, shift = 0, b;
  do { b = bytes[at.i++]; v |= (b & 0x7f) << shift; shift += 7; } while (b & 0x80);
  return v >>> 0;
}

// the side module's needs, from its `dylink.0` section: memory and table
// (size, alignment as a power of two)
function dylink(module) {
  const info = { memSize: 0, memAlign: 0, tableSize: 0 };
  const sec = WebAssembly.Module.customSections(module, 'dylink.0')[0];
  if (!sec) return info;
  const bytes = new Uint8Array(sec), at = { i: 0 };
  while (at.i < bytes.length) {
    const type = bytes[at.i++], len = leb(bytes, at), end = at.i + len;
    if (type === 1) {
      info.memSize = leb(bytes, at); info.memAlign = leb(bytes, at);
      info.tableSize = leb(bytes, at);
    }
    at.i = end;
  }
  return info;
}

// a function's slot in the REPL's table, one per function
const slots = new Map();
function slotOf(fn) {
  let i = slots.get(fn);
  if (i === undefined) {
    const table = repl.exports.__indirect_function_table;
    i = table.length;
    table.grow(1);
    table.set(i, fn);
    slots.set(fn, i);
  }
  return i;
}

// an entry's module: its data on a block of the REPL's heap (never freed --
// a value of the session may point into it), its functions behind the
// REPL's in the table, the stack the REPL's
function run(pathAt, argc, argvAt) {
  const module = new WebAssembly.Module(readFileSync(cString(pathAt)));
  const ex = repl.exports, table = ex.__indirect_function_table;
  const info = dylink(module);
  const align = 2 ** info.memAlign;
  let memBase = 0;
  if (info.memSize > 0) {
    const raw = ex.malloc(info.memSize + align);
    if (raw === 0) throw new Error('entry: no memory for its data');
    memBase = Math.ceil(raw / align) * align;
    new Uint8Array(ex.memory.buffer, memBase, info.memSize).fill(0);
  }
  const tableBase = table.length;
  if (info.tableSize > 0) table.grow(info.tableSize);
  const envImports = {
    memory: ex.memory,
    __indirect_function_table: table,
    __stack_pointer: ex.__stack_pointer,
    __memory_base: new WebAssembly.Global({ value: 'i32' }, memBase),
    __table_base: new WebAssembly.Global({ value: 'i32' }, tableBase),
  };
  const got = { 'GOT.mem': {}, 'GOT.func': {} }, late = [], missing = [];
  for (const imp of WebAssembly.Module.imports(module)) {
    const have = ex[imp.name];
    if (imp.module === 'env') {
      if (imp.name in envImports) continue;
      if (have === undefined) missing.push(imp.name);
      else envImports[imp.name] = have;
    } else if (imp.module in got) {
      const cell = new WebAssembly.Global({ value: 'i32', mutable: true }, 0);
      got[imp.module][imp.name] = cell;
      if (imp.module === 'GOT.mem' && have instanceof WebAssembly.Global) cell.value = have.value;
      else if (imp.module === 'GOT.func' && typeof have === 'function') cell.value = slotOf(have);
      else late.push(imp);
    } else missing.push(`${imp.module}.${imp.name}`);
  }
  if (missing.length > 0) {
    process.stderr.write(`entry: not in the REPL: ${missing.join(', ')}\n`);
    return 127;
  }
  const instance = new WebAssembly.Instance(module, { env: envImports, ...got });
  // what the module defines and reaches through its GOT all the same
  for (const imp of late) {
    const own = instance.exports[imp.name];
    if (imp.module === 'GOT.mem' && own instanceof WebAssembly.Global) got[imp.module][imp.name].value = memBase + own.value;
    else if (imp.module === 'GOT.func' && typeof own === 'function') got[imp.module][imp.name].value = slotOf(own);
    else {
      process.stderr.write(`entry: not in the REPL: ${imp.module}.${imp.name}\n`);
      return 127;
    }
  }
  instance.exports.__wasm_apply_data_relocs?.();
  instance.exports.__wasm_call_ctors?.();
  return instance.exports.__main_argc_argv(argc, argvAt);
}

const wasi = new WASI({ version: 'preview1', args: argv.slice(2), env: { ...env, PWD: cwd() }, preopens: { '/': '/' }, returnOnExit: true });
const module = await WebAssembly.compile(await readFile(argv[2]));
repl = await WebAssembly.instantiate(module, { ...wasi.getImportObject(), scaly: { run } });
exit(wasi.start(repl));
