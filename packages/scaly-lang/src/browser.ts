// Scaly Language - Browser Entry Point
// Exports evaluate function for browser REPL

import { parseAndModelStatements, parseAndModelProgram } from './modeler.js'
import { Runtime, Value } from './runtime.browser.js'
import { formatModelError } from './model-error.js'

export type EvalResult =
  | { _tag: 'Ok'; value: unknown; code: string }
  | { _tag: 'ParseError'; message: string }
  | { _tag: 'ModelError'; message: string }
  | { _tag: 'GenerateError'; message: string }
  | { _tag: 'EvalError'; message: string; code: string }

// Global runtime instance
let runtime: Runtime | null = null

function getRuntime(): Runtime {
  if (!runtime) {
    runtime = new Runtime()
    const result = runtime.loadStdlib()
    if (!result.ok) {
      throw new Error(result.error)
    }
  }
  return runtime
}

export function evaluate(input: string): EvalResult {
  // Parse and build semantic model
  const modelResult = parseAndModelStatements(input)
  if (!modelResult.ok) {
    return { _tag: 'ModelError', message: formatModelError(modelResult.error, input) }
  }

  // Evaluate using runtime
  const rt = getRuntime()
  const evalResult = rt.evaluateBlock(modelResult.value)

  if (!evalResult.ok) {
    return { _tag: 'EvalError', message: evalResult.error, code: input }
  }

  // Collapse wrappers to get final value
  const collapsed = rt.collapseWrapper(evalResult.value)
  if (!collapsed.ok) {
    return { _tag: 'EvalError', message: collapsed.error, code: input }
  }

  // Convert to JavaScript value
  const value = valueToJs(collapsed.value)

  return { _tag: 'Ok', value, code: input }
}

function valueToJs(value: Value): unknown {
  switch (value._tag) {
    case 'Int':
    case 'Float':
      return value.value
    case 'Bool':
      return value.value
    case 'String':
    case 'Char':
      return value.value
    case 'Wrapper': {
      // Convert wrapper to JS object with _type and fields
      const obj: Record<string, unknown> = { _type: value.type }
      for (const [name, val] of value.fields) {
        obj[name] = valueToJs(val)
      }
      return obj
    }
    case 'Unit':
      return undefined
  }
}

// Evaluate a full program (declarations + statements)
export function evaluateProgram(input: string): EvalResult {
  // Parse and build semantic model
  const modelResult = parseAndModelProgram(input)
  if (!modelResult.ok) {
    return { _tag: 'ModelError', message: formatModelError(modelResult.error, input) }
  }

  // Evaluate using runtime (declarations are registered, statements evaluated)
  const rt = getRuntime()
  const evalResult = rt.evaluateProgram(modelResult.value)

  if (!evalResult.ok) {
    return { _tag: 'EvalError', message: evalResult.error, code: input }
  }

  // Collapse wrappers to get final value
  const collapsed = rt.collapseWrapper(evalResult.value)
  if (!collapsed.ok) {
    return { _tag: 'EvalError', message: collapsed.error, code: input }
  }

  // Convert to JavaScript value
  const value = valueToJs(collapsed.value)

  return { _tag: 'Ok', value, code: input }
}

// Reset runtime (useful for testing)
export function resetEvaluator(): void {
  runtime = null
}
