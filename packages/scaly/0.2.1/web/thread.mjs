// A worker for the threads of a Scaly program (threads.mjs): it makes an
// instance of the program's module over the program's memory and enters it
// at `wasi_thread_start(tid, arg)` -- one thread; then it waits for the next
// one the coordinator gives it. Runs as a page's module worker and as a
// node worker alike.
//
// Its WASI is wasi.mjs's WITHOUT FILES: the program's files are held by the
// program's worker, and that is waiting for its threads while they run --
// a thread that asked it for a file would wait for ever. A loop that reads
// files may run in parallel (the standard library's reading routines do not
// keep a loop in order), so a thread that names a path ENDS THE PROGRAM,
// saying so, instead of finding nothing there: the program would go on with
// an empty file. What a thread writes to its standard streams goes to the
// coordinator, as do its end -- `done` where the thread returned, `exit`
// where it ended the program -- and a trap.
import { Wasi } from './wasi.mjs';
import { ThreadHost } from './threads.mjs';

const node = typeof self === 'undefined';
const port = node ? (await import('node:worker_threads')).parentPort : null;
const post = node ? m => port.postMessage(m) : m => self.postMessage(m);
// In node a thread writes its standard streams itself, at once, as the
// program does: through the coordinator a thread's line came after the
// program's next one, or not at all where the program ended first. A page's
// thread has no stream of its own and posts it -- between the program and
// its threads a page keeps no order.
const write = node ? (await import('node:fs')).writeSync : null;
const listen = node ? f => port.on('message', f) : f => { self.onmessage = e => f(e.data); };

listen(async ({ module, memory, counter, tid, arg, env, net }) => {
  // a node of a cluster: its network, which its threads read and write too
  const network = net ? new (await import('./net.mjs')).Net(net.buffer, net.node) : null;
  const wasi = new Wasi({
    args: ['thread'], env, net: network,
    stdout: bytes => write ? write(1, bytes) : post({ type: 'scaly-thread-out', bytes }),
    stderr: bytes => write ? write(2, bytes) : post({ type: 'scaly-thread-err', bytes }),
    threads: new ThreadHost({ memory, post, counter }),
  });
  const imports = wasi.imports(module);
  const host = imports.wasi_snapshot_preview1;
  imports.wasi_snapshot_preview1 = new Proxy({}, {
    get: (_, name) => !String(name).startsWith('path_') ? host[name] : (...a) => {
      // the path follows the directory, or the directory and its flags
      const k = ['path_open', 'path_filestat_get', 'path_filestat_set_times', 'path_link'].includes(name) ? 2 : 1;
      throw new Error(`a thread of this program named the file ${wasi.string_at(a[k] >>> 0, a[k + 1] >>> 0)}: in this host only the program's main thread has its files`);
    },
  });
  let status;
  try {
    status = wasi.thread_start(await WebAssembly.instantiate(module, imports), tid, arg);
  } catch (e) {
    post({ type: 'scaly-thread-crash', text: String(e && e.stack || e) });
    return;
  }
  post(status === null ? { type: 'scaly-thread-done' } : { type: 'scaly-thread-exit', status });
});
