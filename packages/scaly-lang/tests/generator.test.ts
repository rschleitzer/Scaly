import { describe, it, expect } from 'vitest'
import { parseAndModel, generate } from '../src/index.js'

function generateJS(input: string): string {
  const modelResult = parseAndModel(input)
  if (!modelResult.ok) {
    throw new Error(modelResult.error.message)
  }
  const genResult = generate(modelResult.value)
  if (!genResult.ok) {
    throw new Error(genResult.error)
  }
  return genResult.code
}

describe('JS Code Generator', () => {
  describe('constants', () => {
    it('generates integer literals', () => {
      expect(generateJS('42')).toBe('42')
    })

    it('generates floating point literals', () => {
      expect(generateJS('3.14')).toBe('3.14')
    })

    it('generates string literals', () => {
      expect(generateJS('"hello"')).toBe('"hello"')
    })

    it('generates boolean literals', () => {
      expect(generateJS('true')).toBe('true')
      expect(generateJS('false')).toBe('false')
    })
  })

  describe('identifiers', () => {
    it('generates simple identifiers', () => {
      expect(generateJS('x')).toBe('x')
    })

    it('generates qualified names', () => {
      expect(generateJS('Math.PI')).toBe('Math.PI')
    })
  })

  describe('function calls', () => {
    it('generates function calls with arguments', () => {
      expect(generateJS('add 1 2')).toBe('add(1, 2)')
    })

    it('generates method calls', () => {
      expect(generateJS('console.log "hello"')).toBe('console.log("hello")')
    })
  })

  describe('arrays', () => {
    it('generates array literals', () => {
      expect(generateJS('[1, 2, 3]')).toBe('[1, 2, 3]')
    })
  })
})
