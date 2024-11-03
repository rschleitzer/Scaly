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
Literal::Literal(StringLiteral _String) : _tag(String), _String(_String) {}

Literal::Literal(CharacterLiteral _Character) : _tag(Character), _Character(_Character) {}

Literal::Literal(FragmentLiteral _Fragment) : _tag(Fragment), _Fragment(_Fragment) {}

Literal::Literal(IntegerLiteral _Integer) : _tag(Integer), _Integer(_Integer) {}

Literal::Literal(BooleanLiteral _Boolean) : _tag(Boolean), _Boolean(_Boolean) {}

Literal::Literal(FloatingPointLiteral _FloatingPoint) : _tag(FloatingPoint), _FloatingPoint(_FloatingPoint) {}

Literal::Literal(HexLiteral _Hex) : _tag(Hex), _Hex(_Hex) {}


ThrownSyntax::ThrownSyntax(size_t start, size_t end) : start(start), end(end) {}

ReferenceSyntax::ReferenceSyntax(size_t start, size_t end, String location) : start(start), end(end), location(location) {}

LocalSyntax::LocalSyntax(size_t start, size_t end) : start(start), end(end) {}

CallSyntax::CallSyntax(size_t start, size_t end) : start(start), end(end) {}
LifetimeSyntax::LifetimeSyntax(CallSyntax _Call) : _tag(Call), _Call(_Call) {}

LifetimeSyntax::LifetimeSyntax(LocalSyntax _Local) : _tag(Local), _Local(_Local) {}

LifetimeSyntax::LifetimeSyntax(ReferenceSyntax _Reference) : _tag(Reference), _Reference(_Reference) {}

LifetimeSyntax::LifetimeSyntax(ThrownSyntax _Thrown) : _tag(Thrown), _Thrown(_Thrown) {}


ExtensionSyntax::ExtensionSyntax(size_t start, size_t end, String name) : start(start), end(end), name(name) {}

NameSyntax::NameSyntax(size_t start, size_t end, String name, Vector<ExtensionSyntax>* extensions) : start(start), end(end), name(name), extensions(extensions) {}

TypeSyntax::TypeSyntax(size_t start, size_t end, NameSyntax name, GenericArgumentsSyntax* generics, OptionalSyntax* optional, LifetimeSyntax* lifetime) : start(start), end(end), name(name), generics(generics), optional(optional), lifetime(lifetime) {}

SizeOfSyntax::SizeOfSyntax(size_t start, size_t end, TypeSyntax type) : start(start), end(end), type(type) {}

SetSyntax::SetSyntax(size_t start, size_t end, Vector<OperandSyntax> target, Vector<OperandSyntax> source) : start(start), end(end), target(target), source(source) {}

OperationSyntax::OperationSyntax(size_t start, size_t end, Vector<OperandSyntax> operands) : start(start), end(end), operands(operands) {}
ActionSyntax::ActionSyntax(OperationSyntax _Operation) : _tag(Operation), _Operation(_Operation) {}

ActionSyntax::ActionSyntax(SetSyntax _Set) : _tag(Set), _Set(_Set) {}


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
PartSyntax::PartSyntax(FieldSyntax _Field) : _tag(Field), _Field(_Field) {}

PartSyntax::PartSyntax(PropertySyntax _Property) : _tag(Property), _Property(_Property) {}


StructureSyntax::StructureSyntax(size_t start, size_t end, Vector<PartSyntax>* parts) : start(start), end(end), parts(parts) {}

ArraySyntax::ArraySyntax(size_t start, size_t end, Vector<TypeSyntax>* members) : start(start), end(end), members(members) {}
BindingSpecSyntax::BindingSpecSyntax(StructureSyntax _Structure) : _tag(Structure), _Structure(_Structure) {}

BindingSpecSyntax::BindingSpecSyntax(TypeSyntax _Type) : _tag(Type), _Type(_Type) {}

BindingSpecSyntax::BindingSpecSyntax(ArraySyntax _Array) : _tag(Array), _Array(_Array) {}


BindingAnnotationSyntax::BindingAnnotationSyntax(size_t start, size_t end, BindingSpecSyntax spec) : start(start), end(end), spec(spec) {}

BindingSyntax::BindingSyntax(size_t start, size_t end, String name, BindingAnnotationSyntax* annotation, Vector<OperandSyntax> operation) : start(start), end(end), name(name), annotation(annotation), operation(operation) {}

MutableSyntax::MutableSyntax(size_t start, size_t end, BindingSyntax binding) : start(start), end(end), binding(binding) {}

VarSyntax::VarSyntax(size_t start, size_t end, BindingSyntax binding) : start(start), end(end), binding(binding) {}

LetSyntax::LetSyntax(size_t start, size_t end, BindingSyntax binding) : start(start), end(end), binding(binding) {}

WhenSyntax::WhenSyntax(size_t start, size_t end, String name, NameSyntax variant, ActionSyntax action) : start(start), end(end), name(name), variant(variant), action(action) {}
ConditionSyntax::ConditionSyntax(OperationSyntax _Operation) : _tag(Operation), _Operation(_Operation) {}

ConditionSyntax::ConditionSyntax(LetSyntax _Let) : _tag(Let), _Let(_Let) {}


TrySyntax::TrySyntax(size_t start, size_t end, ConditionSyntax condition, Vector<WhenSyntax>* cases, DefaultSyntax* dropper) : start(start), end(end), condition(condition), cases(cases), dropper(dropper) {}

ChooseSyntax::ChooseSyntax(size_t start, size_t end, Vector<OperandSyntax> condition, Vector<WhenSyntax>* cases, DefaultSyntax* alternative) : start(start), end(end), condition(condition), cases(cases), alternative(alternative) {}

WhileSyntax::WhileSyntax(size_t start, size_t end, ConditionSyntax condition, LabelSyntax* name, ActionSyntax action) : start(start), end(end), condition(condition), name(name), action(action) {}
CommandSyntax::CommandSyntax(OperationSyntax _Operation) : _tag(Operation), _Operation(_Operation) {}

CommandSyntax::CommandSyntax(LetSyntax _Let) : _tag(Let), _Let(_Let) {}

CommandSyntax::CommandSyntax(VarSyntax _Var) : _tag(Var), _Var(_Var) {}

CommandSyntax::CommandSyntax(MutableSyntax _Mutable) : _tag(Mutable), _Mutable(_Mutable) {}

CommandSyntax::CommandSyntax(SetSyntax _Set) : _tag(Set), _Set(_Set) {}

CommandSyntax::CommandSyntax(ContinueSyntax _Continue) : _tag(Continue), _Continue(_Continue) {}

CommandSyntax::CommandSyntax(BreakSyntax _Break) : _tag(Break), _Break(_Break) {}

CommandSyntax::CommandSyntax(ReturnSyntax _Return) : _tag(Return), _Return(_Return) {}

CommandSyntax::CommandSyntax(ThrowSyntax _Throw) : _tag(Throw), _Throw(_Throw) {}


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
ExpressionSyntax::ExpressionSyntax(LiteralSyntax _Literal) : _tag(Literal), _Literal(_Literal) {}

ExpressionSyntax::ExpressionSyntax(TypeSyntax _Type) : _tag(Type), _Type(_Type) {}

ExpressionSyntax::ExpressionSyntax(ObjectSyntax _Object) : _tag(Object), _Object(_Object) {}

ExpressionSyntax::ExpressionSyntax(VectorSyntax _Vector) : _tag(Vector), _Vector(_Vector) {}

ExpressionSyntax::ExpressionSyntax(BlockSyntax _Block) : _tag(Block), _Block(_Block) {}

ExpressionSyntax::ExpressionSyntax(IfSyntax _If) : _tag(If), _If(_If) {}

ExpressionSyntax::ExpressionSyntax(MatchSyntax _Match) : _tag(Match), _Match(_Match) {}

ExpressionSyntax::ExpressionSyntax(LambdaSyntax _Lambda) : _tag(Lambda), _Lambda(_Lambda) {}

ExpressionSyntax::ExpressionSyntax(ForSyntax _For) : _tag(For), _For(_For) {}

ExpressionSyntax::ExpressionSyntax(WhileSyntax _While) : _tag(While), _While(_While) {}

ExpressionSyntax::ExpressionSyntax(ChooseSyntax _Choose) : _tag(Choose), _Choose(_Choose) {}

ExpressionSyntax::ExpressionSyntax(TrySyntax _Try) : _tag(Try), _Try(_Try) {}

ExpressionSyntax::ExpressionSyntax(RepeatSyntax _Repeat) : _tag(Repeat), _Repeat(_Repeat) {}

ExpressionSyntax::ExpressionSyntax(SizeOfSyntax _SizeOf) : _tag(SizeOf), _SizeOf(_SizeOf) {}


MemberAccessSyntax::MemberAccessSyntax(size_t start, size_t end, NameSyntax name) : start(start), end(end), name(name) {}

OperandSyntax::OperandSyntax(size_t start, size_t end, ExpressionSyntax expression, Vector<MemberAccessSyntax>* members) : start(start), end(end), expression(expression), members(members) {}

InitializerSyntax::InitializerSyntax(size_t start, size_t end, Vector<OperandSyntax> operands) : start(start), end(end), operands(operands) {}

PackageSyntax::PackageSyntax(size_t start, size_t end, NameSyntax name) : start(start), end(end), name(name) {}

ModuleSyntax::ModuleSyntax(size_t start, size_t end, String name) : start(start), end(end), name(name) {}
ModelSyntax::ModelSyntax(LiteralSyntax _Literal) : _tag(Literal), _Literal(_Literal) {}

ModelSyntax::ModelSyntax(NameSyntax _Name) : _tag(Name), _Name(_Name) {}

ModelSyntax::ModelSyntax(ObjectSyntax _Object) : _tag(Object), _Object(_Object) {}

ModelSyntax::ModelSyntax(VectorSyntax _Vector) : _tag(Vector), _Vector(_Vector) {}


AttributeSyntax::AttributeSyntax(size_t start, size_t end, String name, ModelSyntax model) : start(start), end(end), name(name), model(model) {}

MacroSyntax::MacroSyntax(size_t start, size_t end, String name, ModelSyntax model, Vector<OperandSyntax> rule) : start(start), end(end), name(name), model(model), rule(rule) {}

ExtendSyntax::ExtendSyntax(size_t start, size_t end, TypeSyntax type) : start(start), end(end), type(type) {}

ExtendsSyntax::ExtendsSyntax(size_t start, size_t end, Vector<ExtendSyntax>* extensions) : start(start), end(end), extensions(extensions) {}

IntrinsicSyntax::IntrinsicSyntax(size_t start, size_t end) : start(start), end(end) {}

InstructionSyntax::InstructionSyntax(size_t start, size_t end) : start(start), end(end) {}

ExternSyntax::ExternSyntax(size_t start, size_t end) : start(start), end(end) {}
ImplementationSyntax::ImplementationSyntax(ActionSyntax _Action) : _tag(Action), _Action(_Action) {}

ImplementationSyntax::ImplementationSyntax(ExternSyntax _Extern) : _tag(Extern), _Extern(_Extern) {}

ImplementationSyntax::ImplementationSyntax(InstructionSyntax _Instruction) : _tag(Instruction), _Instruction(_Instruction) {}

ImplementationSyntax::ImplementationSyntax(IntrinsicSyntax _Intrinsic) : _tag(Intrinsic), _Intrinsic(_Intrinsic) {}


RoutineSyntax::RoutineSyntax(size_t start, size_t end, GenericArgumentsSyntax* generics, LifetimeSyntax* lifetime, ParameterSetSyntax* parameters, Vector<AttributeSyntax>* attributes, ReturnsSyntax* returns_, ThrowsSyntax* throws_, ImplementationSyntax implementation) : start(start), end(end), generics(generics), lifetime(lifetime), parameters(parameters), attributes(attributes), returns_(returns_), throws_(throws_), implementation(implementation) {}

NamedSyntax::NamedSyntax(size_t start, size_t end, String name, RoutineSyntax routine) : start(start), end(end), name(name), routine(routine) {}
TargetSyntax::TargetSyntax(NamedSyntax _Named) : _tag(Named), _Named(_Named) {}

TargetSyntax::TargetSyntax(RoutineSyntax _Routine) : _tag(Routine), _Routine(_Routine) {}


OperatorSyntax::OperatorSyntax(size_t start, size_t end, TargetSyntax target) : start(start), end(end), target(target) {}

ProcedureSyntax::ProcedureSyntax(size_t start, size_t end, TargetSyntax target) : start(start), end(end), target(target) {}

FunctionSyntax::FunctionSyntax(size_t start, size_t end, TargetSyntax target) : start(start), end(end), target(target) {}

DeInitSyntax::DeInitSyntax(size_t start, size_t end, ActionSyntax action) : start(start), end(end), action(action) {}

InitSyntax::InitSyntax(size_t start, size_t end, ParameterSetSyntax* parameters, ActionSyntax action) : start(start), end(end), parameters(parameters), action(action) {}
MethodSyntax::MethodSyntax(FunctionSyntax _Function) : _tag(Function), _Function(_Function) {}

MethodSyntax::MethodSyntax(ProcedureSyntax _Procedure) : _tag(Procedure), _Procedure(_Procedure) {}

MethodSyntax::MethodSyntax(OperatorSyntax _Operator) : _tag(Operator), _Operator(_Operator) {}


TraitSyntax::TraitSyntax(size_t start, size_t end, NameSyntax name, ExtendsSyntax* extension, Vector<AttributeSyntax>* attributes, Vector<UseSyntax>* uses, Vector<MethodSyntax>* functions) : start(start), end(end), name(name), extension(extension), attributes(attributes), uses(uses), functions(functions) {}

ImplementSyntax::ImplementSyntax(size_t start, size_t end, TypeSyntax type, Vector<AttributeSyntax>* attributes, Vector<UseSyntax>* uses, Vector<MethodSyntax>* methods) : start(start), end(end), type(type), attributes(attributes), uses(uses), methods(methods) {}

UseSyntax::UseSyntax(size_t start, size_t end, NameSyntax name) : start(start), end(end), name(name) {}

ThrowsSyntax::ThrowsSyntax(size_t start, size_t end, TypeSyntax type, Vector<AttributeSyntax>* attributes) : start(start), end(end), type(type), attributes(attributes) {}

ReturnsSyntax::ReturnsSyntax(size_t start, size_t end, TypeSyntax type, Vector<AttributeSyntax>* attributes) : start(start), end(end), type(type), attributes(attributes) {}

ItemSyntax::ItemSyntax(size_t start, size_t end, String name, TypeAnnotationSyntax* annotation, Vector<AttributeSyntax>* attributes) : start(start), end(end), name(name), annotation(annotation), attributes(attributes) {}

ParametersSyntax::ParametersSyntax(size_t start, size_t end, Vector<ItemSyntax>* items) : start(start), end(end), items(items) {}
ParameterSetSyntax::ParameterSetSyntax(ParametersSyntax _Parameters) : _tag(Parameters), _Parameters(_Parameters) {}

ParameterSetSyntax::ParameterSetSyntax(TypeSyntax _Type) : _tag(Type), _Type(_Type) {}


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
ConceptSyntax::ConceptSyntax(ClassSyntax _Class) : _tag(Class), _Class(_Class) {}

ConceptSyntax::ConceptSyntax(NamespaceSyntax _Namespace) : _tag(Namespace), _Namespace(_Namespace) {}

ConceptSyntax::ConceptSyntax(UnionSyntax _Union) : _tag(Union), _Union(_Union) {}

ConceptSyntax::ConceptSyntax(ConstantSyntax _Constant) : _tag(Constant), _Constant(_Constant) {}

ConceptSyntax::ConceptSyntax(DelegateSyntax _Delegate) : _tag(Delegate), _Delegate(_Delegate) {}

ConceptSyntax::ConceptSyntax(IntrinsicSyntax _Intrinsic) : _tag(Intrinsic), _Intrinsic(_Intrinsic) {}


GenericParameterSyntax::GenericParameterSyntax(size_t start, size_t end, String name, Vector<AttributeSyntax>* attributes) : start(start), end(end), name(name), attributes(attributes) {}

GenericParametersSyntax::GenericParametersSyntax(size_t start, size_t end, Vector<GenericParameterSyntax> parameters) : start(start), end(end), parameters(parameters) {}

DefinitionSyntax::DefinitionSyntax(size_t start, size_t end, String name, GenericParametersSyntax* parameters, Vector<AttributeSyntax>* attributes, ConceptSyntax concept_) : start(start), end(end), name(name), parameters(parameters), attributes(attributes), concept_(concept_) {}
ConstituentSyntax::ConstituentSyntax(DefinitionSyntax _Definition) : _tag(Definition), _Definition(_Definition) {}

ConstituentSyntax::ConstituentSyntax(FunctionSyntax _Function) : _tag(Function), _Function(_Function) {}

ConstituentSyntax::ConstituentSyntax(ProcedureSyntax _Procedure) : _tag(Procedure), _Procedure(_Procedure) {}

ConstituentSyntax::ConstituentSyntax(OperatorSyntax _Operator) : _tag(Operator), _Operator(_Operator) {}

ConstituentSyntax::ConstituentSyntax(ImplementSyntax _Implement) : _tag(Implement), _Implement(_Implement) {}

ConstituentSyntax::ConstituentSyntax(TraitSyntax _Trait) : _tag(Trait), _Trait(_Trait) {}

ConstituentSyntax::ConstituentSyntax(MacroSyntax _Macro) : _tag(Macro), _Macro(_Macro) {}

ConstituentSyntax::ConstituentSyntax(ModuleSyntax _Module) : _tag(Module), _Module(_Module) {}


MemberSyntax::MemberSyntax(size_t start, size_t end, ConstituentSyntax constituent) : start(start), end(end), constituent(constituent) {}
ExportSyntax::ExportSyntax(DefinitionSyntax _Definition) : _tag(Definition), _Definition(_Definition) {}

ExportSyntax::ExportSyntax(FunctionSyntax _Function) : _tag(Function), _Function(_Function) {}

ExportSyntax::ExportSyntax(OperatorSyntax _Operator) : _tag(Operator), _Operator(_Operator) {}

ExportSyntax::ExportSyntax(TraitSyntax _Trait) : _tag(Trait), _Trait(_Trait) {}

ExportSyntax::ExportSyntax(ModuleSyntax _Module) : _tag(Module), _Module(_Module) {}


PrivateSyntax::PrivateSyntax(size_t start, size_t end, ExportSyntax export_) : start(start), end(end), export_(export_) {}
SymbolSyntax::SymbolSyntax(PrivateSyntax _Private) : _tag(Private), _Private(_Private) {}

SymbolSyntax::SymbolSyntax(DefinitionSyntax _Definition) : _tag(Definition), _Definition(_Definition) {}

SymbolSyntax::SymbolSyntax(FunctionSyntax _Function) : _tag(Function), _Function(_Function) {}

SymbolSyntax::SymbolSyntax(OperatorSyntax _Operator) : _tag(Operator), _Operator(_Operator) {}

SymbolSyntax::SymbolSyntax(TraitSyntax _Trait) : _tag(Trait), _Trait(_Trait) {}

SymbolSyntax::SymbolSyntax(MacroSyntax _Macro) : _tag(Macro), _Macro(_Macro) {}

SymbolSyntax::SymbolSyntax(ModuleSyntax _Module) : _tag(Module), _Module(_Module) {}


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

}
}