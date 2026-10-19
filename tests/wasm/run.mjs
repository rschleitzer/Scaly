// Run a WASI module under node: the arguments after the module are its argv,
// the file system is the host's from `/`, and the process ends with the
// module's exit code.
import { readFile } from 'node:fs/promises';
import { WASI } from 'node:wasi';
import { argv, env, exit } from 'node:process';

const wasi = new WASI({ version: 'preview1', args: argv.slice(2), env, preopens: { '/': '/' }, returnOnExit: true });
const module = await WebAssembly.compile(await readFile(argv[2]));
const instance = await WebAssembly.instantiate(module, wasi.getImportObject());
exit(wasi.start(instance));
