#!/usr/bin/env node
// Scaly REPL

import * as readline from 'node:readline'
import { Lexer, Parser, evaluate, parseAndModel, formatModelError } from '@scaly/lang'

const rl = readline.createInterface({
  input: process.stdin,
  output: process.stdout,
})

type Mode = 'eval' | 'lex' | 'parse' | 'model'
let mode: Mode = 'eval'

function formatSyntax(obj: unknown, indent: number = 0): string {
  const pad = '  '.repeat(indent)
  if (obj === null || obj === undefined) return `${pad}null`
  if (typeof obj !== 'object') return `${pad}${JSON.stringify(obj)}`
  if (Array.isArray(obj)) {
    if (obj.length === 0) return `${pad}[]`
    return `${pad}[\n${obj.map(item => formatSyntax(item, indent + 1)).join(',\n')}\n${pad}]`
  }
  const o = obj as Record<string, unknown>
  const syntaxTag = o._syntax as string | undefined
  const entries = Object.entries(o).filter(([k]) => k !== '_syntax' && k !== 'start' && k !== 'end')
  if (syntaxTag) {
    if (entries.length === 0) return `${pad}${syntaxTag}`
    if (entries.length === 1 && typeof entries[0][1] !== 'object') {
      return `${pad}${syntaxTag} ${JSON.stringify(entries[0][1])}`
    }
    return `${pad}${syntaxTag}\n${entries.map(([k, v]) => `${pad}  ${k}: ${formatSyntax(v, indent + 2).trimStart()}`).join('\n')}`
  }
  return `${pad}{\n${entries.map(([k, v]) => `${pad}  ${k}: ${formatSyntax(v, indent + 2).trimStart()}`).join(',\n')}\n${pad}}`
}

function showHelp(): void {
  console.log(`Commands:
  :q, :quit     Exit REPL
  :h, :help     Show this help
  :m, :mode     Show current mode
  :e, :eval     Switch to eval mode (default)
  :l, :lex      Switch to lexer mode (show tokens)
  :p, :parse    Switch to parser mode (show AST)
  :o, :model    Switch to model mode (show semantic model)`)
}

function handleLex(input: string): void {
  const lexer = new Lexer(input)
  const tokens: string[] = []
  while (!lexer.isAtEnd()) {
    lexer.advance()
    const t = lexer.token
    if (t._tag === 'Empty') break
    switch (t._tag) {
      case 'Identifier': tokens.push(`Identifier(${t.name})`); break
      case 'Attribute': tokens.push(`Attribute(${t.name})`); break
      case 'Punctuation': tokens.push(`Punctuation(${t.sign})`); break
      case 'String': tokens.push(`String("${t.value}")`); break
      case 'Character': tokens.push(`Character('${t.value}')`); break
      case 'Fragment': tokens.push(`Fragment(\`${t.value}\`)`); break
      case 'Integer': tokens.push(`Integer(${t.value})`); break
      case 'FloatingPoint': tokens.push(`Float(${t.value})`); break
      case 'Hex': tokens.push(`Hex(${t.value})`); break
      case 'Boolean': tokens.push(`Boolean(${t.value})`); break
      case 'Colon': tokens.push('Colon'); break
      case 'Invalid': tokens.push('Invalid'); break
    }
  }
  console.log(tokens.join(' '))
}

function handleParse(input: string): void {
  const parser = new Parser(input)
  // Try to parse as an operation (expression)
  const result = parser.parseOperation()
  if (result.ok) {
    console.log(formatSyntax(result.value))
  } else {
    if (result.error._tag === 'InvalidSyntax') {
      console.error(`Parse error at ${result.error.start}-${result.error.end}: ${result.error.message}`)
    } else {
      console.error('Parse error: unexpected syntax')
    }
  }
}

function handleEval(input: string): void {
  const result = evaluate(input)
  switch (result._tag) {
    case 'Ok':
      console.log(result.value)
      break
    case 'ParseError':
      console.error(`Parse error: ${result.message}`)
      break
    case 'EvalError':
      console.error(`Eval error: ${result.message}`)
      break
  }
}

function formatModel(obj: unknown, indent: number = 0): string {
  const pad = '  '.repeat(indent)
  if (obj === null || obj === undefined) return `${pad}null`
  if (typeof obj !== 'object') return `${pad}${JSON.stringify(obj)}`
  if (Array.isArray(obj)) {
    if (obj.length === 0) return `${pad}[]`
    return `${pad}[\n${obj.map(item => formatModel(item, indent + 1)).join(',\n')}\n${pad}]`
  }
  const o = obj as Record<string, unknown>
  const modelTag = o._model as string | undefined
  const entries = Object.entries(o).filter(([k]) => k !== '_model' && k !== 'span')
  if (modelTag) {
    if (entries.length === 0) return `${pad}${modelTag}`
    if (entries.length === 1 && typeof entries[0][1] !== 'object') {
      return `${pad}${modelTag} ${JSON.stringify(entries[0][1])}`
    }
    return `${pad}${modelTag}\n${entries.map(([k, v]) => `${pad}  ${k}: ${formatModel(v, indent + 2).trimStart()}`).join('\n')}`
  }
  return `${pad}{\n${entries.map(([k, v]) => `${pad}  ${k}: ${formatModel(v, indent + 2).trimStart()}`).join(',\n')}\n${pad}}`
}

function handleModel(input: string): void {
  const result = parseAndModel(input)
  if (result.ok) {
    console.log(formatModel(result.value))
  } else {
    console.error(formatModelError(result.error, input))
  }
}

function prompt(): void {
  const modeIndicator = mode === 'eval' ? '' : `[${mode}] `
  rl.question(`${modeIndicator}scaly> `, (input) => {
    if (input.trim() === '') {
      prompt()
      return
    }

    // Commands
    if (input.startsWith(':')) {
      const cmd = input.slice(1).toLowerCase()
      switch (cmd) {
        case 'q': case 'quit':
          rl.close()
          return
        case 'h': case 'help':
          showHelp()
          break
        case 'm': case 'mode':
          console.log(`Current mode: ${mode}`)
          break
        case 'e': case 'eval':
          mode = 'eval'
          console.log('Switched to eval mode')
          break
        case 'l': case 'lex':
          mode = 'lex'
          console.log('Switched to lexer mode')
          break
        case 'p': case 'parse':
          mode = 'parse'
          console.log('Switched to parser mode')
          break
        case 'o': case 'model':
          mode = 'model'
          console.log('Switched to model mode')
          break
        default:
          console.error(`Unknown command: ${input}`)
      }
      prompt()
      return
    }

    // Process input based on mode
    switch (mode) {
      case 'lex':
        handleLex(input)
        break
      case 'parse':
        handleParse(input)
        break
      case 'model':
        handleModel(input)
        break
      case 'eval':
        handleEval(input)
        break
    }

    prompt()
  })
}

console.log('Scaly REPL (type :h for help, :q to quit)')
prompt()
