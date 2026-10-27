// The REPL's side modules: an entry the compiler built for WebAssembly writes
// (Emitter.wasm_run) is a position-independent module (`wasm-ld -shared
// --experimental-pic`) that brings no library of its own. Loaded here into
// the REPL's own memory and table, it calls the standard library and the C
// library of the REPL's module and keeps its values on the REPL's pages, as
// an entry does under the native JIT. The same loader serves the page's
// worker (worker.mjs) and the command line (tools/wasm/repl-node.mjs).
//
//   const side = new SideModules(path => bytes);       // reads a written module
//   const repl = await WebAssembly.instantiate(module, { ...wasi.imports(), ...side.imports() });
//   side.attach(repl);
//
// The REPL's module must export what an entry imports: its memory, its table
// (growable), its `__stack_pointer`, `malloc`, and every definition an entry
// may call (tools/wasm/cc.sh links it so). An entry's module is compiled
// synchronously, as the import it answers is a synchronous call: in a page
// that is allowed in a worker only.

function leb(bytes, at) {
  let v = 0, shift = 0, b;
  do { b = bytes[at.i++]; v |= (b & 0x7f) << shift; shift += 7; } while (b & 0x80);
  return v >>> 0;
}

// a module's needs from its `dylink.0` section: memory and table (size,
// alignment as a power of two)
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

export class SideModules {
  // read(path): the bytes of the module the REPL wrote at `path`.
  // warn(text): where a module that cannot be loaded is reported
  constructor(read, warn = text => console.error(text)) {
    this.read = read;
    this.warn = warn;
    this.repl = null;
    this.slots = new Map();     // a function's slot in the REPL's table, one per function
  }

  attach(instance) { this.repl = instance; }

  imports() {
    return { scaly: { run: (path_at, argc, argv_at) => this.run(path_at, argc, argv_at) } };
  }

  c_string(at) {
    const bytes = new Uint8Array(this.repl.exports.memory.buffer);
    let end = at;
    while (bytes[end] !== 0) end++;
    return new TextDecoder().decode(bytes.subarray(at, end));
  }

  slot_of(fn) {
    let i = this.slots.get(fn);
    if (i === undefined) {
      const table = this.repl.exports.__indirect_function_table;
      i = table.length;
      table.grow(1);
      table.set(i, fn);
      this.slots.set(fn, i);
    }
    return i;
  }

  // `scaly.run(path, argc, argv)`: the entry's data on a block of the REPL's
  // heap (never freed -- a value of the session may point into it), its
  // functions behind the REPL's in the table, the stack the REPL's; its
  // `__main_argc_argv(argc, argv)` answers -- `__main_void()` for a program of
  // top-level statements --, 127 where it cannot be loaded
  run(path_at, argc, argv_at) {
    const module = new WebAssembly.Module(this.read(this.c_string(path_at)));
    const ex = this.repl.exports, table = ex.__indirect_function_table;
    const info = dylink(module);
    const align = 2 ** info.memAlign;
    let mem_base = 0;
    if (info.memSize > 0) {
      const raw = ex.malloc(info.memSize + align);
      if (raw === 0) {
        this.warn('entry: no memory for its data');
        return 127;
      }
      mem_base = Math.ceil(raw / align) * align;
      new Uint8Array(ex.memory.buffer, mem_base, info.memSize).fill(0);
    }
    const table_base = table.length;
    if (info.tableSize > 0) table.grow(info.tableSize);
    const env = {
      memory: ex.memory,
      __indirect_function_table: table,
      __stack_pointer: ex.__stack_pointer,
      __memory_base: new WebAssembly.Global({ value: 'i32' }, mem_base),
      __table_base: new WebAssembly.Global({ value: 'i32' }, table_base),
    };
    const got = { 'GOT.mem': {}, 'GOT.func': {} }, late = [], missing = [];
    for (const imp of WebAssembly.Module.imports(module)) {
      const have = ex[imp.name];
      if (imp.module === 'env') {
        if (imp.name in env) continue;
        if (have === undefined) missing.push(imp.name);
        else env[imp.name] = have;
      } else if (imp.module in got) {
        const cell = new WebAssembly.Global({ value: 'i32', mutable: true }, 0);
        got[imp.module][imp.name] = cell;
        if (imp.module === 'GOT.mem' && have instanceof WebAssembly.Global) cell.value = have.value;
        else if (imp.module === 'GOT.func' && typeof have === 'function') cell.value = this.slot_of(have);
        else late.push(imp);
      } else missing.push(`${imp.module}.${imp.name}`);
    }
    if (missing.length > 0) {
      this.warn(`entry: not in the REPL: ${missing.join(', ')}`);
      return 127;
    }
    const instance = new WebAssembly.Instance(module, { env, ...got });
    // what the module defines and reaches through its GOT all the same
    for (const imp of late) {
      const own = instance.exports[imp.name];
      if (imp.module === 'GOT.mem' && own instanceof WebAssembly.Global) got[imp.module][imp.name].value = mem_base + own.value;
      else if (imp.module === 'GOT.func' && typeof own === 'function') got[imp.module][imp.name].value = this.slot_of(own);
      else {
        this.warn(`entry: not in the REPL: ${imp.module}.${imp.name}`);
        return 127;
      }
    }
    instance.exports.__wasm_apply_data_relocs?.();
    instance.exports.__wasm_call_ctors?.();
    const { __main_argc_argv, __main_void } = instance.exports;
    if (__main_argc_argv) return __main_argc_argv(argc, argv_at);
    if (__main_void) return __main_void();
    this.warn('entry: no __main_argc_argv and no __main_void');
    return 127;
  }
}
