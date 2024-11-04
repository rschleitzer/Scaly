#include "../../scaly.h"
namespace scaly {
namespace compiler {
using namespace scaly::containers;


IdentifierLiteral::IdentifierLiteral(String name) : name(name) {}

StringLiteral::StringLiteral(String value) : value(value) {}

CharacterLiteral::CharacterLiteral(String value) : value(value) {}

FragmentLiteral::FragmentLiteral(String value) : value(value) {}

IntegerLiteral::IntegerLiteral(String value) : value(value) {}

BooleanLiteral::BooleanLiteral(bool value) : value(value) {}

FloatingPointLiteral::FloatingPointLiteral(String value) : value(value) {}

HexLiteral::HexLiteral(String value) : value(value) {}
Literal::Literal(struct StringLiteral _String) : _tag(String), _String(_String) {}

Literal::Literal(struct CharacterLiteral _Character) : _tag(Character), _Character(_Character) {}

Literal::Literal(struct FragmentLiteral _Fragment) : _tag(Fragment), _Fragment(_Fragment) {}

Literal::Literal(struct IntegerLiteral _Integer) : _tag(Integer), _Integer(_Integer) {}

Literal::Literal(struct BooleanLiteral _Boolean) : _tag(Boolean), _Boolean(_Boolean) {}

Literal::Literal(struct FloatingPointLiteral _FloatingPoint) : _tag(FloatingPoint), _FloatingPoint(_FloatingPoint) {}

Literal::Literal(struct HexLiteral _Hex) : _tag(Hex), _Hex(_Hex) {}


ThrownSyntax::ThrownSyntax(size_t start, size_t end) : start(start), end(end) {}

ReferenceSyntax::ReferenceSyntax(size_t start, size_t end, String location) : start(start), end(end), location(location) {}

LocalSyntax::LocalSyntax(size_t start, size_t end) : start(start), end(end) {}

CallSyntax::CallSyntax(size_t start, size_t end) : start(start), end(end) {}
LifetimeSyntax::LifetimeSyntax(struct CallSyntax _Call) : _tag(Call), _Call(_Call) {}

LifetimeSyntax::LifetimeSyntax(struct LocalSyntax _Local) : _tag(Local), _Local(_Local) {}

LifetimeSyntax::LifetimeSyntax(struct ReferenceSyntax _Reference) : _tag(Reference), _Reference(_Reference) {}

LifetimeSyntax::LifetimeSyntax(struct ThrownSyntax _Thrown) : _tag(Thrown), _Thrown(_Thrown) {}


ExtensionSyntax::ExtensionSyntax(size_t start, size_t end, String name) : start(start), end(end), name(name) {}

NameSyntax::NameSyntax(size_t start, size_t end, String name, Vector<ExtensionSyntax>* extensions) : start(start), end(end), name(name), extensions(extensions) {}

TypeSyntax::TypeSyntax(size_t start, size_t end, NameSyntax name, GenericArgumentsSyntax* generics, OptionalSyntax* optional, LifetimeSyntax* lifetime) : start(start), end(end), name(name), generics(generics), optional(optional), lifetime(lifetime) {}

SizeOfSyntax::SizeOfSyntax(size_t start, size_t end, TypeSyntax type) : start(start), end(end), type(type) {}

SetSyntax::SetSyntax(size_t start, size_t end, Vector<OperandSyntax> target, Vector<OperandSyntax> source) : start(start), end(end), target(target), source(source) {}

OperationSyntax::OperationSyntax(size_t start, size_t end, Vector<OperandSyntax> operands) : start(start), end(end), operands(operands) {}
ActionSyntax::ActionSyntax(struct OperationSyntax _Operation) : _tag(Operation), _Operation(_Operation) {}

ActionSyntax::ActionSyntax(struct SetSyntax _Set) : _tag(Set), _Set(_Set) {}


RepeatSyntax::RepeatSyntax(size_t start, size_t end, LabelSyntax* name, ActionSyntax action) : start(start), end(end), name(name), action(action) {}

LabelSyntax::LabelSyntax(size_t start, size_t end, String name) : start(start), end(end), name(name) {}

ForSyntax::ForSyntax(size_t start, size_t end, String variable, TypeAnnotationSyntax* annotation, Vector<OperandSyntax> operation, LabelSyntax* name, ActionSyntax action) : start(start), end(end), variable(variable), annotation(annotation), operation(operation), name(name), action(action) {}

LambdaSyntax::LambdaSyntax(size_t start, size_t end, Vector<OperandSyntax> input, ActionSyntax block) : start(start), end(end), input(input), block(block) {}

ThrowSyntax::ThrowSyntax(size_t start, size_t end, Vector<OperandSyntax>* result) : start(start), end(end), result(result) {}

ReturnSyntax::ReturnSyntax(size_t start, size_t end, Vector<OperandSyntax>* result) : start(start), end(end), result(result) {}

LoopSyntax::LoopSyntax(size_t start, size_t end, String name) : start(start), end(end), name(name) {}

BreakSyntax::BreakSyntax(size_t start, size_t end, LoopSyntax* name, Vector<OperandSyntax>* result) : start(start), end(end), name(name), result(result) {}

ContinueSyntax::ContinueSyntax(size_t start, size_t end, LoopSyntax* name) : start(start), end(end), name(name) {}

TypeAnnotationSyntax::TypeAnnotationSyntax(size_t start, size_t end, TypeSyntax type) : start(start), end(end), type(type) {}

PropertySyntax::PropertySyntax(size_t start, size_t end, String name, TypeAnnotationSyntax annotation, InitializerSyntax* initializer, Vector<AttributeSyntax>* attributes) : start(start), end(end), name(name), annotation(annotation), initializer(initializer), attributes(attributes) {}

FieldSyntax::FieldSyntax(size_t start, size_t end, PropertySyntax property) : start(start), end(end), property(property) {}
PartSyntax::PartSyntax(struct FieldSyntax _Field) : _tag(Field), _Field(_Field) {}

PartSyntax::PartSyntax(struct PropertySyntax _Property) : _tag(Property), _Property(_Property) {}


StructureSyntax::StructureSyntax(size_t start, size_t end, Vector<PartSyntax>* parts) : start(start), end(end), parts(parts) {}

ArraySyntax::ArraySyntax(size_t start, size_t end, Vector<TypeSyntax>* members) : start(start), end(end), members(members) {}
BindingSpecSyntax::BindingSpecSyntax(struct StructureSyntax _Structure) : _tag(Structure), _Structure(_Structure) {}

BindingSpecSyntax::BindingSpecSyntax(struct TypeSyntax _Type) : _tag(Type), _Type(_Type) {}

BindingSpecSyntax::BindingSpecSyntax(struct ArraySyntax _Array) : _tag(Array), _Array(_Array) {}


BindingAnnotationSyntax::BindingAnnotationSyntax(size_t start, size_t end, BindingSpecSyntax spec) : start(start), end(end), spec(spec) {}

BindingSyntax::BindingSyntax(size_t start, size_t end, String name, BindingAnnotationSyntax* annotation, Vector<OperandSyntax> operation) : start(start), end(end), name(name), annotation(annotation), operation(operation) {}

MutableSyntax::MutableSyntax(size_t start, size_t end, BindingSyntax binding) : start(start), end(end), binding(binding) {}

VarSyntax::VarSyntax(size_t start, size_t end, BindingSyntax binding) : start(start), end(end), binding(binding) {}

LetSyntax::LetSyntax(size_t start, size_t end, BindingSyntax binding) : start(start), end(end), binding(binding) {}
CommandSyntax::CommandSyntax(struct OperationSyntax _Operation) : _tag(Operation), _Operation(_Operation) {}

CommandSyntax::CommandSyntax(struct LetSyntax _Let) : _tag(Let), _Let(_Let) {}

CommandSyntax::CommandSyntax(struct VarSyntax _Var) : _tag(Var), _Var(_Var) {}

CommandSyntax::CommandSyntax(struct MutableSyntax _Mutable) : _tag(Mutable), _Mutable(_Mutable) {}

CommandSyntax::CommandSyntax(struct SetSyntax _Set) : _tag(Set), _Set(_Set) {}

CommandSyntax::CommandSyntax(struct ContinueSyntax _Continue) : _tag(Continue), _Continue(_Continue) {}

CommandSyntax::CommandSyntax(struct BreakSyntax _Break) : _tag(Break), _Break(_Break) {}

CommandSyntax::CommandSyntax(struct ReturnSyntax _Return) : _tag(Return), _Return(_Return) {}

CommandSyntax::CommandSyntax(struct ThrowSyntax _Throw) : _tag(Throw), _Throw(_Throw) {}


WhenSyntax::WhenSyntax(size_t start, size_t end, String name, NameSyntax variant, CommandSyntax command) : start(start), end(end), name(name), variant(variant), command(command) {}
ConditionSyntax::ConditionSyntax(struct OperationSyntax _Operation) : _tag(Operation), _Operation(_Operation) {}

ConditionSyntax::ConditionSyntax(struct LetSyntax _Let) : _tag(Let), _Let(_Let) {}


TrySyntax::TrySyntax(size_t start, size_t end, ConditionSyntax condition, Vector<WhenSyntax>* cases, DefaultSyntax* dropper) : start(start), end(end), condition(condition), cases(cases), dropper(dropper) {}

ChooseSyntax::ChooseSyntax(size_t start, size_t end, Vector<OperandSyntax> condition, Vector<WhenSyntax>* cases, DefaultSyntax* alternative) : start(start), end(end), condition(condition), cases(cases), alternative(alternative) {}

WhileSyntax::WhileSyntax(size_t start, size_t end, ConditionSyntax condition, LabelSyntax* name, ActionSyntax action) : start(start), end(end), condition(condition), name(name), action(action) {}

StatementSyntax::StatementSyntax(size_t start, size_t end, CommandSyntax command) : start(start), end(end), command(command) {}

DefaultSyntax::DefaultSyntax(size_t start, size_t end, CommandSyntax alternative) : start(start), end(end), alternative(alternative) {}

CaseSyntax::CaseSyntax(size_t start, size_t end, Vector<OperandSyntax> condition) : start(start), end(end), condition(condition) {}

BranchSyntax::BranchSyntax(size_t start, size_t end, Vector<CaseSyntax> cases, StatementSyntax consequent) : start(start), end(end), cases(cases), consequent(consequent) {}

MatchSyntax::MatchSyntax(size_t start, size_t end, Vector<OperandSyntax> scrutinee, Vector<BranchSyntax> branches, DefaultSyntax* alternative) : start(start), end(end), scrutinee(scrutinee), branches(branches), alternative(alternative) {}

ElseSyntax::ElseSyntax(size_t start, size_t end, CommandSyntax alternative) : start(start), end(end), alternative(alternative) {}

IfSyntax::IfSyntax(size_t start, size_t end, Vector<OperandSyntax> condition, CommandSyntax consequent, ElseSyntax* alternative) : start(start), end(end), condition(condition), consequent(consequent), alternative(alternative) {}

BlockSyntax::BlockSyntax(size_t start, size_t end, Vector<UseSyntax>* uses, Vector<StatementSyntax>* statements) : start(start), end(end), uses(uses), statements(statements) {}

ElementSyntax::ElementSyntax(size_t start, size_t end, Vector<OperandSyntax> operation, Vector<AttributeSyntax>* attributes) : start(start), end(end), operation(operation), attributes(attributes) {}

VectorSyntax::VectorSyntax(size_t start, size_t end, Vector<ElementSyntax> elements, LifetimeSyntax* lifetime) : start(start), end(end), elements(elements), lifetime(lifetime) {}

ValueSyntax::ValueSyntax(size_t start, size_t end, Vector<OperandSyntax> value, Vector<AttributeSyntax>* attributes) : start(start), end(end), value(value), attributes(attributes) {}

ComponentSyntax::ComponentSyntax(size_t start, size_t end, Vector<OperandSyntax> operands, Vector<AttributeSyntax>* attributes, ValueSyntax* value) : start(start), end(end), operands(operands), attributes(attributes), value(value) {}

ObjectSyntax::ObjectSyntax(size_t start, size_t end, Vector<ComponentSyntax>* components) : start(start), end(end), components(components) {}

LiteralSyntax::LiteralSyntax(size_t start, size_t end, Literal literal) : start(start), end(end), literal(literal) {}
ExpressionSyntax::ExpressionSyntax(struct LiteralSyntax _Literal) : _tag(Literal), _Literal(_Literal) {}

ExpressionSyntax::ExpressionSyntax(struct TypeSyntax _Type) : _tag(Type), _Type(_Type) {}

ExpressionSyntax::ExpressionSyntax(struct ObjectSyntax _Object) : _tag(Object), _Object(_Object) {}

ExpressionSyntax::ExpressionSyntax(struct VectorSyntax _Vector) : _tag(Vector), _Vector(_Vector) {}

ExpressionSyntax::ExpressionSyntax(struct BlockSyntax _Block) : _tag(Block), _Block(_Block) {}

ExpressionSyntax::ExpressionSyntax(struct IfSyntax _If) : _tag(If), _If(_If) {}

ExpressionSyntax::ExpressionSyntax(struct MatchSyntax _Match) : _tag(Match), _Match(_Match) {}

ExpressionSyntax::ExpressionSyntax(struct LambdaSyntax _Lambda) : _tag(Lambda), _Lambda(_Lambda) {}

ExpressionSyntax::ExpressionSyntax(struct ForSyntax _For) : _tag(For), _For(_For) {}

ExpressionSyntax::ExpressionSyntax(struct WhileSyntax _While) : _tag(While), _While(_While) {}

ExpressionSyntax::ExpressionSyntax(struct ChooseSyntax _Choose) : _tag(Choose), _Choose(_Choose) {}

ExpressionSyntax::ExpressionSyntax(struct TrySyntax _Try) : _tag(Try), _Try(_Try) {}

ExpressionSyntax::ExpressionSyntax(struct RepeatSyntax _Repeat) : _tag(Repeat), _Repeat(_Repeat) {}

ExpressionSyntax::ExpressionSyntax(struct SizeOfSyntax _SizeOf) : _tag(SizeOf), _SizeOf(_SizeOf) {}


MemberAccessSyntax::MemberAccessSyntax(size_t start, size_t end, NameSyntax name) : start(start), end(end), name(name) {}

OperandSyntax::OperandSyntax(size_t start, size_t end, ExpressionSyntax expression, Vector<MemberAccessSyntax>* members) : start(start), end(end), expression(expression), members(members) {}

InitializerSyntax::InitializerSyntax(size_t start, size_t end, Vector<OperandSyntax> operands) : start(start), end(end), operands(operands) {}

PackageSyntax::PackageSyntax(size_t start, size_t end, NameSyntax name) : start(start), end(end), name(name) {}

ModuleSyntax::ModuleSyntax(size_t start, size_t end, String name) : start(start), end(end), name(name) {}
ModelSyntax::ModelSyntax(struct LiteralSyntax _Literal) : _tag(Literal), _Literal(_Literal) {}

ModelSyntax::ModelSyntax(struct NameSyntax _Name) : _tag(Name), _Name(_Name) {}

ModelSyntax::ModelSyntax(struct ObjectSyntax _Object) : _tag(Object), _Object(_Object) {}

ModelSyntax::ModelSyntax(struct VectorSyntax _Vector) : _tag(Vector), _Vector(_Vector) {}


AttributeSyntax::AttributeSyntax(size_t start, size_t end, String name, ModelSyntax model) : start(start), end(end), name(name), model(model) {}

MacroSyntax::MacroSyntax(size_t start, size_t end, String name, ModelSyntax model, Vector<OperandSyntax> rule) : start(start), end(end), name(name), model(model), rule(rule) {}

ExtendSyntax::ExtendSyntax(size_t start, size_t end, TypeSyntax type) : start(start), end(end), type(type) {}

ExtendsSyntax::ExtendsSyntax(size_t start, size_t end, Vector<ExtendSyntax>* extensions) : start(start), end(end), extensions(extensions) {}

IntrinsicSyntax::IntrinsicSyntax(size_t start, size_t end) : start(start), end(end) {}

InstructionSyntax::InstructionSyntax(size_t start, size_t end) : start(start), end(end) {}

ExternSyntax::ExternSyntax(size_t start, size_t end) : start(start), end(end) {}
ImplementationSyntax::ImplementationSyntax(struct ActionSyntax _Action) : _tag(Action), _Action(_Action) {}

ImplementationSyntax::ImplementationSyntax(struct ExternSyntax _Extern) : _tag(Extern), _Extern(_Extern) {}

ImplementationSyntax::ImplementationSyntax(struct InstructionSyntax _Instruction) : _tag(Instruction), _Instruction(_Instruction) {}

ImplementationSyntax::ImplementationSyntax(struct IntrinsicSyntax _Intrinsic) : _tag(Intrinsic), _Intrinsic(_Intrinsic) {}


RoutineSyntax::RoutineSyntax(size_t start, size_t end, GenericArgumentsSyntax* generics, LifetimeSyntax* lifetime, ParameterSetSyntax* parameters, Vector<AttributeSyntax>* attributes, ReturnsSyntax* returns_, ThrowsSyntax* throws_, ImplementationSyntax implementation) : start(start), end(end), generics(generics), lifetime(lifetime), parameters(parameters), attributes(attributes), returns_(returns_), throws_(throws_), implementation(implementation) {}

NamedSyntax::NamedSyntax(size_t start, size_t end, String name, RoutineSyntax routine) : start(start), end(end), name(name), routine(routine) {}
TargetSyntax::TargetSyntax(struct NamedSyntax _Named) : _tag(Named), _Named(_Named) {}

TargetSyntax::TargetSyntax(struct RoutineSyntax _Routine) : _tag(Routine), _Routine(_Routine) {}


OperatorSyntax::OperatorSyntax(size_t start, size_t end, TargetSyntax target) : start(start), end(end), target(target) {}

ProcedureSyntax::ProcedureSyntax(size_t start, size_t end, TargetSyntax target) : start(start), end(end), target(target) {}

FunctionSyntax::FunctionSyntax(size_t start, size_t end, TargetSyntax target) : start(start), end(end), target(target) {}

DeInitSyntax::DeInitSyntax(size_t start, size_t end, ActionSyntax action) : start(start), end(end), action(action) {}

InitSyntax::InitSyntax(size_t start, size_t end, ParameterSetSyntax* parameters, ActionSyntax action) : start(start), end(end), parameters(parameters), action(action) {}
MethodSyntax::MethodSyntax(struct FunctionSyntax _Function) : _tag(Function), _Function(_Function) {}

MethodSyntax::MethodSyntax(struct ProcedureSyntax _Procedure) : _tag(Procedure), _Procedure(_Procedure) {}

MethodSyntax::MethodSyntax(struct OperatorSyntax _Operator) : _tag(Operator), _Operator(_Operator) {}


TraitSyntax::TraitSyntax(size_t start, size_t end, NameSyntax name, ExtendsSyntax* extension, Vector<AttributeSyntax>* attributes, Vector<UseSyntax>* uses, Vector<MethodSyntax>* functions) : start(start), end(end), name(name), extension(extension), attributes(attributes), uses(uses), functions(functions) {}

ImplementSyntax::ImplementSyntax(size_t start, size_t end, TypeSyntax type, Vector<AttributeSyntax>* attributes, Vector<UseSyntax>* uses, Vector<MethodSyntax>* methods) : start(start), end(end), type(type), attributes(attributes), uses(uses), methods(methods) {}

UseSyntax::UseSyntax(size_t start, size_t end, NameSyntax name) : start(start), end(end), name(name) {}

ThrowsSyntax::ThrowsSyntax(size_t start, size_t end, TypeSyntax type, Vector<AttributeSyntax>* attributes) : start(start), end(end), type(type), attributes(attributes) {}

ReturnsSyntax::ReturnsSyntax(size_t start, size_t end, TypeSyntax type, Vector<AttributeSyntax>* attributes) : start(start), end(end), type(type), attributes(attributes) {}

ItemSyntax::ItemSyntax(size_t start, size_t end, String name, TypeAnnotationSyntax* annotation, Vector<AttributeSyntax>* attributes) : start(start), end(end), name(name), annotation(annotation), attributes(attributes) {}

ParametersSyntax::ParametersSyntax(size_t start, size_t end, Vector<ItemSyntax>* items) : start(start), end(end), items(items) {}
ParameterSetSyntax::ParameterSetSyntax(struct ParametersSyntax _Parameters) : _tag(Parameters), _Parameters(_Parameters) {}

ParameterSetSyntax::ParameterSetSyntax(struct TypeSyntax _Type) : _tag(Type), _Type(_Type) {}


OptionalSyntax::OptionalSyntax(size_t start, size_t end) : start(start), end(end) {}

GenericArgumentSyntax::GenericArgumentSyntax(size_t start, size_t end, TypeSyntax type) : start(start), end(end), type(type) {}

GenericArgumentsSyntax::GenericArgumentsSyntax(size_t start, size_t end, Vector<GenericArgumentSyntax>* generics) : start(start), end(end), generics(generics) {}

DelegateSyntax::DelegateSyntax(size_t start, size_t end, ParameterSetSyntax* parameters, Vector<AttributeSyntax>* attributes, ReturnsSyntax* result, ThrowsSyntax* error) : start(start), end(end), parameters(parameters), attributes(attributes), result(result), error(error) {}

ConstantSyntax::ConstantSyntax(size_t start, size_t end, TypeSyntax type, Vector<OperandSyntax> operation) : start(start), end(end), type(type), operation(operation) {}

VariantSyntax::VariantSyntax(size_t start, size_t end, String name, Vector<AttributeSyntax>* attributes, TypeAnnotationSyntax* annotation) : start(start), end(end), name(name), attributes(attributes), annotation(annotation) {}

UnionSyntax::UnionSyntax(size_t start, size_t end, Vector<VariantSyntax> variants) : start(start), end(end), variants(variants) {}

NamespaceSyntax::NamespaceSyntax(size_t start, size_t end, Vector<UseSyntax>* uses, Vector<DeclarationSyntax>* declarations) : start(start), end(end), uses(uses), declarations(declarations) {}

BodySyntax::BodySyntax(size_t start, size_t end, Vector<UseSyntax>* uses, Vector<InitSyntax>* inits, DeInitSyntax* deInit, Vector<MemberSyntax>* members) : start(start), end(end), uses(uses), inits(inits), deInit(deInit), members(members) {}

ClassSyntax::ClassSyntax(size_t start, size_t end, StructureSyntax structure, BodySyntax* body) : start(start), end(end), structure(structure), body(body) {}
ConceptSyntax::ConceptSyntax(struct ClassSyntax _Class) : _tag(Class), _Class(_Class) {}

ConceptSyntax::ConceptSyntax(struct NamespaceSyntax _Namespace) : _tag(Namespace), _Namespace(_Namespace) {}

ConceptSyntax::ConceptSyntax(struct UnionSyntax _Union) : _tag(Union), _Union(_Union) {}

ConceptSyntax::ConceptSyntax(struct ConstantSyntax _Constant) : _tag(Constant), _Constant(_Constant) {}

ConceptSyntax::ConceptSyntax(struct DelegateSyntax _Delegate) : _tag(Delegate), _Delegate(_Delegate) {}

ConceptSyntax::ConceptSyntax(struct IntrinsicSyntax _Intrinsic) : _tag(Intrinsic), _Intrinsic(_Intrinsic) {}


GenericParameterSyntax::GenericParameterSyntax(size_t start, size_t end, String name, Vector<AttributeSyntax>* attributes) : start(start), end(end), name(name), attributes(attributes) {}

GenericParametersSyntax::GenericParametersSyntax(size_t start, size_t end, Vector<GenericParameterSyntax> parameters) : start(start), end(end), parameters(parameters) {}

DefinitionSyntax::DefinitionSyntax(size_t start, size_t end, String name, GenericParametersSyntax* parameters, Vector<AttributeSyntax>* attributes, ConceptSyntax concept_) : start(start), end(end), name(name), parameters(parameters), attributes(attributes), concept_(concept_) {}
ConstituentSyntax::ConstituentSyntax(struct DefinitionSyntax _Definition) : _tag(Definition), _Definition(_Definition) {}

ConstituentSyntax::ConstituentSyntax(struct FunctionSyntax _Function) : _tag(Function), _Function(_Function) {}

ConstituentSyntax::ConstituentSyntax(struct ProcedureSyntax _Procedure) : _tag(Procedure), _Procedure(_Procedure) {}

ConstituentSyntax::ConstituentSyntax(struct OperatorSyntax _Operator) : _tag(Operator), _Operator(_Operator) {}

ConstituentSyntax::ConstituentSyntax(struct ImplementSyntax _Implement) : _tag(Implement), _Implement(_Implement) {}

ConstituentSyntax::ConstituentSyntax(struct TraitSyntax _Trait) : _tag(Trait), _Trait(_Trait) {}

ConstituentSyntax::ConstituentSyntax(struct MacroSyntax _Macro) : _tag(Macro), _Macro(_Macro) {}

ConstituentSyntax::ConstituentSyntax(struct ModuleSyntax _Module) : _tag(Module), _Module(_Module) {}


MemberSyntax::MemberSyntax(size_t start, size_t end, ConstituentSyntax constituent) : start(start), end(end), constituent(constituent) {}
ExportSyntax::ExportSyntax(struct DefinitionSyntax _Definition) : _tag(Definition), _Definition(_Definition) {}

ExportSyntax::ExportSyntax(struct FunctionSyntax _Function) : _tag(Function), _Function(_Function) {}

ExportSyntax::ExportSyntax(struct OperatorSyntax _Operator) : _tag(Operator), _Operator(_Operator) {}

ExportSyntax::ExportSyntax(struct TraitSyntax _Trait) : _tag(Trait), _Trait(_Trait) {}

ExportSyntax::ExportSyntax(struct ModuleSyntax _Module) : _tag(Module), _Module(_Module) {}


PrivateSyntax::PrivateSyntax(size_t start, size_t end, ExportSyntax export_) : start(start), end(end), export_(export_) {}
SymbolSyntax::SymbolSyntax(struct PrivateSyntax _Private) : _tag(Private), _Private(_Private) {}

SymbolSyntax::SymbolSyntax(struct DefinitionSyntax _Definition) : _tag(Definition), _Definition(_Definition) {}

SymbolSyntax::SymbolSyntax(struct FunctionSyntax _Function) : _tag(Function), _Function(_Function) {}

SymbolSyntax::SymbolSyntax(struct OperatorSyntax _Operator) : _tag(Operator), _Operator(_Operator) {}

SymbolSyntax::SymbolSyntax(struct TraitSyntax _Trait) : _tag(Trait), _Trait(_Trait) {}

SymbolSyntax::SymbolSyntax(struct MacroSyntax _Macro) : _tag(Macro), _Macro(_Macro) {}

SymbolSyntax::SymbolSyntax(struct ModuleSyntax _Module) : _tag(Module), _Module(_Module) {}


DeclarationSyntax::DeclarationSyntax(size_t start, size_t end, SymbolSyntax symbol) : start(start), end(end), symbol(symbol) {}

FileSyntax::FileSyntax(size_t start, size_t end, Vector<PackageSyntax>* packages, Vector<UseSyntax>* uses, Vector<DeclarationSyntax>* declarations) : start(start), end(end), packages(packages), uses(uses), declarations(declarations) {}

ProgramSyntax::ProgramSyntax(size_t start, size_t end, FileSyntax file, Vector<StatementSyntax>* statements) : start(start), end(end), file(file), statements(statements) {}

Parser::Parser(Lexer lexer, Vector<String> keywords_index, HashSet<String> keywords) : lexer(lexer), keywords_index(keywords_index), keywords(keywords) {}

Parser::Parser(String text) : lexer(Lexer(text)), keywords_index(initialize_keywords_index()), keywords(initialize_keywords()){
}

Vector<String> Parser::initialize_keywords_index() {
    auto r = Region();
    auto keywords_builder = new (alignof(Array<String>), r.get_page()) Array<String>();
    auto p = Page::get(this);
    (*keywords_builder).add(String(p, "break"));
    (*keywords_builder).add(String(p, "case"));
    (*keywords_builder).add(String(p, "choose"));
    (*keywords_builder).add(String(p, "continue"));
    (*keywords_builder).add(String(p, "define"));
    (*keywords_builder).add(String(p, "default"));
    (*keywords_builder).add(String(p, "deinit"));
    (*keywords_builder).add(String(p, "delegate"));
    (*keywords_builder).add(String(p, "else"));
    (*keywords_builder).add(String(p, "extends"));
    (*keywords_builder).add(String(p, "extern"));
    (*keywords_builder).add(String(p, "for"));
    (*keywords_builder).add(String(p, "function"));
    (*keywords_builder).add(String(p, "if"));
    (*keywords_builder).add(String(p, "init"));
    (*keywords_builder).add(String(p, "implement"));
    (*keywords_builder).add(String(p, "in"));
    (*keywords_builder).add(String(p, "instruction"));
    (*keywords_builder).add(String(p, "intrinsic"));
    (*keywords_builder).add(String(p, "label"));
    (*keywords_builder).add(String(p, "lambda"));
    (*keywords_builder).add(String(p, "let"));
    (*keywords_builder).add(String(p, "loop"));
    (*keywords_builder).add(String(p, "macro"));
    (*keywords_builder).add(String(p, "match"));
    (*keywords_builder).add(String(p, "module"));
    (*keywords_builder).add(String(p, "mutable"));
    (*keywords_builder).add(String(p, "operator"));
    (*keywords_builder).add(String(p, "procedure"));
    (*keywords_builder).add(String(p, "private"));
    (*keywords_builder).add(String(p, "return"));
    (*keywords_builder).add(String(p, "returns"));
    (*keywords_builder).add(String(p, "repeat"));
    (*keywords_builder).add(String(p, "set"));
    (*keywords_builder).add(String(p, "sizeof"));
    (*keywords_builder).add(String(p, "throw"));
    (*keywords_builder).add(String(p, "throws"));
    (*keywords_builder).add(String(p, "trait"));
    (*keywords_builder).add(String(p, "try"));
    (*keywords_builder).add(String(p, "union"));
    (*keywords_builder).add(String(p, "use"));
    (*keywords_builder).add(String(p, "var"));
    (*keywords_builder).add(String(p, "when"));
    (*keywords_builder).add(String(p, "while"));
    (*keywords_builder).add(String(p, "package"));
    return Vector<String>(p, *keywords_builder);
}

HashSet<String> Parser::initialize_keywords() {
    auto r = Region();
    auto hash_set_builder = new (alignof(HashSetBuilder<String>), r.get_page()) HashSetBuilder<String>(keywords_index);
    return HashSet<String>(Page::get(this), *hash_set_builder);
}

Result<Literal, ParserError> Parser::parse_literal_token(Page* rp, Page* ep) {
    {
        auto _result = lexer.token;
        switch (_result._tag)
        {
            case Token::Empty:
            {
                auto empty = _result._Empty;
                lexer.advance();
                break;
            }
            default:
                {
            };
        }
    };
    {
        auto _result = lexer.token;
        switch (_result._tag)
        {
            case Token::Literal:
            {
                auto literal = _result._Literal;
                {
                    {
                        auto _result = literal;
                        switch (_result._tag)
                        {
                            case LiteralToken::String:
                            {
                                auto string = _result._String;
                                {
                                    const auto ret = Literal(StringLiteral(String(rp, string.value)));
                                    lexer.empty();
                                    return Result<Literal, ParserError>(ret);
                                };
                                break;
                            }
                            case LiteralToken::Character:
                            {
                                auto character = _result._Character;
                                {
                                    const auto ret = Literal(CharacterLiteral(String(rp, character.value)));
                                    lexer.empty();
                                    return Result<Literal, ParserError>(ret);
                                };
                                break;
                            }
                            case LiteralToken::Integer:
                            {
                                auto integer = _result._Integer;
                                {
                                    const auto ret = Literal(IntegerLiteral(String(rp, integer.value)));
                                    lexer.empty();
                                    return Result<Literal, ParserError>(ret);
                                };
                                break;
                            }
                            case LiteralToken::FloatingPoint:
                            {
                                auto fp = _result._FloatingPoint;
                                {
                                    const auto ret = Literal(FloatingPointLiteral(String(rp, fp.value)));
                                    lexer.empty();
                                    return Result<Literal, ParserError>(ret);
                                };
                                break;
                            }
                            case LiteralToken::Hex:
                            {
                                auto hex = _result._Hex;
                                {
                                    const auto ret = Literal(HexLiteral(String(rp, hex.value)));
                                    lexer.empty();
                                    return Result<Literal, ParserError>(ret);
                                };
                                break;
                            }
                            case LiteralToken::Boolean:
                            {
                                auto boolean = _result._Boolean;
                                {
                                    const auto ret = Literal(BooleanLiteral(boolean.value));
                                    lexer.empty();
                                    return Result<Literal, ParserError>(ret);
                                };
                                break;
                            }
                            case LiteralToken::Fragment:
                            {
                                auto fragment = _result._Fragment;
                                {
                                    const auto ret = Literal(FragmentLiteral(String(rp, fragment.value)));
                                    lexer.empty();
                                    return Result<Literal, ParserError>(ret);
                                };
                                break;
                            }
                        }
                    };
                };
                break;
            }
            default:
                return Result<Literal, ParserError>(Result<Literal, ParserError>(ParserError(DifferentSyntax())));
        }
    };
}

Result<ProgramSyntax, ParserError> Parser::parse_program(Page* rp, Page* ep) {
    const auto start = lexer.previous_position;
    const auto file_start = lexer.position;
    const auto _file_result = parse_file(rp, ep);
    auto file = _file_result._Ok;
    if (_file_result._tag == Success::Error) {
        const auto _file_Error = _file_result._Error;
        switch (_file_Error._tag) {
            case ParserError::Different: {
                const auto d = _file_Error._Different;
                return Result<ProgramSyntax, ParserError>(d);
                break;
            }
            case ParserError::Invalid: {
                const auto i = _file_Error._Invalid;
                return Result<ProgramSyntax, ParserError>(i);
                break;
            }
        }
    }
    ;
    const auto statements_start = lexer.position;
    Vector<StatementSyntax>* statements = nullptr;
    {
        auto _result = parse_statement_list(rp, ep);
        switch (_result._tag)
        {
            case Success::Error:
            {
                auto error = _result._Error;
                {
                    {
                        auto _result = error;
                        switch (_result._tag)
                        {
                            case ParserError::Different:
                            {
                                auto d = _result._Different;
                                {
                                };
                                break;
                            }
                            case ParserError::Invalid:
                            {
                                auto i = _result._Invalid;
                                return Result<ProgramSyntax, ParserError>(i);
                                break;
                            }
                        }
                    };
                };
                break;
            }
            case Success::Ok:
            {
                auto success = _result._Ok;
                statements = success;
                break;
            }
        }
    };
    const auto end = lexer.position;
    return Result<ProgramSyntax, ParserError>(ProgramSyntax(start, end, file, statements));
}

Result<FileSyntax, ParserError> Parser::parse_file(Page* rp, Page* ep) {
    const auto start = lexer.previous_position;
    const auto packages_start = lexer.position;
    Vector<PackageSyntax>* packages = nullptr;
    {
        auto _result = parse_package_list(rp, ep);
        switch (_result._tag)
        {
            case Success::Error:
            {
                auto error = _result._Error;
                {
                    {
                        auto _result = error;
                        switch (_result._tag)
                        {
                            case ParserError::Different:
                            {
                                auto d = _result._Different;
                                {
                                };
                                break;
                            }
                            case ParserError::Invalid:
                            {
                                auto i = _result._Invalid;
                                return Result<FileSyntax, ParserError>(i);
                                break;
                            }
                        }
                    };
                };
                break;
            }
            case Success::Ok:
            {
                auto success = _result._Ok;
                packages = success;
                break;
            }
        }
    };
    const auto uses_start = lexer.position;
    Vector<UseSyntax>* uses = nullptr;
    {
        auto _result = parse_use_list(rp, ep);
        switch (_result._tag)
        {
            case Success::Error:
            {
                auto error = _result._Error;
                {
                    {
                        auto _result = error;
                        switch (_result._tag)
                        {
                            case ParserError::Different:
                            {
                                auto d = _result._Different;
                                {
                                };
                                break;
                            }
                            case ParserError::Invalid:
                            {
                                auto i = _result._Invalid;
                                return Result<FileSyntax, ParserError>(i);
                                break;
                            }
                        }
                    };
                };
                break;
            }
            case Success::Ok:
            {
                auto success = _result._Ok;
                uses = success;
                break;
            }
        }
    };
    const auto declarations_start = lexer.position;
    Vector<DeclarationSyntax>* declarations = nullptr;
    {
        auto _result = parse_declaration_list(rp, ep);
        switch (_result._tag)
        {
            case Success::Error:
            {
                auto error = _result._Error;
                {
                    {
                        auto _result = error;
                        switch (_result._tag)
                        {
                            case ParserError::Different:
                            {
                                auto d = _result._Different;
                                {
                                };
                                break;
                            }
                            case ParserError::Invalid:
                            {
                                auto i = _result._Invalid;
                                return Result<FileSyntax, ParserError>(i);
                                break;
                            }
                        }
                    };
                };
                break;
            }
            case Success::Ok:
            {
                auto success = _result._Ok;
                declarations = success;
                break;
            }
        }
    };
    const auto end = lexer.position;
    return Result<FileSyntax, ParserError>(FileSyntax(start, end, packages, uses, declarations));
}

Result<Vector<DeclarationSyntax>*, ParserError> Parser::parse_declaration_list(Page* rp, Page* ep) {
    auto r = Region();
    auto list = new (alignof(List<DeclarationSyntax>), r.get_page()) List<DeclarationSyntax>();
    while (true) {
        const auto _node_result = parse_declaration(rp, ep);
        auto node = _node_result._Ok;
        if (_node_result._tag == Success::Error) {
            const auto _node_Error = _node_result._Error;
            switch (_node_Error._tag) {
                case ParserError::Invalid: {
                    const auto i = _node_Error._Invalid;
                    return Result<Vector<DeclarationSyntax>*, ParserError>(i);
                    break;
                }
                case ParserError::Different: {
                    const auto d = _node_Error._Different;
                    {
                        if ((*list).head == 0) {
                            return Result<Vector<DeclarationSyntax>*, ParserError>(d);
                        }
                        else {
                            return Result<Vector<DeclarationSyntax>*, ParserError>(new (alignof(Vector<DeclarationSyntax>), rp) Vector<DeclarationSyntax>(rp, *list));
                        };
                    };
                    break;
                }
            }
        }
        (*list).add(node);
    };
}

Result<DeclarationSyntax, ParserError> Parser::parse_declaration(Page* rp, Page* ep) {
    const auto start = lexer.previous_position;
    const auto symbol_start = lexer.position;
    const auto _symbol_result = parse_symbol(rp, ep);
    auto symbol = _symbol_result._Ok;
    if (_symbol_result._tag == Success::Error) {
        const auto _symbol_Error = _symbol_result._Error;
        switch (_symbol_Error._tag) {
            case ParserError::Different: {
                const auto d = _symbol_Error._Different;
                return Result<DeclarationSyntax, ParserError>(d);
                break;
            }
            case ParserError::Invalid: {
                const auto i = _symbol_Error._Invalid;
                return Result<DeclarationSyntax, ParserError>(i);
                break;
            }
        }
    }
    ;
    const auto start_colon_2 = lexer.previous_position;
    const auto success_colon_2 = lexer.parse_colon();
    const auto end = lexer.position;
    return Result<DeclarationSyntax, ParserError>(DeclarationSyntax(start, end, symbol));
}

Result<SymbolSyntax, ParserError> Parser::parse_symbol(Page* rp, Page* ep) {
    {
        {
            auto _result = parse_private(rp, ep);
            switch (_result._tag)
            {
                case Success::Error:
                {
                    auto error = _result._Error;
                    {
                        {
                            auto _result = error;
                            switch (_result._tag)
                            {
                                case ParserError::Invalid:
                                {
                                    auto i = _result._Invalid;
                                    return Result<SymbolSyntax, ParserError>(i);
                                    break;
                                }
                                case ParserError::Different:
                                {
                                    auto d = _result._Different;
                                    {
                                    };
                                    break;
                                }
                            }
                        };
                    };
                    break;
                }
                case Success::Ok:
                {
                    auto node = _result._Ok;
                    return Result<SymbolSyntax, ParserError>(SymbolSyntax(PrivateSyntax(node)));
                    break;
                }
            }
        };
    };
    {
        {
            auto _result = parse_definition(rp, ep);
            switch (_result._tag)
            {
                case Success::Error:
                {
                    auto error = _result._Error;
                    {
                        {
                            auto _result = error;
                            switch (_result._tag)
                            {
                                case ParserError::Invalid:
                                {
                                    auto i = _result._Invalid;
                                    return Result<SymbolSyntax, ParserError>(i);
                                    break;
                                }
                                case ParserError::Different:
                                {
                                    auto d = _result._Different;
                                    {
                                    };
                                    break;
                                }
                            }
                        };
                    };
                    break;
                }
                case Success::Ok:
                {
                    auto node = _result._Ok;
                    return Result<SymbolSyntax, ParserError>(SymbolSyntax(DefinitionSyntax(node)));
                    break;
                }
            }
        };
    };
    {
        {
            auto _result = parse_function(rp, ep);
            switch (_result._tag)
            {
                case Success::Error:
                {
                    auto error = _result._Error;
                    {
                        {
                            auto _result = error;
                            switch (_result._tag)
                            {
                                case ParserError::Invalid:
                                {
                                    auto i = _result._Invalid;
                                    return Result<SymbolSyntax, ParserError>(i);
                                    break;
                                }
                                case ParserError::Different:
                                {
                                    auto d = _result._Different;
                                    {
                                    };
                                    break;
                                }
                            }
                        };
                    };
                    break;
                }
                case Success::Ok:
                {
                    auto node = _result._Ok;
                    return Result<SymbolSyntax, ParserError>(SymbolSyntax(FunctionSyntax(node)));
                    break;
                }
            }
        };
    };
    {
        {
            auto _result = parse_operator(rp, ep);
            switch (_result._tag)
            {
                case Success::Error:
                {
                    auto error = _result._Error;
                    {
                        {
                            auto _result = error;
                            switch (_result._tag)
                            {
                                case ParserError::Invalid:
                                {
                                    auto i = _result._Invalid;
                                    return Result<SymbolSyntax, ParserError>(i);
                                    break;
                                }
                                case ParserError::Different:
                                {
                                    auto d = _result._Different;
                                    {
                                    };
                                    break;
                                }
                            }
                        };
                    };
                    break;
                }
                case Success::Ok:
                {
                    auto node = _result._Ok;
                    return Result<SymbolSyntax, ParserError>(SymbolSyntax(OperatorSyntax(node)));
                    break;
                }
            }
        };
    };
    {
        {
            auto _result = parse_trait(rp, ep);
            switch (_result._tag)
            {
                case Success::Error:
                {
                    auto error = _result._Error;
                    {
                        {
                            auto _result = error;
                            switch (_result._tag)
                            {
                                case ParserError::Invalid:
                                {
                                    auto i = _result._Invalid;
                                    return Result<SymbolSyntax, ParserError>(i);
                                    break;
                                }
                                case ParserError::Different:
                                {
                                    auto d = _result._Different;
                                    {
                                    };
                                    break;
                                }
                            }
                        };
                    };
                    break;
                }
                case Success::Ok:
                {
                    auto node = _result._Ok;
                    return Result<SymbolSyntax, ParserError>(SymbolSyntax(TraitSyntax(node)));
                    break;
                }
            }
        };
    };
    {
        {
            auto _result = parse_macro(rp, ep);
            switch (_result._tag)
            {
                case Success::Error:
                {
                    auto error = _result._Error;
                    {
                        {
                            auto _result = error;
                            switch (_result._tag)
                            {
                                case ParserError::Invalid:
                                {
                                    auto i = _result._Invalid;
                                    return Result<SymbolSyntax, ParserError>(i);
                                    break;
                                }
                                case ParserError::Different:
                                {
                                    auto d = _result._Different;
                                    {
                                    };
                                    break;
                                }
                            }
                        };
                    };
                    break;
                }
                case Success::Ok:
                {
                    auto node = _result._Ok;
                    return Result<SymbolSyntax, ParserError>(SymbolSyntax(MacroSyntax(node)));
                    break;
                }
            }
        };
    };
    {
        {
            auto _result = parse_module(rp, ep);
            switch (_result._tag)
            {
                case Success::Error:
                {
                    auto error = _result._Error;
                    {
                        {
                            auto _result = error;
                            switch (_result._tag)
                            {
                                case ParserError::Invalid:
                                {
                                    auto i = _result._Invalid;
                                    return Result<SymbolSyntax, ParserError>(i);
                                    break;
                                }
                                case ParserError::Different:
                                {
                                    auto d = _result._Different;
                                    {
                                    };
                                    break;
                                }
                            }
                        };
                    };
                    break;
                }
                case Success::Ok:
                {
                    auto node = _result._Ok;
                    return Result<SymbolSyntax, ParserError>(SymbolSyntax(ModuleSyntax(node)));
                    break;
                }
            }
        };
    };
    return Result<SymbolSyntax, ParserError>(ParserError(DifferentSyntax()));
}

Result<PrivateSyntax, ParserError> Parser::parse_private(Page* rp, Page* ep) {
    const auto start = lexer.previous_position;
    const auto start_private_1 = lexer.previous_position;
    const auto success_private_1 = lexer.parse_keyword(*keywords_index.get(29));
    if (success_private_1 == false) {
        return Result<PrivateSyntax, ParserError>(ParserError(DifferentSyntax()));
    };
    const auto export__start = lexer.position;
    const auto _export__result = parse_export(rp, ep);
    auto export_ = _export__result._Ok;
    if (_export__result._tag == Success::Error) {
        const auto _export__Error = _export__result._Error;
        switch (_export__Error._tag) {
            case ParserError::Different: {
                const auto d = _export__Error._Different;
                return Result<PrivateSyntax, ParserError>(InvalidSyntax(export__start, lexer.position, String(ep, "a valid Export syntax")));
                break;
            }
            case ParserError::Invalid: {
                const auto i = _export__Error._Invalid;
                return Result<PrivateSyntax, ParserError>(i);
                break;
            }
        }
    }
    ;
    const auto end = lexer.position;
    return Result<PrivateSyntax, ParserError>(PrivateSyntax(start, end, export_));
}

Result<ExportSyntax, ParserError> Parser::parse_export(Page* rp, Page* ep) {
    {
        {
            auto _result = parse_definition(rp, ep);
            switch (_result._tag)
            {
                case Success::Error:
                {
                    auto error = _result._Error;
                    {
                        {
                            auto _result = error;
                            switch (_result._tag)
                            {
                                case ParserError::Invalid:
                                {
                                    auto i = _result._Invalid;
                                    return Result<ExportSyntax, ParserError>(i);
                                    break;
                                }
                                case ParserError::Different:
                                {
                                    auto d = _result._Different;
                                    {
                                    };
                                    break;
                                }
                            }
                        };
                    };
                    break;
                }
                case Success::Ok:
                {
                    auto node = _result._Ok;
                    return Result<ExportSyntax, ParserError>(ExportSyntax(DefinitionSyntax(node)));
                    break;
                }
            }
        };
    };
    {
        {
            auto _result = parse_function(rp, ep);
            switch (_result._tag)
            {
                case Success::Error:
                {
                    auto error = _result._Error;
                    {
                        {
                            auto _result = error;
                            switch (_result._tag)
                            {
                                case ParserError::Invalid:
                                {
                                    auto i = _result._Invalid;
                                    return Result<ExportSyntax, ParserError>(i);
                                    break;
                                }
                                case ParserError::Different:
                                {
                                    auto d = _result._Different;
                                    {
                                    };
                                    break;
                                }
                            }
                        };
                    };
                    break;
                }
                case Success::Ok:
                {
                    auto node = _result._Ok;
                    return Result<ExportSyntax, ParserError>(ExportSyntax(FunctionSyntax(node)));
                    break;
                }
            }
        };
    };
    {
        {
            auto _result = parse_operator(rp, ep);
            switch (_result._tag)
            {
                case Success::Error:
                {
                    auto error = _result._Error;
                    {
                        {
                            auto _result = error;
                            switch (_result._tag)
                            {
                                case ParserError::Invalid:
                                {
                                    auto i = _result._Invalid;
                                    return Result<ExportSyntax, ParserError>(i);
                                    break;
                                }
                                case ParserError::Different:
                                {
                                    auto d = _result._Different;
                                    {
                                    };
                                    break;
                                }
                            }
                        };
                    };
                    break;
                }
                case Success::Ok:
                {
                    auto node = _result._Ok;
                    return Result<ExportSyntax, ParserError>(ExportSyntax(OperatorSyntax(node)));
                    break;
                }
            }
        };
    };
    {
        {
            auto _result = parse_trait(rp, ep);
            switch (_result._tag)
            {
                case Success::Error:
                {
                    auto error = _result._Error;
                    {
                        {
                            auto _result = error;
                            switch (_result._tag)
                            {
                                case ParserError::Invalid:
                                {
                                    auto i = _result._Invalid;
                                    return Result<ExportSyntax, ParserError>(i);
                                    break;
                                }
                                case ParserError::Different:
                                {
                                    auto d = _result._Different;
                                    {
                                    };
                                    break;
                                }
                            }
                        };
                    };
                    break;
                }
                case Success::Ok:
                {
                    auto node = _result._Ok;
                    return Result<ExportSyntax, ParserError>(ExportSyntax(TraitSyntax(node)));
                    break;
                }
            }
        };
    };
    {
        {
            auto _result = parse_module(rp, ep);
            switch (_result._tag)
            {
                case Success::Error:
                {
                    auto error = _result._Error;
                    {
                        {
                            auto _result = error;
                            switch (_result._tag)
                            {
                                case ParserError::Invalid:
                                {
                                    auto i = _result._Invalid;
                                    return Result<ExportSyntax, ParserError>(i);
                                    break;
                                }
                                case ParserError::Different:
                                {
                                    auto d = _result._Different;
                                    {
                                    };
                                    break;
                                }
                            }
                        };
                    };
                    break;
                }
                case Success::Ok:
                {
                    auto node = _result._Ok;
                    return Result<ExportSyntax, ParserError>(ExportSyntax(ModuleSyntax(node)));
                    break;
                }
            }
        };
    };
    return Result<ExportSyntax, ParserError>(ParserError(DifferentSyntax()));
}

Result<Vector<MemberSyntax>*, ParserError> Parser::parse_member_list(Page* rp, Page* ep) {
    auto r = Region();
    auto list = new (alignof(List<MemberSyntax>), r.get_page()) List<MemberSyntax>();
    while (true) {
        const auto _node_result = parse_member(rp, ep);
        auto node = _node_result._Ok;
        if (_node_result._tag == Success::Error) {
            const auto _node_Error = _node_result._Error;
            switch (_node_Error._tag) {
                case ParserError::Invalid: {
                    const auto i = _node_Error._Invalid;
                    return Result<Vector<MemberSyntax>*, ParserError>(i);
                    break;
                }
                case ParserError::Different: {
                    const auto d = _node_Error._Different;
                    {
                        if ((*list).head == 0) {
                            return Result<Vector<MemberSyntax>*, ParserError>(d);
                        }
                        else {
                            return Result<Vector<MemberSyntax>*, ParserError>(new (alignof(Vector<MemberSyntax>), rp) Vector<MemberSyntax>(rp, *list));
                        };
                    };
                    break;
                }
            }
        }
        (*list).add(node);
    };
}

Result<MemberSyntax, ParserError> Parser::parse_member(Page* rp, Page* ep) {
    const auto start = lexer.previous_position;
    const auto constituent_start = lexer.position;
    const auto _constituent_result = parse_constituent(rp, ep);
    auto constituent = _constituent_result._Ok;
    if (_constituent_result._tag == Success::Error) {
        const auto _constituent_Error = _constituent_result._Error;
        switch (_constituent_Error._tag) {
            case ParserError::Different: {
                const auto d = _constituent_Error._Different;
                return Result<MemberSyntax, ParserError>(d);
                break;
            }
            case ParserError::Invalid: {
                const auto i = _constituent_Error._Invalid;
                return Result<MemberSyntax, ParserError>(i);
                break;
            }
        }
    }
    ;
    const auto start_colon_2 = lexer.previous_position;
    const auto success_colon_2 = lexer.parse_colon();
    const auto end = lexer.position;
    return Result<MemberSyntax, ParserError>(MemberSyntax(start, end, constituent));
}

Result<ConstituentSyntax, ParserError> Parser::parse_constituent(Page* rp, Page* ep) {
    {
        {
            auto _result = parse_definition(rp, ep);
            switch (_result._tag)
            {
                case Success::Error:
                {
                    auto error = _result._Error;
                    {
                        {
                            auto _result = error;
                            switch (_result._tag)
                            {
                                case ParserError::Invalid:
                                {
                                    auto i = _result._Invalid;
                                    return Result<ConstituentSyntax, ParserError>(i);
                                    break;
                                }
                                case ParserError::Different:
                                {
                                    auto d = _result._Different;
                                    {
                                    };
                                    break;
                                }
                            }
                        };
                    };
                    break;
                }
                case Success::Ok:
                {
                    auto node = _result._Ok;
                    return Result<ConstituentSyntax, ParserError>(ConstituentSyntax(DefinitionSyntax(node)));
                    break;
                }
            }
        };
    };
    {
        {
            auto _result = parse_function(rp, ep);
            switch (_result._tag)
            {
                case Success::Error:
                {
                    auto error = _result._Error;
                    {
                        {
                            auto _result = error;
                            switch (_result._tag)
                            {
                                case ParserError::Invalid:
                                {
                                    auto i = _result._Invalid;
                                    return Result<ConstituentSyntax, ParserError>(i);
                                    break;
                                }
                                case ParserError::Different:
                                {
                                    auto d = _result._Different;
                                    {
                                    };
                                    break;
                                }
                            }
                        };
                    };
                    break;
                }
                case Success::Ok:
                {
                    auto node = _result._Ok;
                    return Result<ConstituentSyntax, ParserError>(ConstituentSyntax(FunctionSyntax(node)));
                    break;
                }
            }
        };
    };
    {
        {
            auto _result = parse_procedure(rp, ep);
            switch (_result._tag)
            {
                case Success::Error:
                {
                    auto error = _result._Error;
                    {
                        {
                            auto _result = error;
                            switch (_result._tag)
                            {
                                case ParserError::Invalid:
                                {
                                    auto i = _result._Invalid;
                                    return Result<ConstituentSyntax, ParserError>(i);
                                    break;
                                }
                                case ParserError::Different:
                                {
                                    auto d = _result._Different;
                                    {
                                    };
                                    break;
                                }
                            }
                        };
                    };
                    break;
                }
                case Success::Ok:
                {
                    auto node = _result._Ok;
                    return Result<ConstituentSyntax, ParserError>(ConstituentSyntax(ProcedureSyntax(node)));
                    break;
                }
            }
        };
    };
    {
        {
            auto _result = parse_operator(rp, ep);
            switch (_result._tag)
            {
                case Success::Error:
                {
                    auto error = _result._Error;
                    {
                        {
                            auto _result = error;
                            switch (_result._tag)
                            {
                                case ParserError::Invalid:
                                {
                                    auto i = _result._Invalid;
                                    return Result<ConstituentSyntax, ParserError>(i);
                                    break;
                                }
                                case ParserError::Different:
                                {
                                    auto d = _result._Different;
                                    {
                                    };
                                    break;
                                }
                            }
                        };
                    };
                    break;
                }
                case Success::Ok:
                {
                    auto node = _result._Ok;
                    return Result<ConstituentSyntax, ParserError>(ConstituentSyntax(OperatorSyntax(node)));
                    break;
                }
            }
        };
    };
    {
        {
            auto _result = parse_implement(rp, ep);
            switch (_result._tag)
            {
                case Success::Error:
                {
                    auto error = _result._Error;
                    {
                        {
                            auto _result = error;
                            switch (_result._tag)
                            {
                                case ParserError::Invalid:
                                {
                                    auto i = _result._Invalid;
                                    return Result<ConstituentSyntax, ParserError>(i);
                                    break;
                                }
                                case ParserError::Different:
                                {
                                    auto d = _result._Different;
                                    {
                                    };
                                    break;
                                }
                            }
                        };
                    };
                    break;
                }
                case Success::Ok:
                {
                    auto node = _result._Ok;
                    return Result<ConstituentSyntax, ParserError>(ConstituentSyntax(ImplementSyntax(node)));
                    break;
                }
            }
        };
    };
    {
        {
            auto _result = parse_trait(rp, ep);
            switch (_result._tag)
            {
                case Success::Error:
                {
                    auto error = _result._Error;
                    {
                        {
                            auto _result = error;
                            switch (_result._tag)
                            {
                                case ParserError::Invalid:
                                {
                                    auto i = _result._Invalid;
                                    return Result<ConstituentSyntax, ParserError>(i);
                                    break;
                                }
                                case ParserError::Different:
                                {
                                    auto d = _result._Different;
                                    {
                                    };
                                    break;
                                }
                            }
                        };
                    };
                    break;
                }
                case Success::Ok:
                {
                    auto node = _result._Ok;
                    return Result<ConstituentSyntax, ParserError>(ConstituentSyntax(TraitSyntax(node)));
                    break;
                }
            }
        };
    };
    {
        {
            auto _result = parse_macro(rp, ep);
            switch (_result._tag)
            {
                case Success::Error:
                {
                    auto error = _result._Error;
                    {
                        {
                            auto _result = error;
                            switch (_result._tag)
                            {
                                case ParserError::Invalid:
                                {
                                    auto i = _result._Invalid;
                                    return Result<ConstituentSyntax, ParserError>(i);
                                    break;
                                }
                                case ParserError::Different:
                                {
                                    auto d = _result._Different;
                                    {
                                    };
                                    break;
                                }
                            }
                        };
                    };
                    break;
                }
                case Success::Ok:
                {
                    auto node = _result._Ok;
                    return Result<ConstituentSyntax, ParserError>(ConstituentSyntax(MacroSyntax(node)));
                    break;
                }
            }
        };
    };
    {
        {
            auto _result = parse_module(rp, ep);
            switch (_result._tag)
            {
                case Success::Error:
                {
                    auto error = _result._Error;
                    {
                        {
                            auto _result = error;
                            switch (_result._tag)
                            {
                                case ParserError::Invalid:
                                {
                                    auto i = _result._Invalid;
                                    return Result<ConstituentSyntax, ParserError>(i);
                                    break;
                                }
                                case ParserError::Different:
                                {
                                    auto d = _result._Different;
                                    {
                                    };
                                    break;
                                }
                            }
                        };
                    };
                    break;
                }
                case Success::Ok:
                {
                    auto node = _result._Ok;
                    return Result<ConstituentSyntax, ParserError>(ConstituentSyntax(ModuleSyntax(node)));
                    break;
                }
            }
        };
    };
    return Result<ConstituentSyntax, ParserError>(ParserError(DifferentSyntax()));
}

Result<DefinitionSyntax, ParserError> Parser::parse_definition(Page* rp, Page* ep) {
    const auto start = lexer.previous_position;
    const auto start_define_1 = lexer.previous_position;
    const auto success_define_1 = lexer.parse_keyword(*keywords_index.get(4));
    if (success_define_1 == false) {
        return Result<DefinitionSyntax, ParserError>(ParserError(DifferentSyntax()));
    };
    const auto start_name = lexer.previous_position;
    const auto name = lexer.parse_identifier(rp, keywords);
    if (name == nullptr) {
        return Result<DefinitionSyntax, ParserError>(ParserError(InvalidSyntax(start_name, lexer.position, String(ep, "an identifier"))));
    };
    const auto parameters_start = lexer.position;
    GenericParametersSyntax* parameters = nullptr;
    {
        auto _result = parse_genericparameters(rp, ep);
        switch (_result._tag)
        {
            case Success::Error:
            {
                auto error = _result._Error;
                {
                    {
                        auto _result = error;
                        switch (_result._tag)
                        {
                            case ParserError::Different:
                            {
                                auto d = _result._Different;
                                {
                                };
                                break;
                            }
                            case ParserError::Invalid:
                            {
                                auto i = _result._Invalid;
                                return Result<DefinitionSyntax, ParserError>(i);
                                break;
                            }
                        }
                    };
                };
                break;
            }
            case Success::Ok:
            {
                auto success = _result._Ok;
                parameters = new (alignof(GenericParametersSyntax), rp) GenericParametersSyntax(success);
                break;
            }
        }
    };
    const auto start_colon_4 = lexer.previous_position;
    const auto success_colon_4 = lexer.parse_colon();
    const auto attributes_start = lexer.position;
    Vector<AttributeSyntax>* attributes = nullptr;
    {
        auto _result = parse_attribute_list(rp, ep);
        switch (_result._tag)
        {
            case Success::Error:
            {
                auto error = _result._Error;
                {
                    {
                        auto _result = error;
                        switch (_result._tag)
                        {
                            case ParserError::Different:
                            {
                                auto d = _result._Different;
                                {
                                };
                                break;
                            }
                            case ParserError::Invalid:
                            {
                                auto i = _result._Invalid;
                                return Result<DefinitionSyntax, ParserError>(i);
                                break;
                            }
                        }
                    };
                };
                break;
            }
            case Success::Ok:
            {
                auto success = _result._Ok;
                attributes = success;
                break;
            }
        }
    };
    const auto concept__start = lexer.position;
    const auto _concept__result = parse_concept(rp, ep);
    auto concept_ = _concept__result._Ok;
    if (_concept__result._tag == Success::Error) {
        const auto _concept__Error = _concept__result._Error;
        switch (_concept__Error._tag) {
            case ParserError::Different: {
                const auto d = _concept__Error._Different;
                return Result<DefinitionSyntax, ParserError>(InvalidSyntax(concept__start, lexer.position, String(ep, "a valid Concept syntax")));
                break;
            }
            case ParserError::Invalid: {
                const auto i = _concept__Error._Invalid;
                return Result<DefinitionSyntax, ParserError>(i);
                break;
            }
        }
    }
    ;
    const auto end = lexer.position;
    return Result<DefinitionSyntax, ParserError>(DefinitionSyntax(start, end, *name, parameters, attributes, concept_));
}

Result<GenericParametersSyntax, ParserError> Parser::parse_genericparameters(Page* rp, Page* ep) {
    const auto start = lexer.previous_position;
    const auto start_left_bracket_1 = lexer.previous_position;
    const auto success_left_bracket_1 = lexer.parse_punctuation('[');
    if (success_left_bracket_1 == false) {
        return Result<GenericParametersSyntax, ParserError>(ParserError(DifferentSyntax()));
    };
    const auto parameters_start = lexer.position;
    const auto _parameters_result = parse_genericparameter_list(rp, ep);
    auto parameters = _parameters_result._Ok;
    if (_parameters_result._tag == Success::Error) {
        const auto _parameters_Error = _parameters_result._Error;
        switch (_parameters_Error._tag) {
            case ParserError::Different: {
                const auto d = _parameters_Error._Different;
                return Result<GenericParametersSyntax, ParserError>(InvalidSyntax(parameters_start, lexer.position, String(ep, "a valid GenericParameter syntax")));
                break;
            }
            case ParserError::Invalid: {
                const auto i = _parameters_Error._Invalid;
                return Result<GenericParametersSyntax, ParserError>(i);
                break;
            }
        }
    }
    ;
    const auto start_right_bracket_3 = lexer.previous_position;
    const auto success_right_bracket_3 = lexer.parse_punctuation(']');
    if (success_right_bracket_3 == false) {
        return Result<GenericParametersSyntax, ParserError>(ParserError(InvalidSyntax(start_right_bracket_3, lexer.position, String(ep, "]"))));
    };
    const auto end = lexer.position;
    return Result<GenericParametersSyntax, ParserError>(GenericParametersSyntax(start, end, *parameters));
}

Result<Vector<GenericParameterSyntax>*, ParserError> Parser::parse_genericparameter_list(Page* rp, Page* ep) {
    auto r = Region();
    auto list = new (alignof(List<GenericParameterSyntax>), r.get_page()) List<GenericParameterSyntax>();
    while (true) {
        const auto _node_result = parse_genericparameter(rp, ep);
        auto node = _node_result._Ok;
        if (_node_result._tag == Success::Error) {
            const auto _node_Error = _node_result._Error;
            switch (_node_Error._tag) {
                case ParserError::Invalid: {
                    const auto i = _node_Error._Invalid;
                    return Result<Vector<GenericParameterSyntax>*, ParserError>(i);
                    break;
                }
                case ParserError::Different: {
                    const auto d = _node_Error._Different;
                    {
                        if ((*list).head == 0) {
                            return Result<Vector<GenericParameterSyntax>*, ParserError>(d);
                        }
                        else {
                            return Result<Vector<GenericParameterSyntax>*, ParserError>(new (alignof(Vector<GenericParameterSyntax>), rp) Vector<GenericParameterSyntax>(rp, *list));
                        };
                    };
                    break;
                }
            }
        }
        (*list).add(node);
    };
}

Result<GenericParameterSyntax, ParserError> Parser::parse_genericparameter(Page* rp, Page* ep) {
    const auto start = lexer.previous_position;
    const auto start_name = lexer.previous_position;
    const auto name = lexer.parse_identifier(rp, keywords);
    if (name == nullptr) {
        return Result<GenericParameterSyntax, ParserError>(ParserError(DifferentSyntax()));
    };
    const auto attributes_start = lexer.position;
    Vector<AttributeSyntax>* attributes = nullptr;
    {
        auto _result = parse_attribute_list(rp, ep);
        switch (_result._tag)
        {
            case Success::Error:
            {
                auto error = _result._Error;
                {
                    {
                        auto _result = error;
                        switch (_result._tag)
                        {
                            case ParserError::Different:
                            {
                                auto d = _result._Different;
                                {
                                };
                                break;
                            }
                            case ParserError::Invalid:
                            {
                                auto i = _result._Invalid;
                                return Result<GenericParameterSyntax, ParserError>(i);
                                break;
                            }
                        }
                    };
                };
                break;
            }
            case Success::Ok:
            {
                auto success = _result._Ok;
                attributes = success;
                break;
            }
        }
    };
    const auto start_comma_3 = lexer.previous_position;
    const auto success_comma_3 = lexer.parse_punctuation(',');
    const auto end = lexer.position;
    return Result<GenericParameterSyntax, ParserError>(GenericParameterSyntax(start, end, *name, attributes));
}

Result<ConceptSyntax, ParserError> Parser::parse_concept(Page* rp, Page* ep) {
    {
        {
            auto _result = parse_class(rp, ep);
            switch (_result._tag)
            {
                case Success::Error:
                {
                    auto error = _result._Error;
                    {
                        {
                            auto _result = error;
                            switch (_result._tag)
                            {
                                case ParserError::Invalid:
                                {
                                    auto i = _result._Invalid;
                                    return Result<ConceptSyntax, ParserError>(i);
                                    break;
                                }
                                case ParserError::Different:
                                {
                                    auto d = _result._Different;
                                    {
                                    };
                                    break;
                                }
                            }
                        };
                    };
                    break;
                }
                case Success::Ok:
                {
                    auto node = _result._Ok;
                    return Result<ConceptSyntax, ParserError>(ConceptSyntax(ClassSyntax(node)));
                    break;
                }
            }
        };
    };
    {
        {
            auto _result = parse_namespace(rp, ep);
            switch (_result._tag)
            {
                case Success::Error:
                {
                    auto error = _result._Error;
                    {
                        {
                            auto _result = error;
                            switch (_result._tag)
                            {
                                case ParserError::Invalid:
                                {
                                    auto i = _result._Invalid;
                                    return Result<ConceptSyntax, ParserError>(i);
                                    break;
                                }
                                case ParserError::Different:
                                {
                                    auto d = _result._Different;
                                    {
                                    };
                                    break;
                                }
                            }
                        };
                    };
                    break;
                }
                case Success::Ok:
                {
                    auto node = _result._Ok;
                    return Result<ConceptSyntax, ParserError>(ConceptSyntax(NamespaceSyntax(node)));
                    break;
                }
            }
        };
    };
    {
        {
            auto _result = parse_union(rp, ep);
            switch (_result._tag)
            {
                case Success::Error:
                {
                    auto error = _result._Error;
                    {
                        {
                            auto _result = error;
                            switch (_result._tag)
                            {
                                case ParserError::Invalid:
                                {
                                    auto i = _result._Invalid;
                                    return Result<ConceptSyntax, ParserError>(i);
                                    break;
                                }
                                case ParserError::Different:
                                {
                                    auto d = _result._Different;
                                    {
                                    };
                                    break;
                                }
                            }
                        };
                    };
                    break;
                }
                case Success::Ok:
                {
                    auto node = _result._Ok;
                    return Result<ConceptSyntax, ParserError>(ConceptSyntax(UnionSyntax(node)));
                    break;
                }
            }
        };
    };
    {
        {
            auto _result = parse_constant(rp, ep);
            switch (_result._tag)
            {
                case Success::Error:
                {
                    auto error = _result._Error;
                    {
                        {
                            auto _result = error;
                            switch (_result._tag)
                            {
                                case ParserError::Invalid:
                                {
                                    auto i = _result._Invalid;
                                    return Result<ConceptSyntax, ParserError>(i);
                                    break;
                                }
                                case ParserError::Different:
                                {
                                    auto d = _result._Different;
                                    {
                                    };
                                    break;
                                }
                            }
                        };
                    };
                    break;
                }
                case Success::Ok:
                {
                    auto node = _result._Ok;
                    return Result<ConceptSyntax, ParserError>(ConceptSyntax(ConstantSyntax(node)));
                    break;
                }
            }
        };
    };
    {
        {
            auto _result = parse_delegate(rp, ep);
            switch (_result._tag)
            {
                case Success::Error:
                {
                    auto error = _result._Error;
                    {
                        {
                            auto _result = error;
                            switch (_result._tag)
                            {
                                case ParserError::Invalid:
                                {
                                    auto i = _result._Invalid;
                                    return Result<ConceptSyntax, ParserError>(i);
                                    break;
                                }
                                case ParserError::Different:
                                {
                                    auto d = _result._Different;
                                    {
                                    };
                                    break;
                                }
                            }
                        };
                    };
                    break;
                }
                case Success::Ok:
                {
                    auto node = _result._Ok;
                    return Result<ConceptSyntax, ParserError>(ConceptSyntax(DelegateSyntax(node)));
                    break;
                }
            }
        };
    };
    {
        {
            auto _result = parse_intrinsic(rp, ep);
            switch (_result._tag)
            {
                case Success::Error:
                {
                    auto error = _result._Error;
                    {
                        {
                            auto _result = error;
                            switch (_result._tag)
                            {
                                case ParserError::Invalid:
                                {
                                    auto i = _result._Invalid;
                                    return Result<ConceptSyntax, ParserError>(i);
                                    break;
                                }
                                case ParserError::Different:
                                {
                                    auto d = _result._Different;
                                    {
                                    };
                                    break;
                                }
                            }
                        };
                    };
                    break;
                }
                case Success::Ok:
                {
                    auto node = _result._Ok;
                    return Result<ConceptSyntax, ParserError>(ConceptSyntax(IntrinsicSyntax(node)));
                    break;
                }
            }
        };
    };
    return Result<ConceptSyntax, ParserError>(ParserError(DifferentSyntax()));
}

Result<ClassSyntax, ParserError> Parser::parse_class(Page* rp, Page* ep) {
    const auto start = lexer.previous_position;
    const auto structure_start = lexer.position;
    const auto _structure_result = parse_structure(rp, ep);
    auto structure = _structure_result._Ok;
    if (_structure_result._tag == Success::Error) {
        const auto _structure_Error = _structure_result._Error;
        switch (_structure_Error._tag) {
            case ParserError::Different: {
                const auto d = _structure_Error._Different;
                return Result<ClassSyntax, ParserError>(d);
                break;
            }
            case ParserError::Invalid: {
                const auto i = _structure_Error._Invalid;
                return Result<ClassSyntax, ParserError>(i);
                break;
            }
        }
    }
    ;
    const auto start_colon_2 = lexer.previous_position;
    const auto success_colon_2 = lexer.parse_colon();
    const auto body_start = lexer.position;
    BodySyntax* body = nullptr;
    {
        auto _result = parse_body(rp, ep);
        switch (_result._tag)
        {
            case Success::Error:
            {
                auto error = _result._Error;
                {
                    {
                        auto _result = error;
                        switch (_result._tag)
                        {
                            case ParserError::Different:
                            {
                                auto d = _result._Different;
                                {
                                };
                                break;
                            }
                            case ParserError::Invalid:
                            {
                                auto i = _result._Invalid;
                                return Result<ClassSyntax, ParserError>(i);
                                break;
                            }
                        }
                    };
                };
                break;
            }
            case Success::Ok:
            {
                auto success = _result._Ok;
                body = new (alignof(BodySyntax), rp) BodySyntax(success);
                break;
            }
        }
    };
    const auto start_colon_4 = lexer.previous_position;
    const auto success_colon_4 = lexer.parse_colon();
    const auto end = lexer.position;
    return Result<ClassSyntax, ParserError>(ClassSyntax(start, end, structure, body));
}

Result<BodySyntax, ParserError> Parser::parse_body(Page* rp, Page* ep) {
    const auto start = lexer.previous_position;
    const auto start_left_curly_1 = lexer.previous_position;
    const auto success_left_curly_1 = lexer.parse_punctuation('{');
    if (success_left_curly_1 == false) {
        return Result<BodySyntax, ParserError>(ParserError(DifferentSyntax()));
    };
    const auto uses_start = lexer.position;
    Vector<UseSyntax>* uses = nullptr;
    {
        auto _result = parse_use_list(rp, ep);
        switch (_result._tag)
        {
            case Success::Error:
            {
                auto error = _result._Error;
                {
                    {
                        auto _result = error;
                        switch (_result._tag)
                        {
                            case ParserError::Different:
                            {
                                auto d = _result._Different;
                                {
                                };
                                break;
                            }
                            case ParserError::Invalid:
                            {
                                auto i = _result._Invalid;
                                return Result<BodySyntax, ParserError>(i);
                                break;
                            }
                        }
                    };
                };
                break;
            }
            case Success::Ok:
            {
                auto success = _result._Ok;
                uses = success;
                break;
            }
        }
    };
    const auto inits_start = lexer.position;
    Vector<InitSyntax>* inits = nullptr;
    {
        auto _result = parse_init_list(rp, ep);
        switch (_result._tag)
        {
            case Success::Error:
            {
                auto error = _result._Error;
                {
                    {
                        auto _result = error;
                        switch (_result._tag)
                        {
                            case ParserError::Different:
                            {
                                auto d = _result._Different;
                                {
                                };
                                break;
                            }
                            case ParserError::Invalid:
                            {
                                auto i = _result._Invalid;
                                return Result<BodySyntax, ParserError>(i);
                                break;
                            }
                        }
                    };
                };
                break;
            }
            case Success::Ok:
            {
                auto success = _result._Ok;
                inits = success;
                break;
            }
        }
    };
    const auto deInit_start = lexer.position;
    DeInitSyntax* deInit = nullptr;
    {
        auto _result = parse_deinit(rp, ep);
        switch (_result._tag)
        {
            case Success::Error:
            {
                auto error = _result._Error;
                {
                    {
                        auto _result = error;
                        switch (_result._tag)
                        {
                            case ParserError::Different:
                            {
                                auto d = _result._Different;
                                {
                                };
                                break;
                            }
                            case ParserError::Invalid:
                            {
                                auto i = _result._Invalid;
                                return Result<BodySyntax, ParserError>(i);
                                break;
                            }
                        }
                    };
                };
                break;
            }
            case Success::Ok:
            {
                auto success = _result._Ok;
                deInit = new (alignof(DeInitSyntax), rp) DeInitSyntax(success);
                break;
            }
        }
    };
    const auto members_start = lexer.position;
    Vector<MemberSyntax>* members = nullptr;
    {
        auto _result = parse_member_list(rp, ep);
        switch (_result._tag)
        {
            case Success::Error:
            {
                auto error = _result._Error;
                {
                    {
                        auto _result = error;
                        switch (_result._tag)
                        {
                            case ParserError::Different:
                            {
                                auto d = _result._Different;
                                {
                                };
                                break;
                            }
                            case ParserError::Invalid:
                            {
                                auto i = _result._Invalid;
                                return Result<BodySyntax, ParserError>(i);
                                break;
                            }
                        }
                    };
                };
                break;
            }
            case Success::Ok:
            {
                auto success = _result._Ok;
                members = success;
                break;
            }
        }
    };
    const auto start_right_curly_6 = lexer.previous_position;
    const auto success_right_curly_6 = lexer.parse_punctuation('}');
    if (success_right_curly_6 == false) {
        return Result<BodySyntax, ParserError>(ParserError(InvalidSyntax(start_right_curly_6, lexer.position, String(ep, "}"))));
    };
    const auto start_colon_7 = lexer.previous_position;
    const auto success_colon_7 = lexer.parse_colon();
    const auto end = lexer.position;
    return Result<BodySyntax, ParserError>(BodySyntax(start, end, uses, inits, deInit, members));
}

Result<NamespaceSyntax, ParserError> Parser::parse_namespace(Page* rp, Page* ep) {
    const auto start = lexer.previous_position;
    const auto start_left_curly_1 = lexer.previous_position;
    const auto success_left_curly_1 = lexer.parse_punctuation('{');
    if (success_left_curly_1 == false) {
        return Result<NamespaceSyntax, ParserError>(ParserError(DifferentSyntax()));
    };
    const auto uses_start = lexer.position;
    Vector<UseSyntax>* uses = nullptr;
    {
        auto _result = parse_use_list(rp, ep);
        switch (_result._tag)
        {
            case Success::Error:
            {
                auto error = _result._Error;
                {
                    {
                        auto _result = error;
                        switch (_result._tag)
                        {
                            case ParserError::Different:
                            {
                                auto d = _result._Different;
                                {
                                };
                                break;
                            }
                            case ParserError::Invalid:
                            {
                                auto i = _result._Invalid;
                                return Result<NamespaceSyntax, ParserError>(i);
                                break;
                            }
                        }
                    };
                };
                break;
            }
            case Success::Ok:
            {
                auto success = _result._Ok;
                uses = success;
                break;
            }
        }
    };
    const auto declarations_start = lexer.position;
    Vector<DeclarationSyntax>* declarations = nullptr;
    {
        auto _result = parse_declaration_list(rp, ep);
        switch (_result._tag)
        {
            case Success::Error:
            {
                auto error = _result._Error;
                {
                    {
                        auto _result = error;
                        switch (_result._tag)
                        {
                            case ParserError::Different:
                            {
                                auto d = _result._Different;
                                {
                                };
                                break;
                            }
                            case ParserError::Invalid:
                            {
                                auto i = _result._Invalid;
                                return Result<NamespaceSyntax, ParserError>(i);
                                break;
                            }
                        }
                    };
                };
                break;
            }
            case Success::Ok:
            {
                auto success = _result._Ok;
                declarations = success;
                break;
            }
        }
    };
    const auto start_right_curly_4 = lexer.previous_position;
    const auto success_right_curly_4 = lexer.parse_punctuation('}');
    if (success_right_curly_4 == false) {
        return Result<NamespaceSyntax, ParserError>(ParserError(InvalidSyntax(start_right_curly_4, lexer.position, String(ep, "}"))));
    };
    const auto start_colon_5 = lexer.previous_position;
    const auto success_colon_5 = lexer.parse_colon();
    const auto end = lexer.position;
    return Result<NamespaceSyntax, ParserError>(NamespaceSyntax(start, end, uses, declarations));
}

Result<UnionSyntax, ParserError> Parser::parse_union(Page* rp, Page* ep) {
    const auto start = lexer.previous_position;
    const auto start_union_1 = lexer.previous_position;
    const auto success_union_1 = lexer.parse_keyword(*keywords_index.get(39));
    if (success_union_1 == false) {
        return Result<UnionSyntax, ParserError>(ParserError(DifferentSyntax()));
    };
    const auto start_colon_2 = lexer.previous_position;
    const auto success_colon_2 = lexer.parse_colon();
    const auto start_left_paren_3 = lexer.previous_position;
    const auto success_left_paren_3 = lexer.parse_punctuation('(');
    if (success_left_paren_3 == false) {
        return Result<UnionSyntax, ParserError>(ParserError(InvalidSyntax(start_left_paren_3, lexer.position, String(ep, "("))));
    };
    const auto variants_start = lexer.position;
    const auto _variants_result = parse_variant_list(rp, ep);
    auto variants = _variants_result._Ok;
    if (_variants_result._tag == Success::Error) {
        const auto _variants_Error = _variants_result._Error;
        switch (_variants_Error._tag) {
            case ParserError::Different: {
                const auto d = _variants_Error._Different;
                return Result<UnionSyntax, ParserError>(InvalidSyntax(variants_start, lexer.position, String(ep, "a valid Variant syntax")));
                break;
            }
            case ParserError::Invalid: {
                const auto i = _variants_Error._Invalid;
                return Result<UnionSyntax, ParserError>(i);
                break;
            }
        }
    }
    ;
    const auto start_right_paren_5 = lexer.previous_position;
    const auto success_right_paren_5 = lexer.parse_punctuation(')');
    if (success_right_paren_5 == false) {
        return Result<UnionSyntax, ParserError>(ParserError(InvalidSyntax(start_right_paren_5, lexer.position, String(ep, ")"))));
    };
    const auto start_colon_6 = lexer.previous_position;
    const auto success_colon_6 = lexer.parse_colon();
    const auto end = lexer.position;
    return Result<UnionSyntax, ParserError>(UnionSyntax(start, end, *variants));
}

Result<Vector<VariantSyntax>*, ParserError> Parser::parse_variant_list(Page* rp, Page* ep) {
    auto r = Region();
    auto list = new (alignof(List<VariantSyntax>), r.get_page()) List<VariantSyntax>();
    while (true) {
        const auto _node_result = parse_variant(rp, ep);
        auto node = _node_result._Ok;
        if (_node_result._tag == Success::Error) {
            const auto _node_Error = _node_result._Error;
            switch (_node_Error._tag) {
                case ParserError::Invalid: {
                    const auto i = _node_Error._Invalid;
                    return Result<Vector<VariantSyntax>*, ParserError>(i);
                    break;
                }
                case ParserError::Different: {
                    const auto d = _node_Error._Different;
                    {
                        if ((*list).head == 0) {
                            return Result<Vector<VariantSyntax>*, ParserError>(d);
                        }
                        else {
                            return Result<Vector<VariantSyntax>*, ParserError>(new (alignof(Vector<VariantSyntax>), rp) Vector<VariantSyntax>(rp, *list));
                        };
                    };
                    break;
                }
            }
        }
        (*list).add(node);
    };
}

Result<VariantSyntax, ParserError> Parser::parse_variant(Page* rp, Page* ep) {
    const auto start = lexer.previous_position;
    const auto start_name = lexer.previous_position;
    const auto name = lexer.parse_identifier(rp, keywords);
    if (name == nullptr) {
        return Result<VariantSyntax, ParserError>(ParserError(DifferentSyntax()));
    };
    const auto attributes_start = lexer.position;
    Vector<AttributeSyntax>* attributes = nullptr;
    {
        auto _result = parse_attribute_list(rp, ep);
        switch (_result._tag)
        {
            case Success::Error:
            {
                auto error = _result._Error;
                {
                    {
                        auto _result = error;
                        switch (_result._tag)
                        {
                            case ParserError::Different:
                            {
                                auto d = _result._Different;
                                {
                                };
                                break;
                            }
                            case ParserError::Invalid:
                            {
                                auto i = _result._Invalid;
                                return Result<VariantSyntax, ParserError>(i);
                                break;
                            }
                        }
                    };
                };
                break;
            }
            case Success::Ok:
            {
                auto success = _result._Ok;
                attributes = success;
                break;
            }
        }
    };
    const auto annotation_start = lexer.position;
    TypeAnnotationSyntax* annotation = nullptr;
    {
        auto _result = parse_typeannotation(rp, ep);
        switch (_result._tag)
        {
            case Success::Error:
            {
                auto error = _result._Error;
                {
                    {
                        auto _result = error;
                        switch (_result._tag)
                        {
                            case ParserError::Different:
                            {
                                auto d = _result._Different;
                                {
                                };
                                break;
                            }
                            case ParserError::Invalid:
                            {
                                auto i = _result._Invalid;
                                return Result<VariantSyntax, ParserError>(i);
                                break;
                            }
                        }
                    };
                };
                break;
            }
            case Success::Ok:
            {
                auto success = _result._Ok;
                annotation = new (alignof(TypeAnnotationSyntax), rp) TypeAnnotationSyntax(success);
                break;
            }
        }
    };
    const auto start_comma_4 = lexer.previous_position;
    const auto success_comma_4 = lexer.parse_punctuation(',');
    const auto start_colon_5 = lexer.previous_position;
    const auto success_colon_5 = lexer.parse_colon();
    const auto end = lexer.position;
    return Result<VariantSyntax, ParserError>(VariantSyntax(start, end, *name, attributes, annotation));
}

Result<ConstantSyntax, ParserError> Parser::parse_constant(Page* rp, Page* ep) {
    const auto start = lexer.previous_position;
    const auto type_start = lexer.position;
    const auto _type_result = parse_type(rp, ep);
    auto type = _type_result._Ok;
    if (_type_result._tag == Success::Error) {
        const auto _type_Error = _type_result._Error;
        switch (_type_Error._tag) {
            case ParserError::Different: {
                const auto d = _type_Error._Different;
                return Result<ConstantSyntax, ParserError>(d);
                break;
            }
            case ParserError::Invalid: {
                const auto i = _type_Error._Invalid;
                return Result<ConstantSyntax, ParserError>(i);
                break;
            }
        }
    }
    ;
    const auto operation_start = lexer.position;
    const auto _operation_result = parse_operand_list(rp, ep);
    auto operation = _operation_result._Ok;
    if (_operation_result._tag == Success::Error) {
        const auto _operation_Error = _operation_result._Error;
        switch (_operation_Error._tag) {
            case ParserError::Different: {
                const auto d = _operation_Error._Different;
                return Result<ConstantSyntax, ParserError>(InvalidSyntax(operation_start, lexer.position, String(ep, "a valid Operand syntax")));
                break;
            }
            case ParserError::Invalid: {
                const auto i = _operation_Error._Invalid;
                return Result<ConstantSyntax, ParserError>(i);
                break;
            }
        }
    }
    ;
    const auto start_colon_3 = lexer.previous_position;
    const auto success_colon_3 = lexer.parse_colon();
    const auto end = lexer.position;
    return Result<ConstantSyntax, ParserError>(ConstantSyntax(start, end, type, *operation));
}

Result<DelegateSyntax, ParserError> Parser::parse_delegate(Page* rp, Page* ep) {
    const auto start = lexer.previous_position;
    const auto start_delegate_1 = lexer.previous_position;
    const auto success_delegate_1 = lexer.parse_keyword(*keywords_index.get(7));
    if (success_delegate_1 == false) {
        return Result<DelegateSyntax, ParserError>(ParserError(DifferentSyntax()));
    };
    const auto parameters_start = lexer.position;
    ParameterSetSyntax* parameters = nullptr;
    {
        auto _result = parse_parameterset(rp, ep);
        switch (_result._tag)
        {
            case Success::Error:
            {
                auto error = _result._Error;
                {
                    {
                        auto _result = error;
                        switch (_result._tag)
                        {
                            case ParserError::Different:
                            {
                                auto d = _result._Different;
                                {
                                };
                                break;
                            }
                            case ParserError::Invalid:
                            {
                                auto i = _result._Invalid;
                                return Result<DelegateSyntax, ParserError>(i);
                                break;
                            }
                        }
                    };
                };
                break;
            }
            case Success::Ok:
            {
                auto success = _result._Ok;
                parameters = new (alignof(ParameterSetSyntax), rp) ParameterSetSyntax(success);
                break;
            }
        }
    };
    const auto attributes_start = lexer.position;
    Vector<AttributeSyntax>* attributes = nullptr;
    {
        auto _result = parse_attribute_list(rp, ep);
        switch (_result._tag)
        {
            case Success::Error:
            {
                auto error = _result._Error;
                {
                    {
                        auto _result = error;
                        switch (_result._tag)
                        {
                            case ParserError::Different:
                            {
                                auto d = _result._Different;
                                {
                                };
                                break;
                            }
                            case ParserError::Invalid:
                            {
                                auto i = _result._Invalid;
                                return Result<DelegateSyntax, ParserError>(i);
                                break;
                            }
                        }
                    };
                };
                break;
            }
            case Success::Ok:
            {
                auto success = _result._Ok;
                attributes = success;
                break;
            }
        }
    };
    const auto result_start = lexer.position;
    ReturnsSyntax* result = nullptr;
    {
        auto _result = parse_returns(rp, ep);
        switch (_result._tag)
        {
            case Success::Error:
            {
                auto error = _result._Error;
                {
                    {
                        auto _result = error;
                        switch (_result._tag)
                        {
                            case ParserError::Different:
                            {
                                auto d = _result._Different;
                                {
                                };
                                break;
                            }
                            case ParserError::Invalid:
                            {
                                auto i = _result._Invalid;
                                return Result<DelegateSyntax, ParserError>(i);
                                break;
                            }
                        }
                    };
                };
                break;
            }
            case Success::Ok:
            {
                auto success = _result._Ok;
                result = new (alignof(ReturnsSyntax), rp) ReturnsSyntax(success);
                break;
            }
        }
    };
    const auto error_start = lexer.position;
    ThrowsSyntax* error = nullptr;
    {
        auto _result = parse_throws(rp, ep);
        switch (_result._tag)
        {
            case Success::Error:
            {
                auto error = _result._Error;
                {
                    {
                        auto _result = error;
                        switch (_result._tag)
                        {
                            case ParserError::Different:
                            {
                                auto d = _result._Different;
                                {
                                };
                                break;
                            }
                            case ParserError::Invalid:
                            {
                                auto i = _result._Invalid;
                                return Result<DelegateSyntax, ParserError>(i);
                                break;
                            }
                        }
                    };
                };
                break;
            }
            case Success::Ok:
            {
                auto success = _result._Ok;
                error = new (alignof(ThrowsSyntax), rp) ThrowsSyntax(success);
                break;
            }
        }
    };
    const auto end = lexer.position;
    return Result<DelegateSyntax, ParserError>(DelegateSyntax(start, end, parameters, attributes, result, error));
}

Result<GenericArgumentsSyntax, ParserError> Parser::parse_genericarguments(Page* rp, Page* ep) {
    const auto start = lexer.previous_position;
    const auto start_left_bracket_1 = lexer.previous_position;
    const auto success_left_bracket_1 = lexer.parse_punctuation('[');
    if (success_left_bracket_1 == false) {
        return Result<GenericArgumentsSyntax, ParserError>(ParserError(DifferentSyntax()));
    };
    const auto generics_start = lexer.position;
    Vector<GenericArgumentSyntax>* generics = nullptr;
    {
        auto _result = parse_genericargument_list(rp, ep);
        switch (_result._tag)
        {
            case Success::Error:
            {
                auto error = _result._Error;
                {
                    {
                        auto _result = error;
                        switch (_result._tag)
                        {
                            case ParserError::Different:
                            {
                                auto d = _result._Different;
                                {
                                };
                                break;
                            }
                            case ParserError::Invalid:
                            {
                                auto i = _result._Invalid;
                                return Result<GenericArgumentsSyntax, ParserError>(i);
                                break;
                            }
                        }
                    };
                };
                break;
            }
            case Success::Ok:
            {
                auto success = _result._Ok;
                generics = success;
                break;
            }
        }
    };
    const auto start_right_bracket_3 = lexer.previous_position;
    const auto success_right_bracket_3 = lexer.parse_punctuation(']');
    if (success_right_bracket_3 == false) {
        return Result<GenericArgumentsSyntax, ParserError>(ParserError(InvalidSyntax(start_right_bracket_3, lexer.position, String(ep, "]"))));
    };
    const auto end = lexer.position;
    return Result<GenericArgumentsSyntax, ParserError>(GenericArgumentsSyntax(start, end, generics));
}

Result<Vector<GenericArgumentSyntax>*, ParserError> Parser::parse_genericargument_list(Page* rp, Page* ep) {
    auto r = Region();
    auto list = new (alignof(List<GenericArgumentSyntax>), r.get_page()) List<GenericArgumentSyntax>();
    while (true) {
        const auto _node_result = parse_genericargument(rp, ep);
        auto node = _node_result._Ok;
        if (_node_result._tag == Success::Error) {
            const auto _node_Error = _node_result._Error;
            switch (_node_Error._tag) {
                case ParserError::Invalid: {
                    const auto i = _node_Error._Invalid;
                    return Result<Vector<GenericArgumentSyntax>*, ParserError>(i);
                    break;
                }
                case ParserError::Different: {
                    const auto d = _node_Error._Different;
                    {
                        if ((*list).head == 0) {
                            return Result<Vector<GenericArgumentSyntax>*, ParserError>(d);
                        }
                        else {
                            return Result<Vector<GenericArgumentSyntax>*, ParserError>(new (alignof(Vector<GenericArgumentSyntax>), rp) Vector<GenericArgumentSyntax>(rp, *list));
                        };
                    };
                    break;
                }
            }
        }
        (*list).add(node);
    };
}

Result<GenericArgumentSyntax, ParserError> Parser::parse_genericargument(Page* rp, Page* ep) {
    const auto start = lexer.previous_position;
    const auto type_start = lexer.position;
    const auto _type_result = parse_type(rp, ep);
    auto type = _type_result._Ok;
    if (_type_result._tag == Success::Error) {
        const auto _type_Error = _type_result._Error;
        switch (_type_Error._tag) {
            case ParserError::Different: {
                const auto d = _type_Error._Different;
                return Result<GenericArgumentSyntax, ParserError>(d);
                break;
            }
            case ParserError::Invalid: {
                const auto i = _type_Error._Invalid;
                return Result<GenericArgumentSyntax, ParserError>(i);
                break;
            }
        }
    }
    ;
    const auto start_comma_2 = lexer.previous_position;
    const auto success_comma_2 = lexer.parse_punctuation(',');
    const auto end = lexer.position;
    return Result<GenericArgumentSyntax, ParserError>(GenericArgumentSyntax(start, end, type));
}

Result<OptionalSyntax, ParserError> Parser::parse_optional(Page* rp, Page* ep) {
    const auto start = lexer.previous_position;
    const auto start_question_1 = lexer.previous_position;
    const auto success_question_1 = lexer.parse_punctuation('?');
    if (success_question_1 == false) {
        return Result<OptionalSyntax, ParserError>(ParserError(DifferentSyntax()));
    };
    const auto end = lexer.position;
    return Result<OptionalSyntax, ParserError>(OptionalSyntax(start, end));
}

Result<ParameterSetSyntax, ParserError> Parser::parse_parameterset(Page* rp, Page* ep) {
    {
        {
            auto _result = parse_parameters(rp, ep);
            switch (_result._tag)
            {
                case Success::Error:
                {
                    auto error = _result._Error;
                    {
                        {
                            auto _result = error;
                            switch (_result._tag)
                            {
                                case ParserError::Invalid:
                                {
                                    auto i = _result._Invalid;
                                    return Result<ParameterSetSyntax, ParserError>(i);
                                    break;
                                }
                                case ParserError::Different:
                                {
                                    auto d = _result._Different;
                                    {
                                    };
                                    break;
                                }
                            }
                        };
                    };
                    break;
                }
                case Success::Ok:
                {
                    auto node = _result._Ok;
                    return Result<ParameterSetSyntax, ParserError>(ParameterSetSyntax(ParametersSyntax(node)));
                    break;
                }
            }
        };
    };
    {
        {
            auto _result = parse_type(rp, ep);
            switch (_result._tag)
            {
                case Success::Error:
                {
                    auto error = _result._Error;
                    {
                        {
                            auto _result = error;
                            switch (_result._tag)
                            {
                                case ParserError::Invalid:
                                {
                                    auto i = _result._Invalid;
                                    return Result<ParameterSetSyntax, ParserError>(i);
                                    break;
                                }
                                case ParserError::Different:
                                {
                                    auto d = _result._Different;
                                    {
                                    };
                                    break;
                                }
                            }
                        };
                    };
                    break;
                }
                case Success::Ok:
                {
                    auto node = _result._Ok;
                    return Result<ParameterSetSyntax, ParserError>(ParameterSetSyntax(TypeSyntax(node)));
                    break;
                }
            }
        };
    };
    return Result<ParameterSetSyntax, ParserError>(ParserError(DifferentSyntax()));
}

Result<ParametersSyntax, ParserError> Parser::parse_parameters(Page* rp, Page* ep) {
    const auto start = lexer.previous_position;
    const auto start_left_paren_1 = lexer.previous_position;
    const auto success_left_paren_1 = lexer.parse_punctuation('(');
    if (success_left_paren_1 == false) {
        return Result<ParametersSyntax, ParserError>(ParserError(DifferentSyntax()));
    };
    const auto items_start = lexer.position;
    Vector<ItemSyntax>* items = nullptr;
    {
        auto _result = parse_item_list(rp, ep);
        switch (_result._tag)
        {
            case Success::Error:
            {
                auto error = _result._Error;
                {
                    {
                        auto _result = error;
                        switch (_result._tag)
                        {
                            case ParserError::Different:
                            {
                                auto d = _result._Different;
                                {
                                };
                                break;
                            }
                            case ParserError::Invalid:
                            {
                                auto i = _result._Invalid;
                                return Result<ParametersSyntax, ParserError>(i);
                                break;
                            }
                        }
                    };
                };
                break;
            }
            case Success::Ok:
            {
                auto success = _result._Ok;
                items = success;
                break;
            }
        }
    };
    const auto start_right_paren_3 = lexer.previous_position;
    const auto success_right_paren_3 = lexer.parse_punctuation(')');
    if (success_right_paren_3 == false) {
        return Result<ParametersSyntax, ParserError>(ParserError(InvalidSyntax(start_right_paren_3, lexer.position, String(ep, ")"))));
    };
    const auto end = lexer.position;
    return Result<ParametersSyntax, ParserError>(ParametersSyntax(start, end, items));
}

Result<Vector<ItemSyntax>*, ParserError> Parser::parse_item_list(Page* rp, Page* ep) {
    auto r = Region();
    auto list = new (alignof(List<ItemSyntax>), r.get_page()) List<ItemSyntax>();
    while (true) {
        const auto _node_result = parse_item(rp, ep);
        auto node = _node_result._Ok;
        if (_node_result._tag == Success::Error) {
            const auto _node_Error = _node_result._Error;
            switch (_node_Error._tag) {
                case ParserError::Invalid: {
                    const auto i = _node_Error._Invalid;
                    return Result<Vector<ItemSyntax>*, ParserError>(i);
                    break;
                }
                case ParserError::Different: {
                    const auto d = _node_Error._Different;
                    {
                        if ((*list).head == 0) {
                            return Result<Vector<ItemSyntax>*, ParserError>(d);
                        }
                        else {
                            return Result<Vector<ItemSyntax>*, ParserError>(new (alignof(Vector<ItemSyntax>), rp) Vector<ItemSyntax>(rp, *list));
                        };
                    };
                    break;
                }
            }
        }
        (*list).add(node);
    };
}

Result<ItemSyntax, ParserError> Parser::parse_item(Page* rp, Page* ep) {
    const auto start = lexer.previous_position;
    const auto start_name = lexer.previous_position;
    const auto name = lexer.parse_identifier(rp, keywords);
    if (name == nullptr) {
        return Result<ItemSyntax, ParserError>(ParserError(DifferentSyntax()));
    };
    const auto annotation_start = lexer.position;
    TypeAnnotationSyntax* annotation = nullptr;
    {
        auto _result = parse_typeannotation(rp, ep);
        switch (_result._tag)
        {
            case Success::Error:
            {
                auto error = _result._Error;
                {
                    {
                        auto _result = error;
                        switch (_result._tag)
                        {
                            case ParserError::Different:
                            {
                                auto d = _result._Different;
                                {
                                };
                                break;
                            }
                            case ParserError::Invalid:
                            {
                                auto i = _result._Invalid;
                                return Result<ItemSyntax, ParserError>(i);
                                break;
                            }
                        }
                    };
                };
                break;
            }
            case Success::Ok:
            {
                auto success = _result._Ok;
                annotation = new (alignof(TypeAnnotationSyntax), rp) TypeAnnotationSyntax(success);
                break;
            }
        }
    };
    const auto start_comma_3 = lexer.previous_position;
    const auto success_comma_3 = lexer.parse_punctuation(',');
    const auto start_colon_4 = lexer.previous_position;
    const auto success_colon_4 = lexer.parse_colon();
    const auto attributes_start = lexer.position;
    Vector<AttributeSyntax>* attributes = nullptr;
    {
        auto _result = parse_attribute_list(rp, ep);
        switch (_result._tag)
        {
            case Success::Error:
            {
                auto error = _result._Error;
                {
                    {
                        auto _result = error;
                        switch (_result._tag)
                        {
                            case ParserError::Different:
                            {
                                auto d = _result._Different;
                                {
                                };
                                break;
                            }
                            case ParserError::Invalid:
                            {
                                auto i = _result._Invalid;
                                return Result<ItemSyntax, ParserError>(i);
                                break;
                            }
                        }
                    };
                };
                break;
            }
            case Success::Ok:
            {
                auto success = _result._Ok;
                attributes = success;
                break;
            }
        }
    };
    const auto start_colon_6 = lexer.previous_position;
    const auto success_colon_6 = lexer.parse_colon();
    const auto end = lexer.position;
    return Result<ItemSyntax, ParserError>(ItemSyntax(start, end, *name, annotation, attributes));
}

Result<ReturnsSyntax, ParserError> Parser::parse_returns(Page* rp, Page* ep) {
    const auto start = lexer.previous_position;
    const auto start_returns_1 = lexer.previous_position;
    const auto success_returns_1 = lexer.parse_keyword(*keywords_index.get(31));
    if (success_returns_1 == false) {
        return Result<ReturnsSyntax, ParserError>(ParserError(DifferentSyntax()));
    };
    const auto type_start = lexer.position;
    const auto _type_result = parse_type(rp, ep);
    auto type = _type_result._Ok;
    if (_type_result._tag == Success::Error) {
        const auto _type_Error = _type_result._Error;
        switch (_type_Error._tag) {
            case ParserError::Different: {
                const auto d = _type_Error._Different;
                return Result<ReturnsSyntax, ParserError>(InvalidSyntax(type_start, lexer.position, String(ep, "a valid Type syntax")));
                break;
            }
            case ParserError::Invalid: {
                const auto i = _type_Error._Invalid;
                return Result<ReturnsSyntax, ParserError>(i);
                break;
            }
        }
    }
    ;
    const auto attributes_start = lexer.position;
    Vector<AttributeSyntax>* attributes = nullptr;
    {
        auto _result = parse_attribute_list(rp, ep);
        switch (_result._tag)
        {
            case Success::Error:
            {
                auto error = _result._Error;
                {
                    {
                        auto _result = error;
                        switch (_result._tag)
                        {
                            case ParserError::Different:
                            {
                                auto d = _result._Different;
                                {
                                };
                                break;
                            }
                            case ParserError::Invalid:
                            {
                                auto i = _result._Invalid;
                                return Result<ReturnsSyntax, ParserError>(i);
                                break;
                            }
                        }
                    };
                };
                break;
            }
            case Success::Ok:
            {
                auto success = _result._Ok;
                attributes = success;
                break;
            }
        }
    };
    const auto end = lexer.position;
    return Result<ReturnsSyntax, ParserError>(ReturnsSyntax(start, end, type, attributes));
}

Result<ThrowsSyntax, ParserError> Parser::parse_throws(Page* rp, Page* ep) {
    const auto start = lexer.previous_position;
    const auto start_throws_1 = lexer.previous_position;
    const auto success_throws_1 = lexer.parse_keyword(*keywords_index.get(36));
    if (success_throws_1 == false) {
        return Result<ThrowsSyntax, ParserError>(ParserError(DifferentSyntax()));
    };
    const auto type_start = lexer.position;
    const auto _type_result = parse_type(rp, ep);
    auto type = _type_result._Ok;
    if (_type_result._tag == Success::Error) {
        const auto _type_Error = _type_result._Error;
        switch (_type_Error._tag) {
            case ParserError::Different: {
                const auto d = _type_Error._Different;
                return Result<ThrowsSyntax, ParserError>(InvalidSyntax(type_start, lexer.position, String(ep, "a valid Type syntax")));
                break;
            }
            case ParserError::Invalid: {
                const auto i = _type_Error._Invalid;
                return Result<ThrowsSyntax, ParserError>(i);
                break;
            }
        }
    }
    ;
    const auto attributes_start = lexer.position;
    Vector<AttributeSyntax>* attributes = nullptr;
    {
        auto _result = parse_attribute_list(rp, ep);
        switch (_result._tag)
        {
            case Success::Error:
            {
                auto error = _result._Error;
                {
                    {
                        auto _result = error;
                        switch (_result._tag)
                        {
                            case ParserError::Different:
                            {
                                auto d = _result._Different;
                                {
                                };
                                break;
                            }
                            case ParserError::Invalid:
                            {
                                auto i = _result._Invalid;
                                return Result<ThrowsSyntax, ParserError>(i);
                                break;
                            }
                        }
                    };
                };
                break;
            }
            case Success::Ok:
            {
                auto success = _result._Ok;
                attributes = success;
                break;
            }
        }
    };
    const auto end = lexer.position;
    return Result<ThrowsSyntax, ParserError>(ThrowsSyntax(start, end, type, attributes));
}

Result<Vector<UseSyntax>*, ParserError> Parser::parse_use_list(Page* rp, Page* ep) {
    auto r = Region();
    auto list = new (alignof(List<UseSyntax>), r.get_page()) List<UseSyntax>();
    while (true) {
        const auto _node_result = parse_use(rp, ep);
        auto node = _node_result._Ok;
        if (_node_result._tag == Success::Error) {
            const auto _node_Error = _node_result._Error;
            switch (_node_Error._tag) {
                case ParserError::Invalid: {
                    const auto i = _node_Error._Invalid;
                    return Result<Vector<UseSyntax>*, ParserError>(i);
                    break;
                }
                case ParserError::Different: {
                    const auto d = _node_Error._Different;
                    {
                        if ((*list).head == 0) {
                            return Result<Vector<UseSyntax>*, ParserError>(d);
                        }
                        else {
                            return Result<Vector<UseSyntax>*, ParserError>(new (alignof(Vector<UseSyntax>), rp) Vector<UseSyntax>(rp, *list));
                        };
                    };
                    break;
                }
            }
        }
        (*list).add(node);
    };
}

Result<UseSyntax, ParserError> Parser::parse_use(Page* rp, Page* ep) {
    const auto start = lexer.previous_position;
    const auto start_use_1 = lexer.previous_position;
    const auto success_use_1 = lexer.parse_keyword(*keywords_index.get(40));
    if (success_use_1 == false) {
        return Result<UseSyntax, ParserError>(ParserError(DifferentSyntax()));
    };
    const auto name_start = lexer.position;
    const auto _name_result = parse_name(rp, ep);
    auto name = _name_result._Ok;
    if (_name_result._tag == Success::Error) {
        const auto _name_Error = _name_result._Error;
        switch (_name_Error._tag) {
            case ParserError::Different: {
                const auto d = _name_Error._Different;
                return Result<UseSyntax, ParserError>(InvalidSyntax(name_start, lexer.position, String(ep, "a valid Name syntax")));
                break;
            }
            case ParserError::Invalid: {
                const auto i = _name_Error._Invalid;
                return Result<UseSyntax, ParserError>(i);
                break;
            }
        }
    }
    ;
    const auto start_colon_3 = lexer.previous_position;
    const auto success_colon_3 = lexer.parse_colon();
    const auto end = lexer.position;
    return Result<UseSyntax, ParserError>(UseSyntax(start, end, name));
}

Result<ImplementSyntax, ParserError> Parser::parse_implement(Page* rp, Page* ep) {
    const auto start = lexer.previous_position;
    const auto start_implement_1 = lexer.previous_position;
    const auto success_implement_1 = lexer.parse_keyword(*keywords_index.get(15));
    if (success_implement_1 == false) {
        return Result<ImplementSyntax, ParserError>(ParserError(DifferentSyntax()));
    };
    const auto type_start = lexer.position;
    const auto _type_result = parse_type(rp, ep);
    auto type = _type_result._Ok;
    if (_type_result._tag == Success::Error) {
        const auto _type_Error = _type_result._Error;
        switch (_type_Error._tag) {
            case ParserError::Different: {
                const auto d = _type_Error._Different;
                return Result<ImplementSyntax, ParserError>(InvalidSyntax(type_start, lexer.position, String(ep, "a valid Type syntax")));
                break;
            }
            case ParserError::Invalid: {
                const auto i = _type_Error._Invalid;
                return Result<ImplementSyntax, ParserError>(i);
                break;
            }
        }
    }
    ;
    const auto attributes_start = lexer.position;
    Vector<AttributeSyntax>* attributes = nullptr;
    {
        auto _result = parse_attribute_list(rp, ep);
        switch (_result._tag)
        {
            case Success::Error:
            {
                auto error = _result._Error;
                {
                    {
                        auto _result = error;
                        switch (_result._tag)
                        {
                            case ParserError::Different:
                            {
                                auto d = _result._Different;
                                {
                                };
                                break;
                            }
                            case ParserError::Invalid:
                            {
                                auto i = _result._Invalid;
                                return Result<ImplementSyntax, ParserError>(i);
                                break;
                            }
                        }
                    };
                };
                break;
            }
            case Success::Ok:
            {
                auto success = _result._Ok;
                attributes = success;
                break;
            }
        }
    };
    const auto start_colon_4 = lexer.previous_position;
    const auto success_colon_4 = lexer.parse_colon();
    const auto start_left_curly_5 = lexer.previous_position;
    const auto success_left_curly_5 = lexer.parse_punctuation('{');
    if (success_left_curly_5 == false) {
        return Result<ImplementSyntax, ParserError>(ParserError(InvalidSyntax(start_left_curly_5, lexer.position, String(ep, "{"))));
    };
    const auto uses_start = lexer.position;
    Vector<UseSyntax>* uses = nullptr;
    {
        auto _result = parse_use_list(rp, ep);
        switch (_result._tag)
        {
            case Success::Error:
            {
                auto error = _result._Error;
                {
                    {
                        auto _result = error;
                        switch (_result._tag)
                        {
                            case ParserError::Different:
                            {
                                auto d = _result._Different;
                                {
                                };
                                break;
                            }
                            case ParserError::Invalid:
                            {
                                auto i = _result._Invalid;
                                return Result<ImplementSyntax, ParserError>(i);
                                break;
                            }
                        }
                    };
                };
                break;
            }
            case Success::Ok:
            {
                auto success = _result._Ok;
                uses = success;
                break;
            }
        }
    };
    const auto methods_start = lexer.position;
    Vector<MethodSyntax>* methods = nullptr;
    {
        auto _result = parse_method_list(rp, ep);
        switch (_result._tag)
        {
            case Success::Error:
            {
                auto error = _result._Error;
                {
                    {
                        auto _result = error;
                        switch (_result._tag)
                        {
                            case ParserError::Different:
                            {
                                auto d = _result._Different;
                                {
                                };
                                break;
                            }
                            case ParserError::Invalid:
                            {
                                auto i = _result._Invalid;
                                return Result<ImplementSyntax, ParserError>(i);
                                break;
                            }
                        }
                    };
                };
                break;
            }
            case Success::Ok:
            {
                auto success = _result._Ok;
                methods = success;
                break;
            }
        }
    };
    const auto start_right_curly_8 = lexer.previous_position;
    const auto success_right_curly_8 = lexer.parse_punctuation('}');
    if (success_right_curly_8 == false) {
        return Result<ImplementSyntax, ParserError>(ParserError(InvalidSyntax(start_right_curly_8, lexer.position, String(ep, "}"))));
    };
    const auto start_colon_9 = lexer.previous_position;
    const auto success_colon_9 = lexer.parse_colon();
    const auto end = lexer.position;
    return Result<ImplementSyntax, ParserError>(ImplementSyntax(start, end, type, attributes, uses, methods));
}

Result<TraitSyntax, ParserError> Parser::parse_trait(Page* rp, Page* ep) {
    const auto start = lexer.previous_position;
    const auto start_trait_1 = lexer.previous_position;
    const auto success_trait_1 = lexer.parse_keyword(*keywords_index.get(37));
    if (success_trait_1 == false) {
        return Result<TraitSyntax, ParserError>(ParserError(DifferentSyntax()));
    };
    const auto name_start = lexer.position;
    const auto _name_result = parse_name(rp, ep);
    auto name = _name_result._Ok;
    if (_name_result._tag == Success::Error) {
        const auto _name_Error = _name_result._Error;
        switch (_name_Error._tag) {
            case ParserError::Different: {
                const auto d = _name_Error._Different;
                return Result<TraitSyntax, ParserError>(InvalidSyntax(name_start, lexer.position, String(ep, "a valid Name syntax")));
                break;
            }
            case ParserError::Invalid: {
                const auto i = _name_Error._Invalid;
                return Result<TraitSyntax, ParserError>(i);
                break;
            }
        }
    }
    ;
    const auto extension_start = lexer.position;
    ExtendsSyntax* extension = nullptr;
    {
        auto _result = parse_extends(rp, ep);
        switch (_result._tag)
        {
            case Success::Error:
            {
                auto error = _result._Error;
                {
                    {
                        auto _result = error;
                        switch (_result._tag)
                        {
                            case ParserError::Different:
                            {
                                auto d = _result._Different;
                                {
                                };
                                break;
                            }
                            case ParserError::Invalid:
                            {
                                auto i = _result._Invalid;
                                return Result<TraitSyntax, ParserError>(i);
                                break;
                            }
                        }
                    };
                };
                break;
            }
            case Success::Ok:
            {
                auto success = _result._Ok;
                extension = new (alignof(ExtendsSyntax), rp) ExtendsSyntax(success);
                break;
            }
        }
    };
    const auto attributes_start = lexer.position;
    Vector<AttributeSyntax>* attributes = nullptr;
    {
        auto _result = parse_attribute_list(rp, ep);
        switch (_result._tag)
        {
            case Success::Error:
            {
                auto error = _result._Error;
                {
                    {
                        auto _result = error;
                        switch (_result._tag)
                        {
                            case ParserError::Different:
                            {
                                auto d = _result._Different;
                                {
                                };
                                break;
                            }
                            case ParserError::Invalid:
                            {
                                auto i = _result._Invalid;
                                return Result<TraitSyntax, ParserError>(i);
                                break;
                            }
                        }
                    };
                };
                break;
            }
            case Success::Ok:
            {
                auto success = _result._Ok;
                attributes = success;
                break;
            }
        }
    };
    const auto start_left_curly_5 = lexer.previous_position;
    const auto success_left_curly_5 = lexer.parse_punctuation('{');
    if (success_left_curly_5 == false) {
        return Result<TraitSyntax, ParserError>(ParserError(InvalidSyntax(start_left_curly_5, lexer.position, String(ep, "{"))));
    };
    const auto uses_start = lexer.position;
    Vector<UseSyntax>* uses = nullptr;
    {
        auto _result = parse_use_list(rp, ep);
        switch (_result._tag)
        {
            case Success::Error:
            {
                auto error = _result._Error;
                {
                    {
                        auto _result = error;
                        switch (_result._tag)
                        {
                            case ParserError::Different:
                            {
                                auto d = _result._Different;
                                {
                                };
                                break;
                            }
                            case ParserError::Invalid:
                            {
                                auto i = _result._Invalid;
                                return Result<TraitSyntax, ParserError>(i);
                                break;
                            }
                        }
                    };
                };
                break;
            }
            case Success::Ok:
            {
                auto success = _result._Ok;
                uses = success;
                break;
            }
        }
    };
    const auto functions_start = lexer.position;
    Vector<MethodSyntax>* functions = nullptr;
    {
        auto _result = parse_method_list(rp, ep);
        switch (_result._tag)
        {
            case Success::Error:
            {
                auto error = _result._Error;
                {
                    {
                        auto _result = error;
                        switch (_result._tag)
                        {
                            case ParserError::Different:
                            {
                                auto d = _result._Different;
                                {
                                };
                                break;
                            }
                            case ParserError::Invalid:
                            {
                                auto i = _result._Invalid;
                                return Result<TraitSyntax, ParserError>(i);
                                break;
                            }
                        }
                    };
                };
                break;
            }
            case Success::Ok:
            {
                auto success = _result._Ok;
                functions = success;
                break;
            }
        }
    };
    const auto start_right_curly_8 = lexer.previous_position;
    const auto success_right_curly_8 = lexer.parse_punctuation('}');
    if (success_right_curly_8 == false) {
        return Result<TraitSyntax, ParserError>(ParserError(InvalidSyntax(start_right_curly_8, lexer.position, String(ep, "}"))));
    };
    const auto start_colon_9 = lexer.previous_position;
    const auto success_colon_9 = lexer.parse_colon();
    const auto end = lexer.position;
    return Result<TraitSyntax, ParserError>(TraitSyntax(start, end, name, extension, attributes, uses, functions));
}

Result<Vector<MethodSyntax>*, ParserError> Parser::parse_method_list(Page* rp, Page* ep) {
    auto r = Region();
    auto list = new (alignof(List<MethodSyntax>), r.get_page()) List<MethodSyntax>();
    while (true) {
        const auto _node_result = parse_method(rp, ep);
        auto node = _node_result._Ok;
        if (_node_result._tag == Success::Error) {
            const auto _node_Error = _node_result._Error;
            switch (_node_Error._tag) {
                case ParserError::Invalid: {
                    const auto i = _node_Error._Invalid;
                    return Result<Vector<MethodSyntax>*, ParserError>(i);
                    break;
                }
                case ParserError::Different: {
                    const auto d = _node_Error._Different;
                    {
                        if ((*list).head == 0) {
                            return Result<Vector<MethodSyntax>*, ParserError>(d);
                        }
                        else {
                            return Result<Vector<MethodSyntax>*, ParserError>(new (alignof(Vector<MethodSyntax>), rp) Vector<MethodSyntax>(rp, *list));
                        };
                    };
                    break;
                }
            }
        }
        (*list).add(node);
    };
}

Result<MethodSyntax, ParserError> Parser::parse_method(Page* rp, Page* ep) {
    {
        {
            auto _result = parse_function(rp, ep);
            switch (_result._tag)
            {
                case Success::Error:
                {
                    auto error = _result._Error;
                    {
                        {
                            auto _result = error;
                            switch (_result._tag)
                            {
                                case ParserError::Invalid:
                                {
                                    auto i = _result._Invalid;
                                    return Result<MethodSyntax, ParserError>(i);
                                    break;
                                }
                                case ParserError::Different:
                                {
                                    auto d = _result._Different;
                                    {
                                    };
                                    break;
                                }
                            }
                        };
                    };
                    break;
                }
                case Success::Ok:
                {
                    auto node = _result._Ok;
                    return Result<MethodSyntax, ParserError>(MethodSyntax(FunctionSyntax(node)));
                    break;
                }
            }
        };
    };
    {
        {
            auto _result = parse_procedure(rp, ep);
            switch (_result._tag)
            {
                case Success::Error:
                {
                    auto error = _result._Error;
                    {
                        {
                            auto _result = error;
                            switch (_result._tag)
                            {
                                case ParserError::Invalid:
                                {
                                    auto i = _result._Invalid;
                                    return Result<MethodSyntax, ParserError>(i);
                                    break;
                                }
                                case ParserError::Different:
                                {
                                    auto d = _result._Different;
                                    {
                                    };
                                    break;
                                }
                            }
                        };
                    };
                    break;
                }
                case Success::Ok:
                {
                    auto node = _result._Ok;
                    return Result<MethodSyntax, ParserError>(MethodSyntax(ProcedureSyntax(node)));
                    break;
                }
            }
        };
    };
    {
        {
            auto _result = parse_operator(rp, ep);
            switch (_result._tag)
            {
                case Success::Error:
                {
                    auto error = _result._Error;
                    {
                        {
                            auto _result = error;
                            switch (_result._tag)
                            {
                                case ParserError::Invalid:
                                {
                                    auto i = _result._Invalid;
                                    return Result<MethodSyntax, ParserError>(i);
                                    break;
                                }
                                case ParserError::Different:
                                {
                                    auto d = _result._Different;
                                    {
                                    };
                                    break;
                                }
                            }
                        };
                    };
                    break;
                }
                case Success::Ok:
                {
                    auto node = _result._Ok;
                    return Result<MethodSyntax, ParserError>(MethodSyntax(OperatorSyntax(node)));
                    break;
                }
            }
        };
    };
    return Result<MethodSyntax, ParserError>(ParserError(DifferentSyntax()));
}

Result<Vector<InitSyntax>*, ParserError> Parser::parse_init_list(Page* rp, Page* ep) {
    auto r = Region();
    auto list = new (alignof(List<InitSyntax>), r.get_page()) List<InitSyntax>();
    while (true) {
        const auto _node_result = parse_init(rp, ep);
        auto node = _node_result._Ok;
        if (_node_result._tag == Success::Error) {
            const auto _node_Error = _node_result._Error;
            switch (_node_Error._tag) {
                case ParserError::Invalid: {
                    const auto i = _node_Error._Invalid;
                    return Result<Vector<InitSyntax>*, ParserError>(i);
                    break;
                }
                case ParserError::Different: {
                    const auto d = _node_Error._Different;
                    {
                        if ((*list).head == 0) {
                            return Result<Vector<InitSyntax>*, ParserError>(d);
                        }
                        else {
                            return Result<Vector<InitSyntax>*, ParserError>(new (alignof(Vector<InitSyntax>), rp) Vector<InitSyntax>(rp, *list));
                        };
                    };
                    break;
                }
            }
        }
        (*list).add(node);
    };
}

Result<InitSyntax, ParserError> Parser::parse_init(Page* rp, Page* ep) {
    const auto start = lexer.previous_position;
    const auto start_init_1 = lexer.previous_position;
    const auto success_init_1 = lexer.parse_keyword(*keywords_index.get(14));
    if (success_init_1 == false) {
        return Result<InitSyntax, ParserError>(ParserError(DifferentSyntax()));
    };
    const auto parameters_start = lexer.position;
    ParameterSetSyntax* parameters = nullptr;
    {
        auto _result = parse_parameterset(rp, ep);
        switch (_result._tag)
        {
            case Success::Error:
            {
                auto error = _result._Error;
                {
                    {
                        auto _result = error;
                        switch (_result._tag)
                        {
                            case ParserError::Different:
                            {
                                auto d = _result._Different;
                                {
                                };
                                break;
                            }
                            case ParserError::Invalid:
                            {
                                auto i = _result._Invalid;
                                return Result<InitSyntax, ParserError>(i);
                                break;
                            }
                        }
                    };
                };
                break;
            }
            case Success::Ok:
            {
                auto success = _result._Ok;
                parameters = new (alignof(ParameterSetSyntax), rp) ParameterSetSyntax(success);
                break;
            }
        }
    };
    const auto start_colon_3 = lexer.previous_position;
    const auto success_colon_3 = lexer.parse_colon();
    const auto action_start = lexer.position;
    const auto _action_result = parse_action(rp, ep);
    auto action = _action_result._Ok;
    if (_action_result._tag == Success::Error) {
        const auto _action_Error = _action_result._Error;
        switch (_action_Error._tag) {
            case ParserError::Different: {
                const auto d = _action_Error._Different;
                return Result<InitSyntax, ParserError>(InvalidSyntax(action_start, lexer.position, String(ep, "a valid Action syntax")));
                break;
            }
            case ParserError::Invalid: {
                const auto i = _action_Error._Invalid;
                return Result<InitSyntax, ParserError>(i);
                break;
            }
        }
    }
    ;
    const auto start_colon_5 = lexer.previous_position;
    const auto success_colon_5 = lexer.parse_colon();
    const auto end = lexer.position;
    return Result<InitSyntax, ParserError>(InitSyntax(start, end, parameters, action));
}

Result<DeInitSyntax, ParserError> Parser::parse_deinit(Page* rp, Page* ep) {
    const auto start = lexer.previous_position;
    const auto start_deinit_1 = lexer.previous_position;
    const auto success_deinit_1 = lexer.parse_keyword(*keywords_index.get(6));
    if (success_deinit_1 == false) {
        return Result<DeInitSyntax, ParserError>(ParserError(DifferentSyntax()));
    };
    const auto start_colon_2 = lexer.previous_position;
    const auto success_colon_2 = lexer.parse_colon();
    const auto action_start = lexer.position;
    const auto _action_result = parse_action(rp, ep);
    auto action = _action_result._Ok;
    if (_action_result._tag == Success::Error) {
        const auto _action_Error = _action_result._Error;
        switch (_action_Error._tag) {
            case ParserError::Different: {
                const auto d = _action_Error._Different;
                return Result<DeInitSyntax, ParserError>(InvalidSyntax(action_start, lexer.position, String(ep, "a valid Action syntax")));
                break;
            }
            case ParserError::Invalid: {
                const auto i = _action_Error._Invalid;
                return Result<DeInitSyntax, ParserError>(i);
                break;
            }
        }
    }
    ;
    const auto start_colon_4 = lexer.previous_position;
    const auto success_colon_4 = lexer.parse_colon();
    const auto end = lexer.position;
    return Result<DeInitSyntax, ParserError>(DeInitSyntax(start, end, action));
}

Result<FunctionSyntax, ParserError> Parser::parse_function(Page* rp, Page* ep) {
    const auto start = lexer.previous_position;
    const auto start_function_1 = lexer.previous_position;
    const auto success_function_1 = lexer.parse_keyword(*keywords_index.get(12));
    if (success_function_1 == false) {
        return Result<FunctionSyntax, ParserError>(ParserError(DifferentSyntax()));
    };
    const auto target_start = lexer.position;
    const auto _target_result = parse_target(rp, ep);
    auto target = _target_result._Ok;
    if (_target_result._tag == Success::Error) {
        const auto _target_Error = _target_result._Error;
        switch (_target_Error._tag) {
            case ParserError::Different: {
                const auto d = _target_Error._Different;
                return Result<FunctionSyntax, ParserError>(InvalidSyntax(target_start, lexer.position, String(ep, "a valid Target syntax")));
                break;
            }
            case ParserError::Invalid: {
                const auto i = _target_Error._Invalid;
                return Result<FunctionSyntax, ParserError>(i);
                break;
            }
        }
    }
    ;
    const auto end = lexer.position;
    return Result<FunctionSyntax, ParserError>(FunctionSyntax(start, end, target));
}

Result<ProcedureSyntax, ParserError> Parser::parse_procedure(Page* rp, Page* ep) {
    const auto start = lexer.previous_position;
    const auto start_procedure_1 = lexer.previous_position;
    const auto success_procedure_1 = lexer.parse_keyword(*keywords_index.get(28));
    if (success_procedure_1 == false) {
        return Result<ProcedureSyntax, ParserError>(ParserError(DifferentSyntax()));
    };
    const auto target_start = lexer.position;
    const auto _target_result = parse_target(rp, ep);
    auto target = _target_result._Ok;
    if (_target_result._tag == Success::Error) {
        const auto _target_Error = _target_result._Error;
        switch (_target_Error._tag) {
            case ParserError::Different: {
                const auto d = _target_Error._Different;
                return Result<ProcedureSyntax, ParserError>(InvalidSyntax(target_start, lexer.position, String(ep, "a valid Target syntax")));
                break;
            }
            case ParserError::Invalid: {
                const auto i = _target_Error._Invalid;
                return Result<ProcedureSyntax, ParserError>(i);
                break;
            }
        }
    }
    ;
    const auto end = lexer.position;
    return Result<ProcedureSyntax, ParserError>(ProcedureSyntax(start, end, target));
}

Result<OperatorSyntax, ParserError> Parser::parse_operator(Page* rp, Page* ep) {
    const auto start = lexer.previous_position;
    const auto start_operator_1 = lexer.previous_position;
    const auto success_operator_1 = lexer.parse_keyword(*keywords_index.get(27));
    if (success_operator_1 == false) {
        return Result<OperatorSyntax, ParserError>(ParserError(DifferentSyntax()));
    };
    const auto target_start = lexer.position;
    const auto _target_result = parse_target(rp, ep);
    auto target = _target_result._Ok;
    if (_target_result._tag == Success::Error) {
        const auto _target_Error = _target_result._Error;
        switch (_target_Error._tag) {
            case ParserError::Different: {
                const auto d = _target_Error._Different;
                return Result<OperatorSyntax, ParserError>(InvalidSyntax(target_start, lexer.position, String(ep, "a valid Target syntax")));
                break;
            }
            case ParserError::Invalid: {
                const auto i = _target_Error._Invalid;
                return Result<OperatorSyntax, ParserError>(i);
                break;
            }
        }
    }
    ;
    const auto end = lexer.position;
    return Result<OperatorSyntax, ParserError>(OperatorSyntax(start, end, target));
}

Result<TargetSyntax, ParserError> Parser::parse_target(Page* rp, Page* ep) {
    {
        {
            auto _result = parse_named(rp, ep);
            switch (_result._tag)
            {
                case Success::Error:
                {
                    auto error = _result._Error;
                    {
                        {
                            auto _result = error;
                            switch (_result._tag)
                            {
                                case ParserError::Invalid:
                                {
                                    auto i = _result._Invalid;
                                    return Result<TargetSyntax, ParserError>(i);
                                    break;
                                }
                                case ParserError::Different:
                                {
                                    auto d = _result._Different;
                                    {
                                    };
                                    break;
                                }
                            }
                        };
                    };
                    break;
                }
                case Success::Ok:
                {
                    auto node = _result._Ok;
                    return Result<TargetSyntax, ParserError>(TargetSyntax(NamedSyntax(node)));
                    break;
                }
            }
        };
    };
    {
        {
            auto _result = parse_routine(rp, ep);
            switch (_result._tag)
            {
                case Success::Error:
                {
                    auto error = _result._Error;
                    {
                        {
                            auto _result = error;
                            switch (_result._tag)
                            {
                                case ParserError::Invalid:
                                {
                                    auto i = _result._Invalid;
                                    return Result<TargetSyntax, ParserError>(i);
                                    break;
                                }
                                case ParserError::Different:
                                {
                                    auto d = _result._Different;
                                    {
                                    };
                                    break;
                                }
                            }
                        };
                    };
                    break;
                }
                case Success::Ok:
                {
                    auto node = _result._Ok;
                    return Result<TargetSyntax, ParserError>(TargetSyntax(RoutineSyntax(node)));
                    break;
                }
            }
        };
    };
    return Result<TargetSyntax, ParserError>(ParserError(DifferentSyntax()));
}

Result<NamedSyntax, ParserError> Parser::parse_named(Page* rp, Page* ep) {
    const auto start = lexer.previous_position;
    const auto start_name = lexer.previous_position;
    const auto name = lexer.parse_identifier(rp, keywords);
    if (name == nullptr) {
        return Result<NamedSyntax, ParserError>(ParserError(DifferentSyntax()));
    };
    const auto routine_start = lexer.position;
    const auto _routine_result = parse_routine(rp, ep);
    auto routine = _routine_result._Ok;
    if (_routine_result._tag == Success::Error) {
        const auto _routine_Error = _routine_result._Error;
        switch (_routine_Error._tag) {
            case ParserError::Different: {
                const auto d = _routine_Error._Different;
                return Result<NamedSyntax, ParserError>(InvalidSyntax(routine_start, lexer.position, String(ep, "a valid Routine syntax")));
                break;
            }
            case ParserError::Invalid: {
                const auto i = _routine_Error._Invalid;
                return Result<NamedSyntax, ParserError>(i);
                break;
            }
        }
    }
    ;
    const auto end = lexer.position;
    return Result<NamedSyntax, ParserError>(NamedSyntax(start, end, *name, routine));
}

Result<RoutineSyntax, ParserError> Parser::parse_routine(Page* rp, Page* ep) {
    const auto start = lexer.previous_position;
    const auto generics_start = lexer.position;
    GenericArgumentsSyntax* generics = nullptr;
    {
        auto _result = parse_genericarguments(rp, ep);
        switch (_result._tag)
        {
            case Success::Error:
            {
                auto error = _result._Error;
                {
                    {
                        auto _result = error;
                        switch (_result._tag)
                        {
                            case ParserError::Different:
                            {
                                auto d = _result._Different;
                                {
                                };
                                break;
                            }
                            case ParserError::Invalid:
                            {
                                auto i = _result._Invalid;
                                return Result<RoutineSyntax, ParserError>(i);
                                break;
                            }
                        }
                    };
                };
                break;
            }
            case Success::Ok:
            {
                auto success = _result._Ok;
                generics = new (alignof(GenericArgumentsSyntax), rp) GenericArgumentsSyntax(success);
                break;
            }
        }
    };
    const auto lifetime_start = lexer.position;
    LifetimeSyntax* lifetime = nullptr;
    {
        auto _result = parse_lifetime(rp, ep);
        switch (_result._tag)
        {
            case Success::Error:
            {
                auto error = _result._Error;
                {
                    {
                        auto _result = error;
                        switch (_result._tag)
                        {
                            case ParserError::Different:
                            {
                                auto d = _result._Different;
                                {
                                };
                                break;
                            }
                            case ParserError::Invalid:
                            {
                                auto i = _result._Invalid;
                                return Result<RoutineSyntax, ParserError>(i);
                                break;
                            }
                        }
                    };
                };
                break;
            }
            case Success::Ok:
            {
                auto success = _result._Ok;
                lifetime = new (alignof(LifetimeSyntax), rp) LifetimeSyntax(success);
                break;
            }
        }
    };
    const auto parameters_start = lexer.position;
    ParameterSetSyntax* parameters = nullptr;
    {
        auto _result = parse_parameterset(rp, ep);
        switch (_result._tag)
        {
            case Success::Error:
            {
                auto error = _result._Error;
                {
                    {
                        auto _result = error;
                        switch (_result._tag)
                        {
                            case ParserError::Different:
                            {
                                auto d = _result._Different;
                                {
                                };
                                break;
                            }
                            case ParserError::Invalid:
                            {
                                auto i = _result._Invalid;
                                return Result<RoutineSyntax, ParserError>(i);
                                break;
                            }
                        }
                    };
                };
                break;
            }
            case Success::Ok:
            {
                auto success = _result._Ok;
                parameters = new (alignof(ParameterSetSyntax), rp) ParameterSetSyntax(success);
                break;
            }
        }
    };
    const auto attributes_start = lexer.position;
    Vector<AttributeSyntax>* attributes = nullptr;
    {
        auto _result = parse_attribute_list(rp, ep);
        switch (_result._tag)
        {
            case Success::Error:
            {
                auto error = _result._Error;
                {
                    {
                        auto _result = error;
                        switch (_result._tag)
                        {
                            case ParserError::Different:
                            {
                                auto d = _result._Different;
                                {
                                };
                                break;
                            }
                            case ParserError::Invalid:
                            {
                                auto i = _result._Invalid;
                                return Result<RoutineSyntax, ParserError>(i);
                                break;
                            }
                        }
                    };
                };
                break;
            }
            case Success::Ok:
            {
                auto success = _result._Ok;
                attributes = success;
                break;
            }
        }
    };
    const auto returns__start = lexer.position;
    ReturnsSyntax* returns_ = nullptr;
    {
        auto _result = parse_returns(rp, ep);
        switch (_result._tag)
        {
            case Success::Error:
            {
                auto error = _result._Error;
                {
                    {
                        auto _result = error;
                        switch (_result._tag)
                        {
                            case ParserError::Different:
                            {
                                auto d = _result._Different;
                                {
                                };
                                break;
                            }
                            case ParserError::Invalid:
                            {
                                auto i = _result._Invalid;
                                return Result<RoutineSyntax, ParserError>(i);
                                break;
                            }
                        }
                    };
                };
                break;
            }
            case Success::Ok:
            {
                auto success = _result._Ok;
                returns_ = new (alignof(ReturnsSyntax), rp) ReturnsSyntax(success);
                break;
            }
        }
    };
    const auto start_colon_6 = lexer.previous_position;
    const auto success_colon_6 = lexer.parse_colon();
    const auto throws__start = lexer.position;
    ThrowsSyntax* throws_ = nullptr;
    {
        auto _result = parse_throws(rp, ep);
        switch (_result._tag)
        {
            case Success::Error:
            {
                auto error = _result._Error;
                {
                    {
                        auto _result = error;
                        switch (_result._tag)
                        {
                            case ParserError::Different:
                            {
                                auto d = _result._Different;
                                {
                                };
                                break;
                            }
                            case ParserError::Invalid:
                            {
                                auto i = _result._Invalid;
                                return Result<RoutineSyntax, ParserError>(i);
                                break;
                            }
                        }
                    };
                };
                break;
            }
            case Success::Ok:
            {
                auto success = _result._Ok;
                throws_ = new (alignof(ThrowsSyntax), rp) ThrowsSyntax(success);
                break;
            }
        }
    };
    const auto start_colon_8 = lexer.previous_position;
    const auto success_colon_8 = lexer.parse_colon();
    const auto implementation_start = lexer.position;
    const auto _implementation_result = parse_implementation(rp, ep);
    auto implementation = _implementation_result._Ok;
    if (_implementation_result._tag == Success::Error) {
        const auto _implementation_Error = _implementation_result._Error;
        switch (_implementation_Error._tag) {
            case ParserError::Different: {
                const auto d = _implementation_Error._Different;
                return Result<RoutineSyntax, ParserError>(InvalidSyntax(implementation_start, lexer.position, String(ep, "a valid Implementation syntax")));
                break;
            }
            case ParserError::Invalid: {
                const auto i = _implementation_Error._Invalid;
                return Result<RoutineSyntax, ParserError>(i);
                break;
            }
        }
    }
    ;
    const auto end = lexer.position;
    return Result<RoutineSyntax, ParserError>(RoutineSyntax(start, end, generics, lifetime, parameters, attributes, returns_, throws_, implementation));
}

Result<ImplementationSyntax, ParserError> Parser::parse_implementation(Page* rp, Page* ep) {
    {
        {
            auto _result = parse_action(rp, ep);
            switch (_result._tag)
            {
                case Success::Error:
                {
                    auto error = _result._Error;
                    {
                        {
                            auto _result = error;
                            switch (_result._tag)
                            {
                                case ParserError::Invalid:
                                {
                                    auto i = _result._Invalid;
                                    return Result<ImplementationSyntax, ParserError>(i);
                                    break;
                                }
                                case ParserError::Different:
                                {
                                    auto d = _result._Different;
                                    {
                                    };
                                    break;
                                }
                            }
                        };
                    };
                    break;
                }
                case Success::Ok:
                {
                    auto node = _result._Ok;
                    return Result<ImplementationSyntax, ParserError>(ImplementationSyntax(ActionSyntax(node)));
                    break;
                }
            }
        };
    };
    {
        {
            auto _result = parse_extern(rp, ep);
            switch (_result._tag)
            {
                case Success::Error:
                {
                    auto error = _result._Error;
                    {
                        {
                            auto _result = error;
                            switch (_result._tag)
                            {
                                case ParserError::Invalid:
                                {
                                    auto i = _result._Invalid;
                                    return Result<ImplementationSyntax, ParserError>(i);
                                    break;
                                }
                                case ParserError::Different:
                                {
                                    auto d = _result._Different;
                                    {
                                    };
                                    break;
                                }
                            }
                        };
                    };
                    break;
                }
                case Success::Ok:
                {
                    auto node = _result._Ok;
                    return Result<ImplementationSyntax, ParserError>(ImplementationSyntax(ExternSyntax(node)));
                    break;
                }
            }
        };
    };
    {
        {
            auto _result = parse_instruction(rp, ep);
            switch (_result._tag)
            {
                case Success::Error:
                {
                    auto error = _result._Error;
                    {
                        {
                            auto _result = error;
                            switch (_result._tag)
                            {
                                case ParserError::Invalid:
                                {
                                    auto i = _result._Invalid;
                                    return Result<ImplementationSyntax, ParserError>(i);
                                    break;
                                }
                                case ParserError::Different:
                                {
                                    auto d = _result._Different;
                                    {
                                    };
                                    break;
                                }
                            }
                        };
                    };
                    break;
                }
                case Success::Ok:
                {
                    auto node = _result._Ok;
                    return Result<ImplementationSyntax, ParserError>(ImplementationSyntax(InstructionSyntax(node)));
                    break;
                }
            }
        };
    };
    {
        {
            auto _result = parse_intrinsic(rp, ep);
            switch (_result._tag)
            {
                case Success::Error:
                {
                    auto error = _result._Error;
                    {
                        {
                            auto _result = error;
                            switch (_result._tag)
                            {
                                case ParserError::Invalid:
                                {
                                    auto i = _result._Invalid;
                                    return Result<ImplementationSyntax, ParserError>(i);
                                    break;
                                }
                                case ParserError::Different:
                                {
                                    auto d = _result._Different;
                                    {
                                    };
                                    break;
                                }
                            }
                        };
                    };
                    break;
                }
                case Success::Ok:
                {
                    auto node = _result._Ok;
                    return Result<ImplementationSyntax, ParserError>(ImplementationSyntax(IntrinsicSyntax(node)));
                    break;
                }
            }
        };
    };
    return Result<ImplementationSyntax, ParserError>(ParserError(DifferentSyntax()));
}

Result<ExternSyntax, ParserError> Parser::parse_extern(Page* rp, Page* ep) {
    const auto start = lexer.previous_position;
    const auto start_extern_1 = lexer.previous_position;
    const auto success_extern_1 = lexer.parse_keyword(*keywords_index.get(10));
    if (success_extern_1 == false) {
        return Result<ExternSyntax, ParserError>(ParserError(DifferentSyntax()));
    };
    const auto end = lexer.position;
    return Result<ExternSyntax, ParserError>(ExternSyntax(start, end));
}

Result<InstructionSyntax, ParserError> Parser::parse_instruction(Page* rp, Page* ep) {
    const auto start = lexer.previous_position;
    const auto start_instruction_1 = lexer.previous_position;
    const auto success_instruction_1 = lexer.parse_keyword(*keywords_index.get(17));
    if (success_instruction_1 == false) {
        return Result<InstructionSyntax, ParserError>(ParserError(DifferentSyntax()));
    };
    const auto start_colon_2 = lexer.previous_position;
    const auto success_colon_2 = lexer.parse_colon();
    const auto end = lexer.position;
    return Result<InstructionSyntax, ParserError>(InstructionSyntax(start, end));
}

Result<IntrinsicSyntax, ParserError> Parser::parse_intrinsic(Page* rp, Page* ep) {
    const auto start = lexer.previous_position;
    const auto start_intrinsic_1 = lexer.previous_position;
    const auto success_intrinsic_1 = lexer.parse_keyword(*keywords_index.get(18));
    if (success_intrinsic_1 == false) {
        return Result<IntrinsicSyntax, ParserError>(ParserError(DifferentSyntax()));
    };
    const auto start_colon_2 = lexer.previous_position;
    const auto success_colon_2 = lexer.parse_colon();
    const auto end = lexer.position;
    return Result<IntrinsicSyntax, ParserError>(IntrinsicSyntax(start, end));
}

Result<ExtendsSyntax, ParserError> Parser::parse_extends(Page* rp, Page* ep) {
    const auto start = lexer.previous_position;
    const auto start_extends_1 = lexer.previous_position;
    const auto success_extends_1 = lexer.parse_keyword(*keywords_index.get(9));
    if (success_extends_1 == false) {
        return Result<ExtendsSyntax, ParserError>(ParserError(DifferentSyntax()));
    };
    const auto extensions_start = lexer.position;
    Vector<ExtendSyntax>* extensions = nullptr;
    {
        auto _result = parse_extend_list(rp, ep);
        switch (_result._tag)
        {
            case Success::Error:
            {
                auto error = _result._Error;
                {
                    {
                        auto _result = error;
                        switch (_result._tag)
                        {
                            case ParserError::Different:
                            {
                                auto d = _result._Different;
                                {
                                };
                                break;
                            }
                            case ParserError::Invalid:
                            {
                                auto i = _result._Invalid;
                                return Result<ExtendsSyntax, ParserError>(i);
                                break;
                            }
                        }
                    };
                };
                break;
            }
            case Success::Ok:
            {
                auto success = _result._Ok;
                extensions = success;
                break;
            }
        }
    };
    const auto end = lexer.position;
    return Result<ExtendsSyntax, ParserError>(ExtendsSyntax(start, end, extensions));
}

Result<Vector<ExtendSyntax>*, ParserError> Parser::parse_extend_list(Page* rp, Page* ep) {
    auto r = Region();
    auto list = new (alignof(List<ExtendSyntax>), r.get_page()) List<ExtendSyntax>();
    while (true) {
        const auto _node_result = parse_extend(rp, ep);
        auto node = _node_result._Ok;
        if (_node_result._tag == Success::Error) {
            const auto _node_Error = _node_result._Error;
            switch (_node_Error._tag) {
                case ParserError::Invalid: {
                    const auto i = _node_Error._Invalid;
                    return Result<Vector<ExtendSyntax>*, ParserError>(i);
                    break;
                }
                case ParserError::Different: {
                    const auto d = _node_Error._Different;
                    {
                        if ((*list).head == 0) {
                            return Result<Vector<ExtendSyntax>*, ParserError>(d);
                        }
                        else {
                            return Result<Vector<ExtendSyntax>*, ParserError>(new (alignof(Vector<ExtendSyntax>), rp) Vector<ExtendSyntax>(rp, *list));
                        };
                    };
                    break;
                }
            }
        }
        (*list).add(node);
    };
}

Result<ExtendSyntax, ParserError> Parser::parse_extend(Page* rp, Page* ep) {
    const auto start = lexer.previous_position;
    const auto type_start = lexer.position;
    const auto _type_result = parse_type(rp, ep);
    auto type = _type_result._Ok;
    if (_type_result._tag == Success::Error) {
        const auto _type_Error = _type_result._Error;
        switch (_type_Error._tag) {
            case ParserError::Different: {
                const auto d = _type_Error._Different;
                return Result<ExtendSyntax, ParserError>(d);
                break;
            }
            case ParserError::Invalid: {
                const auto i = _type_Error._Invalid;
                return Result<ExtendSyntax, ParserError>(i);
                break;
            }
        }
    }
    ;
    const auto start_comma_2 = lexer.previous_position;
    const auto success_comma_2 = lexer.parse_punctuation(',');
    const auto end = lexer.position;
    return Result<ExtendSyntax, ParserError>(ExtendSyntax(start, end, type));
}

Result<MacroSyntax, ParserError> Parser::parse_macro(Page* rp, Page* ep) {
    const auto start = lexer.previous_position;
    const auto start_macro_1 = lexer.previous_position;
    const auto success_macro_1 = lexer.parse_keyword(*keywords_index.get(23));
    if (success_macro_1 == false) {
        return Result<MacroSyntax, ParserError>(ParserError(DifferentSyntax()));
    };
    const auto start_name = lexer.previous_position;
    const auto name = lexer.parse_identifier(rp, keywords);
    if (name == nullptr) {
        return Result<MacroSyntax, ParserError>(ParserError(InvalidSyntax(start_name, lexer.position, String(ep, "an identifier"))));
    };
    const auto model_start = lexer.position;
    const auto _model_result = parse_model(rp, ep);
    auto model = _model_result._Ok;
    if (_model_result._tag == Success::Error) {
        const auto _model_Error = _model_result._Error;
        switch (_model_Error._tag) {
            case ParserError::Different: {
                const auto d = _model_Error._Different;
                return Result<MacroSyntax, ParserError>(InvalidSyntax(model_start, lexer.position, String(ep, "a valid Model syntax")));
                break;
            }
            case ParserError::Invalid: {
                const auto i = _model_Error._Invalid;
                return Result<MacroSyntax, ParserError>(i);
                break;
            }
        }
    }
    ;
    const auto rule_start = lexer.position;
    const auto _rule_result = parse_operand_list(rp, ep);
    auto rule = _rule_result._Ok;
    if (_rule_result._tag == Success::Error) {
        const auto _rule_Error = _rule_result._Error;
        switch (_rule_Error._tag) {
            case ParserError::Different: {
                const auto d = _rule_Error._Different;
                return Result<MacroSyntax, ParserError>(InvalidSyntax(rule_start, lexer.position, String(ep, "a valid Operand syntax")));
                break;
            }
            case ParserError::Invalid: {
                const auto i = _rule_Error._Invalid;
                return Result<MacroSyntax, ParserError>(i);
                break;
            }
        }
    }
    ;
    const auto end = lexer.position;
    return Result<MacroSyntax, ParserError>(MacroSyntax(start, end, *name, model, *rule));
}

Result<Vector<AttributeSyntax>*, ParserError> Parser::parse_attribute_list(Page* rp, Page* ep) {
    auto r = Region();
    auto list = new (alignof(List<AttributeSyntax>), r.get_page()) List<AttributeSyntax>();
    while (true) {
        const auto _node_result = parse_attribute(rp, ep);
        auto node = _node_result._Ok;
        if (_node_result._tag == Success::Error) {
            const auto _node_Error = _node_result._Error;
            switch (_node_Error._tag) {
                case ParserError::Invalid: {
                    const auto i = _node_Error._Invalid;
                    return Result<Vector<AttributeSyntax>*, ParserError>(i);
                    break;
                }
                case ParserError::Different: {
                    const auto d = _node_Error._Different;
                    {
                        if ((*list).head == 0) {
                            return Result<Vector<AttributeSyntax>*, ParserError>(d);
                        }
                        else {
                            return Result<Vector<AttributeSyntax>*, ParserError>(new (alignof(Vector<AttributeSyntax>), rp) Vector<AttributeSyntax>(rp, *list));
                        };
                    };
                    break;
                }
            }
        }
        (*list).add(node);
    };
}

Result<AttributeSyntax, ParserError> Parser::parse_attribute(Page* rp, Page* ep) {
    const auto start = lexer.previous_position;
    const auto start_name = lexer.previous_position;
    const auto name = lexer.parse_attribute(rp);
    if (name == nullptr) {
        return Result<AttributeSyntax, ParserError>(ParserError(DifferentSyntax()));
    };
    const auto model_start = lexer.position;
    const auto _model_result = parse_model(rp, ep);
    auto model = _model_result._Ok;
    if (_model_result._tag == Success::Error) {
        const auto _model_Error = _model_result._Error;
        switch (_model_Error._tag) {
            case ParserError::Different: {
                const auto d = _model_Error._Different;
                return Result<AttributeSyntax, ParserError>(InvalidSyntax(model_start, lexer.position, String(ep, "a valid Model syntax")));
                break;
            }
            case ParserError::Invalid: {
                const auto i = _model_Error._Invalid;
                return Result<AttributeSyntax, ParserError>(i);
                break;
            }
        }
    }
    ;
    const auto start_colon_3 = lexer.previous_position;
    const auto success_colon_3 = lexer.parse_colon();
    const auto end = lexer.position;
    return Result<AttributeSyntax, ParserError>(AttributeSyntax(start, end, *name, model));
}

Result<ModelSyntax, ParserError> Parser::parse_model(Page* rp, Page* ep) {
    {
        {
            auto _result = parse_literal(rp, ep);
            switch (_result._tag)
            {
                case Success::Error:
                {
                    auto error = _result._Error;
                    {
                        {
                            auto _result = error;
                            switch (_result._tag)
                            {
                                case ParserError::Invalid:
                                {
                                    auto i = _result._Invalid;
                                    return Result<ModelSyntax, ParserError>(i);
                                    break;
                                }
                                case ParserError::Different:
                                {
                                    auto d = _result._Different;
                                    {
                                    };
                                    break;
                                }
                            }
                        };
                    };
                    break;
                }
                case Success::Ok:
                {
                    auto node = _result._Ok;
                    return Result<ModelSyntax, ParserError>(ModelSyntax(LiteralSyntax(node)));
                    break;
                }
            }
        };
    };
    {
        {
            auto _result = parse_name(rp, ep);
            switch (_result._tag)
            {
                case Success::Error:
                {
                    auto error = _result._Error;
                    {
                        {
                            auto _result = error;
                            switch (_result._tag)
                            {
                                case ParserError::Invalid:
                                {
                                    auto i = _result._Invalid;
                                    return Result<ModelSyntax, ParserError>(i);
                                    break;
                                }
                                case ParserError::Different:
                                {
                                    auto d = _result._Different;
                                    {
                                    };
                                    break;
                                }
                            }
                        };
                    };
                    break;
                }
                case Success::Ok:
                {
                    auto node = _result._Ok;
                    return Result<ModelSyntax, ParserError>(ModelSyntax(NameSyntax(node)));
                    break;
                }
            }
        };
    };
    {
        {
            auto _result = parse_object(rp, ep);
            switch (_result._tag)
            {
                case Success::Error:
                {
                    auto error = _result._Error;
                    {
                        {
                            auto _result = error;
                            switch (_result._tag)
                            {
                                case ParserError::Invalid:
                                {
                                    auto i = _result._Invalid;
                                    return Result<ModelSyntax, ParserError>(i);
                                    break;
                                }
                                case ParserError::Different:
                                {
                                    auto d = _result._Different;
                                    {
                                    };
                                    break;
                                }
                            }
                        };
                    };
                    break;
                }
                case Success::Ok:
                {
                    auto node = _result._Ok;
                    return Result<ModelSyntax, ParserError>(ModelSyntax(ObjectSyntax(node)));
                    break;
                }
            }
        };
    };
    {
        {
            auto _result = parse_vector(rp, ep);
            switch (_result._tag)
            {
                case Success::Error:
                {
                    auto error = _result._Error;
                    {
                        {
                            auto _result = error;
                            switch (_result._tag)
                            {
                                case ParserError::Invalid:
                                {
                                    auto i = _result._Invalid;
                                    return Result<ModelSyntax, ParserError>(i);
                                    break;
                                }
                                case ParserError::Different:
                                {
                                    auto d = _result._Different;
                                    {
                                    };
                                    break;
                                }
                            }
                        };
                    };
                    break;
                }
                case Success::Ok:
                {
                    auto node = _result._Ok;
                    return Result<ModelSyntax, ParserError>(ModelSyntax(VectorSyntax(node)));
                    break;
                }
            }
        };
    };
    return Result<ModelSyntax, ParserError>(ParserError(DifferentSyntax()));
}

Result<ModuleSyntax, ParserError> Parser::parse_module(Page* rp, Page* ep) {
    const auto start = lexer.previous_position;
    const auto start_module_1 = lexer.previous_position;
    const auto success_module_1 = lexer.parse_keyword(*keywords_index.get(25));
    if (success_module_1 == false) {
        return Result<ModuleSyntax, ParserError>(ParserError(DifferentSyntax()));
    };
    const auto start_name = lexer.previous_position;
    const auto name = lexer.parse_identifier(rp, keywords);
    if (name == nullptr) {
        return Result<ModuleSyntax, ParserError>(ParserError(InvalidSyntax(start_name, lexer.position, String(ep, "an identifier"))));
    };
    const auto start_colon_3 = lexer.previous_position;
    const auto success_colon_3 = lexer.parse_colon();
    const auto end = lexer.position;
    return Result<ModuleSyntax, ParserError>(ModuleSyntax(start, end, *name));
}

Result<Vector<PackageSyntax>*, ParserError> Parser::parse_package_list(Page* rp, Page* ep) {
    auto r = Region();
    auto list = new (alignof(List<PackageSyntax>), r.get_page()) List<PackageSyntax>();
    while (true) {
        const auto _node_result = parse_package(rp, ep);
        auto node = _node_result._Ok;
        if (_node_result._tag == Success::Error) {
            const auto _node_Error = _node_result._Error;
            switch (_node_Error._tag) {
                case ParserError::Invalid: {
                    const auto i = _node_Error._Invalid;
                    return Result<Vector<PackageSyntax>*, ParserError>(i);
                    break;
                }
                case ParserError::Different: {
                    const auto d = _node_Error._Different;
                    {
                        if ((*list).head == 0) {
                            return Result<Vector<PackageSyntax>*, ParserError>(d);
                        }
                        else {
                            return Result<Vector<PackageSyntax>*, ParserError>(new (alignof(Vector<PackageSyntax>), rp) Vector<PackageSyntax>(rp, *list));
                        };
                    };
                    break;
                }
            }
        }
        (*list).add(node);
    };
}

Result<PackageSyntax, ParserError> Parser::parse_package(Page* rp, Page* ep) {
    const auto start = lexer.previous_position;
    const auto start_package_1 = lexer.previous_position;
    const auto success_package_1 = lexer.parse_keyword(*keywords_index.get(44));
    if (success_package_1 == false) {
        return Result<PackageSyntax, ParserError>(ParserError(DifferentSyntax()));
    };
    const auto name_start = lexer.position;
    const auto _name_result = parse_name(rp, ep);
    auto name = _name_result._Ok;
    if (_name_result._tag == Success::Error) {
        const auto _name_Error = _name_result._Error;
        switch (_name_Error._tag) {
            case ParserError::Different: {
                const auto d = _name_Error._Different;
                return Result<PackageSyntax, ParserError>(InvalidSyntax(name_start, lexer.position, String(ep, "a valid Name syntax")));
                break;
            }
            case ParserError::Invalid: {
                const auto i = _name_Error._Invalid;
                return Result<PackageSyntax, ParserError>(i);
                break;
            }
        }
    }
    ;
    const auto start_colon_3 = lexer.previous_position;
    const auto success_colon_3 = lexer.parse_colon();
    const auto end = lexer.position;
    return Result<PackageSyntax, ParserError>(PackageSyntax(start, end, name));
}

Result<InitializerSyntax, ParserError> Parser::parse_initializer(Page* rp, Page* ep) {
    const auto start = lexer.previous_position;
    const auto start_left_paren_1 = lexer.previous_position;
    const auto success_left_paren_1 = lexer.parse_punctuation('(');
    if (success_left_paren_1 == false) {
        return Result<InitializerSyntax, ParserError>(ParserError(DifferentSyntax()));
    };
    const auto operands_start = lexer.position;
    const auto _operands_result = parse_operand_list(rp, ep);
    auto operands = _operands_result._Ok;
    if (_operands_result._tag == Success::Error) {
        const auto _operands_Error = _operands_result._Error;
        switch (_operands_Error._tag) {
            case ParserError::Different: {
                const auto d = _operands_Error._Different;
                return Result<InitializerSyntax, ParserError>(InvalidSyntax(operands_start, lexer.position, String(ep, "a valid Operand syntax")));
                break;
            }
            case ParserError::Invalid: {
                const auto i = _operands_Error._Invalid;
                return Result<InitializerSyntax, ParserError>(i);
                break;
            }
        }
    }
    ;
    const auto start_right_paren_3 = lexer.previous_position;
    const auto success_right_paren_3 = lexer.parse_punctuation(')');
    if (success_right_paren_3 == false) {
        return Result<InitializerSyntax, ParserError>(ParserError(InvalidSyntax(start_right_paren_3, lexer.position, String(ep, ")"))));
    };
    const auto end = lexer.position;
    return Result<InitializerSyntax, ParserError>(InitializerSyntax(start, end, *operands));
}

Result<Vector<OperandSyntax>*, ParserError> Parser::parse_operand_list(Page* rp, Page* ep) {
    auto r = Region();
    auto list = new (alignof(List<OperandSyntax>), r.get_page()) List<OperandSyntax>();
    while (true) {
        const auto _node_result = parse_operand(rp, ep);
        auto node = _node_result._Ok;
        if (_node_result._tag == Success::Error) {
            const auto _node_Error = _node_result._Error;
            switch (_node_Error._tag) {
                case ParserError::Invalid: {
                    const auto i = _node_Error._Invalid;
                    return Result<Vector<OperandSyntax>*, ParserError>(i);
                    break;
                }
                case ParserError::Different: {
                    const auto d = _node_Error._Different;
                    {
                        if ((*list).head == 0) {
                            return Result<Vector<OperandSyntax>*, ParserError>(d);
                        }
                        else {
                            return Result<Vector<OperandSyntax>*, ParserError>(new (alignof(Vector<OperandSyntax>), rp) Vector<OperandSyntax>(rp, *list));
                        };
                    };
                    break;
                }
            }
        }
        (*list).add(node);
    };
}

Result<OperandSyntax, ParserError> Parser::parse_operand(Page* rp, Page* ep) {
    const auto start = lexer.previous_position;
    const auto expression_start = lexer.position;
    const auto _expression_result = parse_expression(rp, ep);
    auto expression = _expression_result._Ok;
    if (_expression_result._tag == Success::Error) {
        const auto _expression_Error = _expression_result._Error;
        switch (_expression_Error._tag) {
            case ParserError::Different: {
                const auto d = _expression_Error._Different;
                return Result<OperandSyntax, ParserError>(d);
                break;
            }
            case ParserError::Invalid: {
                const auto i = _expression_Error._Invalid;
                return Result<OperandSyntax, ParserError>(i);
                break;
            }
        }
    }
    ;
    const auto members_start = lexer.position;
    Vector<MemberAccessSyntax>* members = nullptr;
    {
        auto _result = parse_memberaccess_list(rp, ep);
        switch (_result._tag)
        {
            case Success::Error:
            {
                auto error = _result._Error;
                {
                    {
                        auto _result = error;
                        switch (_result._tag)
                        {
                            case ParserError::Different:
                            {
                                auto d = _result._Different;
                                {
                                };
                                break;
                            }
                            case ParserError::Invalid:
                            {
                                auto i = _result._Invalid;
                                return Result<OperandSyntax, ParserError>(i);
                                break;
                            }
                        }
                    };
                };
                break;
            }
            case Success::Ok:
            {
                auto success = _result._Ok;
                members = success;
                break;
            }
        }
    };
    const auto end = lexer.position;
    return Result<OperandSyntax, ParserError>(OperandSyntax(start, end, expression, members));
}

Result<Vector<MemberAccessSyntax>*, ParserError> Parser::parse_memberaccess_list(Page* rp, Page* ep) {
    auto r = Region();
    auto list = new (alignof(List<MemberAccessSyntax>), r.get_page()) List<MemberAccessSyntax>();
    while (true) {
        const auto _node_result = parse_memberaccess(rp, ep);
        auto node = _node_result._Ok;
        if (_node_result._tag == Success::Error) {
            const auto _node_Error = _node_result._Error;
            switch (_node_Error._tag) {
                case ParserError::Invalid: {
                    const auto i = _node_Error._Invalid;
                    return Result<Vector<MemberAccessSyntax>*, ParserError>(i);
                    break;
                }
                case ParserError::Different: {
                    const auto d = _node_Error._Different;
                    {
                        if ((*list).head == 0) {
                            return Result<Vector<MemberAccessSyntax>*, ParserError>(d);
                        }
                        else {
                            return Result<Vector<MemberAccessSyntax>*, ParserError>(new (alignof(Vector<MemberAccessSyntax>), rp) Vector<MemberAccessSyntax>(rp, *list));
                        };
                    };
                    break;
                }
            }
        }
        (*list).add(node);
    };
}

Result<MemberAccessSyntax, ParserError> Parser::parse_memberaccess(Page* rp, Page* ep) {
    const auto start = lexer.previous_position;
    const auto start_dot_1 = lexer.previous_position;
    const auto success_dot_1 = lexer.parse_punctuation('.');
    if (success_dot_1 == false) {
        return Result<MemberAccessSyntax, ParserError>(ParserError(DifferentSyntax()));
    };
    const auto name_start = lexer.position;
    const auto _name_result = parse_name(rp, ep);
    auto name = _name_result._Ok;
    if (_name_result._tag == Success::Error) {
        const auto _name_Error = _name_result._Error;
        switch (_name_Error._tag) {
            case ParserError::Different: {
                const auto d = _name_Error._Different;
                return Result<MemberAccessSyntax, ParserError>(InvalidSyntax(name_start, lexer.position, String(ep, "a valid Name syntax")));
                break;
            }
            case ParserError::Invalid: {
                const auto i = _name_Error._Invalid;
                return Result<MemberAccessSyntax, ParserError>(i);
                break;
            }
        }
    }
    ;
    const auto end = lexer.position;
    return Result<MemberAccessSyntax, ParserError>(MemberAccessSyntax(start, end, name));
}

Result<ExpressionSyntax, ParserError> Parser::parse_expression(Page* rp, Page* ep) {
    {
        {
            auto _result = parse_literal(rp, ep);
            switch (_result._tag)
            {
                case Success::Error:
                {
                    auto error = _result._Error;
                    {
                        {
                            auto _result = error;
                            switch (_result._tag)
                            {
                                case ParserError::Invalid:
                                {
                                    auto i = _result._Invalid;
                                    return Result<ExpressionSyntax, ParserError>(i);
                                    break;
                                }
                                case ParserError::Different:
                                {
                                    auto d = _result._Different;
                                    {
                                    };
                                    break;
                                }
                            }
                        };
                    };
                    break;
                }
                case Success::Ok:
                {
                    auto node = _result._Ok;
                    return Result<ExpressionSyntax, ParserError>(ExpressionSyntax(LiteralSyntax(node)));
                    break;
                }
            }
        };
    };
    {
        {
            auto _result = parse_type(rp, ep);
            switch (_result._tag)
            {
                case Success::Error:
                {
                    auto error = _result._Error;
                    {
                        {
                            auto _result = error;
                            switch (_result._tag)
                            {
                                case ParserError::Invalid:
                                {
                                    auto i = _result._Invalid;
                                    return Result<ExpressionSyntax, ParserError>(i);
                                    break;
                                }
                                case ParserError::Different:
                                {
                                    auto d = _result._Different;
                                    {
                                    };
                                    break;
                                }
                            }
                        };
                    };
                    break;
                }
                case Success::Ok:
                {
                    auto node = _result._Ok;
                    return Result<ExpressionSyntax, ParserError>(ExpressionSyntax(TypeSyntax(node)));
                    break;
                }
            }
        };
    };
    {
        {
            auto _result = parse_object(rp, ep);
            switch (_result._tag)
            {
                case Success::Error:
                {
                    auto error = _result._Error;
                    {
                        {
                            auto _result = error;
                            switch (_result._tag)
                            {
                                case ParserError::Invalid:
                                {
                                    auto i = _result._Invalid;
                                    return Result<ExpressionSyntax, ParserError>(i);
                                    break;
                                }
                                case ParserError::Different:
                                {
                                    auto d = _result._Different;
                                    {
                                    };
                                    break;
                                }
                            }
                        };
                    };
                    break;
                }
                case Success::Ok:
                {
                    auto node = _result._Ok;
                    return Result<ExpressionSyntax, ParserError>(ExpressionSyntax(ObjectSyntax(node)));
                    break;
                }
            }
        };
    };
    {
        {
            auto _result = parse_vector(rp, ep);
            switch (_result._tag)
            {
                case Success::Error:
                {
                    auto error = _result._Error;
                    {
                        {
                            auto _result = error;
                            switch (_result._tag)
                            {
                                case ParserError::Invalid:
                                {
                                    auto i = _result._Invalid;
                                    return Result<ExpressionSyntax, ParserError>(i);
                                    break;
                                }
                                case ParserError::Different:
                                {
                                    auto d = _result._Different;
                                    {
                                    };
                                    break;
                                }
                            }
                        };
                    };
                    break;
                }
                case Success::Ok:
                {
                    auto node = _result._Ok;
                    return Result<ExpressionSyntax, ParserError>(ExpressionSyntax(VectorSyntax(node)));
                    break;
                }
            }
        };
    };
    {
        {
            auto _result = parse_block(rp, ep);
            switch (_result._tag)
            {
                case Success::Error:
                {
                    auto error = _result._Error;
                    {
                        {
                            auto _result = error;
                            switch (_result._tag)
                            {
                                case ParserError::Invalid:
                                {
                                    auto i = _result._Invalid;
                                    return Result<ExpressionSyntax, ParserError>(i);
                                    break;
                                }
                                case ParserError::Different:
                                {
                                    auto d = _result._Different;
                                    {
                                    };
                                    break;
                                }
                            }
                        };
                    };
                    break;
                }
                case Success::Ok:
                {
                    auto node = _result._Ok;
                    return Result<ExpressionSyntax, ParserError>(ExpressionSyntax(BlockSyntax(node)));
                    break;
                }
            }
        };
    };
    {
        {
            auto _result = parse_if(rp, ep);
            switch (_result._tag)
            {
                case Success::Error:
                {
                    auto error = _result._Error;
                    {
                        {
                            auto _result = error;
                            switch (_result._tag)
                            {
                                case ParserError::Invalid:
                                {
                                    auto i = _result._Invalid;
                                    return Result<ExpressionSyntax, ParserError>(i);
                                    break;
                                }
                                case ParserError::Different:
                                {
                                    auto d = _result._Different;
                                    {
                                    };
                                    break;
                                }
                            }
                        };
                    };
                    break;
                }
                case Success::Ok:
                {
                    auto node = _result._Ok;
                    return Result<ExpressionSyntax, ParserError>(ExpressionSyntax(IfSyntax(node)));
                    break;
                }
            }
        };
    };
    {
        {
            auto _result = parse_match(rp, ep);
            switch (_result._tag)
            {
                case Success::Error:
                {
                    auto error = _result._Error;
                    {
                        {
                            auto _result = error;
                            switch (_result._tag)
                            {
                                case ParserError::Invalid:
                                {
                                    auto i = _result._Invalid;
                                    return Result<ExpressionSyntax, ParserError>(i);
                                    break;
                                }
                                case ParserError::Different:
                                {
                                    auto d = _result._Different;
                                    {
                                    };
                                    break;
                                }
                            }
                        };
                    };
                    break;
                }
                case Success::Ok:
                {
                    auto node = _result._Ok;
                    return Result<ExpressionSyntax, ParserError>(ExpressionSyntax(MatchSyntax(node)));
                    break;
                }
            }
        };
    };
    {
        {
            auto _result = parse_lambda(rp, ep);
            switch (_result._tag)
            {
                case Success::Error:
                {
                    auto error = _result._Error;
                    {
                        {
                            auto _result = error;
                            switch (_result._tag)
                            {
                                case ParserError::Invalid:
                                {
                                    auto i = _result._Invalid;
                                    return Result<ExpressionSyntax, ParserError>(i);
                                    break;
                                }
                                case ParserError::Different:
                                {
                                    auto d = _result._Different;
                                    {
                                    };
                                    break;
                                }
                            }
                        };
                    };
                    break;
                }
                case Success::Ok:
                {
                    auto node = _result._Ok;
                    return Result<ExpressionSyntax, ParserError>(ExpressionSyntax(LambdaSyntax(node)));
                    break;
                }
            }
        };
    };
    {
        {
            auto _result = parse_for(rp, ep);
            switch (_result._tag)
            {
                case Success::Error:
                {
                    auto error = _result._Error;
                    {
                        {
                            auto _result = error;
                            switch (_result._tag)
                            {
                                case ParserError::Invalid:
                                {
                                    auto i = _result._Invalid;
                                    return Result<ExpressionSyntax, ParserError>(i);
                                    break;
                                }
                                case ParserError::Different:
                                {
                                    auto d = _result._Different;
                                    {
                                    };
                                    break;
                                }
                            }
                        };
                    };
                    break;
                }
                case Success::Ok:
                {
                    auto node = _result._Ok;
                    return Result<ExpressionSyntax, ParserError>(ExpressionSyntax(ForSyntax(node)));
                    break;
                }
            }
        };
    };
    {
        {
            auto _result = parse_while(rp, ep);
            switch (_result._tag)
            {
                case Success::Error:
                {
                    auto error = _result._Error;
                    {
                        {
                            auto _result = error;
                            switch (_result._tag)
                            {
                                case ParserError::Invalid:
                                {
                                    auto i = _result._Invalid;
                                    return Result<ExpressionSyntax, ParserError>(i);
                                    break;
                                }
                                case ParserError::Different:
                                {
                                    auto d = _result._Different;
                                    {
                                    };
                                    break;
                                }
                            }
                        };
                    };
                    break;
                }
                case Success::Ok:
                {
                    auto node = _result._Ok;
                    return Result<ExpressionSyntax, ParserError>(ExpressionSyntax(WhileSyntax(node)));
                    break;
                }
            }
        };
    };
    {
        {
            auto _result = parse_choose(rp, ep);
            switch (_result._tag)
            {
                case Success::Error:
                {
                    auto error = _result._Error;
                    {
                        {
                            auto _result = error;
                            switch (_result._tag)
                            {
                                case ParserError::Invalid:
                                {
                                    auto i = _result._Invalid;
                                    return Result<ExpressionSyntax, ParserError>(i);
                                    break;
                                }
                                case ParserError::Different:
                                {
                                    auto d = _result._Different;
                                    {
                                    };
                                    break;
                                }
                            }
                        };
                    };
                    break;
                }
                case Success::Ok:
                {
                    auto node = _result._Ok;
                    return Result<ExpressionSyntax, ParserError>(ExpressionSyntax(ChooseSyntax(node)));
                    break;
                }
            }
        };
    };
    {
        {
            auto _result = parse_try(rp, ep);
            switch (_result._tag)
            {
                case Success::Error:
                {
                    auto error = _result._Error;
                    {
                        {
                            auto _result = error;
                            switch (_result._tag)
                            {
                                case ParserError::Invalid:
                                {
                                    auto i = _result._Invalid;
                                    return Result<ExpressionSyntax, ParserError>(i);
                                    break;
                                }
                                case ParserError::Different:
                                {
                                    auto d = _result._Different;
                                    {
                                    };
                                    break;
                                }
                            }
                        };
                    };
                    break;
                }
                case Success::Ok:
                {
                    auto node = _result._Ok;
                    return Result<ExpressionSyntax, ParserError>(ExpressionSyntax(TrySyntax(node)));
                    break;
                }
            }
        };
    };
    {
        {
            auto _result = parse_repeat(rp, ep);
            switch (_result._tag)
            {
                case Success::Error:
                {
                    auto error = _result._Error;
                    {
                        {
                            auto _result = error;
                            switch (_result._tag)
                            {
                                case ParserError::Invalid:
                                {
                                    auto i = _result._Invalid;
                                    return Result<ExpressionSyntax, ParserError>(i);
                                    break;
                                }
                                case ParserError::Different:
                                {
                                    auto d = _result._Different;
                                    {
                                    };
                                    break;
                                }
                            }
                        };
                    };
                    break;
                }
                case Success::Ok:
                {
                    auto node = _result._Ok;
                    return Result<ExpressionSyntax, ParserError>(ExpressionSyntax(RepeatSyntax(node)));
                    break;
                }
            }
        };
    };
    {
        {
            auto _result = parse_sizeof(rp, ep);
            switch (_result._tag)
            {
                case Success::Error:
                {
                    auto error = _result._Error;
                    {
                        {
                            auto _result = error;
                            switch (_result._tag)
                            {
                                case ParserError::Invalid:
                                {
                                    auto i = _result._Invalid;
                                    return Result<ExpressionSyntax, ParserError>(i);
                                    break;
                                }
                                case ParserError::Different:
                                {
                                    auto d = _result._Different;
                                    {
                                    };
                                    break;
                                }
                            }
                        };
                    };
                    break;
                }
                case Success::Ok:
                {
                    auto node = _result._Ok;
                    return Result<ExpressionSyntax, ParserError>(ExpressionSyntax(SizeOfSyntax(node)));
                    break;
                }
            }
        };
    };
    return Result<ExpressionSyntax, ParserError>(ParserError(DifferentSyntax()));
}

Result<LiteralSyntax, ParserError> Parser::parse_literal(Page* rp, Page* ep) {
    const auto start = lexer.previous_position;
    const auto _literal_result = parse_literal_token(rp, ep);
    auto literal = _literal_result._Ok;
    if (_literal_result._tag == Success::Error) {
        const auto _literal_Error = _literal_result._Error;
        switch (_literal_Error._tag) {
            case ParserError::Different: {
                const auto d = _literal_Error._Different;
                return Result<LiteralSyntax, ParserError>(d);
                break;
            }
            case ParserError::Invalid: {
                const auto i = _literal_Error._Invalid;
                return Result<LiteralSyntax, ParserError>(i);
                break;
            }
        }
    }
    ;
    const auto end = lexer.position;
    return Result<LiteralSyntax, ParserError>(LiteralSyntax(start, end, literal));
}

Result<ObjectSyntax, ParserError> Parser::parse_object(Page* rp, Page* ep) {
    const auto start = lexer.previous_position;
    const auto start_left_paren_1 = lexer.previous_position;
    const auto success_left_paren_1 = lexer.parse_punctuation('(');
    if (success_left_paren_1 == false) {
        return Result<ObjectSyntax, ParserError>(ParserError(DifferentSyntax()));
    };
    const auto components_start = lexer.position;
    Vector<ComponentSyntax>* components = nullptr;
    {
        auto _result = parse_component_list(rp, ep);
        switch (_result._tag)
        {
            case Success::Error:
            {
                auto error = _result._Error;
                {
                    {
                        auto _result = error;
                        switch (_result._tag)
                        {
                            case ParserError::Different:
                            {
                                auto d = _result._Different;
                                {
                                };
                                break;
                            }
                            case ParserError::Invalid:
                            {
                                auto i = _result._Invalid;
                                return Result<ObjectSyntax, ParserError>(i);
                                break;
                            }
                        }
                    };
                };
                break;
            }
            case Success::Ok:
            {
                auto success = _result._Ok;
                components = success;
                break;
            }
        }
    };
    const auto start_right_paren_3 = lexer.previous_position;
    const auto success_right_paren_3 = lexer.parse_punctuation(')');
    if (success_right_paren_3 == false) {
        return Result<ObjectSyntax, ParserError>(ParserError(InvalidSyntax(start_right_paren_3, lexer.position, String(ep, ")"))));
    };
    const auto end = lexer.position;
    return Result<ObjectSyntax, ParserError>(ObjectSyntax(start, end, components));
}

Result<Vector<ComponentSyntax>*, ParserError> Parser::parse_component_list(Page* rp, Page* ep) {
    auto r = Region();
    auto list = new (alignof(List<ComponentSyntax>), r.get_page()) List<ComponentSyntax>();
    while (true) {
        const auto _node_result = parse_component(rp, ep);
        auto node = _node_result._Ok;
        if (_node_result._tag == Success::Error) {
            const auto _node_Error = _node_result._Error;
            switch (_node_Error._tag) {
                case ParserError::Invalid: {
                    const auto i = _node_Error._Invalid;
                    return Result<Vector<ComponentSyntax>*, ParserError>(i);
                    break;
                }
                case ParserError::Different: {
                    const auto d = _node_Error._Different;
                    {
                        if ((*list).head == 0) {
                            return Result<Vector<ComponentSyntax>*, ParserError>(d);
                        }
                        else {
                            return Result<Vector<ComponentSyntax>*, ParserError>(new (alignof(Vector<ComponentSyntax>), rp) Vector<ComponentSyntax>(rp, *list));
                        };
                    };
                    break;
                }
            }
        }
        (*list).add(node);
    };
}

Result<ComponentSyntax, ParserError> Parser::parse_component(Page* rp, Page* ep) {
    const auto start = lexer.previous_position;
    const auto operands_start = lexer.position;
    const auto _operands_result = parse_operand_list(rp, ep);
    auto operands = _operands_result._Ok;
    if (_operands_result._tag == Success::Error) {
        const auto _operands_Error = _operands_result._Error;
        switch (_operands_Error._tag) {
            case ParserError::Different: {
                const auto d = _operands_Error._Different;
                return Result<ComponentSyntax, ParserError>(d);
                break;
            }
            case ParserError::Invalid: {
                const auto i = _operands_Error._Invalid;
                return Result<ComponentSyntax, ParserError>(i);
                break;
            }
        }
    }
    ;
    const auto attributes_start = lexer.position;
    Vector<AttributeSyntax>* attributes = nullptr;
    {
        auto _result = parse_attribute_list(rp, ep);
        switch (_result._tag)
        {
            case Success::Error:
            {
                auto error = _result._Error;
                {
                    {
                        auto _result = error;
                        switch (_result._tag)
                        {
                            case ParserError::Different:
                            {
                                auto d = _result._Different;
                                {
                                };
                                break;
                            }
                            case ParserError::Invalid:
                            {
                                auto i = _result._Invalid;
                                return Result<ComponentSyntax, ParserError>(i);
                                break;
                            }
                        }
                    };
                };
                break;
            }
            case Success::Ok:
            {
                auto success = _result._Ok;
                attributes = success;
                break;
            }
        }
    };
    const auto value_start = lexer.position;
    ValueSyntax* value = nullptr;
    {
        auto _result = parse_value(rp, ep);
        switch (_result._tag)
        {
            case Success::Error:
            {
                auto error = _result._Error;
                {
                    {
                        auto _result = error;
                        switch (_result._tag)
                        {
                            case ParserError::Different:
                            {
                                auto d = _result._Different;
                                {
                                };
                                break;
                            }
                            case ParserError::Invalid:
                            {
                                auto i = _result._Invalid;
                                return Result<ComponentSyntax, ParserError>(i);
                                break;
                            }
                        }
                    };
                };
                break;
            }
            case Success::Ok:
            {
                auto success = _result._Ok;
                value = new (alignof(ValueSyntax), rp) ValueSyntax(success);
                break;
            }
        }
    };
    const auto start_comma_4 = lexer.previous_position;
    const auto success_comma_4 = lexer.parse_punctuation(',');
    const auto end = lexer.position;
    return Result<ComponentSyntax, ParserError>(ComponentSyntax(start, end, *operands, attributes, value));
}

Result<ValueSyntax, ParserError> Parser::parse_value(Page* rp, Page* ep) {
    const auto start = lexer.previous_position;
    const auto start_colon_1 = lexer.previous_position;
    const auto success_colon_1 = lexer.parse_colon();
    if (success_colon_1 == false) {
        return Result<ValueSyntax, ParserError>(ParserError(DifferentSyntax()));
    };
    const auto value_start = lexer.position;
    const auto _value_result = parse_operand_list(rp, ep);
    auto value = _value_result._Ok;
    if (_value_result._tag == Success::Error) {
        const auto _value_Error = _value_result._Error;
        switch (_value_Error._tag) {
            case ParserError::Different: {
                const auto d = _value_Error._Different;
                return Result<ValueSyntax, ParserError>(InvalidSyntax(value_start, lexer.position, String(ep, "a valid Operand syntax")));
                break;
            }
            case ParserError::Invalid: {
                const auto i = _value_Error._Invalid;
                return Result<ValueSyntax, ParserError>(i);
                break;
            }
        }
    }
    ;
    const auto attributes_start = lexer.position;
    Vector<AttributeSyntax>* attributes = nullptr;
    {
        auto _result = parse_attribute_list(rp, ep);
        switch (_result._tag)
        {
            case Success::Error:
            {
                auto error = _result._Error;
                {
                    {
                        auto _result = error;
                        switch (_result._tag)
                        {
                            case ParserError::Different:
                            {
                                auto d = _result._Different;
                                {
                                };
                                break;
                            }
                            case ParserError::Invalid:
                            {
                                auto i = _result._Invalid;
                                return Result<ValueSyntax, ParserError>(i);
                                break;
                            }
                        }
                    };
                };
                break;
            }
            case Success::Ok:
            {
                auto success = _result._Ok;
                attributes = success;
                break;
            }
        }
    };
    const auto end = lexer.position;
    return Result<ValueSyntax, ParserError>(ValueSyntax(start, end, *value, attributes));
}

Result<VectorSyntax, ParserError> Parser::parse_vector(Page* rp, Page* ep) {
    const auto start = lexer.previous_position;
    const auto start_left_bracket_1 = lexer.previous_position;
    const auto success_left_bracket_1 = lexer.parse_punctuation('[');
    if (success_left_bracket_1 == false) {
        return Result<VectorSyntax, ParserError>(ParserError(DifferentSyntax()));
    };
    const auto elements_start = lexer.position;
    const auto _elements_result = parse_element_list(rp, ep);
    auto elements = _elements_result._Ok;
    if (_elements_result._tag == Success::Error) {
        const auto _elements_Error = _elements_result._Error;
        switch (_elements_Error._tag) {
            case ParserError::Different: {
                const auto d = _elements_Error._Different;
                return Result<VectorSyntax, ParserError>(InvalidSyntax(elements_start, lexer.position, String(ep, "a valid Element syntax")));
                break;
            }
            case ParserError::Invalid: {
                const auto i = _elements_Error._Invalid;
                return Result<VectorSyntax, ParserError>(i);
                break;
            }
        }
    }
    ;
    const auto start_right_bracket_3 = lexer.previous_position;
    const auto success_right_bracket_3 = lexer.parse_punctuation(']');
    if (success_right_bracket_3 == false) {
        return Result<VectorSyntax, ParserError>(ParserError(InvalidSyntax(start_right_bracket_3, lexer.position, String(ep, "]"))));
    };
    const auto lifetime_start = lexer.position;
    LifetimeSyntax* lifetime = nullptr;
    {
        auto _result = parse_lifetime(rp, ep);
        switch (_result._tag)
        {
            case Success::Error:
            {
                auto error = _result._Error;
                {
                    {
                        auto _result = error;
                        switch (_result._tag)
                        {
                            case ParserError::Different:
                            {
                                auto d = _result._Different;
                                {
                                };
                                break;
                            }
                            case ParserError::Invalid:
                            {
                                auto i = _result._Invalid;
                                return Result<VectorSyntax, ParserError>(i);
                                break;
                            }
                        }
                    };
                };
                break;
            }
            case Success::Ok:
            {
                auto success = _result._Ok;
                lifetime = new (alignof(LifetimeSyntax), rp) LifetimeSyntax(success);
                break;
            }
        }
    };
    const auto end = lexer.position;
    return Result<VectorSyntax, ParserError>(VectorSyntax(start, end, *elements, lifetime));
}

Result<Vector<ElementSyntax>*, ParserError> Parser::parse_element_list(Page* rp, Page* ep) {
    auto r = Region();
    auto list = new (alignof(List<ElementSyntax>), r.get_page()) List<ElementSyntax>();
    while (true) {
        const auto _node_result = parse_element(rp, ep);
        auto node = _node_result._Ok;
        if (_node_result._tag == Success::Error) {
            const auto _node_Error = _node_result._Error;
            switch (_node_Error._tag) {
                case ParserError::Invalid: {
                    const auto i = _node_Error._Invalid;
                    return Result<Vector<ElementSyntax>*, ParserError>(i);
                    break;
                }
                case ParserError::Different: {
                    const auto d = _node_Error._Different;
                    {
                        if ((*list).head == 0) {
                            return Result<Vector<ElementSyntax>*, ParserError>(d);
                        }
                        else {
                            return Result<Vector<ElementSyntax>*, ParserError>(new (alignof(Vector<ElementSyntax>), rp) Vector<ElementSyntax>(rp, *list));
                        };
                    };
                    break;
                }
            }
        }
        (*list).add(node);
    };
}

Result<ElementSyntax, ParserError> Parser::parse_element(Page* rp, Page* ep) {
    const auto start = lexer.previous_position;
    const auto operation_start = lexer.position;
    const auto _operation_result = parse_operand_list(rp, ep);
    auto operation = _operation_result._Ok;
    if (_operation_result._tag == Success::Error) {
        const auto _operation_Error = _operation_result._Error;
        switch (_operation_Error._tag) {
            case ParserError::Different: {
                const auto d = _operation_Error._Different;
                return Result<ElementSyntax, ParserError>(d);
                break;
            }
            case ParserError::Invalid: {
                const auto i = _operation_Error._Invalid;
                return Result<ElementSyntax, ParserError>(i);
                break;
            }
        }
    }
    ;
    const auto attributes_start = lexer.position;
    Vector<AttributeSyntax>* attributes = nullptr;
    {
        auto _result = parse_attribute_list(rp, ep);
        switch (_result._tag)
        {
            case Success::Error:
            {
                auto error = _result._Error;
                {
                    {
                        auto _result = error;
                        switch (_result._tag)
                        {
                            case ParserError::Different:
                            {
                                auto d = _result._Different;
                                {
                                };
                                break;
                            }
                            case ParserError::Invalid:
                            {
                                auto i = _result._Invalid;
                                return Result<ElementSyntax, ParserError>(i);
                                break;
                            }
                        }
                    };
                };
                break;
            }
            case Success::Ok:
            {
                auto success = _result._Ok;
                attributes = success;
                break;
            }
        }
    };
    const auto start_comma_3 = lexer.previous_position;
    const auto success_comma_3 = lexer.parse_punctuation(',');
    const auto end = lexer.position;
    return Result<ElementSyntax, ParserError>(ElementSyntax(start, end, *operation, attributes));
}

Result<BlockSyntax, ParserError> Parser::parse_block(Page* rp, Page* ep) {
    const auto start = lexer.previous_position;
    const auto start_left_curly_1 = lexer.previous_position;
    const auto success_left_curly_1 = lexer.parse_punctuation('{');
    if (success_left_curly_1 == false) {
        return Result<BlockSyntax, ParserError>(ParserError(DifferentSyntax()));
    };
    const auto uses_start = lexer.position;
    Vector<UseSyntax>* uses = nullptr;
    {
        auto _result = parse_use_list(rp, ep);
        switch (_result._tag)
        {
            case Success::Error:
            {
                auto error = _result._Error;
                {
                    {
                        auto _result = error;
                        switch (_result._tag)
                        {
                            case ParserError::Different:
                            {
                                auto d = _result._Different;
                                {
                                };
                                break;
                            }
                            case ParserError::Invalid:
                            {
                                auto i = _result._Invalid;
                                return Result<BlockSyntax, ParserError>(i);
                                break;
                            }
                        }
                    };
                };
                break;
            }
            case Success::Ok:
            {
                auto success = _result._Ok;
                uses = success;
                break;
            }
        }
    };
    const auto statements_start = lexer.position;
    Vector<StatementSyntax>* statements = nullptr;
    {
        auto _result = parse_statement_list(rp, ep);
        switch (_result._tag)
        {
            case Success::Error:
            {
                auto error = _result._Error;
                {
                    {
                        auto _result = error;
                        switch (_result._tag)
                        {
                            case ParserError::Different:
                            {
                                auto d = _result._Different;
                                {
                                };
                                break;
                            }
                            case ParserError::Invalid:
                            {
                                auto i = _result._Invalid;
                                return Result<BlockSyntax, ParserError>(i);
                                break;
                            }
                        }
                    };
                };
                break;
            }
            case Success::Ok:
            {
                auto success = _result._Ok;
                statements = success;
                break;
            }
        }
    };
    const auto start_right_curly_4 = lexer.previous_position;
    const auto success_right_curly_4 = lexer.parse_punctuation('}');
    if (success_right_curly_4 == false) {
        return Result<BlockSyntax, ParserError>(ParserError(InvalidSyntax(start_right_curly_4, lexer.position, String(ep, "}"))));
    };
    const auto end = lexer.position;
    return Result<BlockSyntax, ParserError>(BlockSyntax(start, end, uses, statements));
}

Result<IfSyntax, ParserError> Parser::parse_if(Page* rp, Page* ep) {
    const auto start = lexer.previous_position;
    const auto start_if_1 = lexer.previous_position;
    const auto success_if_1 = lexer.parse_keyword(*keywords_index.get(13));
    if (success_if_1 == false) {
        return Result<IfSyntax, ParserError>(ParserError(DifferentSyntax()));
    };
    const auto condition_start = lexer.position;
    const auto _condition_result = parse_operand_list(rp, ep);
    auto condition = _condition_result._Ok;
    if (_condition_result._tag == Success::Error) {
        const auto _condition_Error = _condition_result._Error;
        switch (_condition_Error._tag) {
            case ParserError::Different: {
                const auto d = _condition_Error._Different;
                return Result<IfSyntax, ParserError>(InvalidSyntax(condition_start, lexer.position, String(ep, "a valid Operand syntax")));
                break;
            }
            case ParserError::Invalid: {
                const auto i = _condition_Error._Invalid;
                return Result<IfSyntax, ParserError>(i);
                break;
            }
        }
    }
    ;
    const auto start_colon_3 = lexer.previous_position;
    const auto success_colon_3 = lexer.parse_colon();
    if (success_colon_3 == false) {
        return Result<IfSyntax, ParserError>(ParserError(InvalidSyntax(start_colon_3, lexer.position, String(ep, "a colon or a line feed"))));
    };
    const auto consequent_start = lexer.position;
    const auto _consequent_result = parse_command(rp, ep);
    auto consequent = _consequent_result._Ok;
    if (_consequent_result._tag == Success::Error) {
        const auto _consequent_Error = _consequent_result._Error;
        switch (_consequent_Error._tag) {
            case ParserError::Different: {
                const auto d = _consequent_Error._Different;
                return Result<IfSyntax, ParserError>(InvalidSyntax(consequent_start, lexer.position, String(ep, "a valid Command syntax")));
                break;
            }
            case ParserError::Invalid: {
                const auto i = _consequent_Error._Invalid;
                return Result<IfSyntax, ParserError>(i);
                break;
            }
        }
    }
    ;
    const auto alternative_start = lexer.position;
    ElseSyntax* alternative = nullptr;
    {
        auto _result = parse_else(rp, ep);
        switch (_result._tag)
        {
            case Success::Error:
            {
                auto error = _result._Error;
                {
                    {
                        auto _result = error;
                        switch (_result._tag)
                        {
                            case ParserError::Different:
                            {
                                auto d = _result._Different;
                                {
                                };
                                break;
                            }
                            case ParserError::Invalid:
                            {
                                auto i = _result._Invalid;
                                return Result<IfSyntax, ParserError>(i);
                                break;
                            }
                        }
                    };
                };
                break;
            }
            case Success::Ok:
            {
                auto success = _result._Ok;
                alternative = new (alignof(ElseSyntax), rp) ElseSyntax(success);
                break;
            }
        }
    };
    const auto end = lexer.position;
    return Result<IfSyntax, ParserError>(IfSyntax(start, end, *condition, consequent, alternative));
}

Result<ElseSyntax, ParserError> Parser::parse_else(Page* rp, Page* ep) {
    const auto start = lexer.previous_position;
    const auto start_else_1 = lexer.previous_position;
    const auto success_else_1 = lexer.parse_keyword(*keywords_index.get(8));
    if (success_else_1 == false) {
        return Result<ElseSyntax, ParserError>(ParserError(DifferentSyntax()));
    };
    const auto start_colon_2 = lexer.previous_position;
    const auto success_colon_2 = lexer.parse_colon();
    const auto alternative_start = lexer.position;
    const auto _alternative_result = parse_command(rp, ep);
    auto alternative = _alternative_result._Ok;
    if (_alternative_result._tag == Success::Error) {
        const auto _alternative_Error = _alternative_result._Error;
        switch (_alternative_Error._tag) {
            case ParserError::Different: {
                const auto d = _alternative_Error._Different;
                return Result<ElseSyntax, ParserError>(InvalidSyntax(alternative_start, lexer.position, String(ep, "a valid Command syntax")));
                break;
            }
            case ParserError::Invalid: {
                const auto i = _alternative_Error._Invalid;
                return Result<ElseSyntax, ParserError>(i);
                break;
            }
        }
    }
    ;
    const auto end = lexer.position;
    return Result<ElseSyntax, ParserError>(ElseSyntax(start, end, alternative));
}

Result<MatchSyntax, ParserError> Parser::parse_match(Page* rp, Page* ep) {
    const auto start = lexer.previous_position;
    const auto start_match_1 = lexer.previous_position;
    const auto success_match_1 = lexer.parse_keyword(*keywords_index.get(24));
    if (success_match_1 == false) {
        return Result<MatchSyntax, ParserError>(ParserError(DifferentSyntax()));
    };
    const auto scrutinee_start = lexer.position;
    const auto _scrutinee_result = parse_operand_list(rp, ep);
    auto scrutinee = _scrutinee_result._Ok;
    if (_scrutinee_result._tag == Success::Error) {
        const auto _scrutinee_Error = _scrutinee_result._Error;
        switch (_scrutinee_Error._tag) {
            case ParserError::Different: {
                const auto d = _scrutinee_Error._Different;
                return Result<MatchSyntax, ParserError>(InvalidSyntax(scrutinee_start, lexer.position, String(ep, "a valid Operand syntax")));
                break;
            }
            case ParserError::Invalid: {
                const auto i = _scrutinee_Error._Invalid;
                return Result<MatchSyntax, ParserError>(i);
                break;
            }
        }
    }
    ;
    const auto start_colon_3 = lexer.previous_position;
    const auto success_colon_3 = lexer.parse_colon();
    if (success_colon_3 == false) {
        return Result<MatchSyntax, ParserError>(ParserError(InvalidSyntax(start_colon_3, lexer.position, String(ep, "a colon or a line feed"))));
    };
    const auto branches_start = lexer.position;
    const auto _branches_result = parse_branch_list(rp, ep);
    auto branches = _branches_result._Ok;
    if (_branches_result._tag == Success::Error) {
        const auto _branches_Error = _branches_result._Error;
        switch (_branches_Error._tag) {
            case ParserError::Different: {
                const auto d = _branches_Error._Different;
                return Result<MatchSyntax, ParserError>(InvalidSyntax(branches_start, lexer.position, String(ep, "a valid Branch syntax")));
                break;
            }
            case ParserError::Invalid: {
                const auto i = _branches_Error._Invalid;
                return Result<MatchSyntax, ParserError>(i);
                break;
            }
        }
    }
    ;
    const auto alternative_start = lexer.position;
    DefaultSyntax* alternative = nullptr;
    {
        auto _result = parse_default(rp, ep);
        switch (_result._tag)
        {
            case Success::Error:
            {
                auto error = _result._Error;
                {
                    {
                        auto _result = error;
                        switch (_result._tag)
                        {
                            case ParserError::Different:
                            {
                                auto d = _result._Different;
                                {
                                };
                                break;
                            }
                            case ParserError::Invalid:
                            {
                                auto i = _result._Invalid;
                                return Result<MatchSyntax, ParserError>(i);
                                break;
                            }
                        }
                    };
                };
                break;
            }
            case Success::Ok:
            {
                auto success = _result._Ok;
                alternative = new (alignof(DefaultSyntax), rp) DefaultSyntax(success);
                break;
            }
        }
    };
    const auto end = lexer.position;
    return Result<MatchSyntax, ParserError>(MatchSyntax(start, end, *scrutinee, *branches, alternative));
}

Result<Vector<BranchSyntax>*, ParserError> Parser::parse_branch_list(Page* rp, Page* ep) {
    auto r = Region();
    auto list = new (alignof(List<BranchSyntax>), r.get_page()) List<BranchSyntax>();
    while (true) {
        const auto _node_result = parse_branch(rp, ep);
        auto node = _node_result._Ok;
        if (_node_result._tag == Success::Error) {
            const auto _node_Error = _node_result._Error;
            switch (_node_Error._tag) {
                case ParserError::Invalid: {
                    const auto i = _node_Error._Invalid;
                    return Result<Vector<BranchSyntax>*, ParserError>(i);
                    break;
                }
                case ParserError::Different: {
                    const auto d = _node_Error._Different;
                    {
                        if ((*list).head == 0) {
                            return Result<Vector<BranchSyntax>*, ParserError>(d);
                        }
                        else {
                            return Result<Vector<BranchSyntax>*, ParserError>(new (alignof(Vector<BranchSyntax>), rp) Vector<BranchSyntax>(rp, *list));
                        };
                    };
                    break;
                }
            }
        }
        (*list).add(node);
    };
}

Result<BranchSyntax, ParserError> Parser::parse_branch(Page* rp, Page* ep) {
    const auto start = lexer.previous_position;
    const auto cases_start = lexer.position;
    const auto _cases_result = parse_case_list(rp, ep);
    auto cases = _cases_result._Ok;
    if (_cases_result._tag == Success::Error) {
        const auto _cases_Error = _cases_result._Error;
        switch (_cases_Error._tag) {
            case ParserError::Different: {
                const auto d = _cases_Error._Different;
                return Result<BranchSyntax, ParserError>(d);
                break;
            }
            case ParserError::Invalid: {
                const auto i = _cases_Error._Invalid;
                return Result<BranchSyntax, ParserError>(i);
                break;
            }
        }
    }
    ;
    const auto start_colon_2 = lexer.previous_position;
    const auto success_colon_2 = lexer.parse_colon();
    if (success_colon_2 == false) {
        return Result<BranchSyntax, ParserError>(ParserError(InvalidSyntax(start_colon_2, lexer.position, String(ep, "a colon or a line feed"))));
    };
    const auto consequent_start = lexer.position;
    const auto _consequent_result = parse_statement(rp, ep);
    auto consequent = _consequent_result._Ok;
    if (_consequent_result._tag == Success::Error) {
        const auto _consequent_Error = _consequent_result._Error;
        switch (_consequent_Error._tag) {
            case ParserError::Different: {
                const auto d = _consequent_Error._Different;
                return Result<BranchSyntax, ParserError>(InvalidSyntax(consequent_start, lexer.position, String(ep, "a valid Statement syntax")));
                break;
            }
            case ParserError::Invalid: {
                const auto i = _consequent_Error._Invalid;
                return Result<BranchSyntax, ParserError>(i);
                break;
            }
        }
    }
    ;
    const auto end = lexer.position;
    return Result<BranchSyntax, ParserError>(BranchSyntax(start, end, *cases, consequent));
}

Result<Vector<CaseSyntax>*, ParserError> Parser::parse_case_list(Page* rp, Page* ep) {
    auto r = Region();
    auto list = new (alignof(List<CaseSyntax>), r.get_page()) List<CaseSyntax>();
    while (true) {
        const auto _node_result = parse_case(rp, ep);
        auto node = _node_result._Ok;
        if (_node_result._tag == Success::Error) {
            const auto _node_Error = _node_result._Error;
            switch (_node_Error._tag) {
                case ParserError::Invalid: {
                    const auto i = _node_Error._Invalid;
                    return Result<Vector<CaseSyntax>*, ParserError>(i);
                    break;
                }
                case ParserError::Different: {
                    const auto d = _node_Error._Different;
                    {
                        if ((*list).head == 0) {
                            return Result<Vector<CaseSyntax>*, ParserError>(d);
                        }
                        else {
                            return Result<Vector<CaseSyntax>*, ParserError>(new (alignof(Vector<CaseSyntax>), rp) Vector<CaseSyntax>(rp, *list));
                        };
                    };
                    break;
                }
            }
        }
        (*list).add(node);
    };
}

Result<CaseSyntax, ParserError> Parser::parse_case(Page* rp, Page* ep) {
    const auto start = lexer.previous_position;
    const auto start_case_1 = lexer.previous_position;
    const auto success_case_1 = lexer.parse_keyword(*keywords_index.get(1));
    if (success_case_1 == false) {
        return Result<CaseSyntax, ParserError>(ParserError(DifferentSyntax()));
    };
    const auto condition_start = lexer.position;
    const auto _condition_result = parse_operand_list(rp, ep);
    auto condition = _condition_result._Ok;
    if (_condition_result._tag == Success::Error) {
        const auto _condition_Error = _condition_result._Error;
        switch (_condition_Error._tag) {
            case ParserError::Different: {
                const auto d = _condition_Error._Different;
                return Result<CaseSyntax, ParserError>(InvalidSyntax(condition_start, lexer.position, String(ep, "a valid Operand syntax")));
                break;
            }
            case ParserError::Invalid: {
                const auto i = _condition_Error._Invalid;
                return Result<CaseSyntax, ParserError>(i);
                break;
            }
        }
    }
    ;
    const auto end = lexer.position;
    return Result<CaseSyntax, ParserError>(CaseSyntax(start, end, *condition));
}

Result<DefaultSyntax, ParserError> Parser::parse_default(Page* rp, Page* ep) {
    const auto start = lexer.previous_position;
    const auto start_default_1 = lexer.previous_position;
    const auto success_default_1 = lexer.parse_keyword(*keywords_index.get(5));
    if (success_default_1 == false) {
        return Result<DefaultSyntax, ParserError>(ParserError(DifferentSyntax()));
    };
    const auto start_colon_2 = lexer.previous_position;
    const auto success_colon_2 = lexer.parse_colon();
    const auto alternative_start = lexer.position;
    const auto _alternative_result = parse_command(rp, ep);
    auto alternative = _alternative_result._Ok;
    if (_alternative_result._tag == Success::Error) {
        const auto _alternative_Error = _alternative_result._Error;
        switch (_alternative_Error._tag) {
            case ParserError::Different: {
                const auto d = _alternative_Error._Different;
                return Result<DefaultSyntax, ParserError>(InvalidSyntax(alternative_start, lexer.position, String(ep, "a valid Command syntax")));
                break;
            }
            case ParserError::Invalid: {
                const auto i = _alternative_Error._Invalid;
                return Result<DefaultSyntax, ParserError>(i);
                break;
            }
        }
    }
    ;
    const auto end = lexer.position;
    return Result<DefaultSyntax, ParserError>(DefaultSyntax(start, end, alternative));
}

Result<Vector<StatementSyntax>*, ParserError> Parser::parse_statement_list(Page* rp, Page* ep) {
    auto r = Region();
    auto list = new (alignof(List<StatementSyntax>), r.get_page()) List<StatementSyntax>();
    while (true) {
        const auto _node_result = parse_statement(rp, ep);
        auto node = _node_result._Ok;
        if (_node_result._tag == Success::Error) {
            const auto _node_Error = _node_result._Error;
            switch (_node_Error._tag) {
                case ParserError::Invalid: {
                    const auto i = _node_Error._Invalid;
                    return Result<Vector<StatementSyntax>*, ParserError>(i);
                    break;
                }
                case ParserError::Different: {
                    const auto d = _node_Error._Different;
                    {
                        if ((*list).head == 0) {
                            return Result<Vector<StatementSyntax>*, ParserError>(d);
                        }
                        else {
                            return Result<Vector<StatementSyntax>*, ParserError>(new (alignof(Vector<StatementSyntax>), rp) Vector<StatementSyntax>(rp, *list));
                        };
                    };
                    break;
                }
            }
        }
        (*list).add(node);
    };
}

Result<StatementSyntax, ParserError> Parser::parse_statement(Page* rp, Page* ep) {
    const auto start = lexer.previous_position;
    const auto command_start = lexer.position;
    const auto _command_result = parse_command(rp, ep);
    auto command = _command_result._Ok;
    if (_command_result._tag == Success::Error) {
        const auto _command_Error = _command_result._Error;
        switch (_command_Error._tag) {
            case ParserError::Different: {
                const auto d = _command_Error._Different;
                return Result<StatementSyntax, ParserError>(d);
                break;
            }
            case ParserError::Invalid: {
                const auto i = _command_Error._Invalid;
                return Result<StatementSyntax, ParserError>(i);
                break;
            }
        }
    }
    ;
    const auto start_colon_2 = lexer.previous_position;
    const auto success_colon_2 = lexer.parse_colon();
    const auto end = lexer.position;
    return Result<StatementSyntax, ParserError>(StatementSyntax(start, end, command));
}

Result<WhileSyntax, ParserError> Parser::parse_while(Page* rp, Page* ep) {
    const auto start = lexer.previous_position;
    const auto start_while_1 = lexer.previous_position;
    const auto success_while_1 = lexer.parse_keyword(*keywords_index.get(43));
    if (success_while_1 == false) {
        return Result<WhileSyntax, ParserError>(ParserError(DifferentSyntax()));
    };
    const auto condition_start = lexer.position;
    const auto _condition_result = parse_condition(rp, ep);
    auto condition = _condition_result._Ok;
    if (_condition_result._tag == Success::Error) {
        const auto _condition_Error = _condition_result._Error;
        switch (_condition_Error._tag) {
            case ParserError::Different: {
                const auto d = _condition_Error._Different;
                return Result<WhileSyntax, ParserError>(InvalidSyntax(condition_start, lexer.position, String(ep, "a valid Condition syntax")));
                break;
            }
            case ParserError::Invalid: {
                const auto i = _condition_Error._Invalid;
                return Result<WhileSyntax, ParserError>(i);
                break;
            }
        }
    }
    ;
    const auto start_colon_3 = lexer.previous_position;
    const auto success_colon_3 = lexer.parse_colon();
    if (success_colon_3 == false) {
        return Result<WhileSyntax, ParserError>(ParserError(InvalidSyntax(start_colon_3, lexer.position, String(ep, "a colon or a line feed"))));
    };
    const auto name_start = lexer.position;
    LabelSyntax* name = nullptr;
    {
        auto _result = parse_label(rp, ep);
        switch (_result._tag)
        {
            case Success::Error:
            {
                auto error = _result._Error;
                {
                    {
                        auto _result = error;
                        switch (_result._tag)
                        {
                            case ParserError::Different:
                            {
                                auto d = _result._Different;
                                {
                                };
                                break;
                            }
                            case ParserError::Invalid:
                            {
                                auto i = _result._Invalid;
                                return Result<WhileSyntax, ParserError>(i);
                                break;
                            }
                        }
                    };
                };
                break;
            }
            case Success::Ok:
            {
                auto success = _result._Ok;
                name = new (alignof(LabelSyntax), rp) LabelSyntax(success);
                break;
            }
        }
    };
    const auto action_start = lexer.position;
    const auto _action_result = parse_action(rp, ep);
    auto action = _action_result._Ok;
    if (_action_result._tag == Success::Error) {
        const auto _action_Error = _action_result._Error;
        switch (_action_Error._tag) {
            case ParserError::Different: {
                const auto d = _action_Error._Different;
                return Result<WhileSyntax, ParserError>(InvalidSyntax(action_start, lexer.position, String(ep, "a valid Action syntax")));
                break;
            }
            case ParserError::Invalid: {
                const auto i = _action_Error._Invalid;
                return Result<WhileSyntax, ParserError>(i);
                break;
            }
        }
    }
    ;
    const auto end = lexer.position;
    return Result<WhileSyntax, ParserError>(WhileSyntax(start, end, condition, name, action));
}

Result<ChooseSyntax, ParserError> Parser::parse_choose(Page* rp, Page* ep) {
    const auto start = lexer.previous_position;
    const auto start_choose_1 = lexer.previous_position;
    const auto success_choose_1 = lexer.parse_keyword(*keywords_index.get(2));
    if (success_choose_1 == false) {
        return Result<ChooseSyntax, ParserError>(ParserError(DifferentSyntax()));
    };
    const auto condition_start = lexer.position;
    const auto _condition_result = parse_operand_list(rp, ep);
    auto condition = _condition_result._Ok;
    if (_condition_result._tag == Success::Error) {
        const auto _condition_Error = _condition_result._Error;
        switch (_condition_Error._tag) {
            case ParserError::Different: {
                const auto d = _condition_Error._Different;
                return Result<ChooseSyntax, ParserError>(InvalidSyntax(condition_start, lexer.position, String(ep, "a valid Operand syntax")));
                break;
            }
            case ParserError::Invalid: {
                const auto i = _condition_Error._Invalid;
                return Result<ChooseSyntax, ParserError>(i);
                break;
            }
        }
    }
    ;
    const auto start_colon_3 = lexer.previous_position;
    const auto success_colon_3 = lexer.parse_colon();
    if (success_colon_3 == false) {
        return Result<ChooseSyntax, ParserError>(ParserError(InvalidSyntax(start_colon_3, lexer.position, String(ep, "a colon or a line feed"))));
    };
    const auto cases_start = lexer.position;
    Vector<WhenSyntax>* cases = nullptr;
    {
        auto _result = parse_when_list(rp, ep);
        switch (_result._tag)
        {
            case Success::Error:
            {
                auto error = _result._Error;
                {
                    {
                        auto _result = error;
                        switch (_result._tag)
                        {
                            case ParserError::Different:
                            {
                                auto d = _result._Different;
                                {
                                };
                                break;
                            }
                            case ParserError::Invalid:
                            {
                                auto i = _result._Invalid;
                                return Result<ChooseSyntax, ParserError>(i);
                                break;
                            }
                        }
                    };
                };
                break;
            }
            case Success::Ok:
            {
                auto success = _result._Ok;
                cases = success;
                break;
            }
        }
    };
    const auto alternative_start = lexer.position;
    DefaultSyntax* alternative = nullptr;
    {
        auto _result = parse_default(rp, ep);
        switch (_result._tag)
        {
            case Success::Error:
            {
                auto error = _result._Error;
                {
                    {
                        auto _result = error;
                        switch (_result._tag)
                        {
                            case ParserError::Different:
                            {
                                auto d = _result._Different;
                                {
                                };
                                break;
                            }
                            case ParserError::Invalid:
                            {
                                auto i = _result._Invalid;
                                return Result<ChooseSyntax, ParserError>(i);
                                break;
                            }
                        }
                    };
                };
                break;
            }
            case Success::Ok:
            {
                auto success = _result._Ok;
                alternative = new (alignof(DefaultSyntax), rp) DefaultSyntax(success);
                break;
            }
        }
    };
    const auto end = lexer.position;
    return Result<ChooseSyntax, ParserError>(ChooseSyntax(start, end, *condition, cases, alternative));
}

Result<TrySyntax, ParserError> Parser::parse_try(Page* rp, Page* ep) {
    const auto start = lexer.previous_position;
    const auto start_try_1 = lexer.previous_position;
    const auto success_try_1 = lexer.parse_keyword(*keywords_index.get(38));
    if (success_try_1 == false) {
        return Result<TrySyntax, ParserError>(ParserError(DifferentSyntax()));
    };
    const auto condition_start = lexer.position;
    const auto _condition_result = parse_condition(rp, ep);
    auto condition = _condition_result._Ok;
    if (_condition_result._tag == Success::Error) {
        const auto _condition_Error = _condition_result._Error;
        switch (_condition_Error._tag) {
            case ParserError::Different: {
                const auto d = _condition_Error._Different;
                return Result<TrySyntax, ParserError>(InvalidSyntax(condition_start, lexer.position, String(ep, "a valid Condition syntax")));
                break;
            }
            case ParserError::Invalid: {
                const auto i = _condition_Error._Invalid;
                return Result<TrySyntax, ParserError>(i);
                break;
            }
        }
    }
    ;
    const auto start_colon_3 = lexer.previous_position;
    const auto success_colon_3 = lexer.parse_colon();
    if (success_colon_3 == false) {
        return Result<TrySyntax, ParserError>(ParserError(InvalidSyntax(start_colon_3, lexer.position, String(ep, "a colon or a line feed"))));
    };
    const auto cases_start = lexer.position;
    Vector<WhenSyntax>* cases = nullptr;
    {
        auto _result = parse_when_list(rp, ep);
        switch (_result._tag)
        {
            case Success::Error:
            {
                auto error = _result._Error;
                {
                    {
                        auto _result = error;
                        switch (_result._tag)
                        {
                            case ParserError::Different:
                            {
                                auto d = _result._Different;
                                {
                                };
                                break;
                            }
                            case ParserError::Invalid:
                            {
                                auto i = _result._Invalid;
                                return Result<TrySyntax, ParserError>(i);
                                break;
                            }
                        }
                    };
                };
                break;
            }
            case Success::Ok:
            {
                auto success = _result._Ok;
                cases = success;
                break;
            }
        }
    };
    const auto dropper_start = lexer.position;
    DefaultSyntax* dropper = nullptr;
    {
        auto _result = parse_default(rp, ep);
        switch (_result._tag)
        {
            case Success::Error:
            {
                auto error = _result._Error;
                {
                    {
                        auto _result = error;
                        switch (_result._tag)
                        {
                            case ParserError::Different:
                            {
                                auto d = _result._Different;
                                {
                                };
                                break;
                            }
                            case ParserError::Invalid:
                            {
                                auto i = _result._Invalid;
                                return Result<TrySyntax, ParserError>(i);
                                break;
                            }
                        }
                    };
                };
                break;
            }
            case Success::Ok:
            {
                auto success = _result._Ok;
                dropper = new (alignof(DefaultSyntax), rp) DefaultSyntax(success);
                break;
            }
        }
    };
    const auto end = lexer.position;
    return Result<TrySyntax, ParserError>(TrySyntax(start, end, condition, cases, dropper));
}

Result<ConditionSyntax, ParserError> Parser::parse_condition(Page* rp, Page* ep) {
    {
        {
            auto _result = parse_operation(rp, ep);
            switch (_result._tag)
            {
                case Success::Error:
                {
                    auto error = _result._Error;
                    {
                        {
                            auto _result = error;
                            switch (_result._tag)
                            {
                                case ParserError::Invalid:
                                {
                                    auto i = _result._Invalid;
                                    return Result<ConditionSyntax, ParserError>(i);
                                    break;
                                }
                                case ParserError::Different:
                                {
                                    auto d = _result._Different;
                                    {
                                    };
                                    break;
                                }
                            }
                        };
                    };
                    break;
                }
                case Success::Ok:
                {
                    auto node = _result._Ok;
                    return Result<ConditionSyntax, ParserError>(ConditionSyntax(OperationSyntax(node)));
                    break;
                }
            }
        };
    };
    {
        {
            auto _result = parse_let(rp, ep);
            switch (_result._tag)
            {
                case Success::Error:
                {
                    auto error = _result._Error;
                    {
                        {
                            auto _result = error;
                            switch (_result._tag)
                            {
                                case ParserError::Invalid:
                                {
                                    auto i = _result._Invalid;
                                    return Result<ConditionSyntax, ParserError>(i);
                                    break;
                                }
                                case ParserError::Different:
                                {
                                    auto d = _result._Different;
                                    {
                                    };
                                    break;
                                }
                            }
                        };
                    };
                    break;
                }
                case Success::Ok:
                {
                    auto node = _result._Ok;
                    return Result<ConditionSyntax, ParserError>(ConditionSyntax(LetSyntax(node)));
                    break;
                }
            }
        };
    };
    return Result<ConditionSyntax, ParserError>(ParserError(DifferentSyntax()));
}

Result<Vector<WhenSyntax>*, ParserError> Parser::parse_when_list(Page* rp, Page* ep) {
    auto r = Region();
    auto list = new (alignof(List<WhenSyntax>), r.get_page()) List<WhenSyntax>();
    while (true) {
        const auto _node_result = parse_when(rp, ep);
        auto node = _node_result._Ok;
        if (_node_result._tag == Success::Error) {
            const auto _node_Error = _node_result._Error;
            switch (_node_Error._tag) {
                case ParserError::Invalid: {
                    const auto i = _node_Error._Invalid;
                    return Result<Vector<WhenSyntax>*, ParserError>(i);
                    break;
                }
                case ParserError::Different: {
                    const auto d = _node_Error._Different;
                    {
                        if ((*list).head == 0) {
                            return Result<Vector<WhenSyntax>*, ParserError>(d);
                        }
                        else {
                            return Result<Vector<WhenSyntax>*, ParserError>(new (alignof(Vector<WhenSyntax>), rp) Vector<WhenSyntax>(rp, *list));
                        };
                    };
                    break;
                }
            }
        }
        (*list).add(node);
    };
}

Result<WhenSyntax, ParserError> Parser::parse_when(Page* rp, Page* ep) {
    const auto start = lexer.previous_position;
    const auto start_when_1 = lexer.previous_position;
    const auto success_when_1 = lexer.parse_keyword(*keywords_index.get(42));
    if (success_when_1 == false) {
        return Result<WhenSyntax, ParserError>(ParserError(DifferentSyntax()));
    };
    const auto start_name = lexer.previous_position;
    const auto name = lexer.parse_identifier(rp, keywords);
    if (name == nullptr) {
        return Result<WhenSyntax, ParserError>(ParserError(InvalidSyntax(start_name, lexer.position, String(ep, "an identifier"))));
    };
    const auto start_colon_3 = lexer.previous_position;
    const auto success_colon_3 = lexer.parse_colon();
    if (success_colon_3 == false) {
        return Result<WhenSyntax, ParserError>(ParserError(InvalidSyntax(start_colon_3, lexer.position, String(ep, "a colon or a line feed"))));
    };
    const auto variant_start = lexer.position;
    const auto _variant_result = parse_name(rp, ep);
    auto variant = _variant_result._Ok;
    if (_variant_result._tag == Success::Error) {
        const auto _variant_Error = _variant_result._Error;
        switch (_variant_Error._tag) {
            case ParserError::Different: {
                const auto d = _variant_Error._Different;
                return Result<WhenSyntax, ParserError>(InvalidSyntax(variant_start, lexer.position, String(ep, "a valid Name syntax")));
                break;
            }
            case ParserError::Invalid: {
                const auto i = _variant_Error._Invalid;
                return Result<WhenSyntax, ParserError>(i);
                break;
            }
        }
    }
    ;
    const auto start_colon_5 = lexer.previous_position;
    const auto success_colon_5 = lexer.parse_colon();
    const auto command_start = lexer.position;
    const auto _command_result = parse_command(rp, ep);
    auto command = _command_result._Ok;
    if (_command_result._tag == Success::Error) {
        const auto _command_Error = _command_result._Error;
        switch (_command_Error._tag) {
            case ParserError::Different: {
                const auto d = _command_Error._Different;
                return Result<WhenSyntax, ParserError>(InvalidSyntax(command_start, lexer.position, String(ep, "a valid Command syntax")));
                break;
            }
            case ParserError::Invalid: {
                const auto i = _command_Error._Invalid;
                return Result<WhenSyntax, ParserError>(i);
                break;
            }
        }
    }
    ;
    const auto start_colon_7 = lexer.previous_position;
    const auto success_colon_7 = lexer.parse_colon();
    const auto end = lexer.position;
    return Result<WhenSyntax, ParserError>(WhenSyntax(start, end, *name, variant, command));
}

Result<CommandSyntax, ParserError> Parser::parse_command(Page* rp, Page* ep) {
    {
        {
            auto _result = parse_operation(rp, ep);
            switch (_result._tag)
            {
                case Success::Error:
                {
                    auto error = _result._Error;
                    {
                        {
                            auto _result = error;
                            switch (_result._tag)
                            {
                                case ParserError::Invalid:
                                {
                                    auto i = _result._Invalid;
                                    return Result<CommandSyntax, ParserError>(i);
                                    break;
                                }
                                case ParserError::Different:
                                {
                                    auto d = _result._Different;
                                    {
                                    };
                                    break;
                                }
                            }
                        };
                    };
                    break;
                }
                case Success::Ok:
                {
                    auto node = _result._Ok;
                    return Result<CommandSyntax, ParserError>(CommandSyntax(OperationSyntax(node)));
                    break;
                }
            }
        };
    };
    {
        {
            auto _result = parse_let(rp, ep);
            switch (_result._tag)
            {
                case Success::Error:
                {
                    auto error = _result._Error;
                    {
                        {
                            auto _result = error;
                            switch (_result._tag)
                            {
                                case ParserError::Invalid:
                                {
                                    auto i = _result._Invalid;
                                    return Result<CommandSyntax, ParserError>(i);
                                    break;
                                }
                                case ParserError::Different:
                                {
                                    auto d = _result._Different;
                                    {
                                    };
                                    break;
                                }
                            }
                        };
                    };
                    break;
                }
                case Success::Ok:
                {
                    auto node = _result._Ok;
                    return Result<CommandSyntax, ParserError>(CommandSyntax(LetSyntax(node)));
                    break;
                }
            }
        };
    };
    {
        {
            auto _result = parse_var(rp, ep);
            switch (_result._tag)
            {
                case Success::Error:
                {
                    auto error = _result._Error;
                    {
                        {
                            auto _result = error;
                            switch (_result._tag)
                            {
                                case ParserError::Invalid:
                                {
                                    auto i = _result._Invalid;
                                    return Result<CommandSyntax, ParserError>(i);
                                    break;
                                }
                                case ParserError::Different:
                                {
                                    auto d = _result._Different;
                                    {
                                    };
                                    break;
                                }
                            }
                        };
                    };
                    break;
                }
                case Success::Ok:
                {
                    auto node = _result._Ok;
                    return Result<CommandSyntax, ParserError>(CommandSyntax(VarSyntax(node)));
                    break;
                }
            }
        };
    };
    {
        {
            auto _result = parse_mutable(rp, ep);
            switch (_result._tag)
            {
                case Success::Error:
                {
                    auto error = _result._Error;
                    {
                        {
                            auto _result = error;
                            switch (_result._tag)
                            {
                                case ParserError::Invalid:
                                {
                                    auto i = _result._Invalid;
                                    return Result<CommandSyntax, ParserError>(i);
                                    break;
                                }
                                case ParserError::Different:
                                {
                                    auto d = _result._Different;
                                    {
                                    };
                                    break;
                                }
                            }
                        };
                    };
                    break;
                }
                case Success::Ok:
                {
                    auto node = _result._Ok;
                    return Result<CommandSyntax, ParserError>(CommandSyntax(MutableSyntax(node)));
                    break;
                }
            }
        };
    };
    {
        {
            auto _result = parse_set(rp, ep);
            switch (_result._tag)
            {
                case Success::Error:
                {
                    auto error = _result._Error;
                    {
                        {
                            auto _result = error;
                            switch (_result._tag)
                            {
                                case ParserError::Invalid:
                                {
                                    auto i = _result._Invalid;
                                    return Result<CommandSyntax, ParserError>(i);
                                    break;
                                }
                                case ParserError::Different:
                                {
                                    auto d = _result._Different;
                                    {
                                    };
                                    break;
                                }
                            }
                        };
                    };
                    break;
                }
                case Success::Ok:
                {
                    auto node = _result._Ok;
                    return Result<CommandSyntax, ParserError>(CommandSyntax(SetSyntax(node)));
                    break;
                }
            }
        };
    };
    {
        {
            auto _result = parse_continue(rp, ep);
            switch (_result._tag)
            {
                case Success::Error:
                {
                    auto error = _result._Error;
                    {
                        {
                            auto _result = error;
                            switch (_result._tag)
                            {
                                case ParserError::Invalid:
                                {
                                    auto i = _result._Invalid;
                                    return Result<CommandSyntax, ParserError>(i);
                                    break;
                                }
                                case ParserError::Different:
                                {
                                    auto d = _result._Different;
                                    {
                                    };
                                    break;
                                }
                            }
                        };
                    };
                    break;
                }
                case Success::Ok:
                {
                    auto node = _result._Ok;
                    return Result<CommandSyntax, ParserError>(CommandSyntax(ContinueSyntax(node)));
                    break;
                }
            }
        };
    };
    {
        {
            auto _result = parse_break(rp, ep);
            switch (_result._tag)
            {
                case Success::Error:
                {
                    auto error = _result._Error;
                    {
                        {
                            auto _result = error;
                            switch (_result._tag)
                            {
                                case ParserError::Invalid:
                                {
                                    auto i = _result._Invalid;
                                    return Result<CommandSyntax, ParserError>(i);
                                    break;
                                }
                                case ParserError::Different:
                                {
                                    auto d = _result._Different;
                                    {
                                    };
                                    break;
                                }
                            }
                        };
                    };
                    break;
                }
                case Success::Ok:
                {
                    auto node = _result._Ok;
                    return Result<CommandSyntax, ParserError>(CommandSyntax(BreakSyntax(node)));
                    break;
                }
            }
        };
    };
    {
        {
            auto _result = parse_return(rp, ep);
            switch (_result._tag)
            {
                case Success::Error:
                {
                    auto error = _result._Error;
                    {
                        {
                            auto _result = error;
                            switch (_result._tag)
                            {
                                case ParserError::Invalid:
                                {
                                    auto i = _result._Invalid;
                                    return Result<CommandSyntax, ParserError>(i);
                                    break;
                                }
                                case ParserError::Different:
                                {
                                    auto d = _result._Different;
                                    {
                                    };
                                    break;
                                }
                            }
                        };
                    };
                    break;
                }
                case Success::Ok:
                {
                    auto node = _result._Ok;
                    return Result<CommandSyntax, ParserError>(CommandSyntax(ReturnSyntax(node)));
                    break;
                }
            }
        };
    };
    {
        {
            auto _result = parse_throw(rp, ep);
            switch (_result._tag)
            {
                case Success::Error:
                {
                    auto error = _result._Error;
                    {
                        {
                            auto _result = error;
                            switch (_result._tag)
                            {
                                case ParserError::Invalid:
                                {
                                    auto i = _result._Invalid;
                                    return Result<CommandSyntax, ParserError>(i);
                                    break;
                                }
                                case ParserError::Different:
                                {
                                    auto d = _result._Different;
                                    {
                                    };
                                    break;
                                }
                            }
                        };
                    };
                    break;
                }
                case Success::Ok:
                {
                    auto node = _result._Ok;
                    return Result<CommandSyntax, ParserError>(CommandSyntax(ThrowSyntax(node)));
                    break;
                }
            }
        };
    };
    return Result<CommandSyntax, ParserError>(ParserError(DifferentSyntax()));
}

Result<LetSyntax, ParserError> Parser::parse_let(Page* rp, Page* ep) {
    const auto start = lexer.previous_position;
    const auto start_let_1 = lexer.previous_position;
    const auto success_let_1 = lexer.parse_keyword(*keywords_index.get(21));
    if (success_let_1 == false) {
        return Result<LetSyntax, ParserError>(ParserError(DifferentSyntax()));
    };
    const auto binding_start = lexer.position;
    const auto _binding_result = parse_binding(rp, ep);
    auto binding = _binding_result._Ok;
    if (_binding_result._tag == Success::Error) {
        const auto _binding_Error = _binding_result._Error;
        switch (_binding_Error._tag) {
            case ParserError::Different: {
                const auto d = _binding_Error._Different;
                return Result<LetSyntax, ParserError>(InvalidSyntax(binding_start, lexer.position, String(ep, "a valid Binding syntax")));
                break;
            }
            case ParserError::Invalid: {
                const auto i = _binding_Error._Invalid;
                return Result<LetSyntax, ParserError>(i);
                break;
            }
        }
    }
    ;
    const auto end = lexer.position;
    return Result<LetSyntax, ParserError>(LetSyntax(start, end, binding));
}

Result<VarSyntax, ParserError> Parser::parse_var(Page* rp, Page* ep) {
    const auto start = lexer.previous_position;
    const auto start_var_1 = lexer.previous_position;
    const auto success_var_1 = lexer.parse_keyword(*keywords_index.get(41));
    if (success_var_1 == false) {
        return Result<VarSyntax, ParserError>(ParserError(DifferentSyntax()));
    };
    const auto binding_start = lexer.position;
    const auto _binding_result = parse_binding(rp, ep);
    auto binding = _binding_result._Ok;
    if (_binding_result._tag == Success::Error) {
        const auto _binding_Error = _binding_result._Error;
        switch (_binding_Error._tag) {
            case ParserError::Different: {
                const auto d = _binding_Error._Different;
                return Result<VarSyntax, ParserError>(InvalidSyntax(binding_start, lexer.position, String(ep, "a valid Binding syntax")));
                break;
            }
            case ParserError::Invalid: {
                const auto i = _binding_Error._Invalid;
                return Result<VarSyntax, ParserError>(i);
                break;
            }
        }
    }
    ;
    const auto end = lexer.position;
    return Result<VarSyntax, ParserError>(VarSyntax(start, end, binding));
}

Result<MutableSyntax, ParserError> Parser::parse_mutable(Page* rp, Page* ep) {
    const auto start = lexer.previous_position;
    const auto start_mutable_1 = lexer.previous_position;
    const auto success_mutable_1 = lexer.parse_keyword(*keywords_index.get(26));
    if (success_mutable_1 == false) {
        return Result<MutableSyntax, ParserError>(ParserError(DifferentSyntax()));
    };
    const auto binding_start = lexer.position;
    const auto _binding_result = parse_binding(rp, ep);
    auto binding = _binding_result._Ok;
    if (_binding_result._tag == Success::Error) {
        const auto _binding_Error = _binding_result._Error;
        switch (_binding_Error._tag) {
            case ParserError::Different: {
                const auto d = _binding_Error._Different;
                return Result<MutableSyntax, ParserError>(InvalidSyntax(binding_start, lexer.position, String(ep, "a valid Binding syntax")));
                break;
            }
            case ParserError::Invalid: {
                const auto i = _binding_Error._Invalid;
                return Result<MutableSyntax, ParserError>(i);
                break;
            }
        }
    }
    ;
    const auto end = lexer.position;
    return Result<MutableSyntax, ParserError>(MutableSyntax(start, end, binding));
}

Result<BindingSyntax, ParserError> Parser::parse_binding(Page* rp, Page* ep) {
    const auto start = lexer.previous_position;
    const auto start_name = lexer.previous_position;
    const auto name = lexer.parse_identifier(rp, keywords);
    if (name == nullptr) {
        return Result<BindingSyntax, ParserError>(ParserError(DifferentSyntax()));
    };
    const auto annotation_start = lexer.position;
    BindingAnnotationSyntax* annotation = nullptr;
    {
        auto _result = parse_bindingannotation(rp, ep);
        switch (_result._tag)
        {
            case Success::Error:
            {
                auto error = _result._Error;
                {
                    {
                        auto _result = error;
                        switch (_result._tag)
                        {
                            case ParserError::Different:
                            {
                                auto d = _result._Different;
                                {
                                };
                                break;
                            }
                            case ParserError::Invalid:
                            {
                                auto i = _result._Invalid;
                                return Result<BindingSyntax, ParserError>(i);
                                break;
                            }
                        }
                    };
                };
                break;
            }
            case Success::Ok:
            {
                auto success = _result._Ok;
                annotation = new (alignof(BindingAnnotationSyntax), rp) BindingAnnotationSyntax(success);
                break;
            }
        }
    };
    const auto operation_start = lexer.position;
    const auto _operation_result = parse_operand_list(rp, ep);
    auto operation = _operation_result._Ok;
    if (_operation_result._tag == Success::Error) {
        const auto _operation_Error = _operation_result._Error;
        switch (_operation_Error._tag) {
            case ParserError::Different: {
                const auto d = _operation_Error._Different;
                return Result<BindingSyntax, ParserError>(InvalidSyntax(operation_start, lexer.position, String(ep, "a valid Operand syntax")));
                break;
            }
            case ParserError::Invalid: {
                const auto i = _operation_Error._Invalid;
                return Result<BindingSyntax, ParserError>(i);
                break;
            }
        }
    }
    ;
    const auto end = lexer.position;
    return Result<BindingSyntax, ParserError>(BindingSyntax(start, end, *name, annotation, *operation));
}

Result<BindingAnnotationSyntax, ParserError> Parser::parse_bindingannotation(Page* rp, Page* ep) {
    const auto start = lexer.previous_position;
    const auto start_colon_1 = lexer.previous_position;
    const auto success_colon_1 = lexer.parse_colon();
    if (success_colon_1 == false) {
        return Result<BindingAnnotationSyntax, ParserError>(ParserError(DifferentSyntax()));
    };
    const auto spec_start = lexer.position;
    const auto _spec_result = parse_bindingspec(rp, ep);
    auto spec = _spec_result._Ok;
    if (_spec_result._tag == Success::Error) {
        const auto _spec_Error = _spec_result._Error;
        switch (_spec_Error._tag) {
            case ParserError::Different: {
                const auto d = _spec_Error._Different;
                return Result<BindingAnnotationSyntax, ParserError>(InvalidSyntax(spec_start, lexer.position, String(ep, "a valid BindingSpec syntax")));
                break;
            }
            case ParserError::Invalid: {
                const auto i = _spec_Error._Invalid;
                return Result<BindingAnnotationSyntax, ParserError>(i);
                break;
            }
        }
    }
    ;
    const auto end = lexer.position;
    return Result<BindingAnnotationSyntax, ParserError>(BindingAnnotationSyntax(start, end, spec));
}

Result<Vector<BindingSpecSyntax>*, ParserError> Parser::parse_bindingspec_list(Page* rp, Page* ep) {
    auto r = Region();
    auto list = new (alignof(List<BindingSpecSyntax>), r.get_page()) List<BindingSpecSyntax>();
    while (true) {
        const auto _node_result = parse_bindingspec(rp, ep);
        auto node = _node_result._Ok;
        if (_node_result._tag == Success::Error) {
            const auto _node_Error = _node_result._Error;
            switch (_node_Error._tag) {
                case ParserError::Invalid: {
                    const auto i = _node_Error._Invalid;
                    return Result<Vector<BindingSpecSyntax>*, ParserError>(i);
                    break;
                }
                case ParserError::Different: {
                    const auto d = _node_Error._Different;
                    {
                        if ((*list).head == 0) {
                            return Result<Vector<BindingSpecSyntax>*, ParserError>(d);
                        }
                        else {
                            return Result<Vector<BindingSpecSyntax>*, ParserError>(new (alignof(Vector<BindingSpecSyntax>), rp) Vector<BindingSpecSyntax>(rp, *list));
                        };
                    };
                    break;
                }
            }
        }
        (*list).add(node);
    };
}

Result<BindingSpecSyntax, ParserError> Parser::parse_bindingspec(Page* rp, Page* ep) {
    {
        {
            auto _result = parse_structure(rp, ep);
            switch (_result._tag)
            {
                case Success::Error:
                {
                    auto error = _result._Error;
                    {
                        {
                            auto _result = error;
                            switch (_result._tag)
                            {
                                case ParserError::Invalid:
                                {
                                    auto i = _result._Invalid;
                                    return Result<BindingSpecSyntax, ParserError>(i);
                                    break;
                                }
                                case ParserError::Different:
                                {
                                    auto d = _result._Different;
                                    {
                                    };
                                    break;
                                }
                            }
                        };
                    };
                    break;
                }
                case Success::Ok:
                {
                    auto node = _result._Ok;
                    return Result<BindingSpecSyntax, ParserError>(BindingSpecSyntax(StructureSyntax(node)));
                    break;
                }
            }
        };
    };
    {
        {
            auto _result = parse_type(rp, ep);
            switch (_result._tag)
            {
                case Success::Error:
                {
                    auto error = _result._Error;
                    {
                        {
                            auto _result = error;
                            switch (_result._tag)
                            {
                                case ParserError::Invalid:
                                {
                                    auto i = _result._Invalid;
                                    return Result<BindingSpecSyntax, ParserError>(i);
                                    break;
                                }
                                case ParserError::Different:
                                {
                                    auto d = _result._Different;
                                    {
                                    };
                                    break;
                                }
                            }
                        };
                    };
                    break;
                }
                case Success::Ok:
                {
                    auto node = _result._Ok;
                    return Result<BindingSpecSyntax, ParserError>(BindingSpecSyntax(TypeSyntax(node)));
                    break;
                }
            }
        };
    };
    {
        {
            auto _result = parse_array(rp, ep);
            switch (_result._tag)
            {
                case Success::Error:
                {
                    auto error = _result._Error;
                    {
                        {
                            auto _result = error;
                            switch (_result._tag)
                            {
                                case ParserError::Invalid:
                                {
                                    auto i = _result._Invalid;
                                    return Result<BindingSpecSyntax, ParserError>(i);
                                    break;
                                }
                                case ParserError::Different:
                                {
                                    auto d = _result._Different;
                                    {
                                    };
                                    break;
                                }
                            }
                        };
                    };
                    break;
                }
                case Success::Ok:
                {
                    auto node = _result._Ok;
                    return Result<BindingSpecSyntax, ParserError>(BindingSpecSyntax(ArraySyntax(node)));
                    break;
                }
            }
        };
    };
    return Result<BindingSpecSyntax, ParserError>(ParserError(DifferentSyntax()));
}

Result<ArraySyntax, ParserError> Parser::parse_array(Page* rp, Page* ep) {
    const auto start = lexer.previous_position;
    const auto start_left_bracket_1 = lexer.previous_position;
    const auto success_left_bracket_1 = lexer.parse_punctuation('[');
    if (success_left_bracket_1 == false) {
        return Result<ArraySyntax, ParserError>(ParserError(DifferentSyntax()));
    };
    const auto members_start = lexer.position;
    Vector<TypeSyntax>* members = nullptr;
    {
        auto _result = parse_type_list(rp, ep);
        switch (_result._tag)
        {
            case Success::Error:
            {
                auto error = _result._Error;
                {
                    {
                        auto _result = error;
                        switch (_result._tag)
                        {
                            case ParserError::Different:
                            {
                                auto d = _result._Different;
                                {
                                };
                                break;
                            }
                            case ParserError::Invalid:
                            {
                                auto i = _result._Invalid;
                                return Result<ArraySyntax, ParserError>(i);
                                break;
                            }
                        }
                    };
                };
                break;
            }
            case Success::Ok:
            {
                auto success = _result._Ok;
                members = success;
                break;
            }
        }
    };
    const auto start_right_bracket_3 = lexer.previous_position;
    const auto success_right_bracket_3 = lexer.parse_punctuation(']');
    if (success_right_bracket_3 == false) {
        return Result<ArraySyntax, ParserError>(ParserError(InvalidSyntax(start_right_bracket_3, lexer.position, String(ep, "]"))));
    };
    const auto end = lexer.position;
    return Result<ArraySyntax, ParserError>(ArraySyntax(start, end, members));
}

Result<StructureSyntax, ParserError> Parser::parse_structure(Page* rp, Page* ep) {
    const auto start = lexer.previous_position;
    const auto start_left_paren_1 = lexer.previous_position;
    const auto success_left_paren_1 = lexer.parse_punctuation('(');
    if (success_left_paren_1 == false) {
        return Result<StructureSyntax, ParserError>(ParserError(DifferentSyntax()));
    };
    const auto parts_start = lexer.position;
    Vector<PartSyntax>* parts = nullptr;
    {
        auto _result = parse_part_list(rp, ep);
        switch (_result._tag)
        {
            case Success::Error:
            {
                auto error = _result._Error;
                {
                    {
                        auto _result = error;
                        switch (_result._tag)
                        {
                            case ParserError::Different:
                            {
                                auto d = _result._Different;
                                {
                                };
                                break;
                            }
                            case ParserError::Invalid:
                            {
                                auto i = _result._Invalid;
                                return Result<StructureSyntax, ParserError>(i);
                                break;
                            }
                        }
                    };
                };
                break;
            }
            case Success::Ok:
            {
                auto success = _result._Ok;
                parts = success;
                break;
            }
        }
    };
    const auto start_right_paren_3 = lexer.previous_position;
    const auto success_right_paren_3 = lexer.parse_punctuation(')');
    if (success_right_paren_3 == false) {
        return Result<StructureSyntax, ParserError>(ParserError(InvalidSyntax(start_right_paren_3, lexer.position, String(ep, ")"))));
    };
    const auto end = lexer.position;
    return Result<StructureSyntax, ParserError>(StructureSyntax(start, end, parts));
}

Result<Vector<PartSyntax>*, ParserError> Parser::parse_part_list(Page* rp, Page* ep) {
    auto r = Region();
    auto list = new (alignof(List<PartSyntax>), r.get_page()) List<PartSyntax>();
    while (true) {
        const auto _node_result = parse_part(rp, ep);
        auto node = _node_result._Ok;
        if (_node_result._tag == Success::Error) {
            const auto _node_Error = _node_result._Error;
            switch (_node_Error._tag) {
                case ParserError::Invalid: {
                    const auto i = _node_Error._Invalid;
                    return Result<Vector<PartSyntax>*, ParserError>(i);
                    break;
                }
                case ParserError::Different: {
                    const auto d = _node_Error._Different;
                    {
                        if ((*list).head == 0) {
                            return Result<Vector<PartSyntax>*, ParserError>(d);
                        }
                        else {
                            return Result<Vector<PartSyntax>*, ParserError>(new (alignof(Vector<PartSyntax>), rp) Vector<PartSyntax>(rp, *list));
                        };
                    };
                    break;
                }
            }
        }
        (*list).add(node);
    };
}

Result<PartSyntax, ParserError> Parser::parse_part(Page* rp, Page* ep) {
    {
        {
            auto _result = parse_field(rp, ep);
            switch (_result._tag)
            {
                case Success::Error:
                {
                    auto error = _result._Error;
                    {
                        {
                            auto _result = error;
                            switch (_result._tag)
                            {
                                case ParserError::Invalid:
                                {
                                    auto i = _result._Invalid;
                                    return Result<PartSyntax, ParserError>(i);
                                    break;
                                }
                                case ParserError::Different:
                                {
                                    auto d = _result._Different;
                                    {
                                    };
                                    break;
                                }
                            }
                        };
                    };
                    break;
                }
                case Success::Ok:
                {
                    auto node = _result._Ok;
                    return Result<PartSyntax, ParserError>(PartSyntax(FieldSyntax(node)));
                    break;
                }
            }
        };
    };
    {
        {
            auto _result = parse_property(rp, ep);
            switch (_result._tag)
            {
                case Success::Error:
                {
                    auto error = _result._Error;
                    {
                        {
                            auto _result = error;
                            switch (_result._tag)
                            {
                                case ParserError::Invalid:
                                {
                                    auto i = _result._Invalid;
                                    return Result<PartSyntax, ParserError>(i);
                                    break;
                                }
                                case ParserError::Different:
                                {
                                    auto d = _result._Different;
                                    {
                                    };
                                    break;
                                }
                            }
                        };
                    };
                    break;
                }
                case Success::Ok:
                {
                    auto node = _result._Ok;
                    return Result<PartSyntax, ParserError>(PartSyntax(PropertySyntax(node)));
                    break;
                }
            }
        };
    };
    return Result<PartSyntax, ParserError>(ParserError(DifferentSyntax()));
}

Result<FieldSyntax, ParserError> Parser::parse_field(Page* rp, Page* ep) {
    const auto start = lexer.previous_position;
    const auto start_private_1 = lexer.previous_position;
    const auto success_private_1 = lexer.parse_keyword(*keywords_index.get(29));
    if (success_private_1 == false) {
        return Result<FieldSyntax, ParserError>(ParserError(DifferentSyntax()));
    };
    const auto property_start = lexer.position;
    const auto _property_result = parse_property(rp, ep);
    auto property = _property_result._Ok;
    if (_property_result._tag == Success::Error) {
        const auto _property_Error = _property_result._Error;
        switch (_property_Error._tag) {
            case ParserError::Different: {
                const auto d = _property_Error._Different;
                return Result<FieldSyntax, ParserError>(InvalidSyntax(property_start, lexer.position, String(ep, "a valid Property syntax")));
                break;
            }
            case ParserError::Invalid: {
                const auto i = _property_Error._Invalid;
                return Result<FieldSyntax, ParserError>(i);
                break;
            }
        }
    }
    ;
    const auto end = lexer.position;
    return Result<FieldSyntax, ParserError>(FieldSyntax(start, end, property));
}

Result<Vector<PropertySyntax>*, ParserError> Parser::parse_property_list(Page* rp, Page* ep) {
    auto r = Region();
    auto list = new (alignof(List<PropertySyntax>), r.get_page()) List<PropertySyntax>();
    while (true) {
        const auto _node_result = parse_property(rp, ep);
        auto node = _node_result._Ok;
        if (_node_result._tag == Success::Error) {
            const auto _node_Error = _node_result._Error;
            switch (_node_Error._tag) {
                case ParserError::Invalid: {
                    const auto i = _node_Error._Invalid;
                    return Result<Vector<PropertySyntax>*, ParserError>(i);
                    break;
                }
                case ParserError::Different: {
                    const auto d = _node_Error._Different;
                    {
                        if ((*list).head == 0) {
                            return Result<Vector<PropertySyntax>*, ParserError>(d);
                        }
                        else {
                            return Result<Vector<PropertySyntax>*, ParserError>(new (alignof(Vector<PropertySyntax>), rp) Vector<PropertySyntax>(rp, *list));
                        };
                    };
                    break;
                }
            }
        }
        (*list).add(node);
    };
}

Result<PropertySyntax, ParserError> Parser::parse_property(Page* rp, Page* ep) {
    const auto start = lexer.previous_position;
    const auto start_name = lexer.previous_position;
    const auto name = lexer.parse_identifier(rp, keywords);
    if (name == nullptr) {
        return Result<PropertySyntax, ParserError>(ParserError(DifferentSyntax()));
    };
    const auto annotation_start = lexer.position;
    const auto _annotation_result = parse_typeannotation(rp, ep);
    auto annotation = _annotation_result._Ok;
    if (_annotation_result._tag == Success::Error) {
        const auto _annotation_Error = _annotation_result._Error;
        switch (_annotation_Error._tag) {
            case ParserError::Different: {
                const auto d = _annotation_Error._Different;
                return Result<PropertySyntax, ParserError>(InvalidSyntax(annotation_start, lexer.position, String(ep, "a valid TypeAnnotation syntax")));
                break;
            }
            case ParserError::Invalid: {
                const auto i = _annotation_Error._Invalid;
                return Result<PropertySyntax, ParserError>(i);
                break;
            }
        }
    }
    ;
    const auto initializer_start = lexer.position;
    InitializerSyntax* initializer = nullptr;
    {
        auto _result = parse_initializer(rp, ep);
        switch (_result._tag)
        {
            case Success::Error:
            {
                auto error = _result._Error;
                {
                    {
                        auto _result = error;
                        switch (_result._tag)
                        {
                            case ParserError::Different:
                            {
                                auto d = _result._Different;
                                {
                                };
                                break;
                            }
                            case ParserError::Invalid:
                            {
                                auto i = _result._Invalid;
                                return Result<PropertySyntax, ParserError>(i);
                                break;
                            }
                        }
                    };
                };
                break;
            }
            case Success::Ok:
            {
                auto success = _result._Ok;
                initializer = new (alignof(InitializerSyntax), rp) InitializerSyntax(success);
                break;
            }
        }
    };
    const auto start_comma_4 = lexer.previous_position;
    const auto success_comma_4 = lexer.parse_punctuation(',');
    const auto start_colon_5 = lexer.previous_position;
    const auto success_colon_5 = lexer.parse_colon();
    const auto attributes_start = lexer.position;
    Vector<AttributeSyntax>* attributes = nullptr;
    {
        auto _result = parse_attribute_list(rp, ep);
        switch (_result._tag)
        {
            case Success::Error:
            {
                auto error = _result._Error;
                {
                    {
                        auto _result = error;
                        switch (_result._tag)
                        {
                            case ParserError::Different:
                            {
                                auto d = _result._Different;
                                {
                                };
                                break;
                            }
                            case ParserError::Invalid:
                            {
                                auto i = _result._Invalid;
                                return Result<PropertySyntax, ParserError>(i);
                                break;
                            }
                        }
                    };
                };
                break;
            }
            case Success::Ok:
            {
                auto success = _result._Ok;
                attributes = success;
                break;
            }
        }
    };
    const auto start_colon_7 = lexer.previous_position;
    const auto success_colon_7 = lexer.parse_colon();
    const auto end = lexer.position;
    return Result<PropertySyntax, ParserError>(PropertySyntax(start, end, *name, annotation, initializer, attributes));
}

Result<TypeAnnotationSyntax, ParserError> Parser::parse_typeannotation(Page* rp, Page* ep) {
    const auto start = lexer.previous_position;
    const auto start_colon_1 = lexer.previous_position;
    const auto success_colon_1 = lexer.parse_colon();
    if (success_colon_1 == false) {
        return Result<TypeAnnotationSyntax, ParserError>(ParserError(DifferentSyntax()));
    };
    const auto type_start = lexer.position;
    const auto _type_result = parse_type(rp, ep);
    auto type = _type_result._Ok;
    if (_type_result._tag == Success::Error) {
        const auto _type_Error = _type_result._Error;
        switch (_type_Error._tag) {
            case ParserError::Different: {
                const auto d = _type_Error._Different;
                return Result<TypeAnnotationSyntax, ParserError>(InvalidSyntax(type_start, lexer.position, String(ep, "a valid Type syntax")));
                break;
            }
            case ParserError::Invalid: {
                const auto i = _type_Error._Invalid;
                return Result<TypeAnnotationSyntax, ParserError>(i);
                break;
            }
        }
    }
    ;
    const auto end = lexer.position;
    return Result<TypeAnnotationSyntax, ParserError>(TypeAnnotationSyntax(start, end, type));
}

Result<ContinueSyntax, ParserError> Parser::parse_continue(Page* rp, Page* ep) {
    const auto start = lexer.previous_position;
    const auto start_continue_1 = lexer.previous_position;
    const auto success_continue_1 = lexer.parse_keyword(*keywords_index.get(3));
    if (success_continue_1 == false) {
        return Result<ContinueSyntax, ParserError>(ParserError(DifferentSyntax()));
    };
    const auto name_start = lexer.position;
    LoopSyntax* name = nullptr;
    {
        auto _result = parse_loop(rp, ep);
        switch (_result._tag)
        {
            case Success::Error:
            {
                auto error = _result._Error;
                {
                    {
                        auto _result = error;
                        switch (_result._tag)
                        {
                            case ParserError::Different:
                            {
                                auto d = _result._Different;
                                {
                                };
                                break;
                            }
                            case ParserError::Invalid:
                            {
                                auto i = _result._Invalid;
                                return Result<ContinueSyntax, ParserError>(i);
                                break;
                            }
                        }
                    };
                };
                break;
            }
            case Success::Ok:
            {
                auto success = _result._Ok;
                name = new (alignof(LoopSyntax), rp) LoopSyntax(success);
                break;
            }
        }
    };
    const auto start_colon_3 = lexer.previous_position;
    const auto success_colon_3 = lexer.parse_colon();
    const auto end = lexer.position;
    return Result<ContinueSyntax, ParserError>(ContinueSyntax(start, end, name));
}

Result<BreakSyntax, ParserError> Parser::parse_break(Page* rp, Page* ep) {
    const auto start = lexer.previous_position;
    const auto start_break_1 = lexer.previous_position;
    const auto success_break_1 = lexer.parse_keyword(*keywords_index.get(0));
    if (success_break_1 == false) {
        return Result<BreakSyntax, ParserError>(ParserError(DifferentSyntax()));
    };
    const auto name_start = lexer.position;
    LoopSyntax* name = nullptr;
    {
        auto _result = parse_loop(rp, ep);
        switch (_result._tag)
        {
            case Success::Error:
            {
                auto error = _result._Error;
                {
                    {
                        auto _result = error;
                        switch (_result._tag)
                        {
                            case ParserError::Different:
                            {
                                auto d = _result._Different;
                                {
                                };
                                break;
                            }
                            case ParserError::Invalid:
                            {
                                auto i = _result._Invalid;
                                return Result<BreakSyntax, ParserError>(i);
                                break;
                            }
                        }
                    };
                };
                break;
            }
            case Success::Ok:
            {
                auto success = _result._Ok;
                name = new (alignof(LoopSyntax), rp) LoopSyntax(success);
                break;
            }
        }
    };
    const auto result_start = lexer.position;
    Vector<OperandSyntax>* result = nullptr;
    {
        auto _result = parse_operand_list(rp, ep);
        switch (_result._tag)
        {
            case Success::Error:
            {
                auto error = _result._Error;
                {
                    {
                        auto _result = error;
                        switch (_result._tag)
                        {
                            case ParserError::Different:
                            {
                                auto d = _result._Different;
                                {
                                };
                                break;
                            }
                            case ParserError::Invalid:
                            {
                                auto i = _result._Invalid;
                                return Result<BreakSyntax, ParserError>(i);
                                break;
                            }
                        }
                    };
                };
                break;
            }
            case Success::Ok:
            {
                auto success = _result._Ok;
                result = success;
                break;
            }
        }
    };
    const auto start_colon_4 = lexer.previous_position;
    const auto success_colon_4 = lexer.parse_colon();
    const auto end = lexer.position;
    return Result<BreakSyntax, ParserError>(BreakSyntax(start, end, name, result));
}

Result<LoopSyntax, ParserError> Parser::parse_loop(Page* rp, Page* ep) {
    const auto start = lexer.previous_position;
    const auto start_loop_1 = lexer.previous_position;
    const auto success_loop_1 = lexer.parse_keyword(*keywords_index.get(22));
    if (success_loop_1 == false) {
        return Result<LoopSyntax, ParserError>(ParserError(DifferentSyntax()));
    };
    const auto start_name = lexer.previous_position;
    const auto name = lexer.parse_identifier(rp, keywords);
    if (name == nullptr) {
        return Result<LoopSyntax, ParserError>(ParserError(InvalidSyntax(start_name, lexer.position, String(ep, "an identifier"))));
    };
    const auto end = lexer.position;
    return Result<LoopSyntax, ParserError>(LoopSyntax(start, end, *name));
}

Result<ReturnSyntax, ParserError> Parser::parse_return(Page* rp, Page* ep) {
    const auto start = lexer.previous_position;
    const auto start_return_1 = lexer.previous_position;
    const auto success_return_1 = lexer.parse_keyword(*keywords_index.get(30));
    if (success_return_1 == false) {
        return Result<ReturnSyntax, ParserError>(ParserError(DifferentSyntax()));
    };
    const auto result_start = lexer.position;
    Vector<OperandSyntax>* result = nullptr;
    {
        auto _result = parse_operand_list(rp, ep);
        switch (_result._tag)
        {
            case Success::Error:
            {
                auto error = _result._Error;
                {
                    {
                        auto _result = error;
                        switch (_result._tag)
                        {
                            case ParserError::Different:
                            {
                                auto d = _result._Different;
                                {
                                };
                                break;
                            }
                            case ParserError::Invalid:
                            {
                                auto i = _result._Invalid;
                                return Result<ReturnSyntax, ParserError>(i);
                                break;
                            }
                        }
                    };
                };
                break;
            }
            case Success::Ok:
            {
                auto success = _result._Ok;
                result = success;
                break;
            }
        }
    };
    const auto end = lexer.position;
    return Result<ReturnSyntax, ParserError>(ReturnSyntax(start, end, result));
}

Result<ThrowSyntax, ParserError> Parser::parse_throw(Page* rp, Page* ep) {
    const auto start = lexer.previous_position;
    const auto start_throw_1 = lexer.previous_position;
    const auto success_throw_1 = lexer.parse_keyword(*keywords_index.get(35));
    if (success_throw_1 == false) {
        return Result<ThrowSyntax, ParserError>(ParserError(DifferentSyntax()));
    };
    const auto result_start = lexer.position;
    Vector<OperandSyntax>* result = nullptr;
    {
        auto _result = parse_operand_list(rp, ep);
        switch (_result._tag)
        {
            case Success::Error:
            {
                auto error = _result._Error;
                {
                    {
                        auto _result = error;
                        switch (_result._tag)
                        {
                            case ParserError::Different:
                            {
                                auto d = _result._Different;
                                {
                                };
                                break;
                            }
                            case ParserError::Invalid:
                            {
                                auto i = _result._Invalid;
                                return Result<ThrowSyntax, ParserError>(i);
                                break;
                            }
                        }
                    };
                };
                break;
            }
            case Success::Ok:
            {
                auto success = _result._Ok;
                result = success;
                break;
            }
        }
    };
    const auto end = lexer.position;
    return Result<ThrowSyntax, ParserError>(ThrowSyntax(start, end, result));
}

Result<LambdaSyntax, ParserError> Parser::parse_lambda(Page* rp, Page* ep) {
    const auto start = lexer.previous_position;
    const auto start_lambda_1 = lexer.previous_position;
    const auto success_lambda_1 = lexer.parse_keyword(*keywords_index.get(20));
    if (success_lambda_1 == false) {
        return Result<LambdaSyntax, ParserError>(ParserError(DifferentSyntax()));
    };
    const auto input_start = lexer.position;
    const auto _input_result = parse_operand_list(rp, ep);
    auto input = _input_result._Ok;
    if (_input_result._tag == Success::Error) {
        const auto _input_Error = _input_result._Error;
        switch (_input_Error._tag) {
            case ParserError::Different: {
                const auto d = _input_Error._Different;
                return Result<LambdaSyntax, ParserError>(InvalidSyntax(input_start, lexer.position, String(ep, "a valid Operand syntax")));
                break;
            }
            case ParserError::Invalid: {
                const auto i = _input_Error._Invalid;
                return Result<LambdaSyntax, ParserError>(i);
                break;
            }
        }
    }
    ;
    const auto start_colon_3 = lexer.previous_position;
    const auto success_colon_3 = lexer.parse_colon();
    if (success_colon_3 == false) {
        return Result<LambdaSyntax, ParserError>(ParserError(InvalidSyntax(start_colon_3, lexer.position, String(ep, "a colon or a line feed"))));
    };
    const auto block_start = lexer.position;
    const auto _block_result = parse_action(rp, ep);
    auto block = _block_result._Ok;
    if (_block_result._tag == Success::Error) {
        const auto _block_Error = _block_result._Error;
        switch (_block_Error._tag) {
            case ParserError::Different: {
                const auto d = _block_Error._Different;
                return Result<LambdaSyntax, ParserError>(InvalidSyntax(block_start, lexer.position, String(ep, "a valid Action syntax")));
                break;
            }
            case ParserError::Invalid: {
                const auto i = _block_Error._Invalid;
                return Result<LambdaSyntax, ParserError>(i);
                break;
            }
        }
    }
    ;
    const auto end = lexer.position;
    return Result<LambdaSyntax, ParserError>(LambdaSyntax(start, end, *input, block));
}

Result<ForSyntax, ParserError> Parser::parse_for(Page* rp, Page* ep) {
    const auto start = lexer.previous_position;
    const auto start_for_1 = lexer.previous_position;
    const auto success_for_1 = lexer.parse_keyword(*keywords_index.get(11));
    if (success_for_1 == false) {
        return Result<ForSyntax, ParserError>(ParserError(DifferentSyntax()));
    };
    const auto start_variable = lexer.previous_position;
    const auto variable = lexer.parse_identifier(rp, keywords);
    if (variable == nullptr) {
        return Result<ForSyntax, ParserError>(ParserError(InvalidSyntax(start_variable, lexer.position, String(ep, "an identifier"))));
    };
    const auto annotation_start = lexer.position;
    TypeAnnotationSyntax* annotation = nullptr;
    {
        auto _result = parse_typeannotation(rp, ep);
        switch (_result._tag)
        {
            case Success::Error:
            {
                auto error = _result._Error;
                {
                    {
                        auto _result = error;
                        switch (_result._tag)
                        {
                            case ParserError::Different:
                            {
                                auto d = _result._Different;
                                {
                                };
                                break;
                            }
                            case ParserError::Invalid:
                            {
                                auto i = _result._Invalid;
                                return Result<ForSyntax, ParserError>(i);
                                break;
                            }
                        }
                    };
                };
                break;
            }
            case Success::Ok:
            {
                auto success = _result._Ok;
                annotation = new (alignof(TypeAnnotationSyntax), rp) TypeAnnotationSyntax(success);
                break;
            }
        }
    };
    const auto start_in_4 = lexer.previous_position;
    const auto success_in_4 = lexer.parse_keyword(*keywords_index.get(16));
    if (success_in_4 == false) {
        return Result<ForSyntax, ParserError>(ParserError(InvalidSyntax(start_in_4, lexer.position, String(ep, "in"))));
    };
    const auto operation_start = lexer.position;
    const auto _operation_result = parse_operand_list(rp, ep);
    auto operation = _operation_result._Ok;
    if (_operation_result._tag == Success::Error) {
        const auto _operation_Error = _operation_result._Error;
        switch (_operation_Error._tag) {
            case ParserError::Different: {
                const auto d = _operation_Error._Different;
                return Result<ForSyntax, ParserError>(InvalidSyntax(operation_start, lexer.position, String(ep, "a valid Operand syntax")));
                break;
            }
            case ParserError::Invalid: {
                const auto i = _operation_Error._Invalid;
                return Result<ForSyntax, ParserError>(i);
                break;
            }
        }
    }
    ;
    const auto start_colon_6 = lexer.previous_position;
    const auto success_colon_6 = lexer.parse_colon();
    if (success_colon_6 == false) {
        return Result<ForSyntax, ParserError>(ParserError(InvalidSyntax(start_colon_6, lexer.position, String(ep, "a colon or a line feed"))));
    };
    const auto name_start = lexer.position;
    LabelSyntax* name = nullptr;
    {
        auto _result = parse_label(rp, ep);
        switch (_result._tag)
        {
            case Success::Error:
            {
                auto error = _result._Error;
                {
                    {
                        auto _result = error;
                        switch (_result._tag)
                        {
                            case ParserError::Different:
                            {
                                auto d = _result._Different;
                                {
                                };
                                break;
                            }
                            case ParserError::Invalid:
                            {
                                auto i = _result._Invalid;
                                return Result<ForSyntax, ParserError>(i);
                                break;
                            }
                        }
                    };
                };
                break;
            }
            case Success::Ok:
            {
                auto success = _result._Ok;
                name = new (alignof(LabelSyntax), rp) LabelSyntax(success);
                break;
            }
        }
    };
    const auto action_start = lexer.position;
    const auto _action_result = parse_action(rp, ep);
    auto action = _action_result._Ok;
    if (_action_result._tag == Success::Error) {
        const auto _action_Error = _action_result._Error;
        switch (_action_Error._tag) {
            case ParserError::Different: {
                const auto d = _action_Error._Different;
                return Result<ForSyntax, ParserError>(InvalidSyntax(action_start, lexer.position, String(ep, "a valid Action syntax")));
                break;
            }
            case ParserError::Invalid: {
                const auto i = _action_Error._Invalid;
                return Result<ForSyntax, ParserError>(i);
                break;
            }
        }
    }
    ;
    const auto end = lexer.position;
    return Result<ForSyntax, ParserError>(ForSyntax(start, end, *variable, annotation, *operation, name, action));
}

Result<LabelSyntax, ParserError> Parser::parse_label(Page* rp, Page* ep) {
    const auto start = lexer.previous_position;
    const auto start_label_1 = lexer.previous_position;
    const auto success_label_1 = lexer.parse_keyword(*keywords_index.get(19));
    if (success_label_1 == false) {
        return Result<LabelSyntax, ParserError>(ParserError(DifferentSyntax()));
    };
    const auto start_name = lexer.previous_position;
    const auto name = lexer.parse_identifier(rp, keywords);
    if (name == nullptr) {
        return Result<LabelSyntax, ParserError>(ParserError(InvalidSyntax(start_name, lexer.position, String(ep, "an identifier"))));
    };
    const auto start_colon_3 = lexer.previous_position;
    const auto success_colon_3 = lexer.parse_colon();
    const auto end = lexer.position;
    return Result<LabelSyntax, ParserError>(LabelSyntax(start, end, *name));
}

Result<RepeatSyntax, ParserError> Parser::parse_repeat(Page* rp, Page* ep) {
    const auto start = lexer.previous_position;
    const auto start_repeat_1 = lexer.previous_position;
    const auto success_repeat_1 = lexer.parse_keyword(*keywords_index.get(32));
    if (success_repeat_1 == false) {
        return Result<RepeatSyntax, ParserError>(ParserError(DifferentSyntax()));
    };
    const auto name_start = lexer.position;
    LabelSyntax* name = nullptr;
    {
        auto _result = parse_label(rp, ep);
        switch (_result._tag)
        {
            case Success::Error:
            {
                auto error = _result._Error;
                {
                    {
                        auto _result = error;
                        switch (_result._tag)
                        {
                            case ParserError::Different:
                            {
                                auto d = _result._Different;
                                {
                                };
                                break;
                            }
                            case ParserError::Invalid:
                            {
                                auto i = _result._Invalid;
                                return Result<RepeatSyntax, ParserError>(i);
                                break;
                            }
                        }
                    };
                };
                break;
            }
            case Success::Ok:
            {
                auto success = _result._Ok;
                name = new (alignof(LabelSyntax), rp) LabelSyntax(success);
                break;
            }
        }
    };
    const auto action_start = lexer.position;
    const auto _action_result = parse_action(rp, ep);
    auto action = _action_result._Ok;
    if (_action_result._tag == Success::Error) {
        const auto _action_Error = _action_result._Error;
        switch (_action_Error._tag) {
            case ParserError::Different: {
                const auto d = _action_Error._Different;
                return Result<RepeatSyntax, ParserError>(InvalidSyntax(action_start, lexer.position, String(ep, "a valid Action syntax")));
                break;
            }
            case ParserError::Invalid: {
                const auto i = _action_Error._Invalid;
                return Result<RepeatSyntax, ParserError>(i);
                break;
            }
        }
    }
    ;
    const auto end = lexer.position;
    return Result<RepeatSyntax, ParserError>(RepeatSyntax(start, end, name, action));
}

Result<Vector<ActionSyntax>*, ParserError> Parser::parse_action_list(Page* rp, Page* ep) {
    auto r = Region();
    auto list = new (alignof(List<ActionSyntax>), r.get_page()) List<ActionSyntax>();
    while (true) {
        const auto _node_result = parse_action(rp, ep);
        auto node = _node_result._Ok;
        if (_node_result._tag == Success::Error) {
            const auto _node_Error = _node_result._Error;
            switch (_node_Error._tag) {
                case ParserError::Invalid: {
                    const auto i = _node_Error._Invalid;
                    return Result<Vector<ActionSyntax>*, ParserError>(i);
                    break;
                }
                case ParserError::Different: {
                    const auto d = _node_Error._Different;
                    {
                        if ((*list).head == 0) {
                            return Result<Vector<ActionSyntax>*, ParserError>(d);
                        }
                        else {
                            return Result<Vector<ActionSyntax>*, ParserError>(new (alignof(Vector<ActionSyntax>), rp) Vector<ActionSyntax>(rp, *list));
                        };
                    };
                    break;
                }
            }
        }
        (*list).add(node);
    };
}

Result<ActionSyntax, ParserError> Parser::parse_action(Page* rp, Page* ep) {
    {
        {
            auto _result = parse_operation(rp, ep);
            switch (_result._tag)
            {
                case Success::Error:
                {
                    auto error = _result._Error;
                    {
                        {
                            auto _result = error;
                            switch (_result._tag)
                            {
                                case ParserError::Invalid:
                                {
                                    auto i = _result._Invalid;
                                    return Result<ActionSyntax, ParserError>(i);
                                    break;
                                }
                                case ParserError::Different:
                                {
                                    auto d = _result._Different;
                                    {
                                    };
                                    break;
                                }
                            }
                        };
                    };
                    break;
                }
                case Success::Ok:
                {
                    auto node = _result._Ok;
                    return Result<ActionSyntax, ParserError>(ActionSyntax(OperationSyntax(node)));
                    break;
                }
            }
        };
    };
    {
        {
            auto _result = parse_set(rp, ep);
            switch (_result._tag)
            {
                case Success::Error:
                {
                    auto error = _result._Error;
                    {
                        {
                            auto _result = error;
                            switch (_result._tag)
                            {
                                case ParserError::Invalid:
                                {
                                    auto i = _result._Invalid;
                                    return Result<ActionSyntax, ParserError>(i);
                                    break;
                                }
                                case ParserError::Different:
                                {
                                    auto d = _result._Different;
                                    {
                                    };
                                    break;
                                }
                            }
                        };
                    };
                    break;
                }
                case Success::Ok:
                {
                    auto node = _result._Ok;
                    return Result<ActionSyntax, ParserError>(ActionSyntax(SetSyntax(node)));
                    break;
                }
            }
        };
    };
    return Result<ActionSyntax, ParserError>(ParserError(DifferentSyntax()));
}

Result<OperationSyntax, ParserError> Parser::parse_operation(Page* rp, Page* ep) {
    const auto start = lexer.previous_position;
    const auto operands_start = lexer.position;
    const auto _operands_result = parse_operand_list(rp, ep);
    auto operands = _operands_result._Ok;
    if (_operands_result._tag == Success::Error) {
        const auto _operands_Error = _operands_result._Error;
        switch (_operands_Error._tag) {
            case ParserError::Different: {
                const auto d = _operands_Error._Different;
                return Result<OperationSyntax, ParserError>(d);
                break;
            }
            case ParserError::Invalid: {
                const auto i = _operands_Error._Invalid;
                return Result<OperationSyntax, ParserError>(i);
                break;
            }
        }
    }
    ;
    const auto end = lexer.position;
    return Result<OperationSyntax, ParserError>(OperationSyntax(start, end, *operands));
}

Result<SetSyntax, ParserError> Parser::parse_set(Page* rp, Page* ep) {
    const auto start = lexer.previous_position;
    const auto start_set_1 = lexer.previous_position;
    const auto success_set_1 = lexer.parse_keyword(*keywords_index.get(33));
    if (success_set_1 == false) {
        return Result<SetSyntax, ParserError>(ParserError(DifferentSyntax()));
    };
    const auto target_start = lexer.position;
    const auto _target_result = parse_operand_list(rp, ep);
    auto target = _target_result._Ok;
    if (_target_result._tag == Success::Error) {
        const auto _target_Error = _target_result._Error;
        switch (_target_Error._tag) {
            case ParserError::Different: {
                const auto d = _target_Error._Different;
                return Result<SetSyntax, ParserError>(InvalidSyntax(target_start, lexer.position, String(ep, "a valid Operand syntax")));
                break;
            }
            case ParserError::Invalid: {
                const auto i = _target_Error._Invalid;
                return Result<SetSyntax, ParserError>(i);
                break;
            }
        }
    }
    ;
    const auto start_colon_3 = lexer.previous_position;
    const auto success_colon_3 = lexer.parse_colon();
    if (success_colon_3 == false) {
        return Result<SetSyntax, ParserError>(ParserError(InvalidSyntax(start_colon_3, lexer.position, String(ep, "a colon or a line feed"))));
    };
    const auto source_start = lexer.position;
    const auto _source_result = parse_operand_list(rp, ep);
    auto source = _source_result._Ok;
    if (_source_result._tag == Success::Error) {
        const auto _source_Error = _source_result._Error;
        switch (_source_Error._tag) {
            case ParserError::Different: {
                const auto d = _source_Error._Different;
                return Result<SetSyntax, ParserError>(InvalidSyntax(source_start, lexer.position, String(ep, "a valid Operand syntax")));
                break;
            }
            case ParserError::Invalid: {
                const auto i = _source_Error._Invalid;
                return Result<SetSyntax, ParserError>(i);
                break;
            }
        }
    }
    ;
    const auto end = lexer.position;
    return Result<SetSyntax, ParserError>(SetSyntax(start, end, *target, *source));
}

Result<SizeOfSyntax, ParserError> Parser::parse_sizeof(Page* rp, Page* ep) {
    const auto start = lexer.previous_position;
    const auto start_sizeof_1 = lexer.previous_position;
    const auto success_sizeof_1 = lexer.parse_keyword(*keywords_index.get(34));
    if (success_sizeof_1 == false) {
        return Result<SizeOfSyntax, ParserError>(ParserError(DifferentSyntax()));
    };
    const auto type_start = lexer.position;
    const auto _type_result = parse_type(rp, ep);
    auto type = _type_result._Ok;
    if (_type_result._tag == Success::Error) {
        const auto _type_Error = _type_result._Error;
        switch (_type_Error._tag) {
            case ParserError::Different: {
                const auto d = _type_Error._Different;
                return Result<SizeOfSyntax, ParserError>(InvalidSyntax(type_start, lexer.position, String(ep, "a valid Type syntax")));
                break;
            }
            case ParserError::Invalid: {
                const auto i = _type_Error._Invalid;
                return Result<SizeOfSyntax, ParserError>(i);
                break;
            }
        }
    }
    ;
    const auto end = lexer.position;
    return Result<SizeOfSyntax, ParserError>(SizeOfSyntax(start, end, type));
}

Result<Vector<TypeSyntax>*, ParserError> Parser::parse_type_list(Page* rp, Page* ep) {
    auto r = Region();
    auto list = new (alignof(List<TypeSyntax>), r.get_page()) List<TypeSyntax>();
    while (true) {
        const auto _node_result = parse_type(rp, ep);
        auto node = _node_result._Ok;
        if (_node_result._tag == Success::Error) {
            const auto _node_Error = _node_result._Error;
            switch (_node_Error._tag) {
                case ParserError::Invalid: {
                    const auto i = _node_Error._Invalid;
                    return Result<Vector<TypeSyntax>*, ParserError>(i);
                    break;
                }
                case ParserError::Different: {
                    const auto d = _node_Error._Different;
                    {
                        if ((*list).head == 0) {
                            return Result<Vector<TypeSyntax>*, ParserError>(d);
                        }
                        else {
                            return Result<Vector<TypeSyntax>*, ParserError>(new (alignof(Vector<TypeSyntax>), rp) Vector<TypeSyntax>(rp, *list));
                        };
                    };
                    break;
                }
            }
        }
        (*list).add(node);
    };
}

Result<TypeSyntax, ParserError> Parser::parse_type(Page* rp, Page* ep) {
    const auto start = lexer.previous_position;
    const auto name_start = lexer.position;
    const auto _name_result = parse_name(rp, ep);
    auto name = _name_result._Ok;
    if (_name_result._tag == Success::Error) {
        const auto _name_Error = _name_result._Error;
        switch (_name_Error._tag) {
            case ParserError::Different: {
                const auto d = _name_Error._Different;
                return Result<TypeSyntax, ParserError>(d);
                break;
            }
            case ParserError::Invalid: {
                const auto i = _name_Error._Invalid;
                return Result<TypeSyntax, ParserError>(i);
                break;
            }
        }
    }
    ;
    const auto generics_start = lexer.position;
    GenericArgumentsSyntax* generics = nullptr;
    {
        auto _result = parse_genericarguments(rp, ep);
        switch (_result._tag)
        {
            case Success::Error:
            {
                auto error = _result._Error;
                {
                    {
                        auto _result = error;
                        switch (_result._tag)
                        {
                            case ParserError::Different:
                            {
                                auto d = _result._Different;
                                {
                                };
                                break;
                            }
                            case ParserError::Invalid:
                            {
                                auto i = _result._Invalid;
                                return Result<TypeSyntax, ParserError>(i);
                                break;
                            }
                        }
                    };
                };
                break;
            }
            case Success::Ok:
            {
                auto success = _result._Ok;
                generics = new (alignof(GenericArgumentsSyntax), rp) GenericArgumentsSyntax(success);
                break;
            }
        }
    };
    const auto optional_start = lexer.position;
    OptionalSyntax* optional = nullptr;
    {
        auto _result = parse_optional(rp, ep);
        switch (_result._tag)
        {
            case Success::Error:
            {
                auto error = _result._Error;
                {
                    {
                        auto _result = error;
                        switch (_result._tag)
                        {
                            case ParserError::Different:
                            {
                                auto d = _result._Different;
                                {
                                };
                                break;
                            }
                            case ParserError::Invalid:
                            {
                                auto i = _result._Invalid;
                                return Result<TypeSyntax, ParserError>(i);
                                break;
                            }
                        }
                    };
                };
                break;
            }
            case Success::Ok:
            {
                auto success = _result._Ok;
                optional = new (alignof(OptionalSyntax), rp) OptionalSyntax(success);
                break;
            }
        }
    };
    const auto lifetime_start = lexer.position;
    LifetimeSyntax* lifetime = nullptr;
    {
        auto _result = parse_lifetime(rp, ep);
        switch (_result._tag)
        {
            case Success::Error:
            {
                auto error = _result._Error;
                {
                    {
                        auto _result = error;
                        switch (_result._tag)
                        {
                            case ParserError::Different:
                            {
                                auto d = _result._Different;
                                {
                                };
                                break;
                            }
                            case ParserError::Invalid:
                            {
                                auto i = _result._Invalid;
                                return Result<TypeSyntax, ParserError>(i);
                                break;
                            }
                        }
                    };
                };
                break;
            }
            case Success::Ok:
            {
                auto success = _result._Ok;
                lifetime = new (alignof(LifetimeSyntax), rp) LifetimeSyntax(success);
                break;
            }
        }
    };
    const auto end = lexer.position;
    return Result<TypeSyntax, ParserError>(TypeSyntax(start, end, name, generics, optional, lifetime));
}

Result<NameSyntax, ParserError> Parser::parse_name(Page* rp, Page* ep) {
    const auto start = lexer.previous_position;
    const auto start_name = lexer.previous_position;
    const auto name = lexer.parse_identifier(rp, keywords);
    if (name == nullptr) {
        return Result<NameSyntax, ParserError>(ParserError(DifferentSyntax()));
    };
    const auto extensions_start = lexer.position;
    Vector<ExtensionSyntax>* extensions = nullptr;
    {
        auto _result = parse_extension_list(rp, ep);
        switch (_result._tag)
        {
            case Success::Error:
            {
                auto error = _result._Error;
                {
                    {
                        auto _result = error;
                        switch (_result._tag)
                        {
                            case ParserError::Different:
                            {
                                auto d = _result._Different;
                                {
                                };
                                break;
                            }
                            case ParserError::Invalid:
                            {
                                auto i = _result._Invalid;
                                return Result<NameSyntax, ParserError>(i);
                                break;
                            }
                        }
                    };
                };
                break;
            }
            case Success::Ok:
            {
                auto success = _result._Ok;
                extensions = success;
                break;
            }
        }
    };
    const auto end = lexer.position;
    return Result<NameSyntax, ParserError>(NameSyntax(start, end, *name, extensions));
}

Result<Vector<ExtensionSyntax>*, ParserError> Parser::parse_extension_list(Page* rp, Page* ep) {
    auto r = Region();
    auto list = new (alignof(List<ExtensionSyntax>), r.get_page()) List<ExtensionSyntax>();
    while (true) {
        const auto _node_result = parse_extension(rp, ep);
        auto node = _node_result._Ok;
        if (_node_result._tag == Success::Error) {
            const auto _node_Error = _node_result._Error;
            switch (_node_Error._tag) {
                case ParserError::Invalid: {
                    const auto i = _node_Error._Invalid;
                    return Result<Vector<ExtensionSyntax>*, ParserError>(i);
                    break;
                }
                case ParserError::Different: {
                    const auto d = _node_Error._Different;
                    {
                        if ((*list).head == 0) {
                            return Result<Vector<ExtensionSyntax>*, ParserError>(d);
                        }
                        else {
                            return Result<Vector<ExtensionSyntax>*, ParserError>(new (alignof(Vector<ExtensionSyntax>), rp) Vector<ExtensionSyntax>(rp, *list));
                        };
                    };
                    break;
                }
            }
        }
        (*list).add(node);
    };
}

Result<ExtensionSyntax, ParserError> Parser::parse_extension(Page* rp, Page* ep) {
    const auto start = lexer.previous_position;
    const auto start_dot_1 = lexer.previous_position;
    const auto success_dot_1 = lexer.parse_punctuation('.');
    if (success_dot_1 == false) {
        return Result<ExtensionSyntax, ParserError>(ParserError(DifferentSyntax()));
    };
    const auto start_name = lexer.previous_position;
    const auto name = lexer.parse_identifier(rp, keywords);
    if (name == nullptr) {
        return Result<ExtensionSyntax, ParserError>(ParserError(InvalidSyntax(start_name, lexer.position, String(ep, "an identifier"))));
    };
    const auto end = lexer.position;
    return Result<ExtensionSyntax, ParserError>(ExtensionSyntax(start, end, *name));
}

Result<LifetimeSyntax, ParserError> Parser::parse_lifetime(Page* rp, Page* ep) {
    {
        {
            auto _result = parse_call(rp, ep);
            switch (_result._tag)
            {
                case Success::Error:
                {
                    auto error = _result._Error;
                    {
                        {
                            auto _result = error;
                            switch (_result._tag)
                            {
                                case ParserError::Invalid:
                                {
                                    auto i = _result._Invalid;
                                    return Result<LifetimeSyntax, ParserError>(i);
                                    break;
                                }
                                case ParserError::Different:
                                {
                                    auto d = _result._Different;
                                    {
                                    };
                                    break;
                                }
                            }
                        };
                    };
                    break;
                }
                case Success::Ok:
                {
                    auto node = _result._Ok;
                    return Result<LifetimeSyntax, ParserError>(LifetimeSyntax(CallSyntax(node)));
                    break;
                }
            }
        };
    };
    {
        {
            auto _result = parse_local(rp, ep);
            switch (_result._tag)
            {
                case Success::Error:
                {
                    auto error = _result._Error;
                    {
                        {
                            auto _result = error;
                            switch (_result._tag)
                            {
                                case ParserError::Invalid:
                                {
                                    auto i = _result._Invalid;
                                    return Result<LifetimeSyntax, ParserError>(i);
                                    break;
                                }
                                case ParserError::Different:
                                {
                                    auto d = _result._Different;
                                    {
                                    };
                                    break;
                                }
                            }
                        };
                    };
                    break;
                }
                case Success::Ok:
                {
                    auto node = _result._Ok;
                    return Result<LifetimeSyntax, ParserError>(LifetimeSyntax(LocalSyntax(node)));
                    break;
                }
            }
        };
    };
    {
        {
            auto _result = parse_reference(rp, ep);
            switch (_result._tag)
            {
                case Success::Error:
                {
                    auto error = _result._Error;
                    {
                        {
                            auto _result = error;
                            switch (_result._tag)
                            {
                                case ParserError::Invalid:
                                {
                                    auto i = _result._Invalid;
                                    return Result<LifetimeSyntax, ParserError>(i);
                                    break;
                                }
                                case ParserError::Different:
                                {
                                    auto d = _result._Different;
                                    {
                                    };
                                    break;
                                }
                            }
                        };
                    };
                    break;
                }
                case Success::Ok:
                {
                    auto node = _result._Ok;
                    return Result<LifetimeSyntax, ParserError>(LifetimeSyntax(ReferenceSyntax(node)));
                    break;
                }
            }
        };
    };
    {
        {
            auto _result = parse_thrown(rp, ep);
            switch (_result._tag)
            {
                case Success::Error:
                {
                    auto error = _result._Error;
                    {
                        {
                            auto _result = error;
                            switch (_result._tag)
                            {
                                case ParserError::Invalid:
                                {
                                    auto i = _result._Invalid;
                                    return Result<LifetimeSyntax, ParserError>(i);
                                    break;
                                }
                                case ParserError::Different:
                                {
                                    auto d = _result._Different;
                                    {
                                    };
                                    break;
                                }
                            }
                        };
                    };
                    break;
                }
                case Success::Ok:
                {
                    auto node = _result._Ok;
                    return Result<LifetimeSyntax, ParserError>(LifetimeSyntax(ThrownSyntax(node)));
                    break;
                }
            }
        };
    };
    return Result<LifetimeSyntax, ParserError>(ParserError(DifferentSyntax()));
}

Result<CallSyntax, ParserError> Parser::parse_call(Page* rp, Page* ep) {
    const auto start = lexer.previous_position;
    const auto start_hash_1 = lexer.previous_position;
    const auto success_hash_1 = lexer.parse_punctuation('#');
    if (success_hash_1 == false) {
        return Result<CallSyntax, ParserError>(ParserError(DifferentSyntax()));
    };
    const auto end = lexer.position;
    return Result<CallSyntax, ParserError>(CallSyntax(start, end));
}

Result<LocalSyntax, ParserError> Parser::parse_local(Page* rp, Page* ep) {
    const auto start = lexer.previous_position;
    const auto start_dollar_1 = lexer.previous_position;
    const auto success_dollar_1 = lexer.parse_punctuation('$');
    if (success_dollar_1 == false) {
        return Result<LocalSyntax, ParserError>(ParserError(DifferentSyntax()));
    };
    const auto end = lexer.position;
    return Result<LocalSyntax, ParserError>(LocalSyntax(start, end));
}

Result<ReferenceSyntax, ParserError> Parser::parse_reference(Page* rp, Page* ep) {
    const auto start = lexer.previous_position;
    const auto start_caret_1 = lexer.previous_position;
    const auto success_caret_1 = lexer.parse_punctuation('^');
    if (success_caret_1 == false) {
        return Result<ReferenceSyntax, ParserError>(ParserError(DifferentSyntax()));
    };
    const auto start_location = lexer.previous_position;
    const auto location = lexer.parse_identifier(rp, keywords);
    if (location == nullptr) {
        return Result<ReferenceSyntax, ParserError>(ParserError(InvalidSyntax(start_location, lexer.position, String(ep, "an identifier"))));
    };
    const auto end = lexer.position;
    return Result<ReferenceSyntax, ParserError>(ReferenceSyntax(start, end, *location));
}

Result<ThrownSyntax, ParserError> Parser::parse_thrown(Page* rp, Page* ep) {
    const auto start = lexer.previous_position;
    const auto start_exclamation_1 = lexer.previous_position;
    const auto success_exclamation_1 = lexer.parse_punctuation('!');
    if (success_exclamation_1 == false) {
        return Result<ThrownSyntax, ParserError>(ParserError(DifferentSyntax()));
    };
    const auto end = lexer.position;
    return Result<ThrownSyntax, ParserError>(ThrownSyntax(start, end));
}

bool Parser::is_at_end() {
    return lexer.is_at_end();
}

}
}