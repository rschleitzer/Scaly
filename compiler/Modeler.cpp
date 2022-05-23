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

struct ModelError : Object {
    ModelError(String file_name, size_t position) : file_name(file_name), start(position), end(position) {}
    ModelError(ParserError* parser_error) : file_name(parser_error->text), start(parser_error->start), end(parser_error->end) {}
    String file_name;
    size_t start;
    size_t end;
};

Result<Model*, ModelError*> build_model(Region& _pr, Page* _rp, Page* _ep, FileSyntax& file_syntax) {
    auto ret = new(alignof(Model), _rp) Model();
    return Result<Model*, ModelError*> { .tag = Result<Model*, ModelError*>::Ok, .ok = ret };
}

Result<Model*, ModelError*> build_program_model(Region& _pr, Page* _rp, Page* _ep, String& program) {
    auto _r = Region::create(_pr);
    Parser& parser = *new (alignof(Parser), _r.page) Parser(program);
    auto file_syntax_result = parser.parse_file(_r, _r.page, _r.page, *String::from_c_string(_r.page, "<test>"));
    if (file_syntax_result.tag == Result<FileSyntax*, ParserError*>::Error)
        return Result<Model*, ModelError*> { .tag = Result<Model*, ModelError*>::Error, .error = new(alignof(ModelError), _ep) ModelError(file_syntax_result.error) };
    auto model = build_model(_r, _rp, _ep, *file_syntax_result.ok);
    auto ret = new(alignof(Model), _rp) Model();
    return Result<Model*, ModelError*> { .tag = Result<Model*, ModelError*>::Ok, .ok = ret };
}

}
}
}