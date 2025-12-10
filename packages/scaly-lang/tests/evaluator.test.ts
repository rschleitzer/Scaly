import { describe, it, expect } from 'vitest'
import { evaluate } from '../src/index.js'

describe('Evaluator', () => {
  it('evaluates integer literals', () => {
    const result = evaluate('42')
    expect(result._tag).toBe('Ok')
    if (result._tag === 'Ok') {
      expect(result.value).toBe(42)
      expect(result.code).toBe('42')
    }
  })

  it('evaluates floating point literals', () => {
    const result = evaluate('3.14')
    expect(result._tag).toBe('Ok')
    if (result._tag === 'Ok') {
      expect(result.value).toBe(3.14)
    }
  })

  it('evaluates string literals', () => {
    const result = evaluate('"hello"')
    expect(result._tag).toBe('Ok')
    if (result._tag === 'Ok') {
      expect(result.value).toBe('hello')
    }
  })

  it('evaluates boolean literals', () => {
    const result = evaluate('true')
    expect(result._tag).toBe('Ok')
    if (result._tag === 'Ok') {
      expect(result.value).toBe(true)
    }
  })

  it('evaluates arithmetic operations', () => {
    const result = evaluate('3 + 4 * 2')
    expect(result._tag).toBe('Ok')
    if (result._tag === 'Ok') {
      expect(result.value).toBe(11)  // Precedence: 3 + 8 = 11
    }
  })

  it('evaluates comparison operators', () => {
    const result = evaluate('5 > 3')
    expect(result._tag).toBe('Ok')
    if (result._tag === 'Ok') {
      expect(result.value).toBe(true)
    }
  })

  it('evaluates if expressions with true condition', () => {
    const result = evaluate('if true: 1 else 0')
    expect(result._tag).toBe('Ok')
    if (result._tag === 'Ok') {
      expect(result.value).toBe(1)
    }
  })

  it('evaluates if expressions with false condition', () => {
    const result = evaluate('if false: 1 else 0')
    expect(result._tag).toBe('Ok')
    if (result._tag === 'Ok') {
      expect(result.value).toBe(0)
    }
  })

  it('evaluates if expressions with comparison', () => {
    const result = evaluate('if 5 > 3: 10 else 20')
    expect(result._tag).toBe('Ok')
    if (result._tag === 'Ok') {
      expect(result.value).toBe(10)
    }
  })

  it('evaluates match expressions', () => {
    const result = evaluate('match 2: case 1: 10 case 2: 20 else 0')
    expect(result._tag).toBe('Ok')
    if (result._tag === 'Ok') {
      expect(result.value).toBe(20)
    }
  })

  it('evaluates match with multiple cases per branch', () => {
    const result = evaluate('match 2: case 1 case 2: "one or two" else "other"')
    expect(result._tag).toBe('Ok')
    if (result._tag === 'Ok') {
      expect(result.value).toBe('one or two')
    }
  })

  it('evaluates match else branch', () => {
    const result = evaluate('match 99: case 1: 10 else 0')
    expect(result._tag).toBe('Ok')
    if (result._tag === 'Ok') {
      expect(result.value).toBe(0)
    }
  })

  it('evaluates let bindings', () => {
    const result = evaluate('let x 5: x')
    expect(result._tag).toBe('Ok')
    if (result._tag === 'Ok') {
      expect(result.value).toBe(5)
    }
  })

  it('evaluates let bindings with arithmetic', () => {
    const result = evaluate('let x 5: x + 3')
    expect(result._tag).toBe('Ok')
    if (result._tag === 'Ok') {
      expect(result.value).toBe(8)
    }
  })

  it('evaluates multiple let bindings', () => {
    const result = evaluate('let x 5: let y 3: x + y')
    expect(result._tag).toBe('Ok')
    if (result._tag === 'Ok') {
      expect(result.value).toBe(8)
    }
  })
})
