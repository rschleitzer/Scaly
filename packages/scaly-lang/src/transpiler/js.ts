// Scaly JavaScript Code Generator
// Generates JavaScript from semantic model

import * as Model from '../model.js'

export type GenerateResult =
  | { ok: true; code: string }
  | { ok: false; error: string }

function ok(code: string): GenerateResult {
  return { ok: true, code }
}

function fail(error: string): GenerateResult {
  return { ok: false, error }
}

// === Code Generator ===

export function generate(operands: Model.Operand[]): GenerateResult {
  if (operands.length === 0) {
    return ok('')
  }

  // For now, handle single operand expressions
  if (operands.length === 1) {
    return generateOperand(operands[0])
  }

  // Multiple operands: first is function/operator, rest are arguments
  // e.g., [add, 1, 2] -> add(1, 2) or 1 + 2
  const parts: string[] = []
  for (const operand of operands) {
    const result = generateOperand(operand)
    if (!result.ok) return result
    parts.push(result.code)
  }

  // First operand is the function, rest are arguments
  const [fn, ...args] = parts
  if (args.length === 0) {
    return ok(fn)
  }
  return ok(`${fn}(${args.join(', ')})`)
}

function generateOperand(operand: Model.Operand): GenerateResult {
  const exprResult = generateExpression(operand.expression)
  if (!exprResult.ok) return exprResult

  // Handle member access chain
  if (operand.memberAccess && operand.memberAccess.length > 0) {
    return ok(`${exprResult.code}.${operand.memberAccess.join('.')}`)
  }

  return exprResult
}

function generateExpression(expr: Model.Expression): GenerateResult {
  switch (expr._tag) {
    // === Constants ===
    case 'BooleanConstant':
      return ok(String(expr.value))

    case 'IntegerConstant':
    case 'HexConstant':
    case 'FloatingPointConstant':
      return ok(String(expr.value))

    case 'StringConstant':
      return ok(JSON.stringify(expr.value))

    case 'CharacterConstant':
      return ok(JSON.stringify(expr.value))

    case 'FragmentConstant':
      return ok('`' + expr.value + '`')

    // === Type (used as identifier reference) ===
    case 'Type':
      return ok(expr.name.join('.'))

    // === Tuple (object/function call) ===
    case 'Tuple':
      return generateTuple(expr)

    // === Matrix (array) ===
    case 'Matrix':
      return generateMatrix(expr)

    // === Block ===
    case 'Block':
      return generateBlock(expr)

    // === Control Flow ===
    case 'If':
      return generateIf(expr)

    case 'Match':
      return fail('Match not yet implemented')

    case 'Choose':
      return fail('Choose not yet implemented')

    case 'For':
      return generateFor(expr)

    case 'While':
      return generateWhile(expr)

    case 'Try':
      return fail('Try not yet implemented')

    // === Other ===
    case 'SizeOf':
      return fail('SizeOf not applicable to JavaScript')

    case 'Is':
      return fail('Is not yet implemented')
  }
}

function generateTuple(tuple: Model.Tuple): GenerateResult {
  if (tuple.components.length === 0) {
    return ok('()')
  }

  // Single component without name: just the value
  if (tuple.components.length === 1 && !tuple.components[0].name) {
    return generate(tuple.components[0].value)
  }

  // Multiple components or named components: object literal
  const parts: string[] = []
  for (const comp of tuple.components) {
    const valueResult = generate(comp.value)
    if (!valueResult.ok) return valueResult

    if (comp.name) {
      parts.push(`${comp.name}: ${valueResult.code}`)
    } else {
      parts.push(valueResult.code)
    }
  }

  return ok(`{ ${parts.join(', ')} }`)
}

function generateMatrix(matrix: Model.Matrix): GenerateResult {
  const rows: string[] = []
  for (const row of matrix.operations) {
    const rowResult = generate(row)
    if (!rowResult.ok) return rowResult
    rows.push(rowResult.code)
  }
  return ok(`[${rows.join(', ')}]`)
}

function generateBlock(block: Model.Block): GenerateResult {
  const statements: string[] = []
  for (const stmt of block.statements) {
    const stmtResult = generateStatement(stmt)
    if (!stmtResult.ok) return stmtResult
    statements.push(stmtResult.code)
  }
  return ok(`{\n  ${statements.join('\n  ')}\n}`)
}

function generateStatement(stmt: Model.Statement): GenerateResult {
  switch (stmt._tag) {
    case 'Action':
      return generateAction(stmt)

    case 'Binding':
      return generateBinding(stmt)

    case 'Break':
      if (stmt.result.length > 0) {
        const result = generate(stmt.result)
        if (!result.ok) return result
        return ok(`break /* ${result.code} */`)
      }
      return ok('break')

    case 'Continue':
      return ok('continue')

    case 'Return':
      if (stmt.result.length > 0) {
        const result = generate(stmt.result)
        if (!result.ok) return result
        return ok(`return ${result.code}`)
      }
      return ok('return')

    case 'Throw':
      if (stmt.result.length > 0) {
        const result = generate(stmt.result)
        if (!result.ok) return result
        return ok(`throw ${result.code}`)
      }
      return ok('throw new Error()')
  }
}

function generateAction(action: Model.Action): GenerateResult {
  const source = generate(action.source)
  if (!source.ok) return source

  if (action.target.length === 0) {
    return ok(source.code)
  }

  const target = generate(action.target)
  if (!target.ok) return target

  return ok(`${target.code} = ${source.code}`)
}

function generateBinding(binding: Model.Binding): GenerateResult {
  const keyword = binding.bindingType === 'let' ? 'const' : 'let'
  const name = binding.item.name || '_'

  if (binding.operation.length === 0) {
    return ok(`${keyword} ${name}`)
  }

  const value = generate(binding.operation)
  if (!value.ok) return value

  return ok(`${keyword} ${name} = ${value.code}`)
}

function generateIf(ifExpr: Model.If): GenerateResult {
  const condition = generate(ifExpr.condition)
  if (!condition.ok) return condition

  const consequent = generateStatement(ifExpr.consequent)
  if (!consequent.ok) return consequent

  let code = `if (${condition.code}) ${consequent.code}`

  if (ifExpr.alternative) {
    const alternative = generateStatement(ifExpr.alternative)
    if (!alternative.ok) return alternative
    code += ` else ${alternative.code}`
  }

  return ok(code)
}

function generateFor(forExpr: Model.For): GenerateResult {
  const expr = generate(forExpr.expression)
  if (!expr.ok) return expr

  const action = generateAction(forExpr.action)
  if (!action.ok) return action

  return ok(`for (const ${forExpr.identifier} of ${expr.code}) { ${action.code} }`)
}

function generateWhile(whileExpr: Model.While): GenerateResult {
  const condition = generate(whileExpr.condition.operation)
  if (!condition.ok) return condition

  const action = generateAction(whileExpr.action)
  if (!action.ok) return action

  return ok(`while (${condition.code}) { ${action.code} }`)
}
