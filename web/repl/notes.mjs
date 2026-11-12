// Results beside the code: the playground's notes (web/repl/playground.html).
//
// Before a program runs, instrument() writes a call after each binding and
// each assignment of the PROGRAM -- its top-level statements and the loops
// and blocks among them, never the inside of a definition (a `function` does
// no output) --
//
//   let answer 42
//   playground_note(1, "answer", answer)
//
// and appends the procedures it calls, one per type that has a text: they
// print a line the page takes out of the output (Notes.take) and shows
// beside the line it names. A value of another type has no overload: the
// compiler refuses that call, and the page drops exactly the notes on the
// lines it names (`without`) and compiles again. A body of one statement
// without braces (`for i in 4` / `    total := total + i`) gets them, so that
// the note runs with it.
//
// Lines move: `lines[k]` is the original line of instrumented line k + 1;
// relines() puts the original numbers into the compiler's messages.

const MARK = '⁣';             // INVISIBLE SEPARATOR: what starts a note's line
const LIMIT = 4000;                // notes a run prints at most

const NAME = '[A-Za-z][A-Za-z0-9_]*';
const BINDING = new RegExp(`^(\\s*)(?:let|var)\\s+(${NAME})(?=[\\s:])`);
const ASSIGNMENT = new RegExp(`^(\\s*)(${NAME}(?:\\.${NAME})*)\\s*:=`);
const DEFINITION = /^(function|procedure|operator|define|init|extern|package|module|use)\b/;
const HEADER = /^(while|for|relaxed|if|else|repeat|when|case|choose|match|try|label)\b/;

const indentOf = line => line.length - line.trimStart().length;
const blank = line => line.trim() === '' || line.trim().startsWith(';');

const HELPERS = `; --- the playground's notes: each value beside its line (not part of the program)
mutable playground_notes: int 0
procedure playground_note(line: int, name: String, value: String) mutable playground_notes
{
    if playground_notes < ${LIMIT}
        print "${MARK}\`line\`${MARK}\`name\`${MARK}\`value\`"
    playground_notes := playground_notes + 1
}
procedure playground_note(line: int, name: String, value: int) mutable playground_notes
    playground_note(line, name, "\`value\`")
procedure playground_note(line: int, name: String, value: size_t) mutable playground_notes
    playground_note(line, name, "\`value\`")
procedure playground_note(line: int, name: String, value: f64) mutable playground_notes
    playground_note(line, name, "\`value\`")
procedure playground_note(line: int, name: String, value: f32) mutable playground_notes
    playground_note(line, name, "\`value as f64\`")
procedure playground_note(line: int, name: String, value: bool) mutable playground_notes
    playground_note(line, name, "\`value\`")
procedure playground_note(line: int, name: String, value: char) mutable playground_notes
    playground_note(line, name, "\`value\`")

`;

// The program with its notes; `without` holds original line numbers whose
// notes are left out.
export function instrument(source, without = new Set()) {
  const src = source.replace(/\r\n?/g, '\n').split('\n');
  const out = [], lines = [];
  const put = (text, from) => { out.push(text); lines.push(from); };
  // which lines are inside a definition: from a definition's head at the
  // left margin to the next line there that is not a brace
  const inDef = new Array(src.length).fill(false);
  let def = false;
  for (let i = 0; i < src.length; i++) {
    const l = src[i];
    if (!blank(l) && indentOf(l) === 0) {
      const t = l.trim();
      if (DEFINITION.test(t)) def = true;
      else if (t !== '{' && t !== '}') def = false;
    }
    inDef[i] = def;
  }
  // where each statement ends: the lines after it that stand further in,
  // and a block that opens on its own line at its own indentation
  const endOf = i => {
    const ind = indentOf(src[i]);
    let end = i;
    for (let j = i + 1; j < src.length; j++) {
      if (blank(src[j])) continue;
      const jind = indentOf(src[j]);
      if (jind > ind) { end = j; continue; }
      // an `else` at its own indentation goes on with the statement
      if (jind === ind && /^else\b/.test(src[j].trim())) { end = j; continue; }
      if (jind === ind && src[j].trim() === '{') {
        let depth = 0;
        for (; j < src.length; j++) {
          const t = src[j].trim();
          if (t === '{') depth++;
          else if (t === '}' && --depth === 0) break;
        }
        end = Math.min(j, src.length - 1);
        continue;
      }
      break;
    }
    return end;
  };
  // the line a statement stands under, where it is not at the margin
  const parentOf = i => {
    const ind = indentOf(src[i]);
    for (let j = i - 1; j >= 0; j--)
      if (!blank(src[j]) && indentOf(src[j]) < ind) return j;
    return -1;
  };
  const notes = new Map();          // line index -> { name, end, wrap }
  for (let i = 0; i < src.length; i++) {
    if (inDef[i] || blank(src[i]) || without.has(i + 1)) continue;
    const m = BINDING.exec(src[i]) || ASSIGNMENT.exec(src[i]);
    if (!m) continue;
    const rest = src[i].slice(m[0].length);
    if (/^\s*\\/.test(rest) || /^\s*:\s*\\/.test(rest)) continue;   // a lambda: no value to show
    const ind = indentOf(src[i]);
    let wrap = -1;
    if (ind > 0) {
      const p = parentOf(i);
      if (p < 0) continue;
      const pt = src[p].trim();
      if (pt !== '{') {
        if (!HEADER.test(pt)) continue;                              // a continuation, not a statement
        wrap = indentOf(src[p]);
      }
    }
    notes.set(i, { name: m[2], end: endOf(i), wrap, ind });
  }
  // write it out
  const after = new Map();          // line index -> the lines to put after it
  const before = new Map();
  for (const [i, n] of notes) {
    const note = `${' '.repeat(n.ind)}playground_note(${i + 1}, "${n.name}", ${n.name})`;
    const close = [];
    if (n.wrap >= 0) {
      before.set(i, `${' '.repeat(n.wrap)}{`);
      close.push(`${' '.repeat(n.wrap)}}`);
    }
    after.set(n.end, [...(after.get(n.end) ?? []), note, ...close]);
  }
  if (notes.size === 0) return { text: source, lines: src.map((_, i) => i + 1), count: 0 };
  // the helpers stand before the program's first statement: what a program
  // declares comes before what it does
  let first = src.findIndex((l, i) => !inDef[i] && !blank(l) && indentOf(l) === 0 && !['{', '}'].includes(l.trim()));
  if (first < 0) first = src.length;
  for (let i = 0; i < src.length; i++) {
    if (i === first) for (const l of HELPERS.split('\n')) put(l, 0);
    if (before.has(i)) put(before.get(i), i + 1);
    put(src[i], i + 1);
    for (const extra of after.get(i) ?? []) put(extra, i + 1);
  }
  return { text: out.join('\n'), lines, count: notes.size };
}

// The compiler's messages with the program's own line numbers.
export function relines(text, lines) {
  return text.replace(/main\.scaly:(\d+):(\d+)/g, (all, l, c) => {
    const k = Number(l) - 1;
    return k >= 0 && k < lines.length && lines[k] > 0 ? `main.scaly:${lines[k]}:${c}` : all;
  });
}

// the original lines whose notes a compiler's message names: a call of
// playground_note that has no overload for the value
export function failedNotes(text, instrumented, lines) {
  const src = instrumented.split('\n');
  const failed = new Set();
  for (const m of text.matchAll(/main\.scaly:(\d+):\d+/g)) {
    const k = Number(m[1]) - 1;
    if (k >= 0 && k < src.length && src[k].trimStart().startsWith('playground_note(') && lines[k] > 0) failed.add(lines[k]);
  }
  return failed;
}

// The notes of one run, out of what the program writes: take() hands back
// the text with the notes taken out; `byLine` holds, per original line, the
// name and the values in order.
export class Notes {
  constructor() { this.byLine = new Map(); this.rest = ''; this.count = 0; }
  take(text, final = false) {
    const all = this.rest + text;
    const cut = final ? all.length : all.lastIndexOf('\n') + 1;
    this.rest = all.slice(cut);
    let kept = '';
    for (const line of all.slice(0, cut).split(/(?<=\n)/)) {
      if (line.startsWith(MARK)) {
        const [, l, name, ...value] = line.replace(/\n$/, '').split(MARK);
        const at = Number(l);
        if (!this.byLine.has(at)) this.byLine.set(at, { name, values: [] });
        const entry = this.byLine.get(at);
        if (entry.values.length < 200) entry.values.push(value.join(MARK));
        else entry.more = (entry.more ?? 0) + 1;
        this.count++;
      } else kept += line;
    }
    return kept;
  }
}
