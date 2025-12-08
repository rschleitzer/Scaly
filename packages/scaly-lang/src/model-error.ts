// Scaly Model Errors
// Ported from retired/packages/scaly/scaly/compiler/ModelError.scaly

import type { Span } from './model.js'
import type { ParserError } from './parser.js'

// === Position Utilities ===

export interface Position {
  line: number
  column: number
}

export function calculatePosition(text: string, offset: number): Position {
  let line = 1
  let column = 1

  for (let i = 0; i < offset && i < text.length; i++) {
    if (text[i] === '\n') {
      line++
      column = 1
    } else {
      column++
    }
  }

  return { line, column }
}

export function buildHintLines(
  text: string,
  startOffset: number,
  endOffset: number,
  startPosition: Position,
  endPosition: Position
): string {
  const lines: string[] = []
  let currentLine = 1
  let lineStart = 0

  for (let i = 0; i <= text.length; i++) {
    if (i === text.length || text[i] === '\n') {
      if (currentLine >= startPosition.line && currentLine <= endPosition.line) {
        const lineContent = text.slice(lineStart, i)
        lines.push(lineContent)

        // Build indicator line
        let indicator = ''
        for (let col = 0; col < lineContent.length; col++) {
          const pos = lineStart + col
          if (pos >= startOffset && pos < endOffset) {
            indicator += '^'
          } else {
            indicator += ' '
          }
        }
        lines.push(indicator)
      }
      currentLine++
      lineStart = i + 1

      if (currentLine > endPosition.line) break
    }
  }

  return lines.join('\n')
}

// === Error Types ===

export interface NotImplementedError {
  _tag: 'NotImplemented'
  file: string
  name: string
  span: Span
}

export interface DuplicateNameError {
  _tag: 'DuplicateName'
  file: string
  span: Span
}

export interface NonFunctionSymbolExistsError {
  _tag: 'NonFunctionSymbolExists'
  file: string
  span: Span
}

export interface FunctionSymbolExistsError {
  _tag: 'FunctionSymbolExists'
  file: string
  span: Span
}

export interface DeInitializerExistsError {
  _tag: 'DeInitializerExists'
  file: string
  span: Span
}

export interface InvalidConstantError {
  _tag: 'InvalidConstant'
  file: string
  span: Span
}

export interface InvalidComponentNameError {
  _tag: 'InvalidComponentName'
  file: string
  span: Span
}

export interface ModuleRootMustBeConceptError {
  _tag: 'ModuleRootMustBeConcept'
  file: string
  span: Span
}

export type ModelBuilderError =
  | NotImplementedError
  | DuplicateNameError
  | NonFunctionSymbolExistsError
  | FunctionSymbolExistsError
  | DeInitializerExistsError
  | InvalidConstantError
  | InvalidComponentNameError
  | ModuleRootMustBeConceptError

// === Parser Error Wrapper ===

export interface ParserModelError {
  _tag: 'Parser'
  file: string
  error: ParserError
}

// === Builder Error Wrapper ===

export interface BuilderModelError {
  _tag: 'Builder'
  error: ModelBuilderError
}

// === Combined Model Error ===

export type ModelError =
  | ParserModelError
  | BuilderModelError

// === Error Formatting ===

export function formatModelError(error: ModelError, sourceText?: string): string {
  switch (error._tag) {
    case 'Parser': {
      const { file, error: parserError } = error
      if (parserError._tag === 'DifferentSyntax') {
        return `${file}: A different syntax was expected here.`
      }
      const pos = sourceText
        ? calculatePosition(sourceText, parserError.start)
        : { line: 0, column: 0 }
      let msg = `${file}:${pos.line}:${pos.column}: error: Expected ${parserError.message}.`
      if (sourceText) {
        const endPos = calculatePosition(sourceText, parserError.end)
        msg += '\n' + buildHintLines(sourceText, parserError.start, parserError.end, pos, endPos)
      }
      return msg
    }

    case 'Builder': {
      const builderError = error.error
      const pos = sourceText
        ? calculatePosition(sourceText, builderError.span.start)
        : { line: 0, column: 0 }
      const file = builderError.file
      let message: string

      switch (builderError._tag) {
        case 'NotImplemented':
          message = `The ${builderError.name} syntax cannot be processed by the modeler yet.`
          break
        case 'DuplicateName':
          message = 'This declaration already exists.'
          break
        case 'NonFunctionSymbolExists':
          message = 'This declaration already exists, but not as a function.'
          break
        case 'FunctionSymbolExists':
          message = 'This declaration already exists, but as a function.'
          break
        case 'DeInitializerExists':
          message = 'A deinitializer has already been defined.'
          break
        case 'InvalidConstant':
          message = 'This is an invalid constant.'
          break
        case 'InvalidComponentName':
          message = 'The component does not have an identifier as name.'
          break
        case 'ModuleRootMustBeConcept':
          message = 'The root definition of a module must be a concept.'
          break
      }

      let msg = `${file}:${pos.line}:${pos.column}: error: ${message}`
      if (sourceText) {
        const endPos = calculatePosition(sourceText, builderError.span.end)
        msg += '\n' + buildHintLines(sourceText, builderError.span.start, builderError.span.end, pos, endPos)
      }
      return msg
    }
  }
}

// === Helper to create errors ===

export function notImplemented(file: string, name: string, span: Span): ModelError {
  return { _tag: 'Builder', error: { _tag: 'NotImplemented', file, name, span } }
}

export function duplicateName(file: string, span: Span): ModelError {
  return { _tag: 'Builder', error: { _tag: 'DuplicateName', file, span } }
}

export function nonFunctionSymbolExists(file: string, span: Span): ModelError {
  return { _tag: 'Builder', error: { _tag: 'NonFunctionSymbolExists', file, span } }
}

export function functionSymbolExists(file: string, span: Span): ModelError {
  return { _tag: 'Builder', error: { _tag: 'FunctionSymbolExists', file, span } }
}

export function deInitializerExists(file: string, span: Span): ModelError {
  return { _tag: 'Builder', error: { _tag: 'DeInitializerExists', file, span } }
}

export function invalidConstant(file: string, span: Span): ModelError {
  return { _tag: 'Builder', error: { _tag: 'InvalidConstant', file, span } }
}

export function invalidComponentName(file: string, span: Span): ModelError {
  return { _tag: 'Builder', error: { _tag: 'InvalidComponentName', file, span } }
}

export function moduleRootMustBeConcept(file: string, span: Span): ModelError {
  return { _tag: 'Builder', error: { _tag: 'ModuleRootMustBeConcept', file, span } }
}

export function parserError(file: string, error: ParserError): ModelError {
  return { _tag: 'Parser', file, error }
}
