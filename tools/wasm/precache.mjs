// tools/wasm/precache.mjs <scaly.wasm> <tree> <out.json> [target] -- what the
// compiler in a page needs to BUILD a program for `target` (default
// wasm32-wasip1-threads) and not only to run one: the standard library
// compiled for it, as that compiler's build cache holds it, and what its link
// takes -- written as one JSON, path -> base64, as web/repl/stdlib.mjs writes
// the sources (web/repl/build.sh).
//
// A package in the cache is keyed by the compiler that compiled it (its
// stamp: a hash of its own code), so the cache cannot be made by the native
// tool: this runs the compiler built for WebAssembly itself, under node,
// through the page's own hosts, in the page's file system -- SCALY_HOME
// /scaly (read from <tree>), the program under /work, the cache /cache, all
// in memory -- `scaly build --target <target> --gpu` of a small program. What
// that writes under /cache is the cache the page carries. Beside it:
//
//   /scaly/packages/scaly/<v>/_native/<os>-<arch>/   the standard library's C
//       files, compiled for the target by the wasi-sdk's clang: a page has no
//       C compiler, and the tool links a ready-made object where it finds one
//       (tool.native_objects#)
//   /scaly/wasi/<target>/   the start code, the C library and the compiler's
//       builtins, which cli.wasm_link# hands wasm-ld
//
// The program the build made is then run (packages/scaly/<v>/web/run.mjs):
// it must print what it is written to print, on every core it is given.
//
// Needs a wasi-sdk ($WASI_SDK, else ~/opt/wasi-sdk-*).
import { execFileSync } from 'node:child_process';
import { existsSync, mkdtempSync, readFileSync, readdirSync, rmSync, statSync, writeFileSync } from 'node:fs';
import { tmpdir } from 'node:os';
import { join } from 'node:path';
import process from 'node:process';

const [wasm, tree, out, target = 'wasm32-wasip1-threads'] = process.argv.slice(2);
const sdk = process.env.WASI_SDK || readdirSync(join(process.env.HOME, 'opt')).filter(d => d.startsWith('wasi-sdk-'))
  .map(d => join(process.env.HOME, 'opt', d)).sort().pop();
if (!sdk || !existsSync(join(sdk, 'bin/clang'))) {
  console.error('precache: no wasi-sdk (set WASI_SDK)');
  process.exit(3);
}
const version = execFileSync(join(tree, 'tools/version.sh'), ['scaly'], { encoding: 'utf8' }).trim();
const std = `packages/scaly/${version}`;
const arch = target.endsWith('-threads') ? 'wasm32-threads' : 'wasm32';
const scratch = mkdtempSync(join(tmpdir(), 'scaly-precache-'));
const extra = {};          // what the page carries beside the sources

// the standard library's C files for this target, as the tool chooses them
// (tool.select_native#: per base name the file `_wasm` names, else the one
// that names no system) and compiles them (tool.native_objects#: -O2, the
// target, wasm's setjmp)
const sysroot = join(sdk, 'share/wasi-sysroot');
const chosen = new Map();
for (const file of execFileSync('find', [std, '-name', '*.c'], { cwd: tree, encoding: 'utf8' }).split('\n').filter(Boolean)) {
  const stem = file.slice(std.length + 1, -2);
  if (/_(windows|linux|darwin|arm64|x86_64|aarch64)$/.test(stem)) continue;
  const base = stem.replace(/_wasm$/, '');
  if (!chosen.has(base) || stem.endsWith('_wasm')) chosen.set(base, file);
}
for (const file of chosen.values()) {
  const stem = file.slice(std.length + 1, -2);
  const flat = (stem + '.c').replaceAll('/', '-');
  const object = join(scratch, flat + '.o');
  execFileSync(join(sdk, 'bin/clang'), [`--sysroot=${sysroot}`, `--target=${target}`, '-O2', '-c',
    join(tree, file), '-o', object, '-mllvm', '-wasm-enable-sjlj']);
  extra[`/scaly/${std}/_native/wasm-${arch}/${flat}.o`] = readFileSync(object);
}
// what the link takes
const libdir = join(sysroot, 'lib', target);
for (const name of ['crt1-command.o', 'libc.a', 'libsetjmp.a', 'libwasi-emulated-mman.a', 'libpthread.a'])
  if (existsSync(join(libdir, name))) extra[`/scaly/wasi/${target}/${name}`] = readFileSync(join(libdir, name));
const builtins = execFileSync('find', [join(sdk, 'lib/clang'), '-path', `*${target.replace('wasm32-', 'wasm32-unknown-')}/libclang_rt.builtins.a`], { encoding: 'utf8' }).trim().split('\n')[0];
extra[`/scaly/wasi/${target}/libclang_rt.builtins.a`] = readFileSync(builtins);

// the compiler, as the page runs it
const { MemoryFiles, Wasi } = await import(join(tree, std, 'web/wasi.mjs'));
const { SideModules } = await import(join(tree, 'web/repl/side.mjs'));
const program = `; precache.mjs's program: the standard library compiled for the page
var squares Vector[int](4000)
for i in 4000
    squares.put(i as size_t, i * i)
var sum 0
for i in 4000
    sum := sum + squares[i]
print "sum \`sum\`"
`;
const files = new MemoryFiles({ ...extra, '/work/main.scaly': program }, {
  fallback(path) {
    if (!path.startsWith('/scaly/')) return null;
    const p = join(tree, path.slice(7));
    try {
      const s = statSync(p);
      return s.isDirectory() ? 'dir' : s.isFile() ? readFileSync(p) : null;
    } catch { return null; }
  },
  fallback_list(path) {
    if (!path.startsWith('/scaly/')) return [];
    try { return readdirSync(join(tree, path.slice(7))); } catch { return []; }
  },
});
for (const dir of ['/tmp', '/work', '/home', '/cache']) files.dirs.add(dir);
const wasi = new Wasi({
  args: ['scaly', 'build', '--target', target, '--gpu', 'main.scaly', '-o', 'main.wasm'],
  env: { SCALY_HOME: '/scaly', HOME: '/home', SCALY_CACHE: '/cache' }, cwd: '/work', files,
  stdout: chunk => process.stdout.write(chunk), stderr: chunk => process.stderr.write(chunk),
});
const side = new SideModules(path => files.read(path));
const compiler = await WebAssembly.instantiate(await WebAssembly.compile(readFileSync(wasm)), { ...wasi.imports(), ...side.imports() });
side.attach(compiler);
const t0 = performance.now();
const status = wasi.start(compiler);
console.error(`precache: the build in ${Math.round(performance.now() - t0)} ms, status ${status}`);
const built = files.read('/work/main.wasm');
if (status !== 0 || !built) {
  rmSync(scratch, { recursive: true });
  console.error('precache: the compiler in the page could not build a program');
  process.exit(1);
}

// the program it made must run
writeFileSync(join(scratch, 'main.wasm'), built);
const ran = execFileSync(process.execPath, ['--no-warnings', join(tree, std, 'web/run.mjs'), join(scratch, 'main.wasm')], { encoding: 'utf8' });
rmSync(scratch, { recursive: true });
if (ran.trim() !== 'sum 21325334000') {
  console.error(`precache: the program the page built printed ${JSON.stringify(ran)}`);
  process.exit(1);
}

const bundle = {};
for (const [path, bytes] of Object.entries(extra)) bundle[path] = Buffer.from(bytes).toString('base64');
for (const path of files.written) if (path.startsWith('/cache/')) bundle[path] = Buffer.from(files.read(path)).toString('base64');
writeFileSync(out, JSON.stringify(bundle));
const size = Object.values(bundle).reduce((n, b) => n + b.length * 3 / 4, 0);
console.error(`precache: ${Object.keys(bundle).length} files, ${(size / 1048576).toFixed(1)} MB, into ${out}`);
