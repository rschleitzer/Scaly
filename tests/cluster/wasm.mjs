// The cluster suite's runs again, its programs built for
// wasm32-wasip1-threads and their nodes in ONE node process, as a page would
// hold them: each node a worker of its own memory (web/run.mjs) with its
// threads (web/threads.mjs), all of them on one network (web/net.mjs) that
// stands in for TCP. The scenarios are tests/cluster/run.sh's, with the same
// markers; "kill -9" is the end of a node's workers, its sockets closed as
// the kernel closes a killed process's.
//
//   node tests/cluster/wasm.mjs <web-dir> <dir of the built .wasm>
//
// Prints one line per scenario and `cluster (wasm): N PASS, M FAIL`; exits 1
// on a failure.
import { Worker } from 'node:worker_threads';
import { pathToFileURL } from 'node:url';
import { resolve } from 'node:path';

const [webDir, built] = process.argv.slice(2);
const web = pathToFileURL(resolve(webDir) + '/');
const { Threads } = await import(new URL('threads.mjs', web));
const { Net } = await import(new URL('net.mjs', web));
const decoder = new TextDecoder();
const sleep = ms => new Promise(r => setTimeout(r, ms));

// One node: a program on the network as node `id`, with its environment.
// `out` holds what it wrote; `ended` resolves with its exit status.
let node = async function (net, id, wasm, env) {
  const self = { out: '', ended: null, gone: false };
  let finish;
  self.ended = new Promise(r => { finish = r; });
  const take = bytes => { self.out += decoder.decode(bytes); };
  const share = { buffer: net.buffer, node: id };
  const end = (status, silent = false) => {
    if (self.gone) return;
    self.gone = true;
    threads.end();
    program.terminate();
    if (silent) net.gone(id);
    else net.kill(id);             // what it had open closes, as a process's does
    finish(status);
  };
  const threads = await Threads.create({ out: take, err: take, exit: end, env: { ...process.env, SCALY_CPUS: '4', ...env }, net: share });
  const program = new Worker(new URL('run.mjs', web), { workerData: { args: [`${built}/${wasm}`], env, net: share, relay: true } });
  program.on('message', data => {
    if (threads.handle(data)) return;
    if (data && data.type === 'out') take(data.bytes);
    else if (data && data.type === 'exit') end(data.status);
  });
  program.on('error', e => { take(new TextEncoder().encode(`[the program stopped: ${e && e.message || e}]\n`)); end(134); });
  // kill -9: no word from it, its workers ended, its sockets closed by the
  // kernel; silent: they stay open, and nothing comes through them any more
  // (a machine that is gone, or a cable pulled)
  self.kill = () => end(137);
  self.silence = () => end(137, true);
  // wait until its output has a line matching `re` (false after ms)
  self.marker = async (re, ms = 10000) => {
    for (const until = Date.now() + ms; Date.now() < until; await sleep(20))
      if (re.test(self.out) || self.gone && re.test(self.out)) return true;
    return re.test(self.out);
  };
  return self;
};

const last = s => s.trim().split('\n').pop() ?? '';
const lines = (s, re) => s.split('\n').filter(l => re.test(l)).join('\n');
const scenarios = {
  // the in-process round-trip: stdout as the program's "; Expected:" lines
  async roundtrip(net) {
    const n = await node(net, 1, 'roundtrip.wasm', {});
    const rc = await n.ended;
    const want = (await import('node:fs')).readFileSync(new URL('roundtrip.scaly', import.meta.url), 'utf8')
      .split('\n').filter(l => l.startsWith('; Expected: ')).map(l => l.slice(12)).join('\n');
    return rc === 0 && n.out.trim() === want.trim() || `rc=${rc} ${last(n.out)}`;
  },
  // cross-process round-trip: receiver first, the sender retries its connect
  async xproc(net) {
    const r = await node(net, 1, 'xproc_recv.wasm', {});
    const s = await node(net, 2, 'xproc_send.wasm', {});
    const [src, rrc] = await Promise.all([s.ended, r.ended]);
    const ok = src === 0 && rrc === 0 && /^S: PASS$/m.test(s.out) && /^R: verify PASS$/m.test(r.out);
    return ok || `send rc=${src} recv rc=${rrc} ${last(s.out)} / ${last(r.out)}`;
  },
  // 7.3 remote-channel ping-pong: one binary, two roles
  async pingpong(net) {
    const b = await node(net, 1, 'pingpong.wasm', { PINGPONG_ROLE: 'b' });
    await b.marker(/^B: ready$/m);
    const a = await node(net, 2, 'pingpong.wasm', { PINGPONG_ROLE: 'a' });
    const [arc, brc] = await Promise.all([a.ended, b.ended]);
    const ok = arc === 0 && brc === 0 && /^A: PASS$/m.test(a.out) && /^B: PASS$/m.test(b.out);
    return ok || `a rc=${arc} b rc=${brc} ${last(a.out)} / ${last(b.out)}`;
  },
  // a frame that arrives after the local close is dropped, not a trap
  async lateframe(net) {
    const b = await node(net, 1, 'lateframe.wasm', { LATEFRAME_ROLE: 'b' });
    await b.marker(/^B: ready$/m);
    const a = await node(net, 2, 'lateframe.wasm', { LATEFRAME_ROLE: 'a' });
    const arc = await a.ended;
    if (arc !== 0) b.kill();
    const brc = await b.ended;
    const ok = arc === 0 && brc === 0 && /^A: PASS$/m.test(a.out) && /^B: PASS$/m.test(b.out);
    return ok || `a rc=${arc} b rc=${brc} ${last(a.out)} / ${last(b.out)}`;
  },
  // 7.3 kill -9 mid-run: the survivor's receive nulls, its monitor fires
  async kill9(net) {
    const s = await node(net, 1, 'kill9.wasm', { KILL9_ROLE: 's' });
    await s.marker(/^S: ready$/m);
    const v = await node(net, 2, 'kill9.wasm', { KILL9_ROLE: 'v' });
    if (await s.marker(/^S: linked$/m, 20000)) v.kill();
    else { v.kill(); s.kill(); }
    const src = await s.ended;
    const ok = src === 0 && /^S: PASS$/m.test(s.out) && /^S: down suspect$/m.test(s.out);
    return ok || `survivor rc=${src} ${last(s.out)}`;
  },
  // not in run.sh: the victim falls SILENT -- its sockets stay open, nothing
  // more comes through them -- and the survivor's heartbeat clock has to
  // suspect it (rule 1)
  async silence(net) {
    const s = await node(net, 1, 'kill9.wasm', { KILL9_ROLE: 's' });
    await s.marker(/^S: ready$/m);
    const v = await node(net, 2, 'kill9.wasm', { KILL9_ROLE: 'v' });
    if (await s.marker(/^S: linked$/m, 20000)) v.silence();
    else { v.kill(); s.kill(); }
    const t0 = Date.now();
    const src = await s.ended;
    const ok = src === 0 && /^S: PASS$/m.test(s.out) && /^S: down suspect$/m.test(s.out) && Date.now() - t0 >= 2000;
    return ok || `survivor rc=${src} after ${Date.now() - t0} ms ${last(s.out)}`;
  },
  // rule 10: two builds refuse each other, both stamps said
  async stamp(net) {
    const a = await node(net, 1, 'stamp_a.wasm', {});
    await a.marker(/^A: ready$/m);
    const b = await node(net, 2, 'stamp_b.wasm', {});
    const [brc, arc] = await Promise.all([b.ended, a.ended]);
    const said = /build stamp mismatch.*local .* peer /;
    const ok = arc === 0 && brc === 0 && /^A: refused$/m.test(a.out) && /^B: refused$/m.test(b.out) && said.test(a.out) && said.test(b.out);
    return ok || `a rc=${arc} b rc=${brc} ${last(a.out)} / ${last(b.out)}`;
  },
  // 7.4 the distributed trainer: its loss lines are the single node's, bit
  // for bit, on 2 and on 4 ranks
  async train(net) {
    for (const n of [2, 4]) {
      const solo = await node(net, 10 + n, 'train.wasm', { TRAIN_N: String(n) });
      await solo.ended;
      const port = String(47720 + n);
      const r = await node(net, 20 + n * 10, 'train.wasm', { TRAIN_N: String(n), TRAIN_RANK: '0', TRAIN_PORT: port });
      await r.marker(/^R: ready/m);
      const workers = [];
      for (let k = 1; k < n; k++)
        workers.push(await node(net, 20 + n * 10 + k, 'train.wasm', { TRAIN_N: String(n), TRAIN_RANK: String(k), TRAIN_PORT: port }));
      const rrc = await r.ended;
      await Promise.all(workers.map(w => w.ended));
      const want = lines(solo.out, /^L /), got = lines(r.out, /^L /);
      if (rrc !== 0 || want === '' || want !== got) return `parity N=${n}: rrc=${rrc} ${last(r.out)}`;
    }
    return true;
  },
  // 7.4 fault tolerance: rank 2 killed once training is under way; the run
  // completes on the survivors
  async train_fault(net) {
    const env = k => ({ TRAIN_N: '3', TRAIN_STEPS: '1200', TRAIN_RANK: String(k), TRAIN_PORT: '47730' });
    const r = await node(net, 1, 'train.wasm', env(0));
    await r.marker(/^R: ready/m);
    const w1 = await node(net, 2, 'train.wasm', env(1));
    const w2 = await node(net, 3, 'train.wasm', env(2));
    if (await r.marker(/^L /m, 60000)) w2.kill();
    const [rrc, w1rc] = await Promise.all([r.ended, w1.ended]);
    const ok = rrc === 0 && w1rc === 0 && /^R: down 2$/m.test(r.out) && /^R: PASS/m.test(r.out);
    return ok || `rrc=${rrc} w1=${w1rc} ${lines(r.out, /down|PASS/).replace(/\n/g, ' ')}`;
  },
  // the render farm (web/cluster/render.scaly): its picture is the solo
  // program's, bit for bit, rendered by three nodes -- and again where one
  // of them is killed, where one falls silent and where all are killed
  // halfway (the coordinator renders the rest), its job handed on each time
  async render(net) {
    const size = { RENDER_WIDTH: '96', RENDER_HEIGHT: '72', RENDER_TILE: '16', RENDER_PASSES: '5', RENDER_SAMPLES: '2' };
    const image = s => (s.match(/^C: image (\d+)$/m) || [])[1];
    const solo = await node(net, 1, 'render.wasm', { ...size, RENDER_ROLE: 's' });
    await solo.ended;
    const want = image(solo.out);
    if (!want) return `solo: ${last(solo.out)}`;
    const ways = { whole: [], kill: [2], silence: [2], all: [1, 2, 3] };
    let id = 10;
    for (const [way, victims] of Object.entries(ways)) {
      const env = { ...size, RENDER_NODES: '3', RENDER_PORT: String(47750 + id) };
      const c = await node(net, id++, 'render.wasm', { ...env, RENDER_ROLE: 'c' });
      await c.marker(/^C: ready$/m);
      const ws = [];
      for (let k = 1; k <= 3; k++) ws.push(await node(net, id++, 'render.wasm', { ...env, RENDER_ROLE: 'w', RENDER_RANK: String(k) }));
      if (victims.length) {
        // once every victim has rendered a job: one is in its hands now
        for (const v of victims) await c.marker(new RegExp(`^P \\d+ \\d+ ${v} `, 'm'), 30000);
        for (const v of victims) way === 'silence' ? ws[v - 1].silence() : ws[v - 1].kill();
      }
      const crc = await c.ended;
      const got = image(c.out);
      const downs = victims.every(v => new RegExp(`^C: down ${v}$`, 'm').test(c.out)) && /^C: again /m.test(c.out) === victims.length > 0;
      if (crc !== 0 || got !== want || !downs) return `${way}: rc=${crc} image ${got} (solo ${want}) ${lines(c.out, /^C:/).replace(/\n/g, ' ')}`;
      if (way === 'all' && !/^P \d+ \d+ 0 /m.test(c.out)) return 'all: the coordinator rendered nothing itself';
      await Promise.all(ws.filter((w, k) => !victims.includes(k + 1)).map(w => w.ended));
    }
    return true;
  },
};

const only = process.env.CLUSTER_WASM_ONLY ? process.env.CLUSTER_WASM_ONLY.split(',') : null;
let pass = 0, fail = 0;
const failures = [];
for (const [name, run] of Object.entries(scenarios)) {
  if (only && !only.includes(name)) continue;
  const net = Net.create();
  const nodes = [];
  const made = node;
  node = async (...a) => { const n = await made(...a); nodes.push(n); return n; };
  const verdict = await Promise.race([run(net), sleep(90000).then(() => 'no end within 90 s')]);
  node = made;
  // whatever is still running of a scenario that is over (one that failed)
  nodes.forEach(n => n.kill());
  // CLUSTER_WASM_VERBOSE=1: what each node wrote
  if (process.env.CLUSTER_WASM_VERBOSE)
    nodes.forEach((n, i) => console.log(`--- ${name}, node ${i + 1}:\n${n.out.trim()}`));
  if (verdict === true) pass++;
  else { fail++; failures.push(`${name}: ${verdict}`); }
}
console.log(`cluster (wasm): ${pass} PASS, ${fail} FAIL${failures.length ? '\n  ' + failures.join('\n  ') : ''}`);
process.exit(fail ? 1 : 0);
