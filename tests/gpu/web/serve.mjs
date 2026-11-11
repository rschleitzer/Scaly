// serve.mjs <dir> <port> <result-file> [seconds] -- serve a directory to ONE page run: static files,
// and POST /result, whose body is written to <result-file>; then the server ends.
import http from 'node:http';
import fs from 'node:fs';
import path from 'node:path';
const [dir, port, resultFile, seconds] = process.argv.slice(2);
const types = { '.html': 'text/html', '.mjs': 'text/javascript', '.js': 'text/javascript', '.json': 'application/json', '.wgsl': 'text/plain', '.wasm': 'application/wasm' };
const server = http.createServer((req, res) => {
  if (req.method === 'POST' && req.url === '/result') {
    let body = '';
    req.on('data', c => body += c);
    req.on('end', () => { fs.writeFileSync(resultFile, body); res.end('ok'); server.close(); setTimeout(() => process.exit(0), 50); });
    return;
  }
  const file = path.join(dir, decodeURIComponent(req.url.split('?')[0]));
  if (!file.startsWith(path.resolve(dir)) || !fs.existsSync(file) || fs.statSync(file).isDirectory()) { res.statusCode = 404; res.end('no'); return; }
  res.setHeader('Content-Type', types[path.extname(file)] || 'application/octet-stream');
  res.setHeader('Cross-Origin-Opener-Policy', 'same-origin');
  res.setHeader('Cross-Origin-Embedder-Policy', 'require-corp');
  res.end(fs.readFileSync(file));
});
server.listen(Number(port), '127.0.0.1');
setTimeout(() => { fs.writeFileSync(resultFile, 'TIMEOUT'); process.exit(3); }, Number(seconds || 30) * 1000);
