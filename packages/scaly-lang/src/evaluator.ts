// Scaly Evaluator - evaluates Scaly code by generating and running JavaScript

import { parseAndModel } from './modeler.js'
import { generate } from './transpiler/js.js'
import { formatModelError } from './model-error.js'

export type EvalResult =
  | { _tag: 'Ok'; value: unknown; code: string }
  | { _tag: 'ParseError'; message: string }
  | { _tag: 'ModelError'; message: string }
  | { _tag: 'GenerateError'; message: string }
  | { _tag: 'EvalError'; message: string; code: string }

export function evaluate(input: string): EvalResult {
  // Parse and build semantic model
  const modelResult = parseAndModel(input)
  if (!modelResult.ok) {
    return { _tag: 'ModelError', message: formatModelError(modelResult.error, input) }
  }

  // Generate JavaScript
  const genResult = generate(modelResult.value)
  if (!genResult.ok) {
    return { _tag: 'GenerateError', message: genResult.error }
  }

  const code = genResult.code

  // Evaluate the generated JavaScript
  try {
    const value = eval(code)
    return { _tag: 'Ok', value, code }
  } catch (e) {
    const message = e instanceof Error ? e.message : String(e)
    return { _tag: 'EvalError', message, code }
  }
}
