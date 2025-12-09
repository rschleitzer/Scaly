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
})
