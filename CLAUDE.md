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

- `.scaly` files: Scaly source code
- `.cpp/.h` files: Generated C++ code and manual implementations
- `build.sh`: Build scripts for each component
- `main.cpp`: Entry points for executables