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

The entire language is defined in a single SGML document that serves as the source of truth for grammar, tests, and documentation:

```
scaly.sgm  →  ./mkl.sh (openjade + DSSSL)  →  scalyc/Parser.cpp
                                            →  scalyc/Tests.cpp
                                            →  docs/*.xml (DocBook)
```

**Source files:**
- **scaly.sgm**: Unified language specification (grammar + tests + documentation)
- **language.dtd**: DTD governing the SGML structure
- **codegen/scaly.dsl**: DSSSL stylesheet that generates all outputs

**Generated outputs:**
- **scalyc/Parser.cpp**: Generated parser code
- **scalyc/Tests.cpp**: Generated test code (compiled into scalyc)
- **docs/*.xml**: DocBook chapters for the language reference

**Benefits of unification:**
- Single source of truth - grammar, tests, and docs are co-located
- Tests serve as executable specification examples
- Documentation always matches implementation
- One codegen step generates everything
- DTD enforces structural consistency

**Codegen script:**
```bash
./mkl.sh   # Regenerates Parser.cpp, Tests.cpp, and docs/*.xml
```

This runs `openjade -G -t sgml -d codegen/scaly.dsl scaly.sgm`

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
function formatName(first: String, last: String) returns String {
  StringBuilder.build(b => {
    b.append(first)
    b.append(" ")
    b.append(last)
  })
}

; Procedure - can mutate the builder
procedure appendItems(builder: StringBuilder, items: List[String]) {
  for item in items {
    builder.append(item)
  }
}
```

**Operators** are always pure (like functions):
- No side effects, always return a result

**Benefits of this distinction:**
- Explicit at call site - you know if something can mutate
- Compiler enforced - functions can't mutate, procedures can
- Parallelism-friendly - functions and operators are automatically safe to parallelize
- Builders fit naturally - procedures work with mutable state

### Package System

Scaly has a built-in package system with no external tooling or configuration files.

**Core principles:**
- **Package = source distribution**: A package is a collection of `.scaly` source files, versioned and distributed together
- **No external files**: No `package.json`, `Cargo.toml`, or similar - everything is in the language
- **No external tools**: No `cargo`, `npm`, `nvm` - the compiler handles building, versioning, and dependency resolution
- **Multiple versions coexist**: Different versions of the same package can exist in one binary, solving the diamond dependency problem

**Why source-only distribution:**
- **Simplicity**: A package is just a directory of `.scaly` files - copy to distribute
- **Transparency**: Fits open source culture; consumers can read, learn from, and contribute to packages
- **Generics require source**: Monomorphization happens at consumer compile time - generic types need full source anyway
- **Community building**: No barriers to understanding how packages work
- **Future optimization**: Compiled artifact caching (like Rust's `.rlib`) can be added later without changing the distribution model

**Version in directory structure:**

Package versions are encoded in the filesystem - the directory path is the single source of truth:

```
packages/
└── scaly/
    └── 0.1.0/
        ├── scaly.scaly           ; entry file, just contains definitions
        └── scaly/
            ├── memory.scaly      ; define memory { module Page ... }
            └── memory/
                ├── Page.scaly
                ├── PageNode.scaly
                ├── PageList.scaly
                └── PageListIterator.scaly
```

This enables simple distribution - just copy directories.

**No version in code:**

The package version comes entirely from the filesystem path. The entry file contains only definitions:

```scaly
; packages/scaly/0.1.0/scaly.scaly
define scaly {
    module memory
}
```

No `define packagename version` syntax - the version is the directory name. One source of truth.

**Publishing packages:**

Publishing is simple - just host your package directory somewhere:

```bash
# Create a release on GitHub, or host a tarball
# Consumers download and extract:
curl -L https://github.com/user/scaly/archive/0.1.0.tar.gz | tar xz
mv scaly-0.1.0 packages/scaly/0.1.0/
```

No special tooling required. A central registry on scaly.io may be added later for discovery as the ecosystem grows.

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

All three can appear in any source file. The compiler collects `package` declarations during source tree traversal. If multiple files declare the same package with the same version, they're deduplicated. Conflicting versions of the same package within one package are an error.

**Example usage:**
```scaly
; my-app/main.scaly
package scaly 0.1.0

use scaly.memory.Page

function main() {
    let page Page.allocate_page()
    ; ...
}
```

**Package resolution:**

The compiler searches for packages in this order:

1. `./packages/` - Project-local (vendored dependencies, checked into repo)
2. `/usr/share/scaly/packages/` - System-wide (stdlib, OS-installed packages)
3. Paths from `-I` flags (in order given)

For `package scaly 0.1.0`, the compiler looks for:
- `./packages/scaly/0.1.0/scaly.scaly`
- `/usr/share/scaly/packages/scaly/0.1.0/scaly.scaly`
- `<-I path>/scaly/0.1.0/scaly.scaly`

Additional paths (organization shared, personal convenience) are added via `-I`:

```bash
scalyc -I/org/shared/packages -I~/my-packages main.scaly
```

**Transitive dependencies:**

Dependencies are **not transitively visible**. Each package must explicitly declare what it uses:

```
my-app
└── lib-a 1.0
    └── scaly 0.1.0
```

In this example, my-app can only see lib-a's API. Even though lib-a uses scaly internally, my-app cannot access `scaly.memory.Page` unless it explicitly declares `package scaly 0.1.0`.

The compiler still compiles all transitive dependencies (required for monomorphization), but namespace visibility is explicit. This ensures:
- Clean encapsulation - dependencies are implementation details
- No accidental coupling to transitive dependencies
- Explicit, reproducible dependency graphs

If lib-a exposes scaly types in its public API, it should document that consumers need to also declare the dependency.

**Circular dependencies:**

Circular package dependencies are **forbidden** (compiler error):

```
package-a → package-b → package-a   ; ERROR: cycle detected
```

If you have a cycle, extract shared concepts into a separate package:

```
Before:  package-a ←→ package-b
After:   package-a → package-common ← package-b
```

This forces clean architecture and avoids complex initialization ordering issues. Cycles are almost always a sign that some shared concepts should be extracted.

Note: This applies to *packages*. Module cycles within the same package are also discouraged but may be allowed since they compile together.

**Standard library (stdlib):**

The stdlib is implicitly available without declaration:

- **Primitives** (`int`, `bool`, `float`, etc.) are built into the compiler
- **stdlib** (operators, `Vector`, `String` methods, etc.) is automatically available
- stdlib version is tied to compiler version - deterministic and reproducible
- Lives in `/usr/share/scaly/packages/stdlib/<compiler-version>/`

No boilerplate needed for common operations:

```scaly
; Just works - no package declaration needed
function example() returns int {
    3 + 4 * 5   ; operators from stdlib
}
```

**Opting out of stdlib:**

Use `--no-stdlib` for bare-metal/embedded scenarios:

```bash
scalyc --no-stdlib kernel.scaly
```

When opted out, only primitives are available. Use cases:
- OS kernels
- Embedded systems
- Custom runtime implementations
- Freestanding environments

**Diamond dependency solution:**

When dependencies require different versions of the same package:

```
your-app
├── lib-a 1.0 → uses json 2.0.1
└── lib-b 1.0 → uses json 3.0.0
```

Both versions are compiled into the final binary. Each dependent sees only its expected version. Types from different versions are distinct (`json_2_0_1::Value` ≠ `json_3_0_0::Value`).

**Version in mangled names (Itanium compatible):**

Package version is encoded into the namespace name using underscores:

```
Package: json version 2.0.1
Function: parse(input: String)

Namespace: json_2_0_1
Mangled:   _ZN10json_2_0_15parseE...
```

This is 100% Itanium ABI compatible - `c++filt` sees a namespace named `json_2_0_1`. No ABI extensions required.

**Why exact versions:**
- Reproducible builds by default
- No resolver algorithm needed - compiler loads exactly what you specify
- Simple mental model - if it works, it works forever
- Trade-off: more potential duplication, but correctness is prioritized

## Project Structure

### Compiler (scalyc/)

Single monolithic executable with GCC-style command line:

```
scalyc [options] <input-files>

Options:
  -o <file>       Output file
  -c              Compile only (no link)
  -S              Emit LLVM IR
  -I<path>        Add package search path
  --no-stdlib     Disable implicit stdlib (for bare-metal/embedded)
  --test          Run compiled-in test suite
  --test=<name>   Run specific test
  -v              Verbose output
```

**Source files:**
- `main.cpp` - Entry point, command line parsing
- `Lexer.h/cpp` - Lexical analysis
- `Syntax.h` - Generated AST types from scaly.sgm
- `Parser.h/cpp` - Generated parser from scaly.sgm
- `Model.h` - Semantic model types (with generics, unresolved)
- `ModelError.h/cpp` - Errors for semantic analysis
- `Modeler.h/cpp` - Syntax → Model transformation
- `Plan.h` - Resolved model (concrete types, mangled names)
- `PlannerError.h/cpp` - Errors for planning phase
- `Planner.h/cpp` - Model → Plan (inference, monomorphization, mangling)
- `Emitter.h/cpp` - Plan → LLVM IR generation (future)
- `Tests.h/cpp` - Generated from scaly.sgm

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

The Planner transforms the abstract semantic Model into a concrete execution Plan. It has three conceptual phases:

**Phase 1: Type Inference (Hindley-Milner)**
- Generate type variables for unknowns
- Collect constraints from expressions
- Unify/solve constraints
- Support let-polymorphism for generic functions
- Designed for full inference (needed for future DSSSL→Scaly transpiler)

**Phase 2: Monomorphization**
- Instantiate generic types with inferred/specified concrete types
- Create specialized versions of generic functions
- Track instantiation provenance (where generic was defined, where instantiated)
- Cache instantiations to avoid duplicates

**Phase 3: Name Mangling (Itanium ABI inspired)**
- Generate unique symbol names for LLVM
- Format: `_Z` prefix + length-prefixed names + encoded types
- Example: `List[int].get(index: int)` → `_ZN4ListIiE3getEi`
- Compatible with `c++filt` for debugging
- Both readable name (for debug info) and mangled name (for LLVM) stored in Plan

**Key data structures:**

```cpp
// Type variable for inference
struct TypeVariable {
    uint64_t Id;
};

// Instantiation tracking for debug info backtracking
struct InstantiationInfo {
    Span DefinitionLoc;      // Where List[T] was defined
    Span InstantiationLoc;   // Where List[int] was requested
    std::vector<std::string> TypeArgs;
};

// Resolved type with both names
struct PlannedType {
    std::string Name;        // "List.int" - for debug info
    std::string MangledName; // "_Z4ListIiE" - for LLVM
    std::unique_ptr<InstantiationInfo> Origin;
};
```

**Debug info support:**
- All Span information preserved through pipeline
- InstantiationInfo enables backtracking from monomorphized code to source
- Supports CodeLLDB debugging in VS Code
- DWARF generation in Emitter uses readable names + source locations

### Language Specification (codegen/)

DSSSL/Scheme scripts that process scaly.sgm:
- `scaly.dsl` - Main DSSSL stylesheet
- `parser.scm` - Parser generation logic
- `tests.scm` - Test code generation logic
- `docbook.scm` - Documentation generation logic
- `helpers.scm` - Shared helper functions

### Standard Library and Runtime (packages/)

The `packages/` directory contains the **active development** of Scaly's standard library and runtime. This is where new Scaly code is written for the "real" compiler (LLVM-based).

**Current structure:**
```
packages/
└── scaly/
    └── 0.1.0/
        ├── scaly.scaly           # Package entry: define scaly { module memory }
        └── scaly/
            ├── memory.scaly      # Memory module definition
            └── memory/
                ├── Page.scaly
                ├── PageNode.scaly
                ├── PageList.scaly
                └── PageListIterator.scaly
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

### Reference Implementation (Retired)

The `retired/` directory contains the previous transpiler-based implementation. It is **not actively developed** but serves as a rich reference for:

- **Structure and API design**: How modules, types, and functions should be organized
- **Algorithm implementations**: Memory management, containers, JSON parsing, etc.
- **Syntax patterns**: Real-world usage of Scaly language features

**Contents:**
- **retired/packages/scaly/**: Complete Scaly runtime with memory, containers, io, json, compiler
- **retired/extension/**: VSCode extension for Scaly language support

**Key differences from active development:**
| Aspect | Retired (Transpiler) | Active (LLVM) |
|--------|---------------------|---------------|
| Target | C++ source code | LLVM IR directly |
| Runtime | C++ stdlib | Native Scaly runtime |
| Memory | `rp`/`ep` parameters | Automatic RBMM |
| Testing | External C++ compiler | JIT execution |

**Using retired code as reference:**
When implementing new stdlib features, consult the retired code for:
1. Type definitions and API signatures
2. Algorithm logic (adapt, don't copy verbatim)
3. Test case ideas and edge cases
4. Module organization patterns

### Compiler Testing with Retired Code

The `retired/packages/scaly/` package remains useful for testing the compiler's ability to parse and plan complex Scaly code.

**Testing commands:**
```bash
# Test parser/modeler with complex retired code
./scalyc/build/scalyc -v --plan retired/packages/scaly/scaly/containers/Node.scaly

# Test full package parsing
cd retired/packages/scaly && ../../../scalyc/build/scalyc -v --plan scaly.scaly
```

**What the retired code exercises:**
- Generic types: `Node[T]`, `Vector[T]`, `List[T]`, `HashMap[K,V]`
- Self-referencing types: `pointer[Node[T]]` within `Node[T]`
- Module system: `module` declarations, `use` statements
- All syntax: `init`, `procedure`, `function`, `operator`, `choose`/`when`
- Attributes: `@summary`, `@remarks` on types and parameters
- Lifetime annotations: `$` (call), `^name` (reference)

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
- **IMPORTANT**: In if statements, `else` must be on the same line as the closing brace
- See scaly.sgm for full language specification (grammar, tests, and documentation)

### Standard Library and Operator Precedence

The standard library (`stdlib.scaly`) defines arithmetic operators with precedence encoded in **wrapper types**:

```scaly
; Wrapper types encode precedence level
define Sum(left: int, right: int) {
    function value(this) returns int intrinsic
}
define Product(left: int, right: int) {
    function value(this) returns int intrinsic
}

; Primitive operators return wrappers (intrinsic = compiler-implemented)
operator +(left: int, right: int) returns Sum intrinsic
operator *(left: int, right: int) returns Product intrinsic

; Higher precedence operators "reach into" lower precedence wrappers
operator *(left: Sum, right: int) returns Sum {
    Sum(left.left, left.right * right)  ; Multiply only the right component
}

; Lower precedence operators evaluate higher precedence wrappers first
operator +(left: Product, right: int) returns Sum {
    Sum(left.value, right)  ; .value collapses Product to int
}
```

**How it works:**
- `3 + 4` → `Sum(3, 4)` (wrapper, not yet computed)
- `3 + 4 * 5` → `Sum(3, 4) * 5` → `Sum(3, 20)` (multiplication reaches into Sum)
- Parentheses collapse wrappers: `(3 + 4) * 5` → `7 * 5` → `Product(7, 5)`
- Final result collapsed at expression boundary

**Key design principles:**
- No hardcoded operators in runtime - all defined in stdlib
- Precedence emerges from type-based operator dispatch
- Unary functions (`-`, `+`, `abs`) are prefix when context is empty
- Binary operators are postfix, taking left from context and right as argument

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
./mkl.sh

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
./mkl.sh   # Generates Parser.cpp, Tests.cpp, docs/*.xml

# Build only (after code generation)
cd scalyc/build && make

# Run tests only
./scalyc/build/scalyc --test

# Run specific test
./scalyc/build/scalyc --test=arithmetic
```

### mkl.sh

Runs `openjade -G -t sgml -d codegen/scaly.dsl scaly.sgm` to generate:
- `scalyc/Parser.cpp` - Generated parser
- `scalyc/Tests.cpp` - Generated test code
- `docs/*.xml` - DocBook chapters

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
├── scaly.sgm             # Unified language specification (grammar + tests + docs)
├── language.dtd          # DTD governing SGML structure
├── build.sh              # Main build script (codegen + compile)
├── mkl.sh                # Codegen only (generates Parser.cpp, Tests.cpp, docs/)
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
│   ├── Emitter.h / Emitter.cpp  # Future: LLVM IR generation
│   ├── LexerTests.cpp
│   ├── ParserTests.cpp
│   └── build/            # CMake build directory
├── packages/             # Active stdlib and runtime development
│   └── scaly/
│       └── 0.1.0/        # Scaly core package (memory, containers, etc.)
│           ├── scaly.scaly
│           └── scaly/
│               └── memory/
├── codegen/              # DSSSL code generation scripts
│   ├── scaly.dsl         # Main stylesheet
│   ├── parser.scm        # Parser generation
│   ├── tests.scm         # Test generation
│   ├── docbook.scm       # Documentation generation
│   └── helpers.scm       # Shared utilities
├── docs/                 # Generated DocBook (deployed to scaly.io)
└── retired/              # Previous implementations (reference only)
```
