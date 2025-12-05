// Scaly Evaluator - evaluates generated JavaScript
// TODO: full implementation when parser is generated

import { Lexer } from './lexer.js'

export type EvalResult =
  | { _tag: 'Ok'; value: unknown; code: string }
  | { _tag: 'ParseError'; message: string }
  | { _tag: 'EvalError'; message: string; code: string }

export function evaluate(input: string): EvalResult {
  // For now, just tokenize and return a simple result
  // Full implementation will come when parser is generated
  const lexer = new Lexer(input)
  lexer.advance()

  // Simple literal evaluation for bootstrapping
  if (lexer.token._tag === 'Integer') {
    const code = lexer.token.value
    return { _tag: 'Ok', value: Number(code), code }
  }

  if (lexer.token._tag === 'FloatingPoint') {
    const code = lexer.token.value
    return { _tag: 'Ok', value: Number(code), code }
  }

  if (lexer.token._tag === 'String') {
    const value = lexer.token.value
    const code = JSON.stringify(value)
    return { _tag: 'Ok', value, code }
  }

  if (lexer.token._tag === 'Boolean') {
    const value = lexer.token.value
    const code = String(value)
    return { _tag: 'Ok', value, code }
  }

  if (lexer.token._tag === 'Character') {
    const value = lexer.token.value
    const code = JSON.stringify(value)
    return { _tag: 'Ok', value, code }
  }

  if (lexer.token._tag === 'Fragment') {
    const value = lexer.token.value
    const code = '`' + value + '`'
    return { _tag: 'Ok', value, code }
  }

  if (lexer.token._tag === 'Hex') {
    const code = lexer.token.value
    return { _tag: 'Ok', value: parseInt(code, 16), code }
  }

  return { _tag: 'ParseError', message: 'Only literals supported until parser is generated' }
}
