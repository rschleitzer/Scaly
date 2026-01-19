# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

> **⚠️ COMMIT MESSAGES: One line, 50 chars max, no fluff. See [Commit Policy](#commit-policy).**

## Overview

Scaly is envisioned to be a self-scaling programming language. The goal is a self-hosted compiler that manages memory through regions automatically (Region-Based Memory Management / RBMM).

The current approach builds the compiler in **idiomatic C++** with **LLVM code generation** from the start. This replaces the previous TypeScript bootstrapper approach, which was abandoned because it could never achieve the core goals of LLVM backend and RBMM.

## Development Strategy

### Overall Approach

1. **Build the compiler in idiomatic C++** - write clean, modern C++ code (not generated or transpiled)
2. **Target LLVM from the start** - no intermediate targets; go directly to LLVM IR
3. **JIT testing from the start** - use LLVM's JIT capabilities to dynamically test XML test cases
4. **No early bootstrapping pressure** - focus on correctness and completeness before self-hosting

### Key Differences from Previous C++ Attempts

- **Idiomatic C++**: Write natural C++ code, not code designed to look like Scaly or be easily transpiled
- **LLVM-first**: Previous attempts used C++ as an intermediate target; now we generate LLVM IR directly
- **JIT testing**: Test cases defined in XML are compiled and executed via JIT during development
- **Long-term bootstrapping**: Self-hosting is a goal but not a near-term constraint on implementation choices

### Self-Hosting Roadmap

The C++ compiler pipeline is **feature-complete** (Lexer → Parser → Modeler → Planner → Emitter → LLVM). Path to self-hosting:

| Phase | Goal | Status |
|-------|------|--------|
| 1 | Validate AOT | **Done** - 26+ AOT tests in `tests/aot/`, linking works |
| 2 | Complete stdlib | In progress - memory, containers, io, llvm modules exist |
| 3 | Port to Scaly | Next - LLVM-C bindings in `packages/scaly/0.1.0/scaly/llvm.scaly` |
| 4 | Bootstrap | Future - C++ compiles Scaly compiler → stage1 compiles itself |

**Current focus:** Complete stdlib modules (containers, io), then port Lexer.cpp to Scaly using the LLVM-C bindings.

### LLVM Coding Guidelines

Follow the [LLVM Programmer's Manual](https://llvm.org/docs/ProgrammersManual.html) conventions. These idioms align well with Scaly's design and will ease eventual self-hosting:

| LLVM C++ Idiom | Scaly Equivalent | Rationale |
|----------------|------------------|-----------|
| `Expected<T>` | `Result[T, Error]` | Explicit error handling, no exceptions |
| `StringRef` | Borrowed `String` | Non-owning references, no hidden copies |
| `SmallVector<T, N>` | `Vector[T]` | Stack-first allocation, region-friendly |
| `isa<>/cast<>/dyn_cast<>` | `choose`/`when` | Pattern matching on type hierarchies |
| No exceptions | Explicit returns | No hidden control flow |
| Value semantics | Immutable params | Functions receive immutable inputs |

**Key practices:**
- Use LLVM's ADT library: `SmallVector`, `StringRef`, `ArrayRef`, `SmallPtrSet`
- Return `Expected<T>` for operations that can fail (parsing, file I/O)
- Pass `StringRef` and `ArrayRef` by value (they're lightweight views)
- Use `isa<>`/`dyn_cast<>` for AST node dispatch instead of `dynamic_cast`
- Avoid `std::string` where `StringRef` suffices
- Avoid exceptions entirely - use explicit error propagation

**Why this matters for bootstrapping:**
The C++ code will have a similar "shape" to eventual Scaly code. When we port the compiler to Scaly, `Expected<T>` becomes `Result[T, Error]` with pattern matching, `StringRef` becomes a borrowed reference, and `dyn_cast<>` dispatch becomes `choose`/`when` expressions. No fundamental restructuring required.

### Build System and C++ Conventions

**Platform Support:**
- Linux: Required (must work)
- macOS: Supported (development platform)
- Windows: Not supported during C++ bootstrapping phase

**Compiler and Standard:**
- Use `clang++` (stay in LLVM ecosystem)
- C++17 standard (`-std=c++17`)

**Build System: CMake (minimal)**
- Single `CMakeLists.txt` - keep it simple
- Use `find_package(LLVM)` for LLVM integration
- Export compile commands for IDE support

```cmake
cmake_minimum_required(VERSION 3.16)
project(scalyc)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_COMPILER clang++)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(LLVM REQUIRED CONFIG)
include_directories(${LLVM_INCLUDE_DIRS})
link_directories(${LLVM_LIBRARY_DIRS})

add_executable(scalyc
  main.cpp
  Lexer.cpp
  Parser.cpp
)

target_link_libraries(scalyc LLVM)
```

**File Conventions:**
- `.h` for headers, `.cpp` for implementation
- `#pragma once` for include guards (simpler, universally supported)
- One logical unit per file (class or related functions)
- Namespace: `scaly::`

**Directory Structure:**
```
scalyc/
├── CMakeLists.txt
├── main.cpp
├── Lexer.h
├── Lexer.cpp
├── Parser.h
├── Parser.cpp
├── AST.h
├── AST.cpp
└── ...
```

**Build Workflow:**
```bash
cd scalyc
mkdir -p build && cd build
cmake ..
make
```

### Unified Language Specification

The language grammar is defined in SGML and processed with DSSSL to generate parser code:

```
scaly.sgm  →  ./mkp (openjade + DSSSL)  →  scalyc/Parser.cpp
                                            →  scalyc/Syntax.h
```

**Source files:**
- **scaly.sgm**: Grammar specification (syntax rules, keywords, punctuation)
- **grammar.dtd**: DTD governing the grammar structure
- **codegen/scaly.dsl**: DSSSL stylesheet that generates parser code

### Literate Testing and Documentation

Tests are written as **literate documentation** - prose explanations with embedded executable tests. The same source generates both test code and user documentation:

```
tests/*.sgm  →  codegen  →  scalyc/Tests.cpp (executable tests)
                         →  docs/*.xml (DocBook documentation)
```

**Test file structure** (defined in `test.dtd`):
```sgml
<suite id="expressions-tests" title="Expression Tests">

<category id="literals" title="Literals">
    <prose>Expressions are the core computational units of Scaly.

    <tests id="boolean-literals" title="Boolean Literals">
        <prose>The boolean literals are true and false.
        <test id="bool-true"><input>true<expect>true
        <test id="bool-false"><input>false<expect>false

    <tests id="integer-literals" title="Integer Literals">
        <prose>Integer literals are sequences of decimal digits.
        <test id="int-positive"><input>42<expect>42
        <test id="int-negative"><input>-7<expect>-7
</category>
</suite>
```

**Key elements:**
- `<suite>`: Top-level test collection
- `<category>`: Major section (becomes documentation chapter)
- `<tests>`: Group of related tests with shared prose
- `<prose>`: Documentation text explaining the feature
- `<test>`: Individual test with `<input>` and `<expect>`

**Current test files:**
- `tests/expressions.sgm` - Literals, operations, bindings
- `tests/definitions.sgm` - Structures, unions, namespaces
- `tests/choose.sgm` - Pattern matching
- `tests/controlflow.sgm` - Control flow (if, while, for, return)

**Benefits of literate testing:**
- Documentation IS the specification - no drift between docs and behavior
- Tests serve as executable examples in documentation
- Prose explains the "why", tests prove the "what"
- Adding a test automatically updates documentation

**Codegen script:**
```bash
./mkp   # Regenerates Parser.cpp, Syntax.h, Tests.cpp, docs/*.xml
```

This runs `openjade -G -t sgml -d codegen/scaly.dsl scaly.sgm`

### AOT (Ahead-of-Time) Testing

In addition to JIT-based literate tests, the compiler has **AOT tests** - standalone Scaly programs that are compiled to object code and executed:

```
tests/aot/
├── arithmetic.scaly      # Basic arithmetic operations
├── binding.scaly         # Variable bindings (let, var)
├── boolean.scaly         # Boolean operations
├── function_call.scaly   # Function calls and returns
├── generic_box.scaly     # Generic type instantiation
├── hello.scaly           # Hello world (printf)
├── pointer_ops.scaly     # Pointer arithmetic and dereferencing
├── string_literal.scaly  # String handling
└── ... (26+ test files)
```

**Running AOT tests:**
```bash
# Compile and run a single AOT test
./scalyc/build/scalyc -o tests/aot/build/hello tests/aot/hello.scaly
./tests/aot/build/hello

# The build directory contains compiled binaries
ls tests/aot/build/
```

**Why both JIT and AOT tests:**
- **JIT tests** (SGML-based): Fast iteration, immediate feedback, good for language semantics
- **AOT tests**: Validate full compilation pipeline, linking, runtime behavior, RBMM

**Test categories in AOT:**
- Basic: `arithmetic`, `boolean`, `binding`, `string_literal`
- Functions: `function_call`, `function_pointer`, `generic_box`, `generic_function`
- Memory: `pointer_ops`, `struct_ops`, `array_ops`
- Control flow: `if_else`, `while_loop`, `for_loop`
- Advanced: `method_call`, `operator_overload`, `choose_when`

### Memory Model: Function vs Procedure

Scaly distinguishes between **functions** and **procedures** with distinct semantic guarantees:

**Functions** (pure, mathematical):
- Inputs are immutable (borrowed references)
- Output only via return value
- No side effects
- Safe for automatic parallelization

**Procedures** (imperative):
- Can mutate input parameters
- Can have side effects
- Used for builder patterns and I/O

```scaly
; Pure function - inputs immutable, result returned
function formatName(first: String, last: String) returns String
{
    StringBuilder.build(b =>
    {
        b.append(first)
        b.append(" ")
        b.append(last)
    })
}

; Procedure - can mutate the builder
procedure appendItems(builder: StringBuilder, items: List[String])
{
    for item in items
        builder.append(item)
}
```

**Operators** are always pure (like functions):
- No side effects, always return a result

**Benefits of this distinction:**
- Explicit at call site - you know if something can mutate
- Compiler enforced - functions can't mutate, procedures can
- Parallelism-friendly - functions and operators are automatically safe to parallelize
- Builders fit naturally - procedures work with mutable state

### Memory Allocation and Lifetimes

Scaly uses Region-Based Memory Management (RBMM) with explicit lifetime annotations on allocations. The rule is simple: **lifetime suffix = heap allocation, no suffix = stack allocation**. The lifetime suffix comes **before** the constructor parameters.

| Syntax | Meaning | Allocation |
|--------|---------|------------|
| `Foo(args)` | No lifetime | Stack (by value) |
| `Foo$(args)` | Local lifetime | Heap, on local page |
| `Foo#(args)` | Caller lifetime | Heap, on caller's page (rp) |
| `Foo^name(args)` | Reference lifetime | Heap, on named page |

**Examples:**

```scaly
; Stack allocation (by value, small/short-lived):
function get_iterator(this: List[T]) returns ListIterator[T]
{
    ListIterator[T](head)    ; No suffix = stack, returned by value
}

; Local page allocation (automatically cleaned up at block exit):
function process()
{
    let builder StringBuilder$()   ; $ = allocate on local page
    builder.append("hello")
}   ; Compiler cleans up local page here

; Caller's page allocation (returned to caller):
function create_node(data: T) returns pointer[Node[T]]
{
    Node[T]#(data, null)     ; # = allocate on caller's rp
}

; Explicit page allocation (for container internals):
procedure add(this: List[T], element: T)
{
    let lp get_page()                    ; Get page where 'this' lives
    let new_node Node[T]^lp(element, head)  ; ^lp = allocate on lp
    set head: new_node
}
```

**Important rules:**

1. **`$` on return type is forbidden** - Use no lifetime for by-value return, or `#` for caller's page:
   ```scaly
   function foo() returns int$ { ... }   ; ERROR: $ not allowed on return type
   function foo() returns int { ... }    ; OK: by-value return
   function foo() returns pointer[int]   ; OK: return pointer allocated with #
   {
       42#
   }
   ```

2. **`^page_ptr` requires pointer[Page] type** - The variable must be a page pointer:
   ```scaly
   let c Car$()           ; c is pointer[Car], not pointer[Page]
   let p Person^c()       ; ERROR: c is not pointer[Page]

   let page Page.allocate_page()   ; page is pointer[Page]
   let p Person^page()             ; OK: page is pointer[Page]
   ```

3. **Empty constructor parentheses are optional** - `Person^c` is equivalent to `Person^c()`.

4. **No explicit Region or `new` keyword needed** - The compiler automatically:
   - Allocates local pages when `$` allocations exist in a block
   - Cleans up local pages at block exit
   - Passes `rp` parameter for `#` return types

5. **`init#` for types needing page parameter** - Some types are value types (stack-allocatable) but need a page for internal allocations:
   ```scaly
   ; Definition: init# takes an implicit page parameter as first argument
   define String(data: pointer[char], length: size_t)
   {
       init#(page, text: pointer[const_char])
       {
           let len strlen(text)
           set data: page.allocate(len + 1, 1) as pointer[char]
           memcpy(data, text, len)
           set length: len
       }
   }

   ; Call sites: lifetime modifier determines which page to pass
   var s1 String#("hello")        ; Pass caller's page (rp) to init#
   var s2 String$("world")        ; Pass local page to init#
   var s3 String^hashMap("key")   ; Pass named page (hashMap) to init#
   ```

   The `init#` pattern is for types where:
   - The struct itself is a value type (can live on stack)
   - But internal data needs to be allocated on a page
   - The call-site lifetime determines which page to use

   Reserved: `init$`, `init^`, `init!` are reserved for future use and will error.

### Package System

Scaly has a built-in package system with no external tooling or configuration files.

**Core principles:**
- **Package = source distribution**: A package is a collection of `.scaly` source files, versioned and distributed together
- **No external files**: No `package.json`, `Cargo.toml`, or similar - everything is in the language
- **No external tools**: No `cargo`, `npm`, `nvm` - the compiler handles building, versioning, and dependency resolution
- **Multiple versions coexist**: Different versions of the same package can exist in one binary, solving the diamond dependency problem

**Version in directory structure:** Package versions are encoded in the filesystem path (`packages/scaly/0.1.0/`). No version in code - the directory name is the single source of truth.

**Three distinct statements:**

Scaly has three statements for organizing code, each with a distinct purpose:

**`module`** - Load more source files within the same package:
```scaly
module parser      ; loads ./parser.scaly or ./parser/parser.scaly
module lexer       ; loads ./lexer.scaly
```

**`package`** - Declare dependency on external package:
```scaly
package scaly 0.1.0   ; loads from package search path
```

**`use`** - Shorthand for fully qualified paths (naming convenience only):
```scaly
use scaly.memory.Page   ; now can write Page instead of scaly.memory.Page
use scaly.memory.PageList
```

**Package resolution:** `./packages/` → `/usr/share/scaly/packages/` → `-I` paths. Dependencies are not transitively visible. Circular package dependencies are forbidden.

**Standard library (stdlib):** Primitives and stdlib are implicitly available. Use `--no-prelude` for bare-metal/embedded. Diamond dependencies are solved by compiling both versions - version is encoded in mangled names (Itanium ABI compatible: `json_2_0_1`).

## Project Structure

### Compiler (scalyc/)

Single monolithic executable with GCC-style command line:

```
scalyc [options] <input-files>

Options:
  -o <file>         Output file
  -c                Compile only (no link)
  -S                Emit LLVM IR
  -I<path>          Add package search path
  -L<path>          Add library search path
  -l<library>       Link library
  -flto             Enable Link Time Optimization
  --no-prelude      Disable implicit prelude loading
  --test            Run compiled-in test suite
  --test-name <n>   Run specific test by name
  -v                Verbose output

Pipeline stages (for debugging):
  -lex              Lex input and print tokens
  -parse            Parse input and validate syntax
  -model            Build semantic model
  -plan             Run planner (type resolution, name mangling)
  -run <function>   JIT-execute a function by name
```

**Source files:**
- `main.cpp` - Entry point, command line parsing, test runner
- `Lexer.h/cpp` - Lexical analysis
- `Syntax.h` - Generated AST types from scaly.sgm
- `Parser.h/cpp` - Generated parser from scaly.sgm
- `Model.h` - Semantic model types (with generics, unresolved)
- `ModelError.h/cpp` - Errors for semantic analysis
- `Modeler.h/cpp` - Syntax → Model transformation
- `Plan.h` - Resolved model (concrete types, mangled names)
- `PlannerError.h/cpp` - Errors for planning phase
- `Planner.h/cpp` - Model → Plan (inference, monomorphization, mangling)
- `Emitter.h/cpp` - Plan → LLVM IR generation, JIT execution, AOT compilation

**Test files:**
- `ExpressionTests.cpp` - Generated from tests/expressions.sgm
- `DefinitionTests.cpp` - Generated from tests/definitions.sgm
- `ChooseTests.cpp` - Generated from tests/choose.sgm
- `ControlFlowTests.cpp` - Generated from tests/controlflow.sgm
- `LexerTests.cpp` - Manual lexer tests
- `ParserTests.cpp` - Manual parser tests
- `ModuleTests.cpp` - Package/module loading tests
- `EmitterTests.cpp` - LLVM IR generation tests

### Compiler Pipeline Architecture

```
Source (.scaly)
    ↓
Lexer → Tokens
    ↓
Parser → Syntax (AST, generated from scaly.sgm)
    ↓
Modeler → Model (semantic model, generics unresolved)
    ↓
Planner → Plan (concrete types, mangled names, fully typed)
    ↓
Emitter → LLVM IR + DWARF debug info
    ↓
LLVM → Object code / JIT execution
```

### Planner Architecture (Model → Plan)

The Planner transforms the abstract semantic Model into a concrete execution Plan:

1. **Type Inference** (Hindley-Milner): Generate type variables, collect constraints, unify/solve
2. **Monomorphization**: Instantiate generic types with concrete types, cache instantiations
3. **Name Mangling** (Itanium ABI): `List[int].get(index: int)` → `_ZN4ListIiE3getEi` (works with `c++filt`)

Debug info preserves source locations through the pipeline for CodeLLDB debugging.

### Language Specification (codegen/)

DSSSL/Scheme scripts that process scaly.sgm:
- `scaly.dsl` - Main DSSSL stylesheet (entry point)
- `parser.scm` - Parser generation logic
- `syntax.scm`, `syntax-scaly.scm` - Syntax/AST code generation
- `helpers.scm`, `rules.scm` - Helper and rule functions
- `testcpp.scm`, `testdoc.scm` - Test generation for C++ and docs
- `fodeclare.scm` - FOD (Flow Object Declaration) processing
- `grammar-doc.scm` - Grammar documentation generation
- `test-expressions.dsl`, `test-definitions.dsl`, `test-choose.dsl`, `test-controlflow.dsl` - Individual test stylesheets

### Standard Library and Runtime (packages/)

The `packages/` directory contains the **active development** of Scaly's standard library and runtime. This is where new Scaly code is written for the "real" compiler (LLVM-based).

**Current structure:**
```
packages/
└── scaly/
    └── 0.1.0/
        ├── scaly.scaly           # Package entry with test orchestration
        └── scaly/
            ├── memory.scaly      # Memory module (Page-based allocation)
            ├── memory/
            │   ├── Page.scaly
            │   ├── PageNode.scaly
            │   ├── PageList.scaly
            │   └── PageListIterator.scaly
            ├── containers.scaly  # Containers (Vector, List, HashMap, etc.)
            ├── io.scaly          # File/Directory I/O, Console
            └── llvm.scaly        # LLVM-C API bindings (for self-hosting)
```

**Development approach:**
- Build stdlib incrementally, testing each component with the LLVM-based compiler
- Start with low-level memory primitives (Page-based allocation)
- Add containers (Vector, List, HashMap) as the compiler supports them
- Reference `retired/packages/scaly/` for design patterns and API shapes
- Adapt retired code to work with the new compiler (no transpiler, direct LLVM)

**Testing the active packages:**
```bash
# Test package planning
./scalyc/build/scalyc -v --plan packages/scaly/0.1.0/scaly.scaly

# Test individual modules
./scalyc/build/scalyc -v --plan packages/scaly/0.1.0/scaly/memory/Page.scaly
```

### Reference Implementation (retired/)

The `retired/` directory contains the previous transpiler-based implementation (C++ target). Not actively developed but useful as reference for API design, algorithms, and syntax patterns. The retired code exercises complex features (generics, self-referencing types, `choose`/`when`, lifetime annotations) and can be used to test the compiler:

```bash
./scalyc/build/scalyc -v -plan retired/packages/scaly/scaly/containers/Node.scaly
```

## Language Features

Scaly syntax includes:
- Module system with `define` and `module` declarations
- Memory regions for automatic memory management (RBMM)
- Pattern matching with `choose`/`when`/`else`
- Package imports and namespace resolution

### Lexical Structure
- Single line comments start with a semicolon (;)
- Multiline comments: `;*` to `*;`
- Line feeds separate language constructs (can be replaced by colons)
- Operator identifiers: start with arithmetic character (+, -, etc.)
- Normal identifiers: alphanumeric followed by alphanumeric or whitespace
- String identifiers: enclosed in single quotes

### Grammar Details
- Operators are not hardcoded; no built-in operator precedence in the grammar
- Functions apply as prefix, operators as postfix (binary)
- See scaly.sgm for full language specification (grammar, tests, and documentation)

### Implicit Returns

Like Rust and ML, Scaly supports **implicit returns** - the last expression in a function body is the return value without needing an explicit `return` keyword:

```scaly
function get_capacity(this: Page, align: size_t) returns size_t
{
    var location next_object as size_t
    let aligned_location (location + align - 1) & (~(align - 1))
    var location_after_page this as size_t + PAGE_SIZE
    var capacity location_after_page - aligned_location
    capacity  ; implicit return - no 'return' keyword needed
}

function equals(this: String, other: String) returns bool
{
    if length <> other.get_length()
        return false  ; early return still uses 'return'

    if data = null
        return true

    memcmp(data, other.data, length) = 0  ; implicit return
}
```

**Patterns that work:**
- Simple variables: `capacity`, `result`, `i`
- Pointer arithmetic: `data + i`, `data + index + 1`
- Literals: `true`, `false`, `0`
- Comparisons: `memcmp(...) = 0`, `length = 0`
- Function calls: `buffer.get_length()`, `hashing.hash(...)`
- Constructor calls: `Slice[T](data, length)`, `ListIterator[T](head)`

**Known limitation:**
- Address-of expressions (`&(*head).element`) as implicit returns cause type mismatch errors - use explicit `return` for these

**Style guideline:** Prefer implicit returns for the final expression. Use explicit `return` only for early exits in the middle of a function.

### Single-Line Function Bodies

When a function, procedure, or operator has a single-expression body, the braces can be omitted entirely. The expression is placed on the next line, indented:

```scaly
; Single-line functions - no braces needed
function get_iterator(this: List[T]) returns ListIterator[T]
    ListIterator[T](head)

function is_empty(this: Slice[T]) returns bool
    length = 0

function get_length(this: StringBuilder) returns size_t
    buffer.get_length()

operator ==(other: Slice[T]) returns bool
    equals(other)
```

**When to use single-line syntax:**
- Function body is a single expression (no `let`, `var`, `set`, `if`, `while`, etc.)
- The expression fits comfortably on one line
- No early returns needed

**When to keep braces:**
- Multiple statements in the body
- Control flow (`if`, `while`, `for`, `match`)
- Variable bindings (`let`, `var`)
- Side effects with `set`

```scaly
; Keep braces for multi-statement bodies
function get(this: Vector[T], i: size_t) returns pointer[T]
{
    if i >= length
        return null
    data + i
}
```

### Code Style: Allman Braces

Scaly uses **Allman style** (also called BSD style) for brace placement. All opening braces go on their own line:

```scaly
function allocate(this: Page, size: size_t) returns pointer[void]
{
    if capacity < size
    {
        return allocate_oversized(size)
    }
    else
        return null
}

define Vector[T]
(
    length: size_t
    data: pointer[T]
)
{
    init ()
    {
        set length: 0
        set data: null
    }
}
```

**Why Allman style:**

1. **Future doc attributes**: Allows documentation attributes between signature and body:
   ```scaly
   function allocate(this: Page, size: size_t) returns pointer[void]
       @summary "Allocates memory from this page"
       @param size "Number of bytes to allocate"
   {
       ...
   }
   ```

2. **Visual clarity**: Block boundaries are immediately visible - the opening and closing braces align vertically

3. **Historical precedent**: Named after Eric Allman, a UC Berkeley hacker who wrote many BSD Unix utilities (including sendmail) in the late 1970s. The style resembles Pascal/Algol block structure.

4. **Modern tooling**: Works well with IDE code folding and diff tools

**Single-line bodies**: When a control structure has a single statement, braces can be omitted:
```scaly
if capacity < size
    return allocate_oversized(size)
else
    return null
```

### Operator Precedence

Precedence is encoded in **wrapper types** (e.g., `Sum`, `Product`). No hardcoded operators - precedence emerges from type-based dispatch. `3 + 4 * 5` → `Sum(3, 4) * 5` → `Sum(3, 20)`. Parentheses collapse wrappers.

### Programs and Libraries
- A program contains top-level statements with access to argc/argv
- A library has no top-level statements and compiles to an archive

## Documentation

The `docs/` subdirectory contains the documentation for the Scaly language. Documentation is built using DocBook and deployed to scaly.io.

### Deploying to scaly.io

```bash
# Deploy documentation to S3 bucket
aws s3 sync docs/ s3://scaly.io --delete
```

## Commit Policy

**IMPORTANT: READ THIS BEFORE EVERY COMMIT**

Commit messages MUST be:
- **ONE LINE ONLY** - no multi-line messages, no body, no bullet points
- **50 characters or less** - this is a hard limit, not a suggestion
- **No co-author lines** - do not add "Co-Authored-By" footers
- **No emoji** - unless explicitly requested by the user
- **No marketing** - no "Generated with Claude Code" or similar

**Good examples:**
```
Add type inference to Planner
Fix binding scope in planFor
Document packages directory
```

**Bad examples:**
```
Add expression type inference to Planner

- inferConstantType: maps constants to their types    <- NO! Multi-line
- resolveNameOrVariable: distinguishes variables      <- NO! Bullet points

🤖 Generated with [Claude Code]                       <- NO! Marketing
Co-Authored-By: Claude <noreply@anthropic.com>        <- NO! Co-author
```

**Format:** `<what changed>` - be concise and descriptive

## Development Commands

### Quick Start

```bash
./build.sh        # Regenerate code + build compiler
./build.sh test   # Regenerate + build + run tests
```

### build.sh

Main build script that orchestrates the full workflow:

```bash
#!/bin/bash
set -e

# Regenerate from spec
./mkp

# Build compiler
cd scalyc
mkdir -p build
cd build
cmake ..
make

# Optionally run tests
if [ "$1" = "test" ]; then
    ./scalyc --test
fi
```

### Individual Steps

```bash
# Regenerate code from specification only
./mkp   # Generates Parser.cpp, Syntax.h, *Tests.cpp, docs/*.xml

# Build only (after code generation)
cd scalyc/build && make

# Run all JIT tests
./scalyc/build/scalyc --test

# Run specific JIT test
./scalyc/build/scalyc --test-name arithmetic

# Run AOT test (compile and execute)
./scalyc/build/scalyc -o tests/aot/build/hello tests/aot/hello.scaly
./tests/aot/build/hello
```

### mkp

Runs openjade to generate parser and test code from SGML:
- `scalyc/Parser.cpp` - Generated parser from `scaly.sgm`
- `scalyc/Syntax.h` - Generated AST types from `scaly.sgm`
- `scalyc/ExpressionTests.cpp` - Generated from `tests/expressions.sgm`
- `scalyc/DefinitionTests.cpp` - Generated from `tests/definitions.sgm`
- `scalyc/ChooseTests.cpp` - Generated from `tests/choose.sgm`
- `scalyc/ControlFlowTests.cpp` - Generated from `tests/controlflow.sgm`
- `docs/*.xml` - DocBook documentation

### Dependencies

- `clang++` (C++17 support)
- **LLVM 18** (target version, matches Ubuntu 24.04 LTS)
- CMake >= 3.16
- openjade (for SGML/DSSSL processing)

**Ubuntu 24.04 LTS:**

```bash
# Install all dependencies
sudo apt install llvm-18-dev clang-18 cmake openjade zlib1g-dev libzstd-dev

# Set up clang++ to use clang-18
sudo update-alternatives --install /usr/bin/clang++ clang++ /usr/bin/clang++-18 100
sudo update-alternatives --install /usr/bin/clang clang /usr/bin/clang-18 100
```

**Ubuntu 22.04 LTS:**

```bash
# Add LLVM repository first
wget https://apt.llvm.org/llvm.sh
chmod +x llvm.sh
sudo ./llvm.sh 18

# Install dependencies
sudo apt install llvm-18-dev clang-18 cmake openjade zlib1g-dev libzstd-dev

# Set up alternatives
sudo update-alternatives --install /usr/bin/clang++ clang++ /usr/bin/clang++-18 100
sudo update-alternatives --install /usr/bin/clang clang /usr/bin/clang-18 100
```

**macOS (Homebrew):**

```bash
brew install llvm@18 cmake openjade
```

**Why LLVM 18:**
- Ships with Ubuntu 24.04 LTS (supported until 2029)
- Mature ORC JIT for test infrastructure
- Available via Homebrew on macOS
- No bleeding edge instability

## File Structure

```
.
├── scaly.sgm             # Grammar specification (generates Parser.cpp, Syntax.h)
├── grammar.dtd           # DTD for grammar structure
├── test.dtd              # DTD for literate test structure
├── tests/                # Test files
│   ├── expressions.sgm   # Literate: Literals, operations, bindings
│   ├── definitions.sgm   # Literate: Structures, unions, namespaces
│   ├── choose.sgm        # Literate: Pattern matching
│   ├── controlflow.sgm   # Literate: Control flow
│   └── aot/              # AOT tests (standalone Scaly programs)
│       ├── arithmetic.scaly
│       ├── hello.scaly
│       ├── generic_box.scaly
│       └── ... (26+ test files)
├── build.sh              # Main build script (codegen + compile)
├── mkp                   # Codegen script (generates Parser.cpp, Syntax.h, test files)
├── stdlib.scaly          # Standard library (operators, functions)
├── scalyc/               # C++ compiler (CMake project)
│   ├── CMakeLists.txt
│   ├── main.cpp
│   ├── Lexer.h / Lexer.cpp
│   ├── Syntax.h              # Generated AST types
│   ├── Parser.h / Parser.cpp # Generated parser
│   ├── Model.h               # Semantic model (generics unresolved)
│   ├── ModelError.h / ModelError.cpp
│   ├── Modeler.h / Modeler.cpp
│   ├── Plan.h                # Resolved model (concrete, mangled)
│   ├── PlannerError.h / PlannerError.cpp
│   ├── Planner.h / Planner.cpp
│   ├── Emitter.h / Emitter.cpp  # LLVM IR generation, JIT, AOT
│   ├── LexerTests.cpp
│   ├── ParserTests.cpp
│   ├── ModuleTests.cpp       # Package/module loading tests
│   ├── EmitterTests.cpp      # LLVM IR generation tests
│   └── build/            # CMake build directory
├── packages/             # Active stdlib and runtime development
│   └── scaly/
│       └── 0.1.0/        # Scaly core package
│           ├── scaly.scaly
│           └── scaly/
│               ├── memory.scaly, memory/
│               ├── containers.scaly
│               ├── io.scaly
│               └── llvm.scaly
├── codegen/              # DSSSL code generation scripts
│   ├── scaly.dsl         # Main stylesheet (entry point)
│   ├── parser.scm        # Parser generation
│   ├── syntax.scm        # Syntax/AST generation
│   ├── helpers.scm       # Shared utilities
│   └── test-*.dsl        # Individual test stylesheets
├── docs/                 # Generated DocBook (deployed to scaly.io)
└── retired/              # Previous implementations (reference only)
```
