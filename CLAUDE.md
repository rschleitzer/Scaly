# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

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

## Project Structure

### Compiler (scalyc/)

Single monolithic executable with GCC-style command line:

```
scalyc [options] <input-files>

Options:
  -o <file>      Output file
  -c             Compile only (no link)
  -S             Emit LLVM IR
  --test         Run compiled-in test suite
  --test=<name>  Run specific test
  -v             Verbose output
```

**Source files:**
- `main.cpp` - Entry point, command line parsing
- `Lexer.h/cpp` - Lexical analysis
- `Parser.h/cpp` - Generated from scaly.sgm
- `AST.h/cpp` - Abstract syntax tree nodes
- `Modeler.h/cpp` - Semantic analysis
- `Codegen.h/cpp` - LLVM IR generation
- `Tests.h/cpp` - Generated from scaly.sgm

### Language Specification (codegen/)

DSSSL/Scheme scripts that process scaly.sgm:
- `scaly.dsl` - Main DSSSL stylesheet
- `parser.scm` - Parser generation logic
- `tests.scm` - Test code generation logic
- `docbook.scm` - Documentation generation logic
- `helpers.scm` - Shared helper functions

### Reference Implementation (Retired)

The `retired/` directory contains previous implementation attempts for reference:

- **retired/packages/**: Original Scaly runtime, compiler (scalyc), and language server (scals)
- **retired/extension/**: VSCode extension for Scaly language support

This retired code demonstrates:
- Memory management with regions and pages (`rp`/`ep` parameters)
- The transpiler converting `.scaly` to C++

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

- Use short, one-line commit messages (50 chars or less ideal)
- No marketing blurbs or promotional text
- No emojis unless explicitly requested
- Format: `<what changed>` - be concise and descriptive

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
│   ├── Parser.h / Parser.cpp   # Generated
│   ├── Tests.h / Tests.cpp     # Generated
│   ├── AST.h / AST.cpp
│   ├── Modeler.h / Modeler.cpp
│   ├── Codegen.h / Codegen.cpp
│   └── build/            # CMake build directory
├── codegen/              # DSSSL code generation scripts
│   ├── scaly.dsl         # Main stylesheet
│   ├── parser.scm        # Parser generation
│   ├── tests.scm         # Test generation
│   ├── docbook.scm       # Documentation generation
│   └── helpers.scm       # Shared utilities
├── docs/                 # Generated DocBook (deployed to scaly.io)
└── retired/              # Previous implementations (reference)
```
