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

### Parser Generator

The parser is generated from the SGML grammar:
- **scaly.sgm**: Grammar definition for the Scaly language
- **codegen/**: DSSSL/Scheme scripts that generate parser code
- **mkp**: Shell script to regenerate parser using openjade

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

### Active Development (C++ Compiler)

- **scalyc/**: C++ compiler implementation
  - Lexer
  - Parser (generated from grammar)
  - Semantic analysis / modeler
  - LLVM code generation
  - JIT test runner

### Grammar and Code Generation

- **scaly.sgm**: SGML grammar definition for the Scaly language
- **codegen/**: DSSSL/Scheme scripts for parser generation
  - `parser.scm` - Parser generation logic
  - `helpers.scm` - Helper functions
  - `syntax.scm` - Syntax definitions
  - `scaly.dsl` - DSSSL stylesheet
- **mkp**: Shell script to regenerate parser from grammar using openjade

### Test Infrastructure

- **tests.sgm**: Test definitions in XML format
- Tests are compiled and executed via LLVM JIT during development
- Enables rapid iteration: change code, run tests, see results immediately

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
- See scaly.sgm for full grammar specification

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

### Building and Testing

```bash
# Build the compiler
./build.sh

# Run tests (JIT execution of test cases)
./test.sh
```

### Changing the Grammar

```bash
# Edit scaly.sgm, then regenerate parser:
./mkp
```

This runs `openjade -G -t sgml -d codegen/scaly.dsl scaly.sgm`

### Dependencies

- C++ compiler with C++17 or later support
- LLVM development libraries
- openjade (for grammar processing)

## File Structure

```
.
├── scalyc/               # C++ compiler implementation
├── docs/                 # Documentation (deployed to scaly.io)
├── codegen/              # DSSSL parser generation scripts
├── scaly.sgm             # Grammar definition
├── stdlib.scaly          # Standard library (operators, functions)
├── tests.sgm             # Test definitions (XML format, JIT-executed)
├── mkp                   # Parser generation script
├── build.sh              # Build script
├── test.sh               # Test runner script
└── retired/              # Previous implementations (reference)
```
