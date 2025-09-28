# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Overview

Scaly is a self-scaling programming language with a self-hosted compiler. The repository contains:

- **packages/scaly**: Runtime support and standard library (memory management, containers, I/O, JSON, compiler modules)
- **packages/scalyc**: Self-hosted Scaly compiler
- **packages/scals**: Scaly Language Server for IDE integration
- **extension**: VSCode extension for Scaly language support

## Architecture

### Core Components

1. **Scaly Runtime** (`packages/scaly/`): Foundation library with modules for:
   - Memory management (`scaly/memory/`) - custom memory allocator with regions and pages
   - Containers (`scaly/containers/`) - lists, vectors, arrays, hash maps, strings
   - I/O (`scaly/io/`) - file system operations, console I/O
   - JSON parsing (`scaly/json/`)
   - Compiler infrastructure (`scaly/compiler/`) - lexer, parser, modeler, transpiler, LLVM code generator

2. **Self-hosted Compiler** (`packages/scalyc/`): Scaly compiler written in Scaly itself, transpiles Scaly to C++ and compiles with LLVM

3. **Language Server** (`packages/scals/`): LSP implementation for IDE features

### Build System

Each package uses shell scripts for building:

- Build runtime: `cd packages/scaly && ./build.sh`
- Build compiler: `cd packages/scalyc && ./build.sh`
- Build language server: `cd packages/scals && ./build.sh`

Build process:
1. Compiles `.scaly` source files to C++ using `scalyc`
2. Links with LLVM libraries using `clang++`
3. Produces native executables in `bin/` directories

### Language Features

Scaly syntax includes:
- Module system with `define` and `module` declarations
- Memory regions for automatic memory management
- Pattern matching with `choose`/`when`/`else`
- Package imports and namespace resolution
- C++ transpilation target with LLVM backend

### Some Lexical Structure Details
- Single line comments start with a semicolon (;)
- Multiline comments start with a semicolon followed by an asterisk and end with an asterisk followed by a semicolon
- A line feed is no white space but separates many language constructs. One or more line feeds can be replaced by a colon and vice versa.
- An operator identifier starts with an arithmetic character like + or - and is followed by zero or more arithmetic characters.
- A normal identifier starts with an alphanumeric character followed by zero or more alphanumeric characters or whitespaces.
- A string identifier starts with a single quote and ends with a single quote with almost everything in between.
- For details see Lexer.scaly

### Some Grammar Details
- Operators are neither hardcoded into the language, nor is an operator precedence defined by the language.
- A function is applied as a prefix on anything that follows, an operator works as a postfix on the preceding operation. Both functions and operators can use either alphanumeric, operator-style, and string identifiers.
- For details see scaly.sgm

### Changing the grammar
- Adjust scaly.sgm and run the mkp shell script which will use openjade to re-generate Parser.scaly.

## Development Workflow

### Code Generation Process
The development workflow uses the Transpiler to automatically convert Scaly source code into C++ implementations:

1. **Scaly source files** (`.scaly`) contain the primary implementation
2. **Transpiler** converts `.scaly` files to corresponding `.cpp` and `.h` files
3. **Exception**: `Generator.h` and `Generator.cpp` are manually edited (not generated)
4. **Build process** compiles the generated C++ code with LLVM

### Important Notes
- **Always edit `.scaly` files** for most changes, not the generated `.cpp/.h` files
- Only `Generator.h` and `Generator.cpp` should be edited directly
- The transpiler will overwrite generated C++ files on each build
- Changes to `.scaly` files require rebuilding to regenerate C++ code

## Development Commands

### Building
```bash
# Build individual components
cd packages/scaly && ./build.sh    # Build runtime library
cd packages/scalyc && ./build.sh   # Build compiler
cd packages/scals && ./build.sh    # Build language server

# Test with simple program
cd packages/scaly && scalyc test.scaly test
```

### Dependencies
- LLVM development libraries (required for compilation)
- clang++ compiler
- Existing `scalyc` binary (for bootstrapping)

## File Structure

- `.scaly` files: Scaly source code (primary implementation)
- `.cpp/.h` files: Generated C++ code (except Generator.cpp/h)
- `Generator.cpp/.h`: Manually edited LLVM code generation
- `build.sh`: Build scripts for each component
- `main.cpp`: Entry points for executables