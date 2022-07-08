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

struct Component : Object {
    Span span;
    String name;
    Vector<Operand> value;
    Component(Span span, String name, Vector<Operand> value) : span(span), name(name), value(value) {}
};

struct Name : Object {
    String path;
    Name(String path) : path(path) {}
};

struct Tuple : Object {
    Span span;
    Vector<Component> components;
    Tuple(Span span, Vector<Component> components) : span(span), components(components) {}
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

struct Block {

};

struct Expression {
    enum {
        Constant,
        Name,
        Tuple,
        Block,
    } _tag;
    union {
        struct Constant _Constant;
        struct Name _Name;
        struct Tuple _Tuple;
        struct Block _Block;
    };

};

struct Type : Object {
    String name;
    Type(String name) : name(name) {}
};

struct Property {
    String* name;
    Type* type;
    Property(String* name, Type* type) : name(name), type(type) {}
};

struct Postfix {

};

struct Operand : Object {
    Span span;
    Expression expression;
    Vector<Postfix>* postfixes;
    Operand(Span span, Expression expression, Vector<Postfix>* postfixes) : span(span), expression(expression), postfixes(postfixes) {}
};

struct Operation : Object {
    Vector<Operand> operands;
    Operation(Vector<Operand> operands) : operands(operands) {}
};

struct Statement;


struct Binding {
    enum Mutability { Constant, Extendable, Mutable, } binding_type;
    String identifier;
    Type* type;
    Operation operation;
    Vector<Statement> body;
    Binding(Mutability binding_type, String identifier, Type* type, Operation operation, Vector<Statement> body)
      : binding_type(binding_type), identifier(identifier), type(type), operation(operation), body(body) {}
};

struct Mutation : Object {
    Operation source;
    Operation target;
    Mutation(Operation source, Operation target) : source(source), target(target) {}
};

struct Action {
    enum {
        Operation,
        Mutation,
    } _tag;
    union {
        struct Operation _Operation;
        struct Mutation _Mutation;
    };

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
        Statement,
        Extern,
        Instruction,
        Intrinsic,
    } _tag;
    union {
        struct Statement _Statement;
        struct Extern _Extern;
        struct Instruction _Instruction;
        struct Intrinsic _Intrinsic;
    };
};

struct Function : Object {
    Span span;
    String name;
    Vector<Property>* input;
    Vector<Property>* output;
    Implementation implementation;
    Function(Span span, String name, Vector<Property>* input, Vector<Property>* output, Implementation implementation)
      : span(span),
        name(name),
        input(input),
        output(output),
        implementation(implementation) {}
};

struct Operator : Object {
    Span span;
    String name;
    Vector<Property>* output;
    Operation* operation;
    Operator(Span span, String name, Vector<Property>* output, Operation* operation)
      : span(span),
        name(name),
        output(output),
        operation(operation) {}
};

struct Module;
struct Concept;
struct Nameable;

struct Code {
    HashMap<String, Nameable> symbols;
    Code(HashMap<String, Nameable> symbols)
      : symbols(symbols) {}
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
    Code code;
    NameSpace(Code code)
      : code(code) {}
};

struct Body {
    enum {
        Intrinsic,
        Constant,
        NameSpace,
        Structure,
        Union,
    } _tag;
    union {
        struct Intrinsic _Intrinsic;
        Operation _Constant;
        struct NameSpace _NameSpace;
        struct Structure _Structure;
        struct Union _Union;
    };
};

struct Concept : Object {
    Span span;
    String name;
    Body body;
    Concept(Span span, String name, Body body)
      : span(span),
        name(name),
        body(body) {}
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

struct Nameable {
    enum {
        Module,
        Concept,
        Operator,
        Functions,
    } _tag;
    union {
        struct Module _Module;
        struct Concept _Concept;
        struct Operator _Operator;
        Vector<Function> _Functions;
    };
};

}
}
}