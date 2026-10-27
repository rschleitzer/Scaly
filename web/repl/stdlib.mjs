// node web/repl/stdlib.mjs <tree> <packages/scaly/version> -- the standard
// library's files as JSON on standard output, each under the path the page's
// REPL finds it at (SCALY_HOME is /scaly), its bytes in base64. Every file of
// the directory, not only the sources: the compiler keys a package by all of
// them (web/repl/build.sh).
import { readFileSync, readdirSync } from 'node:fs';
import { join } from 'node:path';

const [tree, dir] = process.argv.slice(2);
const files = {};
(function walk(rel) {
  for (const entry of readdirSync(join(tree, rel), { withFileTypes: true }).sort((a, b) => a.name < b.name ? -1 : 1)) {
    const path = join(rel, entry.name);
    if (entry.isDirectory()) walk(path);
    else if (entry.isFile()) files['/scaly/' + path] = readFileSync(join(tree, path)).toString('base64');
  }
})(dir);
process.stdout.write(JSON.stringify(files));
