// A network for the nodes of a Scaly cluster in one host: a page whose
// workers are the nodes, or node's worker threads. WASI preview 1 opens no
// socket, so a program built for wasm32-wasip1-threads asks its host for one
// (the import module `scaly_net`, scaly/fiber/eio_wasm.c), and the host
// answers from here: listening ports and connections between the nodes, each
// connection two rings of bytes, one each way, in ONE SharedArrayBuffer that
// every node and every thread of every node is handed. The cluster's own
// code -- its handshake, frames, heartbeats, the regions it sends --
// runs unchanged over it; only TCP is not there.
//
//   const net = Net.create();                       // the coordinator
//   wasi = new Wasi({ ..., net: new Net(net.buffer, 1) });   // node 1's program and its threads
//   net.kill(1);                                    // node 1 is gone: its ends close
//
// One port space for all nodes: a node listens on a port, any node connects
// to it by number, whatever host it names. A call that would wait waits on
// one word of the buffer that changes with every change of the network --
// in a worker; a page's main thread never calls here. A node that is ended
// leaves its connections as a killed process leaves its sockets: the host
// closes them (kill), and the peers read the end.

const LISTENERS = 32;
const QUEUE = 16;               // connections a listener holds before it accepts them
const CONNECTIONS = 64;
const RING = 1 << 16;           // bytes each way of a connection
const FD = 0x40000000;          // the first descriptor of the network (eio_wasm.c says the same)
const LISTEN_FD = FD + 0x100000;
const FIRST_PORT = 49152;       // where a listener on port 0 is put

const NODES = 256;              // node numbers below this
const STEAL_MS = 50;            // how long a node that is gone may hold the lock

// the words of the buffer
const LOCK = 0, CHANGE = 1, NEXT_PORT = 2;      // LOCK: 0, or the node + 1 that holds it
const LB = 4, LW = 4 + QUEUE;                    // a listener: port, owner + 1, count, head, queue
const CB = LB + LISTENERS * LW, CW = 9;          // a connection: state, owner + 1 of each side, closed of each side, read and written of each ring
const C_STATE = 0, C_OWNER = 1, C_CLOSED = 3, C_RING = 5;
const GONE = CB + CONNECTIONS * CW;              // a word a node: 1 once it is gone
const DATA = (GONE + NODES) * 4;
const BYTES = DATA + CONNECTIONS * 2 * RING;

export class Net {
  // a new network, its buffer to hand to every node
  static create() {
    const net = new Net(new SharedArrayBuffer(BYTES), 0);
    Atomics.store(net.i, NEXT_PORT, FIRST_PORT);
    return net;
  }

  // the network in `buffer`, as node `node` (1, 2, ...; 0 the coordinator) sees it
  constructor(buffer, node) {
    if (!(node >= 0 && node < NODES)) throw new RangeError(`a node of the network is numbered below ${NODES}`);
    this.buffer = buffer;
    this.node = node;
    this.i = new Int32Array(buffer, 0, DATA / 4);
    this.u = new Uint32Array(buffer, 0, DATA / 4);
    this.b = new Uint8Array(buffer, DATA);
  }

  // The lock over the network. A node's workers end where they are -- one
  // of them may hold it then, and would hold it for ever: a node that is
  // gone (kill) loses it after STEAL_MS, and a worker of such a node that
  // is still running throws rather than take it again.
  lock() {
    let since = 0;
    for (let spins = 1; ; spins++) {
      const held = Atomics.compareExchange(this.i, LOCK, 0, this.node + 1);
      if (held === 0) return;
      if ((spins & 1023) !== 0) continue;
      if (this.node > 0 && Atomics.load(this.i, GONE + this.node)) throw new Error(`node ${this.node} is gone`);
      if (!Atomics.load(this.i, GONE + held - 1)) { since = 0; continue; }
      const now = performance.now();
      if (since === 0) since = now;
      else if (now - since > STEAL_MS) Atomics.compareExchange(this.i, LOCK, held, 0);
    }
  }
  unlock() {
    Atomics.store(this.i, LOCK, 0);
  }
  changed() {
    Atomics.add(this.i, CHANGE, 1);
    Atomics.notify(this.i, CHANGE);
  }

  // Wait under the lock for `ready` (a function answering a result or
  // undefined): at most ms milliseconds (below 0: for ever), then `late`.
  wait(ms, ready, late) {
    const until = ms < 0 ? Infinity : performance.now() + ms;
    for (;;) {
      this.lock();
      const seen = Atomics.load(this.i, CHANGE);
      let r;
      try { r = ready(); } finally { this.unlock(); }
      if (r !== undefined) return r;
      const left = until - performance.now();
      if (left <= 0) return late;
      Atomics.wait(this.i, CHANGE, seen, left === Infinity ? undefined : left);
    }
  }

  // a connection's side from its descriptor: [connection, side] or null
  side(fd) {
    const n = fd - FD;
    if (n < 0 || n >= CONNECTIONS * 2) return null;
    const c = n >> 1;
    return this.i[CB + c * CW + C_STATE] ? [c, n & 1] : null;
  }
  listener(fd) {
    const n = fd - LISTEN_FD;
    return n >= 0 && n < LISTENERS && this.i[LB + n * LW] ? n : null;
  }

  // what a ring holds: written less read (they count on, round 2^32)
  held(c, ring) {
    const w = CB + c * CW + C_RING + ring * 2;
    return (this.u[w + 1] - this.u[w]) >>> 0;
  }

  // --- what a program asks (the import module scaly_net) -------------------

  listen(port) {
    this.lock();
    try {
      if (port === 0) port = Atomics.add(this.i, NEXT_PORT, 1);
      let free = -1;
      for (let l = 0; l < LISTENERS; l++) {
        const p = this.i[LB + l * LW];
        if (p === port) return -1;
        if (p === 0 && free < 0) free = l;
      }
      if (free < 0) return -1;
      const w = LB + free * LW;
      this.i.fill(0, w, w + LW);
      this.i[w] = port;
      this.i[w + 1] = this.node + 1;
      return LISTEN_FD + free;
    } finally { this.unlock(); }
  }

  port(fd) {
    const l = this.listener(fd);
    return l === null ? -1 : this.i[LB + l * LW];
  }

  connect(port) {
    this.lock();
    try {
      let l = 0;
      while (l < LISTENERS && this.i[LB + l * LW] !== port) l++;
      if (l === LISTENERS) return -1;                 // refused: nobody listens
      const w = LB + l * LW;
      if (this.i[w + 2] >= QUEUE) return -1;
      let c = 0;
      while (c < CONNECTIONS && this.i[CB + c * CW + C_STATE]) c++;
      if (c === CONNECTIONS) return -1;
      const cw = CB + c * CW;
      this.i.fill(0, cw, cw + CW);
      this.i[cw + C_STATE] = 1;
      this.i[cw + C_OWNER] = this.node + 1;
      this.i[cw + C_OWNER + 1] = this.i[w + 1];
      this.i[w + 4 + (this.i[w + 3] + this.i[w + 2]) % QUEUE] = c;
      this.i[w + 2]++;
      this.changed();
      return FD + c * 2;
    } finally { this.unlock(); }
  }

  accept(fd, ms) {
    const l = this.listener(fd);
    if (l === null) return -1;
    const w = LB + l * LW;
    return this.wait(ms, () => {
      if (this.i[w] === 0) return -1;
      if (this.i[w + 2] === 0) return undefined;
      const c = this.i[w + 4 + this.i[w + 3]];
      this.i[w + 3] = (this.i[w + 3] + 1) % QUEUE;
      this.i[w + 2]--;
      this.i[CB + c * CW + C_OWNER + 1] = this.node + 1;
      this.changed();
      return FD + c * 2 + 1;
    }, -2);
  }

  // into the program's memory at ptr: what came the other way, at most n bytes
  read(memory, fd, ptr, n, ms) {
    const s = this.side(fd);
    if (!s) return -1;
    const [c, side] = s, ring = 1 - side, cw = CB + c * CW;
    return this.wait(ms, () => {
      if (!this.i[cw + C_STATE] || this.i[cw + C_CLOSED + side]) return -1;
      const held = this.held(c, ring);
      if (held === 0) return this.i[cw + C_CLOSED + ring] ? 0 : undefined;   // the other side closed: the end
      const k = Math.min(n, held);
      const w = cw + C_RING + ring * 2, base = (c * 2 + ring) * RING;
      const mem = new Uint8Array(memory().buffer);
      for (let i = 0, at = this.u[w] % RING; i < k; i++, at = (at + 1) % RING) mem[ptr + i] = this.b[base + at];
      this.u[w] = (this.u[w] + k) >>> 0;
      this.changed();
      return k;
    }, -2);
  }

  write(memory, fd, ptr, n, ms) {
    const s = this.side(fd);
    if (!s) return -1;
    const [c, side] = s, ring = side, cw = CB + c * CW;
    return this.wait(ms, () => {
      if (!this.i[cw + C_STATE] || this.i[cw + C_CLOSED + side]) return -1;
      if (this.i[cw + C_CLOSED + 1 - side]) return -1;                     // the other side is gone
      const room = RING - this.held(c, ring);
      if (room === 0) return undefined;
      const k = Math.min(n, room);
      const w = cw + C_RING + ring * 2, base = (c * 2 + ring) * RING;
      const mem = new Uint8Array(memory().buffer);
      for (let i = 0, at = this.u[w + 1] % RING; i < k; i++, at = (at + 1) % RING) this.b[base + at] = mem[ptr + i];
      this.u[w + 1] = (this.u[w + 1] + k) >>> 0;
      this.changed();
      return k;
    }, -2);
  }

  // whether a call would not wait: 1 or 0
  ready(fd, for_write) {
    this.lock();
    try {
      const l = this.listener(fd);
      if (l !== null) return this.i[LB + l * LW + 2] > 0 ? 1 : 0;
      const s = this.side(fd);
      if (!s) return 1;
      const [c, side] = s, cw = CB + c * CW;
      if (this.i[cw + C_CLOSED + 1 - side]) return 1;
      if (for_write) return this.held(c, side) < RING ? 1 : 0;
      return this.held(c, 1 - side) > 0 ? 1 : 0;
    } finally { this.unlock(); }
  }

  close(fd) {
    this.lock();
    try {
      const l = this.listener(fd);
      if (l !== null) {
        this.drop_listener(l);
        this.changed();
        return 0;
      }
      const s = this.side(fd);
      if (!s) return -1;
      this.close_side(s[0], s[1]);
      this.changed();
      return 0;
    } finally { this.unlock(); }
  }

  // (under the lock) one side of a connection closed; both: the slot is free
  close_side(c, side) {
    const cw = CB + c * CW;
    this.i[cw + C_CLOSED + side] = 1;
    if (this.i[cw + C_CLOSED] && this.i[cw + C_CLOSED + 1]) this.i[cw + C_STATE] = 0;
  }
  // (under the lock) a listener gone, and the connections it had not taken
  drop_listener(l) {
    const w = LB + l * LW;
    for (let k = 0; k < this.i[w + 2]; k++) this.close_side(this.i[w + 4 + (this.i[w + 3] + k) % QUEUE], 1);
    this.i.fill(0, w, w + LW);
  }

  // --- what the coordinator does ------------------------------------------

  // Node `node` is gone: as the kernel of a killed process does, its
  // listeners stop and its ends of every connection close -- its peers read
  // the end of what it sent.
  kill(node) {
    this.gone(node);
    this.lock();
    try {
      for (let l = 0; l < LISTENERS; l++)
        if (this.i[LB + l * LW] && this.i[LB + l * LW + 1] === node + 1) this.drop_listener(l);
      for (let c = 0; c < CONNECTIONS; c++) {
        const cw = CB + c * CW;
        if (!this.i[cw + C_STATE]) continue;
        for (const side of [0, 1])
          if (this.i[cw + C_OWNER + side] === node + 1 && this.state(c) && !this.i[cw + C_CLOSED + side]) this.close_side(c, side);
      }
      this.changed();
    } finally { this.unlock(); }
  }
  state(c) { return this.i[CB + c * CW + C_STATE]; }

  // Node `node`'s workers are ended, its connections left as they are -- a
  // machine that is gone without a word, or a cable pulled: the lock is not
  // its any more.
  gone(node) {
    Atomics.store(this.i, GONE + node, 1);
  }

  // The import module of one instance of a node's program, its memory from
  // `memory` (a function: a thread's instance shares it, and it grows).
  // Addresses come as wasm's i32: read unsigned.
  imports(memory) {
    return {
      listen: port => this.listen(port),
      port: fd => this.port(fd),
      connect: port => this.connect(port),
      accept: (fd, ms) => this.accept(fd, ms),
      read: (fd, ptr, n, ms) => this.read(memory, fd, ptr >>> 0, n >>> 0, ms),
      write: (fd, ptr, n, ms) => this.write(memory, fd, ptr >>> 0, n >>> 0, ms),
      ready: (fd, for_write) => this.ready(fd, for_write),
      close: fd => this.close(fd),
    };
  }
}
