// Scaly Documentation Generator
// Extracts @doc attributes and generates DocBook XML

import { Parser } from './parser.js'

export interface DocEntry {
  kind: 'define' | 'operator' | 'function'
  name: string
  doc: string
  parameters: { name: string; type: string; doc?: string }[]
  returnType: string | null
}

export function extractDocs(source: string): DocEntry[] {
  const parser = new Parser(source)
  const result = parser.parseFile()
  if (!result.ok) {
    throw new Error(`Parse error: ${JSON.stringify(result.error)}`)
  }

  const entries: DocEntry[] = []
  const declarations = result.value.declarations || []

  for (const decl of declarations) {
    const entry = extractFromDeclaration(decl)
    if (entry) {
      entries.push(entry)
    }
  }

  return entries
}

function extractFromDeclaration(decl: any): DocEntry | null {
  const symbol = decl.symbol
  if (!symbol) return null

  switch (symbol._syntax) {
    case 'DefinitionSyntax':
      return extractFromDefinition(symbol)
    case 'FunctionSyntax':
      return extractFromFunction(symbol)
    case 'OperatorSyntax':
      return extractFromOperator(symbol)
    default:
      return null
  }
}

function extractFromDefinition(def: any): DocEntry | null {
  const name = def.name
  if (!name) return null

  const doc = extractDocAttribute(def.attributes)
  if (!doc) return null

  // For definitions, parameters are in concept_.structure.parts as PropertySyntax
  const parts = def.concept_?.structure?.parts || []
  const parameters = parts.map((p: any) => {
    // Handle both PropertySyntax directly and FieldSyntax (which wraps PropertySyntax)
    const prop = p._syntax === 'FieldSyntax' ? p.property : p
    return {
      name: prop.name,
      type: prop.annotation?.type?.name?.name || 'unknown',
      doc: extractDocAttribute(prop.attributes) || undefined,
    }
  })

  return {
    kind: 'define',
    name,
    doc,
    parameters,
    returnType: null,
  }
}

function extractFromFunction(fn: any): DocEntry | null {
  const target = fn.target
  if (!target || target._syntax !== 'NamedSyntax') return null

  const name = target.name
  const routine = target.routine
  if (!routine) return null

  const doc = extractDocAttribute(routine.attributes)
  if (!doc) return null

  const parameters = routine.parameters
    ? extractParameters(routine.parameters)
    : []

  const returnType = routine.returns_?.type?.name?.name || null

  return {
    kind: 'function',
    name,
    doc,
    parameters,
    returnType,
  }
}

function extractFromOperator(op: any): DocEntry | null {
  const target = op.target
  if (!target || target._syntax !== 'NamedSyntax') return null

  const name = target.name
  const routine = target.routine
  if (!routine) return null

  const doc = extractDocAttribute(routine.attributes)
  if (!doc) return null

  const parameters = routine.parameters
    ? extractParameters(routine.parameters)
    : []

  const returnType = routine.returns_?.type?.name?.name || null

  return {
    kind: 'operator',
    name,
    doc,
    parameters,
    returnType,
  }
}

function extractDocAttribute(attributes: any[] | null): string | null {
  if (!attributes) return null

  for (const attr of attributes) {
    if (attr.name === 'doc' && attr.model?._syntax === 'LiteralSyntax') {
      const literal = attr.model.literal
      if (literal._syntax === 'StringLiteral') {
        return literal.value
      }
    }
  }

  return null
}

function extractParameters(params: any): { name: string; type: string; doc?: string }[] {
  if (!params.items) return []

  return params.items.map((item: any) => ({
    name: item.name,
    type: item.annotation?.type?.name?.name || 'unknown',
    doc: extractDocAttribute(item.attributes) || undefined,
  }))
}

export function generateDocBook(entries: DocEntry[]): string {
  const lines: string[] = []

  lines.push('<?xml version="1.0" encoding="UTF-8"?>')
  lines.push('<appendix xmlns="http://docbook.org/ns/docbook" version="5.0" xml:id="stdlib">')
  lines.push('  <title>Standard Library Reference</title>')
  lines.push('')

  // Group by kind
  const defines = entries.filter(e => e.kind === 'define')
  const operators = entries.filter(e => e.kind === 'operator')
  const functions = entries.filter(e => e.kind === 'function')

  if (defines.length > 0) {
    lines.push('  <section xml:id="stdlib-types">')
    lines.push('    <title>Types</title>')
    for (const entry of defines) {
      lines.push(generateEntryDocBook(entry, '    '))
    }
    lines.push('  </section>')
    lines.push('')
  }

  if (operators.length > 0) {
    lines.push('  <section xml:id="stdlib-operators">')
    lines.push('    <title>Operators</title>')
    for (const entry of operators) {
      lines.push(generateEntryDocBook(entry, '    '))
    }
    lines.push('  </section>')
    lines.push('')
  }

  if (functions.length > 0) {
    lines.push('  <section xml:id="stdlib-functions">')
    lines.push('    <title>Functions</title>')
    for (const entry of functions) {
      lines.push(generateEntryDocBook(entry, '    '))
    }
    lines.push('  </section>')
  }

  lines.push('</appendix>')

  return lines.join('\n')
}

function generateEntryDocBook(entry: DocEntry, indent: string): string {
  const lines: string[] = []
  const id = `stdlib-${entry.kind}-${sanitizeId(entry.name)}`

  lines.push(`${indent}<section xml:id="${id}">`)

  // Title with signature
  const signature = formatSignature(entry)
  lines.push(`${indent}  <title><code>${escapeXml(signature)}</code></title>`)

  // Description
  lines.push(`${indent}  <para>${escapeXml(entry.doc)}</para>`)

  // Parameters with documentation
  const paramsWithDoc = entry.parameters.filter(p => p.doc)
  if (paramsWithDoc.length > 0) {
    lines.push(`${indent}  <variablelist>`)
    for (const param of paramsWithDoc) {
      lines.push(`${indent}    <varlistentry>`)
      lines.push(`${indent}      <term><parameter>${escapeXml(param.name)}</parameter>: <type>${escapeXml(param.type)}</type></term>`)
      lines.push(`${indent}      <listitem><para>${escapeXml(param.doc!)}</para></listitem>`)
      lines.push(`${indent}    </varlistentry>`)
    }
    lines.push(`${indent}  </variablelist>`)
  }

  lines.push(`${indent}</section>`)

  return lines.join('\n')
}

function formatSignature(entry: DocEntry): string {
  const params = entry.parameters
    .map(p => `${p.name}: ${p.type}`)
    .join(', ')

  switch (entry.kind) {
    case 'define':
      return `define ${entry.name}(${params})`
    case 'operator':
      return `operator ${entry.name}(${params})${entry.returnType ? ` returns ${entry.returnType}` : ''}`
    case 'function':
      return `function ${entry.name}(${params})${entry.returnType ? ` returns ${entry.returnType}` : ''}`
  }
}

function sanitizeId(name: string): string {
  const replacements: Record<string, string> = {
    '+': 'plus',
    '-': 'minus',
    '*': 'times',
    '/': 'divide',
    '=': 'equals',
    '<>': 'notequals',
    '<': 'lt',
    '>': 'gt',
    '<=': 'lte',
    '>=': 'gte',
  }

  return replacements[name] || name.replace(/[^a-zA-Z0-9]/g, '_')
}

function escapeXml(text: string): string {
  return text
    .replace(/&/g, '&amp;')
    .replace(/</g, '&lt;')
    .replace(/>/g, '&gt;')
    .replace(/"/g, '&quot;')
}
