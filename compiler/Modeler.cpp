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
    HashMap<String, Vector<Function>> functions;
    Vector<Operation> operations;
    Model(HashMap<String, Vector<Function>> functions, Vector<Operation> operations) : functions(functions), operations(operations) {};
};

struct ModelError : Object {
    ModelError(String file_name, size_t position) : file_name(file_name), start(position), end(position) {}
    ModelError(ParserError* parser_error) : file_name(parser_error->text), start(parser_error->start), end(parser_error->end) {}
    String file_name;
    size_t start;
    size_t end;
};

Result<Model*, ModelError*> build_model(Region& _pr, Page* _rp, Page* _ep, Vector<DeclarationSyntax>& declarations) {
    auto _r = Region::create(_pr);
    HashMapBuilder<String, Vector<Function>>& functions_builder = *new(alignof(HashMapBuilder<String, Vector<Function>>), _r.page) HashMapBuilder<String, Vector<Function>>();
    Array<Operation>& operations_array = *new(alignof(Array<Operation>), _r.page) Array<Operation>(); 

    HashMap<String, Vector<Function>>& functions = *HashMap<String, Vector<Function>>::from_hash_map_builder(_r, _rp, functions_builder);
    Vector<Operation>& operations = *Vector<Operation>::from_array(_rp, operations_array);
    auto ret = new(alignof(Model), _rp) Model(functions, operations);
    return Result<Model*, ModelError*> { .tag = Result<Model*, ModelError*>::Ok, .ok = ret };
}


Result<RoutineSyntax*, ParserError*> parse_main_function_stub_routine(Region& _pr, Page* _rp, Page* _ep, Parser& parser) {
    auto _r = Region::create(_pr);
    auto start = parser.lexer.previous_position;

    auto parameters_result = parser.parse_parameterset(_r, _rp, _ep);
    if (parameters_result.tag == Result<ParameterSetSyntax*, ParserError*>::Error)
        return Result<RoutineSyntax*, ParserError*> { .tag = Result<RoutineSyntax*, ParserError*>::Error, .error = parameters_result.error };
    auto parameters = parameters_result.ok;

    auto returns_result = parser.parse_returns(_r, _rp, _ep);
    if (returns_result.tag == Result<ReturnsSyntax*, ParserError*>::Error)
        return Result<RoutineSyntax*, ParserError*> { .tag = Result<RoutineSyntax*, ParserError*>::Error, .error = returns_result.error };
    auto returns = returns_result.ok;

    auto end = parser.lexer.position;

    auto ret = new(alignof(RoutineSyntax), _rp) RoutineSyntax(start, end, parameters, nullptr, returns, nullptr, nullptr);

    return Result<RoutineSyntax*, ParserError*> { .tag = Result<RoutineSyntax*, ParserError*>::Ok, .ok = ret };
}

Result<FunctionSyntax*, ParserError*> parse_main_function_stub(Region& _pr, Page* _rp, Page* _ep) {
    auto _r = Region::create(_pr);
    Parser& parser = *new(alignof(Parser), _r.page) Parser(*String::from_c_string(_r.page, "function main('argument count': int, 'argument values': pointer[pointer[byte]]) returns int"));
    auto start = parser.lexer.previous_position;

    auto success_function = parser.lexer.parse_keyword(_r, _r.page, *String::from_c_string(_r.page, "function"));
    if (!success_function)
        return Result<FunctionSyntax*, ParserError*> { .tag = Result<FunctionSyntax*, ParserError*>::Ok, .ok = nullptr };

    auto name = parser.lexer.parse_identifier(_r, _r.page, parser.keywords);
    if (name != nullptr) {
        if (!parser.is_identifier(*name)) {
            return Result<FunctionSyntax*, ParserError*> { .tag = Result<FunctionSyntax*, ParserError*>::Error, .error = new(alignof(ParserError), _ep) ParserError(*parser.text.copy(_ep), start, parser.lexer.position) };
        }
    }
    else
        return Result<FunctionSyntax*, ParserError*> { .tag = Result<FunctionSyntax*, ParserError*>::Error, .error = new(alignof(ParserError), _ep) ParserError(*parser.text.copy(_ep), start, parser.lexer.position) };

    auto routine_result = parse_main_function_stub_routine(_r, _rp, _ep, parser);
    if (routine_result.tag == Result<RoutineSyntax*, ParserError*>::Error)
        return Result<FunctionSyntax*, ParserError*> { .tag = Result<FunctionSyntax*, ParserError*>::Error, .error = routine_result.error };
    auto routine = routine_result.ok;
    if (routine == nullptr)
        return Result<FunctionSyntax*, ParserError*> { .tag = Result<FunctionSyntax*, ParserError*>::Error, .error = new(alignof(ParserError), _ep) ParserError(*parser.text.copy(_ep), start, parser.lexer.position) };

    auto end = parser.lexer.position;
    auto ret = new(alignof(FunctionSyntax), _rp) FunctionSyntax(start, end, name, nullptr, routine);
    return Result<FunctionSyntax*, ParserError*> { .tag = Result<FunctionSyntax*, ParserError*>::Ok, .ok = ret };
}

Result<Model*, ModelError*> build_program_model(Region& _pr, Page* _rp, Page* _ep, String& program) {
    auto _r = Region::create(_pr);

    // Parse the scaly module inclusion
    Parser& parser_module = *new(alignof(Parser), _r.page) Parser(*String::from_c_string(_r.page, "module scaly"));
    auto module_syntax_result = parser_module.parse_module(_r, _r.page, _r.page);
    if (module_syntax_result.tag == Result<ModuleSyntax*, ParserError*>::Error)
        return Result<Model*, ModelError*> { .tag = Result<Model*, ModelError*>::Error, .error = new(alignof(ModelError), _ep) ModelError(module_syntax_result.error) };
    auto module_syntax = module_syntax_result.ok;
    DeclarationSyntax* module_declaration = nullptr;
    if (module_syntax != nullptr)
        module_declaration =  new (alignof(DeclarationSyntax), _r.page) DeclarationSyntax(ModuleSyntax(*module_syntax));

    // Parse the declarations of the program
    Parser& parser = *new(alignof(Parser), _r.page) Parser(program);
    Array<DeclarationSyntax>* array = Array<DeclarationSyntax>::create(_r.page);
    while(true) {
        auto node_result = parser.parse_declaration(_r, _r.page, _r.page);
        if (node_result.tag == Result<DeclarationSyntax*, ParserError*>::Error)
            return Result<Model*, ModelError*> { .tag = Result<Model*, ModelError*>::Error, .error = new(alignof(ModelError), _ep) ModelError(node_result.error) };
        auto node = node_result.ok;
        if (node != nullptr) {
            array->add(*node);
        } else {
            break;
        }
    }

    // Parse the main function stub
    auto main_function_syntax_result = parse_main_function_stub(_r, _r.page, _r.page);
    if (module_syntax_result.tag == Result<FunctionSyntax*, ParserError*>::Error)
        return Result<Model*, ModelError*> { .tag = Result<Model*, ModelError*>::Error, .error = new(alignof(ModelError), _ep) ModelError(module_syntax_result.error) };
    auto main_function_syntax = main_function_syntax_result.ok;
    DeclarationSyntax* main_function_declaration = nullptr;
    if (main_function_syntax != nullptr)
        main_function_declaration =  new (alignof(DeclarationSyntax), _r.page) DeclarationSyntax(FunctionSyntax(*main_function_syntax));

    auto declarations = Vector<DeclarationSyntax>::from_array(_rp, *array);
    auto model_result = build_model(_r, _rp, _ep, *declarations);
    if (model_result.tag == Result<Model*, ModelError*>::Error)
        return model_result;
    auto model = model_result.ok;

    auto operations = model->operations;

    return Result<Model*, ModelError*> { .tag = Result<Model*, ModelError*>::Ok, .ok = model };
}

}
}
}