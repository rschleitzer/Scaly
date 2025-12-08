# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Overview

Scaly is envisioned to be a self-scaling programming language. The goal is a self-hosted compiler that manages memory through regions automatically.

The current approach uses a **TypeScript bootstrapper** to develop and test the language implementation before eventually achieving self-hosting. This replaces the previous C++ transpilation approach.

## Development Strategy

### Overall Approach

1. **Stay with TypeScript** as the implementation language for the bootstrapper
2. **Port parser and modeler** from `retired/packages` (Scaly code) to TypeScript
3. **Build a transpiler targeting TypeScript** (instead of C++)
4. **Switch back to Scaly** once the TS transpiler works - the Scaly code should closely resemble the retired implementation by then

### Parser Generator

The parser will be generated, not hand-written:
- Copy the parser generator infrastructure from `retired/packages`
- Modify the DSSSL/Scheme code in `codegen/` to emit TypeScript instead of Scaly
- The grammar (`scaly.sgm`) stays the same; only the output format changes

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
function formatName(first: String, last: String) -> String {
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
- Operator identifiers need mangling in TypeScript output (and any traditional-language backend):
  - Arithmetic: `+` → `op$plus`, `>=` → `op$gte`
  - String identifiers: `'divided by'` → requires robust encoding
- String identifiers can contain arbitrary content (escapes, Unicode, quotes), so mangling must be:
  - Deterministic (same input → same output)
  - Reversible (for error messages/debugging)
  - Valid in target language

**Benefits of this distinction:**
- Explicit at call site - you know if something can mutate
- Compiler enforced - functions can't mutate, procedures can
- Parallelism-friendly - functions and operators are automatically safe to parallelize
- Builders fit naturally - procedures work with mutable state

**In the TypeScript bootstrapper**, this is modeled as:
- Functions: all params treated as `readonly`, return results
- Procedures: can take mutable references

This prepares for Scaly's region-based memory management while keeping the TypeScript implementation pragmatic.

### Incremental REPL Development

The REPL (in scaly-doc repo) drives development with incremental functionality:

1. **Literals**: numbers, strings, booleans
2. **Operators**: arithmetic, comparison
3. **Variables/bindings**: let expressions
4. **Function calls**: built-in and user-defined
5. **Function definitions**: lambdas and named functions
6. **Control flow**: if/else, pattern matching
7. **Data structures**: tuples, records, lists
8. **Modules**: imports and namespaces

Each stage: parse → evaluate → print result, growing the language incrementally.

## Project Structure

### Active Development (TypeScript Bootstrapper)

The project uses npm workspaces with these packages:

- **packages/scaly-lang** (`@scaly/lang`): Core language implementation
  - Lexer (`src/lexer.ts`)
  - Evaluator (`src/evaluator.ts`)
  - Transpiler (`src/transpiler/`)

- **packages/scaly-containers** (`@scaly/containers`): Container library (String, Vector, HashMap)

- **packages/scaly-io** (`@scaly/io`): I/O operations (File, Console, Path, Directory)

- **packages/scaly-repl** (`@scaly/repl`): REPL scaffold (main implementation in scaly-doc repo)

### Grammar and Code Generation

- **scaly.sgm**: SGML grammar definition for the Scaly language
- **codegen/**: DSSSL/Scheme scripts for parser generation
  - `parser.scm` - Parser generation logic
  - `helpers.scm` - Helper functions
  - `syntax.scm` - Syntax definitions
  - `scaly.dsl` - DSSSL stylesheet
- **mkp**: Shell script to regenerate parser from grammar using openjade

### Reference Implementation (Retired)

The `retired/` directory contains the previous Scaly-based infrastructure for reference:

- **retired/packages/**: Original Scaly runtime, compiler (scalyc), and language server (scals)
- **retired/extension/**: VSCode extension for Scaly language support

This retired code demonstrates:
- Memory management with regions and pages (`rp`/`ep` parameters)
- The transpiler converting `.scaly` to C++
- LLVM code generation

## Language Features

Scaly syntax includes:
- Module system with `define` and `module` declarations
- Memory regions for automatic memory management
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
- Operators are not hardcoded; no built-in operator precedence
- Functions apply as prefix, operators as postfix
- **IMPORTANT**: In if statements, `else` must be on the same line as the closing brace
- See scaly.sgm for full grammar specification

### Programs and Libraries
- A program contains top-level statements with access to argc/argv
- A library has no top-level statements and compiles to an archive

## Documentation

The **scaly-doc** repository contains the growing documentation for the Scaly language and the REPL implementation. This documentation must be kept in sync with the current state of the compiler/transpiler as features are added or changed.

## Commit Policy

- Use short, one-line commit messages (50 chars or less ideal)
- No marketing blurbs or promotional text
- No emojis unless explicitly requested
- Format: `<what changed>` - be concise and descriptive

## Development Commands

### Building and Testing

```bash
# Install dependencies
npm install

# Build all packages
npm run build

# Run tests
npm test
```

### Changing the Grammar

```bash
# Edit scaly.sgm, then regenerate parser:
./mkp
```

This runs `openjade -G -t sgml -d codegen/scaly.dsl scaly.sgm`

### Dependencies

- Node.js >= 20.0.0
- npm (for workspace management)
- openjade (for grammar processing)

## File Structure

```
.
├── packages/              # TypeScript packages (npm workspaces)
│   ├── scaly-lang/       # Core: lexer, parser, evaluator
│   ├── scaly-containers/ # Container types
│   ├── scaly-io/         # I/O operations
│   └── scaly-repl/       # REPL scaffold (see scaly-doc)
├── codegen/              # DSSSL parser generation scripts
├── scaly.sgm             # Grammar definition
├── mkp                   # Parser generation script
├── retired/              # Previous Scaly-based implementation (reference)
└── package.json          # Root workspace configuration
```
