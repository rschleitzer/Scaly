namespace scaly {
namespace compiler {
namespace model {

using namespace scaly::io;

struct Span : Object {
    size_t start;
    size_t end;
    Span(size_t start, size_t end) : start(start), end(end) {}
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

struct Concept : Object {
    Span span;
    String name;
    Type type;
};

struct Model : Object {
    HashMap<String, Concept> concepts;
    MultiMap<String, Function> functions;
    Model(HashMap<String, Concept> concepts, MultiMap<String, Function> functions)
      : concepts(concepts),
        functions(functions) {}
};

}
}
}