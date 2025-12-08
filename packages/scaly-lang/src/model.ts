// Scaly Semantic Model
// Ported from retired/packages/scaly/scaly/compiler/Model.scaly

// === Basic Types ===

export interface Span {
  start: number
  end: number
}

// === Constants ===

export interface BooleanConstant {
  _model: 'BooleanConstant'
  span: Span
  value: boolean
}

export interface IntegerConstant {
  _model: 'IntegerConstant'
  span: Span
  value: number
}

export interface HexConstant {
  _model: 'HexConstant'
  span: Span
  value: number
}

export interface FloatingPointConstant {
  _model: 'FloatingPointConstant'
  span: Span
  value: number
}

export interface StringConstant {
  _model: 'StringConstant'
  span: Span
  value: string
}

export interface CharacterConstant {
  _model: 'CharacterConstant'
  span: Span
  value: string
}

export interface FragmentConstant {
  _model: 'FragmentConstant'
  span: Span
  value: string
}

export type Constant =
  | BooleanConstant
  | IntegerConstant
  | HexConstant
  | FloatingPointConstant
  | StringConstant
  | CharacterConstant
  | FragmentConstant

// === Components and Structures ===

export interface Component {
  span: Span
  name: string | null
  value: Operand[]
  attributes: Attribute[]
}

export interface Tuple {
  _model: 'Tuple'
  span: Span
  components: Component[]
}

export interface Matrix {
  _model: 'Matrix'
  span: Span
  operations: Operand[][]
}

export interface Block {
  _model: 'Block'
  span: Span
  statements: Statement[]
}

// === Lifetime ===

export interface UnspecifiedLifetime {
  _model: 'Unspecified'
}

export interface CallLifetime {
  _model: 'Call'
  span: Span
}

export interface LocalLifetime {
  _model: 'Local'
  span: Span
}

export interface ReferenceLifetime {
  _model: 'Reference'
  span: Span
  location: string
}

export interface ThrownLifetime {
  _model: 'Thrown'
  span: Span
}

export type Lifetime =
  | UnspecifiedLifetime
  | CallLifetime
  | LocalLifetime
  | ReferenceLifetime
  | ThrownLifetime

// === Statements ===

export interface Action {
  _model: 'Action'
  source: Operand[]
  target: Operand[]
}

export interface Item {
  span: Span
  private: boolean
  name: string | null
  type: Type | null
  attributes: Attribute[]
}

export interface Binding {
  _model: 'Binding'
  span: Span
  bindingType: string
  item: Item
  operation: Operand[]
}

export interface Break {
  _model: 'Break'
  span: Span
  result: Operand[]
}

export interface Continue {
  _model: 'Continue'
  span: Span
}

export interface Return {
  _model: 'Return'
  span: Span
  result: Operand[]
}

export interface Throw {
  _model: 'Throw'
  span: Span
  result: Operand[]
}

export type Statement =
  | Action
  | Binding
  | Break
  | Continue
  | Return
  | Throw

// === Types ===

export interface Type {
  _model: 'Type'
  span: Span
  name: string[]
  generics: Type[] | null
  lifetime: Lifetime
}

// === Properties ===

export interface Property {
  span: Span
  private: boolean
  name: string
  type: Type | null
  initializer: Operand[] | null
  attributes: Attribute[]
}

// === Control Flow ===

export interface If {
  _model: 'If'
  span: Span
  condition: Operand[]
  property: Property | null
  consequent: Statement
  alternative: Statement | null
}

export interface Case {
  span: Span
  condition: Operand[]
}

export interface Branch {
  span: Span
  cases: Case[]
  consequent: Statement
}

export interface Match {
  _model: 'Match'
  span: Span
  condition: Operand[]
  branches: Branch[]
  alternative: Statement | null
}

export interface When {
  span: Span
  name: string
  variant: string[]
  consequent: Statement
}

export interface Choose {
  _model: 'Choose'
  span: Span
  condition: Operand[]
  cases: When[]
  alternative: Statement | null
}

export interface For {
  _model: 'For'
  span: Span
  identifier: string
  expression: Operand[]
  action: Action
}

export interface While {
  _model: 'While'
  span: Span
  condition: Binding
  action: Action
}

export interface Try {
  _model: 'Try'
  span: Span
  binding: Binding
  catches: When[]
  alternative: Statement | null
}

// === Other Expressions ===

export interface SizeOf {
  _model: 'SizeOf'
  span: Span
  type: Type
}

export interface Is {
  _model: 'Is'
  span: Span
  name: string[]
}

// === Expression Union ===

export type Expression =
  | Constant
  | Type
  | Tuple
  | Matrix
  | Block
  | If
  | Match
  | Choose
  | For
  | While
  | Try
  | SizeOf
  | Is

// === Operand ===

export interface Operand {
  span: Span
  expression: Expression
  memberAccess: string[] | null
}

// === Implementation ===

export interface ExternImpl {
  _model: 'Extern'
  span: Span
}

export interface InstructionImpl {
  _model: 'Instruction'
  span: Span
}

export interface IntrinsicImpl {
  _model: 'Intrinsic'
  span: Span
}

export type Implementation =
  | Action
  | ExternImpl
  | InstructionImpl
  | IntrinsicImpl

// === Functions and Operators ===

export interface Function {
  _model: 'Function'
  span: Span
  private: boolean
  pure: boolean
  name: string
  input: Item[]
  returns: Type | null
  throws: Type | null
  lifetime: Lifetime
  implementation: Implementation
}

export interface Operator {
  _model: 'Operator'
  span: Span
  private: boolean
  name: string
  input: Item[]
  returns: Type | null
  throws: Type | null
  implementation: Implementation
}

// === Attributes ===

export interface VariableModel {
  _model: 'Variable'
  name: string
}

export type Model =
  | Constant
  | VariableModel
  | Tuple
  | Matrix

export interface Attribute {
  span: Span
  name: string
  model: Model
}

// === Initializers ===

export interface Initializer {
  span: Span
  private: boolean
  input: Item[]
  implementation: Implementation
}

export interface DeInitializer {
  span: Span
  implementation: Implementation
}

// === Definitions ===

export interface Variant {
  span: Span
  name: string
  type: Type | null
  attributes: Attribute[]
}

export interface GenericParameter {
  span: Span
  name: string
  attributes: Attribute[]
}

export interface Structure {
  _model: 'Structure'
  span: Span
  private: boolean
  properties: Property[]
  modules: Module[]
  uses: Use[]
  initializers: Initializer[]
  deinitializer: DeInitializer | null
  members: Member[]
  symbols: Map<string, Nameable>
}

export interface Union {
  _model: 'Union'
  span: Span
  private: boolean
  variants: Variant[]
  members: Member[]
  symbols: Map<string, Nameable>
}

export interface Namespace {
  _model: 'Namespace'
  span: Span
  modules: Module[]
  members: Member[]
  symbols: Map<string, Nameable>
}

export interface Global {
  _model: 'Global'
  span: Span
  type: Type
  value: Operand[]
}

export type Definition =
  | IntrinsicImpl
  | Global
  | Namespace
  | Structure
  | Union
  | Type

// === Use ===

export interface Use {
  span: Span
  path: string[]
}

// === Concept ===

export interface Concept {
  _model: 'Concept'
  span: Span
  name: string
  parameters: GenericParameter[]
  attributes: Attribute[]
  definition: Definition
}

// === Module ===

export interface Module {
  private: boolean
  file: string
  name: string
  modules: Module[]
  uses: Use[]
  members: Member[]
  symbols: Map<string, Nameable>
}

// === Program ===

export interface Program {
  packages: Module[]
  module: Module
  statements: Statement[]
}

// === Member Union ===

export interface PackageMember {
  _model: 'Package'
  modules: Module[]
}

export type Member =
  | PackageMember
  | Concept
  | Operator
  | Function

// === Nameable Union ===

export interface PackageNameable {
  _model: 'Package'
  modules: Module[]
}

export interface FunctionsNameable {
  _model: 'Functions'
  functions: Function[]
}

export interface PropertyNameable {
  _model: 'Property'
  property: Property
}

export interface VariantNameable {
  _model: 'Variant'
  variant: Variant
}

export type Nameable =
  | PackageNameable
  | Concept
  | Operator
  | FunctionsNameable
  | PropertyNameable
  | VariantNameable
