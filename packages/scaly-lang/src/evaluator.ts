// Scaly Evaluator - evaluates Scaly code using the runtime

import { parseAndModel } from './modeler.js'
import { Runtime, Value } from './runtime.js'
import { formatModelError } from './model-error.js'
import * as path from 'path'
import { fileURLToPath } from 'url'

const __filename = fileURLToPath(import.meta.url)
const __dirname = path.dirname(__filename)

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
    // Try to load stdlib from various locations
    const locations = [
      path.join(process.cwd(), 'stdlib.scaly'),
      path.join(process.cwd(), '../../stdlib.scaly'),
      path.join(__dirname, '../../../stdlib.scaly'),
    ]
    for (const loc of locations) {
      const result = runtime.loadStdlib(loc)
      if (result.ok) break
    }
  }
  return runtime
}

export function evaluate(input: string): EvalResult {
  // Parse and build semantic model
  const modelResult = parseAndModel(input)
  if (!modelResult.ok) {
    return { _tag: 'ModelError', message: formatModelError(modelResult.error, input) }
  }

  // Evaluate using runtime
  const rt = getRuntime()
  const evalResult = rt.evaluate(modelResult.value)

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
    case 'Wrapper':
      // This shouldn't happen after collapse, but just in case
      return `${value.type}(...)`
    case 'Unit':
      return undefined
  }
}

// Reset runtime (useful for testing)
export function resetEvaluator(): void {
  runtime = null
}
