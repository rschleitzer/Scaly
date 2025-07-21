#include "../../scaly.h"
namespace scaly {
namespace compiler {
using namespace scaly::containers;

using namespace scaly::io;


Span::Span(size_t start, size_t end) : start(start), end(end) {}

Component::Component(Span span, String* name, Vector<Operand> value, Vector<Attribute> attributes) : span(span), name(name), value(value), attributes(attributes) {}

Name::Name(Span span, Vector<String> path) : span(span), path(path) {}

BooleanConstant::BooleanConstant(bool value) : value(value) {}

IntegerConstant::IntegerConstant(size_t value) : value(value) {}

HexConstant::HexConstant(size_t value) : value(value) {}

FloatingPointConstant::FloatingPointConstant(double value) : value(value) {}

StringConstant::StringConstant(String value) : value(value) {}

CharacterConstant::CharacterConstant(String value) : value(value) {}

FragmentConstant::FragmentConstant(String value) : value(value) {}
Constant::Constant(struct BooleanConstant _Boolean) : _tag(Boolean), _Boolean(_Boolean) {}

Constant::Constant(struct IntegerConstant _Integer) : _tag(Integer), _Integer(_Integer) {}

Constant::Constant(struct HexConstant _Hex) : _tag(Hex), _Hex(_Hex) {}

Constant::Constant(struct FloatingPointConstant _FloatingPoint) : _tag(FloatingPoint), _FloatingPoint(_FloatingPoint) {}

Constant::Constant(struct StringConstant _String) : _tag(String), _String(_String) {}

Constant::Constant(struct CharacterConstant _Character) : _tag(Character), _Character(_Character) {}

Constant::Constant(struct FragmentConstant _Fragment) : _tag(Fragment), _Fragment(_Fragment) {}


Tuple::Tuple(Span span, Vector<Component> components) : span(span), components(components) {}

Matrix::Matrix(Span span, Vector<Vector<Operand>> operations) : span(span), operations(operations) {}

Block::Block(Span span, Vector<Statement> statements) : span(span), statements(statements) {}

Reference::Reference(String location) : location(location) {}
Lifetime::Lifetime(struct Unspecified _Unspecified) : _tag(Unspecified), _Unspecified(_Unspecified) {}

Lifetime::Lifetime(struct Call _Call) : _tag(Call), _Call(_Call) {}

Lifetime::Lifetime(struct Local _Local) : _tag(Local), _Local(_Local) {}

Lifetime::Lifetime(struct Reference _Reference) : _tag(Reference), _Reference(_Reference) {}

Lifetime::Lifetime(struct Thrown _Thrown) : _tag(Thrown), _Thrown(_Thrown) {}


Action::Action(Vector<Operand> source, Vector<Operand> target) : source(source), target(target) {}

Item::Item(Span span, bool private_, String* name, Type* type, Vector<Attribute> attributes) : span(span), private_(private_), name(name), type(type), attributes(attributes) {}

Binding::Binding(String binding_type, Item item, Vector<Operand> operation) : binding_type(binding_type), item(item), operation(operation) {}

Break::Break(Span span, Vector<Operand> result) : span(span), result(result) {}

Continue::Continue(Span span) : span(span) {}

Return::Return(Span span, Vector<Operand> result) : span(span), result(result) {}

Throw::Throw(Span span, Vector<Operand> result) : span(span), result(result) {}
Statement::Statement(struct Action _Action) : _tag(Action), _Action(_Action) {}

Statement::Statement(struct Binding _Binding) : _tag(Binding), _Binding(_Binding) {}

Statement::Statement(struct Break _Break) : _tag(Break), _Break(_Break) {}

Statement::Statement(struct Continue _Continue) : _tag(Continue), _Continue(_Continue) {}

Statement::Statement(struct Return _Return) : _tag(Return), _Return(_Return) {}

Statement::Statement(struct Throw _Throw) : _tag(Throw), _Throw(_Throw) {}


Type::Type(Span span, Vector<String> name, Vector<Type>* generics, Lifetime lifetime) : span(span), name(name), generics(generics), lifetime(lifetime) {}

Property::Property(Span span, bool private_, String name, Type* type, Vector<Operand>* initializer, Vector<Attribute> attributes) : span(span), private_(private_), name(name), type(type), initializer(initializer), attributes(attributes) {}

If::If(Span span, Vector<Operand> condition, Property* property, Statement consequent, Statement* alternative) : span(span), condition(condition), property(property), consequent(consequent), alternative(alternative) {}

Case::Case(Span span, Vector<Operand> condition) : span(span), condition(condition) {}

Branch::Branch(Span span, Vector<Case> cases, Statement consequent) : span(span), cases(cases), consequent(consequent) {}

Match::Match(Span span, Vector<Operand> condition, Vector<Branch> branches, Statement* alternative) : span(span), condition(condition), branches(branches), alternative(alternative) {}

When::When(Span span, String name, Vector<String> variant, Statement consequent) : span(span), name(name), variant(variant), consequent(consequent) {}

Choose::Choose(Span span, Vector<Operand> condition, Vector<When> cases, Statement* alternative) : span(span), condition(condition), cases(cases), alternative(alternative) {}

For::For(Span span, String identifier, Vector<Operand> expression, Action action) : span(span), identifier(identifier), expression(expression), action(action) {}

While::While(Span span, Binding condition, Action action) : span(span), condition(condition), action(action) {}

Drop::Drop(Span span, Action action) : span(span), action(action) {}

Try::Try(Span span, Binding binding, Vector<When> catches, Statement* alternative) : span(span), binding(binding), catches(catches), alternative(alternative) {}

SizeOf::SizeOf(Span span, Type type) : span(span), type(type) {}

Is::Is(Span span, Vector<String> name) : span(span), name(name) {}

New::New(Span span, Type type, Tuple arguments) : span(span), type(type), arguments(arguments) {}
Expression::Expression(struct Constant _Constant) : _tag(Constant), _Constant(_Constant) {}

Expression::Expression(struct Type _Type) : _tag(Type), _Type(_Type) {}

Expression::Expression(struct Tuple _Tuple) : _tag(Tuple), _Tuple(_Tuple) {}

Expression::Expression(struct Matrix _Matrix) : _tag(Matrix), _Matrix(_Matrix) {}

Expression::Expression(struct Block _Block) : _tag(Block), _Block(_Block) {}

Expression::Expression(struct If _If) : _tag(If), _If(_If) {}

Expression::Expression(struct Match _Match) : _tag(Match), _Match(_Match) {}

Expression::Expression(struct Choose _Choose) : _tag(Choose), _Choose(_Choose) {}

Expression::Expression(struct For _For) : _tag(For), _For(_For) {}

Expression::Expression(struct While _While) : _tag(While), _While(_While) {}

Expression::Expression(struct Try _Try) : _tag(Try), _Try(_Try) {}

Expression::Expression(struct SizeOf _SizeOf) : _tag(SizeOf), _SizeOf(_SizeOf) {}

Expression::Expression(struct Is _Is) : _tag(Is), _Is(_Is) {}

Expression::Expression(struct New _New) : _tag(New), _New(_New) {}


Operand::Operand(Span span, Expression expression, Vector<String>* member_access) : span(span), expression(expression), member_access(member_access) {}
Implementation::Implementation(struct Action _Action) : _tag(Action), _Action(_Action) {}

Implementation::Implementation(struct Extern _Extern) : _tag(Extern), _Extern(_Extern) {}

Implementation::Implementation(struct Instruction _Instruction) : _tag(Instruction), _Instruction(_Instruction) {}

Implementation::Implementation(struct Intrinsic _Intrinsic) : _tag(Intrinsic), _Intrinsic(_Intrinsic) {}


Function::Function(Span span, bool private_, bool pure, String name, Vector<Item> input, Type* returns_, Type* throws_, Lifetime lifetime, Implementation implementation) : span(span), private_(private_), pure(pure), name(name), input(input), returns_(returns_), throws_(throws_), lifetime(lifetime), implementation(implementation) {}
Model::Model(struct Constant _Constant) : _tag(Constant), _Constant(_Constant) {}

Model::Model(struct String _Variable) : _tag(Variable), _Variable(_Variable) {}

Model::Model(struct Tuple _Tuple) : _tag(Tuple), _Tuple(_Tuple) {}

Model::Model(struct Matrix _Matrix) : _tag(Matrix), _Matrix(_Matrix) {}


Attribute::Attribute(Span span, String name, Model model) : span(span), name(name), model(model) {}

Initializer::Initializer(Span span, bool private_, Vector<Item> input, Implementation implementation) : span(span), private_(private_), input(input), implementation(implementation) {}

DeInitializer::DeInitializer(Span span, Implementation implementation) : span(span), implementation(implementation) {}

Operator::Operator(Span span, bool private_, String name, Vector<Item> input, Type* returns_, Type* throws_, Implementation implementation) : span(span), private_(private_), name(name), input(input), returns_(returns_), throws_(throws_), implementation(implementation) {}

Structure::Structure(Span span, bool private_, Vector<Property> properties, Vector<Module> modules, Vector<Use> uses, Vector<Initializer> initializers, DeInitializer* deinitializer, Vector<Member> members, HashMap<String, Nameable> symbols) : span(span), private_(private_), properties(properties), modules(modules), uses(uses), initializers(initializers), deinitializer(deinitializer), members(members), symbols(symbols) {}

Variant::Variant(Span span, String name, Type* type, Vector<Attribute> attributes) : span(span), name(name), type(type), attributes(attributes) {}

Union::Union(Span span, bool private_, Vector<Variant> variants, Vector<Member> members, HashMap<String, Nameable> symbols) : span(span), private_(private_), variants(variants), members(members), symbols(symbols) {}

Namespace::Namespace(Span span, Vector<Module> modules, Vector<Member> members, HashMap<String, Nameable> symbols) : span(span), modules(modules), members(members), symbols(symbols) {}

Global::Global(Span span, Type type, Vector<Operand> value) : span(span), type(type), value(value) {}
Definition::Definition(struct Intrinsic _Intrinsic) : _tag(Intrinsic), _Intrinsic(_Intrinsic) {}

Definition::Definition(struct Global _Global) : _tag(Global), _Global(_Global) {}

Definition::Definition(struct Namespace _Namespace) : _tag(Namespace), _Namespace(_Namespace) {}

Definition::Definition(struct Structure _Structure) : _tag(Structure), _Structure(_Structure) {}

Definition::Definition(struct Union _Union) : _tag(Union), _Union(_Union) {}


Use::Use(Span span, Vector<String> path) : span(span), path(path) {}

GenericParameter::GenericParameter(Span span, String name, Vector<Attribute> attributes) : span(span), name(name), attributes(attributes) {}

Concept::Concept(Span span, String name, Vector<GenericParameter> parameters, Vector<Attribute> attributes, Definition definition) : span(span), name(name), parameters(parameters), attributes(attributes), definition(definition) {}

Module::Module(bool private_, String file, String name, Vector<Module> modules, Vector<Use> uses, Vector<Member> members, HashMap<String, Nameable> symbols) : private_(private_), file(file), name(name), modules(modules), uses(uses), members(members), symbols(symbols) {}

Program::Program(Vector<Module> packages, Module module_, Vector<Statement> statements) : packages(packages), module_(module_), statements(statements) {}
Member::Member(struct Vector<Module> _Package) : _tag(Package), _Package(_Package) {}

Member::Member(struct Concept _Concept) : _tag(Concept), _Concept(_Concept) {}

Member::Member(struct Operator _Operator) : _tag(Operator), _Operator(_Operator) {}

Member::Member(struct Function _Function) : _tag(Function), _Function(_Function) {}

Nameable::Nameable(struct Vector<Module> _Package) : _tag(Package), _Package(_Package) {}

Nameable::Nameable(struct Concept _Concept) : _tag(Concept), _Concept(_Concept) {}

Nameable::Nameable(struct Operator _Operator) : _tag(Operator), _Operator(_Operator) {}

Nameable::Nameable(struct Vector<Function> _Functions) : _tag(Functions), _Functions(_Functions) {}

Nameable::Nameable(struct Property _Property) : _tag(Property), _Property(_Property) {}

Nameable::Nameable(struct Variant _Variant) : _tag(Variant), _Variant(_Variant) {}


}
}