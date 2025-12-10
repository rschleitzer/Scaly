import { describe, it, expect, beforeEach } from 'vitest'
import { evaluate, evaluateProgram, resetEvaluator } from '../src/index.js'

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

describe('Programs with define', () => {
  beforeEach(() => {
    resetEvaluator()
  })

  it('evaluates program with define and constructor', () => {
    const result = evaluateProgram(`define Point(x: int, y: int)
Point(3, 4)`)
    expect(result._tag).toBe('Ok')
    if (result._tag === 'Ok') {
      expect(result.value).toEqual({ _type: 'Point', x: 3, y: 4 })
    }
  })

  it('evaluates program with define and field access', () => {
    const result = evaluateProgram(`define Point(x: int, y: int)
Point(3, 4).x`)
    expect(result._tag).toBe('Ok')
    if (result._tag === 'Ok') {
      expect(result.value).toBe(3)
    }
  })

  it('evaluates program with define, let binding and field access', () => {
    const result = evaluateProgram(`define Point(x: int, y: int)
let p Point(3, 4): p.x + p.y`)
    expect(result._tag).toBe('Ok')
    if (result._tag === 'Ok') {
      expect(result.value).toBe(7)
    }
  })
})
