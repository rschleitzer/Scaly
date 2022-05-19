namespace scaly {
namespace compiler {
namespace model {

struct Span : Object {
    size_t file_id;
    size_t start;
    size_t end;
    Span(size_t file_id, size_t start, size_t end) : file_id(file_id), start(start), end(end) {}
};

struct Expression;

struct Parameter {
    String name;
};


struct NameExpression : Object {
    String path;
    NameExpression(String path) : path(path) {}
};

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

struct Routine : Object {
    Span span;
    Vector<Parameter> input;
    Vector<Parameter> result;
    Operation operation;
    Routine(Span span, Vector<Parameter> input, Vector<Parameter> result, Operation operation) : span(span), input(input), result(result), operation(operation) {}
};

struct Function : Object {
    Span span;
    String name;
    Routine routine;
    Function(Span span, String name, Routine routine) : span(span), name(name), routine(routine) {}
};

struct Model : Object {
    HashMap<String, List<Function>> functions; 
};

}
}
}