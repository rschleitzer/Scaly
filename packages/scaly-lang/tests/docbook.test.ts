// DocBook documentation tests
// Extracts and runs test cases from documentation chapters

import { describe, it, expect, beforeEach } from 'vitest'
import { extractTestCases, evaluateProgram, resetEvaluator } from '../src/index.js'
import { readFileSync, readdirSync } from 'fs'
import { join } from 'path'

const chaptersDir = join(process.cwd(), 'docs/scaly/chapters')

// Find all chapter files
const chapterFiles = readdirSync(chaptersDir)
  .filter(f => f.endsWith('.xml'))

describe('Documentation Examples', () => {
  for (const file of chapterFiles) {
    const content = readFileSync(join(chaptersDir, file), 'utf-8')
    const tests = extractTestCases(content, file)

    if (tests.length > 0) {
      describe(file, () => {
        for (const test of tests) {
          const shortProgram = test.program.split('\n')[0].substring(0, 40)
          it(`${shortProgram}... => ${test.expected}`, () => {
            resetEvaluator()
            const result = evaluateProgram(test.program)
            expect(result._tag).toBe('Ok')
            if (result._tag === 'Ok') {
              expect(JSON.stringify(result.value)).toBe(test.expected)
            }
          })
        }
      })
    }
  }
})
