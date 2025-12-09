// Scaly Semantic Model
// Ported from retired/packages/scaly/scaly/compiler/Model.scaly

// === Basic Types ===

export interface Span {
  start: number
  end: number
}

// === Constants ===

export interface BooleanConstant {
  _tag: 'BooleanConstant'
  span: Span
  value: boolean
}

export interface IntegerConstant {
  _tag: 'IntegerConstant'
  span: Span
  value: number
}

export interface HexConstant {
  _tag: 'HexConstant'
  span: Span
  value: number
}

export interface FloatingPointConstant {
  _tag: 'FloatingPointConstant'
  span: Span
  value: number
}

export interface StringConstant {
  _tag: 'StringConstant'
  span: Span
  value: string
}

export interface CharacterConstant {
  _tag: 'CharacterConstant'
  span: Span
  value: string
}

export interface FragmentConstant {
  _tag: 'FragmentConstant'
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
  _tag: 'Tuple'
  span: Span
  components: Component[]
}

export interface Matrix {
  _tag: 'Matrix'
  span: Span
  operations: Operand[][]
}

export interface Block {
  _tag: 'Block'
  span: Span
  statements: Statement[]
}

// === Lifetime ===

export interface UnspecifiedLifetime {
  _tag: 'Unspecified'
}

export interface CallLifetime {
  _tag: 'Call'
  span: Span
}

export interface LocalLifetime {
  _tag: 'Local'
  span: Span
}

export interface ReferenceLifetime {
  _tag: 'Reference'
  span: Span
  location: string
}

export interface ThrownLifetime {
  _tag: 'Thrown'
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
  _tag: 'Action'
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
  _tag: 'Binding'
  span: Span
  bindingType: string
  item: Item
  operation: Operand[]
}

export interface Break {
  _tag: 'Break'
  span: Span
  result: Operand[]
}

export interface Continue {
  _tag: 'Continue'
  span: Span
}

export interface Return {
  _tag: 'Return'
  span: Span
  result: Operand[]
}

export interface Throw {
  _tag: 'Throw'
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
  _tag: 'Type'
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
  _tag: 'If'
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
  _tag: 'Match'
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
  _tag: 'Choose'
  span: Span
  condition: Operand[]
  cases: When[]
  alternative: Statement | null
}

export interface For {
  _tag: 'For'
  span: Span
  identifier: string
  expression: Operand[]
  action: Action
}

export interface While {
  _tag: 'While'
  span: Span
  condition: Binding
  action: Action
}

export interface Try {
  _tag: 'Try'
  span: Span
  binding: Binding
  catches: When[]
  alternative: Statement | null
}

// === Other Expressions ===

export interface SizeOf {
  _tag: 'SizeOf'
  span: Span
  type: Type
}

export interface Is {
  _tag: 'Is'
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
  _tag: 'Extern'
  span: Span
}

export interface InstructionImpl {
  _tag: 'Instruction'
  span: Span
}

export interface IntrinsicImpl {
  _tag: 'Intrinsic'
  span: Span
}

export type Implementation =
  | Action
  | ExternImpl
  | InstructionImpl
  | IntrinsicImpl

// === Functions and Operators ===

export interface Function {
  _tag: 'Function'
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
  _tag: 'Operator'
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
  _tag: 'Variable'
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
  _tag: 'Structure'
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
  _tag: 'Union'
  span: Span
  private: boolean
  variants: Variant[]
  members: Member[]
  symbols: Map<string, Nameable>
}

export interface Namespace {
  _tag: 'Namespace'
  span: Span
  modules: Module[]
  members: Member[]
  symbols: Map<string, Nameable>
}

export interface Global {
  _tag: 'Global'
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
  _tag: 'Concept'
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
  _tag: 'Package'
  modules: Module[]
}

export type Member =
  | PackageMember
  | Concept
  | Operator
  | Function

// === Nameable Union ===

export interface PackageNameable {
  _tag: 'Package'
  modules: Module[]
}

export interface FunctionsNameable {
  _tag: 'Functions'
  functions: Function[]
}

export interface PropertyNameable {
  _tag: 'Property'
  property: Property
}

export interface VariantNameable {
  _tag: 'Variant'
  variant: Variant
}

export type Nameable =
  | PackageNameable
  | Concept
  | Operator
  | FunctionsNameable
  | PropertyNameable
  | VariantNameable
