// A Scaly program in a web page: the WASI preview 1 host it runs on.
//
// `scaly build --target wasm32-wasip1` writes a WASI module, and the runtime
// reaches the world only through the calls of WASI preview 1 (eio_wasm.c and
// wasi-libc). A page has no files and no standard streams, so this module is
// what a page offers in their place: a file system held in memory, which the
// page fills before the run and reads after it, standard input as bytes,
// standard output and error handed to the page as they are written. The
// program itself does not change -- the same module runs under node's WASI on
// the command line (tests/wasm/run.mjs) and here.
//
//   import { MemoryFiles, Wasi } from './wasi.mjs';
//   const files = new MemoryFiles({ '/work/doc.sgm': text, '/work/doc.dtd': dtd });
//   const wasi = new Wasi({ args: ['onsgmls', 'doc.sgm'], cwd: '/work', files });
//   const instance = await WebAssembly.instantiate(module, wasi.imports());
//   const status = wasi.start(instance);   // the exit code
//   wasi.stdout_text(), wasi.stderr_text(), files.read('/work/out.txt'),
//   files.written   // the paths the program created or wrote, in order
//   files.made      // the directories it made
//   files.removed   // the files it removed or renamed away (and did not write again)
//
// One instance runs once (a module's memory holds what its run left behind);
// compile the module once and instantiate it per run.
//
// Every call is synchronous, as WASI's are: a file the program opens must be in
// the MemoryFiles before the run. `fallback(path)` is asked for a path that is
// not there -- a Uint8Array for a file, the string 'dir' for a directory, null
// for nothing -- and what it answers is kept; `fallback_list(path)` adds the
// names of a directory's entries to those the MemoryFiles holds (a program
// that lists a directory: dazzle looking for its font files), and
// `fallback_kind(path)` says what an entry is -- 'dir', 'file' or null --
// without its content, which a listing does not need (without it, an entry's
// kind is asked of `fallback`). The command-line runner (run.mjs) reads the
// host's disk through all three. Nothing here waits: a clock
// subscription of poll_oneoff answers at once (a page cannot sleep on its main
// thread), a descriptor is always ready.

const ERRNO = { SUCCESS: 0, BADF: 8, EXIST: 20, INVAL: 28, ISDIR: 31, NOENT: 44, NOSYS: 52, NOTDIR: 54, NOTEMPTY: 55, SPIPE: 70 };
const FILETYPE = { UNKNOWN: 0, CHARACTER_DEVICE: 2, DIRECTORY: 3, REGULAR_FILE: 4 };
const OFLAGS = { CREAT: 1, DIRECTORY: 2, EXCL: 4, TRUNC: 8 };
const FDFLAGS_APPEND = 1;
const RIGHTS_ALL = 0xFFFFFFFFFFFFFFFFn;

const encoder = new TextEncoder();
const decoder = new TextDecoder();

function bytes_of(content) {
  if (content instanceof Uint8Array) return content;
  if (content instanceof ArrayBuffer) return new Uint8Array(content);
  return encoder.encode(String(content));
}

// "/a/./b/../c" -> "/a/c"; a path that climbs above the root stays at the root
function normalize(path) {
  const parts = [];
  for (const part of path.split('/')) {
    if (part === '' || part === '.') continue;
    if (part === '..') parts.pop();
    else parts.push(part);
  }
  return '/' + parts.join('/');
}

// The file system: absolute paths to file contents. A directory is a path some
// file lies under, or one made explicitly; `/` always is one.
export class MemoryFiles {
  constructor(files = {}, { fallback = null, fallback_list = null, fallback_kind = null } = {}) {
    this.files = new Map();      // path -> { data: Uint8Array, size }
    this.dirs = new Set(['/']);
    this.fallback = fallback;
    this.fallback_list = fallback_list;
    this.fallback_kind = fallback_kind;
    this.written = [];           // paths the program created, truncated or wrote
    this.made = [];              // directories the program made, in order
    this.removed = [];           // files it removed or renamed away
    for (const [path, content] of Object.entries(files)) this.write(path, content);
  }

  write(path, content) {
    const p = normalize(path);
    const data = bytes_of(content);
    this.files.set(p, { data, size: data.length });
    for (let d = p.slice(0, p.lastIndexOf('/')) || '/'; ; d = d.slice(0, d.lastIndexOf('/')) || '/') {
      this.dirs.add(d);
      if (d === '/') break;
    }
  }

  note_written(p) {
    if (!this.written.includes(p)) this.written.push(p);
    this.removed = this.removed.filter(x => x !== p);
  }

  // a file goes to another path (a file there is replaced)
  rename(p, q) {
    const e = this.lookup(p);
    this.files.delete(p);
    this.files.set(q, e);
    this.forget(p);
    this.note_written(q);
  }

  remove(p) {
    this.files.delete(p);
    this.forget(p);
  }

  forget(p) {
    this.written = this.written.filter(x => x !== p);
    if (!this.removed.includes(p)) this.removed.push(p);
  }

  // a file's bytes, or null
  read(path) {
    const f = this.lookup(normalize(path));
    return f && f !== 'dir' ? f.data.subarray(0, f.size) : null;
  }

  text(path) {
    const b = this.read(path);
    return b === null ? null : decoder.decode(b);
  }

  // the paths of the files, sorted
  paths() {
    return [...this.files.keys()].sort();
  }

  // the names of a directory's entries, sorted: what the files and directories
  // held lie under, and what fallback_list adds
  list(path) {
    const p = normalize(path);
    const prefix = p === '/' ? '/' : p + '/';
    const names = new Set();
    for (const q of [...this.files.keys(), ...this.dirs])
      if (q.startsWith(prefix) && q.length > prefix.length)
        names.add(q.slice(prefix.length).split('/')[0]);
    if (this.fallback_list)
      for (const name of this.fallback_list(p) ?? []) names.add(name);
    return [...names].sort();
  }

  // what a normalized path is -- 'dir', 'file' or null -- without reading a
  // file the MemoryFiles do not hold yet
  kind(p) {
    if (this.files.has(p)) return 'file';
    if (this.dirs.has(p)) return 'dir';
    for (const q of this.files.keys())
      if (q.startsWith(p + '/')) return 'dir';
    if (this.fallback_kind) {
      const k = this.fallback_kind(p);
      if (k === 'dir') this.dirs.add(p);
      return k === 'dir' || k === 'file' ? k : null;
    }
    const e = this.lookup(p);
    return e === 'dir' ? 'dir' : e ? 'file' : null;
  }

  // the entry for a normalized path: a file record, 'dir', or null
  lookup(p) {
    const f = this.files.get(p);
    if (f) return f;
    if (this.dirs.has(p)) return 'dir';
    for (const q of this.files.keys())
      if (q.startsWith(p + '/')) return 'dir';
    if (this.fallback) {
      const got = this.fallback(p);
      if (got === 'dir') { this.dirs.add(p); return 'dir'; }
      if (got) { this.write(p, got); return this.files.get(p); }
    }
    return null;
  }
}

class WasiExit extends Error {
  constructor(code) { super('exit ' + code); this.code = code; }
}

export class Wasi {
  // args: argv, the program's name first. env: an object. cwd: the working
  // directory (handed in as PWD, which the runtime adopts). stdin: bytes or a
  // string, or a function answering them, asked when the program first reads
  // its standard input (a program that never does is not kept waiting for it).
  // stdin_more: a function asked each time the program has read all it was
  // given, answering the next bytes -- empty at the end of the input: for a
  // program that reads its input as it comes (the REPL in a worker, which
  // waits there for the next line). stdout/stderr: called with each written
  // Uint8Array; without them the output is kept (stdout_text(), stderr_text()).
  constructor({ args = ['program'], env = {}, cwd = '/', files = new MemoryFiles(), stdin = new Uint8Array(),
                stdin_more = null, stdout = null, stderr = null } = {}) {
    this.args = args.map(a => encoder.encode(a + '\0'));
    this.env = Object.entries({ ...env, PWD: cwd }).map(([k, v]) => encoder.encode(k + '=' + v + '\0'));
    this.files = files;
    this.stdin_source = stdin;
    this.stdin = null;
    this.stdin_pos = 0;
    this.stdin_more = stdin_more;
    this.out = [[], []];
    this.sinks = [stdout, stderr];
    // fd 0-2 the streams, fd 3 the one preopened directory: the root
    this.fds = [{ kind: 'stdin' }, { kind: 'out', n: 0 }, { kind: 'out', n: 1 }, { kind: 'dir', path: '/' }];
    this.memory = null;
  }

  // run the instance's `_start`; the exit code
  start(instance) {
    this.memory = instance.exports.memory;
    try {
      instance.exports._start();
      return 0;
    } catch (e) {
      if (e instanceof WasiExit) return e.code;
      throw e;
    }
  }

  stdout_bytes() { return concat(this.out[0]); }
  stderr_bytes() { return concat(this.out[1]); }
  stdout_text() { return decoder.decode(this.stdout_bytes()); }
  stderr_text() { return decoder.decode(this.stderr_bytes()); }

  // standard input as far as it is there, asked for when first read
  stdin_ready(more) {
    if (this.stdin === null)
      this.stdin = bytes_of(typeof this.stdin_source === 'function' ? this.stdin_source() : this.stdin_source);
    if (more && this.stdin_more && this.stdin_pos >= this.stdin.length) {
      const next = bytes_of(this.stdin_more());
      if (next.length === 0) this.stdin_more = null;
      this.stdin = next;
      this.stdin_pos = 0;
    }
  }

  view() { return new DataView(this.memory.buffer); }
  mem() { return new Uint8Array(this.memory.buffer); }
  string_at(ptr, len) { return decoder.decode(this.mem().subarray(ptr, ptr + len)); }

  // a path handed to a call, against the directory descriptor it names
  resolve(dirfd, ptr, len) {
    const d = this.fds[dirfd];
    if (!d || d.kind !== 'dir') return null;
    return normalize(d.path + '/' + this.string_at(ptr, len));
  }

  put_list(items, list_ptr, buf_ptr) {
    const v = this.view(), m = this.mem();
    for (const item of items) {
      v.setUint32(list_ptr, buf_ptr, true);
      list_ptr += 4;
      m.set(item, buf_ptr);
      buf_ptr += item.length;
    }
    return ERRNO.SUCCESS;
  }

  put_sizes(items, count_ptr, size_ptr) {
    const v = this.view();
    v.setUint32(count_ptr, items.length, true);
    v.setUint32(size_ptr, items.reduce((s, i) => s + i.length, 0), true);
    return ERRNO.SUCCESS;
  }

  put_filestat(ptr, type, size) {
    const v = this.view();
    for (let i = 0; i < 64; i += 8) v.setBigUint64(ptr + i, 0n, true);
    v.setUint8(ptr + 16, type);
    v.setBigUint64(ptr + 24, 1n, true);
    v.setBigUint64(ptr + 32, BigInt(size), true);
  }

  imports() {
    const w = this;
    const wasi = {
      args_sizes_get: (count_ptr, size_ptr) => w.put_sizes(w.args, count_ptr, size_ptr),
      args_get: (list_ptr, buf_ptr) => w.put_list(w.args, list_ptr, buf_ptr),
      environ_sizes_get: (count_ptr, size_ptr) => w.put_sizes(w.env, count_ptr, size_ptr),
      environ_get: (list_ptr, buf_ptr) => w.put_list(w.env, list_ptr, buf_ptr),

      clock_time_get(id, precision, time_ptr) {
        const ns = id === 0 ? BigInt(Date.now()) * 1000000n : BigInt(Math.round(performance.now() * 1e6));
        w.view().setBigUint64(time_ptr, ns, true);
        return ERRNO.SUCCESS;
      },

      fd_prestat_get(fd, ptr) {
        if (fd !== 3) return ERRNO.BADF;
        const v = w.view();
        v.setUint8(ptr, 0);
        v.setUint32(ptr + 4, 1, true);
        return ERRNO.SUCCESS;
      },
      fd_prestat_dir_name(fd, ptr, len) {
        if (fd !== 3) return ERRNO.BADF;
        if (len >= 1) w.mem()[ptr] = 0x2F;
        return ERRNO.SUCCESS;
      },

      fd_fdstat_get(fd, ptr) {
        const f = w.fds[fd];
        if (!f) return ERRNO.BADF;
        const v = w.view();
        v.setUint8(ptr, f.kind === 'dir' ? FILETYPE.DIRECTORY : f.kind === 'file' ? FILETYPE.REGULAR_FILE : FILETYPE.UNKNOWN);
        v.setUint16(ptr + 2, f.append ? FDFLAGS_APPEND : 0, true);
        v.setBigUint64(ptr + 8, RIGHTS_ALL, true);
        v.setBigUint64(ptr + 16, RIGHTS_ALL, true);
        return ERRNO.SUCCESS;
      },
      fd_fdstat_set_flags(fd, flags) {
        const f = w.fds[fd];
        if (!f) return ERRNO.BADF;
        f.append = (flags & FDFLAGS_APPEND) !== 0;
        return ERRNO.SUCCESS;
      },
      fd_filestat_get(fd, ptr) {
        const f = w.fds[fd];
        if (!f) return ERRNO.BADF;
        if (f.kind === 'file') w.put_filestat(ptr, FILETYPE.REGULAR_FILE, f.entry.size);
        else if (f.kind === 'dir') w.put_filestat(ptr, FILETYPE.DIRECTORY, 0);
        else w.put_filestat(ptr, FILETYPE.UNKNOWN, 0);
        return ERRNO.SUCCESS;
      },
      // a file made longer (zeros) or shorter
      fd_filestat_set_size(fd, size) {
        const f = w.fds[fd];
        if (!f) return ERRNO.BADF;
        if (f.kind !== 'file') return ERRNO.INVAL;
        const e = f.entry, n = Number(size);
        if (n > e.data.length) {
          const grown = new Uint8Array(n);
          grown.set(e.data.subarray(0, e.size));
          e.data = grown;
        } else if (n > e.size) e.data.fill(0, e.size, n);
        e.size = n;
        w.files.note_written(f.path);
        return ERRNO.SUCCESS;
      },
      path_filestat_get(fd, flags, path_ptr, path_len, ptr) {
        const p = w.resolve(fd, path_ptr, path_len);
        if (p === null) return ERRNO.BADF;
        const e = w.files.lookup(p);
        if (!e) return ERRNO.NOENT;
        if (e === 'dir') w.put_filestat(ptr, FILETYPE.DIRECTORY, 0);
        else w.put_filestat(ptr, FILETYPE.REGULAR_FILE, e.size);
        return ERRNO.SUCCESS;
      },

      path_open(fd, dirflags, path_ptr, path_len, oflags, rights_base, rights_inheriting, fdflags, fd_ptr) {
        const p = w.resolve(fd, path_ptr, path_len);
        if (p === null) return ERRNO.BADF;
        let e = w.files.lookup(p);
        if (e && (oflags & OFLAGS.CREAT) && (oflags & OFLAGS.EXCL)) return ERRNO.EXIST;
        if (e === 'dir') {
          if (oflags & (OFLAGS.CREAT | OFLAGS.TRUNC)) return ERRNO.ISDIR;
          w.view().setUint32(fd_ptr, w.fds.push({ kind: 'dir', path: p }) - 1, true);
          return ERRNO.SUCCESS;
        }
        if (oflags & OFLAGS.DIRECTORY) return e ? ERRNO.NOTDIR : ERRNO.NOENT;
        if (!e) {
          if (!(oflags & OFLAGS.CREAT)) return ERRNO.NOENT;
          const parent = p.slice(0, p.lastIndexOf('/')) || '/';
          if (w.files.lookup(parent) !== 'dir') return ERRNO.NOENT;
          w.files.write(p, new Uint8Array());
          e = w.files.lookup(p);
          w.files.note_written(p);
        } else if (oflags & OFLAGS.TRUNC) {
          e.size = 0;
          w.files.note_written(p);
        }
        const f = { kind: 'file', path: p, entry: e, pos: 0, append: (fdflags & FDFLAGS_APPEND) !== 0 };
        w.view().setUint32(fd_ptr, w.fds.push(f) - 1, true);
        return ERRNO.SUCCESS;
      },
      // the entries of a directory: a record each (the cookie of the next, an
      // inode, the name's length, the type, then the name), as many as fit;
      // a buffer filled to its end tells the caller to ask again from the
      // cookie of the last whole one
      fd_readdir(fd, buf, buf_len, cookie, bufused_ptr) {
        const f = w.fds[fd];
        if (!f) return ERRNO.BADF;
        if (f.kind !== 'dir') return ERRNO.NOTDIR;
        const names = w.files.list(f.path);
        const m = w.mem();
        let used = 0;
        for (let i = Number(cookie); i < names.length && used < buf_len; i++) {
          const name = encoder.encode(names[i]);
          const k = w.files.kind(normalize(f.path + '/' + names[i]));
          const rec = new Uint8Array(24 + name.length);
          const v = new DataView(rec.buffer);
          v.setBigUint64(0, BigInt(i + 1), true);
          v.setBigUint64(8, BigInt(i + 1), true);
          v.setUint32(16, name.length, true);
          v.setUint8(20, k === 'dir' ? FILETYPE.DIRECTORY : k === 'file' ? FILETYPE.REGULAR_FILE : FILETYPE.UNKNOWN);
          rec.set(name, 24);
          const n = Math.min(rec.length, buf_len - used);
          m.set(rec.subarray(0, n), buf + used);
          used += n;
        }
        w.view().setUint32(bufused_ptr, used, true);
        return ERRNO.SUCCESS;
      },
      path_create_directory(fd, path_ptr, path_len) {
        const p = w.resolve(fd, path_ptr, path_len);
        if (p === null) return ERRNO.BADF;
        if (w.files.lookup(p)) return ERRNO.EXIST;
        const parent = p.slice(0, p.lastIndexOf('/')) || '/';
        if (w.files.lookup(parent) !== 'dir') return ERRNO.NOENT;
        w.files.dirs.add(p);
        w.files.made.push(p);
        return ERRNO.SUCCESS;
      },
      path_rename(fd, old_ptr, old_len, new_fd, new_ptr, new_len) {
        const p = w.resolve(fd, old_ptr, old_len), q = w.resolve(new_fd, new_ptr, new_len);
        if (p === null || q === null) return ERRNO.BADF;
        const e = w.files.lookup(p);
        if (!e) return ERRNO.NOENT;
        if (e === 'dir' || w.files.lookup(q) === 'dir') return ERRNO.ISDIR;
        const parent = q.slice(0, q.lastIndexOf('/')) || '/';
        if (w.files.lookup(parent) !== 'dir') return ERRNO.NOENT;
        if (p !== q) w.files.rename(p, q);
        return ERRNO.SUCCESS;
      },
      // there are no symbolic links here: what is there is no link
      path_readlink(fd, path_ptr, path_len, buf, buf_len, bufused_ptr) {
        const p = w.resolve(fd, path_ptr, path_len);
        if (p === null) return ERRNO.BADF;
        return w.files.lookup(p) ? ERRNO.INVAL : ERRNO.NOENT;
      },
      // an empty directory the program made or found
      path_remove_directory(fd, path_ptr, path_len) {
        const p = w.resolve(fd, path_ptr, path_len);
        if (p === null) return ERRNO.BADF;
        const e = w.files.lookup(p);
        if (!e) return ERRNO.NOENT;
        if (e !== 'dir') return ERRNO.NOTDIR;
        if (w.files.list(p).length > 0) return ERRNO.NOTEMPTY;
        w.files.dirs.delete(p);
        w.files.made = w.files.made.filter(q => q !== p);
        return ERRNO.SUCCESS;
      },
      path_unlink_file(fd, path_ptr, path_len) {
        const p = w.resolve(fd, path_ptr, path_len);
        if (p === null) return ERRNO.BADF;
        const e = w.files.lookup(p);
        if (!e) return ERRNO.NOENT;
        if (e === 'dir') return ERRNO.ISDIR;
        w.files.remove(p);
        return ERRNO.SUCCESS;
      },
      fd_close(fd) {
        if (!w.fds[fd]) return ERRNO.BADF;
        if (fd > 3) w.fds[fd] = undefined;
        return ERRNO.SUCCESS;
      },

      fd_read(fd, iovs, iovs_len, nread_ptr) {
        const f = w.fds[fd];
        if (!f) return ERRNO.BADF;
        if (f.kind === 'dir') return ERRNO.ISDIR;
        if (f.kind === 'out') return ERRNO.BADF;
        const v = w.view(), m = w.mem();
        if (f.kind === 'stdin') w.stdin_ready(true);
        const src = f.kind === 'stdin' ? w.stdin : f.entry.data;
        const end = f.kind === 'stdin' ? w.stdin.length : f.entry.size;
        let pos = f.kind === 'stdin' ? w.stdin_pos : f.pos;
        let total = 0;
        for (let i = 0; i < iovs_len; i++) {
          const buf = v.getUint32(iovs + 8 * i, true), len = v.getUint32(iovs + 8 * i + 4, true);
          const n = Math.min(len, end - pos);
          m.set(src.subarray(pos, pos + n), buf);
          pos += n;
          total += n;
          if (n < len) break;
        }
        if (f.kind === 'stdin') w.stdin_pos = pos; else f.pos = pos;
        v.setUint32(nread_ptr, total, true);
        return ERRNO.SUCCESS;
      },
      // a read at an offset that leaves the position where it is (wasi-libc's
      // emulated mmap reads a mapped file so)
      fd_pread(fd, iovs, iovs_len, offset, nread_ptr) {
        const f = w.fds[fd];
        if (!f) return ERRNO.BADF;
        if (f.kind === 'dir') return ERRNO.ISDIR;
        if (f.kind !== 'file') return ERRNO.SPIPE;
        const v = w.view(), m = w.mem();
        let pos = Number(offset), total = 0;
        for (let i = 0; i < iovs_len; i++) {
          const buf = v.getUint32(iovs + 8 * i, true), len = v.getUint32(iovs + 8 * i + 4, true);
          const n = Math.max(0, Math.min(len, f.entry.size - pos));
          m.set(f.entry.data.subarray(pos, pos + n), buf);
          pos += n;
          total += n;
          if (n < len) break;
        }
        v.setUint32(nread_ptr, total, true);
        return ERRNO.SUCCESS;
      },
      fd_write(fd, iovs, iovs_len, nwritten_ptr) {
        const f = w.fds[fd];
        if (!f) return ERRNO.BADF;
        if (f.kind === 'dir') return ERRNO.ISDIR;
        if (f.kind === 'stdin') return ERRNO.BADF;
        const v = w.view(), m = w.mem();
        let total = 0;
        for (let i = 0; i < iovs_len; i++) {
          const buf = v.getUint32(iovs + 8 * i, true), len = v.getUint32(iovs + 8 * i + 4, true);
          const chunk = m.slice(buf, buf + len);
          if (f.kind === 'out') {
            if (w.sinks[f.n]) w.sinks[f.n](chunk); else w.out[f.n].push(chunk);
          } else {
            write_at(f, chunk);
            w.files.note_written(f.path);
          }
          total += len;
        }
        v.setUint32(nwritten_ptr, total, true);
        return ERRNO.SUCCESS;
      },
      fd_seek(fd, offset, whence, pos_ptr) {
        const f = w.fds[fd];
        if (!f) return ERRNO.BADF;
        if (f.kind !== 'file') return ERRNO.SPIPE;
        const base = whence === 0 ? 0 : whence === 1 ? f.pos : f.entry.size;
        const to = base + Number(offset);
        if (to < 0) return ERRNO.INVAL;
        f.pos = to;
        w.view().setBigUint64(pos_ptr, BigInt(to), true);
        return ERRNO.SUCCESS;
      },

      // every subscription is ready at once: a descriptor never blocks here,
      // and a clock is taken as elapsed (a page cannot sleep)
      poll_oneoff(in_ptr, out_ptr, nsubscriptions, nevents_ptr) {
        const v = w.view();
        w.stdin_ready(false);
        for (let i = 0; i < nsubscriptions; i++) {
          const s = in_ptr + 48 * i, e = out_ptr + 32 * i;
          const tag = v.getUint8(s + 8);
          v.setBigUint64(e, v.getBigUint64(s, true), true);
          v.setUint16(e + 8, ERRNO.SUCCESS, true);
          v.setUint8(e + 10, tag);
          v.setBigUint64(e + 16, tag === 0 ? 0n : 0x10000n, true);
          v.setUint16(e + 24, 0, true);
        }
        v.setUint32(nevents_ptr, nsubscriptions, true);
        return ERRNO.SUCCESS;
      },

      proc_exit(code) { throw new WasiExit(code); },
      sched_yield: () => ERRNO.SUCCESS,
      random_get(ptr, len) {
        crypto.getRandomValues(w.mem().subarray(ptr, ptr + len));
        return ERRNO.SUCCESS;
      },
    };
    // a call the runtime does not make: answered "not supported", never a
    // missing import that would refuse the module
    return {
      wasi_snapshot_preview1: new Proxy(wasi, {
        get: (target, name) => target[name] ?? (() => ERRNO.NOSYS),
      }),
    };
  }
}

// write a chunk at an open file's position (its end when appending), growing it
function write_at(f, chunk) {
  const e = f.entry;
  if (f.append) f.pos = e.size;
  const need = f.pos + chunk.length;
  if (need > e.data.length) {
    const grown = new Uint8Array(Math.max(need, e.data.length * 2, 4096));
    grown.set(e.data.subarray(0, e.size));
    e.data = grown;
  }
  e.data.set(chunk, f.pos);
  f.pos = need;
  if (need > e.size) e.size = need;
}

function concat(chunks) {
  const out = new Uint8Array(chunks.reduce((s, c) => s + c.length, 0));
  let at = 0;
  for (const c of chunks) { out.set(c, at); at += c.length; }
  return out;
}
