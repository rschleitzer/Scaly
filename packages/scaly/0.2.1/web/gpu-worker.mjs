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
//   word 1   where it did not run: the bytes of the reason that follow the
//            outputs
//   byte 8   the outputs, one after the other, `count` words each
//
// The first message this worker posts says whether the page has a card:
// { card: true | false }.
import { Card } from './gpu.mjs';

const opened = Card.open().catch(() => null);

self.onmessage = async event => {
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
  const data = 8 + outputs.length * count * 4;
  if (answer.ran) {
    outputs.forEach((o, i) =>
      new Uint32Array(shared, 8 + i * count * 4, count).set(new Uint32Array(o.buffer, o.byteOffset, count)));
  } else {
    const why = new TextEncoder().encode(answer.why).subarray(0, shared.byteLength - data);
    new Uint8Array(shared, data, why.length).set(why);
    state[1] = why.length;
  }
  Atomics.store(state, 0, answer.ran ? 1 : answer.stopped ? 3 : 2);
  Atomics.notify(state, 0);
};

opened.then(card => self.postMessage({ card: card !== null }));
