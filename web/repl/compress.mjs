// node web/repl/compress.mjs <file>... -- each file beside itself as
// <file>.br (brotli, quality 11) and <file>.gz (gzip -9), which serve.mjs
// sends where the browser accepts them: the compiler's module goes over the
// wire at about a fifth of its size (web/repl/build.sh).
import { readFileSync, writeFileSync } from 'node:fs';
import { brotliCompressSync, constants, gzipSync } from 'node:zlib';

for (const file of process.argv.slice(2)) {
  const bytes = readFileSync(file);
  writeFileSync(file + '.br', brotliCompressSync(bytes, {
    params: { [constants.BROTLI_PARAM_QUALITY]: 11, [constants.BROTLI_PARAM_SIZE_HINT]: bytes.length },
  }));
  writeFileSync(file + '.gz', gzipSync(bytes, { level: 9 }));
}
