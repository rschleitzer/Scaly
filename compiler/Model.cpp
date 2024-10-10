namespace scaly {
namespace compiler {
namespace model {

using namespace scaly::io;

struct Span : Object {
    size_t start;
    size_t end;
    Span(size_t start, size_t end) : start(start), end(end) {}
};

struct Operand;
struct Attribute;

struct Component : Object {
    Span span;
    String* name;
    Vector<Operand> value;
    Vector<Attribute> attributes;
    Component(Span span, String* name, Vector<Operand> value, Vector<Attribute> attributes)
      : span(span),
        name(name),
        value(value),
        attributes(attributes) {}
};

struct Name : Object {
    Span span;
    Vector<String> path;
    Name(Span span, Vector<String> path) : span(span), path(path) {}
};


struct Constant {
    enum {
        Boolean,
        Integer,
        Hex,
        FloatingPoint,
        String,
        Fragment,
    } _tag;
    union {
        bool _Boolean;
        size_t _Integer;
        size_t _Hex;
        double _FloatingPoint;
        struct String _String;
        struct String _Fragment;
    };
};

struct Tuple : Object {
    Span span;
    Vector<Component> components;
    Tuple(Span span, Vector<Component> components) : span(span), components(components) {}
};

struct Operation;

struct Matrix : Object {
    Span span;
    Vector<Operation> operations;
    Matrix(Span span, Vector<Operation> operations) : span(span), operations(operations) {}
};

struct Statement;

struct Block {
    Span span;
    Vector<Statement> statements;
    Block(Span span, Vector<Statement> statements) : span(span), statements(statements) {}

};

struct Unspecified {};
struct Root {};
struct Local {
    Local(String location) : location(location) {}
    String location;
};
struct Reference {
    Reference(String age) : age(age) {}
    String age;
};
struct Thrown {};

struct Lifetime {
    Lifetime(Unspecified _Unspecified) : _tag(Unspecified), _Unspecified(_Unspecified) {}
    Lifetime(Root _Root) : _tag(Root), _Root(_Root) {}
    Lifetime(Local _Local) : _tag(Local), _Local(_Local) {}
    Lifetime(Reference _Reference) : _tag(Reference), _Reference(_Reference) {}
    Lifetime(Thrown _Thrown) : _tag(Thrown), _Thrown(_Thrown) {}
    enum {
        Unspecified,
        Root,
        Local,
        Reference,
        Thrown,
    } _tag;
    union {
        struct Unspecified _Unspecified;
        struct Root _Root;
        struct Local _Local;
        struct Reference _Reference;
        struct Thrown _Thrown;
    };
};

struct Type : Object {
    Span span;
    Vector<String> name;
    Vector<Type> generics;
    Lifetime lifetime;
    Type(Span span, Vector<String> name, Vector<Type> generics, Lifetime lifetime) : span(span), name(name), generics(generics), lifetime(lifetime) {}
};

struct Property {
    Span span;
    bool private_;
    String* name;
    Type* type;
    Vector<Operand>* initializer;
    Vector<Attribute> attributes;
    Property(Span span, bool private_, String* name, Type* type, Vector<Operand>* initializer, Vector<Attribute> attributes)
      : span(span),
        private_(private_),
        name(name),
        type(type),
        initializer(initializer),
        attributes(attributes) {}
};

struct Item {
    Span span;
    bool private_;
    String* name;
    Type* type;
    Vector<Attribute> attributes;
    Item(Span span, bool private_, String* name, Type* type, Vector<Attribute> attributes)
      : span(span),
        private_(private_),
        name(name),
        type(type),
        attributes(attributes) {}
};

struct Operation : Object {
    Vector<Operand> operands;
    Operation(Vector<Operand> operands) : operands(operands) {}
};

struct Statement;

struct Binding {
    enum Mutability { Constant, Extendable, Mutable, } binding_type;
    Item item;
    Operation operation;
    Binding(Mutability binding_type, Item item, Operation operation)
      : binding_type(binding_type), item(item), operation(operation) {}
};

struct Mutation : Object {
    Operation source;
    Operation target;
    Mutation(Operation source, Operation target) : source(source), target(target) {}
};

struct Action : Object {
    enum {
        Operation,
        Mutation,
    } _tag;
    union {
        struct Operation _Operation;
        struct Mutation _Mutation;
    };
};

struct If : Object {
    Span span;
    Vector<Operand> condition;
    Property* property;
    Action consequent;
    Action* alternative;
    If(Span span, Vector<Operand> condition, Property* property, Action consequent, Action* alternative) : span(span), condition(condition), property(property), consequent(consequent), alternative(alternative) {}
};

struct Case : Object {
    Span span;
    Vector<Operand> condition;
    Action consequent;
    Case(Span span, Vector<Operand> condition, Action consequent) : span(span), condition(condition), consequent(consequent) {}
};

struct Match : Object {
    Span span;
    Vector<Operand> condition;
    Vector<Case> cases;
    Action alternative;
    Match(Span span, Vector<Operand> condition, Vector<Case> cases, Action alternative) : span(span), condition(condition), cases(cases), alternative(alternative) {}
};

struct For : Object {
    Span span;
    String identifier;
    Vector<Operand> expression;
    Action action;
    For(Span span, String identifier, Vector<Operand> expression, Action action) : span(span), identifier(identifier), expression(expression), action(action) {}
};

struct While : Object {
    Span span;
    Vector<Operand> condition;
    Action action;
    While(Span span, Vector<Operand> condition, Action action) : span(span), condition(condition), action(action) {}
};

struct SizeOf : Object {
    Span span;
    Type type;
    SizeOf(Span span, Type type) : span(span), type(type) {}
};

struct Return : Object {
    Span span;
    Vector<Operand> result;
    Return(Span span, Vector<Operand> result) : span(span), result(result) {}
};

struct Expression {
    enum {
        Constant,
        Type,
        Tuple,
        Matrix,
        Block,
        If,
        For,
        While,
        SizeOf,
        Return,
    } _tag;
    union {
        struct Constant _Constant;
        struct Type _Type;
        struct Tuple _Tuple;
        struct Matrix _Matrix;
        struct Block _Block;
        struct If _If;
        struct Match _Match;
        struct For _For;
        struct While _While;
        struct SizeOf _SizeOf;
        struct Return _Return;
    };

};

struct Model {
    enum {
        Constant,
        Variable,
        Tuple,
        Matrix,
    } _tag;
    union {
        struct Constant _Constant;
        struct String _Variable;
        struct Tuple _Tuple;
        struct Matrix _Matrix;
    };
};

struct Attribute {
    Span span;
    String name;
    Model model;
    Attribute(Span span, String name, Model model) : span(span), name(name), model(model) {}
};

struct Catch {
    Span span;
    Operation condition;
    Operation handler;
    Catch(Span span, Operation condition, Operation handler) : span(span), condition(condition), handler(handler) {}
};

struct Drop {
    Span span;
    Operation handler;
    Drop(Span span, Operation handler) : span(span), handler(handler) {}
};

struct Catcher {
    Span span;
    Vector<Catch> catches;
    Drop* drop;
    Catcher(Span span, Vector<Catch> catches, Drop* drop) : span(span), catches(catches), drop(drop) {}
};

struct Postfix {
    enum {
        MemberAccess,
        Catcher,
    } _tag;
    union {
        Vector<String> _MemberAccess;
        struct Catcher _Catcher;
    };
};

struct Operand : Object {
    Span span;
    Expression expression;
    Vector<Postfix>* postfixes;
    Operand(Span span, Expression expression, Vector<Postfix>* postfixes) : span(span), expression(expression), postfixes(postfixes) {}
};

struct Statement {
    enum {
        Action,
        Binding,
    } _tag;
    union {
        struct Action _Action;
        struct Binding _Binding;
    };
};


struct Extern {};

struct Instruction {};

struct Intrinsic {};

struct Implementation {
    enum {
        Action,
        Extern,
        Instruction,
        Intrinsic,
    } _tag;
    union {
        struct Action _Action;
        struct Extern _Extern;
        struct Instruction _Instruction;
        struct Intrinsic _Intrinsic;
    };
};

struct Function : Object {
    Span span;
    bool private_;
    bool pure;
    String name;
    Vector<Item> input;
    Vector<Item> output;
    Lifetime lifetime;
    Implementation implementation;
    Function(Span span, bool private_, bool pure, String name, Vector<Item> input, Vector<Item> output, Lifetime lifetime, Implementation implementation)
      : span(span),
        private_(private_),
        pure(pure),
        name(name),
        input(input),
        output(output),
        lifetime(lifetime),
        implementation(implementation) {}
};

struct Initializer : Object {
    Span span;
    bool private_;
    Vector<Item> input;
    Implementation implementation;
    Initializer(Span span, bool private_, Vector<Item> input, Implementation implementation)
      : span(span),
        private_(private_),
        input(input),
        implementation(implementation) {}
};

struct DeInitializer : Object {
    Span span;
    Implementation implementation;
    DeInitializer(Span span, Implementation implementation)
      : span(span),
        implementation(implementation) {}
};

struct Operator : Object {
    Span span;
    bool private_;
    String name;
    Vector<Item> input;
    Vector<Item> output;
    Implementation implementation;
    Operator(Span span, bool private_, String name, Vector<Item> input, Vector<Item> output, Implementation implementation)
      : span(span),
        private_(private_),
        name(name),
        input(input),
        output(output),
        implementation(implementation) {}
};

struct Module;
struct Concept;
struct Use;
struct Nameable;

struct Structure {
    Span span;
    bool private_;
    HashMap<String, Property> properties;
    Vector<Module> modules;
    Vector<Use> uses;
    Vector<Initializer> initializers;
    DeInitializer* deinitializer;
    HashMap<String, Nameable> symbols;
    Structure(Span span, bool private_, HashMap<String, Property> properties, Vector<Module> modules, Vector<Use> uses, Vector<Initializer> initializers, DeInitializer* deinitializer, HashMap<String, Nameable> symbols)
      : span(span),
        private_(private_),
        properties(properties),
        modules(modules),
        uses(uses),
        initializers(initializers),
        deinitializer(deinitializer),
        symbols(symbols) {}
};


struct Variant : Object {
    Span span;
    String name;
    HashMap<String, Property> properties;
    Variant(Span span, String name, HashMap<String, Property> properties)
      : span(span),
        name(name),
        properties(properties) {}
};

struct Union : Object {
    Span span;
    bool private_;
    HashMap<String, Variant> variants;
    Union(Span span, bool private_, HashMap<String, Variant> variants)
      : span(span),
        private_(private_),
        variants(variants) {}
};

struct Namespace : Object {
    Span span;
    Vector<Module> modules;
    HashMap<String, Nameable> symbols;
    Namespace(Span span, Vector<Module> modules, HashMap<String, Nameable> symbols)
      : span(span),
        modules(modules),
        symbols(symbols) {}
};

struct Definition {
    enum {
        Intrinsic,
        Constant,
        Namespace,
        Structure,
        Union,
    } _tag;
    union {
        struct Intrinsic _Intrinsic;
        Operation _Constant;
        struct Namespace _Namespace;
        struct Structure _Structure;
        struct Union _Union;
    };
};

struct Use {
    Span span;
    Vector<String> path;
    Use(Span span, Vector<String> path)
      : span(span), path(path) {}
};

struct GenericParameter {
    Span span;
    String name;
    Vector<Attribute> attributes;
    GenericParameter(Span span, String name, Vector<Attribute> attributes)
      : span(span),
        name(name),
        attributes(attributes) {}
};

struct Concept : Object {
    Span span;
    String name;
    Vector<GenericParameter> parameters;
    Vector<Attribute> attributes;
    Definition definition;
    Concept(Span span, String name, Vector<GenericParameter> parameters, Vector<Attribute> attributes, Definition definition)
      : span(span),
        name(name),
        parameters(parameters),
        attributes(attributes),
        definition(definition) {}
};

struct Module : Object {
    bool private_;
    String file;
    String name;
    Vector<Module> modules;
    Vector<Use> uses;
    HashMap<String, Nameable> symbols;
    Module(bool private_, String file, String name, Vector<Module> modules, Vector<Use> uses, HashMap<String, Nameable> symbols)
      : private_(private_),
        file(file),
        name(name),
        modules(modules),
        uses(uses),
        symbols(symbols) {}
};

struct Program : Object {
    Vector<Module> packages;
    Module module;
    Vector<Statement> statements;
    Program(Vector<Module> packages, Module module, Vector<Statement> statements)
      : packages(packages),
        module(module),
        statements(statements) {}
};

struct Nameable {
    enum {
        Package,
        Concept,
        Operator,
        Functions,
    } _tag;
    union {
        struct Concept _Concept;
        struct Operator _Operator;
        Vector<Function> _Functions;
    };
};

}
}
}