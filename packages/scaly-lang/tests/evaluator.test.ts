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

  it('evaluates arithmetic via JS builtins', () => {
    const result = evaluate('Math.sqrt 16')
    expect(result._tag).toBe('Ok')
    if (result._tag === 'Ok') {
      expect(result.value).toBe(4)
    }
  })

  it('evaluates array literals', () => {
    const result = evaluate('[1, 2, 3]')
    expect(result._tag).toBe('Ok')
    if (result._tag === 'Ok') {
      expect(result.value).toEqual([1, 2, 3])
    }
  })
})
