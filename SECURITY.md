Security
========

Reporting
---------

Please report a security problem privately, not as a public issue: on
GitHub, under this repository's **Security** tab, **Report a
vulnerability**. Say what you found, what it takes to bring it about, and
which version or commit you saw it in; a request, a program or a test that
shows it helps most.

Scaly is developed by one person. A report is read and answered as soon as
there is time, and a fix goes into the next release; there is no promised
timeline and no bounty.

What is in scope
----------------

- the compiler, the tool and the language server (`packages/scalyc/`,
  `packages/scalyls/`);
- the runtime and the standard library (`packages/scaly/`);
- the standard packages beside them -- `http`, `https`, `tls`, `h3`, `json`,
  `pg`, `redis` and the rest under `packages/`: input from the network that
  ends a server, hangs it, makes it hold memory without bound, or reads or
  writes memory it should not;
- the installers and the archives they fetch (`docs/website/install.sh`,
  `install.ps1`, `scaly.io/downloads/`).

A problem in OpenSSL, ngtcp2, libdeflate or LLVM belongs to that project;
if Scaly uses one of them in a way that makes it worse, that is in scope.

Versions
--------

Fixes go into the newest release of the toolchain and of the packages
(`scaly.io/downloads/latest`); older versions are not patched.
