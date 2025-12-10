// Scaly Runtime - Browser-compatible version
// Embeds stdlib content instead of loading from filesystem

import * as Model from './model.js'
import { parseAndModelFile } from './modeler.js'
import { formatModelError } from './model-error.js'

// Embedded stdlib content
const STDLIB_CONTENT = `; Scaly Standard Library
; Arithmetic operators with precedence via wrapper types

; -----------------------------------------------------------------------------
; Precedence Wrapper Types
; -----------------------------------------------------------------------------

define Sum(left: int, right: int) {
    function value(this) returns int intrinsic
}

define Difference(left: int, right: int) {
    function value(this) returns int intrinsic
}

define Product(left: int, right: int) {
    function value(this) returns int intrinsic
}

define Quotient(left: int, right: int) {
    function value(this) returns int intrinsic
}

; -----------------------------------------------------------------------------
; Addition Operators
; -----------------------------------------------------------------------------

; Primitive: creates Sum wrapper
operator +(left: int, right: int) returns Sum intrinsic

; These evaluate the left wrapper first
operator +(left: Sum, right: int) returns Sum {
    Sum(left.value, right)
}

operator +(left: Difference, right: int) returns Sum {
    Sum(left.value, right)
}

operator +(left: Product, right: int) returns Sum {
    Sum(left.value, right)
}

operator +(left: Quotient, right: int) returns Sum {
    Sum(left.value, right)
}

; -----------------------------------------------------------------------------
; Subtraction Operators
; -----------------------------------------------------------------------------

; Primitive: creates Difference wrapper
operator -(left: int, right: int) returns Difference intrinsic

operator -(left: Sum, right: int) returns Difference {
    Difference(left.value, right)
}

operator -(left: Difference, right: int) returns Difference {
    Difference(left.value, right)
}

operator -(left: Product, right: int) returns Difference {
    Difference(left.value, right)
}

operator -(left: Quotient, right: int) returns Difference {
    Difference(left.value, right)
}

; -----------------------------------------------------------------------------
; Multiplication Operators
; -----------------------------------------------------------------------------

; Primitive: creates Product wrapper
operator *(left: int, right: int) returns Product intrinsic

; Higher precedence: reaches into Sum/Difference
operator *(left: Sum, right: int) returns Sum {
    Sum(left.left, left.right * right)
}

operator *(left: Difference, right: int) returns Difference {
    Difference(left.left, left.right * right)
}

operator *(left: Product, right: int) returns Product {
    Product(left.value, right)
}

operator *(left: Quotient, right: int) returns Product {
    Product(left.value, right)
}

; -----------------------------------------------------------------------------
; Division Operators
; -----------------------------------------------------------------------------

; Primitive: creates Quotient wrapper
operator /(left: int, right: int) returns Quotient intrinsic

; Higher precedence: reaches into Sum/Difference
operator /(left: Sum, right: int) returns Sum {
    Sum(left.left, left.right / right)
}

operator /(left: Difference, right: int) returns Difference {
    Difference(left.left, left.right / right)
}

operator /(left: Product, right: int) returns Quotient {
    Quotient(left.value, right)
}

operator /(left: Quotient, right: int) returns Quotient {
    Quotient(left.value, right)
}

; -----------------------------------------------------------------------------
; Integer Division and Modulo
; -----------------------------------------------------------------------------

operator div(left: int, right: int) returns int intrinsic
operator mod(left: int, right: int) returns int intrinsic

; -----------------------------------------------------------------------------
; Comparison Operators
; -----------------------------------------------------------------------------

operator =(left: int, right: int) returns bool intrinsic
operator <>(left: int, right: int) returns bool intrinsic
operator <(left: int, right: int) returns bool intrinsic
operator >(left: int, right: int) returns bool intrinsic
operator <=(left: int, right: int) returns bool intrinsic
operator >=(left: int, right: int) returns bool intrinsic

; -----------------------------------------------------------------------------
; Unary Operators
; -----------------------------------------------------------------------------

function -(right: int) returns int intrinsic
function +(right: int) returns int intrinsic

; -----------------------------------------------------------------------------
; Math Functions
; -----------------------------------------------------------------------------

function abs(x: int) returns int intrinsic
`

// === Runtime Values ===

export type Value =
  | { _tag: 'Int'; value: number }
  | { _tag: 'Bool'; value: boolean }
  | { _tag: 'Float'; value: number }
  | { _tag: 'String'; value: string }
  | { _tag: 'Char'; value: string }
  | { _tag: 'Wrapper'; type: string; fields: Map<string, Value> }
  | { _tag: 'Unit' }

// === Operator Registry ===

type OperatorSignature = {
  leftType: string
  rightType: string
  returnType: string
  intrinsic: boolean
  implementation: Model.Implementation | null
}

type FunctionSignature = {
  paramType: string
  returnType: string
  intrinsic: boolean
  implementation: Model.Implementation | null
}

// Scope for variable bindings
type Scope = Map<string, Value>

export class Runtime {
  private operators: Map<string, OperatorSignature[]> = new Map()
  private functions: Map<string, FunctionSignature[]> = new Map()
  private types: Map<string, Model.Concept> = new Map()

  // Load stdlib from embedded content
  loadStdlib(): { ok: true } | { ok: false; error: string } {
    const result = parseAndModelFile(STDLIB_CONTENT, 'stdlib.scaly')
    if (!result.ok) {
      return { ok: false, error: `Failed to parse stdlib: ${formatModelError(result.error, STDLIB_CONTENT)}` }
    }

    // Register all members
    for (const member of result.value.members) {
      if (member._tag === 'Concept') {
        this.types.set(member.name, member)
      } else if (member._tag === 'Operator') {
        this.registerOperator(member)
      } else if (member._tag === 'Function') {
        this.registerFunction(member)
      }
    }

    return { ok: true }
  }

  private registerOperator(op: Model.Operator): void {
    const name = op.name
    const signatures = this.operators.get(name) || []

    // Extract parameter types
    const leftParam = op.input[0]
    const rightParam = op.input[1]

    const leftType = leftParam?.type ? this.typeToString(leftParam.type) : 'int'
    const rightType = rightParam?.type ? this.typeToString(rightParam.type) : 'int'
    const returnType = op.returns ? this.typeToString(op.returns) : 'void'

    const intrinsic = op.implementation?._tag === 'Intrinsic'

    signatures.push({
      leftType,
      rightType,
      returnType,
      intrinsic,
      implementation: op.implementation
    })

    this.operators.set(name, signatures)
  }

  private registerFunction(fn: Model.Function): void {
    const name = fn.name
    const signatures = this.functions.get(name) || []

    // Extract parameter type (for unary functions)
    const param = fn.input[0]
    const paramType = param?.type ? this.typeToString(param.type) : 'int'
    const returnType = fn.returns ? this.typeToString(fn.returns) : 'void'

    const intrinsic = fn.implementation?._tag === 'Intrinsic'

    signatures.push({
      paramType,
      returnType,
      intrinsic,
      implementation: fn.implementation
    })

    this.functions.set(name, signatures)
  }

  private typeToString(type: Model.Type): string {
    return type.name.join('.')
  }

  // === Evaluation ===

  evaluate(operands: Model.Operand[], scope: Scope = new Map()): { ok: true; value: Value } | { ok: false; error: string } {
    if (operands.length === 0) {
      return { ok: true, value: { _tag: 'Unit' } }
    }

    const firstExpr = operands[0].expression

    // Check for prefix patterns when first operand is a Type
    if (firstExpr._tag === 'Type' && operands.length >= 2) {
      const name = firstExpr.name[0]

      // Constructor call: Type followed by Tuple (e.g., Sum(1, 2))
      if (this.types.has(name)) {
        const secondExpr = operands[1].expression
        if (secondExpr._tag === 'Tuple') {
          const result = this.constructWrapper(name, secondExpr, scope)
          if (!result.ok) return result
          if (operands.length > 2) {
            return this.evaluateWithContext(result.value, operands.slice(2), scope)
          }
          return result
        }
      }

      // Function call: function followed by Tuple (e.g., abs(5))
      if (this.functions.has(name)) {
        const secondExpr = operands[1].expression
        if (secondExpr._tag === 'Tuple') {
          // Evaluate the tuple argument
          if (secondExpr.components.length === 1 && !secondExpr.components[0].name) {
            const argResult = this.evaluate(secondExpr.components[0].value, scope)
            if (!argResult.ok) return argResult
            const collapsed = this.collapseWrapper(argResult.value)
            if (!collapsed.ok) return collapsed
            const funcResult = this.applyFunction(name, collapsed.value, scope)
            if (!funcResult.ok) return funcResult
            if (operands.length > 2) {
              return this.evaluateWithContext(funcResult.value, operands.slice(2), scope)
            }
            return funcResult
          }
        }
      }

      // Prefix unary function (e.g., -7, +3, abs 5)
      if (this.functions.has(name)) {
        // Get the argument (next operand)
        const argResult = this.evaluateOperand(operands[1], scope)
        if (!argResult.ok) return argResult
        const collapsed = this.collapseWrapper(argResult.value)
        if (!collapsed.ok) return collapsed

        const funcResult = this.applyFunction(name, collapsed.value, scope)
        if (!funcResult.ok) return funcResult

        if (operands.length > 2) {
          return this.evaluateWithContext(funcResult.value, operands.slice(2), scope)
        }
        return funcResult
      }
    }

    // Start with first operand as context
    let context = this.evaluateOperand(operands[0], scope)
    if (!context.ok) return context

    // Process remaining operands
    for (let i = 1; i < operands.length; i++) {
      const operand = operands[i]
      const expr = operand.expression

      if (expr._tag === 'Type') {
        // This is an operator or function reference
        const name = expr.name[0]

        // Check if this is a constructor call
        if (this.types.has(name) && i + 1 < operands.length) {
          const nextExpr = operands[i + 1].expression
          if (nextExpr._tag === 'Tuple') {
            const result = this.constructWrapper(name, nextExpr, scope)
            if (!result.ok) return result
            context = result
            i++ // Skip the tuple
            continue
          }
        }

        // Check if there's a next operand (binary operator)
        if (i + 1 < operands.length) {
          const rightOperand = operands[i + 1]
          const rightExpr = rightOperand.expression

          // Check if right is also a Type (another operator) - then this is unary
          if (rightExpr._tag === 'Type' && this.operators.has(rightExpr.name[0])) {
            // Apply as unary function
            const result = this.applyFunction(name, context.value, scope)
            if (!result.ok) return result
            context = result
          } else {
            // Binary operator
            const rightValue = this.evaluateOperand(rightOperand, scope)
            if (!rightValue.ok) return rightValue

            const result = this.applyOperator(name, context.value, rightValue.value, scope)
            if (!result.ok) return result
            context = result
            i++ // Skip the right operand
          }
        } else {
          // No more operands - check if this operator expects a right operand
          const contextType = this.valueType(context.value)
          if (this.hasBinarySignature(name, contextType)) {
            return { ok: false, error: `Operator '${name}' requires a right operand` }
          }
          // Apply as unary function
          const result = this.applyFunction(name, context.value, scope)
          if (!result.ok) return result
          context = result
        }
      } else {
        // Not an operator - this shouldn't happen in well-formed input
        return { ok: false, error: `Unexpected operand: ${expr._tag}` }
      }
    }

    return context
  }

  private evaluateWithContext(context: Value, operands: Model.Operand[], scope: Scope): { ok: true; value: Value } | { ok: false; error: string } {
    let current = { ok: true as const, value: context }

    for (let i = 0; i < operands.length; i++) {
      const operand = operands[i]
      const expr = operand.expression

      if (expr._tag === 'Type') {
        const name = expr.name[0]

        if (i + 1 < operands.length) {
          const rightOperand = operands[i + 1]
          const rightExpr = rightOperand.expression

          if (rightExpr._tag === 'Type' && this.operators.has(rightExpr.name[0])) {
            const result = this.applyFunction(name, current.value, scope)
            if (!result.ok) return result
            current = result
          } else {
            const rightValue = this.evaluateOperand(rightOperand, scope)
            if (!rightValue.ok) return rightValue

            const result = this.applyOperator(name, current.value, rightValue.value, scope)
            if (!result.ok) return result
            current = result
            i++
          }
        } else {
          // No more operands - check if this operator expects a right operand
          const contextType = this.valueType(current.value)
          if (this.hasBinarySignature(name, contextType)) {
            return { ok: false, error: `Operator '${name}' requires a right operand` }
          }
          const result = this.applyFunction(name, current.value, scope)
          if (!result.ok) return result
          current = result
        }
      }
    }

    return current
  }

  private constructWrapper(typeName: string, tuple: Model.Tuple, scope: Scope): { ok: true; value: Value } | { ok: false; error: string } {
    const fields = new Map<string, Value>()

    // For now, assume positional parameters map to 'left' and 'right'
    const fieldNames = ['left', 'right']

    for (let i = 0; i < tuple.components.length && i < fieldNames.length; i++) {
      const component = tuple.components[i]
      const fieldResult = this.evaluate(component.value, scope)
      if (!fieldResult.ok) return fieldResult

      // Collapse wrappers when passing as constructor args
      const collapsed = this.collapseWrapper(fieldResult.value)
      if (!collapsed.ok) return collapsed

      fields.set(fieldNames[i], collapsed.value)
    }

    return { ok: true, value: { _tag: 'Wrapper', type: typeName, fields } }
  }

  private evaluateOperand(operand: Model.Operand, scope: Scope): { ok: true; value: Value } | { ok: false; error: string } {
    let value = this.evaluateExpression(operand.expression, scope)
    if (!value.ok) return value

    // Handle member access chain (e.g., left.right)
    if (operand.memberAccess && operand.memberAccess.length > 0) {
      for (const member of operand.memberAccess) {
        if (value.value._tag !== 'Wrapper') {
          return { ok: false, error: `Cannot access member '${member}' on ${value.value._tag}` }
        }
        const field = value.value.fields.get(member)
        if (!field) {
          return { ok: false, error: `Unknown field '${member}' on ${value.value.type}` }
        }
        value = { ok: true, value: field }
      }
    }

    return value
  }

  private evaluateExpression(expr: Model.Expression, scope: Scope): { ok: true; value: Value } | { ok: false; error: string } {
    switch (expr._tag) {
      case 'IntegerConstant':
      case 'HexConstant':
        return { ok: true, value: { _tag: 'Int', value: expr.value } }

      case 'FloatingPointConstant':
        return { ok: true, value: { _tag: 'Float', value: expr.value } }

      case 'BooleanConstant':
        return { ok: true, value: { _tag: 'Bool', value: expr.value } }

      case 'StringConstant':
        return { ok: true, value: { _tag: 'String', value: expr.value } }

      case 'CharacterConstant':
        return { ok: true, value: { _tag: 'Char', value: expr.value } }

      case 'FragmentConstant':
        // Backtick-delimited strings (used for characters)
        return { ok: true, value: { _tag: 'Char', value: expr.value } }

      case 'Tuple':
        // For parenthesized expressions, evaluate and collapse
        if (expr.components.length === 1 && !expr.components[0].name) {
          const innerResult = this.evaluate(expr.components[0].value, scope)
          if (!innerResult.ok) return innerResult
          // Collapse wrappers at expression boundaries
          return this.collapseWrapper(innerResult.value)
        }
        return { ok: false, error: 'Complex tuples not yet implemented' }

      case 'Block':
        return this.evaluateBlock(expr, scope)

      case 'If': {
        // Evaluate condition
        const condResult = this.evaluate(expr.condition, scope)
        if (!condResult.ok) return condResult
        const condValue = condResult.value
        if (condValue._tag !== 'Bool') {
          return { ok: false, error: `If condition must be boolean, got ${condValue._tag}` }
        }
        // Evaluate appropriate branch
        if (condValue.value) {
          return this.evaluateStatement(expr.consequent, scope)
        } else if (expr.alternative) {
          return this.evaluateStatement(expr.alternative, scope)
        }
        return { ok: true, value: { _tag: 'Unit' } }
      }

      case 'Match': {
        // Evaluate the scrutinee
        const scrutineeResult = this.evaluate(expr.condition, scope)
        if (!scrutineeResult.ok) return scrutineeResult
        const scrutineeValue = scrutineeResult.value

        // Check each branch
        for (const branch of expr.branches) {
          for (const c of branch.cases) {
            const caseResult = this.evaluate(c.condition, scope)
            if (!caseResult.ok) return caseResult
            if (this.valuesEqual(scrutineeValue, caseResult.value)) {
              return this.evaluateStatement(branch.consequent, scope)
            }
          }
        }

        // No match - use alternative if present
        if (expr.alternative) {
          return this.evaluateStatement(expr.alternative, scope)
        }
        return { ok: true, value: { _tag: 'Unit' } }
      }

      case 'Type':
        // Variable lookup or type constructor
        const name = expr.name[0]

        // Handle boolean literals (true/false parse as Type)
        if (name === 'true' && expr.name.length === 1) {
          return { ok: true, value: { _tag: 'Bool', value: true } }
        }
        if (name === 'false' && expr.name.length === 1) {
          return { ok: true, value: { _tag: 'Bool', value: false } }
        }

        // Check scope for variable
        const scopeValue = scope.get(name)
        if (scopeValue) {
          // Handle member access: left.right parses as Type["left", "right"]
          if (expr.name.length > 1) {
            let value: Value = scopeValue
            for (let i = 1; i < expr.name.length; i++) {
              const memberName = expr.name[i]
              if (value._tag !== 'Wrapper') {
                return { ok: false, error: `Cannot access member '${memberName}' on ${value._tag}` }
              }

              // Check if this is a method call (e.g., .value on Sum/Product/etc)
              if (memberName === 'value') {
                // Intrinsic value method - collapse the wrapper
                const collapsed = this.collapseWrapper(value)
                if (!collapsed.ok) return collapsed
                value = collapsed.value
                continue
              }

              // Otherwise, field access
              const field = value.fields.get(memberName)
              if (!field) {
                return { ok: false, error: `Unknown field '${memberName}' on ${value.type}` }
              }
              value = field
            }
            return { ok: true, value }
          }
          return { ok: true, value: scopeValue }
        }

        // Check if this is a type constructor (e.g., Sum)
        if (this.types.has(name)) {
          // This will be handled as a constructor call
          return { ok: false, error: `Constructor ${name} needs arguments` }
        }

        return { ok: false, error: `Unresolved reference: ${expr.name.join('.')}` }

      default:
        return { ok: false, error: `Expression type not implemented: ${expr._tag}` }
    }
  }

  evaluateBlock(block: Model.Block, scope: Scope = new Map()): { ok: true; value: Value } | { ok: false; error: string } {
    let lastValue: Value = { _tag: 'Unit' }
    // Create a new scope for the block to hold let bindings
    const blockScope = new Map(scope)

    for (const stmt of block.statements) {
      if (stmt._tag === 'Action') {
        const result = this.evaluate(stmt.source, blockScope)
        if (!result.ok) return result
        lastValue = result.value
      } else if (stmt._tag === 'Binding') {
        const name = stmt.item.name
        if (!name) {
          return { ok: false, error: 'Binding must have a name' }
        }
        const result = this.evaluate(stmt.operation, blockScope)
        if (!result.ok) return result
        blockScope.set(name, result.value)
      } else if (stmt._tag === 'Return') {
        const result = this.evaluate(stmt.result, blockScope)
        if (!result.ok) return result
        return result
      }
    }

    return { ok: true, value: lastValue }
  }

  private evaluateStatement(stmt: Model.Statement, scope: Scope): { ok: true; value: Value } | { ok: false; error: string } {
    switch (stmt._tag) {
      case 'Action':
        return this.evaluate(stmt.source, scope)
      case 'Binding': {
        const name = stmt.item.name
        if (!name) {
          return { ok: false, error: 'Binding must have a name' }
        }
        const result = this.evaluate(stmt.operation, scope)
        if (!result.ok) return result
        scope.set(name, result.value)
        return { ok: true, value: { _tag: 'Unit' } }
      }
      case 'Return':
        return this.evaluate(stmt.result, scope)
      default:
        return { ok: false, error: `Statement type not implemented: ${stmt._tag}` }
    }
  }

  private valuesEqual(a: Value, b: Value): boolean {
    if (a._tag !== b._tag) return false
    switch (a._tag) {
      case 'Int':
      case 'Float':
      case 'Bool':
      case 'String':
      case 'Char':
        return a.value === (b as typeof a).value
      case 'Unit':
        return true
      case 'Wrapper':
        return false // Wrappers not compared by value
    }
  }

  private applyOperator(name: string, left: Value, right: Value, scope: Scope): { ok: true; value: Value } | { ok: false; error: string } {
    const signatures = this.operators.get(name)
    if (!signatures) {
      return { ok: false, error: `Unknown operator: ${name}` }
    }

    const leftType = this.valueType(left)
    const rightType = this.valueType(right)

    // Find matching signature
    const sig = signatures.find(s => s.leftType === leftType && s.rightType === rightType)
    if (!sig) {
      return { ok: false, error: `No matching operator ${name}(${leftType}, ${rightType})` }
    }

    if (sig.intrinsic) {
      return this.applyIntrinsicOperator(name, left, right, sig.returnType)
    }

    // Non-intrinsic: evaluate the implementation with left/right bound
    if (sig.implementation && sig.implementation._tag === 'Action') {
      const innerScope = new Map(scope)
      innerScope.set('left', left)
      innerScope.set('right', right)
      return this.evaluate(sig.implementation.source, innerScope)
    }

    return { ok: false, error: `No implementation for operator ${name}(${leftType}, ${rightType})` }
  }

  private applyFunction(name: string, arg: Value, scope: Scope): { ok: true; value: Value } | { ok: false; error: string } {
    const signatures = this.functions.get(name)
    if (!signatures) {
      return { ok: false, error: `Unknown function: ${name}` }
    }

    const argType = this.valueType(arg)

    // Find matching signature
    const sig = signatures.find(s => s.paramType === argType)
    if (!sig) {
      return { ok: false, error: `No matching function ${name}(${argType})` }
    }

    if (sig.intrinsic) {
      return this.applyIntrinsicFunction(name, arg, sig.returnType)
    }

    // Non-intrinsic: evaluate with argument bound
    if (sig.implementation && sig.implementation._tag === 'Action') {
      const innerScope = new Map(scope)
      // First parameter name would be needed here
      innerScope.set('arg', arg)
      return this.evaluate(sig.implementation.source, innerScope)
    }

    return { ok: false, error: `Non-intrinsic functions not yet implemented` }
  }

  private valueType(value: Value): string {
    switch (value._tag) {
      case 'Int': return 'int'
      case 'Bool': return 'bool'
      case 'Float': return 'float'
      case 'String': return 'string'
      case 'Char': return 'char'
      case 'Wrapper': return value.type
      case 'Unit': return 'void'
    }
  }

  // Check if operator has a binary signature for the given left type
  private hasBinarySignature(name: string, leftType: string): boolean {
    const signatures = this.operators.get(name)
    if (!signatures) return false
    return signatures.some(s => s.leftType === leftType)
  }

  // === Intrinsic Implementations ===

  private applyIntrinsicOperator(
    name: string,
    left: Value,
    right: Value,
    _returnType: string
  ): { ok: true; value: Value } | { ok: false; error: string } {
    // For now, only handle int operands
    if (left._tag !== 'Int' || right._tag !== 'Int') {
      return { ok: false, error: `Intrinsic ${name} only implemented for int` }
    }

    const l = left.value
    const r = right.value

    switch (name) {
      case '+':
        return { ok: true, value: this.makeWrapper('Sum', l, r) }
      case '-':
        return { ok: true, value: this.makeWrapper('Difference', l, r) }
      case '*':
        return { ok: true, value: this.makeWrapper('Product', l, r) }
      case '/':
        return { ok: true, value: this.makeWrapper('Quotient', l, r) }
      case 'div':
        return { ok: true, value: { _tag: 'Int', value: Math.trunc(l / r) } }
      case 'mod':
        return { ok: true, value: { _tag: 'Int', value: l % r } }
      case '=':
        return { ok: true, value: { _tag: 'Bool', value: l === r } }
      case '<>':
        return { ok: true, value: { _tag: 'Bool', value: l !== r } }
      case '<':
        return { ok: true, value: { _tag: 'Bool', value: l < r } }
      case '>':
        return { ok: true, value: { _tag: 'Bool', value: l > r } }
      case '<=':
        return { ok: true, value: { _tag: 'Bool', value: l <= r } }
      case '>=':
        return { ok: true, value: { _tag: 'Bool', value: l >= r } }
      default:
        return { ok: false, error: `Unknown intrinsic operator: ${name}` }
    }
  }

  private applyIntrinsicFunction(
    name: string,
    arg: Value,
    _returnType: string
  ): { ok: true; value: Value } | { ok: false; error: string } {
    if (arg._tag !== 'Int') {
      return { ok: false, error: `Intrinsic ${name} only implemented for int` }
    }

    const v = arg.value

    switch (name) {
      case '-':
        return { ok: true, value: { _tag: 'Int', value: -v } }
      case '+':
        return { ok: true, value: { _tag: 'Int', value: v } }
      case 'abs':
        return { ok: true, value: { _tag: 'Int', value: Math.abs(v) } }
      default:
        return { ok: false, error: `Unknown intrinsic function: ${name}` }
    }
  }

  private makeWrapper(type: string, left: number, right: number): Value {
    const fields = new Map<string, Value>()
    fields.set('left', { _tag: 'Int', value: left })
    fields.set('right', { _tag: 'Int', value: right })
    return { _tag: 'Wrapper', type, fields }
  }

  // Collapse a wrapper to its value (for expression boundaries like parentheses)
  collapseWrapper(value: Value): { ok: true; value: Value } | { ok: false; error: string } {
    if (value._tag !== 'Wrapper') {
      return { ok: true, value }
    }

    const left = value.fields.get('left')
    const right = value.fields.get('right')

    if (!left || !right || left._tag !== 'Int' || right._tag !== 'Int') {
      return { ok: false, error: `Invalid wrapper structure` }
    }

    switch (value.type) {
      case 'Sum':
        return { ok: true, value: { _tag: 'Int', value: left.value + right.value } }
      case 'Difference':
        return { ok: true, value: { _tag: 'Int', value: left.value - right.value } }
      case 'Product':
        return { ok: true, value: { _tag: 'Int', value: left.value * right.value } }
      case 'Quotient':
        return { ok: true, value: { _tag: 'Int', value: left.value / right.value } }
      default:
        return { ok: false, error: `Unknown wrapper type: ${value.type}` }
    }
  }

  // Format a value for display
  formatValue(value: Value): string {
    switch (value._tag) {
      case 'Int': return String(value.value)
      case 'Bool': return String(value.value)
      case 'Float': return String(value.value)
      case 'String': return JSON.stringify(value.value)
      case 'Char': return `'${value.value}'`
      case 'Wrapper': return `${value.type}(${this.formatValue(value.fields.get('left')!)}, ${this.formatValue(value.fields.get('right')!)})`
      case 'Unit': return '()'
    }
  }
}
