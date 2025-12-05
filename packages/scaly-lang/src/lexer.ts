// Scaly Lexer - ported from Lexer.scaly

// Token types
export type EmptyToken = { _tag: 'Empty' }
export type InvalidToken = { _tag: 'Invalid' }
export type ColonToken = { _tag: 'Colon' }
export type IdentifierToken = { _tag: 'Identifier'; name: string }
export type AttributeToken = { _tag: 'Attribute'; name: string }
export type PunctuationToken = { _tag: 'Punctuation'; sign: string }

export type StringToken = { _tag: 'String'; value: string }
export type CharacterToken = { _tag: 'Character'; value: string }
export type FragmentToken = { _tag: 'Fragment'; value: string }
export type IntegerToken = { _tag: 'Integer'; value: string }
export type BooleanToken = { _tag: 'Boolean'; value: boolean }
export type FloatingPointToken = { _tag: 'FloatingPoint'; value: string }
export type HexToken = { _tag: 'Hex'; value: string }

export type LiteralToken =
  | StringToken
  | CharacterToken
  | FragmentToken
  | IntegerToken
  | BooleanToken
  | FloatingPointToken
  | HexToken

export type Token =
  | EmptyToken
  | InvalidToken
  | ColonToken
  | IdentifierToken
  | AttributeToken
  | PunctuationToken
  | LiteralToken

export class Lexer {
  token: Token = { _tag: 'Empty' }
  private character: string | null = null
  private chars: string
  private index: number = 0
  previousPosition: number = 0
  position: number = 0

  constructor(input: string) {
    this.chars = input
    this.readCharacter()
    this.skipWhitespace(true)
  }

  private readCharacter(): void {
    if (this.index < this.chars.length) {
      this.character = this.chars[this.index++]
      this.position++
    } else {
      this.character = null
    }
  }

  advance(): void {
    this.previousPosition = this.position

    if (this.character === null) return

    const c = this.character

    // Identifier starting with letter
    if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')) {
      this.token = this.scanIdentifier()
      this.skipWhitespace(false)
      return
    }

    // Integer starting with 1-9
    if (c >= '1' && c <= '9') {
      this.token = this.scanIntegerLiteral(this.index - 1, 0)
      this.skipWhitespace(false)
      return
    }

    // Operators
    if ('+-*/=%&|~<>'.includes(c)) {
      this.token = this.scanOperator()
      this.skipWhitespace(false)
      return
    }

    // Punctuation that doesn't skip line feeds after
    if ('})]?.!$#^'.includes(c)) {
      this.token = { _tag: 'Punctuation', sign: c }
      this.readCharacter()
      this.skipWhitespace(false)
      return
    }

    // Punctuation that skips line feeds after
    if ('{([,'.includes(c)) {
      this.token = { _tag: 'Punctuation', sign: c }
      this.readCharacter()
      this.skipWhitespace(true)
      return
    }

    switch (c) {
      case '\n':
        this.token = this.scanLineFeed()
        break

      case ':':
        this.readCharacter()
        this.token = { _tag: 'Colon' }
        break

      case '0':
        this.token = this.scanNumericLiteral()
        break

      case '@':
        this.readCharacter()
        this.token = this.scanAttribute()
        break

      case '"':
        this.token = this.scanStringLiteral()
        break

      case "'":
        this.token = this.scanCharacterLiteral()
        break

      case '`':
        this.token = this.scanFragmentLiteral()
        break

      default:
        this.token = { _tag: 'Invalid' }
    }

    this.skipWhitespace(false)
  }

  empty(): void {
    this.token = { _tag: 'Empty' }
  }

  private scanLineFeed(): Token {
    while (true) {
      this.readCharacter()
      this.skipWhitespace(false)
      if (this.character === null) return { _tag: 'Colon' }
      if (this.character === '\n') continue
      return { _tag: 'Colon' }
    }
  }

  private scanIdentifier(): Token {
    const start = this.index - 1
    let length = 0

    while (true) {
      if (this.character === null) {
        if (length === 0) return { _tag: 'Invalid' }
        return { _tag: 'Identifier', name: this.chars.slice(start, start + length) }
      }

      const c = this.character
      if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') || c === '_') {
        this.readCharacter()
        length++
      } else {
        return { _tag: 'Identifier', name: this.chars.slice(start, start + length) }
      }
    }
  }

  private scanAttribute(): Token {
    const start = this.index - 1
    let length = 0

    while (true) {
      if (this.character === null) {
        if (length === 0) return { _tag: 'Invalid' }
        return { _tag: 'Attribute', name: this.chars.slice(start, start + length) }
      }

      const c = this.character
      if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') || c === '_') {
        this.readCharacter()
        length++
      } else {
        return { _tag: 'Attribute', name: this.chars.slice(start, start + length) }
      }
    }
  }

  private scanOperator(): Token {
    const start = this.index - 1
    let length = 0

    while (true) {
      this.readCharacter()
      length++

      if (this.character === null) {
        if (length === 1) return { _tag: 'Invalid' }
        return { _tag: 'Identifier', name: this.chars.slice(start, start + length) }
      }

      if ('+-*/=%&|^~<>'.includes(this.character)) {
        continue
      }

      return { _tag: 'Identifier', name: this.chars.slice(start, start + length) }
    }
  }

  private scanStringLiteral(): Token {
    const start = this.index
    let content = ''

    while (true) {
      this.readCharacter()
      if (this.character === null) return { _tag: 'Invalid' }

      if (this.character === '"') {
        this.readCharacter()
        return { _tag: 'String', value: content }
      }

      if (this.character === '\\') {
        this.readCharacter()
        if (this.character === null) return { _tag: 'Invalid' }
        if (!'"\\\'nrt0'.includes(this.character)) return { _tag: 'Invalid' }
        content += this.escapeChar(this.character)
      } else {
        content += this.character
      }
    }
  }

  private scanCharacterLiteral(): Token {
    const start = this.index
    let content = ''

    while (true) {
      this.readCharacter()
      if (this.character === null) return { _tag: 'Invalid' }

      if (this.character === "'") {
        this.readCharacter()
        return { _tag: 'Character', value: content }
      }

      if (this.character === '\\') {
        this.readCharacter()
        if (this.character === null) return { _tag: 'Invalid' }
        if (!'"\\\'nrt0'.includes(this.character)) return { _tag: 'Invalid' }
        content += this.escapeChar(this.character)
      } else {
        content += this.character
      }
    }
  }

  private scanFragmentLiteral(): Token {
    let content = ''

    while (true) {
      this.readCharacter()
      if (this.character === null) return { _tag: 'Invalid' }

      if (this.character === '`') {
        this.readCharacter()
        return { _tag: 'Fragment', value: content }
      }

      if (this.character === '\\') {
        this.readCharacter()
        if (this.character === null) return { _tag: 'Invalid' }
        if (!'`\\\'nrt0'.includes(this.character)) return { _tag: 'Invalid' }
        content += this.escapeChar(this.character)
      } else {
        content += this.character
      }
    }
  }

  private escapeChar(c: string): string {
    switch (c) {
      case 'n': return '\n'
      case 'r': return '\r'
      case 't': return '\t'
      case '0': return '\0'
      default: return c
    }
  }

  private scanNumericLiteral(): Token {
    const start = this.index - 1
    let length = 1

    this.readCharacter()
    if (this.character === null) {
      return { _tag: 'Integer', value: this.chars.slice(start, start + length) }
    }

    const c = this.character

    if (c >= '0' && c <= '9') {
      return this.scanIntegerLiteral(start, length)
    }

    switch (c) {
      case '.':
        return this.scanFraction(start, length)
      case 'E':
      case 'e':
        return this.scanExponent(start, length)
      case 'x':
        return this.scanHexLiteral(start, length)
      case 'B':
        return this.scanBooleanLiteral()
      default:
        return { _tag: 'Integer', value: this.chars.slice(start, start + length) }
    }
  }

  private scanIntegerLiteral(start: number, length: number): Token {
    while (true) {
      this.readCharacter()
      length++

      if (this.character === null) {
        return { _tag: 'Integer', value: this.chars.slice(start, start + length) }
      }

      const c = this.character

      if (c >= '0' && c <= '9') continue

      switch (c) {
        case '.':
          return this.scanFraction(start, length)
        case 'E':
        case 'e':
          return this.scanExponent(start, length)
        default:
          return { _tag: 'Integer', value: this.chars.slice(start, start + length) }
      }
    }
  }

  private scanFraction(start: number, length: number): Token {
    while (true) {
      this.readCharacter()
      length++

      if (this.character === null) {
        return { _tag: 'FloatingPoint', value: this.chars.slice(start, start + length) }
      }

      const c = this.character

      if (c >= '0' && c <= '9') continue

      if (c === 'E' || c === 'e') {
        return this.scanExponent(start, length)
      }

      return { _tag: 'FloatingPoint', value: this.chars.slice(start, start + length) }
    }
  }

  private scanExponent(start: number, length: number): Token {
    while (true) {
      this.readCharacter()
      length++

      if (this.character === null) {
        return { _tag: 'FloatingPoint', value: this.chars.slice(start, start + length) }
      }

      const c = this.character

      if (c >= '0' && c <= '9') continue

      return { _tag: 'FloatingPoint', value: this.chars.slice(start, start + length) }
    }
  }

  private scanHexLiteral(start: number, length: number): Token {
    while (true) {
      this.readCharacter()
      length++

      if (this.character === null) {
        return { _tag: 'Hex', value: this.chars.slice(start, start + length) }
      }

      const c = this.character

      if ((c >= '0' && c <= '9') || (c >= 'A' && c <= 'F') || (c >= 'a' && c <= 'f')) {
        continue
      }

      return { _tag: 'Hex', value: this.chars.slice(start, start + length) }
    }
  }

  private scanBooleanLiteral(): Token {
    this.readCharacter()
    if (this.character === null) return { _tag: 'Invalid' }

    const c = this.character
    if (c !== '0' && c !== '1') return { _tag: 'Invalid' }

    this.readCharacter()
    return { _tag: 'Boolean', value: c === '1' }
  }

  private skipWhitespace(skipLineFeed: boolean): void {
    while (true) {
      const c = this.character
      if (c === null) return

      if (c === ' ' || c === '\t' || c === '\r') {
        this.readCharacter()
        continue
      }

      if (c === '\n') {
        if (skipLineFeed) {
          this.readCharacter()
          continue
        }
        return
      }

      if (c === '\\') {
        this.readCharacter()
        const c1 = this.character
        if (c1 === null) return
        if (c1 === '\r') {
          this.readCharacter()
          this.readCharacter()
          continue
        }
        if (c1 === '\n') {
          this.readCharacter()
          continue
        }
        return
      }

      if (c === ';') {
        this.readCharacter()
        const c2 = this.character
        if (c2 === null) return
        if (c2 !== '*') {
          this.readCharacter()
          this.handleSingleLineComment()
        } else {
          this.readCharacter()
          this.handleMultiLineComment()
        }
        continue
      }

      return
    }
  }

  private handleSingleLineComment(): void {
    while (true) {
      if (this.character === null) return
      if (this.character === '\n') {
        this.readCharacter()
        return
      }
      this.readCharacter()
    }
  }

  private handleMultiLineComment(): void {
    while (true) {
      const c = this.character
      if (c === null) return

      if (c === ';') {
        this.readCharacter()
        const c2 = this.character
        if (c2 === null) return
        if (c2 === '*') {
          this.readCharacter()
          this.handleMultiLineComment() // nested
          continue
        }
        continue
      }

      if (c === '*') {
        this.readCharacter()
        const c3 = this.character
        if (c3 === null) return
        if (c3 === ';') {
          this.readCharacter()
          return
        }
        this.readCharacter()
        continue
      }

      this.readCharacter()
    }
  }

  // Parser helper methods
  parseKeyword(keyword: string): boolean {
    if (this.token._tag === 'Empty') this.advance()

    if (this.token._tag === 'Identifier' && this.token.name === keyword) {
      this.empty()
      return true
    }
    return false
  }

  parseIdentifier(keywords: Set<string>): string | null {
    if (this.token._tag === 'Empty') this.advance()

    if (this.token._tag === 'Identifier') {
      if (keywords.has(this.token.name)) return null
      const name = this.token.name
      this.empty()
      return name
    }
    return null
  }

  parseAttribute(): string | null {
    if (this.token._tag === 'Empty') this.advance()

    if (this.token._tag === 'Attribute') {
      const name = this.token.name
      this.empty()
      return name
    }
    return null
  }

  parsePunctuation(sign: string): boolean {
    if (this.token._tag === 'Empty') this.advance()

    if (this.token._tag === 'Punctuation' && this.token.sign === sign) {
      this.empty()
      return true
    }
    return false
  }

  parseColon(): boolean {
    if (this.token._tag === 'Empty') this.advance()

    if (this.token._tag === 'Colon') {
      this.empty()
      return true
    }
    if (this.token._tag === 'Empty') {
      return true
    }
    return false
  }

  isAtEnd(): boolean {
    return this.character === null
  }
}
