namespace scaly {
namespace compiler {
namespace model {

using namespace scaly::io;

struct Span : Object {
    size_t start;
    size_t end;
    Span(size_t start, size_t end) : start(start), end(end) {}
};

struct NameExpression : Object {
    String path;
    NameExpression(String path) : path(path) {}
};

struct Expression;

struct Operand : Object {
    Expression* expression;
};

struct Component : Object {
    Span span;
    String name;
    Vector<Operand> value;
    Component(Span span, String name, Vector<Operand> value) : span(span), name(name), value(value) {}
};


struct TupleExpression : Object {
    Span span;
    Vector<Component> components;
    TupleExpression(Span span, Vector<Component> components) : span(span), components(components) {}
};

struct NameExpression;
struct TupleExpression;
struct Expression : Object {
    Expression(NameExpression nameExpression) : tag(Name), nameExpression(nameExpression) {}
    Expression(TupleExpression tupleExpression) : tag(Tuple), tupleExpression(tupleExpression) {}
    enum {
        Name,
        Tuple,
    } tag;
    union {
        NameExpression nameExpression;
        TupleExpression tupleExpression;
    };

};

struct Operation : Object {
    Vector<Operand> operands;
    Operation(Vector<Operand> operands) : operands(operands) {}
};

struct Type {
    String name;
    Type(String name) : name(name) {}
};

struct Property {
    String* name;
    Type* type;
    Property(String* name, Type* type) : name(name), type(type) {}
};

struct Function : Object {
    Span span;
    String name;
    Vector<Property>* input;
    Vector<Property>* output;
    Operation* operation;
    Function(Span span, String name, Vector<Property>* input, Vector<Property>* output, Operation* operation)
      : span(span),
        name(name),
        input(input),
        output(output),
        operation(operation) {}
};

struct Module;
struct Concept;

struct Code {
    HashMap<String, Module> modules;
    HashMap<String, Concept> concepts;
    MultiMap<String, Function> functions;
    Code(HashMap<String, Module> modules, HashMap<String, Concept> concepts, MultiMap<String, Function> functions)
      : modules(modules),
        concepts(concepts),
        functions(functions) {}
};

struct Structure{
    Span span;
    HashMap<String, Property> properties;
    Code code;
    Structure(Span span, HashMap<String, Property> properties, Code code)
      : span(span),
        properties(properties),
        code(code) {}
};

struct Union {

};

struct NameSpace {
    MultiMap<String, Function> functions;
    NameSpace(MultiMap<String, Function> functions)
      : functions(functions) {}
};

struct Intrinsic {

};

struct Implementation {
    enum {
        Intrinsic,
        Constant,
        NameSpace,
        Structure,
        Union,
    } _tag;
    union {
        struct Intrinsic _Intrinsic;
        String _Constant;
        struct NameSpace _NameSpace;
        struct Structure _Structure;
        struct Union _Union;
    };
};

struct Concept : Object {
    Span span;
    Type type;
    Implementation implementation;
    Concept(Span span, Type type, Implementation implementation)
      : span(span),
        type(type),
        implementation(implementation) {}
};

struct Text {
    enum {
        Program,
        File,
    } _tag;
    union {
        String _Program;
        String _File;
    };
};

struct Module : Object {
    String name;
    Text text;
    Concept concept;
    Module(String name, Text text, Concept concept)
      : name(name),
        text(text),
        concept(concept) {}
};

}
}
}