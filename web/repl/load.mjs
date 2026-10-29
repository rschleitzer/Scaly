// The compiler for a page of this directory (the REPL, index.html, and the
// tour, tour.html) or of the reference (/lang/, its Run buttons): the
// standard library's files (stdlib.json, paths under /scaly) and the compiled
// module of scaly.wasm, both as web/repl/build.sh wrote them. `status(text)`
// hears how far the loading is. A page hands both to a worker (worker.mjs);
// the module is compiled once and every worker instantiates it. `base` is
// where these files are, seen from the page; a page whose programs read no
// input passes `isolated: false` -- only the standard input needs the
// SharedArrayBuffer that cross-origin isolation allows.
export async function loadCompiler(status, { base = '', isolated = true } = {}) {
  if (isolated && !crossOriginIsolated)
    throw new Error('this page must be served cross-origin isolated (COOP and COEP headers) -- web/repl/serve.mjs does');
  const listing = await (await fetch(base + 'stdlib.json')).json();
  const files = {};
  for (const [path, b64] of Object.entries(listing))
    files[path] = Uint8Array.from(atob(b64), c => c.charCodeAt(0));
  const wasm = await fetchWithProgress(base + 'scaly.wasm', 'the compiler', status);
  status('Compiling the compiler…');
  const module = await WebAssembly.compile(wasm);
  return { module, files };
}

async function fetchWithProgress(url, what, status) {
  const response = await fetch(url);
  if (!response.ok) throw new Error(`${url}: ${response.status}`);
  const total = Number(response.headers.get('content-length')) || 0;
  // sent compressed, the length is the compressed one: no total then
  const encoded = response.headers.get('content-encoding');
  const reader = response.body.getReader();
  const parts = [];
  let got = 0;
  const mb = n => (n / 1048576).toFixed(0);
  for (;;) {
    const { done, value } = await reader.read();
    if (done) break;
    parts.push(value);
    got += value.length;
    status(total && !encoded ? `Loading ${what}… ${mb(got)} of ${mb(total)} MB` : `Loading ${what}… ${mb(got)} MB`);
  }
  const all = new Uint8Array(got);
  let p = 0;
  for (const part of parts) { all.set(part, p); p += part.length; }
  return all;
}
