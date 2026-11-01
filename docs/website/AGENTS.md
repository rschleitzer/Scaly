# Scaly for coding agents

This file teaches an AI coding assistant to write correct Scaly. Put it in
your project as `AGENTS.md` or `.github/copilot-instructions.md`, or point
the assistant at https://scaly.io/AGENTS.md. It describes toolchain 0.2.29.

Scaly is a compiled language with LLVM as its backend. Memory is managed in
regions that the compiler infers, so there is no garbage collector and no
manual freeing. The syntax is its own: it looks a little like Swift or Rust,
and that resemblance misleads. **Read "Rules that differ" before writing any
code.** Every program in this file is run by Scaly's test suite and prints
what its `output` block shows.

## Tools

```text
scaly run file.scaly [args]      compile in memory and run
scaly build file.scaly -o prog   build a program (--release optimises, -g debug info)
scaly test file.scaly            run every function named test*: 0 passes
scaly                            the REPL
scalyc                           the compiler alone, with a C compiler's flags
```

The language server `scalyls` gives the editor its errors. The compiler is
strict and its messages usually name the fix: **build or run after every
change and do what the error says.** Exit code 2 is a parse error, 4 a
diagnostic with `file:line:col: error:`.

The standard library is installed as source. For exact signatures, read
`~/.scaly/toolchain/packages/scaly/<version>/scaly/` (on Windows
`%USERPROFILE%\.scaly\toolchain\packages\...`): `containers/String.scaly`,
`containers/Array.scaly`, `os/File.scaly` and so on. The other packages lie
beside it (`http`, `json`, `pg`, ...).

## Rules that differ

1. **`=` compares; `:=` assigns.** `x = 1` as a statement is an error.
   There is no `+=`.
2. **A binding has no `=`:** `let x 5`, `var y 0`, `let z: i32 7`.
   `let` cannot be changed, `var` can.
3. **Negation is `not`, never `!`.** Use `and`/`or` (also `&&`/`||`) and
   `<>` for "not equal".
4. **A `{` never stands on the line of an `if`, `while`, `for`, `choose` or
   `match`.** It begins its own line. A body of one statement needs no
   braces, only indentation.
5. **In a program file, every `define`, `function` and `procedure` comes
   before the first top-level statement.** A definition after a statement
   is the parse error "unexpected trailing input".
6. **`function` is pure; `procedure` may change things.** A procedure marks
   each parameter it writes `mutable` (`this` included). A field that may
   change after construction is declared `var`.
7. **A call without parentheses takes exactly one thing.** `print "hi"` is
   fine. A call, an index or a construction as an argument needs
   parentheses: `print(f(x))`, `print(v[0])`.
8. **An operation reads left to right.** `as T` applies to the whole
   operation before it: `a + b as T` is `(a + b) as T`.
9. **Writing into a container is `put(i, v)`**, never `v[i] := x`. `v[i]`
   reads, and traps (exit 15) outside the bounds.
10. **There is no `null` for ordinary values.** An absent value is an
    `Option`, written `T?`, made with `Some(v)` and `None`.
11. **The last expression of a body is its value.** `return` is only needed
    to leave early.
12. **A character is a one-character string where a `char` is expected**:
    `let c: char "a"` or `"\n" as char`.

## A program

A program is a file of statements. There is no `main` to declare and nothing
to import: the standard library is always visible.

```scaly
function greeting(name: String) returns String
    "Hello, `name`!"

var names Array[String]()
names.add("Ada")
names.add("Grace")
for n in names
    print(greeting(n))
```

```output
Hello, Ada!
Hello, Grace!
```

Backticks inside a string embed a value: a String, char, integer, bool,
floating-point number or byte view. Keep what is embedded simple (names,
fields, calls, arithmetic). It cannot hold a string literal or a lambda;
bind such a value to a name first.

A program may declare `procedure main(argc: int, argv: pointer[pointer[const_char]]) returns int`
for an explicit exit code; `exit(code)` ends a program anywhere.

```scaly
; exits 2 when run without arguments, as here
procedure main(argc: int, argv: pointer[pointer[const_char]]) returns int
{
    let args os.arguments()
    if args.length = 0
    {
        print "usage: hello NAME"
        return 2
    }
    print "hello `args[0]`"
    0
}
```

```output
usage: hello NAME
```

## Values, types and bindings

```scaly
let answer 42            ; int: a 64-bit signed integer, the default
var count 0
count := count + 1
let small: u8 200        ; i8 i16 i32 i64 u8 u16 u32 u64 int size_t
let ratio 7 as f64 / 2.0 ; f32 f64
let ok true              ; bool
let hex 0xFF
let name "Scaly"         ; String
let c: char "A"
let code c as int
print "`answer` `count` `small as int` `ratio` `ok` `hex` `name` `c` `code`"
```

```output
42 1 200 3.5 true 255 Scaly A 65
```

- `;` begins a comment, `;* ... *;` is a block comment.
- Lengths and indexes are `size_t`, which is unsigned. `for i in v.length`
  gives a `size_t` index.
- A `u8` embedded in a string is written as that byte, not as a number:
  write `` `x as int` ``.
- An integer constant takes the type it meets and must fit it:
  `let b: u8 300` is an error. Mixed integer widths widen. A narrowing
  conversion is written `x as T`.
- A colon separates statements on one line: `a := 1: b := 2`.

## Operators

From strongest to weakest: `*` `/` `%`; `+` `-`; `<<` `>>`; `<` `>` `<=`
`>=`; `=` `<>`; `&`; `xor`; `|`; `and`; `or`. Operators of one level combine
from left to right. An operator at the start of an operation is a prefix:
`-x`, `not done`. `and` and `or` evaluate their right side only where the
left does not decide. A null test on the left proves the name for the
right: `p <> null and p.x = 7` reads `p.x` only where `p` is there. Bitwise operators are `&`, `|`, `xor`,
`~`, `<<` and `>>`.

## Control flow

```scaly
let n 7
if n < 0
    print "negative"
else if n = 0
    print "zero"
else
{
    print "positive"
    print "and odd: `n % 2 = 1`"
}

var sum 0
var i 1
while i <= 10
{
    sum := sum + i
    i := i + 1
}

var total 0
for k in 4               ; k runs 0, 1, 2, 3
    total := total + k

let word "b"
match word
    case "a": print "A"
    case "b" case "c": print "B or C"
    else print "other"

var j 0
let found repeat         ; repeat loops until break; break may carry a value
{
    if j * j > 50
        break j
    j := j + 1
}
print "sum `sum` total `total` found `found`"
```

```output
positive
and odd: true
B or C
sum 55 total 6 found 8
```

- An `if` with an `else` is an expression: `let m if a > b: a else b`.
- `break` and `continue` act on the innermost loop. A loop may be labelled
  for nested use: `for i in n: label outer` and then
  `break loop outer` or `continue loop outer`.
- `for x in xs` iterates a `Vector`, `Array`, `List` or `Slice`; `for i in n`
  counts from 0 to n - 1.

## Functions, procedures and operators

```scaly
function add(a: int, b: int) returns int
    a + b

function factorial(n: int) returns int
    if n <= 1: 1 else n * factorial(n - 1)

function largest[T](a: T, b: T) returns T
    if a > b: a else b

procedure double_all(mutable xs: Array[int])
{
    for i in xs.length
        xs.put(i, xs[i] * 2)
}

operator 'plus'(left: int, right: int) returns int
    left + right

var xs Array[int]()
xs.add(1): xs.add(2)
double_all(xs)
print "`add(3, 4)` `factorial(5)` `largest(2.5, 1.5)` `xs[1]` `40 'plus' 2`"
```

```output
7 120 2.5 4 42
```

- A `function` reads its parameters and returns a value. It may not write
  anything, call a procedure that does, or do I/O. `print` is a procedure,
  so a function cannot print.
- A `procedure` that writes a parameter marks it `mutable`, and the compiler
  checks this in both directions.
- A routine that returns nothing omits `returns`.
- A body of one expression needs no braces. A longer body is a block in
  braces, each brace on its own line.

## Records

```scaly
define Point(x: int, y: int)
{
    function length_squared(this) returns int
        x * x + y * y
}

define Counter(var value: int, step: int)
{
    init(step: int)
    {
        this.value := 0
        this.step := step
    }

    procedure bump(mutable this)
        value := value + step
}

let p Point(3, 4)
var c Counter(5)
c.bump()
c.bump()
print "`p.length_squared()` `c.value`"
```

```output
25 10
```

- `define Name(fields)` gives a constructor that takes the fields in order.
  An `init` is only needed for other arguments or defaults.
- Declare a field `var` if it changes after construction. Within the
  routine that builds the record the compiler lets it pass; once the record
  has been handed on, writing a field without `var` is an error.
- Inside methods, fields are reached by name or as `this.x`. A method that
  changes the record is a `procedure` with `mutable this`.
- Pass records by value. The compiler decides where they live. Do not
  reach for `pointer[T]`, which is for low-level code only.

## Unions, Option and choose

```scaly
define Shape union
(
    Circle: f64
    Rect: (w: f64, h: f64)
    Empty
)

function area(s: Shape) returns f64
    choose s
        when r: Circle: 3.14 * r * r
        when b: Rect: b.w * b.h
        else 0.0

function find(xs: Vector[int], wanted: int) returns size_t?
{
    for i in xs.length
    {
        if xs[i] = wanted
            return Some(i)
    }
    None
}

var ys Vector[int](3)
ys.put(0, 5): ys.put(1, 6): ys.put(2, 7)
let r find(ys, 6)
if r.is_some()
    print "found at `r.unwrap()`"
print "missing gives `find(ys, 9).unwrap_or(99)`"
choose find(ys, 7)
    when i: Some
        print "7 at `i`"
    else
        print "no 7"
print "`area(Shape.Circle(1.0))` `area(Shape.Rect((w: 2.0, h: 3.0)))`"
if Shape.Empty is Empty
    print "empty"
```

```output
found at 1
missing gives 99
7 at 2
3.14 6
empty
```

- A variant carries at most one value; use a tuple or a record for more.
- `choose` names a binding, then the variant: `when r: Circle`. An `else`
  covers the remaining variants.
- `T?` is `Option[T]`. A plain value is never wrapped automatically: write
  `Some(v)` and `None`. `is_some()`, `is_none()`, `unwrap()` and
  `unwrap_or(d)` are available.
- `Result[T, E]` (`Ok`, `Error`) works the same way, but errors usually use
  `throws`.

## Errors

```scaly
define ParseError union
(
    Empty
    BadDigit: u8
)

function digit(s: String) returns int throws ParseError
{
    if s.length() = 0
        throw ParseError.Empty
    let c s.get(0)
    if c < 48 or c > 57
        throw ParseError.BadDigit(c)
    (c as int) - 48
}

function twice(s: String) returns int throws ParseError
{
    try let d digit(s)
        else throw                       ; passes the error on
    d * 2
}

procedure show(s: String)
{
    try let d twice(s)
        when e: Empty
        {
            print "empty"
            return
        }
        when b: BadDigit
        {
            print "bad byte `b as int`"
            return
        }
    print "twice is `d`"
}

show("7")
show("")
show("x")
```

```output
twice is 14
empty
bad byte 120
```

- There are no exceptions. `throws E` is a typed error result, and a routine
  throws one type, usually a union.
- `try let name call()` binds on success and runs a `when` arm or the
  `else` on failure. `throw` without a value in an arm or the `else` passes
  the error on. After an arm that does not leave (`return`, `throw`,
  `break`), the binding holds nothing meaningful: let every arm leave when
  the binding is used afterwards.
- Bounds violations, a null unwrap and running out of memory trap with an
  exit code (15, 21, 23).

## Tuples, lambdas and function values

```scaly
define IntFn delegate(int) returns int

function divmod(n: int, d: int) returns (q: int, r: int)
    (q: n / d, r: n % d)

function apply(f: IntFn, x: int) returns int
    f(x)

function make_adder(n: int) returns IntFn
{
    return \x: x + n
}

let (q, r) divmod(17, 5)
let t divmod(9, 4)
let k 10
let scale \x: x * k
let applied apply(\x: x + 1, 41)
let add5 make_adder(5)
let added add5(2)
print "`q` `r` `t.q` `scale(4)` `applied` `added`"
```

```output
3 2 2 40 42 7
```

- A lambda is `\params: body`. It captures by value.
- To pass or store a function, name its type with `delegate`.
- A function returned from a call is bound to a name before it is called.

## Strings and containers

```scaly
let s "Hello, World"
let n s.length()                         ; size_t, in bytes
if s = "Hello, World" and s.starts_with("Hello") and s.contains("World")
    print "`n` `s.substring(7, 5)`"

var sb StringBuilder()                   ; for text built in a loop
for i in 3
{
    sb.append(i)
    sb.append("," as char)
}
let built sb.to_string()

var commas 0
for c in built                           ; a char is a byte: this walks the bytes
{
    if c = ("," as char)
        commas := commas + 1
}

var a Array[int]()                       ; grows
a.add(3): a.add(4)
a.put(0, 30)
var v Vector[int](3)                     ; fixed length
v.put(2, 9)
var l List[String]()                     ; append-only; freeze with Vector(l)
l.add("x")

var mb HashMapBuilder[String, int]()     ; build, then freeze
mb.add("one", 1)
let m HashMap[String, int](mb)
let one m.get("one")                     ; ref[int]?, null when missing
if one <> null and m.contains("one")
    print "one is `*one`"
print "`built` `commas` `a[0]` `a.length` `v[2]`"
```

```output
12 World
one is 1
0,1,2, 3 30 2 9
```

- **String**: `length()`, `=`/`<>`, `starts_with`, `ends_with`, `contains`,
  `index_of`, `substring(start, len)`, `as_slice()` (the bytes as a
  `Slice[u8]`), `get(i)` (a byte). `for c in s` walks the bytes; a `char`
  is one byte, UTF-8 text is not decoded.
  Concatenate with interpolation, ``` "`a``b`" ```, or a `StringBuilder`; there is
  no `+` on strings and no built-in string-to-number parsing.
- **Vector[T]**: fixed length; `Vector[T](n)`, `put`, `[]`, `get(i)` (an
  Option), `.length`.
- **Array[T]**: grows; `add`, `put`, `[]`, `get`, `clear`, `.length`.
- **List[T]**: `add`, then iterate or freeze with `Vector[T](list)`.
- **HashMap[K, V]** and **HashSet[T]**: filled through their builder and
  read with `contains`, `get(key)` or `m[key]` (both answer `ref[V]?`).
- **Slice[T]**: a borrowed view that never owns. `subslice(a, b)`,
  `slice_from(a)`, `[]`, `.length`.
- `for` over a container gives copies of the elements. To change an
  element, read it, change the copy, and `put` it back.

## The operating system

```scaly
let path "scaly-agents-probe.txt"
if File.write_from_string(path, "one\ntwo\n")
{
    let text File.read_to_string(path)
    print "read `text.length()` bytes"
}
let home os.environment_variable("HOME")
if home.length > 0
    print "HOME is set"
for arg in os.arguments()                ; the arguments, the program's own name left out
    print "arg `arg`"
```

```output
read 8 bytes
HOME is set
```

`File` also has `exists`; `Directory` has `list`, `exists`,
`is_directory`, `create` and `remove`; `Path` has `join`,
`get_directory_name` and `get_file_name`. A `Tcp` client and server, and
the `http`, `https`, `json`, `pg`, `redis`, `sqlite`, `tds`, `tls` and
`compress` packages, come with the toolchain.

## Tests

```scaly
function add(a: int, b: int) returns int
    a + b

function test_add() returns int
{
    if add(2, 3) <> 5
        return 1
    0
}

print "the program part"
```

```output
the program part
```

`scaly test file.scaly` runs every function whose name starts with `test`
and that returns `int`; 0 passes. `scaly test file.scaly add` runs those
whose names contain `add`. `scaly new name` creates a program with a test,
and `scaly new name --lib` creates a package.

## Modules and packages

```text
; shapes.scaly, beside the program
define Box(width: int, height: int)

; myprog.scaly
module shapes                     ; loads shapes.scaly
use myprog.shapes.Box             ; a type of another file needs a use (or use myprog.shapes.*)
package json 0.2.0                ; a package of the toolchain
package lib 0.1.3 "github.com/someone/lib"   ; a package from git, fetched by scaly run/build/test
```

A forgotten `use` is an error that names the line to add. The top-level
statements of a loaded module are ignored. A library is a namespace of
definitions: `define mylib { function f() returns int ... }`, called as
`mylib.f()`.

## Memory

Regions are inferred; ordinary code carries no annotations. A value
returned from a function lives in the caller's region, a local one is freed
when its scope ends. The compiler rejects a reference that would outlive its
memory, and its message names the fix. Usually the fix is to return the
value itself instead of a reference to a local. `#` (the caller's region)
and `^name` (the region of `name`) after a type in a construction exist for
the rare case the inference does not cover. `pointer[T]` and `extern` are
for low-level code and calls into C.

## Messages and their fixes

| message | fix |
|---|---|
| `the result of operator '=' is discarded` | assignment is `:=` |
| `'!' is the thrown-lifetime sigil, not negation` | write `not` |
| `cannot assign to x: it is a let binding` | declare it `var` |
| `a function writes nothing - make it a procedure` | `procedure` with `mutable` on what it writes |
| procedure ... writes ..., which its signature does not declare mutable | add `mutable` to that parameter |
| `function ... performs I/O` | make it a `procedure` |
| `no operator += between int and int` | there is no `+=`: `x := x + 1` |
| `a vector literal is not a value` | build a `Vector` with `put` or an `Array` with `add` |
| `a function a call answers is called through a name` | bind the result first, then call it |
| `cannot assign through a subscript` | `v.put(i, x)` |
| `a call without parentheses takes exactly one thing` | add parentheses: `print(f(x))` |
| `expected Then` after an `if` line | move `{` to its own line |
| `unexpected trailing input` | definitions before the first statement, or a syntax error |
| `the returned value does not conform ... Option` | return `Some(v)` or `None` |
| `function not found: ... add: use ...` | add the `use` line it names |
| `unknown identifier: set` | `set x v` is old; write `x := v` |

## Not supported yet

Each of these is a compile error that says so:

- Vector literals as values (`let v [1, 2, 3]`). Build a `Vector` with
  `put` or an `Array` with `add`.
- Calling a call's result directly, `f(1)(2)`. Bind it first:
  `let g f(1)`, then `g(2)`.
- A string literal or a lambda inside a backtick embedding. Bind it first.
