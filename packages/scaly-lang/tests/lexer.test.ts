import { describe, it, expect } from 'vitest'
import { Lexer } from '../src/lexer.js'

describe('Lexer', () => {
  it('should handle empty input', () => {
    const lexer = new Lexer('')
    expect(lexer.token._tag).toBe('Empty')
    lexer.advance()
    expect(lexer.token._tag).toBe('Empty')
  })

  it('should tokenize comprehensive test string', () => {
    const input = ` \t\r
;single line comment
;*multi
line
comment;*nested comment*;
comment end*;
abc_AZ0815_7 42
: 0 012 0.34 0.56E12 0.78e13 0xaB 0xCdEf02 0B0 0B1 @ttribute + -0815 /* <> "a string" "\\"\\n\\r\\t" 'a string identifier' \`a string fragment \\\`\\n\\r\\t\``

    const lexer = new Lexer(input)

    // Initially empty
    expect(lexer.token._tag).toBe('Empty')
    lexer.advance()

    // Identifier: abc_AZ0815_7
    expect(lexer.token._tag).toBe('Identifier')
    if (lexer.token._tag === 'Identifier') {
      expect(lexer.token.name).toBe('abc_AZ0815_7')
    }
    lexer.advance()

    // Integer: 42
    expect(lexer.token._tag).toBe('Integer')
    if (lexer.token._tag === 'Integer') {
      expect(lexer.token.value).toBe('42')
    }
    lexer.advance()

    // Colon (from newline)
    expect(lexer.token._tag).toBe('Colon')
    lexer.advance()

    // Colon (explicit :)
    expect(lexer.token._tag).toBe('Colon')
    lexer.advance()

    // Integer: 0
    expect(lexer.token._tag).toBe('Integer')
    if (lexer.token._tag === 'Integer') {
      expect(lexer.token.value).toBe('0')
    }
    lexer.advance()

    // Integer: 012
    expect(lexer.token._tag).toBe('Integer')
    if (lexer.token._tag === 'Integer') {
      expect(lexer.token.value).toBe('012')
    }
    lexer.advance()

    // Float: 0.34
    expect(lexer.token._tag).toBe('FloatingPoint')
    if (lexer.token._tag === 'FloatingPoint') {
      expect(lexer.token.value).toBe('0.34')
    }
    lexer.advance()

    // Float: 0.56E12
    expect(lexer.token._tag).toBe('FloatingPoint')
    if (lexer.token._tag === 'FloatingPoint') {
      expect(lexer.token.value).toBe('0.56E12')
    }
    lexer.advance()

    // Float: 0.78e13
    expect(lexer.token._tag).toBe('FloatingPoint')
    if (lexer.token._tag === 'FloatingPoint') {
      expect(lexer.token.value).toBe('0.78e13')
    }
    lexer.advance()

    // Hex: 0xaB
    expect(lexer.token._tag).toBe('Hex')
    if (lexer.token._tag === 'Hex') {
      expect(lexer.token.value).toBe('0xaB')
    }
    lexer.advance()

    // Hex: 0xCdEf02
    expect(lexer.token._tag).toBe('Hex')
    if (lexer.token._tag === 'Hex') {
      expect(lexer.token.value).toBe('0xCdEf02')
    }
    lexer.advance()

    // Boolean: false (0B0)
    expect(lexer.token._tag).toBe('Boolean')
    if (lexer.token._tag === 'Boolean') {
      expect(lexer.token.value).toBe(false)
    }
    lexer.advance()

    // Boolean: true (0B1)
    expect(lexer.token._tag).toBe('Boolean')
    if (lexer.token._tag === 'Boolean') {
      expect(lexer.token.value).toBe(true)
    }
    lexer.advance()

    // Attribute: ttribute (from @ttribute)
    expect(lexer.token._tag).toBe('Attribute')
    if (lexer.token._tag === 'Attribute') {
      expect(lexer.token.name).toBe('ttribute')
    }
    lexer.advance()

    // Operator: +
    expect(lexer.token._tag).toBe('Identifier')
    if (lexer.token._tag === 'Identifier') {
      expect(lexer.token.name).toBe('+')
    }
    lexer.advance()

    // Operator: -
    expect(lexer.token._tag).toBe('Identifier')
    if (lexer.token._tag === 'Identifier') {
      expect(lexer.token.name).toBe('-')
    }
    lexer.advance()

    // Integer: 0815
    expect(lexer.token._tag).toBe('Integer')
    if (lexer.token._tag === 'Integer') {
      expect(lexer.token.value).toBe('0815')
    }
    lexer.advance()

    // Operator: /*
    expect(lexer.token._tag).toBe('Identifier')
    if (lexer.token._tag === 'Identifier') {
      expect(lexer.token.name).toBe('/*')
    }
    lexer.advance()

    // Operator: <>
    expect(lexer.token._tag).toBe('Identifier')
    if (lexer.token._tag === 'Identifier') {
      expect(lexer.token.name).toBe('<>')
    }
    lexer.advance()

    // String: "a string"
    expect(lexer.token._tag).toBe('String')
    if (lexer.token._tag === 'String') {
      expect(lexer.token.value).toBe('a string')
    }
    lexer.advance()

    // String with escapes: "\"\n\r\t"
    expect(lexer.token._tag).toBe('String')
    if (lexer.token._tag === 'String') {
      expect(lexer.token.value).toBe('"\n\r\t')
    }
    lexer.advance()

    // Character literal: 'a string identifier'
    expect(lexer.token._tag).toBe('Character')
    if (lexer.token._tag === 'Character') {
      expect(lexer.token.value).toBe('a string identifier')
    }
    lexer.advance()

    // Fragment literal: `a string fragment \`\n\r\t`
    expect(lexer.token._tag).toBe('Fragment')
    if (lexer.token._tag === 'Fragment') {
      expect(lexer.token.value).toBe('a string fragment `\n\r\t')
    }
  })
})
