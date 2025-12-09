// Scaly Modeler - transforms syntax (AST) to semantic model
// Ported from retired/packages/scaly/scaly/compiler/Modeler.scaly

import * as Syntax from './syntax.js'
import * as Model from './model.js'
import { ModelError, notImplemented, invalidConstant, parserError } from './model-error.js'
import { Parser } from './parser.js'

// === Result Type ===

export type ModelResult<T> =
  | { ok: true; value: T }
  | { ok: false; error: ModelError }

function ok<T>(value: T): ModelResult<T> {
  return { ok: true, value }
}

function fail<T>(error: ModelError): ModelResult<T> {
  return { ok: false, error }
}

// === Modeler Class ===

export class Modeler {
  private file: string

  constructor(file: string = '<input>') {
    this.file = file
  }

  // === Public API ===

  modelOperation(syntax: Syntax.OperationSyntax): ModelResult<Model.Operand[]> {
    return this.handleOperation(syntax)
  }

  modelExpression(syntax: Syntax.ExpressionSyntax): ModelResult<Model.Expression> {
    return this.handleExpression(syntax)
  }

  // === Literal Handling ===

  private handleLiteral(syntax: Syntax.LiteralSyntax): ModelResult<Model.Constant> {
    const span: Model.Span = { start: syntax.start, end: syntax.end }
    const literal = syntax.literal

    switch (literal._syntax) {
      case 'BooleanLiteral':
        return ok({
          _tag: 'BooleanConstant',
          span,
          value: literal.value
        })

      case 'IntegerLiteral': {
        const value = parseInt(literal.value, 10)
        if (!Number.isFinite(value)) {
          return fail(invalidConstant(this.file, span))
        }
        return ok({
          _tag: 'IntegerConstant',
          span,
          value
        })
      }

      case 'HexLiteral': {
        const value = parseInt(literal.value, 16)
        if (!Number.isFinite(value)) {
          return fail(invalidConstant(this.file, span))
        }
        return ok({
          _tag: 'HexConstant',
          span,
          value
        })
      }

      case 'FloatingPointLiteral': {
        const value = parseFloat(literal.value)
        if (!Number.isFinite(value)) {
          return fail(invalidConstant(this.file, span))
        }
        return ok({
          _tag: 'FloatingPointConstant',
          span,
          value
        })
      }

      case 'StringLiteral':
        return ok({
          _tag: 'StringConstant',
          span,
          value: literal.value
        })

      case 'CharacterLiteral':
        return ok({
          _tag: 'CharacterConstant',
          span,
          value: literal.value
        })

      case 'FragmentLiteral':
        return ok({
          _tag: 'FragmentConstant',
          span,
          value: literal.value
        })
    }
  }

  // === Type Handling ===

  private handleType(syntax: Syntax.TypeSyntax): ModelResult<Model.Type> {
    const span: Model.Span = { start: syntax.start, end: syntax.end }

    // Build path from name
    const path: string[] = [syntax.name.name]
    if (syntax.name.extensions) {
      for (const ext of syntax.name.extensions) {
        path.push(ext.name)
      }
    }

    // Handle generics
    let generics: Model.Type[] | null = null
    if (syntax.generics) {
      generics = []
      if (syntax.generics.generics) {
        for (const generic of syntax.generics.generics) {
          const typeResult = this.handleType(generic.type)
          if (!typeResult.ok) return typeResult
          generics.push(typeResult.value)
        }
      }
    }

    // Handle lifetime
    let lifetime: Model.Lifetime = { _tag: 'Unspecified' }
    if (syntax.lifetime) {
      const lt = syntax.lifetime
      switch (lt._syntax) {
        case 'CallSyntax':
          lifetime = { _tag: 'Call', span: { start: lt.start, end: lt.end } }
          break
        case 'LocalSyntax':
          lifetime = { _tag: 'Local', span: { start: lt.start, end: lt.end } }
          break
        case 'ReferenceSyntax':
          lifetime = { _tag: 'Reference', span: { start: lt.start, end: lt.end }, location: lt.location }
          break
        case 'ThrownSyntax':
          lifetime = { _tag: 'Thrown', span: { start: lt.start, end: lt.end } }
          break
      }
    }

    return ok({
      _tag: 'Type',
      span,
      name: path,
      generics,
      lifetime
    })
  }

  // === Expression Handling ===

  private handleExpression(syntax: Syntax.ExpressionSyntax): ModelResult<Model.Expression> {
    switch (syntax._syntax) {
      case 'LiteralSyntax':
        return this.handleLiteral(syntax)

      case 'TypeSyntax':
        return this.handleType(syntax)

      case 'ObjectSyntax':
        return this.handleObject(syntax)

      case 'VectorSyntax':
        return this.handleVector(syntax)

      case 'BlockSyntax':
        return this.handleBlock(syntax)

      case 'IfSyntax':
        return this.handleIf(syntax)

      case 'MatchSyntax':
        return this.handleMatch(syntax)

      case 'ForSyntax':
        return this.handleFor(syntax)

      case 'WhileSyntax':
        return this.handleWhile(syntax)

      case 'ChooseSyntax':
        return this.handleChoose(syntax)

      case 'TrySyntax':
        return this.handleTry(syntax)

      case 'SizeOfSyntax':
        return this.handleSizeOf(syntax)

      case 'IsSyntax':
        return this.handleIs(syntax)

      case 'LambdaSyntax':
        return fail(notImplemented(this.file, 'Lambda', { start: syntax.start, end: syntax.end }))

      case 'RepeatSyntax':
        return fail(notImplemented(this.file, 'Repeat', { start: syntax.start, end: syntax.end }))
    }
  }

  // === Object/Tuple Handling ===

  private handleObject(syntax: Syntax.ObjectSyntax): ModelResult<Model.Tuple> {
    const span: Model.Span = { start: syntax.start, end: syntax.end }
    const components: Model.Component[] = []

    if (syntax.components) {
      for (const comp of syntax.components) {
        const compResult = this.handleComponent(comp)
        if (!compResult.ok) return compResult
        components.push(compResult.value)
      }
    }

    return ok({
      _tag: 'Tuple',
      span,
      components
    })
  }

  private handleComponent(syntax: Syntax.ComponentSyntax): ModelResult<Model.Component> {
    const span: Model.Span = { start: syntax.start, end: syntax.end }

    // Handle operands as the value
    const operandsResult = this.handleOperands(syntax.operands)
    if (!operandsResult.ok) return operandsResult

    // Component name comes from a single identifier operand if present
    // For now, we don't extract names from components
    const name: string | null = null

    // Handle attributes
    const attributes: Model.Attribute[] = []
    if (syntax.attributes) {
      for (const attr of syntax.attributes) {
        const attrResult = this.handleAttribute(attr)
        if (!attrResult.ok) return attrResult
        attributes.push(attrResult.value)
      }
    }

    return ok({
      span,
      name,
      value: operandsResult.value,
      attributes
    })
  }

  // === Vector/Matrix Handling ===

  private handleVector(syntax: Syntax.VectorSyntax): ModelResult<Model.Matrix> {
    const span: Model.Span = { start: syntax.start, end: syntax.end }
    const operations: Model.Operand[][] = []

    for (const elem of syntax.elements) {
      const elemResult = this.handleOperands(elem.operation)
      if (!elemResult.ok) return elemResult
      operations.push(elemResult.value)
    }

    return ok({
      _tag: 'Matrix',
      span,
      operations
    })
  }

  // === Block Handling ===

  private handleBlock(syntax: Syntax.BlockSyntax): ModelResult<Model.Block> {
    const span: Model.Span = { start: syntax.start, end: syntax.end }
    const statements: Model.Statement[] = []

    if (syntax.statements) {
      for (const stmt of syntax.statements) {
        const stmtResult = this.handleStatement(stmt)
        if (!stmtResult.ok) return stmtResult
        statements.push(stmtResult.value)
      }
    }

    return ok({
      _tag: 'Block',
      span,
      statements
    })
  }

  // === Statement Handling ===

  private handleStatement(syntax: Syntax.StatementSyntax): ModelResult<Model.Statement> {
    return this.handleCommand(syntax.command)
  }

  private handleCommand(command: Syntax.CommandSyntax): ModelResult<Model.Statement> {
    switch (command._syntax) {
      case 'OperationSyntax': {
        const operands = this.handleOperation(command)
        if (!operands.ok) return operands
        return ok({
          _tag: 'Action',
          source: operands.value,
          target: []
        })
      }

      case 'SetSyntax': {
        const source = this.handleOperands(command.source)
        if (!source.ok) return source
        const target = this.handleOperands(command.target)
        if (!target.ok) return target
        return ok({
          _tag: 'Action',
          source: source.value,
          target: target.value
        })
      }

      case 'LetSyntax': {
        const itemResult = this.handleBindingItem(command.binding)
        if (!itemResult.ok) return itemResult
        const operation = command.binding.operation
          ? this.handleOperands(command.binding.operation)
          : ok([])
        if (!operation.ok) return operation
        return ok({
          _tag: 'Binding',
          span: { start: command.start, end: command.end },
          bindingType: 'let',
          item: itemResult.value,
          operation: operation.value
        })
      }

      case 'VarSyntax': {
        const itemResult = this.handleBindingItem(command.binding)
        if (!itemResult.ok) return itemResult
        const operation = command.binding.operation
          ? this.handleOperands(command.binding.operation)
          : ok([])
        if (!operation.ok) return operation
        return ok({
          _tag: 'Binding',
          span: { start: command.start, end: command.end },
          bindingType: 'var',
          item: itemResult.value,
          operation: operation.value
        })
      }

      case 'MutableSyntax': {
        const itemResult = this.handleBindingItem(command.binding)
        if (!itemResult.ok) return itemResult
        const operation = command.binding.operation
          ? this.handleOperands(command.binding.operation)
          : ok([])
        if (!operation.ok) return operation
        return ok({
          _tag: 'Binding',
          span: { start: command.start, end: command.end },
          bindingType: 'mutable',
          item: itemResult.value,
          operation: operation.value
        })
      }

      case 'BreakSyntax':
        return this.handleBreak(command)

      case 'ContinueSyntax':
        return ok({
          _tag: 'Continue',
          span: { start: command.start, end: command.end }
        })

      case 'ReturnSyntax':
        return this.handleReturn(command)

      case 'ThrowSyntax':
        return this.handleThrow(command)
    }
  }

  private handleBreak(syntax: Syntax.BreakSyntax): ModelResult<Model.Break> {
    const span: Model.Span = { start: syntax.start, end: syntax.end }
    const result = syntax.result
      ? this.handleOperands(syntax.result)
      : ok([])
    if (!result.ok) return result
    return ok({
      _tag: 'Break',
      span,
      result: result.value
    })
  }

  private handleReturn(syntax: Syntax.ReturnSyntax): ModelResult<Model.Return> {
    const span: Model.Span = { start: syntax.start, end: syntax.end }
    const result = syntax.result
      ? this.handleOperands(syntax.result)
      : ok([])
    if (!result.ok) return result
    return ok({
      _tag: 'Return',
      span,
      result: result.value
    })
  }

  private handleThrow(syntax: Syntax.ThrowSyntax): ModelResult<Model.Throw> {
    const span: Model.Span = { start: syntax.start, end: syntax.end }
    const result = syntax.result
      ? this.handleOperands(syntax.result)
      : ok([])
    if (!result.ok) return result
    return ok({
      _tag: 'Throw',
      span,
      result: result.value
    })
  }

  // === Control Flow Handling ===

  private handleIf(syntax: Syntax.IfSyntax): ModelResult<Model.If> {
    const span: Model.Span = { start: syntax.start, end: syntax.end }

    const condition = this.handleOperands(syntax.condition)
    if (!condition.ok) return condition

    const consequentResult = this.handleCommand(syntax.consequent)
    if (!consequentResult.ok) return consequentResult

    let alternative: Model.Statement | null = null
    if (syntax.alternative) {
      const altResult = this.handleCommand(syntax.alternative.alternative)
      if (!altResult.ok) return altResult
      alternative = altResult.value
    }

    return ok({
      _tag: 'If',
      span,
      condition: condition.value,
      property: null,
      consequent: consequentResult.value,
      alternative
    })
  }

  private handleMatch(syntax: Syntax.MatchSyntax): ModelResult<Model.Match> {
    const span: Model.Span = { start: syntax.start, end: syntax.end }

    const condition = this.handleOperands(syntax.scrutinee)
    if (!condition.ok) return condition

    const branches: Model.Branch[] = []
    for (const branch of syntax.branches) {
      const cases: Model.Case[] = []
      for (const caseItem of branch.cases) {
        const caseCondition = this.handleOperands(caseItem.condition)
        if (!caseCondition.ok) return caseCondition
        cases.push({
          span: { start: caseItem.start, end: caseItem.end },
          condition: caseCondition.value
        })
      }
      const consequent = this.handleStatement(branch.consequent)
      if (!consequent.ok) return consequent
      branches.push({
        span: { start: branch.start, end: branch.end },
        cases,
        consequent: consequent.value
      })
    }

    let alternative: Model.Statement | null = null
    if (syntax.alternative) {
      const altResult = this.handleCommand(syntax.alternative.alternative)
      if (!altResult.ok) return altResult
      alternative = altResult.value
    }

    return ok({
      _tag: 'Match',
      span,
      condition: condition.value,
      branches,
      alternative
    })
  }

  private handleChoose(syntax: Syntax.ChooseSyntax): ModelResult<Model.Choose> {
    const span: Model.Span = { start: syntax.start, end: syntax.end }

    const condition = this.handleOperands(syntax.condition)
    if (!condition.ok) return condition

    const cases: Model.When[] = []
    if (syntax.cases) {
      for (const when of syntax.cases) {
        const whenResult = this.handleWhen(when)
        if (!whenResult.ok) return whenResult
        cases.push(whenResult.value)
      }
    }

    let alternative: Model.Statement | null = null
    if (syntax.alternative) {
      const altResult = this.handleCommand(syntax.alternative.alternative)
      if (!altResult.ok) return altResult
      alternative = altResult.value
    }

    return ok({
      _tag: 'Choose',
      span,
      condition: condition.value,
      cases,
      alternative
    })
  }

  private handleWhen(syntax: Syntax.WhenSyntax): ModelResult<Model.When> {
    const span: Model.Span = { start: syntax.start, end: syntax.end }

    const variant: string[] = [syntax.variant.name]
    if (syntax.variant.extensions) {
      for (const ext of syntax.variant.extensions) {
        variant.push(ext.name)
      }
    }

    const consequent = this.handleCommand(syntax.command)
    if (!consequent.ok) return consequent

    return ok({
      span,
      name: syntax.name,
      variant,
      consequent: consequent.value
    })
  }

  private handleFor(syntax: Syntax.ForSyntax): ModelResult<Model.For> {
    const span: Model.Span = { start: syntax.start, end: syntax.end }

    const expression = this.handleOperands(syntax.operation)
    if (!expression.ok) return expression

    const actionResult = this.handleAction(syntax.action)
    if (!actionResult.ok) return actionResult

    return ok({
      _tag: 'For',
      span,
      identifier: syntax.variable,
      expression: expression.value,
      action: actionResult.value
    })
  }

  private handleWhile(syntax: Syntax.WhileSyntax): ModelResult<Model.While> {
    const span: Model.Span = { start: syntax.start, end: syntax.end }

    const conditionResult = this.handleCondition(syntax.condition)
    if (!conditionResult.ok) return conditionResult

    const actionResult = this.handleAction(syntax.action)
    if (!actionResult.ok) return actionResult

    return ok({
      _tag: 'While',
      span,
      condition: conditionResult.value,
      action: actionResult.value
    })
  }

  private handleAction(syntax: Syntax.ActionSyntax): ModelResult<Model.Action> {
    switch (syntax._syntax) {
      case 'OperationSyntax': {
        const source = this.handleOperation(syntax)
        if (!source.ok) return source
        return ok({
          _tag: 'Action',
          source: source.value,
          target: []
        })
      }
      case 'SetSyntax': {
        const source = this.handleOperands(syntax.source)
        if (!source.ok) return source
        const target = this.handleOperands(syntax.target)
        if (!target.ok) return target
        return ok({
          _tag: 'Action',
          source: source.value,
          target: target.value
        })
      }
    }
  }

  private handleCondition(syntax: Syntax.ConditionSyntax): ModelResult<Model.Binding> {
    const span: Model.Span = { start: syntax.start, end: syntax.end }

    switch (syntax._syntax) {
      case 'OperationSyntax': {
        const operands = this.handleOperation(syntax)
        if (!operands.ok) return operands
        return ok({
          _tag: 'Binding',
          span,
          bindingType: '',
          item: {
            span,
            private: false,
            name: null,
            type: null,
            attributes: []
          },
          operation: operands.value
        })
      }

      case 'LetSyntax': {
        const itemResult = this.handleBindingItem(syntax.binding)
        if (!itemResult.ok) return itemResult
        const operation = syntax.binding.operation
          ? this.handleOperands(syntax.binding.operation)
          : ok([])
        if (!operation.ok) return operation
        return ok({
          _tag: 'Binding',
          span,
          bindingType: 'let',
          item: itemResult.value,
          operation: operation.value
        })
      }
    }
  }

  private handleTry(syntax: Syntax.TrySyntax): ModelResult<Model.Try> {
    const span: Model.Span = { start: syntax.start, end: syntax.end }

    const conditionResult = this.handleCondition(syntax.condition)
    if (!conditionResult.ok) return conditionResult

    const catches: Model.When[] = []
    if (syntax.cases) {
      for (const when of syntax.cases) {
        const whenResult = this.handleWhen(when)
        if (!whenResult.ok) return whenResult
        catches.push(whenResult.value)
      }
    }

    let alternative: Model.Statement | null = null
    if (syntax.dropper) {
      const altResult = this.handleCommand(syntax.dropper.alternative)
      if (!altResult.ok) return altResult
      alternative = altResult.value
    }

    return ok({
      _tag: 'Try',
      span,
      binding: conditionResult.value,
      catches,
      alternative
    })
  }

  // === Other Expressions ===

  private handleSizeOf(syntax: Syntax.SizeOfSyntax): ModelResult<Model.SizeOf> {
    const span: Model.Span = { start: syntax.start, end: syntax.end }
    const type = this.handleType(syntax.type)
    if (!type.ok) return type
    return ok({
      _tag: 'SizeOf',
      span,
      type: type.value
    })
  }

  private handleIs(syntax: Syntax.IsSyntax): ModelResult<Model.Is> {
    const span: Model.Span = { start: syntax.start, end: syntax.end }
    const name: string[] = [syntax.name.name]
    if (syntax.name.extensions) {
      for (const ext of syntax.name.extensions) {
        name.push(ext.name)
      }
    }
    return ok({
      _tag: 'Is',
      span,
      name
    })
  }

  // === Item Handling ===

  private handleBindingItem(syntax: Syntax.BindingSyntax): ModelResult<Model.Item> {
    const span: Model.Span = { start: syntax.start, end: syntax.end }

    let type: Model.Type | null = null
    if (syntax.annotation) {
      const spec = syntax.annotation.spec
      if (spec._syntax === 'TypeSyntax') {
        const typeResult = this.handleType(spec)
        if (!typeResult.ok) return typeResult
        type = typeResult.value
      } else {
        return fail(notImplemented(this.file, spec._syntax, { start: spec.start, end: spec.end }))
      }
    }

    return ok({
      span,
      private: false,
      name: syntax.name,
      type,
      attributes: []
    })
  }

  // === Attribute Handling ===

  private handleAttribute(syntax: Syntax.AttributeSyntax): ModelResult<Model.Attribute> {
    const span: Model.Span = { start: syntax.start, end: syntax.end }

    const modelResult = this.handleModel(syntax.model)
    if (!modelResult.ok) return modelResult

    return ok({
      span,
      name: syntax.name,
      model: modelResult.value
    })
  }

  private handleModel(syntax: Syntax.ModelSyntax): ModelResult<Model.Model> {
    switch (syntax._syntax) {
      case 'LiteralSyntax':
        return this.handleLiteral(syntax)
      case 'NameSyntax':
        return ok({
          _tag: 'Variable',
          name: syntax.name
        })
      case 'ObjectSyntax':
        return this.handleObject(syntax)
      case 'VectorSyntax':
        return this.handleVector(syntax)
    }
  }

  // === Operand Handling ===

  private handleOperand(syntax: Syntax.OperandSyntax): ModelResult<Model.Operand> {
    const span: Model.Span = { start: syntax.start, end: syntax.end }

    // Handle member access
    let memberAccess: string[] | null = null
    if (syntax.members) {
      memberAccess = []
      for (const member of syntax.members) {
        memberAccess.push(member.name.name)
        if (member.name.extensions) {
          for (const ext of member.name.extensions) {
            memberAccess.push(ext.name)
          }
        }
      }
    }

    const expression = this.handleExpression(syntax.expression)
    if (!expression.ok) return expression

    return ok({
      span,
      expression: expression.value,
      memberAccess
    })
  }

  private handleOperands(operands: Syntax.OperandSyntax[]): ModelResult<Model.Operand[]> {
    const result: Model.Operand[] = []
    for (const operand of operands) {
      const operandResult = this.handleOperand(operand)
      if (!operandResult.ok) return operandResult
      result.push(operandResult.value)
    }
    return ok(result)
  }

  private handleOperation(syntax: Syntax.OperationSyntax): ModelResult<Model.Operand[]> {
    if (syntax.operands) {
      return this.handleOperands(syntax.operands)
    }
    return ok([])
  }
}

// === Convenience Functions ===

export function parseAndModel(input: string, file: string = '<input>'): ModelResult<Model.Operand[]> {
  const parser = new Parser(input)
  const parseResult = parser.parseOperation()

  if (!parseResult.ok) {
    return fail(parserError(file, parseResult.error))
  }

  const modeler = new Modeler(file)
  return modeler.modelOperation(parseResult.value)
}
