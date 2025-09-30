# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Overview

Scaly is envisioned to be a self-scaling programming language. Scaly is compiled a self-hosted compiler. 

Currently, Scaly works by transpiling to C++. The transpiler performs a mechanical transformation of the Scaly source files with a number of shortcomings:
- Regions must be established explicitly in the code. The final compiler to be developed in this project will hide them.
- Function calls must be formulated by explicitliy passing a return page if it returns an allocated value, and an exception page if it throws an allocated value.
- The life time of references are not checked.
- Namespace qualifiers in expressions must be written with an arrow (->) operator.

This current infrastructure is contained in the packages directory:

- **packages/scaly**: Runtime support and standard library (memory management, containers, I/O, JSON, compiler modules)
- **packages/scalyc**: Command-line program using the transpiler. Stable versions are copied
- **packages/scals**: Scaly Language Server for IDE integration
- **extension**: VSCode extension for Scaly language support

Current development takes place in packages/scalyc. A Planner and Generator are developed and tested which will eventually
implement the final Scaly language implementation.

## Architecture

### Core Components

1. **Scaly Runtime** (`packages/scaly/`): Foundation library with modules for:
   - Memory management (`scaly/memory/`) - custom memory allocator with regions and pages
   - Containers (`scaly/containers/`) - lists, vectors, arrays, hash maps, strings
   - I/O (`scaly/io/`) - file system operations, console I/O
   - JSON parsing (`scaly/json/`)
   - Compiler infrastructure (`scaly/compiler/`) - lexer, parser, modeler, transpiler, LLVM code generator

2. **Self-hosted Compiler** (`packages/scalyc/`): Scaly compiler written in Scaly itself, transpiles Scaly to C++ and compiles with LLVM

3. **Language Server** (`packages/scals/`): LSP implementation for IDE features, still a stub.

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
- **IMPORTANT**: In if statements, the `else` keyword must be on the same line as the closing curly brace of the consequent block. This is a common syntax pitfall.
- For details see scaly.sgm

### Programs and Libraries
- A full program contains top-level statements. These top-level statements have access to argc and argv parameters. 
The compiler compiles the statements into a main function which accepts the usual argc and argv parameters. 
All of the code of the program is linked together to a program.
- A library does not contain top-level statements. All of the code of the program is linked to an archive library.

### Changing the grammar
- Adjust scaly.sgm and run the mkp shell script which will use openjade to re-generate Parser.scaly.

## Development Workflow

### Code Generation Process
The development workflow uses the Transpiler to automatically convert Scaly source code into C++ implementations:

1. **Scaly source files** (`.scaly`) contain the primary implementation
2. **Transpiler** converts `.scaly` files to corresponding `.cpp` and `.h` files
3. **Exception**: `Generator.h` and `Generator.cpp` are manually edited (not generated)
4. **Build process** compiles the generated C++ code with LLVM

This transpilation step is done by calling scalyc in build.sh. This program contains a stable version of the Transpiler
which is used to transpile all of the .scaly files of the current package.

### Important Notes
- **Always edit `.scaly` files** for most changes, not the generated `.cpp/.h` files
- Only `Generator.h` and `Generator.cpp` should be edited directly
- The transpiler will overwrite generated C++ files on each build
- Changes to `.scaly` files require rebuilding to regenerate C++ code
- **Memory Management**: Functions use `rp` (return page) and `ep` (exception page) parameters for memory management - these are required for proper page allocation

## Development Commands

### Building
- See .vscode/launch.json and .vscode/tasks.json how to build and run.
```bash
# Build individual components
cd packages/scaly && ./build.sh    # Build runtime library

# Run the compiled code
cd packages/scaly && bin/scaly
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