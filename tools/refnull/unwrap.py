#!/usr/bin/env python3
"""Write the unwrap the argument gate asks for.

`option_arg_reaches_plain_ref#` reports every place an Option-typed value is
handed to a non-optional `ref[T]` parameter that flow narrowing could not
prove.  The fix is always the same and the diagnostic names the type:
`x` becomes `x as ref[T]`.

Behaviour-preserving by construction.  The unwrap emits the exit-21 trap, and
that trap fires exactly where the old code took a SIGSEGV at the first member
hop -- so a site that works today keeps working, and one that does not now
says so with the enclosing symbol's name instead of a bare signal.

Driven by the compiler's own reports, never by a pattern: only the planner
knows which argument its narrowing already proved.
"""
import re, subprocess, sys, collections

MSG = re.compile(r'(\S+?):(\d+):(\d+): error: optional reference passed to a '
                 r'non-optional reference parameter: \S+ may be nothing, but '
                 r'(\S+) is a borrowed reference')


def argument_at(src, col):
    """The argument text starting after the delimiter the column lands on."""
    i = col - 1
    while i < len(src) and src[i] in '(, \t':
        i += 1
    depth = 0
    j = i
    while j < len(src):
        c = src[j]
        if c in '([':
            depth += 1
        elif c in ')]':
            if depth == 0:
                break
            depth -= 1
        elif c == ',' and depth == 0:
            break
        j += 1
    return i, j, src[i:j].rstrip()


def main(binary, roots, limit=12):
    total = 0
    for rnd in range(1, limit + 1):
        hits = []
        for root in roots:
            r = subprocess.run(['/bin/sh', '-c',
                                'ulimit -s 65520; exec "$1" -c -o /dev/null "$2"',
                                '_', binary, root], capture_output=True, text=True)
            for line in (r.stderr + r.stdout).split('\n'):
                m = MSG.match(line)
                if m:
                    hits.append((m.group(1), int(m.group(2)), int(m.group(3)), m.group(4)))
        if not hits:
            print(f"round {rnd}: clean after {total} unwraps")
            return 0
        per = collections.defaultdict(list)
        for f, ln, col, ty in hits:
            per[f].append((ln, col, ty))
        n = 0
        for f, sites in per.items():
            lines = open(f, encoding='utf-8').read().split('\n')
            # right to left, so earlier columns keep their offsets
            for ln, col, ty in sorted(sites, key=lambda t: (-t[0], -t[1])):
                src = lines[ln - 1]
                i, j, arg = argument_at(src, col)
                # Skip only an argument ALREADY unwrapped at its top level.
                # A nested cast does not count: `f(g(x as ref[A]))` still owes
                # one for g's result, and the first draft read those eight
                # sites as done.
                if arg and not arg.endswith(ty):
                    pass
                elif not arg or arg.endswith(ty):
                    print(f"  SKIP {f}:{ln}:{col}  {src.strip()[:90]}")
                    continue
                # a bare name needs no parens; anything else does, because
                # `as` binds looser than a prefix `*` and tighter than a
                # parenless call
                body = arg if re.fullmatch(r'[A-Za-z_][A-Za-z0-9_.]*', arg) else f'({arg})'
                lines[ln - 1] = src[:i] + f'{body} as {ty}' + src[i + len(arg):]
                n += 1
            open(f, 'w', encoding='utf-8').write('\n'.join(lines))
        print(f"round {rnd}: {len(hits)} reported, {n} unwrapped")
        total += n
        if n == 0:
            print("  no progress -- stopping")
            return 1
    print("  round limit reached")
    return 1


if __name__ == '__main__':
    sys.exit(main(sys.argv[1], sys.argv[2:]))
