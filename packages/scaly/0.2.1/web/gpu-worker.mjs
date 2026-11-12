// The worker that owns a page's graphics card for a Scaly program.
//
// A program calls its host and waits for the answer; WebGPU answers through
// promises, which resolve only on a thread that goes back to its event loop
// -- the program's does not, it is in the middle of a loop. So the card has
// a thread of its own: this one. The program's side (gpu-host.mjs) posts a
// run here and sleeps on a shared word; this worker runs the kernel
// (gpu.mjs), writes the outputs behind that word and wakes it.
//
// A run, as posted: { wgsl, count, words, inputs, outputs, shared } -- the
// typed arrays are copies handed over for good, `shared` is a
// SharedArrayBuffer:
//
//   word 0   0 while the run is under way; then 1 the card ran every round,
//            2 it did not, 3 a round stopped
//   word 1   where it ran: how long compiling its kernel took, in
//            microseconds (0: compiled before); where it did not: the bytes
//            of the reason that follow the outputs
//   byte 8   four doubles, the times of the run: when it arrived here and
//            when it was through (milliseconds since 1970, a clock both
//            workers share), how long the browser took to say the card met
//            no error, and to hand back the results
//   byte 40  the outputs, one after the other, `count` words each
//
// The first message this worker posts says whether the page has a card:
// { card: true | false, why } -- `why`, where it has none, the reason
// (Card.why).
//
// A page that keeps its card while a program's worker is ended and made
// again -- a run broken off -- makes this worker itself and hands each
// program's worker one end of a MessageChannel, the other end here:
// { port }. The port then carries the runs and the first answer, as the
// worker's own messages do; the kernels compiled so far stay.
import { Card } from './gpu.mjs';

// where the outputs begin in `shared` (gpu-host.mjs says the same)
const HEAD = 40;
const now = () => performance.timeOrigin + performance.now();

const opened = Card.open().catch(e => { Card.why = String(e && e.message || e); return null; });
const hello = card => ({ card: card !== null, why: card ? '' : Card.why });

async function serve(event) {
  const arrived = now();
  const { wgsl, count, words, inputs, outputs, shared } = event.data;
  const state = new Int32Array(shared, 0, 2);
  let answer;
  try {
    const card = await opened;
    answer = card ? await card.run({ wgsl, count, words, inputs, outputs })
                  : { ran: false, stopped: false, why: 'no card' };
    if (card && !answer.ran && !answer.stopped && card.said.length)
      answer.why += ': ' + card.said[card.said.length - 1];
  } catch (e) {
    answer = { ran: false, stopped: false, why: String(e && e.message || e) };
  }
  const data = HEAD + outputs.length * count * 4;
  if (answer.ran) {
    state[1] = Math.round((answer.compile_ms ?? 0) * 1000);
    outputs.forEach((o, i) =>
      new Uint32Array(shared, HEAD + i * count * 4, count).set(new Uint32Array(o.buffer, o.byteOffset, count)));
    new Float64Array(shared, 8, 4).set([arrived, now(), answer.errors_ms ?? 0, answer.read_ms ?? 0]);
  } else {
    const why = new TextEncoder().encode(answer.why).subarray(0, shared.byteLength - data);
    new Uint8Array(shared, data, why.length).set(why);
    state[1] = why.length;
  }
  Atomics.store(state, 0, answer.ran ? 1 : answer.stopped ? 3 : 2);
  Atomics.notify(state, 0);
}

self.onmessage = event => {
  const port = event.data && event.data.port;
  if (!port) return serve(event);
  port.onmessage = serve;
  opened.then(card => port.postMessage(hello(card)));
};

opened.then(card => self.postMessage(hello(card)));
