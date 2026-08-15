# scaly-lsp — scalyls as a Claude Code LSP plugin

Serves `.scaly` through Claude Code's `LSP` tool, so an agent asks the planner
what a name MEANS instead of grepping for where a string OCCURS.

## Starting a session

```bash
cd <repo>
claude --plugin-dir .claude/skills/scaly-lsp
```

The flag is **not optional here.** Claude Code normally auto-loads plugins from
`.claude/skills/`, but this org's managed settings carry a
`strictKnownMarketplaces` allowlist without a `{"source":"skills-dir"}` entry,
and that one gate sits in front of the scan for BOTH scopes — user
(`~/.claude/skills/`) and project (`<repo>/.claude/skills/`). There is no
environment variable for `--plugin-dir`; the flag is the only route that does
not need an admin change. Convenience:

```bash
alias claude-scaly='claude --plugin-dir "$HOME/repos/Scaly/.claude/skills/scaly-lsp"'
```

Two alternatives, both needing someone else: the admin adds
`{"source":"skills-dir"}` to `strictKnownMarketplaces` (then this committed
plugin auto-loads, no flag), or the plugin is published into the approved
`in-house-claude-plugins` marketplace.

## Verifying it loaded

Ask the agent to run any LSP operation on a `.scaly` file. Loaded → a real
answer; not loaded → `No LSP server available for file type: .scaly`. That
message is the ONLY signal: a session started without the flag logs nothing
about the plugin at all — no warning, no policy notice. With `--debug`,
`~/.claude/debug/latest` carries the positive side:

```
Loaded inline plugin from path: scaly-lsp
Loaded 1 LSP server(s) from plugin: scaly-lsp
Registered diagnostics handler for plugin:scaly-lsp:scalyls
```

## What the server answers (measured 2026-08-15, this tree)

| Operation | Status |
|---|---|
| goToDefinition | OK — a `.`-qualified member resolves through the RECEIVER's concept |
| findReferences, documentSymbol, workspaceSymbol | OK — matched by name, workspace-wide |
| prepareCallHierarchy, incomingCalls, outgoingCalls | OK — outgoing scopes a member call the same way |
| hover | OK — a declaration answers its signature + doc block, an expression its resolved TYPE, and a genuine miss says `in function X` (where the cursor is, not what it is) |
| goToImplementation | answers "no results" — the server does not implement it |

`goToImplementation` used to HANG the whole session: the server left an
unimplemented request unanswered, the client sends it without checking
capabilities, and the client has no request timeout (measured 94 s, killed by
hand). Fixed 2026-08-15 by answering `result: null` at the tail of
`server.handle_message`; gate `"lsp unhandled request answers null"`.

`goToDefinition` on a METHOD was fixed 2026-08-15 as well, and the defect is the
one worth remembering because it looks like a right answer: the search matched
by NAME alone, and a method name is not unique — this tree declares `append`
twenty-six times, so `line.append(b as char)` on a StringBuilder jumped into
dazzle's `SaveFOTBuilder`. The receiver now picks the file (`this.` → the
enclosing concept, a local → its declared / constructed type, `Type.` / `ns.` →
the token itself; all LEXICAL, no plan). Gate `"lsp definition scoped by the
receiver type"`, six checks, five of which fail on the pre-fix binary — the
sixth is the negative control that a non-member cursor is unchanged. LIMITS:
overloads are not told apart (the concept's first member of that name wins), a
plain FIELD is not found this way, and cross-file the search narrows to the
concept's FILE rather than to the concept. `findReferences` and `rename` are
deliberately still name-wide — a reference search is meant to be broad.

`hover` at a CALL SITE was fixed the same day. It used to answer the enclosing
routine for anything the planner could not resolve, and a file that is a MODULE
of a package could not resolve its SIBLINGS at all — the document was planned
alone. It now re-asks a plan built from the package ROOT (gate `"lsp semantic
hover through the package root"`), which costs that root's plan (0.86 s for
scalyls, ~1.9 s for opensp, 2.45 s for dazzle) on a miss and nothing on a hit.
Two limits worth knowing when an answer looks thin: the root plan is only used
when the buffer matches the file ON DISK (its spans index the saved bytes), and
the planner is demand-driven, so a routine nothing calls is never planned and
every offset in it falls back to `in function X`.

Push diagnostics arrive on didOpen/didChange (0.01 s on a small file).
Latency on the largest source in the tree (`opensp/Parser.scaly`, 881 KB):
documentSymbol 1.3 s, definition 1.0 s, references 0.5 s; a cold
`workspace/symbol` over all 747 files costs 6.5 s and 0.5 s warm. The client
imposes no request timeout, so none of these is at risk of being cut off; the
budgets that DO matter are the worker's own (30 s per document, 60 s
whole-tree, `worker.scaly`) — exceeding one kills the worker and returns an
empty result that reads like "no hits".

## Rebuilding the server

`./build.sh` builds `scalyc/build/scalyls` from the committed seed; the
installed `scalyls` on PATH is a wrapper that sets `SCALY_HOME` and execs it.
After a rebuild, restart the Claude Code session — the server process is
started once per session.
