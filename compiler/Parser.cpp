namespace scaly::compiler {

struct ProgramSyntax; 
struct FileSyntax; 
struct DeclarationSyntax; 
struct PrivateSyntax; 
struct ExportSyntax; 
struct DefinitionSyntax; 
struct NameSyntax; 
struct ExtensionSyntax; 
struct AttributeSyntax; 
struct ConceptSyntax; 
struct ClassSyntax; 
struct NamespaceSyntax; 
struct UnionSyntax; 
struct TagSyntax; 
struct ItemSyntax; 
struct VariantSyntax; 
struct EnumSyntax; 
struct ConstantSyntax; 
struct DelegateSyntax; 
struct StructureSyntax; 
struct BodySyntax; 
struct MemberSyntax; 
struct FieldSyntax; 
struct PropertySyntax; 
struct TypeAnnotationSyntax; 
struct TypeSpecSyntax; 
struct TypeSyntax; 
struct ArraySyntax; 
struct GenericArgumentsSyntax; 
struct GenericArgumentSyntax; 
struct OptionalSyntax; 
struct FunctionSyntax; 
struct ProcedureSyntax; 
struct OperatorSyntax; 
struct TargetSyntax; 
struct RoutineSyntax; 
struct SymbolSyntax; 
struct ParameterSetSyntax; 
struct ParametersSyntax; 
struct ReturnsSyntax; 
struct ThrowsSyntax; 
struct ActionSyntax; 
struct ImplementationSyntax; 
struct ExternSyntax; 
struct InstructionSyntax; 
struct IntrinsicSyntax; 
struct UseSyntax; 
struct ImplementSyntax; 
struct TraitSyntax; 
struct MethodSyntax; 
struct ExtendsSyntax; 
struct ExtendSyntax; 
struct MacroSyntax; 
struct ModuleSyntax; 
struct ModelSyntax; 
struct StatementSyntax; 
struct LetSyntax; 
struct VarSyntax; 
struct MutableSyntax; 
struct BindingSyntax; 
struct BindingAnnotationSyntax; 
struct BindingSpecSyntax; 
struct SetSyntax; 
struct OperationSyntax; 
struct OperandSyntax; 
struct PostfixSyntax; 
struct MemberAccessSyntax; 
struct CatcherSyntax; 
struct CatchSyntax; 
struct DropSyntax; 
struct ContinueSyntax; 
struct LoopSyntax; 
struct BreakSyntax; 
struct ReturnSyntax; 
struct ThrowSyntax; 
struct ExpressionSyntax; 
struct LiteralSyntax; 
struct ObjectSyntax; 
struct ComponentSyntax; 
struct ValueSyntax; 
struct VectorSyntax; 
struct ElementSyntax; 
struct BlockSyntax; 
struct IfSyntax; 
struct IsSyntax; 
struct AsSyntax; 
struct ElseSyntax; 
struct MatchSyntax; 
struct CaseSyntax; 
struct DefaultSyntax; 
struct LambdaSyntax; 
struct ForSyntax; 
struct LabelSyntax; 
struct WhileSyntax; 
struct RepeatSyntax; 
struct SizeOfSyntax; 

struct ProgramSyntax : Object {
    Vector<FileSyntax>* files;
};

struct FileSyntax : Object {
    String file_name;
    Vector<DeclarationSyntax>* declarations;
    Vector<StatementSyntax>* statements;
};

struct PrivateSyntax : Object {
    size_t start;
    size_t end;
    ExportSyntax* export_;
};

struct DefinitionSyntax : Object {
    size_t start;
    size_t end;
    TypeSyntax* type;
    Vector<AttributeSyntax>* attributes;
    ConceptSyntax* concept_;
};

struct NameSyntax : Object {
    size_t start;
    size_t end;
    String* name;
    Vector<ExtensionSyntax>* extensions;
};

struct ExtensionSyntax : Object {
    size_t start;
    size_t end;
    String* name;
};

struct AttributeSyntax : Object {
    size_t start;
    size_t end;
    String* attribute;
    ModelSyntax* value;
};

struct ClassSyntax : Object {
    size_t start;
    size_t end;
    StructureSyntax* structure;
    BodySyntax* body;
};

struct NamespaceSyntax : Object {
    size_t start;
    size_t end;
    BodySyntax* body;
};

struct UnionSyntax : Object {
    size_t start;
    size_t end;
    Vector<TagSyntax>* tags;
    BodySyntax* body;
};

struct TagSyntax : Object {
    size_t start;
    size_t end;
    String* name;
    Vector<AttributeSyntax>* attributes;
    ItemSyntax* item;
};

struct VariantSyntax : Object {
    size_t start;
    size_t end;
    StructureSyntax* structure;
    BodySyntax* body;
};

struct EnumSyntax : Object {
    size_t start;
    size_t end;
    LiteralToken literal;
};

struct ConstantSyntax : Object {
    size_t start;
    size_t end;
    OperationSyntax* operation;
};

struct DelegateSyntax : Object {
    size_t start;
    size_t end;
    ParameterSetSyntax* parameters;
    Vector<AttributeSyntax>* attributes;
    ReturnsSyntax* result;
    ThrowsSyntax* error;
};

struct StructureSyntax : Object {
    size_t start;
    size_t end;
    Vector<MemberSyntax>* members;
};

struct BodySyntax : Object {
    size_t start;
    size_t end;
    Vector<DeclarationSyntax>* declarations;
};

struct FieldSyntax : Object {
    size_t start;
    size_t end;
    PropertySyntax* property;
};

struct PropertySyntax : Object {
    size_t start;
    size_t end;
    String* name;
    TypeAnnotationSyntax* annotation;
    Vector<AttributeSyntax>* attributes;
};

struct TypeAnnotationSyntax : Object {
    size_t start;
    size_t end;
    TypeSpecSyntax* spec;
};

struct TypeSyntax : Object {
    size_t start;
    size_t end;
    NameSyntax* name;
    GenericArgumentsSyntax* generics;
    OptionalSyntax* optional;
};

struct ArraySyntax : Object {
    size_t start;
    size_t end;
    Vector<TypeSpecSyntax>* members;
};

struct GenericArgumentsSyntax : Object {
    size_t start;
    size_t end;
    Vector<GenericArgumentSyntax>* generics;
};

struct GenericArgumentSyntax : Object {
    size_t start;
    size_t end;
    TypeSyntax* spec;
};

struct OptionalSyntax : Object {
    size_t start;
    size_t end;
};

struct FunctionSyntax : Object {
    size_t start;
    size_t end;
    String* name;
    GenericArgumentsSyntax* generics;
    RoutineSyntax* routine;
};

struct ProcedureSyntax : Object {
    size_t start;
    size_t end;
    String* name;
    GenericArgumentsSyntax* generics;
    RoutineSyntax* routine;
};

struct OperatorSyntax : Object {
    size_t start;
    size_t end;
    TargetSyntax* target;
};

struct RoutineSyntax : Object {
    size_t start;
    size_t end;
    ParameterSetSyntax* parameters;
    Vector<AttributeSyntax>* attributes;
    ReturnsSyntax* returns;
    ThrowsSyntax* throws;
    ImplementationSyntax* implementation;
};

struct SymbolSyntax : Object {
    size_t start;
    size_t end;
    String* name;
    Vector<AttributeSyntax>* attributes;
    ReturnsSyntax* returns;
    ThrowsSyntax* throws;
    ImplementationSyntax* implementation;
};

struct ParametersSyntax : Object {
    size_t start;
    size_t end;
    Vector<PropertySyntax>* properties;
};

struct ReturnsSyntax : Object {
    size_t start;
    size_t end;
    ParameterSetSyntax* parameters;
    Vector<AttributeSyntax>* attributes;
};

struct ThrowsSyntax : Object {
    size_t start;
    size_t end;
    TypeSyntax* type;
    Vector<AttributeSyntax>* attributes;
};

struct ExternSyntax : Object {
    size_t start;
    size_t end;
};

struct InstructionSyntax : Object {
    size_t start;
    size_t end;
};

struct IntrinsicSyntax : Object {
    size_t start;
    size_t end;
};

struct UseSyntax : Object {
    size_t start;
    size_t end;
    NameSyntax* name;
};

struct ImplementSyntax : Object {
    size_t start;
    size_t end;
    TypeSyntax* type;
    Vector<AttributeSyntax>* attributes;
    Vector<MethodSyntax>* methods;
};

struct TraitSyntax : Object {
    size_t start;
    size_t end;
    NameSyntax* name;
    ExtendsSyntax* extension;
    Vector<AttributeSyntax>* attributes;
    Vector<MethodSyntax>* functions;
};

struct ExtendsSyntax : Object {
    size_t start;
    size_t end;
    Vector<ExtendSyntax>* extensions;
};

struct ExtendSyntax : Object {
    size_t start;
    size_t end;
    TypeSpecSyntax* spec;
};

struct MacroSyntax : Object {
    size_t start;
    size_t end;
    String* name;
    ModelSyntax* model;
    OperationSyntax* rule;
};

struct ModuleSyntax : Object {
    size_t start;
    size_t end;
    NameSyntax* name;
};

struct LetSyntax : Object {
    size_t start;
    size_t end;
    BindingSyntax* binding;
};

struct VarSyntax : Object {
    size_t start;
    size_t end;
    BindingSyntax* binding;
};

struct MutableSyntax : Object {
    size_t start;
    size_t end;
    BindingSyntax* binding;
};

struct BindingSyntax : Object {
    size_t start;
    size_t end;
    String* name;
    BindingAnnotationSyntax* annotation;
    OperationSyntax* operation;
};

struct BindingAnnotationSyntax : Object {
    size_t start;
    size_t end;
    BindingSpecSyntax* spec;
};

struct SetSyntax : Object {
    size_t start;
    size_t end;
    OperationSyntax* target;
    OperationSyntax* source;
};

struct OperationSyntax : Object {
    size_t start;
    size_t end;
    Vector<OperandSyntax>* operands;
};

struct OperandSyntax : Object {
    size_t start;
    size_t end;
    ExpressionSyntax* expression;
    Vector<PostfixSyntax>* postfixes;
};

struct MemberAccessSyntax : Object {
    size_t start;
    size_t end;
    NameSyntax* member;
};

struct CatcherSyntax : Object {
    size_t start;
    size_t end;
    Vector<CatchSyntax>* catchers;
    DropSyntax* dropper;
};

struct CatchSyntax : Object {
    size_t start;
    size_t end;
    OperationSyntax* condition;
    OperationSyntax* handler;
};

struct DropSyntax : Object {
    size_t start;
    size_t end;
    OperationSyntax* handler;
};

struct ContinueSyntax : Object {
    size_t start;
    size_t end;
    LoopSyntax* name;
};

struct LoopSyntax : Object {
    size_t start;
    size_t end;
    String* name;
};

struct BreakSyntax : Object {
    size_t start;
    size_t end;
    LoopSyntax* name;
    OperationSyntax* result;
};

struct ReturnSyntax : Object {
    size_t start;
    size_t end;
    OperationSyntax* result;
};

struct ThrowSyntax : Object {
    size_t start;
    size_t end;
    OperationSyntax* result;
};

struct LiteralSyntax : Object {
    size_t start;
    size_t end;
    LiteralToken literal;
};

struct ObjectSyntax : Object {
    size_t start;
    size_t end;
    Vector<ComponentSyntax>* components;
};

struct ComponentSyntax : Object {
    size_t start;
    size_t end;
    Vector<OperandSyntax>* operands;
    Vector<AttributeSyntax>* attributes;
    ValueSyntax* value;
};

struct ValueSyntax : Object {
    size_t start;
    size_t end;
    OperationSyntax* value;
    Vector<AttributeSyntax>* attributes;
};

struct VectorSyntax : Object {
    size_t start;
    size_t end;
    Vector<ElementSyntax>* elements;
};

struct ElementSyntax : Object {
    size_t start;
    size_t end;
    OperationSyntax* operation;
    Vector<AttributeSyntax>* attributes;
};

struct BlockSyntax : Object {
    size_t start;
    size_t end;
    Vector<StatementSyntax>* statements;
};

struct IfSyntax : Object {
    size_t start;
    size_t end;
    OperationSyntax* condition;
    IsSyntax* match;
    AsSyntax* alias;
    ActionSyntax* consequent;
    ElseSyntax* alternative;
};

struct IsSyntax : Object {
    size_t start;
    size_t end;
    TypeSyntax* type;
};

struct AsSyntax : Object {
    size_t start;
    size_t end;
    String* name;
};

struct ElseSyntax : Object {
    size_t start;
    size_t end;
    ActionSyntax* alternative;
};

struct MatchSyntax : Object {
    size_t start;
    size_t end;
    OperationSyntax* scrutinee;
    Vector<CaseSyntax>* cases;
    DefaultSyntax* alternative;
};

struct CaseSyntax : Object {
    size_t start;
    size_t end;
    OperationSyntax* condition;
    ActionSyntax* consequent;
};

struct DefaultSyntax : Object {
    size_t start;
    size_t end;
    ActionSyntax* alternative;
};

struct LambdaSyntax : Object {
    size_t start;
    size_t end;
    OperationSyntax* input;
    ActionSyntax* block;
};

struct ForSyntax : Object {
    size_t start;
    size_t end;
    OperationSyntax* condition;
    OperationSyntax* expression;
    LabelSyntax* name;
    ActionSyntax* action;
};

struct LabelSyntax : Object {
    size_t start;
    size_t end;
    String* name;
};

struct WhileSyntax : Object {
    size_t start;
    size_t end;
    OperationSyntax* condition;
    LabelSyntax* name;
    ActionSyntax* action;
};

struct RepeatSyntax : Object {
    size_t start;
    size_t end;
    LabelSyntax* name;
    ActionSyntax* action;
};

struct SizeOfSyntax : Object {
    size_t start;
    size_t end;
    TypeSyntax* type;
};

struct DeclarationSyntax : Object {
    enum {
        Private,
        Definition,
        Function,
        Procedure,
        Operator,
        Use,
        Implement,
        Trait,
        Macro,
        Module,
    } tag;
    union {
        PrivateSyntax privateSyntax;
        DefinitionSyntax definitionSyntax;
        FunctionSyntax functionSyntax;
        ProcedureSyntax procedureSyntax;
        OperatorSyntax operatorSyntax;
        UseSyntax useSyntax;
        ImplementSyntax implementSyntax;
        TraitSyntax traitSyntax;
        MacroSyntax macroSyntax;
        ModuleSyntax moduleSyntax;
    };

};

struct ExportSyntax : Object {
    enum {
        Definition,
        Function,
        Procedure,
        Operator,
        Implement,
        Trait,
        Macro,
        Module,
    } tag;
    union {
        DefinitionSyntax definitionSyntax;
        FunctionSyntax functionSyntax;
        ProcedureSyntax procedureSyntax;
        OperatorSyntax operatorSyntax;
        ImplementSyntax implementSyntax;
        TraitSyntax traitSyntax;
        MacroSyntax macroSyntax;
        ModuleSyntax moduleSyntax;
    };

};

struct ConceptSyntax : Object {
    enum {
        Class,
        Namespace,
        Union,
        Constant,
        Delegate,
        Intrinsic,
    } tag;
    union {
        ClassSyntax classSyntax;
        NamespaceSyntax namespaceSyntax;
        UnionSyntax unionSyntax;
        ConstantSyntax constantSyntax;
        DelegateSyntax delegateSyntax;
        IntrinsicSyntax intrinsicSyntax;
    };

};

struct ItemSyntax : Object {
    enum {
        Variant,
        Enum,
    } tag;
    union {
        VariantSyntax variantSyntax;
        EnumSyntax enumSyntax;
    };

};

struct MemberSyntax : Object {
    enum {
        Field,
        Property,
    } tag;
    union {
        FieldSyntax fieldSyntax;
        PropertySyntax propertySyntax;
    };

};

struct TypeSpecSyntax : Object {
    enum {
        Structure,
        Type,
    } tag;
    union {
        StructureSyntax structureSyntax;
        TypeSyntax typeSyntax;
    };

};

struct TargetSyntax : Object {
    enum {
        Symbol,
        Routine,
    } tag;
    union {
        SymbolSyntax symbolSyntax;
        RoutineSyntax routineSyntax;
    };

};

struct ParameterSetSyntax : Object {
    enum {
        Parameters,
        Type,
    } tag;
    union {
        ParametersSyntax parametersSyntax;
        TypeSyntax typeSyntax;
    };

};

struct ActionSyntax : Object {
    enum {
        Operation,
        Set,
    } tag;
    union {
        OperationSyntax operationSyntax;
        SetSyntax setSyntax;
    };

};

struct ImplementationSyntax : Object {
    enum {
        Action,
        Extern,
        Instruction,
        Intrinsic,
    } tag;
    union {
        ActionSyntax actionSyntax;
        ExternSyntax externSyntax;
        InstructionSyntax instructionSyntax;
        IntrinsicSyntax intrinsicSyntax;
    };

};

struct MethodSyntax : Object {
    enum {
        Function,
        Procedure,
        Operator,
    } tag;
    union {
        FunctionSyntax functionSyntax;
        ProcedureSyntax procedureSyntax;
        OperatorSyntax operatorSyntax;
    };

};

struct ModelSyntax : Object {
    enum {
        Literal,
        Name,
        Object,
        Vector,
    } tag;
    union {
        LiteralSyntax literalSyntax;
        NameSyntax nameSyntax;
        ObjectSyntax objectSyntax;
        VectorSyntax vectorSyntax;
    };

};

struct StatementSyntax : Object {
    enum {
        Operation,
        Let,
        Var,
        Mutable,
        Set,
    } tag;
    union {
        OperationSyntax operationSyntax;
        LetSyntax letSyntax;
        VarSyntax varSyntax;
        MutableSyntax mutableSyntax;
        SetSyntax setSyntax;
    };

};

struct BindingSpecSyntax : Object {
    enum {
        Structure,
        Type,
        Array,
    } tag;
    union {
        StructureSyntax structureSyntax;
        TypeSyntax typeSyntax;
        ArraySyntax arraySyntax;
    };

};

struct PostfixSyntax : Object {
    enum {
        MemberAccess,
        Catcher,
    } tag;
    union {
        MemberAccessSyntax memberAccessSyntax;
        CatcherSyntax catcherSyntax;
    };

};

struct ExpressionSyntax : Object {
    enum {
        Literal,
        Name,
        Object,
        Vector,
        Block,
        If,
        Match,
        Lambda,
        For,
        While,
        Repeat,
        SizeOf,
        Continue,
        Break,
        Return,
        Throw,
    } tag;
    union {
        LiteralSyntax literalSyntax;
        NameSyntax nameSyntax;
        ObjectSyntax objectSyntax;
        VectorSyntax vectorSyntax;
        BlockSyntax blockSyntax;
        IfSyntax ifSyntax;
        MatchSyntax matchSyntax;
        LambdaSyntax lambdaSyntax;
        ForSyntax forSyntax;
        WhileSyntax whileSyntax;
        RepeatSyntax repeatSyntax;
        SizeOfSyntax sizeOfSyntax;
        ContinueSyntax continueSyntax;
        BreakSyntax breakSyntax;
        ReturnSyntax returnSyntax;
        ThrowSyntax throwSyntax;
    };

};

using namespace scaly::containers;

struct Parser : Object {
    Lexer lexer;
    String file_name;
    HashSet<String> keywords;

    Parser(String& deck)
      : lexer(*new(alignof(Lexer), Page::get(this)) Lexer(deck)),
        file_name(*String::from_c_string(Page::get(this), ""))
    {
        auto r = Region::create_from_page(Page::get(this));
        Array<String>& array = *Array<String>::create(r.page);
        array.add(*String::from_c_string(r.page, "as"));
        array.add(*String::from_c_string(r.page, "break"));
        array.add(*String::from_c_string(r.page, "catch"));
        array.add(*String::from_c_string(r.page, "case"));
        array.add(*String::from_c_string(r.page, "continue"));
        array.add(*String::from_c_string(r.page, "define"));
        array.add(*String::from_c_string(r.page, "default"));
        array.add(*String::from_c_string(r.page, "delegate"));
        array.add(*String::from_c_string(r.page, "drop"));
        array.add(*String::from_c_string(r.page, "else"));
        array.add(*String::from_c_string(r.page, "extends"));
        array.add(*String::from_c_string(r.page, "extern"));
        array.add(*String::from_c_string(r.page, "for"));
        array.add(*String::from_c_string(r.page, "function"));
        array.add(*String::from_c_string(r.page, "if"));
        array.add(*String::from_c_string(r.page, "is"));
        array.add(*String::from_c_string(r.page, "implement"));
        array.add(*String::from_c_string(r.page, "in"));
        array.add(*String::from_c_string(r.page, "instruction"));
        array.add(*String::from_c_string(r.page, "intrinsic"));
        array.add(*String::from_c_string(r.page, "label"));
        array.add(*String::from_c_string(r.page, "lambda"));
        array.add(*String::from_c_string(r.page, "let"));
        array.add(*String::from_c_string(r.page, "loop"));
        array.add(*String::from_c_string(r.page, "macro"));
        array.add(*String::from_c_string(r.page, "match"));
        array.add(*String::from_c_string(r.page, "module"));
        array.add(*String::from_c_string(r.page, "mutable"));
        array.add(*String::from_c_string(r.page, "operator"));
        array.add(*String::from_c_string(r.page, "procedure"));
        array.add(*String::from_c_string(r.page, "private"));
        array.add(*String::from_c_string(r.page, "return"));
        array.add(*String::from_c_string(r.page, "returns"));
        array.add(*String::from_c_string(r.page, "repeat"));
        array.add(*String::from_c_string(r.page, "set"));
        array.add(*String::from_c_string(r.page, "sizeof"));
        array.add(*String::from_c_string(r.page, "throw"));
        array.add(*String::from_c_string(r.page, "throws"));
        array.add(*String::from_c_string(r.page, "trait"));
        array.add(*String::from_c_string(r.page, "union"));
        array.add(*String::from_c_string(r.page, "use"));
        array.add(*String::from_c_string(r.page, "var"));
        array.add(*String::from_c_string(r.page, "while"));
        Vector<String>& vector = *Vector<String>::from_array(r.page, array);
        keywords = *HashSet<String>::from_vector(Page::get(this), vector);

    }

    Result<FileSyntax*, ParserError*> parse_file(Region& _pr, Page* _rp, Page* _ep, String& file_name) {
        auto _r = Region::create(_pr);

        auto declarations_result = this->parse_declaration_list(_r, _rp, _ep);
        if (declarations_result.tag == Result<Vector<DeclarationSyntax>*, ParserError*>::Error)
            return Result<FileSyntax*, ParserError*> { .tag = Result<FileSyntax*, ParserError*>::Error, .error = declarations_result.error };
        auto declarations = declarations_result.ok;

        auto statements_result = this->parse_statement_list(_r, _rp, _ep);
        if (statements_result.tag == Result<Vector<StatementSyntax>*, ParserError*>::Error)
            return Result<FileSyntax*, ParserError*> { .tag = Result<FileSyntax*, ParserError*>::Error, .error = statements_result.error };
        auto statements = statements_result.ok;
        if (statements != nullptr) {
            if (!this->is_at_end()) {
                auto error_pos = this->lexer.previous_position;
                return Result<FileSyntax*, ParserError*> { .tag = Result<FileSyntax*, ParserError*>::Error, .error = new(alignof(ParserError), _ep) ParserError {
                    .file_name = file_name,
                    .position = lexer.position,
                }};
            };
        };

        auto ret = new(alignof(FileSyntax), _rp) FileSyntax {
            .file_name = file_name,
            .declarations = declarations,
            .statements = statements,
        };

        return Result<FileSyntax*, ParserError*> { .tag = Result<FileSyntax*, ParserError*>::Ok, .ok = ret };
    }

    Result<Vector<DeclarationSyntax>*, ParserError*> parse_declaration_list(Region& _pr, Page* _rp, Page* _ep) {
        auto _r = Region::create(_pr);
        {
            auto _r_1 = Region::create(_r);
            Array<DeclarationSyntax>* array = nullptr;
            while(true) {
                auto node_result = this->parse_declaration(_r_1, _rp, _ep);
                if (node_result.tag == Result<DeclarationSyntax*, ParserError*>::Error)
                    return Result<Vector<DeclarationSyntax>*, ParserError*> { .tag = Result<Vector<DeclarationSyntax>*, ParserError*>::Error, .error = node_result.error };
                auto node = node_result.ok;
                if (node != nullptr) {
                    if (array == nullptr)
                        array = Array<DeclarationSyntax>::create(_r_1.page);
                    array->add(*node);
                } else {
                    break;
                }
            }

            if (array == nullptr)
                return Result<Vector<DeclarationSyntax>*, ParserError*> { .tag = Result<Vector<DeclarationSyntax>*, ParserError*>::Ok, .ok = nullptr };
            
            return Result<Vector<DeclarationSyntax>*, ParserError*> { .tag = Result<Vector<DeclarationSyntax>*, ParserError*>::Ok, .ok = Vector<DeclarationSyntax>::from_array(_rp, *array) };
        }
    }

    Result<DeclarationSyntax*, ParserError*> parse_declaration(Region& _pr, Page* _rp, Page* _ep) {
        auto _r = Region::create(_pr);
        {
            auto _r_1 = Region::create(_r);
            auto node_result = this->parse_private(_r_1, _rp, _ep);
            if (node_result.tag == Result<PrivateSyntax*, ParserError*>::Error)
                return Result<DeclarationSyntax*, ParserError*> { .tag = Result<DeclarationSyntax*, ParserError*>::Error, .error = node_result.error };
            auto node = node_result.ok;
            if (node != nullptr) {
                return Result<DeclarationSyntax*, ParserError*> { .tag = Result<DeclarationSyntax*, ParserError*>::Ok, .ok = 
                    new (alignof(DeclarationSyntax), _rp) DeclarationSyntax {
                        .tag = DeclarationSyntax::Private,
                        .privateSyntax = *node,
                    }
                };
            }
        }
        {
            auto _r_1 = Region::create(_r);
            auto node_result = this->parse_definition(_r_1, _rp, _ep);
            if (node_result.tag == Result<DefinitionSyntax*, ParserError*>::Error)
                return Result<DeclarationSyntax*, ParserError*> { .tag = Result<DeclarationSyntax*, ParserError*>::Error, .error = node_result.error };
            auto node = node_result.ok;
            if (node != nullptr) {
                return Result<DeclarationSyntax*, ParserError*> { .tag = Result<DeclarationSyntax*, ParserError*>::Ok, .ok = 
                    new (alignof(DeclarationSyntax), _rp) DeclarationSyntax {
                        .tag = DeclarationSyntax::Definition,
                        .definitionSyntax = *node,
                    }
                };
            }
        }
        {
            auto _r_1 = Region::create(_r);
            auto node_result = this->parse_function(_r_1, _rp, _ep);
            if (node_result.tag == Result<FunctionSyntax*, ParserError*>::Error)
                return Result<DeclarationSyntax*, ParserError*> { .tag = Result<DeclarationSyntax*, ParserError*>::Error, .error = node_result.error };
            auto node = node_result.ok;
            if (node != nullptr) {
                return Result<DeclarationSyntax*, ParserError*> { .tag = Result<DeclarationSyntax*, ParserError*>::Ok, .ok = 
                    new (alignof(DeclarationSyntax), _rp) DeclarationSyntax {
                        .tag = DeclarationSyntax::Function,
                        .functionSyntax = *node,
                    }
                };
            }
        }
        {
            auto _r_1 = Region::create(_r);
            auto node_result = this->parse_procedure(_r_1, _rp, _ep);
            if (node_result.tag == Result<ProcedureSyntax*, ParserError*>::Error)
                return Result<DeclarationSyntax*, ParserError*> { .tag = Result<DeclarationSyntax*, ParserError*>::Error, .error = node_result.error };
            auto node = node_result.ok;
            if (node != nullptr) {
                return Result<DeclarationSyntax*, ParserError*> { .tag = Result<DeclarationSyntax*, ParserError*>::Ok, .ok = 
                    new (alignof(DeclarationSyntax), _rp) DeclarationSyntax {
                        .tag = DeclarationSyntax::Procedure,
                        .procedureSyntax = *node,
                    }
                };
            }
        }
        {
            auto _r_1 = Region::create(_r);
            auto node_result = this->parse_operator(_r_1, _rp, _ep);
            if (node_result.tag == Result<OperatorSyntax*, ParserError*>::Error)
                return Result<DeclarationSyntax*, ParserError*> { .tag = Result<DeclarationSyntax*, ParserError*>::Error, .error = node_result.error };
            auto node = node_result.ok;
            if (node != nullptr) {
                return Result<DeclarationSyntax*, ParserError*> { .tag = Result<DeclarationSyntax*, ParserError*>::Ok, .ok = 
                    new (alignof(DeclarationSyntax), _rp) DeclarationSyntax {
                        .tag = DeclarationSyntax::Operator,
                        .operatorSyntax = *node,
                    }
                };
            }
        }
        {
            auto _r_1 = Region::create(_r);
            auto node_result = this->parse_use(_r_1, _rp, _ep);
            if (node_result.tag == Result<UseSyntax*, ParserError*>::Error)
                return Result<DeclarationSyntax*, ParserError*> { .tag = Result<DeclarationSyntax*, ParserError*>::Error, .error = node_result.error };
            auto node = node_result.ok;
            if (node != nullptr) {
                return Result<DeclarationSyntax*, ParserError*> { .tag = Result<DeclarationSyntax*, ParserError*>::Ok, .ok = 
                    new (alignof(DeclarationSyntax), _rp) DeclarationSyntax {
                        .tag = DeclarationSyntax::Use,
                        .useSyntax = *node,
                    }
                };
            }
        }
        {
            auto _r_1 = Region::create(_r);
            auto node_result = this->parse_implement(_r_1, _rp, _ep);
            if (node_result.tag == Result<ImplementSyntax*, ParserError*>::Error)
                return Result<DeclarationSyntax*, ParserError*> { .tag = Result<DeclarationSyntax*, ParserError*>::Error, .error = node_result.error };
            auto node = node_result.ok;
            if (node != nullptr) {
                return Result<DeclarationSyntax*, ParserError*> { .tag = Result<DeclarationSyntax*, ParserError*>::Ok, .ok = 
                    new (alignof(DeclarationSyntax), _rp) DeclarationSyntax {
                        .tag = DeclarationSyntax::Implement,
                        .implementSyntax = *node,
                    }
                };
            }
        }
        {
            auto _r_1 = Region::create(_r);
            auto node_result = this->parse_trait(_r_1, _rp, _ep);
            if (node_result.tag == Result<TraitSyntax*, ParserError*>::Error)
                return Result<DeclarationSyntax*, ParserError*> { .tag = Result<DeclarationSyntax*, ParserError*>::Error, .error = node_result.error };
            auto node = node_result.ok;
            if (node != nullptr) {
                return Result<DeclarationSyntax*, ParserError*> { .tag = Result<DeclarationSyntax*, ParserError*>::Ok, .ok = 
                    new (alignof(DeclarationSyntax), _rp) DeclarationSyntax {
                        .tag = DeclarationSyntax::Trait,
                        .traitSyntax = *node,
                    }
                };
            }
        }
        {
            auto _r_1 = Region::create(_r);
            auto node_result = this->parse_macro(_r_1, _rp, _ep);
            if (node_result.tag == Result<MacroSyntax*, ParserError*>::Error)
                return Result<DeclarationSyntax*, ParserError*> { .tag = Result<DeclarationSyntax*, ParserError*>::Error, .error = node_result.error };
            auto node = node_result.ok;
            if (node != nullptr) {
                return Result<DeclarationSyntax*, ParserError*> { .tag = Result<DeclarationSyntax*, ParserError*>::Ok, .ok = 
                    new (alignof(DeclarationSyntax), _rp) DeclarationSyntax {
                        .tag = DeclarationSyntax::Macro,
                        .macroSyntax = *node,
                    }
                };
            }
        }
        {
            auto _r_1 = Region::create(_r);
            auto node_result = this->parse_module(_r_1, _rp, _ep);
            if (node_result.tag == Result<ModuleSyntax*, ParserError*>::Error)
                return Result<DeclarationSyntax*, ParserError*> { .tag = Result<DeclarationSyntax*, ParserError*>::Error, .error = node_result.error };
            auto node = node_result.ok;
            if (node != nullptr) {
                return Result<DeclarationSyntax*, ParserError*> { .tag = Result<DeclarationSyntax*, ParserError*>::Ok, .ok = 
                    new (alignof(DeclarationSyntax), _rp) DeclarationSyntax {
                        .tag = DeclarationSyntax::Module,
                        .moduleSyntax = *node,
                    }
                };
            }
        }
        return Result<DeclarationSyntax*, ParserError*> { .tag = Result<DeclarationSyntax*, ParserError*>::Ok, .ok = nullptr };
    }

    Result<PrivateSyntax*, ParserError*> parse_private(Region& _pr, Page* _rp, Page* _ep) {
        auto _r = Region::create(_pr);
        auto start = this->lexer.previous_position;

        auto success_private_1 = this->lexer.parse_keyword(_r.page, *String::from_c_string(_r.page, "private"));
        if (!success_private_1) {

                return Result<PrivateSyntax*, ParserError*> { .tag = Result<PrivateSyntax*, ParserError*>::Ok, .ok = nullptr };
        }

        auto export__result = this->parse_export(_r, _rp, _ep);
        if (export__result.tag == Result<ExportSyntax*, ParserError*>::Error)
            return Result<PrivateSyntax*, ParserError*> { .tag = Result<PrivateSyntax*, ParserError*>::Error, .error = export__result.error };
        auto export_ = export__result.ok;
        if (export_ == nullptr) {
            return Result<PrivateSyntax*, ParserError*> { .tag = Result<PrivateSyntax*, ParserError*>::Error, .error = new(alignof(ParserError), _ep) ParserError {
                .file_name = file_name,
                .position = lexer.position,
            }};
        }

        auto end = this->lexer.position;

        auto ret = new(alignof(PrivateSyntax), _rp) PrivateSyntax {
            .start = start,
            .end = end,
            .export_ = export_,
        };

        return Result<PrivateSyntax*, ParserError*> { .tag = Result<PrivateSyntax*, ParserError*>::Ok, .ok = ret };
    }

    Result<ExportSyntax*, ParserError*> parse_export(Region& _pr, Page* _rp, Page* _ep) {
        auto _r = Region::create(_pr);
        {
            auto _r_1 = Region::create(_r);
            auto node_result = this->parse_definition(_r_1, _rp, _ep);
            if (node_result.tag == Result<DefinitionSyntax*, ParserError*>::Error)
                return Result<ExportSyntax*, ParserError*> { .tag = Result<ExportSyntax*, ParserError*>::Error, .error = node_result.error };
            auto node = node_result.ok;
            if (node != nullptr) {
                return Result<ExportSyntax*, ParserError*> { .tag = Result<ExportSyntax*, ParserError*>::Ok, .ok = 
                    new (alignof(ExportSyntax), _rp) ExportSyntax {
                        .tag = ExportSyntax::Definition,
                        .definitionSyntax = *node,
                    }
                };
            }
        }
        {
            auto _r_1 = Region::create(_r);
            auto node_result = this->parse_function(_r_1, _rp, _ep);
            if (node_result.tag == Result<FunctionSyntax*, ParserError*>::Error)
                return Result<ExportSyntax*, ParserError*> { .tag = Result<ExportSyntax*, ParserError*>::Error, .error = node_result.error };
            auto node = node_result.ok;
            if (node != nullptr) {
                return Result<ExportSyntax*, ParserError*> { .tag = Result<ExportSyntax*, ParserError*>::Ok, .ok = 
                    new (alignof(ExportSyntax), _rp) ExportSyntax {
                        .tag = ExportSyntax::Function,
                        .functionSyntax = *node,
                    }
                };
            }
        }
        {
            auto _r_1 = Region::create(_r);
            auto node_result = this->parse_procedure(_r_1, _rp, _ep);
            if (node_result.tag == Result<ProcedureSyntax*, ParserError*>::Error)
                return Result<ExportSyntax*, ParserError*> { .tag = Result<ExportSyntax*, ParserError*>::Error, .error = node_result.error };
            auto node = node_result.ok;
            if (node != nullptr) {
                return Result<ExportSyntax*, ParserError*> { .tag = Result<ExportSyntax*, ParserError*>::Ok, .ok = 
                    new (alignof(ExportSyntax), _rp) ExportSyntax {
                        .tag = ExportSyntax::Procedure,
                        .procedureSyntax = *node,
                    }
                };
            }
        }
        {
            auto _r_1 = Region::create(_r);
            auto node_result = this->parse_operator(_r_1, _rp, _ep);
            if (node_result.tag == Result<OperatorSyntax*, ParserError*>::Error)
                return Result<ExportSyntax*, ParserError*> { .tag = Result<ExportSyntax*, ParserError*>::Error, .error = node_result.error };
            auto node = node_result.ok;
            if (node != nullptr) {
                return Result<ExportSyntax*, ParserError*> { .tag = Result<ExportSyntax*, ParserError*>::Ok, .ok = 
                    new (alignof(ExportSyntax), _rp) ExportSyntax {
                        .tag = ExportSyntax::Operator,
                        .operatorSyntax = *node,
                    }
                };
            }
        }
        {
            auto _r_1 = Region::create(_r);
            auto node_result = this->parse_implement(_r_1, _rp, _ep);
            if (node_result.tag == Result<ImplementSyntax*, ParserError*>::Error)
                return Result<ExportSyntax*, ParserError*> { .tag = Result<ExportSyntax*, ParserError*>::Error, .error = node_result.error };
            auto node = node_result.ok;
            if (node != nullptr) {
                return Result<ExportSyntax*, ParserError*> { .tag = Result<ExportSyntax*, ParserError*>::Ok, .ok = 
                    new (alignof(ExportSyntax), _rp) ExportSyntax {
                        .tag = ExportSyntax::Implement,
                        .implementSyntax = *node,
                    }
                };
            }
        }
        {
            auto _r_1 = Region::create(_r);
            auto node_result = this->parse_trait(_r_1, _rp, _ep);
            if (node_result.tag == Result<TraitSyntax*, ParserError*>::Error)
                return Result<ExportSyntax*, ParserError*> { .tag = Result<ExportSyntax*, ParserError*>::Error, .error = node_result.error };
            auto node = node_result.ok;
            if (node != nullptr) {
                return Result<ExportSyntax*, ParserError*> { .tag = Result<ExportSyntax*, ParserError*>::Ok, .ok = 
                    new (alignof(ExportSyntax), _rp) ExportSyntax {
                        .tag = ExportSyntax::Trait,
                        .traitSyntax = *node,
                    }
                };
            }
        }
        {
            auto _r_1 = Region::create(_r);
            auto node_result = this->parse_macro(_r_1, _rp, _ep);
            if (node_result.tag == Result<MacroSyntax*, ParserError*>::Error)
                return Result<ExportSyntax*, ParserError*> { .tag = Result<ExportSyntax*, ParserError*>::Error, .error = node_result.error };
            auto node = node_result.ok;
            if (node != nullptr) {
                return Result<ExportSyntax*, ParserError*> { .tag = Result<ExportSyntax*, ParserError*>::Ok, .ok = 
                    new (alignof(ExportSyntax), _rp) ExportSyntax {
                        .tag = ExportSyntax::Macro,
                        .macroSyntax = *node,
                    }
                };
            }
        }
        {
            auto _r_1 = Region::create(_r);
            auto node_result = this->parse_module(_r_1, _rp, _ep);
            if (node_result.tag == Result<ModuleSyntax*, ParserError*>::Error)
                return Result<ExportSyntax*, ParserError*> { .tag = Result<ExportSyntax*, ParserError*>::Error, .error = node_result.error };
            auto node = node_result.ok;
            if (node != nullptr) {
                return Result<ExportSyntax*, ParserError*> { .tag = Result<ExportSyntax*, ParserError*>::Ok, .ok = 
                    new (alignof(ExportSyntax), _rp) ExportSyntax {
                        .tag = ExportSyntax::Module,
                        .moduleSyntax = *node,
                    }
                };
            }
        }
        return Result<ExportSyntax*, ParserError*> { .tag = Result<ExportSyntax*, ParserError*>::Ok, .ok = nullptr };
    }

    Result<DefinitionSyntax*, ParserError*> parse_definition(Region& _pr, Page* _rp, Page* _ep) {
        auto _r = Region::create(_pr);
        auto start = this->lexer.previous_position;

        auto success_define_1 = this->lexer.parse_keyword(_r.page, *String::from_c_string(_r.page, "define"));
        if (!success_define_1) {

                return Result<DefinitionSyntax*, ParserError*> { .tag = Result<DefinitionSyntax*, ParserError*>::Ok, .ok = nullptr };
        }

        auto type_result = this->parse_type(_r, _rp, _ep);
        if (type_result.tag == Result<TypeSyntax*, ParserError*>::Error)
            return Result<DefinitionSyntax*, ParserError*> { .tag = Result<DefinitionSyntax*, ParserError*>::Error, .error = type_result.error };
        auto type = type_result.ok;
        if (type == nullptr) {
            return Result<DefinitionSyntax*, ParserError*> { .tag = Result<DefinitionSyntax*, ParserError*>::Error, .error = new(alignof(ParserError), _ep) ParserError {
                .file_name = file_name,
                .position = lexer.position,
            }};
        }

        auto success_colon_3 = this->lexer.parse_colon(_r.page);
        if (!success_colon_3) {

        }

        auto attributes_result = this->parse_attribute_list(_r, _rp, _ep);
        if (attributes_result.tag == Result<Vector<AttributeSyntax>*, ParserError*>::Error)
            return Result<DefinitionSyntax*, ParserError*> { .tag = Result<DefinitionSyntax*, ParserError*>::Error, .error = attributes_result.error };
        auto attributes = attributes_result.ok;

        auto concept__result = this->parse_concept(_r, _rp, _ep);
        if (concept__result.tag == Result<ConceptSyntax*, ParserError*>::Error)
            return Result<DefinitionSyntax*, ParserError*> { .tag = Result<DefinitionSyntax*, ParserError*>::Error, .error = concept__result.error };
        auto concept_ = concept__result.ok;
        if (concept_ == nullptr) {
            return Result<DefinitionSyntax*, ParserError*> { .tag = Result<DefinitionSyntax*, ParserError*>::Error, .error = new(alignof(ParserError), _ep) ParserError {
                .file_name = file_name,
                .position = lexer.position,
            }};
        }

        auto end = this->lexer.position;

        auto ret = new(alignof(DefinitionSyntax), _rp) DefinitionSyntax {
            .start = start,
            .end = end,
            .type = type,
            .attributes = attributes,
            .concept_ = concept_,
        };

        return Result<DefinitionSyntax*, ParserError*> { .tag = Result<DefinitionSyntax*, ParserError*>::Ok, .ok = ret };
    }

    Result<NameSyntax*, ParserError*> parse_name(Region& _pr, Page* _rp, Page* _ep) {
        auto _r = Region::create(_pr);
        auto start = this->lexer.previous_position;

        auto name = this->lexer.parse_identifier(_r.page, this->keywords);
        if (name != nullptr) {
            if (!this->is_identifier(*name)) {
                return Result<NameSyntax*, ParserError*> { .tag = Result<NameSyntax*, ParserError*>::Ok, .ok = nullptr };

            }
        }
        else {
                return Result<NameSyntax*, ParserError*> { .tag = Result<NameSyntax*, ParserError*>::Ok, .ok = nullptr };

        }

        auto extensions_result = this->parse_extension_list(_r, _rp, _ep);
        if (extensions_result.tag == Result<Vector<ExtensionSyntax>*, ParserError*>::Error)
            return Result<NameSyntax*, ParserError*> { .tag = Result<NameSyntax*, ParserError*>::Error, .error = extensions_result.error };
        auto extensions = extensions_result.ok;

        auto end = this->lexer.position;

        auto ret = new(alignof(NameSyntax), _rp) NameSyntax {
            .start = start,
            .end = end,
            .name = name,
            .extensions = extensions,
        };

        return Result<NameSyntax*, ParserError*> { .tag = Result<NameSyntax*, ParserError*>::Ok, .ok = ret };
    }

    Result<Vector<ExtensionSyntax>*, ParserError*> parse_extension_list(Region& _pr, Page* _rp, Page* _ep) {
        auto _r = Region::create(_pr);
        {
            auto _r_1 = Region::create(_r);
            Array<ExtensionSyntax>* array = nullptr;
            while(true) {
                auto node_result = this->parse_extension(_r_1, _rp, _ep);
                if (node_result.tag == Result<ExtensionSyntax*, ParserError*>::Error)
                    return Result<Vector<ExtensionSyntax>*, ParserError*> { .tag = Result<Vector<ExtensionSyntax>*, ParserError*>::Error, .error = node_result.error };
                auto node = node_result.ok;
                if (node != nullptr) {
                    if (array == nullptr)
                        array = Array<ExtensionSyntax>::create(_r_1.page);
                    array->add(*node);
                } else {
                    break;
                }
            }

            if (array == nullptr)
                return Result<Vector<ExtensionSyntax>*, ParserError*> { .tag = Result<Vector<ExtensionSyntax>*, ParserError*>::Ok, .ok = nullptr };
            
            return Result<Vector<ExtensionSyntax>*, ParserError*> { .tag = Result<Vector<ExtensionSyntax>*, ParserError*>::Ok, .ok = Vector<ExtensionSyntax>::from_array(_rp, *array) };
        }
    }

    Result<ExtensionSyntax*, ParserError*> parse_extension(Region& _pr, Page* _rp, Page* _ep) {
        auto _r = Region::create(_pr);
        auto start = this->lexer.previous_position;

        auto success_dot_1 = this->lexer.parse_punctuation(_r.page, *String::from_c_string(_r.page, "."));
        if (!success_dot_1) {

                return Result<ExtensionSyntax*, ParserError*> { .tag = Result<ExtensionSyntax*, ParserError*>::Ok, .ok = nullptr };
        }

        auto name = this->lexer.parse_identifier(_r.page, this->keywords);
        if (name != nullptr) {
            if (!this->is_identifier(*name)) {
            return Result<ExtensionSyntax*, ParserError*> { .tag = Result<ExtensionSyntax*, ParserError*>::Error, .error = new(alignof(ParserError), _ep) ParserError {
                .file_name = file_name,
                .position = lexer.position,
            }};
            }
        }
        else {
            return Result<ExtensionSyntax*, ParserError*> { .tag = Result<ExtensionSyntax*, ParserError*>::Error, .error = new(alignof(ParserError), _ep) ParserError {
                .file_name = file_name,
                .position = lexer.position,
            }};
        }

        auto end = this->lexer.position;

        auto ret = new(alignof(ExtensionSyntax), _rp) ExtensionSyntax {
            .start = start,
            .end = end,
            .name = name,
        };

        return Result<ExtensionSyntax*, ParserError*> { .tag = Result<ExtensionSyntax*, ParserError*>::Ok, .ok = ret };
    }

    Result<Vector<AttributeSyntax>*, ParserError*> parse_attribute_list(Region& _pr, Page* _rp, Page* _ep) {
        auto _r = Region::create(_pr);
        {
            auto _r_1 = Region::create(_r);
            Array<AttributeSyntax>* array = nullptr;
            while(true) {
                auto node_result = this->parse_attribute(_r_1, _rp, _ep);
                if (node_result.tag == Result<AttributeSyntax*, ParserError*>::Error)
                    return Result<Vector<AttributeSyntax>*, ParserError*> { .tag = Result<Vector<AttributeSyntax>*, ParserError*>::Error, .error = node_result.error };
                auto node = node_result.ok;
                if (node != nullptr) {
                    if (array == nullptr)
                        array = Array<AttributeSyntax>::create(_r_1.page);
                    array->add(*node);
                } else {
                    break;
                }
            }

            if (array == nullptr)
                return Result<Vector<AttributeSyntax>*, ParserError*> { .tag = Result<Vector<AttributeSyntax>*, ParserError*>::Ok, .ok = nullptr };
            
            return Result<Vector<AttributeSyntax>*, ParserError*> { .tag = Result<Vector<AttributeSyntax>*, ParserError*>::Ok, .ok = Vector<AttributeSyntax>::from_array(_rp, *array) };
        }
    }

    Result<AttributeSyntax*, ParserError*> parse_attribute(Region& _pr, Page* _rp, Page* _ep) {
        auto _r = Region::create(_pr);
        auto start = this->lexer.previous_position;

        auto attribute = this->lexer.parse_attribute(_r.page);
        if (attribute == nullptr) {

                return Result<AttributeSyntax*, ParserError*> { .tag = Result<AttributeSyntax*, ParserError*>::Ok, .ok = nullptr };
        }

        auto value_result = this->parse_model(_r, _rp, _ep);
        if (value_result.tag == Result<ModelSyntax*, ParserError*>::Error)
            return Result<AttributeSyntax*, ParserError*> { .tag = Result<AttributeSyntax*, ParserError*>::Error, .error = value_result.error };
        auto value = value_result.ok;
        if (value == nullptr) {
            return Result<AttributeSyntax*, ParserError*> { .tag = Result<AttributeSyntax*, ParserError*>::Error, .error = new(alignof(ParserError), _ep) ParserError {
                .file_name = file_name,
                .position = lexer.position,
            }};
        }

        auto end = this->lexer.position;

        auto ret = new(alignof(AttributeSyntax), _rp) AttributeSyntax {
            .start = start,
            .end = end,
            .attribute = attribute,
            .value = value,
        };

        return Result<AttributeSyntax*, ParserError*> { .tag = Result<AttributeSyntax*, ParserError*>::Ok, .ok = ret };
    }

    Result<ConceptSyntax*, ParserError*> parse_concept(Region& _pr, Page* _rp, Page* _ep) {
        auto _r = Region::create(_pr);
        {
            auto _r_1 = Region::create(_r);
            auto node_result = this->parse_class(_r_1, _rp, _ep);
            if (node_result.tag == Result<ClassSyntax*, ParserError*>::Error)
                return Result<ConceptSyntax*, ParserError*> { .tag = Result<ConceptSyntax*, ParserError*>::Error, .error = node_result.error };
            auto node = node_result.ok;
            if (node != nullptr) {
                return Result<ConceptSyntax*, ParserError*> { .tag = Result<ConceptSyntax*, ParserError*>::Ok, .ok = 
                    new (alignof(ConceptSyntax), _rp) ConceptSyntax {
                        .tag = ConceptSyntax::Class,
                        .classSyntax = *node,
                    }
                };
            }
        }
        {
            auto _r_1 = Region::create(_r);
            auto node_result = this->parse_namespace(_r_1, _rp, _ep);
            if (node_result.tag == Result<NamespaceSyntax*, ParserError*>::Error)
                return Result<ConceptSyntax*, ParserError*> { .tag = Result<ConceptSyntax*, ParserError*>::Error, .error = node_result.error };
            auto node = node_result.ok;
            if (node != nullptr) {
                return Result<ConceptSyntax*, ParserError*> { .tag = Result<ConceptSyntax*, ParserError*>::Ok, .ok = 
                    new (alignof(ConceptSyntax), _rp) ConceptSyntax {
                        .tag = ConceptSyntax::Namespace,
                        .namespaceSyntax = *node,
                    }
                };
            }
        }
        {
            auto _r_1 = Region::create(_r);
            auto node_result = this->parse_union(_r_1, _rp, _ep);
            if (node_result.tag == Result<UnionSyntax*, ParserError*>::Error)
                return Result<ConceptSyntax*, ParserError*> { .tag = Result<ConceptSyntax*, ParserError*>::Error, .error = node_result.error };
            auto node = node_result.ok;
            if (node != nullptr) {
                return Result<ConceptSyntax*, ParserError*> { .tag = Result<ConceptSyntax*, ParserError*>::Ok, .ok = 
                    new (alignof(ConceptSyntax), _rp) ConceptSyntax {
                        .tag = ConceptSyntax::Union,
                        .unionSyntax = *node,
                    }
                };
            }
        }
        {
            auto _r_1 = Region::create(_r);
            auto node_result = this->parse_constant(_r_1, _rp, _ep);
            if (node_result.tag == Result<ConstantSyntax*, ParserError*>::Error)
                return Result<ConceptSyntax*, ParserError*> { .tag = Result<ConceptSyntax*, ParserError*>::Error, .error = node_result.error };
            auto node = node_result.ok;
            if (node != nullptr) {
                return Result<ConceptSyntax*, ParserError*> { .tag = Result<ConceptSyntax*, ParserError*>::Ok, .ok = 
                    new (alignof(ConceptSyntax), _rp) ConceptSyntax {
                        .tag = ConceptSyntax::Constant,
                        .constantSyntax = *node,
                    }
                };
            }
        }
        {
            auto _r_1 = Region::create(_r);
            auto node_result = this->parse_delegate(_r_1, _rp, _ep);
            if (node_result.tag == Result<DelegateSyntax*, ParserError*>::Error)
                return Result<ConceptSyntax*, ParserError*> { .tag = Result<ConceptSyntax*, ParserError*>::Error, .error = node_result.error };
            auto node = node_result.ok;
            if (node != nullptr) {
                return Result<ConceptSyntax*, ParserError*> { .tag = Result<ConceptSyntax*, ParserError*>::Ok, .ok = 
                    new (alignof(ConceptSyntax), _rp) ConceptSyntax {
                        .tag = ConceptSyntax::Delegate,
                        .delegateSyntax = *node,
                    }
                };
            }
        }
        {
            auto _r_1 = Region::create(_r);
            auto node_result = this->parse_intrinsic(_r_1, _rp, _ep);
            if (node_result.tag == Result<IntrinsicSyntax*, ParserError*>::Error)
                return Result<ConceptSyntax*, ParserError*> { .tag = Result<ConceptSyntax*, ParserError*>::Error, .error = node_result.error };
            auto node = node_result.ok;
            if (node != nullptr) {
                return Result<ConceptSyntax*, ParserError*> { .tag = Result<ConceptSyntax*, ParserError*>::Ok, .ok = 
                    new (alignof(ConceptSyntax), _rp) ConceptSyntax {
                        .tag = ConceptSyntax::Intrinsic,
                        .intrinsicSyntax = *node,
                    }
                };
            }
        }
        return Result<ConceptSyntax*, ParserError*> { .tag = Result<ConceptSyntax*, ParserError*>::Ok, .ok = nullptr };
    }

    Result<ClassSyntax*, ParserError*> parse_class(Region& _pr, Page* _rp, Page* _ep) {
        auto _r = Region::create(_pr);
        auto start = this->lexer.previous_position;

        auto structure_result = this->parse_structure(_r, _rp, _ep);
        if (structure_result.tag == Result<StructureSyntax*, ParserError*>::Error)
            return Result<ClassSyntax*, ParserError*> { .tag = Result<ClassSyntax*, ParserError*>::Error, .error = structure_result.error };
        auto structure = structure_result.ok;
        if (structure == nullptr) {
            return Result<ClassSyntax*, ParserError*> { .tag = Result<ClassSyntax*, ParserError*>::Ok, .ok = nullptr };
        }

        auto success_colon_2 = this->lexer.parse_colon(_r.page);
        if (!success_colon_2) {

        }

        auto body_result = this->parse_body(_r, _rp, _ep);
        if (body_result.tag == Result<BodySyntax*, ParserError*>::Error)
            return Result<ClassSyntax*, ParserError*> { .tag = Result<ClassSyntax*, ParserError*>::Error, .error = body_result.error };
        auto body = body_result.ok;

        auto end = this->lexer.position;

        auto ret = new(alignof(ClassSyntax), _rp) ClassSyntax {
            .start = start,
            .end = end,
            .structure = structure,
            .body = body,
        };

        return Result<ClassSyntax*, ParserError*> { .tag = Result<ClassSyntax*, ParserError*>::Ok, .ok = ret };
    }

    Result<NamespaceSyntax*, ParserError*> parse_namespace(Region& _pr, Page* _rp, Page* _ep) {
        auto _r = Region::create(_pr);
        auto start = this->lexer.previous_position;

        auto body_result = this->parse_body(_r, _rp, _ep);
        if (body_result.tag == Result<BodySyntax*, ParserError*>::Error)
            return Result<NamespaceSyntax*, ParserError*> { .tag = Result<NamespaceSyntax*, ParserError*>::Error, .error = body_result.error };
        auto body = body_result.ok;
        if (body == nullptr) {
            return Result<NamespaceSyntax*, ParserError*> { .tag = Result<NamespaceSyntax*, ParserError*>::Ok, .ok = nullptr };
        }

        auto end = this->lexer.position;

        auto ret = new(alignof(NamespaceSyntax), _rp) NamespaceSyntax {
            .start = start,
            .end = end,
            .body = body,
        };

        return Result<NamespaceSyntax*, ParserError*> { .tag = Result<NamespaceSyntax*, ParserError*>::Ok, .ok = ret };
    }

    Result<UnionSyntax*, ParserError*> parse_union(Region& _pr, Page* _rp, Page* _ep) {
        auto _r = Region::create(_pr);
        auto start = this->lexer.previous_position;

        auto success_union_1 = this->lexer.parse_keyword(_r.page, *String::from_c_string(_r.page, "union"));
        if (!success_union_1) {

                return Result<UnionSyntax*, ParserError*> { .tag = Result<UnionSyntax*, ParserError*>::Ok, .ok = nullptr };
        }

        auto success_colon_2 = this->lexer.parse_colon(_r.page);
        if (!success_colon_2) {

        }

        auto success_left_paren_3 = this->lexer.parse_punctuation(_r.page, *String::from_c_string(_r.page, "("));
        if (!success_left_paren_3) {

            return Result<UnionSyntax*, ParserError*> { .tag = Result<UnionSyntax*, ParserError*>::Error, .error = new(alignof(ParserError), _ep) ParserError {
                .file_name = file_name,
                .position = lexer.position,
            }};        }

        auto tags_result = this->parse_tag_list(_r, _rp, _ep);
        if (tags_result.tag == Result<Vector<TagSyntax>*, ParserError*>::Error)
            return Result<UnionSyntax*, ParserError*> { .tag = Result<UnionSyntax*, ParserError*>::Error, .error = tags_result.error };
        auto tags = tags_result.ok;
        if (tags == nullptr) {
            return Result<UnionSyntax*, ParserError*> { .tag = Result<UnionSyntax*, ParserError*>::Error, .error = new(alignof(ParserError), _ep) ParserError {
                .file_name = file_name,
                .position = lexer.position,
            }};
        }

        auto success_right_paren_5 = this->lexer.parse_punctuation(_r.page, *String::from_c_string(_r.page, ")"));
        if (!success_right_paren_5) {

            return Result<UnionSyntax*, ParserError*> { .tag = Result<UnionSyntax*, ParserError*>::Error, .error = new(alignof(ParserError), _ep) ParserError {
                .file_name = file_name,
                .position = lexer.position,
            }};        }

        auto success_colon_6 = this->lexer.parse_colon(_r.page);
        if (!success_colon_6) {

        }

        auto body_result = this->parse_body(_r, _rp, _ep);
        if (body_result.tag == Result<BodySyntax*, ParserError*>::Error)
            return Result<UnionSyntax*, ParserError*> { .tag = Result<UnionSyntax*, ParserError*>::Error, .error = body_result.error };
        auto body = body_result.ok;
        if (body == nullptr) {
            return Result<UnionSyntax*, ParserError*> { .tag = Result<UnionSyntax*, ParserError*>::Error, .error = new(alignof(ParserError), _ep) ParserError {
                .file_name = file_name,
                .position = lexer.position,
            }};
        }

        auto end = this->lexer.position;

        auto ret = new(alignof(UnionSyntax), _rp) UnionSyntax {
            .start = start,
            .end = end,
            .tags = tags,
            .body = body,
        };

        return Result<UnionSyntax*, ParserError*> { .tag = Result<UnionSyntax*, ParserError*>::Ok, .ok = ret };
    }

    Result<Vector<TagSyntax>*, ParserError*> parse_tag_list(Region& _pr, Page* _rp, Page* _ep) {
        auto _r = Region::create(_pr);
        {
            auto _r_1 = Region::create(_r);
            Array<TagSyntax>* array = nullptr;
            while(true) {
                auto node_result = this->parse_tag(_r_1, _rp, _ep);
                if (node_result.tag == Result<TagSyntax*, ParserError*>::Error)
                    return Result<Vector<TagSyntax>*, ParserError*> { .tag = Result<Vector<TagSyntax>*, ParserError*>::Error, .error = node_result.error };
                auto node = node_result.ok;
                if (node != nullptr) {
                    if (array == nullptr)
                        array = Array<TagSyntax>::create(_r_1.page);
                    array->add(*node);
                } else {
                    break;
                }
            }

            if (array == nullptr)
                return Result<Vector<TagSyntax>*, ParserError*> { .tag = Result<Vector<TagSyntax>*, ParserError*>::Ok, .ok = nullptr };
            
            return Result<Vector<TagSyntax>*, ParserError*> { .tag = Result<Vector<TagSyntax>*, ParserError*>::Ok, .ok = Vector<TagSyntax>::from_array(_rp, *array) };
        }
    }

    Result<TagSyntax*, ParserError*> parse_tag(Region& _pr, Page* _rp, Page* _ep) {
        auto _r = Region::create(_pr);
        auto start = this->lexer.previous_position;

        auto name = this->lexer.parse_identifier(_r.page, this->keywords);
        if (name != nullptr) {
            if (!this->is_identifier(*name)) {
                return Result<TagSyntax*, ParserError*> { .tag = Result<TagSyntax*, ParserError*>::Ok, .ok = nullptr };

            }
        }
        else {
                return Result<TagSyntax*, ParserError*> { .tag = Result<TagSyntax*, ParserError*>::Ok, .ok = nullptr };

        }

        auto attributes_result = this->parse_attribute_list(_r, _rp, _ep);
        if (attributes_result.tag == Result<Vector<AttributeSyntax>*, ParserError*>::Error)
            return Result<TagSyntax*, ParserError*> { .tag = Result<TagSyntax*, ParserError*>::Error, .error = attributes_result.error };
        auto attributes = attributes_result.ok;

        auto item_result = this->parse_item(_r, _rp, _ep);
        if (item_result.tag == Result<ItemSyntax*, ParserError*>::Error)
            return Result<TagSyntax*, ParserError*> { .tag = Result<TagSyntax*, ParserError*>::Error, .error = item_result.error };
        auto item = item_result.ok;

        auto end = this->lexer.position;

        auto ret = new(alignof(TagSyntax), _rp) TagSyntax {
            .start = start,
            .end = end,
            .name = name,
            .attributes = attributes,
            .item = item,
        };

        return Result<TagSyntax*, ParserError*> { .tag = Result<TagSyntax*, ParserError*>::Ok, .ok = ret };
    }

    Result<ItemSyntax*, ParserError*> parse_item(Region& _pr, Page* _rp, Page* _ep) {
        auto _r = Region::create(_pr);
        {
            auto _r_1 = Region::create(_r);
            auto node_result = this->parse_variant(_r_1, _rp, _ep);
            if (node_result.tag == Result<VariantSyntax*, ParserError*>::Error)
                return Result<ItemSyntax*, ParserError*> { .tag = Result<ItemSyntax*, ParserError*>::Error, .error = node_result.error };
            auto node = node_result.ok;
            if (node != nullptr) {
                return Result<ItemSyntax*, ParserError*> { .tag = Result<ItemSyntax*, ParserError*>::Ok, .ok = 
                    new (alignof(ItemSyntax), _rp) ItemSyntax {
                        .tag = ItemSyntax::Variant,
                        .variantSyntax = *node,
                    }
                };
            }
        }
        {
            auto _r_1 = Region::create(_r);
            auto node_result = this->parse_enum(_r_1, _rp, _ep);
            if (node_result.tag == Result<EnumSyntax*, ParserError*>::Error)
                return Result<ItemSyntax*, ParserError*> { .tag = Result<ItemSyntax*, ParserError*>::Error, .error = node_result.error };
            auto node = node_result.ok;
            if (node != nullptr) {
                return Result<ItemSyntax*, ParserError*> { .tag = Result<ItemSyntax*, ParserError*>::Ok, .ok = 
                    new (alignof(ItemSyntax), _rp) ItemSyntax {
                        .tag = ItemSyntax::Enum,
                        .enumSyntax = *node,
                    }
                };
            }
        }
        return Result<ItemSyntax*, ParserError*> { .tag = Result<ItemSyntax*, ParserError*>::Ok, .ok = nullptr };
    }

    Result<VariantSyntax*, ParserError*> parse_variant(Region& _pr, Page* _rp, Page* _ep) {
        auto _r = Region::create(_pr);
        auto start = this->lexer.previous_position;

        auto success_colon_1 = this->lexer.parse_colon(_r.page);
        if (!success_colon_1) {

        }

        auto structure_result = this->parse_structure(_r, _rp, _ep);
        if (structure_result.tag == Result<StructureSyntax*, ParserError*>::Error)
            return Result<VariantSyntax*, ParserError*> { .tag = Result<VariantSyntax*, ParserError*>::Error, .error = structure_result.error };
        auto structure = structure_result.ok;
        if (structure == nullptr) {
            return Result<VariantSyntax*, ParserError*> { .tag = Result<VariantSyntax*, ParserError*>::Error, .error = new(alignof(ParserError), _ep) ParserError {
                .file_name = file_name,
                .position = lexer.position,
            }};
        }

        auto success_colon_3 = this->lexer.parse_colon(_r.page);
        if (!success_colon_3) {

        }

        auto body_result = this->parse_body(_r, _rp, _ep);
        if (body_result.tag == Result<BodySyntax*, ParserError*>::Error)
            return Result<VariantSyntax*, ParserError*> { .tag = Result<VariantSyntax*, ParserError*>::Error, .error = body_result.error };
        auto body = body_result.ok;

        auto success_colon_5 = this->lexer.parse_colon(_r.page);
        if (!success_colon_5) {

        }

        auto end = this->lexer.position;

        auto ret = new(alignof(VariantSyntax), _rp) VariantSyntax {
            .start = start,
            .end = end,
            .structure = structure,
            .body = body,
        };

        return Result<VariantSyntax*, ParserError*> { .tag = Result<VariantSyntax*, ParserError*>::Ok, .ok = ret };
    }

    Result<EnumSyntax*, ParserError*> parse_enum(Region& _pr, Page* _rp, Page* _ep) {
        auto _r = Region::create(_pr);
        auto start = this->lexer.previous_position;

        auto literal = this->lexer.parse_literal(_r.page);
        if (literal == nullptr) {

                return Result<EnumSyntax*, ParserError*> { .tag = Result<EnumSyntax*, ParserError*>::Ok, .ok = nullptr };
        }

        auto end = this->lexer.position;

        auto ret = new(alignof(EnumSyntax), _rp) EnumSyntax {
            .start = start,
            .end = end,
            .literal = *literal,
        };

        return Result<EnumSyntax*, ParserError*> { .tag = Result<EnumSyntax*, ParserError*>::Ok, .ok = ret };
    }

    Result<ConstantSyntax*, ParserError*> parse_constant(Region& _pr, Page* _rp, Page* _ep) {
        auto _r = Region::create(_pr);
        auto start = this->lexer.previous_position;

        auto operation_result = this->parse_operation(_r, _rp, _ep);
        if (operation_result.tag == Result<OperationSyntax*, ParserError*>::Error)
            return Result<ConstantSyntax*, ParserError*> { .tag = Result<ConstantSyntax*, ParserError*>::Error, .error = operation_result.error };
        auto operation = operation_result.ok;
        if (operation == nullptr) {
            return Result<ConstantSyntax*, ParserError*> { .tag = Result<ConstantSyntax*, ParserError*>::Ok, .ok = nullptr };
        }

        auto success_colon_2 = this->lexer.parse_colon(_r.page);
        if (!success_colon_2) {

        }

        auto end = this->lexer.position;

        auto ret = new(alignof(ConstantSyntax), _rp) ConstantSyntax {
            .start = start,
            .end = end,
            .operation = operation,
        };

        return Result<ConstantSyntax*, ParserError*> { .tag = Result<ConstantSyntax*, ParserError*>::Ok, .ok = ret };
    }

    Result<DelegateSyntax*, ParserError*> parse_delegate(Region& _pr, Page* _rp, Page* _ep) {
        auto _r = Region::create(_pr);
        auto start = this->lexer.previous_position;

        auto success_delegate_1 = this->lexer.parse_keyword(_r.page, *String::from_c_string(_r.page, "delegate"));
        if (!success_delegate_1) {

                return Result<DelegateSyntax*, ParserError*> { .tag = Result<DelegateSyntax*, ParserError*>::Ok, .ok = nullptr };
        }

        auto parameters_result = this->parse_parameterset(_r, _rp, _ep);
        if (parameters_result.tag == Result<ParameterSetSyntax*, ParserError*>::Error)
            return Result<DelegateSyntax*, ParserError*> { .tag = Result<DelegateSyntax*, ParserError*>::Error, .error = parameters_result.error };
        auto parameters = parameters_result.ok;

        auto attributes_result = this->parse_attribute_list(_r, _rp, _ep);
        if (attributes_result.tag == Result<Vector<AttributeSyntax>*, ParserError*>::Error)
            return Result<DelegateSyntax*, ParserError*> { .tag = Result<DelegateSyntax*, ParserError*>::Error, .error = attributes_result.error };
        auto attributes = attributes_result.ok;

        auto result_result = this->parse_returns(_r, _rp, _ep);
        if (result_result.tag == Result<ReturnsSyntax*, ParserError*>::Error)
            return Result<DelegateSyntax*, ParserError*> { .tag = Result<DelegateSyntax*, ParserError*>::Error, .error = result_result.error };
        auto result = result_result.ok;

        auto error_result = this->parse_throws(_r, _rp, _ep);
        if (error_result.tag == Result<ThrowsSyntax*, ParserError*>::Error)
            return Result<DelegateSyntax*, ParserError*> { .tag = Result<DelegateSyntax*, ParserError*>::Error, .error = error_result.error };
        auto error = error_result.ok;

        auto end = this->lexer.position;

        auto ret = new(alignof(DelegateSyntax), _rp) DelegateSyntax {
            .start = start,
            .end = end,
            .parameters = parameters,
            .attributes = attributes,
            .result = result,
            .error = error,
        };

        return Result<DelegateSyntax*, ParserError*> { .tag = Result<DelegateSyntax*, ParserError*>::Ok, .ok = ret };
    }

    Result<StructureSyntax*, ParserError*> parse_structure(Region& _pr, Page* _rp, Page* _ep) {
        auto _r = Region::create(_pr);
        auto start = this->lexer.previous_position;

        auto success_left_paren_1 = this->lexer.parse_punctuation(_r.page, *String::from_c_string(_r.page, "("));
        if (!success_left_paren_1) {

                return Result<StructureSyntax*, ParserError*> { .tag = Result<StructureSyntax*, ParserError*>::Ok, .ok = nullptr };
        }

        auto members_result = this->parse_member_list(_r, _rp, _ep);
        if (members_result.tag == Result<Vector<MemberSyntax>*, ParserError*>::Error)
            return Result<StructureSyntax*, ParserError*> { .tag = Result<StructureSyntax*, ParserError*>::Error, .error = members_result.error };
        auto members = members_result.ok;

        auto success_right_paren_3 = this->lexer.parse_punctuation(_r.page, *String::from_c_string(_r.page, ")"));
        if (!success_right_paren_3) {

            return Result<StructureSyntax*, ParserError*> { .tag = Result<StructureSyntax*, ParserError*>::Error, .error = new(alignof(ParserError), _ep) ParserError {
                .file_name = file_name,
                .position = lexer.position,
            }};        }

        auto end = this->lexer.position;

        auto ret = new(alignof(StructureSyntax), _rp) StructureSyntax {
            .start = start,
            .end = end,
            .members = members,
        };

        return Result<StructureSyntax*, ParserError*> { .tag = Result<StructureSyntax*, ParserError*>::Ok, .ok = ret };
    }

    Result<BodySyntax*, ParserError*> parse_body(Region& _pr, Page* _rp, Page* _ep) {
        auto _r = Region::create(_pr);
        auto start = this->lexer.previous_position;

        auto success_left_curly_1 = this->lexer.parse_punctuation(_r.page, *String::from_c_string(_r.page, "{"));
        if (!success_left_curly_1) {

                return Result<BodySyntax*, ParserError*> { .tag = Result<BodySyntax*, ParserError*>::Ok, .ok = nullptr };
        }

        auto declarations_result = this->parse_declaration_list(_r, _rp, _ep);
        if (declarations_result.tag == Result<Vector<DeclarationSyntax>*, ParserError*>::Error)
            return Result<BodySyntax*, ParserError*> { .tag = Result<BodySyntax*, ParserError*>::Error, .error = declarations_result.error };
        auto declarations = declarations_result.ok;

        auto success_right_curly_3 = this->lexer.parse_punctuation(_r.page, *String::from_c_string(_r.page, "}"));
        if (!success_right_curly_3) {

            return Result<BodySyntax*, ParserError*> { .tag = Result<BodySyntax*, ParserError*>::Error, .error = new(alignof(ParserError), _ep) ParserError {
                .file_name = file_name,
                .position = lexer.position,
            }};        }

        auto success_colon_4 = this->lexer.parse_colon(_r.page);
        if (!success_colon_4) {

        }

        auto end = this->lexer.position;

        auto ret = new(alignof(BodySyntax), _rp) BodySyntax {
            .start = start,
            .end = end,
            .declarations = declarations,
        };

        return Result<BodySyntax*, ParserError*> { .tag = Result<BodySyntax*, ParserError*>::Ok, .ok = ret };
    }

    Result<Vector<MemberSyntax>*, ParserError*> parse_member_list(Region& _pr, Page* _rp, Page* _ep) {
        auto _r = Region::create(_pr);
        {
            auto _r_1 = Region::create(_r);
            Array<MemberSyntax>* array = nullptr;
            while(true) {
                auto node_result = this->parse_member(_r_1, _rp, _ep);
                if (node_result.tag == Result<MemberSyntax*, ParserError*>::Error)
                    return Result<Vector<MemberSyntax>*, ParserError*> { .tag = Result<Vector<MemberSyntax>*, ParserError*>::Error, .error = node_result.error };
                auto node = node_result.ok;
                if (node != nullptr) {
                    if (array == nullptr)
                        array = Array<MemberSyntax>::create(_r_1.page);
                    array->add(*node);
                } else {
                    break;
                }
            }

            if (array == nullptr)
                return Result<Vector<MemberSyntax>*, ParserError*> { .tag = Result<Vector<MemberSyntax>*, ParserError*>::Ok, .ok = nullptr };
            
            return Result<Vector<MemberSyntax>*, ParserError*> { .tag = Result<Vector<MemberSyntax>*, ParserError*>::Ok, .ok = Vector<MemberSyntax>::from_array(_rp, *array) };
        }
    }

    Result<MemberSyntax*, ParserError*> parse_member(Region& _pr, Page* _rp, Page* _ep) {
        auto _r = Region::create(_pr);
        {
            auto _r_1 = Region::create(_r);
            auto node_result = this->parse_field(_r_1, _rp, _ep);
            if (node_result.tag == Result<FieldSyntax*, ParserError*>::Error)
                return Result<MemberSyntax*, ParserError*> { .tag = Result<MemberSyntax*, ParserError*>::Error, .error = node_result.error };
            auto node = node_result.ok;
            if (node != nullptr) {
                return Result<MemberSyntax*, ParserError*> { .tag = Result<MemberSyntax*, ParserError*>::Ok, .ok = 
                    new (alignof(MemberSyntax), _rp) MemberSyntax {
                        .tag = MemberSyntax::Field,
                        .fieldSyntax = *node,
                    }
                };
            }
        }
        {
            auto _r_1 = Region::create(_r);
            auto node_result = this->parse_property(_r_1, _rp, _ep);
            if (node_result.tag == Result<PropertySyntax*, ParserError*>::Error)
                return Result<MemberSyntax*, ParserError*> { .tag = Result<MemberSyntax*, ParserError*>::Error, .error = node_result.error };
            auto node = node_result.ok;
            if (node != nullptr) {
                return Result<MemberSyntax*, ParserError*> { .tag = Result<MemberSyntax*, ParserError*>::Ok, .ok = 
                    new (alignof(MemberSyntax), _rp) MemberSyntax {
                        .tag = MemberSyntax::Property,
                        .propertySyntax = *node,
                    }
                };
            }
        }
        return Result<MemberSyntax*, ParserError*> { .tag = Result<MemberSyntax*, ParserError*>::Ok, .ok = nullptr };
    }

    Result<FieldSyntax*, ParserError*> parse_field(Region& _pr, Page* _rp, Page* _ep) {
        auto _r = Region::create(_pr);
        auto start = this->lexer.previous_position;

        auto success_private_1 = this->lexer.parse_keyword(_r.page, *String::from_c_string(_r.page, "private"));
        if (!success_private_1) {

                return Result<FieldSyntax*, ParserError*> { .tag = Result<FieldSyntax*, ParserError*>::Ok, .ok = nullptr };
        }

        auto property_result = this->parse_property(_r, _rp, _ep);
        if (property_result.tag == Result<PropertySyntax*, ParserError*>::Error)
            return Result<FieldSyntax*, ParserError*> { .tag = Result<FieldSyntax*, ParserError*>::Error, .error = property_result.error };
        auto property = property_result.ok;
        if (property == nullptr) {
            return Result<FieldSyntax*, ParserError*> { .tag = Result<FieldSyntax*, ParserError*>::Error, .error = new(alignof(ParserError), _ep) ParserError {
                .file_name = file_name,
                .position = lexer.position,
            }};
        }

        auto end = this->lexer.position;

        auto ret = new(alignof(FieldSyntax), _rp) FieldSyntax {
            .start = start,
            .end = end,
            .property = property,
        };

        return Result<FieldSyntax*, ParserError*> { .tag = Result<FieldSyntax*, ParserError*>::Ok, .ok = ret };
    }

    Result<Vector<PropertySyntax>*, ParserError*> parse_property_list(Region& _pr, Page* _rp, Page* _ep) {
        auto _r = Region::create(_pr);
        {
            auto _r_1 = Region::create(_r);
            Array<PropertySyntax>* array = nullptr;
            while(true) {
                auto node_result = this->parse_property(_r_1, _rp, _ep);
                if (node_result.tag == Result<PropertySyntax*, ParserError*>::Error)
                    return Result<Vector<PropertySyntax>*, ParserError*> { .tag = Result<Vector<PropertySyntax>*, ParserError*>::Error, .error = node_result.error };
                auto node = node_result.ok;
                if (node != nullptr) {
                    if (array == nullptr)
                        array = Array<PropertySyntax>::create(_r_1.page);
                    array->add(*node);
                } else {
                    break;
                }
            }

            if (array == nullptr)
                return Result<Vector<PropertySyntax>*, ParserError*> { .tag = Result<Vector<PropertySyntax>*, ParserError*>::Ok, .ok = nullptr };
            
            return Result<Vector<PropertySyntax>*, ParserError*> { .tag = Result<Vector<PropertySyntax>*, ParserError*>::Ok, .ok = Vector<PropertySyntax>::from_array(_rp, *array) };
        }
    }

    Result<PropertySyntax*, ParserError*> parse_property(Region& _pr, Page* _rp, Page* _ep) {
        auto _r = Region::create(_pr);
        auto start = this->lexer.previous_position;

        auto name = this->lexer.parse_identifier(_r.page, this->keywords);
        if (name != nullptr) {
            if (!this->is_identifier(*name)) {
                return Result<PropertySyntax*, ParserError*> { .tag = Result<PropertySyntax*, ParserError*>::Ok, .ok = nullptr };

            }
        }
        else {
                return Result<PropertySyntax*, ParserError*> { .tag = Result<PropertySyntax*, ParserError*>::Ok, .ok = nullptr };

        }

        auto annotation_result = this->parse_typeannotation(_r, _rp, _ep);
        if (annotation_result.tag == Result<TypeAnnotationSyntax*, ParserError*>::Error)
            return Result<PropertySyntax*, ParserError*> { .tag = Result<PropertySyntax*, ParserError*>::Error, .error = annotation_result.error };
        auto annotation = annotation_result.ok;

        auto attributes_result = this->parse_attribute_list(_r, _rp, _ep);
        if (attributes_result.tag == Result<Vector<AttributeSyntax>*, ParserError*>::Error)
            return Result<PropertySyntax*, ParserError*> { .tag = Result<PropertySyntax*, ParserError*>::Error, .error = attributes_result.error };
        auto attributes = attributes_result.ok;

        auto success_comma_4 = this->lexer.parse_punctuation(_r.page, *String::from_c_string(_r.page, ","));
        if (!success_comma_4) {

        }

        auto success_colon_5 = this->lexer.parse_colon(_r.page);
        if (!success_colon_5) {

        }

        auto end = this->lexer.position;

        auto ret = new(alignof(PropertySyntax), _rp) PropertySyntax {
            .start = start,
            .end = end,
            .name = name,
            .annotation = annotation,
            .attributes = attributes,
        };

        return Result<PropertySyntax*, ParserError*> { .tag = Result<PropertySyntax*, ParserError*>::Ok, .ok = ret };
    }

    Result<TypeAnnotationSyntax*, ParserError*> parse_typeannotation(Region& _pr, Page* _rp, Page* _ep) {
        auto _r = Region::create(_pr);
        auto start = this->lexer.previous_position;

        auto success_colon_1 = this->lexer.parse_colon(_r.page);
        if (!success_colon_1) {

                return Result<TypeAnnotationSyntax*, ParserError*> { .tag = Result<TypeAnnotationSyntax*, ParserError*>::Ok, .ok = nullptr };
        }

        auto spec_result = this->parse_typespec(_r, _rp, _ep);
        if (spec_result.tag == Result<TypeSpecSyntax*, ParserError*>::Error)
            return Result<TypeAnnotationSyntax*, ParserError*> { .tag = Result<TypeAnnotationSyntax*, ParserError*>::Error, .error = spec_result.error };
        auto spec = spec_result.ok;
        if (spec == nullptr) {
            return Result<TypeAnnotationSyntax*, ParserError*> { .tag = Result<TypeAnnotationSyntax*, ParserError*>::Error, .error = new(alignof(ParserError), _ep) ParserError {
                .file_name = file_name,
                .position = lexer.position,
            }};
        }

        auto end = this->lexer.position;

        auto ret = new(alignof(TypeAnnotationSyntax), _rp) TypeAnnotationSyntax {
            .start = start,
            .end = end,
            .spec = spec,
        };

        return Result<TypeAnnotationSyntax*, ParserError*> { .tag = Result<TypeAnnotationSyntax*, ParserError*>::Ok, .ok = ret };
    }

    Result<Vector<TypeSpecSyntax>*, ParserError*> parse_typespec_list(Region& _pr, Page* _rp, Page* _ep) {
        auto _r = Region::create(_pr);
        {
            auto _r_1 = Region::create(_r);
            Array<TypeSpecSyntax>* array = nullptr;
            while(true) {
                auto node_result = this->parse_typespec(_r_1, _rp, _ep);
                if (node_result.tag == Result<TypeSpecSyntax*, ParserError*>::Error)
                    return Result<Vector<TypeSpecSyntax>*, ParserError*> { .tag = Result<Vector<TypeSpecSyntax>*, ParserError*>::Error, .error = node_result.error };
                auto node = node_result.ok;
                if (node != nullptr) {
                    if (array == nullptr)
                        array = Array<TypeSpecSyntax>::create(_r_1.page);
                    array->add(*node);
                } else {
                    break;
                }
            }

            if (array == nullptr)
                return Result<Vector<TypeSpecSyntax>*, ParserError*> { .tag = Result<Vector<TypeSpecSyntax>*, ParserError*>::Ok, .ok = nullptr };
            
            return Result<Vector<TypeSpecSyntax>*, ParserError*> { .tag = Result<Vector<TypeSpecSyntax>*, ParserError*>::Ok, .ok = Vector<TypeSpecSyntax>::from_array(_rp, *array) };
        }
    }

    Result<TypeSpecSyntax*, ParserError*> parse_typespec(Region& _pr, Page* _rp, Page* _ep) {
        auto _r = Region::create(_pr);
        {
            auto _r_1 = Region::create(_r);
            auto node_result = this->parse_structure(_r_1, _rp, _ep);
            if (node_result.tag == Result<StructureSyntax*, ParserError*>::Error)
                return Result<TypeSpecSyntax*, ParserError*> { .tag = Result<TypeSpecSyntax*, ParserError*>::Error, .error = node_result.error };
            auto node = node_result.ok;
            if (node != nullptr) {
                return Result<TypeSpecSyntax*, ParserError*> { .tag = Result<TypeSpecSyntax*, ParserError*>::Ok, .ok = 
                    new (alignof(TypeSpecSyntax), _rp) TypeSpecSyntax {
                        .tag = TypeSpecSyntax::Structure,
                        .structureSyntax = *node,
                    }
                };
            }
        }
        {
            auto _r_1 = Region::create(_r);
            auto node_result = this->parse_type(_r_1, _rp, _ep);
            if (node_result.tag == Result<TypeSyntax*, ParserError*>::Error)
                return Result<TypeSpecSyntax*, ParserError*> { .tag = Result<TypeSpecSyntax*, ParserError*>::Error, .error = node_result.error };
            auto node = node_result.ok;
            if (node != nullptr) {
                return Result<TypeSpecSyntax*, ParserError*> { .tag = Result<TypeSpecSyntax*, ParserError*>::Ok, .ok = 
                    new (alignof(TypeSpecSyntax), _rp) TypeSpecSyntax {
                        .tag = TypeSpecSyntax::Type,
                        .typeSyntax = *node,
                    }
                };
            }
        }
        return Result<TypeSpecSyntax*, ParserError*> { .tag = Result<TypeSpecSyntax*, ParserError*>::Ok, .ok = nullptr };
    }

    Result<TypeSyntax*, ParserError*> parse_type(Region& _pr, Page* _rp, Page* _ep) {
        auto _r = Region::create(_pr);
        auto start = this->lexer.previous_position;

        auto name_result = this->parse_name(_r, _rp, _ep);
        if (name_result.tag == Result<NameSyntax*, ParserError*>::Error)
            return Result<TypeSyntax*, ParserError*> { .tag = Result<TypeSyntax*, ParserError*>::Error, .error = name_result.error };
        auto name = name_result.ok;
        if (name == nullptr) {
            return Result<TypeSyntax*, ParserError*> { .tag = Result<TypeSyntax*, ParserError*>::Ok, .ok = nullptr };
        }

        auto generics_result = this->parse_genericarguments(_r, _rp, _ep);
        if (generics_result.tag == Result<GenericArgumentsSyntax*, ParserError*>::Error)
            return Result<TypeSyntax*, ParserError*> { .tag = Result<TypeSyntax*, ParserError*>::Error, .error = generics_result.error };
        auto generics = generics_result.ok;

        auto optional_result = this->parse_optional(_r, _rp, _ep);
        if (optional_result.tag == Result<OptionalSyntax*, ParserError*>::Error)
            return Result<TypeSyntax*, ParserError*> { .tag = Result<TypeSyntax*, ParserError*>::Error, .error = optional_result.error };
        auto optional = optional_result.ok;

        auto end = this->lexer.position;

        auto ret = new(alignof(TypeSyntax), _rp) TypeSyntax {
            .start = start,
            .end = end,
            .name = name,
            .generics = generics,
            .optional = optional,
        };

        return Result<TypeSyntax*, ParserError*> { .tag = Result<TypeSyntax*, ParserError*>::Ok, .ok = ret };
    }

    Result<ArraySyntax*, ParserError*> parse_array(Region& _pr, Page* _rp, Page* _ep) {
        auto _r = Region::create(_pr);
        auto start = this->lexer.previous_position;

        auto success_left_bracket_1 = this->lexer.parse_punctuation(_r.page, *String::from_c_string(_r.page, "["));
        if (!success_left_bracket_1) {

                return Result<ArraySyntax*, ParserError*> { .tag = Result<ArraySyntax*, ParserError*>::Ok, .ok = nullptr };
        }

        auto members_result = this->parse_typespec_list(_r, _rp, _ep);
        if (members_result.tag == Result<Vector<TypeSpecSyntax>*, ParserError*>::Error)
            return Result<ArraySyntax*, ParserError*> { .tag = Result<ArraySyntax*, ParserError*>::Error, .error = members_result.error };
        auto members = members_result.ok;

        auto success_right_bracket_3 = this->lexer.parse_punctuation(_r.page, *String::from_c_string(_r.page, "]"));
        if (!success_right_bracket_3) {

            return Result<ArraySyntax*, ParserError*> { .tag = Result<ArraySyntax*, ParserError*>::Error, .error = new(alignof(ParserError), _ep) ParserError {
                .file_name = file_name,
                .position = lexer.position,
            }};        }

        auto end = this->lexer.position;

        auto ret = new(alignof(ArraySyntax), _rp) ArraySyntax {
            .start = start,
            .end = end,
            .members = members,
        };

        return Result<ArraySyntax*, ParserError*> { .tag = Result<ArraySyntax*, ParserError*>::Ok, .ok = ret };
    }

    Result<GenericArgumentsSyntax*, ParserError*> parse_genericarguments(Region& _pr, Page* _rp, Page* _ep) {
        auto _r = Region::create(_pr);
        auto start = this->lexer.previous_position;

        auto success_left_bracket_1 = this->lexer.parse_punctuation(_r.page, *String::from_c_string(_r.page, "["));
        if (!success_left_bracket_1) {

                return Result<GenericArgumentsSyntax*, ParserError*> { .tag = Result<GenericArgumentsSyntax*, ParserError*>::Ok, .ok = nullptr };
        }

        auto generics_result = this->parse_genericargument_list(_r, _rp, _ep);
        if (generics_result.tag == Result<Vector<GenericArgumentSyntax>*, ParserError*>::Error)
            return Result<GenericArgumentsSyntax*, ParserError*> { .tag = Result<GenericArgumentsSyntax*, ParserError*>::Error, .error = generics_result.error };
        auto generics = generics_result.ok;

        auto success_right_bracket_3 = this->lexer.parse_punctuation(_r.page, *String::from_c_string(_r.page, "]"));
        if (!success_right_bracket_3) {

            return Result<GenericArgumentsSyntax*, ParserError*> { .tag = Result<GenericArgumentsSyntax*, ParserError*>::Error, .error = new(alignof(ParserError), _ep) ParserError {
                .file_name = file_name,
                .position = lexer.position,
            }};        }

        auto end = this->lexer.position;

        auto ret = new(alignof(GenericArgumentsSyntax), _rp) GenericArgumentsSyntax {
            .start = start,
            .end = end,
            .generics = generics,
        };

        return Result<GenericArgumentsSyntax*, ParserError*> { .tag = Result<GenericArgumentsSyntax*, ParserError*>::Ok, .ok = ret };
    }

    Result<Vector<GenericArgumentSyntax>*, ParserError*> parse_genericargument_list(Region& _pr, Page* _rp, Page* _ep) {
        auto _r = Region::create(_pr);
        {
            auto _r_1 = Region::create(_r);
            Array<GenericArgumentSyntax>* array = nullptr;
            while(true) {
                auto node_result = this->parse_genericargument(_r_1, _rp, _ep);
                if (node_result.tag == Result<GenericArgumentSyntax*, ParserError*>::Error)
                    return Result<Vector<GenericArgumentSyntax>*, ParserError*> { .tag = Result<Vector<GenericArgumentSyntax>*, ParserError*>::Error, .error = node_result.error };
                auto node = node_result.ok;
                if (node != nullptr) {
                    if (array == nullptr)
                        array = Array<GenericArgumentSyntax>::create(_r_1.page);
                    array->add(*node);
                } else {
                    break;
                }
            }

            if (array == nullptr)
                return Result<Vector<GenericArgumentSyntax>*, ParserError*> { .tag = Result<Vector<GenericArgumentSyntax>*, ParserError*>::Ok, .ok = nullptr };
            
            return Result<Vector<GenericArgumentSyntax>*, ParserError*> { .tag = Result<Vector<GenericArgumentSyntax>*, ParserError*>::Ok, .ok = Vector<GenericArgumentSyntax>::from_array(_rp, *array) };
        }
    }

    Result<GenericArgumentSyntax*, ParserError*> parse_genericargument(Region& _pr, Page* _rp, Page* _ep) {
        auto _r = Region::create(_pr);
        auto start = this->lexer.previous_position;

        auto spec_result = this->parse_type(_r, _rp, _ep);
        if (spec_result.tag == Result<TypeSyntax*, ParserError*>::Error)
            return Result<GenericArgumentSyntax*, ParserError*> { .tag = Result<GenericArgumentSyntax*, ParserError*>::Error, .error = spec_result.error };
        auto spec = spec_result.ok;
        if (spec == nullptr) {
            return Result<GenericArgumentSyntax*, ParserError*> { .tag = Result<GenericArgumentSyntax*, ParserError*>::Ok, .ok = nullptr };
        }

        auto success_comma_2 = this->lexer.parse_punctuation(_r.page, *String::from_c_string(_r.page, ","));
        if (!success_comma_2) {

        }

        auto end = this->lexer.position;

        auto ret = new(alignof(GenericArgumentSyntax), _rp) GenericArgumentSyntax {
            .start = start,
            .end = end,
            .spec = spec,
        };

        return Result<GenericArgumentSyntax*, ParserError*> { .tag = Result<GenericArgumentSyntax*, ParserError*>::Ok, .ok = ret };
    }

    Result<OptionalSyntax*, ParserError*> parse_optional(Region& _pr, Page* _rp, Page* _ep) {
        auto _r = Region::create(_pr);
        auto start = this->lexer.previous_position;

        auto success_question_1 = this->lexer.parse_punctuation(_r.page, *String::from_c_string(_r.page, "?"));
        if (!success_question_1) {

                return Result<OptionalSyntax*, ParserError*> { .tag = Result<OptionalSyntax*, ParserError*>::Ok, .ok = nullptr };
        }

        auto end = this->lexer.position;

        auto ret = new(alignof(OptionalSyntax), _rp) OptionalSyntax {
            .start = start,
            .end = end,
        };

        return Result<OptionalSyntax*, ParserError*> { .tag = Result<OptionalSyntax*, ParserError*>::Ok, .ok = ret };
    }

    Result<FunctionSyntax*, ParserError*> parse_function(Region& _pr, Page* _rp, Page* _ep) {
        auto _r = Region::create(_pr);
        auto start = this->lexer.previous_position;

        auto success_function_1 = this->lexer.parse_keyword(_r.page, *String::from_c_string(_r.page, "function"));
        if (!success_function_1) {

                return Result<FunctionSyntax*, ParserError*> { .tag = Result<FunctionSyntax*, ParserError*>::Ok, .ok = nullptr };
        }

        auto name = this->lexer.parse_identifier(_r.page, this->keywords);
        if (name != nullptr) {
            if (!this->is_identifier(*name)) {
            return Result<FunctionSyntax*, ParserError*> { .tag = Result<FunctionSyntax*, ParserError*>::Error, .error = new(alignof(ParserError), _ep) ParserError {
                .file_name = file_name,
                .position = lexer.position,
            }};
            }
        }
        else {
            return Result<FunctionSyntax*, ParserError*> { .tag = Result<FunctionSyntax*, ParserError*>::Error, .error = new(alignof(ParserError), _ep) ParserError {
                .file_name = file_name,
                .position = lexer.position,
            }};
        }

        auto generics_result = this->parse_genericarguments(_r, _rp, _ep);
        if (generics_result.tag == Result<GenericArgumentsSyntax*, ParserError*>::Error)
            return Result<FunctionSyntax*, ParserError*> { .tag = Result<FunctionSyntax*, ParserError*>::Error, .error = generics_result.error };
        auto generics = generics_result.ok;

        auto routine_result = this->parse_routine(_r, _rp, _ep);
        if (routine_result.tag == Result<RoutineSyntax*, ParserError*>::Error)
            return Result<FunctionSyntax*, ParserError*> { .tag = Result<FunctionSyntax*, ParserError*>::Error, .error = routine_result.error };
        auto routine = routine_result.ok;
        if (routine == nullptr) {
            return Result<FunctionSyntax*, ParserError*> { .tag = Result<FunctionSyntax*, ParserError*>::Error, .error = new(alignof(ParserError), _ep) ParserError {
                .file_name = file_name,
                .position = lexer.position,
            }};
        }

        auto end = this->lexer.position;

        auto ret = new(alignof(FunctionSyntax), _rp) FunctionSyntax {
            .start = start,
            .end = end,
            .name = name,
            .generics = generics,
            .routine = routine,
        };

        return Result<FunctionSyntax*, ParserError*> { .tag = Result<FunctionSyntax*, ParserError*>::Ok, .ok = ret };
    }

    Result<ProcedureSyntax*, ParserError*> parse_procedure(Region& _pr, Page* _rp, Page* _ep) {
        auto _r = Region::create(_pr);
        auto start = this->lexer.previous_position;

        auto success_procedure_1 = this->lexer.parse_keyword(_r.page, *String::from_c_string(_r.page, "procedure"));
        if (!success_procedure_1) {

                return Result<ProcedureSyntax*, ParserError*> { .tag = Result<ProcedureSyntax*, ParserError*>::Ok, .ok = nullptr };
        }

        auto name = this->lexer.parse_identifier(_r.page, this->keywords);
        if (name != nullptr) {
            if (!this->is_identifier(*name)) {
            return Result<ProcedureSyntax*, ParserError*> { .tag = Result<ProcedureSyntax*, ParserError*>::Error, .error = new(alignof(ParserError), _ep) ParserError {
                .file_name = file_name,
                .position = lexer.position,
            }};
            }
        }
        else {
            return Result<ProcedureSyntax*, ParserError*> { .tag = Result<ProcedureSyntax*, ParserError*>::Error, .error = new(alignof(ParserError), _ep) ParserError {
                .file_name = file_name,
                .position = lexer.position,
            }};
        }

        auto generics_result = this->parse_genericarguments(_r, _rp, _ep);
        if (generics_result.tag == Result<GenericArgumentsSyntax*, ParserError*>::Error)
            return Result<ProcedureSyntax*, ParserError*> { .tag = Result<ProcedureSyntax*, ParserError*>::Error, .error = generics_result.error };
        auto generics = generics_result.ok;

        auto routine_result = this->parse_routine(_r, _rp, _ep);
        if (routine_result.tag == Result<RoutineSyntax*, ParserError*>::Error)
            return Result<ProcedureSyntax*, ParserError*> { .tag = Result<ProcedureSyntax*, ParserError*>::Error, .error = routine_result.error };
        auto routine = routine_result.ok;
        if (routine == nullptr) {
            return Result<ProcedureSyntax*, ParserError*> { .tag = Result<ProcedureSyntax*, ParserError*>::Error, .error = new(alignof(ParserError), _ep) ParserError {
                .file_name = file_name,
                .position = lexer.position,
            }};
        }

        auto end = this->lexer.position;

        auto ret = new(alignof(ProcedureSyntax), _rp) ProcedureSyntax {
            .start = start,
            .end = end,
            .name = name,
            .generics = generics,
            .routine = routine,
        };

        return Result<ProcedureSyntax*, ParserError*> { .tag = Result<ProcedureSyntax*, ParserError*>::Ok, .ok = ret };
    }

    Result<OperatorSyntax*, ParserError*> parse_operator(Region& _pr, Page* _rp, Page* _ep) {
        auto _r = Region::create(_pr);
        auto start = this->lexer.previous_position;

        auto success_operator_1 = this->lexer.parse_keyword(_r.page, *String::from_c_string(_r.page, "operator"));
        if (!success_operator_1) {

                return Result<OperatorSyntax*, ParserError*> { .tag = Result<OperatorSyntax*, ParserError*>::Ok, .ok = nullptr };
        }

        auto target_result = this->parse_target(_r, _rp, _ep);
        if (target_result.tag == Result<TargetSyntax*, ParserError*>::Error)
            return Result<OperatorSyntax*, ParserError*> { .tag = Result<OperatorSyntax*, ParserError*>::Error, .error = target_result.error };
        auto target = target_result.ok;
        if (target == nullptr) {
            return Result<OperatorSyntax*, ParserError*> { .tag = Result<OperatorSyntax*, ParserError*>::Error, .error = new(alignof(ParserError), _ep) ParserError {
                .file_name = file_name,
                .position = lexer.position,
            }};
        }

        auto end = this->lexer.position;

        auto ret = new(alignof(OperatorSyntax), _rp) OperatorSyntax {
            .start = start,
            .end = end,
            .target = target,
        };

        return Result<OperatorSyntax*, ParserError*> { .tag = Result<OperatorSyntax*, ParserError*>::Ok, .ok = ret };
    }

    Result<TargetSyntax*, ParserError*> parse_target(Region& _pr, Page* _rp, Page* _ep) {
        auto _r = Region::create(_pr);
        {
            auto _r_1 = Region::create(_r);
            auto node_result = this->parse_symbol(_r_1, _rp, _ep);
            if (node_result.tag == Result<SymbolSyntax*, ParserError*>::Error)
                return Result<TargetSyntax*, ParserError*> { .tag = Result<TargetSyntax*, ParserError*>::Error, .error = node_result.error };
            auto node = node_result.ok;
            if (node != nullptr) {
                return Result<TargetSyntax*, ParserError*> { .tag = Result<TargetSyntax*, ParserError*>::Ok, .ok = 
                    new (alignof(TargetSyntax), _rp) TargetSyntax {
                        .tag = TargetSyntax::Symbol,
                        .symbolSyntax = *node,
                    }
                };
            }
        }
        {
            auto _r_1 = Region::create(_r);
            auto node_result = this->parse_routine(_r_1, _rp, _ep);
            if (node_result.tag == Result<RoutineSyntax*, ParserError*>::Error)
                return Result<TargetSyntax*, ParserError*> { .tag = Result<TargetSyntax*, ParserError*>::Error, .error = node_result.error };
            auto node = node_result.ok;
            if (node != nullptr) {
                return Result<TargetSyntax*, ParserError*> { .tag = Result<TargetSyntax*, ParserError*>::Ok, .ok = 
                    new (alignof(TargetSyntax), _rp) TargetSyntax {
                        .tag = TargetSyntax::Routine,
                        .routineSyntax = *node,
                    }
                };
            }
        }
        return Result<TargetSyntax*, ParserError*> { .tag = Result<TargetSyntax*, ParserError*>::Ok, .ok = nullptr };
    }

    Result<RoutineSyntax*, ParserError*> parse_routine(Region& _pr, Page* _rp, Page* _ep) {
        auto _r = Region::create(_pr);
        auto start = this->lexer.previous_position;

        auto parameters_result = this->parse_parameterset(_r, _rp, _ep);
        if (parameters_result.tag == Result<ParameterSetSyntax*, ParserError*>::Error)
            return Result<RoutineSyntax*, ParserError*> { .tag = Result<RoutineSyntax*, ParserError*>::Error, .error = parameters_result.error };
        auto parameters = parameters_result.ok;

        auto attributes_result = this->parse_attribute_list(_r, _rp, _ep);
        if (attributes_result.tag == Result<Vector<AttributeSyntax>*, ParserError*>::Error)
            return Result<RoutineSyntax*, ParserError*> { .tag = Result<RoutineSyntax*, ParserError*>::Error, .error = attributes_result.error };
        auto attributes = attributes_result.ok;

        auto returns_result = this->parse_returns(_r, _rp, _ep);
        if (returns_result.tag == Result<ReturnsSyntax*, ParserError*>::Error)
            return Result<RoutineSyntax*, ParserError*> { .tag = Result<RoutineSyntax*, ParserError*>::Error, .error = returns_result.error };
        auto returns = returns_result.ok;

        auto success_colon_4 = this->lexer.parse_colon(_r.page);
        if (!success_colon_4) {

        }

        auto throws_result = this->parse_throws(_r, _rp, _ep);
        if (throws_result.tag == Result<ThrowsSyntax*, ParserError*>::Error)
            return Result<RoutineSyntax*, ParserError*> { .tag = Result<RoutineSyntax*, ParserError*>::Error, .error = throws_result.error };
        auto throws = throws_result.ok;

        auto success_colon_6 = this->lexer.parse_colon(_r.page);
        if (!success_colon_6) {

        }

        auto implementation_result = this->parse_implementation(_r, _rp, _ep);
        if (implementation_result.tag == Result<ImplementationSyntax*, ParserError*>::Error)
            return Result<RoutineSyntax*, ParserError*> { .tag = Result<RoutineSyntax*, ParserError*>::Error, .error = implementation_result.error };
        auto implementation = implementation_result.ok;
        if (implementation == nullptr) {
            return Result<RoutineSyntax*, ParserError*> { .tag = Result<RoutineSyntax*, ParserError*>::Error, .error = new(alignof(ParserError), _ep) ParserError {
                .file_name = file_name,
                .position = lexer.position,
            }};
        }

        auto end = this->lexer.position;

        auto ret = new(alignof(RoutineSyntax), _rp) RoutineSyntax {
            .start = start,
            .end = end,
            .parameters = parameters,
            .attributes = attributes,
            .returns = returns,
            .throws = throws,
            .implementation = implementation,
        };

        return Result<RoutineSyntax*, ParserError*> { .tag = Result<RoutineSyntax*, ParserError*>::Ok, .ok = ret };
    }

    Result<SymbolSyntax*, ParserError*> parse_symbol(Region& _pr, Page* _rp, Page* _ep) {
        auto _r = Region::create(_pr);
        auto start = this->lexer.previous_position;

        auto name = this->lexer.parse_identifier(_r.page, this->keywords);
        if (name != nullptr) {
            if (!this->is_identifier(*name)) {
                return Result<SymbolSyntax*, ParserError*> { .tag = Result<SymbolSyntax*, ParserError*>::Ok, .ok = nullptr };

            }
        }
        else {
                return Result<SymbolSyntax*, ParserError*> { .tag = Result<SymbolSyntax*, ParserError*>::Ok, .ok = nullptr };

        }

        auto attributes_result = this->parse_attribute_list(_r, _rp, _ep);
        if (attributes_result.tag == Result<Vector<AttributeSyntax>*, ParserError*>::Error)
            return Result<SymbolSyntax*, ParserError*> { .tag = Result<SymbolSyntax*, ParserError*>::Error, .error = attributes_result.error };
        auto attributes = attributes_result.ok;

        auto returns_result = this->parse_returns(_r, _rp, _ep);
        if (returns_result.tag == Result<ReturnsSyntax*, ParserError*>::Error)
            return Result<SymbolSyntax*, ParserError*> { .tag = Result<SymbolSyntax*, ParserError*>::Error, .error = returns_result.error };
        auto returns = returns_result.ok;

        auto success_colon_4 = this->lexer.parse_colon(_r.page);
        if (!success_colon_4) {

        }

        auto throws_result = this->parse_throws(_r, _rp, _ep);
        if (throws_result.tag == Result<ThrowsSyntax*, ParserError*>::Error)
            return Result<SymbolSyntax*, ParserError*> { .tag = Result<SymbolSyntax*, ParserError*>::Error, .error = throws_result.error };
        auto throws = throws_result.ok;

        auto success_colon_6 = this->lexer.parse_colon(_r.page);
        if (!success_colon_6) {

        }

        auto implementation_result = this->parse_implementation(_r, _rp, _ep);
        if (implementation_result.tag == Result<ImplementationSyntax*, ParserError*>::Error)
            return Result<SymbolSyntax*, ParserError*> { .tag = Result<SymbolSyntax*, ParserError*>::Error, .error = implementation_result.error };
        auto implementation = implementation_result.ok;
        if (implementation == nullptr) {
            return Result<SymbolSyntax*, ParserError*> { .tag = Result<SymbolSyntax*, ParserError*>::Error, .error = new(alignof(ParserError), _ep) ParserError {
                .file_name = file_name,
                .position = lexer.position,
            }};
        }

        auto end = this->lexer.position;

        auto ret = new(alignof(SymbolSyntax), _rp) SymbolSyntax {
            .start = start,
            .end = end,
            .name = name,
            .attributes = attributes,
            .returns = returns,
            .throws = throws,
            .implementation = implementation,
        };

        return Result<SymbolSyntax*, ParserError*> { .tag = Result<SymbolSyntax*, ParserError*>::Ok, .ok = ret };
    }

    Result<ParameterSetSyntax*, ParserError*> parse_parameterset(Region& _pr, Page* _rp, Page* _ep) {
        auto _r = Region::create(_pr);
        {
            auto _r_1 = Region::create(_r);
            auto node_result = this->parse_parameters(_r_1, _rp, _ep);
            if (node_result.tag == Result<ParametersSyntax*, ParserError*>::Error)
                return Result<ParameterSetSyntax*, ParserError*> { .tag = Result<ParameterSetSyntax*, ParserError*>::Error, .error = node_result.error };
            auto node = node_result.ok;
            if (node != nullptr) {
                return Result<ParameterSetSyntax*, ParserError*> { .tag = Result<ParameterSetSyntax*, ParserError*>::Ok, .ok = 
                    new (alignof(ParameterSetSyntax), _rp) ParameterSetSyntax {
                        .tag = ParameterSetSyntax::Parameters,
                        .parametersSyntax = *node,
                    }
                };
            }
        }
        {
            auto _r_1 = Region::create(_r);
            auto node_result = this->parse_type(_r_1, _rp, _ep);
            if (node_result.tag == Result<TypeSyntax*, ParserError*>::Error)
                return Result<ParameterSetSyntax*, ParserError*> { .tag = Result<ParameterSetSyntax*, ParserError*>::Error, .error = node_result.error };
            auto node = node_result.ok;
            if (node != nullptr) {
                return Result<ParameterSetSyntax*, ParserError*> { .tag = Result<ParameterSetSyntax*, ParserError*>::Ok, .ok = 
                    new (alignof(ParameterSetSyntax), _rp) ParameterSetSyntax {
                        .tag = ParameterSetSyntax::Type,
                        .typeSyntax = *node,
                    }
                };
            }
        }
        return Result<ParameterSetSyntax*, ParserError*> { .tag = Result<ParameterSetSyntax*, ParserError*>::Ok, .ok = nullptr };
    }

    Result<ParametersSyntax*, ParserError*> parse_parameters(Region& _pr, Page* _rp, Page* _ep) {
        auto _r = Region::create(_pr);
        auto start = this->lexer.previous_position;

        auto success_left_paren_1 = this->lexer.parse_punctuation(_r.page, *String::from_c_string(_r.page, "("));
        if (!success_left_paren_1) {

                return Result<ParametersSyntax*, ParserError*> { .tag = Result<ParametersSyntax*, ParserError*>::Ok, .ok = nullptr };
        }

        auto properties_result = this->parse_property_list(_r, _rp, _ep);
        if (properties_result.tag == Result<Vector<PropertySyntax>*, ParserError*>::Error)
            return Result<ParametersSyntax*, ParserError*> { .tag = Result<ParametersSyntax*, ParserError*>::Error, .error = properties_result.error };
        auto properties = properties_result.ok;

        auto success_right_paren_3 = this->lexer.parse_punctuation(_r.page, *String::from_c_string(_r.page, ")"));
        if (!success_right_paren_3) {

            return Result<ParametersSyntax*, ParserError*> { .tag = Result<ParametersSyntax*, ParserError*>::Error, .error = new(alignof(ParserError), _ep) ParserError {
                .file_name = file_name,
                .position = lexer.position,
            }};        }

        auto end = this->lexer.position;

        auto ret = new(alignof(ParametersSyntax), _rp) ParametersSyntax {
            .start = start,
            .end = end,
            .properties = properties,
        };

        return Result<ParametersSyntax*, ParserError*> { .tag = Result<ParametersSyntax*, ParserError*>::Ok, .ok = ret };
    }

    Result<ReturnsSyntax*, ParserError*> parse_returns(Region& _pr, Page* _rp, Page* _ep) {
        auto _r = Region::create(_pr);
        auto start = this->lexer.previous_position;

        auto success_returns_1 = this->lexer.parse_keyword(_r.page, *String::from_c_string(_r.page, "returns"));
        if (!success_returns_1) {

                return Result<ReturnsSyntax*, ParserError*> { .tag = Result<ReturnsSyntax*, ParserError*>::Ok, .ok = nullptr };
        }

        auto parameters_result = this->parse_parameterset(_r, _rp, _ep);
        if (parameters_result.tag == Result<ParameterSetSyntax*, ParserError*>::Error)
            return Result<ReturnsSyntax*, ParserError*> { .tag = Result<ReturnsSyntax*, ParserError*>::Error, .error = parameters_result.error };
        auto parameters = parameters_result.ok;
        if (parameters == nullptr) {
            return Result<ReturnsSyntax*, ParserError*> { .tag = Result<ReturnsSyntax*, ParserError*>::Error, .error = new(alignof(ParserError), _ep) ParserError {
                .file_name = file_name,
                .position = lexer.position,
            }};
        }

        auto attributes_result = this->parse_attribute_list(_r, _rp, _ep);
        if (attributes_result.tag == Result<Vector<AttributeSyntax>*, ParserError*>::Error)
            return Result<ReturnsSyntax*, ParserError*> { .tag = Result<ReturnsSyntax*, ParserError*>::Error, .error = attributes_result.error };
        auto attributes = attributes_result.ok;

        auto end = this->lexer.position;

        auto ret = new(alignof(ReturnsSyntax), _rp) ReturnsSyntax {
            .start = start,
            .end = end,
            .parameters = parameters,
            .attributes = attributes,
        };

        return Result<ReturnsSyntax*, ParserError*> { .tag = Result<ReturnsSyntax*, ParserError*>::Ok, .ok = ret };
    }

    Result<ThrowsSyntax*, ParserError*> parse_throws(Region& _pr, Page* _rp, Page* _ep) {
        auto _r = Region::create(_pr);
        auto start = this->lexer.previous_position;

        auto success_throws_1 = this->lexer.parse_keyword(_r.page, *String::from_c_string(_r.page, "throws"));
        if (!success_throws_1) {

                return Result<ThrowsSyntax*, ParserError*> { .tag = Result<ThrowsSyntax*, ParserError*>::Ok, .ok = nullptr };
        }

        auto type_result = this->parse_type(_r, _rp, _ep);
        if (type_result.tag == Result<TypeSyntax*, ParserError*>::Error)
            return Result<ThrowsSyntax*, ParserError*> { .tag = Result<ThrowsSyntax*, ParserError*>::Error, .error = type_result.error };
        auto type = type_result.ok;
        if (type == nullptr) {
            return Result<ThrowsSyntax*, ParserError*> { .tag = Result<ThrowsSyntax*, ParserError*>::Error, .error = new(alignof(ParserError), _ep) ParserError {
                .file_name = file_name,
                .position = lexer.position,
            }};
        }

        auto attributes_result = this->parse_attribute_list(_r, _rp, _ep);
        if (attributes_result.tag == Result<Vector<AttributeSyntax>*, ParserError*>::Error)
            return Result<ThrowsSyntax*, ParserError*> { .tag = Result<ThrowsSyntax*, ParserError*>::Error, .error = attributes_result.error };
        auto attributes = attributes_result.ok;

        auto end = this->lexer.position;

        auto ret = new(alignof(ThrowsSyntax), _rp) ThrowsSyntax {
            .start = start,
            .end = end,
            .type = type,
            .attributes = attributes,
        };

        return Result<ThrowsSyntax*, ParserError*> { .tag = Result<ThrowsSyntax*, ParserError*>::Ok, .ok = ret };
    }

    Result<Vector<ActionSyntax>*, ParserError*> parse_action_list(Region& _pr, Page* _rp, Page* _ep) {
        auto _r = Region::create(_pr);
        {
            auto _r_1 = Region::create(_r);
            Array<ActionSyntax>* array = nullptr;
            while(true) {
                auto node_result = this->parse_action(_r_1, _rp, _ep);
                if (node_result.tag == Result<ActionSyntax*, ParserError*>::Error)
                    return Result<Vector<ActionSyntax>*, ParserError*> { .tag = Result<Vector<ActionSyntax>*, ParserError*>::Error, .error = node_result.error };
                auto node = node_result.ok;
                if (node != nullptr) {
                    if (array == nullptr)
                        array = Array<ActionSyntax>::create(_r_1.page);
                    array->add(*node);
                } else {
                    break;
                }
            }

            if (array == nullptr)
                return Result<Vector<ActionSyntax>*, ParserError*> { .tag = Result<Vector<ActionSyntax>*, ParserError*>::Ok, .ok = nullptr };
            
            return Result<Vector<ActionSyntax>*, ParserError*> { .tag = Result<Vector<ActionSyntax>*, ParserError*>::Ok, .ok = Vector<ActionSyntax>::from_array(_rp, *array) };
        }
    }

    Result<ActionSyntax*, ParserError*> parse_action(Region& _pr, Page* _rp, Page* _ep) {
        auto _r = Region::create(_pr);
        {
            auto _r_1 = Region::create(_r);
            auto node_result = this->parse_operation(_r_1, _rp, _ep);
            if (node_result.tag == Result<OperationSyntax*, ParserError*>::Error)
                return Result<ActionSyntax*, ParserError*> { .tag = Result<ActionSyntax*, ParserError*>::Error, .error = node_result.error };
            auto node = node_result.ok;
            if (node != nullptr) {
                return Result<ActionSyntax*, ParserError*> { .tag = Result<ActionSyntax*, ParserError*>::Ok, .ok = 
                    new (alignof(ActionSyntax), _rp) ActionSyntax {
                        .tag = ActionSyntax::Operation,
                        .operationSyntax = *node,
                    }
                };
            }
        }
        {
            auto _r_1 = Region::create(_r);
            auto node_result = this->parse_set(_r_1, _rp, _ep);
            if (node_result.tag == Result<SetSyntax*, ParserError*>::Error)
                return Result<ActionSyntax*, ParserError*> { .tag = Result<ActionSyntax*, ParserError*>::Error, .error = node_result.error };
            auto node = node_result.ok;
            if (node != nullptr) {
                return Result<ActionSyntax*, ParserError*> { .tag = Result<ActionSyntax*, ParserError*>::Ok, .ok = 
                    new (alignof(ActionSyntax), _rp) ActionSyntax {
                        .tag = ActionSyntax::Set,
                        .setSyntax = *node,
                    }
                };
            }
        }
        return Result<ActionSyntax*, ParserError*> { .tag = Result<ActionSyntax*, ParserError*>::Ok, .ok = nullptr };
    }

    Result<ImplementationSyntax*, ParserError*> parse_implementation(Region& _pr, Page* _rp, Page* _ep) {
        auto _r = Region::create(_pr);
        {
            auto _r_1 = Region::create(_r);
            auto node_result = this->parse_action(_r_1, _rp, _ep);
            if (node_result.tag == Result<ActionSyntax*, ParserError*>::Error)
                return Result<ImplementationSyntax*, ParserError*> { .tag = Result<ImplementationSyntax*, ParserError*>::Error, .error = node_result.error };
            auto node = node_result.ok;
            if (node != nullptr) {
                return Result<ImplementationSyntax*, ParserError*> { .tag = Result<ImplementationSyntax*, ParserError*>::Ok, .ok = 
                    new (alignof(ImplementationSyntax), _rp) ImplementationSyntax {
                        .tag = ImplementationSyntax::Action,
                        .actionSyntax = *node,
                    }
                };
            }
        }
        {
            auto _r_1 = Region::create(_r);
            auto node_result = this->parse_extern(_r_1, _rp, _ep);
            if (node_result.tag == Result<ExternSyntax*, ParserError*>::Error)
                return Result<ImplementationSyntax*, ParserError*> { .tag = Result<ImplementationSyntax*, ParserError*>::Error, .error = node_result.error };
            auto node = node_result.ok;
            if (node != nullptr) {
                return Result<ImplementationSyntax*, ParserError*> { .tag = Result<ImplementationSyntax*, ParserError*>::Ok, .ok = 
                    new (alignof(ImplementationSyntax), _rp) ImplementationSyntax {
                        .tag = ImplementationSyntax::Extern,
                        .externSyntax = *node,
                    }
                };
            }
        }
        {
            auto _r_1 = Region::create(_r);
            auto node_result = this->parse_instruction(_r_1, _rp, _ep);
            if (node_result.tag == Result<InstructionSyntax*, ParserError*>::Error)
                return Result<ImplementationSyntax*, ParserError*> { .tag = Result<ImplementationSyntax*, ParserError*>::Error, .error = node_result.error };
            auto node = node_result.ok;
            if (node != nullptr) {
                return Result<ImplementationSyntax*, ParserError*> { .tag = Result<ImplementationSyntax*, ParserError*>::Ok, .ok = 
                    new (alignof(ImplementationSyntax), _rp) ImplementationSyntax {
                        .tag = ImplementationSyntax::Instruction,
                        .instructionSyntax = *node,
                    }
                };
            }
        }
        {
            auto _r_1 = Region::create(_r);
            auto node_result = this->parse_intrinsic(_r_1, _rp, _ep);
            if (node_result.tag == Result<IntrinsicSyntax*, ParserError*>::Error)
                return Result<ImplementationSyntax*, ParserError*> { .tag = Result<ImplementationSyntax*, ParserError*>::Error, .error = node_result.error };
            auto node = node_result.ok;
            if (node != nullptr) {
                return Result<ImplementationSyntax*, ParserError*> { .tag = Result<ImplementationSyntax*, ParserError*>::Ok, .ok = 
                    new (alignof(ImplementationSyntax), _rp) ImplementationSyntax {
                        .tag = ImplementationSyntax::Intrinsic,
                        .intrinsicSyntax = *node,
                    }
                };
            }
        }
        return Result<ImplementationSyntax*, ParserError*> { .tag = Result<ImplementationSyntax*, ParserError*>::Ok, .ok = nullptr };
    }

    Result<ExternSyntax*, ParserError*> parse_extern(Region& _pr, Page* _rp, Page* _ep) {
        auto _r = Region::create(_pr);
        auto start = this->lexer.previous_position;

        auto success_extern_1 = this->lexer.parse_keyword(_r.page, *String::from_c_string(_r.page, "extern"));
        if (!success_extern_1) {

                return Result<ExternSyntax*, ParserError*> { .tag = Result<ExternSyntax*, ParserError*>::Ok, .ok = nullptr };
        }

        auto end = this->lexer.position;

        auto ret = new(alignof(ExternSyntax), _rp) ExternSyntax {
            .start = start,
            .end = end,
        };

        return Result<ExternSyntax*, ParserError*> { .tag = Result<ExternSyntax*, ParserError*>::Ok, .ok = ret };
    }

    Result<InstructionSyntax*, ParserError*> parse_instruction(Region& _pr, Page* _rp, Page* _ep) {
        auto _r = Region::create(_pr);
        auto start = this->lexer.previous_position;

        auto success_instruction_1 = this->lexer.parse_keyword(_r.page, *String::from_c_string(_r.page, "instruction"));
        if (!success_instruction_1) {

                return Result<InstructionSyntax*, ParserError*> { .tag = Result<InstructionSyntax*, ParserError*>::Ok, .ok = nullptr };
        }

        auto success_colon_2 = this->lexer.parse_colon(_r.page);
        if (!success_colon_2) {

        }

        auto end = this->lexer.position;

        auto ret = new(alignof(InstructionSyntax), _rp) InstructionSyntax {
            .start = start,
            .end = end,
        };

        return Result<InstructionSyntax*, ParserError*> { .tag = Result<InstructionSyntax*, ParserError*>::Ok, .ok = ret };
    }

    Result<IntrinsicSyntax*, ParserError*> parse_intrinsic(Region& _pr, Page* _rp, Page* _ep) {
        auto _r = Region::create(_pr);
        auto start = this->lexer.previous_position;

        auto success_intrinsic_1 = this->lexer.parse_keyword(_r.page, *String::from_c_string(_r.page, "intrinsic"));
        if (!success_intrinsic_1) {

                return Result<IntrinsicSyntax*, ParserError*> { .tag = Result<IntrinsicSyntax*, ParserError*>::Ok, .ok = nullptr };
        }

        auto success_colon_2 = this->lexer.parse_colon(_r.page);
        if (!success_colon_2) {

        }

        auto end = this->lexer.position;

        auto ret = new(alignof(IntrinsicSyntax), _rp) IntrinsicSyntax {
            .start = start,
            .end = end,
        };

        return Result<IntrinsicSyntax*, ParserError*> { .tag = Result<IntrinsicSyntax*, ParserError*>::Ok, .ok = ret };
    }

    Result<UseSyntax*, ParserError*> parse_use(Region& _pr, Page* _rp, Page* _ep) {
        auto _r = Region::create(_pr);
        auto start = this->lexer.previous_position;

        auto success_use_1 = this->lexer.parse_keyword(_r.page, *String::from_c_string(_r.page, "use"));
        if (!success_use_1) {

                return Result<UseSyntax*, ParserError*> { .tag = Result<UseSyntax*, ParserError*>::Ok, .ok = nullptr };
        }

        auto name_result = this->parse_name(_r, _rp, _ep);
        if (name_result.tag == Result<NameSyntax*, ParserError*>::Error)
            return Result<UseSyntax*, ParserError*> { .tag = Result<UseSyntax*, ParserError*>::Error, .error = name_result.error };
        auto name = name_result.ok;
        if (name == nullptr) {
            return Result<UseSyntax*, ParserError*> { .tag = Result<UseSyntax*, ParserError*>::Error, .error = new(alignof(ParserError), _ep) ParserError {
                .file_name = file_name,
                .position = lexer.position,
            }};
        }

        auto success_colon_3 = this->lexer.parse_colon(_r.page);
        if (!success_colon_3) {

        }

        auto end = this->lexer.position;

        auto ret = new(alignof(UseSyntax), _rp) UseSyntax {
            .start = start,
            .end = end,
            .name = name,
        };

        return Result<UseSyntax*, ParserError*> { .tag = Result<UseSyntax*, ParserError*>::Ok, .ok = ret };
    }

    Result<ImplementSyntax*, ParserError*> parse_implement(Region& _pr, Page* _rp, Page* _ep) {
        auto _r = Region::create(_pr);
        auto start = this->lexer.previous_position;

        auto success_implement_1 = this->lexer.parse_keyword(_r.page, *String::from_c_string(_r.page, "implement"));
        if (!success_implement_1) {

                return Result<ImplementSyntax*, ParserError*> { .tag = Result<ImplementSyntax*, ParserError*>::Ok, .ok = nullptr };
        }

        auto type_result = this->parse_type(_r, _rp, _ep);
        if (type_result.tag == Result<TypeSyntax*, ParserError*>::Error)
            return Result<ImplementSyntax*, ParserError*> { .tag = Result<ImplementSyntax*, ParserError*>::Error, .error = type_result.error };
        auto type = type_result.ok;
        if (type == nullptr) {
            return Result<ImplementSyntax*, ParserError*> { .tag = Result<ImplementSyntax*, ParserError*>::Error, .error = new(alignof(ParserError), _ep) ParserError {
                .file_name = file_name,
                .position = lexer.position,
            }};
        }

        auto attributes_result = this->parse_attribute_list(_r, _rp, _ep);
        if (attributes_result.tag == Result<Vector<AttributeSyntax>*, ParserError*>::Error)
            return Result<ImplementSyntax*, ParserError*> { .tag = Result<ImplementSyntax*, ParserError*>::Error, .error = attributes_result.error };
        auto attributes = attributes_result.ok;

        auto success_colon_4 = this->lexer.parse_colon(_r.page);
        if (!success_colon_4) {

        }

        auto success_left_curly_5 = this->lexer.parse_punctuation(_r.page, *String::from_c_string(_r.page, "{"));
        if (!success_left_curly_5) {

            return Result<ImplementSyntax*, ParserError*> { .tag = Result<ImplementSyntax*, ParserError*>::Error, .error = new(alignof(ParserError), _ep) ParserError {
                .file_name = file_name,
                .position = lexer.position,
            }};        }

        auto methods_result = this->parse_method_list(_r, _rp, _ep);
        if (methods_result.tag == Result<Vector<MethodSyntax>*, ParserError*>::Error)
            return Result<ImplementSyntax*, ParserError*> { .tag = Result<ImplementSyntax*, ParserError*>::Error, .error = methods_result.error };
        auto methods = methods_result.ok;

        auto success_right_curly_7 = this->lexer.parse_punctuation(_r.page, *String::from_c_string(_r.page, "}"));
        if (!success_right_curly_7) {

            return Result<ImplementSyntax*, ParserError*> { .tag = Result<ImplementSyntax*, ParserError*>::Error, .error = new(alignof(ParserError), _ep) ParserError {
                .file_name = file_name,
                .position = lexer.position,
            }};        }

        auto success_colon_8 = this->lexer.parse_colon(_r.page);
        if (!success_colon_8) {

        }

        auto end = this->lexer.position;

        auto ret = new(alignof(ImplementSyntax), _rp) ImplementSyntax {
            .start = start,
            .end = end,
            .type = type,
            .attributes = attributes,
            .methods = methods,
        };

        return Result<ImplementSyntax*, ParserError*> { .tag = Result<ImplementSyntax*, ParserError*>::Ok, .ok = ret };
    }

    Result<TraitSyntax*, ParserError*> parse_trait(Region& _pr, Page* _rp, Page* _ep) {
        auto _r = Region::create(_pr);
        auto start = this->lexer.previous_position;

        auto success_trait_1 = this->lexer.parse_keyword(_r.page, *String::from_c_string(_r.page, "trait"));
        if (!success_trait_1) {

                return Result<TraitSyntax*, ParserError*> { .tag = Result<TraitSyntax*, ParserError*>::Ok, .ok = nullptr };
        }

        auto name_result = this->parse_name(_r, _rp, _ep);
        if (name_result.tag == Result<NameSyntax*, ParserError*>::Error)
            return Result<TraitSyntax*, ParserError*> { .tag = Result<TraitSyntax*, ParserError*>::Error, .error = name_result.error };
        auto name = name_result.ok;
        if (name == nullptr) {
            return Result<TraitSyntax*, ParserError*> { .tag = Result<TraitSyntax*, ParserError*>::Error, .error = new(alignof(ParserError), _ep) ParserError {
                .file_name = file_name,
                .position = lexer.position,
            }};
        }

        auto extension_result = this->parse_extends(_r, _rp, _ep);
        if (extension_result.tag == Result<ExtendsSyntax*, ParserError*>::Error)
            return Result<TraitSyntax*, ParserError*> { .tag = Result<TraitSyntax*, ParserError*>::Error, .error = extension_result.error };
        auto extension = extension_result.ok;

        auto attributes_result = this->parse_attribute_list(_r, _rp, _ep);
        if (attributes_result.tag == Result<Vector<AttributeSyntax>*, ParserError*>::Error)
            return Result<TraitSyntax*, ParserError*> { .tag = Result<TraitSyntax*, ParserError*>::Error, .error = attributes_result.error };
        auto attributes = attributes_result.ok;

        auto success_left_curly_5 = this->lexer.parse_punctuation(_r.page, *String::from_c_string(_r.page, "{"));
        if (!success_left_curly_5) {

            return Result<TraitSyntax*, ParserError*> { .tag = Result<TraitSyntax*, ParserError*>::Error, .error = new(alignof(ParserError), _ep) ParserError {
                .file_name = file_name,
                .position = lexer.position,
            }};        }

        auto functions_result = this->parse_method_list(_r, _rp, _ep);
        if (functions_result.tag == Result<Vector<MethodSyntax>*, ParserError*>::Error)
            return Result<TraitSyntax*, ParserError*> { .tag = Result<TraitSyntax*, ParserError*>::Error, .error = functions_result.error };
        auto functions = functions_result.ok;

        auto success_right_curly_7 = this->lexer.parse_punctuation(_r.page, *String::from_c_string(_r.page, "}"));
        if (!success_right_curly_7) {

            return Result<TraitSyntax*, ParserError*> { .tag = Result<TraitSyntax*, ParserError*>::Error, .error = new(alignof(ParserError), _ep) ParserError {
                .file_name = file_name,
                .position = lexer.position,
            }};        }

        auto success_colon_8 = this->lexer.parse_colon(_r.page);
        if (!success_colon_8) {

        }

        auto end = this->lexer.position;

        auto ret = new(alignof(TraitSyntax), _rp) TraitSyntax {
            .start = start,
            .end = end,
            .name = name,
            .extension = extension,
            .attributes = attributes,
            .functions = functions,
        };

        return Result<TraitSyntax*, ParserError*> { .tag = Result<TraitSyntax*, ParserError*>::Ok, .ok = ret };
    }

    Result<Vector<MethodSyntax>*, ParserError*> parse_method_list(Region& _pr, Page* _rp, Page* _ep) {
        auto _r = Region::create(_pr);
        {
            auto _r_1 = Region::create(_r);
            Array<MethodSyntax>* array = nullptr;
            while(true) {
                auto node_result = this->parse_method(_r_1, _rp, _ep);
                if (node_result.tag == Result<MethodSyntax*, ParserError*>::Error)
                    return Result<Vector<MethodSyntax>*, ParserError*> { .tag = Result<Vector<MethodSyntax>*, ParserError*>::Error, .error = node_result.error };
                auto node = node_result.ok;
                if (node != nullptr) {
                    if (array == nullptr)
                        array = Array<MethodSyntax>::create(_r_1.page);
                    array->add(*node);
                } else {
                    break;
                }
            }

            if (array == nullptr)
                return Result<Vector<MethodSyntax>*, ParserError*> { .tag = Result<Vector<MethodSyntax>*, ParserError*>::Ok, .ok = nullptr };
            
            return Result<Vector<MethodSyntax>*, ParserError*> { .tag = Result<Vector<MethodSyntax>*, ParserError*>::Ok, .ok = Vector<MethodSyntax>::from_array(_rp, *array) };
        }
    }

    Result<MethodSyntax*, ParserError*> parse_method(Region& _pr, Page* _rp, Page* _ep) {
        auto _r = Region::create(_pr);
        {
            auto _r_1 = Region::create(_r);
            auto node_result = this->parse_function(_r_1, _rp, _ep);
            if (node_result.tag == Result<FunctionSyntax*, ParserError*>::Error)
                return Result<MethodSyntax*, ParserError*> { .tag = Result<MethodSyntax*, ParserError*>::Error, .error = node_result.error };
            auto node = node_result.ok;
            if (node != nullptr) {
                return Result<MethodSyntax*, ParserError*> { .tag = Result<MethodSyntax*, ParserError*>::Ok, .ok = 
                    new (alignof(MethodSyntax), _rp) MethodSyntax {
                        .tag = MethodSyntax::Function,
                        .functionSyntax = *node,
                    }
                };
            }
        }
        {
            auto _r_1 = Region::create(_r);
            auto node_result = this->parse_procedure(_r_1, _rp, _ep);
            if (node_result.tag == Result<ProcedureSyntax*, ParserError*>::Error)
                return Result<MethodSyntax*, ParserError*> { .tag = Result<MethodSyntax*, ParserError*>::Error, .error = node_result.error };
            auto node = node_result.ok;
            if (node != nullptr) {
                return Result<MethodSyntax*, ParserError*> { .tag = Result<MethodSyntax*, ParserError*>::Ok, .ok = 
                    new (alignof(MethodSyntax), _rp) MethodSyntax {
                        .tag = MethodSyntax::Procedure,
                        .procedureSyntax = *node,
                    }
                };
            }
        }
        {
            auto _r_1 = Region::create(_r);
            auto node_result = this->parse_operator(_r_1, _rp, _ep);
            if (node_result.tag == Result<OperatorSyntax*, ParserError*>::Error)
                return Result<MethodSyntax*, ParserError*> { .tag = Result<MethodSyntax*, ParserError*>::Error, .error = node_result.error };
            auto node = node_result.ok;
            if (node != nullptr) {
                return Result<MethodSyntax*, ParserError*> { .tag = Result<MethodSyntax*, ParserError*>::Ok, .ok = 
                    new (alignof(MethodSyntax), _rp) MethodSyntax {
                        .tag = MethodSyntax::Operator,
                        .operatorSyntax = *node,
                    }
                };
            }
        }
        return Result<MethodSyntax*, ParserError*> { .tag = Result<MethodSyntax*, ParserError*>::Ok, .ok = nullptr };
    }

    Result<ExtendsSyntax*, ParserError*> parse_extends(Region& _pr, Page* _rp, Page* _ep) {
        auto _r = Region::create(_pr);
        auto start = this->lexer.previous_position;

        auto success_extends_1 = this->lexer.parse_keyword(_r.page, *String::from_c_string(_r.page, "extends"));
        if (!success_extends_1) {

                return Result<ExtendsSyntax*, ParserError*> { .tag = Result<ExtendsSyntax*, ParserError*>::Ok, .ok = nullptr };
        }

        auto extensions_result = this->parse_extend_list(_r, _rp, _ep);
        if (extensions_result.tag == Result<Vector<ExtendSyntax>*, ParserError*>::Error)
            return Result<ExtendsSyntax*, ParserError*> { .tag = Result<ExtendsSyntax*, ParserError*>::Error, .error = extensions_result.error };
        auto extensions = extensions_result.ok;

        auto end = this->lexer.position;

        auto ret = new(alignof(ExtendsSyntax), _rp) ExtendsSyntax {
            .start = start,
            .end = end,
            .extensions = extensions,
        };

        return Result<ExtendsSyntax*, ParserError*> { .tag = Result<ExtendsSyntax*, ParserError*>::Ok, .ok = ret };
    }

    Result<Vector<ExtendSyntax>*, ParserError*> parse_extend_list(Region& _pr, Page* _rp, Page* _ep) {
        auto _r = Region::create(_pr);
        {
            auto _r_1 = Region::create(_r);
            Array<ExtendSyntax>* array = nullptr;
            while(true) {
                auto node_result = this->parse_extend(_r_1, _rp, _ep);
                if (node_result.tag == Result<ExtendSyntax*, ParserError*>::Error)
                    return Result<Vector<ExtendSyntax>*, ParserError*> { .tag = Result<Vector<ExtendSyntax>*, ParserError*>::Error, .error = node_result.error };
                auto node = node_result.ok;
                if (node != nullptr) {
                    if (array == nullptr)
                        array = Array<ExtendSyntax>::create(_r_1.page);
                    array->add(*node);
                } else {
                    break;
                }
            }

            if (array == nullptr)
                return Result<Vector<ExtendSyntax>*, ParserError*> { .tag = Result<Vector<ExtendSyntax>*, ParserError*>::Ok, .ok = nullptr };
            
            return Result<Vector<ExtendSyntax>*, ParserError*> { .tag = Result<Vector<ExtendSyntax>*, ParserError*>::Ok, .ok = Vector<ExtendSyntax>::from_array(_rp, *array) };
        }
    }

    Result<ExtendSyntax*, ParserError*> parse_extend(Region& _pr, Page* _rp, Page* _ep) {
        auto _r = Region::create(_pr);
        auto start = this->lexer.previous_position;

        auto spec_result = this->parse_typespec(_r, _rp, _ep);
        if (spec_result.tag == Result<TypeSpecSyntax*, ParserError*>::Error)
            return Result<ExtendSyntax*, ParserError*> { .tag = Result<ExtendSyntax*, ParserError*>::Error, .error = spec_result.error };
        auto spec = spec_result.ok;
        if (spec == nullptr) {
            return Result<ExtendSyntax*, ParserError*> { .tag = Result<ExtendSyntax*, ParserError*>::Ok, .ok = nullptr };
        }

        auto success_comma_2 = this->lexer.parse_punctuation(_r.page, *String::from_c_string(_r.page, ","));
        if (!success_comma_2) {

        }

        auto end = this->lexer.position;

        auto ret = new(alignof(ExtendSyntax), _rp) ExtendSyntax {
            .start = start,
            .end = end,
            .spec = spec,
        };

        return Result<ExtendSyntax*, ParserError*> { .tag = Result<ExtendSyntax*, ParserError*>::Ok, .ok = ret };
    }

    Result<MacroSyntax*, ParserError*> parse_macro(Region& _pr, Page* _rp, Page* _ep) {
        auto _r = Region::create(_pr);
        auto start = this->lexer.previous_position;

        auto success_macro_1 = this->lexer.parse_keyword(_r.page, *String::from_c_string(_r.page, "macro"));
        if (!success_macro_1) {

                return Result<MacroSyntax*, ParserError*> { .tag = Result<MacroSyntax*, ParserError*>::Ok, .ok = nullptr };
        }

        auto name = this->lexer.parse_identifier(_r.page, this->keywords);
        if (name != nullptr) {
            if (!this->is_identifier(*name)) {
            return Result<MacroSyntax*, ParserError*> { .tag = Result<MacroSyntax*, ParserError*>::Error, .error = new(alignof(ParserError), _ep) ParserError {
                .file_name = file_name,
                .position = lexer.position,
            }};
            }
        }
        else {
            return Result<MacroSyntax*, ParserError*> { .tag = Result<MacroSyntax*, ParserError*>::Error, .error = new(alignof(ParserError), _ep) ParserError {
                .file_name = file_name,
                .position = lexer.position,
            }};
        }

        auto model_result = this->parse_model(_r, _rp, _ep);
        if (model_result.tag == Result<ModelSyntax*, ParserError*>::Error)
            return Result<MacroSyntax*, ParserError*> { .tag = Result<MacroSyntax*, ParserError*>::Error, .error = model_result.error };
        auto model = model_result.ok;
        if (model == nullptr) {
            return Result<MacroSyntax*, ParserError*> { .tag = Result<MacroSyntax*, ParserError*>::Error, .error = new(alignof(ParserError), _ep) ParserError {
                .file_name = file_name,
                .position = lexer.position,
            }};
        }

        auto rule_result = this->parse_operation(_r, _rp, _ep);
        if (rule_result.tag == Result<OperationSyntax*, ParserError*>::Error)
            return Result<MacroSyntax*, ParserError*> { .tag = Result<MacroSyntax*, ParserError*>::Error, .error = rule_result.error };
        auto rule = rule_result.ok;
        if (rule == nullptr) {
            return Result<MacroSyntax*, ParserError*> { .tag = Result<MacroSyntax*, ParserError*>::Error, .error = new(alignof(ParserError), _ep) ParserError {
                .file_name = file_name,
                .position = lexer.position,
            }};
        }

        auto end = this->lexer.position;

        auto ret = new(alignof(MacroSyntax), _rp) MacroSyntax {
            .start = start,
            .end = end,
            .name = name,
            .model = model,
            .rule = rule,
        };

        return Result<MacroSyntax*, ParserError*> { .tag = Result<MacroSyntax*, ParserError*>::Ok, .ok = ret };
    }

    Result<ModuleSyntax*, ParserError*> parse_module(Region& _pr, Page* _rp, Page* _ep) {
        auto _r = Region::create(_pr);
        auto start = this->lexer.previous_position;

        auto success_module_1 = this->lexer.parse_keyword(_r.page, *String::from_c_string(_r.page, "module"));
        if (!success_module_1) {

                return Result<ModuleSyntax*, ParserError*> { .tag = Result<ModuleSyntax*, ParserError*>::Ok, .ok = nullptr };
        }

        auto name_result = this->parse_name(_r, _rp, _ep);
        if (name_result.tag == Result<NameSyntax*, ParserError*>::Error)
            return Result<ModuleSyntax*, ParserError*> { .tag = Result<ModuleSyntax*, ParserError*>::Error, .error = name_result.error };
        auto name = name_result.ok;
        if (name == nullptr) {
            return Result<ModuleSyntax*, ParserError*> { .tag = Result<ModuleSyntax*, ParserError*>::Error, .error = new(alignof(ParserError), _ep) ParserError {
                .file_name = file_name,
                .position = lexer.position,
            }};
        }

        auto success_colon_3 = this->lexer.parse_colon(_r.page);
        if (!success_colon_3) {

        }

        auto end = this->lexer.position;

        auto ret = new(alignof(ModuleSyntax), _rp) ModuleSyntax {
            .start = start,
            .end = end,
            .name = name,
        };

        return Result<ModuleSyntax*, ParserError*> { .tag = Result<ModuleSyntax*, ParserError*>::Ok, .ok = ret };
    }

    Result<ModelSyntax*, ParserError*> parse_model(Region& _pr, Page* _rp, Page* _ep) {
        auto _r = Region::create(_pr);
        {
            auto _r_1 = Region::create(_r);
            auto node_result = this->parse_literal(_r_1, _rp, _ep);
            if (node_result.tag == Result<LiteralSyntax*, ParserError*>::Error)
                return Result<ModelSyntax*, ParserError*> { .tag = Result<ModelSyntax*, ParserError*>::Error, .error = node_result.error };
            auto node = node_result.ok;
            if (node != nullptr) {
                return Result<ModelSyntax*, ParserError*> { .tag = Result<ModelSyntax*, ParserError*>::Ok, .ok = 
                    new (alignof(ModelSyntax), _rp) ModelSyntax {
                        .tag = ModelSyntax::Literal,
                        .literalSyntax = *node,
                    }
                };
            }
        }
        {
            auto _r_1 = Region::create(_r);
            auto node_result = this->parse_name(_r_1, _rp, _ep);
            if (node_result.tag == Result<NameSyntax*, ParserError*>::Error)
                return Result<ModelSyntax*, ParserError*> { .tag = Result<ModelSyntax*, ParserError*>::Error, .error = node_result.error };
            auto node = node_result.ok;
            if (node != nullptr) {
                return Result<ModelSyntax*, ParserError*> { .tag = Result<ModelSyntax*, ParserError*>::Ok, .ok = 
                    new (alignof(ModelSyntax), _rp) ModelSyntax {
                        .tag = ModelSyntax::Name,
                        .nameSyntax = *node,
                    }
                };
            }
        }
        {
            auto _r_1 = Region::create(_r);
            auto node_result = this->parse_object(_r_1, _rp, _ep);
            if (node_result.tag == Result<ObjectSyntax*, ParserError*>::Error)
                return Result<ModelSyntax*, ParserError*> { .tag = Result<ModelSyntax*, ParserError*>::Error, .error = node_result.error };
            auto node = node_result.ok;
            if (node != nullptr) {
                return Result<ModelSyntax*, ParserError*> { .tag = Result<ModelSyntax*, ParserError*>::Ok, .ok = 
                    new (alignof(ModelSyntax), _rp) ModelSyntax {
                        .tag = ModelSyntax::Object,
                        .objectSyntax = *node,
                    }
                };
            }
        }
        {
            auto _r_1 = Region::create(_r);
            auto node_result = this->parse_vector(_r_1, _rp, _ep);
            if (node_result.tag == Result<VectorSyntax*, ParserError*>::Error)
                return Result<ModelSyntax*, ParserError*> { .tag = Result<ModelSyntax*, ParserError*>::Error, .error = node_result.error };
            auto node = node_result.ok;
            if (node != nullptr) {
                return Result<ModelSyntax*, ParserError*> { .tag = Result<ModelSyntax*, ParserError*>::Ok, .ok = 
                    new (alignof(ModelSyntax), _rp) ModelSyntax {
                        .tag = ModelSyntax::Vector,
                        .vectorSyntax = *node,
                    }
                };
            }
        }
        return Result<ModelSyntax*, ParserError*> { .tag = Result<ModelSyntax*, ParserError*>::Ok, .ok = nullptr };
    }

    Result<Vector<StatementSyntax>*, ParserError*> parse_statement_list(Region& _pr, Page* _rp, Page* _ep) {
        auto _r = Region::create(_pr);
        {
            auto _r_1 = Region::create(_r);
            Array<StatementSyntax>* array = nullptr;
            while(true) {
                auto node_result = this->parse_statement(_r_1, _rp, _ep);
                if (node_result.tag == Result<StatementSyntax*, ParserError*>::Error)
                    return Result<Vector<StatementSyntax>*, ParserError*> { .tag = Result<Vector<StatementSyntax>*, ParserError*>::Error, .error = node_result.error };
                auto node = node_result.ok;
                if (node != nullptr) {
                    if (array == nullptr)
                        array = Array<StatementSyntax>::create(_r_1.page);
                    array->add(*node);
                } else {
                    break;
                }
            }

            if (array == nullptr)
                return Result<Vector<StatementSyntax>*, ParserError*> { .tag = Result<Vector<StatementSyntax>*, ParserError*>::Ok, .ok = nullptr };
            
            return Result<Vector<StatementSyntax>*, ParserError*> { .tag = Result<Vector<StatementSyntax>*, ParserError*>::Ok, .ok = Vector<StatementSyntax>::from_array(_rp, *array) };
        }
    }

    Result<StatementSyntax*, ParserError*> parse_statement(Region& _pr, Page* _rp, Page* _ep) {
        auto _r = Region::create(_pr);
        {
            auto _r_1 = Region::create(_r);
            auto node_result = this->parse_operation(_r_1, _rp, _ep);
            if (node_result.tag == Result<OperationSyntax*, ParserError*>::Error)
                return Result<StatementSyntax*, ParserError*> { .tag = Result<StatementSyntax*, ParserError*>::Error, .error = node_result.error };
            auto node = node_result.ok;
            if (node != nullptr) {
                return Result<StatementSyntax*, ParserError*> { .tag = Result<StatementSyntax*, ParserError*>::Ok, .ok = 
                    new (alignof(StatementSyntax), _rp) StatementSyntax {
                        .tag = StatementSyntax::Operation,
                        .operationSyntax = *node,
                    }
                };
            }
        }
        {
            auto _r_1 = Region::create(_r);
            auto node_result = this->parse_let(_r_1, _rp, _ep);
            if (node_result.tag == Result<LetSyntax*, ParserError*>::Error)
                return Result<StatementSyntax*, ParserError*> { .tag = Result<StatementSyntax*, ParserError*>::Error, .error = node_result.error };
            auto node = node_result.ok;
            if (node != nullptr) {
                return Result<StatementSyntax*, ParserError*> { .tag = Result<StatementSyntax*, ParserError*>::Ok, .ok = 
                    new (alignof(StatementSyntax), _rp) StatementSyntax {
                        .tag = StatementSyntax::Let,
                        .letSyntax = *node,
                    }
                };
            }
        }
        {
            auto _r_1 = Region::create(_r);
            auto node_result = this->parse_var(_r_1, _rp, _ep);
            if (node_result.tag == Result<VarSyntax*, ParserError*>::Error)
                return Result<StatementSyntax*, ParserError*> { .tag = Result<StatementSyntax*, ParserError*>::Error, .error = node_result.error };
            auto node = node_result.ok;
            if (node != nullptr) {
                return Result<StatementSyntax*, ParserError*> { .tag = Result<StatementSyntax*, ParserError*>::Ok, .ok = 
                    new (alignof(StatementSyntax), _rp) StatementSyntax {
                        .tag = StatementSyntax::Var,
                        .varSyntax = *node,
                    }
                };
            }
        }
        {
            auto _r_1 = Region::create(_r);
            auto node_result = this->parse_mutable(_r_1, _rp, _ep);
            if (node_result.tag == Result<MutableSyntax*, ParserError*>::Error)
                return Result<StatementSyntax*, ParserError*> { .tag = Result<StatementSyntax*, ParserError*>::Error, .error = node_result.error };
            auto node = node_result.ok;
            if (node != nullptr) {
                return Result<StatementSyntax*, ParserError*> { .tag = Result<StatementSyntax*, ParserError*>::Ok, .ok = 
                    new (alignof(StatementSyntax), _rp) StatementSyntax {
                        .tag = StatementSyntax::Mutable,
                        .mutableSyntax = *node,
                    }
                };
            }
        }
        {
            auto _r_1 = Region::create(_r);
            auto node_result = this->parse_set(_r_1, _rp, _ep);
            if (node_result.tag == Result<SetSyntax*, ParserError*>::Error)
                return Result<StatementSyntax*, ParserError*> { .tag = Result<StatementSyntax*, ParserError*>::Error, .error = node_result.error };
            auto node = node_result.ok;
            if (node != nullptr) {
                return Result<StatementSyntax*, ParserError*> { .tag = Result<StatementSyntax*, ParserError*>::Ok, .ok = 
                    new (alignof(StatementSyntax), _rp) StatementSyntax {
                        .tag = StatementSyntax::Set,
                        .setSyntax = *node,
                    }
                };
            }
        }
        return Result<StatementSyntax*, ParserError*> { .tag = Result<StatementSyntax*, ParserError*>::Ok, .ok = nullptr };
    }

    Result<LetSyntax*, ParserError*> parse_let(Region& _pr, Page* _rp, Page* _ep) {
        auto _r = Region::create(_pr);
        auto start = this->lexer.previous_position;

        auto success_let_1 = this->lexer.parse_keyword(_r.page, *String::from_c_string(_r.page, "let"));
        if (!success_let_1) {

                return Result<LetSyntax*, ParserError*> { .tag = Result<LetSyntax*, ParserError*>::Ok, .ok = nullptr };
        }

        auto binding_result = this->parse_binding(_r, _rp, _ep);
        if (binding_result.tag == Result<BindingSyntax*, ParserError*>::Error)
            return Result<LetSyntax*, ParserError*> { .tag = Result<LetSyntax*, ParserError*>::Error, .error = binding_result.error };
        auto binding = binding_result.ok;
        if (binding == nullptr) {
            return Result<LetSyntax*, ParserError*> { .tag = Result<LetSyntax*, ParserError*>::Error, .error = new(alignof(ParserError), _ep) ParserError {
                .file_name = file_name,
                .position = lexer.position,
            }};
        }

        auto end = this->lexer.position;

        auto ret = new(alignof(LetSyntax), _rp) LetSyntax {
            .start = start,
            .end = end,
            .binding = binding,
        };

        return Result<LetSyntax*, ParserError*> { .tag = Result<LetSyntax*, ParserError*>::Ok, .ok = ret };
    }

    Result<VarSyntax*, ParserError*> parse_var(Region& _pr, Page* _rp, Page* _ep) {
        auto _r = Region::create(_pr);
        auto start = this->lexer.previous_position;

        auto success_var_1 = this->lexer.parse_keyword(_r.page, *String::from_c_string(_r.page, "var"));
        if (!success_var_1) {

                return Result<VarSyntax*, ParserError*> { .tag = Result<VarSyntax*, ParserError*>::Ok, .ok = nullptr };
        }

        auto binding_result = this->parse_binding(_r, _rp, _ep);
        if (binding_result.tag == Result<BindingSyntax*, ParserError*>::Error)
            return Result<VarSyntax*, ParserError*> { .tag = Result<VarSyntax*, ParserError*>::Error, .error = binding_result.error };
        auto binding = binding_result.ok;
        if (binding == nullptr) {
            return Result<VarSyntax*, ParserError*> { .tag = Result<VarSyntax*, ParserError*>::Error, .error = new(alignof(ParserError), _ep) ParserError {
                .file_name = file_name,
                .position = lexer.position,
            }};
        }

        auto end = this->lexer.position;

        auto ret = new(alignof(VarSyntax), _rp) VarSyntax {
            .start = start,
            .end = end,
            .binding = binding,
        };

        return Result<VarSyntax*, ParserError*> { .tag = Result<VarSyntax*, ParserError*>::Ok, .ok = ret };
    }

    Result<MutableSyntax*, ParserError*> parse_mutable(Region& _pr, Page* _rp, Page* _ep) {
        auto _r = Region::create(_pr);
        auto start = this->lexer.previous_position;

        auto success_mutable_1 = this->lexer.parse_keyword(_r.page, *String::from_c_string(_r.page, "mutable"));
        if (!success_mutable_1) {

                return Result<MutableSyntax*, ParserError*> { .tag = Result<MutableSyntax*, ParserError*>::Ok, .ok = nullptr };
        }

        auto binding_result = this->parse_binding(_r, _rp, _ep);
        if (binding_result.tag == Result<BindingSyntax*, ParserError*>::Error)
            return Result<MutableSyntax*, ParserError*> { .tag = Result<MutableSyntax*, ParserError*>::Error, .error = binding_result.error };
        auto binding = binding_result.ok;
        if (binding == nullptr) {
            return Result<MutableSyntax*, ParserError*> { .tag = Result<MutableSyntax*, ParserError*>::Error, .error = new(alignof(ParserError), _ep) ParserError {
                .file_name = file_name,
                .position = lexer.position,
            }};
        }

        auto end = this->lexer.position;

        auto ret = new(alignof(MutableSyntax), _rp) MutableSyntax {
            .start = start,
            .end = end,
            .binding = binding,
        };

        return Result<MutableSyntax*, ParserError*> { .tag = Result<MutableSyntax*, ParserError*>::Ok, .ok = ret };
    }

    Result<BindingSyntax*, ParserError*> parse_binding(Region& _pr, Page* _rp, Page* _ep) {
        auto _r = Region::create(_pr);
        auto start = this->lexer.previous_position;

        auto name = this->lexer.parse_identifier(_r.page, this->keywords);
        if (name != nullptr) {
            if (!this->is_identifier(*name)) {
                return Result<BindingSyntax*, ParserError*> { .tag = Result<BindingSyntax*, ParserError*>::Ok, .ok = nullptr };

            }
        }
        else {
                return Result<BindingSyntax*, ParserError*> { .tag = Result<BindingSyntax*, ParserError*>::Ok, .ok = nullptr };

        }

        auto annotation_result = this->parse_bindingannotation(_r, _rp, _ep);
        if (annotation_result.tag == Result<BindingAnnotationSyntax*, ParserError*>::Error)
            return Result<BindingSyntax*, ParserError*> { .tag = Result<BindingSyntax*, ParserError*>::Error, .error = annotation_result.error };
        auto annotation = annotation_result.ok;

        auto operation_result = this->parse_operation(_r, _rp, _ep);
        if (operation_result.tag == Result<OperationSyntax*, ParserError*>::Error)
            return Result<BindingSyntax*, ParserError*> { .tag = Result<BindingSyntax*, ParserError*>::Error, .error = operation_result.error };
        auto operation = operation_result.ok;
        if (operation == nullptr) {
            return Result<BindingSyntax*, ParserError*> { .tag = Result<BindingSyntax*, ParserError*>::Error, .error = new(alignof(ParserError), _ep) ParserError {
                .file_name = file_name,
                .position = lexer.position,
            }};
        }

        auto end = this->lexer.position;

        auto ret = new(alignof(BindingSyntax), _rp) BindingSyntax {
            .start = start,
            .end = end,
            .name = name,
            .annotation = annotation,
            .operation = operation,
        };

        return Result<BindingSyntax*, ParserError*> { .tag = Result<BindingSyntax*, ParserError*>::Ok, .ok = ret };
    }

    Result<BindingAnnotationSyntax*, ParserError*> parse_bindingannotation(Region& _pr, Page* _rp, Page* _ep) {
        auto _r = Region::create(_pr);
        auto start = this->lexer.previous_position;

        auto success_colon_1 = this->lexer.parse_colon(_r.page);
        if (!success_colon_1) {

                return Result<BindingAnnotationSyntax*, ParserError*> { .tag = Result<BindingAnnotationSyntax*, ParserError*>::Ok, .ok = nullptr };
        }

        auto spec_result = this->parse_bindingspec(_r, _rp, _ep);
        if (spec_result.tag == Result<BindingSpecSyntax*, ParserError*>::Error)
            return Result<BindingAnnotationSyntax*, ParserError*> { .tag = Result<BindingAnnotationSyntax*, ParserError*>::Error, .error = spec_result.error };
        auto spec = spec_result.ok;
        if (spec == nullptr) {
            return Result<BindingAnnotationSyntax*, ParserError*> { .tag = Result<BindingAnnotationSyntax*, ParserError*>::Error, .error = new(alignof(ParserError), _ep) ParserError {
                .file_name = file_name,
                .position = lexer.position,
            }};
        }

        auto end = this->lexer.position;

        auto ret = new(alignof(BindingAnnotationSyntax), _rp) BindingAnnotationSyntax {
            .start = start,
            .end = end,
            .spec = spec,
        };

        return Result<BindingAnnotationSyntax*, ParserError*> { .tag = Result<BindingAnnotationSyntax*, ParserError*>::Ok, .ok = ret };
    }

    Result<Vector<BindingSpecSyntax>*, ParserError*> parse_bindingspec_list(Region& _pr, Page* _rp, Page* _ep) {
        auto _r = Region::create(_pr);
        {
            auto _r_1 = Region::create(_r);
            Array<BindingSpecSyntax>* array = nullptr;
            while(true) {
                auto node_result = this->parse_bindingspec(_r_1, _rp, _ep);
                if (node_result.tag == Result<BindingSpecSyntax*, ParserError*>::Error)
                    return Result<Vector<BindingSpecSyntax>*, ParserError*> { .tag = Result<Vector<BindingSpecSyntax>*, ParserError*>::Error, .error = node_result.error };
                auto node = node_result.ok;
                if (node != nullptr) {
                    if (array == nullptr)
                        array = Array<BindingSpecSyntax>::create(_r_1.page);
                    array->add(*node);
                } else {
                    break;
                }
            }

            if (array == nullptr)
                return Result<Vector<BindingSpecSyntax>*, ParserError*> { .tag = Result<Vector<BindingSpecSyntax>*, ParserError*>::Ok, .ok = nullptr };
            
            return Result<Vector<BindingSpecSyntax>*, ParserError*> { .tag = Result<Vector<BindingSpecSyntax>*, ParserError*>::Ok, .ok = Vector<BindingSpecSyntax>::from_array(_rp, *array) };
        }
    }

    Result<BindingSpecSyntax*, ParserError*> parse_bindingspec(Region& _pr, Page* _rp, Page* _ep) {
        auto _r = Region::create(_pr);
        {
            auto _r_1 = Region::create(_r);
            auto node_result = this->parse_structure(_r_1, _rp, _ep);
            if (node_result.tag == Result<StructureSyntax*, ParserError*>::Error)
                return Result<BindingSpecSyntax*, ParserError*> { .tag = Result<BindingSpecSyntax*, ParserError*>::Error, .error = node_result.error };
            auto node = node_result.ok;
            if (node != nullptr) {
                return Result<BindingSpecSyntax*, ParserError*> { .tag = Result<BindingSpecSyntax*, ParserError*>::Ok, .ok = 
                    new (alignof(BindingSpecSyntax), _rp) BindingSpecSyntax {
                        .tag = BindingSpecSyntax::Structure,
                        .structureSyntax = *node,
                    }
                };
            }
        }
        {
            auto _r_1 = Region::create(_r);
            auto node_result = this->parse_type(_r_1, _rp, _ep);
            if (node_result.tag == Result<TypeSyntax*, ParserError*>::Error)
                return Result<BindingSpecSyntax*, ParserError*> { .tag = Result<BindingSpecSyntax*, ParserError*>::Error, .error = node_result.error };
            auto node = node_result.ok;
            if (node != nullptr) {
                return Result<BindingSpecSyntax*, ParserError*> { .tag = Result<BindingSpecSyntax*, ParserError*>::Ok, .ok = 
                    new (alignof(BindingSpecSyntax), _rp) BindingSpecSyntax {
                        .tag = BindingSpecSyntax::Type,
                        .typeSyntax = *node,
                    }
                };
            }
        }
        {
            auto _r_1 = Region::create(_r);
            auto node_result = this->parse_array(_r_1, _rp, _ep);
            if (node_result.tag == Result<ArraySyntax*, ParserError*>::Error)
                return Result<BindingSpecSyntax*, ParserError*> { .tag = Result<BindingSpecSyntax*, ParserError*>::Error, .error = node_result.error };
            auto node = node_result.ok;
            if (node != nullptr) {
                return Result<BindingSpecSyntax*, ParserError*> { .tag = Result<BindingSpecSyntax*, ParserError*>::Ok, .ok = 
                    new (alignof(BindingSpecSyntax), _rp) BindingSpecSyntax {
                        .tag = BindingSpecSyntax::Array,
                        .arraySyntax = *node,
                    }
                };
            }
        }
        return Result<BindingSpecSyntax*, ParserError*> { .tag = Result<BindingSpecSyntax*, ParserError*>::Ok, .ok = nullptr };
    }

    Result<SetSyntax*, ParserError*> parse_set(Region& _pr, Page* _rp, Page* _ep) {
        auto _r = Region::create(_pr);
        auto start = this->lexer.previous_position;

        auto success_set_1 = this->lexer.parse_keyword(_r.page, *String::from_c_string(_r.page, "set"));
        if (!success_set_1) {

                return Result<SetSyntax*, ParserError*> { .tag = Result<SetSyntax*, ParserError*>::Ok, .ok = nullptr };
        }

        auto target_result = this->parse_operation(_r, _rp, _ep);
        if (target_result.tag == Result<OperationSyntax*, ParserError*>::Error)
            return Result<SetSyntax*, ParserError*> { .tag = Result<SetSyntax*, ParserError*>::Error, .error = target_result.error };
        auto target = target_result.ok;
        if (target == nullptr) {
            return Result<SetSyntax*, ParserError*> { .tag = Result<SetSyntax*, ParserError*>::Error, .error = new(alignof(ParserError), _ep) ParserError {
                .file_name = file_name,
                .position = lexer.position,
            }};
        }

        auto source_result = this->parse_operation(_r, _rp, _ep);
        if (source_result.tag == Result<OperationSyntax*, ParserError*>::Error)
            return Result<SetSyntax*, ParserError*> { .tag = Result<SetSyntax*, ParserError*>::Error, .error = source_result.error };
        auto source = source_result.ok;
        if (source == nullptr) {
            return Result<SetSyntax*, ParserError*> { .tag = Result<SetSyntax*, ParserError*>::Error, .error = new(alignof(ParserError), _ep) ParserError {
                .file_name = file_name,
                .position = lexer.position,
            }};
        }

        auto end = this->lexer.position;

        auto ret = new(alignof(SetSyntax), _rp) SetSyntax {
            .start = start,
            .end = end,
            .target = target,
            .source = source,
        };

        return Result<SetSyntax*, ParserError*> { .tag = Result<SetSyntax*, ParserError*>::Ok, .ok = ret };
    }

    Result<OperationSyntax*, ParserError*> parse_operation(Region& _pr, Page* _rp, Page* _ep) {
        auto _r = Region::create(_pr);
        auto start = this->lexer.previous_position;

        auto operands_result = this->parse_operand_list(_r, _rp, _ep);
        if (operands_result.tag == Result<Vector<OperandSyntax>*, ParserError*>::Error)
            return Result<OperationSyntax*, ParserError*> { .tag = Result<OperationSyntax*, ParserError*>::Error, .error = operands_result.error };
        auto operands = operands_result.ok;
        if (operands == nullptr) {
            return Result<OperationSyntax*, ParserError*> { .tag = Result<OperationSyntax*, ParserError*>::Ok, .ok = nullptr };
        }

        auto success_colon_2 = this->lexer.parse_colon(_r.page);
        if (!success_colon_2) {

        }

        auto end = this->lexer.position;

        auto ret = new(alignof(OperationSyntax), _rp) OperationSyntax {
            .start = start,
            .end = end,
            .operands = operands,
        };

        return Result<OperationSyntax*, ParserError*> { .tag = Result<OperationSyntax*, ParserError*>::Ok, .ok = ret };
    }

    Result<Vector<OperandSyntax>*, ParserError*> parse_operand_list(Region& _pr, Page* _rp, Page* _ep) {
        auto _r = Region::create(_pr);
        {
            auto _r_1 = Region::create(_r);
            Array<OperandSyntax>* array = nullptr;
            while(true) {
                auto node_result = this->parse_operand(_r_1, _rp, _ep);
                if (node_result.tag == Result<OperandSyntax*, ParserError*>::Error)
                    return Result<Vector<OperandSyntax>*, ParserError*> { .tag = Result<Vector<OperandSyntax>*, ParserError*>::Error, .error = node_result.error };
                auto node = node_result.ok;
                if (node != nullptr) {
                    if (array == nullptr)
                        array = Array<OperandSyntax>::create(_r_1.page);
                    array->add(*node);
                } else {
                    break;
                }
            }

            if (array == nullptr)
                return Result<Vector<OperandSyntax>*, ParserError*> { .tag = Result<Vector<OperandSyntax>*, ParserError*>::Ok, .ok = nullptr };
            
            return Result<Vector<OperandSyntax>*, ParserError*> { .tag = Result<Vector<OperandSyntax>*, ParserError*>::Ok, .ok = Vector<OperandSyntax>::from_array(_rp, *array) };
        }
    }

    Result<OperandSyntax*, ParserError*> parse_operand(Region& _pr, Page* _rp, Page* _ep) {
        auto _r = Region::create(_pr);
        auto start = this->lexer.previous_position;

        auto expression_result = this->parse_expression(_r, _rp, _ep);
        if (expression_result.tag == Result<ExpressionSyntax*, ParserError*>::Error)
            return Result<OperandSyntax*, ParserError*> { .tag = Result<OperandSyntax*, ParserError*>::Error, .error = expression_result.error };
        auto expression = expression_result.ok;
        if (expression == nullptr) {
            return Result<OperandSyntax*, ParserError*> { .tag = Result<OperandSyntax*, ParserError*>::Ok, .ok = nullptr };
        }

        auto postfixes_result = this->parse_postfix_list(_r, _rp, _ep);
        if (postfixes_result.tag == Result<Vector<PostfixSyntax>*, ParserError*>::Error)
            return Result<OperandSyntax*, ParserError*> { .tag = Result<OperandSyntax*, ParserError*>::Error, .error = postfixes_result.error };
        auto postfixes = postfixes_result.ok;

        auto end = this->lexer.position;

        auto ret = new(alignof(OperandSyntax), _rp) OperandSyntax {
            .start = start,
            .end = end,
            .expression = expression,
            .postfixes = postfixes,
        };

        return Result<OperandSyntax*, ParserError*> { .tag = Result<OperandSyntax*, ParserError*>::Ok, .ok = ret };
    }

    Result<Vector<PostfixSyntax>*, ParserError*> parse_postfix_list(Region& _pr, Page* _rp, Page* _ep) {
        auto _r = Region::create(_pr);
        {
            auto _r_1 = Region::create(_r);
            Array<PostfixSyntax>* array = nullptr;
            while(true) {
                auto node_result = this->parse_postfix(_r_1, _rp, _ep);
                if (node_result.tag == Result<PostfixSyntax*, ParserError*>::Error)
                    return Result<Vector<PostfixSyntax>*, ParserError*> { .tag = Result<Vector<PostfixSyntax>*, ParserError*>::Error, .error = node_result.error };
                auto node = node_result.ok;
                if (node != nullptr) {
                    if (array == nullptr)
                        array = Array<PostfixSyntax>::create(_r_1.page);
                    array->add(*node);
                } else {
                    break;
                }
            }

            if (array == nullptr)
                return Result<Vector<PostfixSyntax>*, ParserError*> { .tag = Result<Vector<PostfixSyntax>*, ParserError*>::Ok, .ok = nullptr };
            
            return Result<Vector<PostfixSyntax>*, ParserError*> { .tag = Result<Vector<PostfixSyntax>*, ParserError*>::Ok, .ok = Vector<PostfixSyntax>::from_array(_rp, *array) };
        }
    }

    Result<PostfixSyntax*, ParserError*> parse_postfix(Region& _pr, Page* _rp, Page* _ep) {
        auto _r = Region::create(_pr);
        {
            auto _r_1 = Region::create(_r);
            auto node_result = this->parse_memberaccess(_r_1, _rp, _ep);
            if (node_result.tag == Result<MemberAccessSyntax*, ParserError*>::Error)
                return Result<PostfixSyntax*, ParserError*> { .tag = Result<PostfixSyntax*, ParserError*>::Error, .error = node_result.error };
            auto node = node_result.ok;
            if (node != nullptr) {
                return Result<PostfixSyntax*, ParserError*> { .tag = Result<PostfixSyntax*, ParserError*>::Ok, .ok = 
                    new (alignof(PostfixSyntax), _rp) PostfixSyntax {
                        .tag = PostfixSyntax::MemberAccess,
                        .memberAccessSyntax = *node,
                    }
                };
            }
        }
        {
            auto _r_1 = Region::create(_r);
            auto node_result = this->parse_catcher(_r_1, _rp, _ep);
            if (node_result.tag == Result<CatcherSyntax*, ParserError*>::Error)
                return Result<PostfixSyntax*, ParserError*> { .tag = Result<PostfixSyntax*, ParserError*>::Error, .error = node_result.error };
            auto node = node_result.ok;
            if (node != nullptr) {
                return Result<PostfixSyntax*, ParserError*> { .tag = Result<PostfixSyntax*, ParserError*>::Ok, .ok = 
                    new (alignof(PostfixSyntax), _rp) PostfixSyntax {
                        .tag = PostfixSyntax::Catcher,
                        .catcherSyntax = *node,
                    }
                };
            }
        }
        return Result<PostfixSyntax*, ParserError*> { .tag = Result<PostfixSyntax*, ParserError*>::Ok, .ok = nullptr };
    }

    Result<MemberAccessSyntax*, ParserError*> parse_memberaccess(Region& _pr, Page* _rp, Page* _ep) {
        auto _r = Region::create(_pr);
        auto start = this->lexer.previous_position;

        auto success_dot_1 = this->lexer.parse_punctuation(_r.page, *String::from_c_string(_r.page, "."));
        if (!success_dot_1) {

                return Result<MemberAccessSyntax*, ParserError*> { .tag = Result<MemberAccessSyntax*, ParserError*>::Ok, .ok = nullptr };
        }

        auto member_result = this->parse_name(_r, _rp, _ep);
        if (member_result.tag == Result<NameSyntax*, ParserError*>::Error)
            return Result<MemberAccessSyntax*, ParserError*> { .tag = Result<MemberAccessSyntax*, ParserError*>::Error, .error = member_result.error };
        auto member = member_result.ok;
        if (member == nullptr) {
            return Result<MemberAccessSyntax*, ParserError*> { .tag = Result<MemberAccessSyntax*, ParserError*>::Error, .error = new(alignof(ParserError), _ep) ParserError {
                .file_name = file_name,
                .position = lexer.position,
            }};
        }

        auto end = this->lexer.position;

        auto ret = new(alignof(MemberAccessSyntax), _rp) MemberAccessSyntax {
            .start = start,
            .end = end,
            .member = member,
        };

        return Result<MemberAccessSyntax*, ParserError*> { .tag = Result<MemberAccessSyntax*, ParserError*>::Ok, .ok = ret };
    }

    Result<CatcherSyntax*, ParserError*> parse_catcher(Region& _pr, Page* _rp, Page* _ep) {
        auto _r = Region::create(_pr);
        auto start = this->lexer.previous_position;

        auto catchers_result = this->parse_catch_list(_r, _rp, _ep);
        if (catchers_result.tag == Result<Vector<CatchSyntax>*, ParserError*>::Error)
            return Result<CatcherSyntax*, ParserError*> { .tag = Result<CatcherSyntax*, ParserError*>::Error, .error = catchers_result.error };
        auto catchers = catchers_result.ok;
        if (catchers == nullptr) {
            return Result<CatcherSyntax*, ParserError*> { .tag = Result<CatcherSyntax*, ParserError*>::Ok, .ok = nullptr };
        }

        auto dropper_result = this->parse_drop(_r, _rp, _ep);
        if (dropper_result.tag == Result<DropSyntax*, ParserError*>::Error)
            return Result<CatcherSyntax*, ParserError*> { .tag = Result<CatcherSyntax*, ParserError*>::Error, .error = dropper_result.error };
        auto dropper = dropper_result.ok;

        auto end = this->lexer.position;

        auto ret = new(alignof(CatcherSyntax), _rp) CatcherSyntax {
            .start = start,
            .end = end,
            .catchers = catchers,
            .dropper = dropper,
        };

        return Result<CatcherSyntax*, ParserError*> { .tag = Result<CatcherSyntax*, ParserError*>::Ok, .ok = ret };
    }

    Result<Vector<CatchSyntax>*, ParserError*> parse_catch_list(Region& _pr, Page* _rp, Page* _ep) {
        auto _r = Region::create(_pr);
        {
            auto _r_1 = Region::create(_r);
            Array<CatchSyntax>* array = nullptr;
            while(true) {
                auto node_result = this->parse_catch(_r_1, _rp, _ep);
                if (node_result.tag == Result<CatchSyntax*, ParserError*>::Error)
                    return Result<Vector<CatchSyntax>*, ParserError*> { .tag = Result<Vector<CatchSyntax>*, ParserError*>::Error, .error = node_result.error };
                auto node = node_result.ok;
                if (node != nullptr) {
                    if (array == nullptr)
                        array = Array<CatchSyntax>::create(_r_1.page);
                    array->add(*node);
                } else {
                    break;
                }
            }

            if (array == nullptr)
                return Result<Vector<CatchSyntax>*, ParserError*> { .tag = Result<Vector<CatchSyntax>*, ParserError*>::Ok, .ok = nullptr };
            
            return Result<Vector<CatchSyntax>*, ParserError*> { .tag = Result<Vector<CatchSyntax>*, ParserError*>::Ok, .ok = Vector<CatchSyntax>::from_array(_rp, *array) };
        }
    }

    Result<CatchSyntax*, ParserError*> parse_catch(Region& _pr, Page* _rp, Page* _ep) {
        auto _r = Region::create(_pr);
        auto start = this->lexer.previous_position;

        auto success_catch_1 = this->lexer.parse_keyword(_r.page, *String::from_c_string(_r.page, "catch"));
        if (!success_catch_1) {

                return Result<CatchSyntax*, ParserError*> { .tag = Result<CatchSyntax*, ParserError*>::Ok, .ok = nullptr };
        }

        auto condition_result = this->parse_operation(_r, _rp, _ep);
        if (condition_result.tag == Result<OperationSyntax*, ParserError*>::Error)
            return Result<CatchSyntax*, ParserError*> { .tag = Result<CatchSyntax*, ParserError*>::Error, .error = condition_result.error };
        auto condition = condition_result.ok;
        if (condition == nullptr) {
            return Result<CatchSyntax*, ParserError*> { .tag = Result<CatchSyntax*, ParserError*>::Error, .error = new(alignof(ParserError), _ep) ParserError {
                .file_name = file_name,
                .position = lexer.position,
            }};
        }

        auto handler_result = this->parse_operation(_r, _rp, _ep);
        if (handler_result.tag == Result<OperationSyntax*, ParserError*>::Error)
            return Result<CatchSyntax*, ParserError*> { .tag = Result<CatchSyntax*, ParserError*>::Error, .error = handler_result.error };
        auto handler = handler_result.ok;
        if (handler == nullptr) {
            return Result<CatchSyntax*, ParserError*> { .tag = Result<CatchSyntax*, ParserError*>::Error, .error = new(alignof(ParserError), _ep) ParserError {
                .file_name = file_name,
                .position = lexer.position,
            }};
        }

        auto end = this->lexer.position;

        auto ret = new(alignof(CatchSyntax), _rp) CatchSyntax {
            .start = start,
            .end = end,
            .condition = condition,
            .handler = handler,
        };

        return Result<CatchSyntax*, ParserError*> { .tag = Result<CatchSyntax*, ParserError*>::Ok, .ok = ret };
    }

    Result<DropSyntax*, ParserError*> parse_drop(Region& _pr, Page* _rp, Page* _ep) {
        auto _r = Region::create(_pr);
        auto start = this->lexer.previous_position;

        auto success_drop_1 = this->lexer.parse_keyword(_r.page, *String::from_c_string(_r.page, "drop"));
        if (!success_drop_1) {

                return Result<DropSyntax*, ParserError*> { .tag = Result<DropSyntax*, ParserError*>::Ok, .ok = nullptr };
        }

        auto handler_result = this->parse_operation(_r, _rp, _ep);
        if (handler_result.tag == Result<OperationSyntax*, ParserError*>::Error)
            return Result<DropSyntax*, ParserError*> { .tag = Result<DropSyntax*, ParserError*>::Error, .error = handler_result.error };
        auto handler = handler_result.ok;
        if (handler == nullptr) {
            return Result<DropSyntax*, ParserError*> { .tag = Result<DropSyntax*, ParserError*>::Error, .error = new(alignof(ParserError), _ep) ParserError {
                .file_name = file_name,
                .position = lexer.position,
            }};
        }

        auto end = this->lexer.position;

        auto ret = new(alignof(DropSyntax), _rp) DropSyntax {
            .start = start,
            .end = end,
            .handler = handler,
        };

        return Result<DropSyntax*, ParserError*> { .tag = Result<DropSyntax*, ParserError*>::Ok, .ok = ret };
    }

    Result<ContinueSyntax*, ParserError*> parse_continue(Region& _pr, Page* _rp, Page* _ep) {
        auto _r = Region::create(_pr);
        auto start = this->lexer.previous_position;

        auto success_continue_1 = this->lexer.parse_keyword(_r.page, *String::from_c_string(_r.page, "continue"));
        if (!success_continue_1) {

                return Result<ContinueSyntax*, ParserError*> { .tag = Result<ContinueSyntax*, ParserError*>::Ok, .ok = nullptr };
        }

        auto name_result = this->parse_loop(_r, _rp, _ep);
        if (name_result.tag == Result<LoopSyntax*, ParserError*>::Error)
            return Result<ContinueSyntax*, ParserError*> { .tag = Result<ContinueSyntax*, ParserError*>::Error, .error = name_result.error };
        auto name = name_result.ok;

        auto success_colon_3 = this->lexer.parse_colon(_r.page);
        if (!success_colon_3) {

        }

        auto end = this->lexer.position;

        auto ret = new(alignof(ContinueSyntax), _rp) ContinueSyntax {
            .start = start,
            .end = end,
            .name = name,
        };

        return Result<ContinueSyntax*, ParserError*> { .tag = Result<ContinueSyntax*, ParserError*>::Ok, .ok = ret };
    }

    Result<LoopSyntax*, ParserError*> parse_loop(Region& _pr, Page* _rp, Page* _ep) {
        auto _r = Region::create(_pr);
        auto start = this->lexer.previous_position;

        auto success_loop_1 = this->lexer.parse_keyword(_r.page, *String::from_c_string(_r.page, "loop"));
        if (!success_loop_1) {

                return Result<LoopSyntax*, ParserError*> { .tag = Result<LoopSyntax*, ParserError*>::Ok, .ok = nullptr };
        }

        auto name = this->lexer.parse_identifier(_r.page, this->keywords);
        if (name != nullptr) {
            if (!this->is_identifier(*name)) {
            return Result<LoopSyntax*, ParserError*> { .tag = Result<LoopSyntax*, ParserError*>::Error, .error = new(alignof(ParserError), _ep) ParserError {
                .file_name = file_name,
                .position = lexer.position,
            }};
            }
        }
        else {
            return Result<LoopSyntax*, ParserError*> { .tag = Result<LoopSyntax*, ParserError*>::Error, .error = new(alignof(ParserError), _ep) ParserError {
                .file_name = file_name,
                .position = lexer.position,
            }};
        }

        auto end = this->lexer.position;

        auto ret = new(alignof(LoopSyntax), _rp) LoopSyntax {
            .start = start,
            .end = end,
            .name = name,
        };

        return Result<LoopSyntax*, ParserError*> { .tag = Result<LoopSyntax*, ParserError*>::Ok, .ok = ret };
    }

    Result<BreakSyntax*, ParserError*> parse_break(Region& _pr, Page* _rp, Page* _ep) {
        auto _r = Region::create(_pr);
        auto start = this->lexer.previous_position;

        auto success_break_1 = this->lexer.parse_keyword(_r.page, *String::from_c_string(_r.page, "break"));
        if (!success_break_1) {

                return Result<BreakSyntax*, ParserError*> { .tag = Result<BreakSyntax*, ParserError*>::Ok, .ok = nullptr };
        }

        auto name_result = this->parse_loop(_r, _rp, _ep);
        if (name_result.tag == Result<LoopSyntax*, ParserError*>::Error)
            return Result<BreakSyntax*, ParserError*> { .tag = Result<BreakSyntax*, ParserError*>::Error, .error = name_result.error };
        auto name = name_result.ok;

        auto result_result = this->parse_operation(_r, _rp, _ep);
        if (result_result.tag == Result<OperationSyntax*, ParserError*>::Error)
            return Result<BreakSyntax*, ParserError*> { .tag = Result<BreakSyntax*, ParserError*>::Error, .error = result_result.error };
        auto result = result_result.ok;

        auto success_colon_4 = this->lexer.parse_colon(_r.page);
        if (!success_colon_4) {

        }

        auto end = this->lexer.position;

        auto ret = new(alignof(BreakSyntax), _rp) BreakSyntax {
            .start = start,
            .end = end,
            .name = name,
            .result = result,
        };

        return Result<BreakSyntax*, ParserError*> { .tag = Result<BreakSyntax*, ParserError*>::Ok, .ok = ret };
    }

    Result<ReturnSyntax*, ParserError*> parse_return(Region& _pr, Page* _rp, Page* _ep) {
        auto _r = Region::create(_pr);
        auto start = this->lexer.previous_position;

        auto success_return_1 = this->lexer.parse_keyword(_r.page, *String::from_c_string(_r.page, "return"));
        if (!success_return_1) {

                return Result<ReturnSyntax*, ParserError*> { .tag = Result<ReturnSyntax*, ParserError*>::Ok, .ok = nullptr };
        }

        auto result_result = this->parse_operation(_r, _rp, _ep);
        if (result_result.tag == Result<OperationSyntax*, ParserError*>::Error)
            return Result<ReturnSyntax*, ParserError*> { .tag = Result<ReturnSyntax*, ParserError*>::Error, .error = result_result.error };
        auto result = result_result.ok;

        auto end = this->lexer.position;

        auto ret = new(alignof(ReturnSyntax), _rp) ReturnSyntax {
            .start = start,
            .end = end,
            .result = result,
        };

        return Result<ReturnSyntax*, ParserError*> { .tag = Result<ReturnSyntax*, ParserError*>::Ok, .ok = ret };
    }

    Result<ThrowSyntax*, ParserError*> parse_throw(Region& _pr, Page* _rp, Page* _ep) {
        auto _r = Region::create(_pr);
        auto start = this->lexer.previous_position;

        auto success_throw_1 = this->lexer.parse_keyword(_r.page, *String::from_c_string(_r.page, "throw"));
        if (!success_throw_1) {

                return Result<ThrowSyntax*, ParserError*> { .tag = Result<ThrowSyntax*, ParserError*>::Ok, .ok = nullptr };
        }

        auto result_result = this->parse_operation(_r, _rp, _ep);
        if (result_result.tag == Result<OperationSyntax*, ParserError*>::Error)
            return Result<ThrowSyntax*, ParserError*> { .tag = Result<ThrowSyntax*, ParserError*>::Error, .error = result_result.error };
        auto result = result_result.ok;

        auto end = this->lexer.position;

        auto ret = new(alignof(ThrowSyntax), _rp) ThrowSyntax {
            .start = start,
            .end = end,
            .result = result,
        };

        return Result<ThrowSyntax*, ParserError*> { .tag = Result<ThrowSyntax*, ParserError*>::Ok, .ok = ret };
    }

    Result<ExpressionSyntax*, ParserError*> parse_expression(Region& _pr, Page* _rp, Page* _ep) {
        auto _r = Region::create(_pr);
        {
            auto _r_1 = Region::create(_r);
            auto node_result = this->parse_literal(_r_1, _rp, _ep);
            if (node_result.tag == Result<LiteralSyntax*, ParserError*>::Error)
                return Result<ExpressionSyntax*, ParserError*> { .tag = Result<ExpressionSyntax*, ParserError*>::Error, .error = node_result.error };
            auto node = node_result.ok;
            if (node != nullptr) {
                return Result<ExpressionSyntax*, ParserError*> { .tag = Result<ExpressionSyntax*, ParserError*>::Ok, .ok = 
                    new (alignof(ExpressionSyntax), _rp) ExpressionSyntax {
                        .tag = ExpressionSyntax::Literal,
                        .literalSyntax = *node,
                    }
                };
            }
        }
        {
            auto _r_1 = Region::create(_r);
            auto node_result = this->parse_name(_r_1, _rp, _ep);
            if (node_result.tag == Result<NameSyntax*, ParserError*>::Error)
                return Result<ExpressionSyntax*, ParserError*> { .tag = Result<ExpressionSyntax*, ParserError*>::Error, .error = node_result.error };
            auto node = node_result.ok;
            if (node != nullptr) {
                return Result<ExpressionSyntax*, ParserError*> { .tag = Result<ExpressionSyntax*, ParserError*>::Ok, .ok = 
                    new (alignof(ExpressionSyntax), _rp) ExpressionSyntax {
                        .tag = ExpressionSyntax::Name,
                        .nameSyntax = *node,
                    }
                };
            }
        }
        {
            auto _r_1 = Region::create(_r);
            auto node_result = this->parse_object(_r_1, _rp, _ep);
            if (node_result.tag == Result<ObjectSyntax*, ParserError*>::Error)
                return Result<ExpressionSyntax*, ParserError*> { .tag = Result<ExpressionSyntax*, ParserError*>::Error, .error = node_result.error };
            auto node = node_result.ok;
            if (node != nullptr) {
                return Result<ExpressionSyntax*, ParserError*> { .tag = Result<ExpressionSyntax*, ParserError*>::Ok, .ok = 
                    new (alignof(ExpressionSyntax), _rp) ExpressionSyntax {
                        .tag = ExpressionSyntax::Object,
                        .objectSyntax = *node,
                    }
                };
            }
        }
        {
            auto _r_1 = Region::create(_r);
            auto node_result = this->parse_vector(_r_1, _rp, _ep);
            if (node_result.tag == Result<VectorSyntax*, ParserError*>::Error)
                return Result<ExpressionSyntax*, ParserError*> { .tag = Result<ExpressionSyntax*, ParserError*>::Error, .error = node_result.error };
            auto node = node_result.ok;
            if (node != nullptr) {
                return Result<ExpressionSyntax*, ParserError*> { .tag = Result<ExpressionSyntax*, ParserError*>::Ok, .ok = 
                    new (alignof(ExpressionSyntax), _rp) ExpressionSyntax {
                        .tag = ExpressionSyntax::Vector,
                        .vectorSyntax = *node,
                    }
                };
            }
        }
        {
            auto _r_1 = Region::create(_r);
            auto node_result = this->parse_block(_r_1, _rp, _ep);
            if (node_result.tag == Result<BlockSyntax*, ParserError*>::Error)
                return Result<ExpressionSyntax*, ParserError*> { .tag = Result<ExpressionSyntax*, ParserError*>::Error, .error = node_result.error };
            auto node = node_result.ok;
            if (node != nullptr) {
                return Result<ExpressionSyntax*, ParserError*> { .tag = Result<ExpressionSyntax*, ParserError*>::Ok, .ok = 
                    new (alignof(ExpressionSyntax), _rp) ExpressionSyntax {
                        .tag = ExpressionSyntax::Block,
                        .blockSyntax = *node,
                    }
                };
            }
        }
        {
            auto _r_1 = Region::create(_r);
            auto node_result = this->parse_if(_r_1, _rp, _ep);
            if (node_result.tag == Result<IfSyntax*, ParserError*>::Error)
                return Result<ExpressionSyntax*, ParserError*> { .tag = Result<ExpressionSyntax*, ParserError*>::Error, .error = node_result.error };
            auto node = node_result.ok;
            if (node != nullptr) {
                return Result<ExpressionSyntax*, ParserError*> { .tag = Result<ExpressionSyntax*, ParserError*>::Ok, .ok = 
                    new (alignof(ExpressionSyntax), _rp) ExpressionSyntax {
                        .tag = ExpressionSyntax::If,
                        .ifSyntax = *node,
                    }
                };
            }
        }
        {
            auto _r_1 = Region::create(_r);
            auto node_result = this->parse_match(_r_1, _rp, _ep);
            if (node_result.tag == Result<MatchSyntax*, ParserError*>::Error)
                return Result<ExpressionSyntax*, ParserError*> { .tag = Result<ExpressionSyntax*, ParserError*>::Error, .error = node_result.error };
            auto node = node_result.ok;
            if (node != nullptr) {
                return Result<ExpressionSyntax*, ParserError*> { .tag = Result<ExpressionSyntax*, ParserError*>::Ok, .ok = 
                    new (alignof(ExpressionSyntax), _rp) ExpressionSyntax {
                        .tag = ExpressionSyntax::Match,
                        .matchSyntax = *node,
                    }
                };
            }
        }
        {
            auto _r_1 = Region::create(_r);
            auto node_result = this->parse_lambda(_r_1, _rp, _ep);
            if (node_result.tag == Result<LambdaSyntax*, ParserError*>::Error)
                return Result<ExpressionSyntax*, ParserError*> { .tag = Result<ExpressionSyntax*, ParserError*>::Error, .error = node_result.error };
            auto node = node_result.ok;
            if (node != nullptr) {
                return Result<ExpressionSyntax*, ParserError*> { .tag = Result<ExpressionSyntax*, ParserError*>::Ok, .ok = 
                    new (alignof(ExpressionSyntax), _rp) ExpressionSyntax {
                        .tag = ExpressionSyntax::Lambda,
                        .lambdaSyntax = *node,
                    }
                };
            }
        }
        {
            auto _r_1 = Region::create(_r);
            auto node_result = this->parse_for(_r_1, _rp, _ep);
            if (node_result.tag == Result<ForSyntax*, ParserError*>::Error)
                return Result<ExpressionSyntax*, ParserError*> { .tag = Result<ExpressionSyntax*, ParserError*>::Error, .error = node_result.error };
            auto node = node_result.ok;
            if (node != nullptr) {
                return Result<ExpressionSyntax*, ParserError*> { .tag = Result<ExpressionSyntax*, ParserError*>::Ok, .ok = 
                    new (alignof(ExpressionSyntax), _rp) ExpressionSyntax {
                        .tag = ExpressionSyntax::For,
                        .forSyntax = *node,
                    }
                };
            }
        }
        {
            auto _r_1 = Region::create(_r);
            auto node_result = this->parse_while(_r_1, _rp, _ep);
            if (node_result.tag == Result<WhileSyntax*, ParserError*>::Error)
                return Result<ExpressionSyntax*, ParserError*> { .tag = Result<ExpressionSyntax*, ParserError*>::Error, .error = node_result.error };
            auto node = node_result.ok;
            if (node != nullptr) {
                return Result<ExpressionSyntax*, ParserError*> { .tag = Result<ExpressionSyntax*, ParserError*>::Ok, .ok = 
                    new (alignof(ExpressionSyntax), _rp) ExpressionSyntax {
                        .tag = ExpressionSyntax::While,
                        .whileSyntax = *node,
                    }
                };
            }
        }
        {
            auto _r_1 = Region::create(_r);
            auto node_result = this->parse_repeat(_r_1, _rp, _ep);
            if (node_result.tag == Result<RepeatSyntax*, ParserError*>::Error)
                return Result<ExpressionSyntax*, ParserError*> { .tag = Result<ExpressionSyntax*, ParserError*>::Error, .error = node_result.error };
            auto node = node_result.ok;
            if (node != nullptr) {
                return Result<ExpressionSyntax*, ParserError*> { .tag = Result<ExpressionSyntax*, ParserError*>::Ok, .ok = 
                    new (alignof(ExpressionSyntax), _rp) ExpressionSyntax {
                        .tag = ExpressionSyntax::Repeat,
                        .repeatSyntax = *node,
                    }
                };
            }
        }
        {
            auto _r_1 = Region::create(_r);
            auto node_result = this->parse_sizeof(_r_1, _rp, _ep);
            if (node_result.tag == Result<SizeOfSyntax*, ParserError*>::Error)
                return Result<ExpressionSyntax*, ParserError*> { .tag = Result<ExpressionSyntax*, ParserError*>::Error, .error = node_result.error };
            auto node = node_result.ok;
            if (node != nullptr) {
                return Result<ExpressionSyntax*, ParserError*> { .tag = Result<ExpressionSyntax*, ParserError*>::Ok, .ok = 
                    new (alignof(ExpressionSyntax), _rp) ExpressionSyntax {
                        .tag = ExpressionSyntax::SizeOf,
                        .sizeOfSyntax = *node,
                    }
                };
            }
        }
        {
            auto _r_1 = Region::create(_r);
            auto node_result = this->parse_continue(_r_1, _rp, _ep);
            if (node_result.tag == Result<ContinueSyntax*, ParserError*>::Error)
                return Result<ExpressionSyntax*, ParserError*> { .tag = Result<ExpressionSyntax*, ParserError*>::Error, .error = node_result.error };
            auto node = node_result.ok;
            if (node != nullptr) {
                return Result<ExpressionSyntax*, ParserError*> { .tag = Result<ExpressionSyntax*, ParserError*>::Ok, .ok = 
                    new (alignof(ExpressionSyntax), _rp) ExpressionSyntax {
                        .tag = ExpressionSyntax::Continue,
                        .continueSyntax = *node,
                    }
                };
            }
        }
        {
            auto _r_1 = Region::create(_r);
            auto node_result = this->parse_break(_r_1, _rp, _ep);
            if (node_result.tag == Result<BreakSyntax*, ParserError*>::Error)
                return Result<ExpressionSyntax*, ParserError*> { .tag = Result<ExpressionSyntax*, ParserError*>::Error, .error = node_result.error };
            auto node = node_result.ok;
            if (node != nullptr) {
                return Result<ExpressionSyntax*, ParserError*> { .tag = Result<ExpressionSyntax*, ParserError*>::Ok, .ok = 
                    new (alignof(ExpressionSyntax), _rp) ExpressionSyntax {
                        .tag = ExpressionSyntax::Break,
                        .breakSyntax = *node,
                    }
                };
            }
        }
        {
            auto _r_1 = Region::create(_r);
            auto node_result = this->parse_return(_r_1, _rp, _ep);
            if (node_result.tag == Result<ReturnSyntax*, ParserError*>::Error)
                return Result<ExpressionSyntax*, ParserError*> { .tag = Result<ExpressionSyntax*, ParserError*>::Error, .error = node_result.error };
            auto node = node_result.ok;
            if (node != nullptr) {
                return Result<ExpressionSyntax*, ParserError*> { .tag = Result<ExpressionSyntax*, ParserError*>::Ok, .ok = 
                    new (alignof(ExpressionSyntax), _rp) ExpressionSyntax {
                        .tag = ExpressionSyntax::Return,
                        .returnSyntax = *node,
                    }
                };
            }
        }
        {
            auto _r_1 = Region::create(_r);
            auto node_result = this->parse_throw(_r_1, _rp, _ep);
            if (node_result.tag == Result<ThrowSyntax*, ParserError*>::Error)
                return Result<ExpressionSyntax*, ParserError*> { .tag = Result<ExpressionSyntax*, ParserError*>::Error, .error = node_result.error };
            auto node = node_result.ok;
            if (node != nullptr) {
                return Result<ExpressionSyntax*, ParserError*> { .tag = Result<ExpressionSyntax*, ParserError*>::Ok, .ok = 
                    new (alignof(ExpressionSyntax), _rp) ExpressionSyntax {
                        .tag = ExpressionSyntax::Throw,
                        .throwSyntax = *node,
                    }
                };
            }
        }
        return Result<ExpressionSyntax*, ParserError*> { .tag = Result<ExpressionSyntax*, ParserError*>::Ok, .ok = nullptr };
    }

    Result<LiteralSyntax*, ParserError*> parse_literal(Region& _pr, Page* _rp, Page* _ep) {
        auto _r = Region::create(_pr);
        auto start = this->lexer.previous_position;

        auto literal = this->lexer.parse_literal(_r.page);
        if (literal == nullptr) {

                return Result<LiteralSyntax*, ParserError*> { .tag = Result<LiteralSyntax*, ParserError*>::Ok, .ok = nullptr };
        }

        auto end = this->lexer.position;

        auto ret = new(alignof(LiteralSyntax), _rp) LiteralSyntax {
            .start = start,
            .end = end,
            .literal = *literal,
        };

        return Result<LiteralSyntax*, ParserError*> { .tag = Result<LiteralSyntax*, ParserError*>::Ok, .ok = ret };
    }

    Result<ObjectSyntax*, ParserError*> parse_object(Region& _pr, Page* _rp, Page* _ep) {
        auto _r = Region::create(_pr);
        auto start = this->lexer.previous_position;

        auto success_left_paren_1 = this->lexer.parse_punctuation(_r.page, *String::from_c_string(_r.page, "("));
        if (!success_left_paren_1) {

                return Result<ObjectSyntax*, ParserError*> { .tag = Result<ObjectSyntax*, ParserError*>::Ok, .ok = nullptr };
        }

        auto components_result = this->parse_component_list(_r, _rp, _ep);
        if (components_result.tag == Result<Vector<ComponentSyntax>*, ParserError*>::Error)
            return Result<ObjectSyntax*, ParserError*> { .tag = Result<ObjectSyntax*, ParserError*>::Error, .error = components_result.error };
        auto components = components_result.ok;

        auto success_right_paren_3 = this->lexer.parse_punctuation(_r.page, *String::from_c_string(_r.page, ")"));
        if (!success_right_paren_3) {

            return Result<ObjectSyntax*, ParserError*> { .tag = Result<ObjectSyntax*, ParserError*>::Error, .error = new(alignof(ParserError), _ep) ParserError {
                .file_name = file_name,
                .position = lexer.position,
            }};        }

        auto end = this->lexer.position;

        auto ret = new(alignof(ObjectSyntax), _rp) ObjectSyntax {
            .start = start,
            .end = end,
            .components = components,
        };

        return Result<ObjectSyntax*, ParserError*> { .tag = Result<ObjectSyntax*, ParserError*>::Ok, .ok = ret };
    }

    Result<Vector<ComponentSyntax>*, ParserError*> parse_component_list(Region& _pr, Page* _rp, Page* _ep) {
        auto _r = Region::create(_pr);
        {
            auto _r_1 = Region::create(_r);
            Array<ComponentSyntax>* array = nullptr;
            while(true) {
                auto node_result = this->parse_component(_r_1, _rp, _ep);
                if (node_result.tag == Result<ComponentSyntax*, ParserError*>::Error)
                    return Result<Vector<ComponentSyntax>*, ParserError*> { .tag = Result<Vector<ComponentSyntax>*, ParserError*>::Error, .error = node_result.error };
                auto node = node_result.ok;
                if (node != nullptr) {
                    if (array == nullptr)
                        array = Array<ComponentSyntax>::create(_r_1.page);
                    array->add(*node);
                } else {
                    break;
                }
            }

            if (array == nullptr)
                return Result<Vector<ComponentSyntax>*, ParserError*> { .tag = Result<Vector<ComponentSyntax>*, ParserError*>::Ok, .ok = nullptr };
            
            return Result<Vector<ComponentSyntax>*, ParserError*> { .tag = Result<Vector<ComponentSyntax>*, ParserError*>::Ok, .ok = Vector<ComponentSyntax>::from_array(_rp, *array) };
        }
    }

    Result<ComponentSyntax*, ParserError*> parse_component(Region& _pr, Page* _rp, Page* _ep) {
        auto _r = Region::create(_pr);
        auto start = this->lexer.previous_position;

        auto operands_result = this->parse_operand_list(_r, _rp, _ep);
        if (operands_result.tag == Result<Vector<OperandSyntax>*, ParserError*>::Error)
            return Result<ComponentSyntax*, ParserError*> { .tag = Result<ComponentSyntax*, ParserError*>::Error, .error = operands_result.error };
        auto operands = operands_result.ok;
        if (operands == nullptr) {
            return Result<ComponentSyntax*, ParserError*> { .tag = Result<ComponentSyntax*, ParserError*>::Ok, .ok = nullptr };
        }

        auto attributes_result = this->parse_attribute_list(_r, _rp, _ep);
        if (attributes_result.tag == Result<Vector<AttributeSyntax>*, ParserError*>::Error)
            return Result<ComponentSyntax*, ParserError*> { .tag = Result<ComponentSyntax*, ParserError*>::Error, .error = attributes_result.error };
        auto attributes = attributes_result.ok;

        auto value_result = this->parse_value(_r, _rp, _ep);
        if (value_result.tag == Result<ValueSyntax*, ParserError*>::Error)
            return Result<ComponentSyntax*, ParserError*> { .tag = Result<ComponentSyntax*, ParserError*>::Error, .error = value_result.error };
        auto value = value_result.ok;

        auto success_comma_4 = this->lexer.parse_punctuation(_r.page, *String::from_c_string(_r.page, ","));
        if (!success_comma_4) {

        }

        auto end = this->lexer.position;

        auto ret = new(alignof(ComponentSyntax), _rp) ComponentSyntax {
            .start = start,
            .end = end,
            .operands = operands,
            .attributes = attributes,
            .value = value,
        };

        return Result<ComponentSyntax*, ParserError*> { .tag = Result<ComponentSyntax*, ParserError*>::Ok, .ok = ret };
    }

    Result<ValueSyntax*, ParserError*> parse_value(Region& _pr, Page* _rp, Page* _ep) {
        auto _r = Region::create(_pr);
        auto start = this->lexer.previous_position;

        auto success_colon_1 = this->lexer.parse_colon(_r.page);
        if (!success_colon_1) {

                return Result<ValueSyntax*, ParserError*> { .tag = Result<ValueSyntax*, ParserError*>::Ok, .ok = nullptr };
        }

        auto value_result = this->parse_operation(_r, _rp, _ep);
        if (value_result.tag == Result<OperationSyntax*, ParserError*>::Error)
            return Result<ValueSyntax*, ParserError*> { .tag = Result<ValueSyntax*, ParserError*>::Error, .error = value_result.error };
        auto value = value_result.ok;
        if (value == nullptr) {
            return Result<ValueSyntax*, ParserError*> { .tag = Result<ValueSyntax*, ParserError*>::Error, .error = new(alignof(ParserError), _ep) ParserError {
                .file_name = file_name,
                .position = lexer.position,
            }};
        }

        auto attributes_result = this->parse_attribute_list(_r, _rp, _ep);
        if (attributes_result.tag == Result<Vector<AttributeSyntax>*, ParserError*>::Error)
            return Result<ValueSyntax*, ParserError*> { .tag = Result<ValueSyntax*, ParserError*>::Error, .error = attributes_result.error };
        auto attributes = attributes_result.ok;

        auto end = this->lexer.position;

        auto ret = new(alignof(ValueSyntax), _rp) ValueSyntax {
            .start = start,
            .end = end,
            .value = value,
            .attributes = attributes,
        };

        return Result<ValueSyntax*, ParserError*> { .tag = Result<ValueSyntax*, ParserError*>::Ok, .ok = ret };
    }

    Result<VectorSyntax*, ParserError*> parse_vector(Region& _pr, Page* _rp, Page* _ep) {
        auto _r = Region::create(_pr);
        auto start = this->lexer.previous_position;

        auto success_left_bracket_1 = this->lexer.parse_punctuation(_r.page, *String::from_c_string(_r.page, "["));
        if (!success_left_bracket_1) {

                return Result<VectorSyntax*, ParserError*> { .tag = Result<VectorSyntax*, ParserError*>::Ok, .ok = nullptr };
        }

        auto elements_result = this->parse_element_list(_r, _rp, _ep);
        if (elements_result.tag == Result<Vector<ElementSyntax>*, ParserError*>::Error)
            return Result<VectorSyntax*, ParserError*> { .tag = Result<VectorSyntax*, ParserError*>::Error, .error = elements_result.error };
        auto elements = elements_result.ok;
        if (elements == nullptr) {
            return Result<VectorSyntax*, ParserError*> { .tag = Result<VectorSyntax*, ParserError*>::Error, .error = new(alignof(ParserError), _ep) ParserError {
                .file_name = file_name,
                .position = lexer.position,
            }};
        }

        auto success_right_bracket_3 = this->lexer.parse_punctuation(_r.page, *String::from_c_string(_r.page, "]"));
        if (!success_right_bracket_3) {

            return Result<VectorSyntax*, ParserError*> { .tag = Result<VectorSyntax*, ParserError*>::Error, .error = new(alignof(ParserError), _ep) ParserError {
                .file_name = file_name,
                .position = lexer.position,
            }};        }

        auto end = this->lexer.position;

        auto ret = new(alignof(VectorSyntax), _rp) VectorSyntax {
            .start = start,
            .end = end,
            .elements = elements,
        };

        return Result<VectorSyntax*, ParserError*> { .tag = Result<VectorSyntax*, ParserError*>::Ok, .ok = ret };
    }

    Result<Vector<ElementSyntax>*, ParserError*> parse_element_list(Region& _pr, Page* _rp, Page* _ep) {
        auto _r = Region::create(_pr);
        {
            auto _r_1 = Region::create(_r);
            Array<ElementSyntax>* array = nullptr;
            while(true) {
                auto node_result = this->parse_element(_r_1, _rp, _ep);
                if (node_result.tag == Result<ElementSyntax*, ParserError*>::Error)
                    return Result<Vector<ElementSyntax>*, ParserError*> { .tag = Result<Vector<ElementSyntax>*, ParserError*>::Error, .error = node_result.error };
                auto node = node_result.ok;
                if (node != nullptr) {
                    if (array == nullptr)
                        array = Array<ElementSyntax>::create(_r_1.page);
                    array->add(*node);
                } else {
                    break;
                }
            }

            if (array == nullptr)
                return Result<Vector<ElementSyntax>*, ParserError*> { .tag = Result<Vector<ElementSyntax>*, ParserError*>::Ok, .ok = nullptr };
            
            return Result<Vector<ElementSyntax>*, ParserError*> { .tag = Result<Vector<ElementSyntax>*, ParserError*>::Ok, .ok = Vector<ElementSyntax>::from_array(_rp, *array) };
        }
    }

    Result<ElementSyntax*, ParserError*> parse_element(Region& _pr, Page* _rp, Page* _ep) {
        auto _r = Region::create(_pr);
        auto start = this->lexer.previous_position;

        auto operation_result = this->parse_operation(_r, _rp, _ep);
        if (operation_result.tag == Result<OperationSyntax*, ParserError*>::Error)
            return Result<ElementSyntax*, ParserError*> { .tag = Result<ElementSyntax*, ParserError*>::Error, .error = operation_result.error };
        auto operation = operation_result.ok;
        if (operation == nullptr) {
            return Result<ElementSyntax*, ParserError*> { .tag = Result<ElementSyntax*, ParserError*>::Ok, .ok = nullptr };
        }

        auto attributes_result = this->parse_attribute_list(_r, _rp, _ep);
        if (attributes_result.tag == Result<Vector<AttributeSyntax>*, ParserError*>::Error)
            return Result<ElementSyntax*, ParserError*> { .tag = Result<ElementSyntax*, ParserError*>::Error, .error = attributes_result.error };
        auto attributes = attributes_result.ok;

        auto success_comma_3 = this->lexer.parse_punctuation(_r.page, *String::from_c_string(_r.page, ","));
        if (!success_comma_3) {

        }

        auto end = this->lexer.position;

        auto ret = new(alignof(ElementSyntax), _rp) ElementSyntax {
            .start = start,
            .end = end,
            .operation = operation,
            .attributes = attributes,
        };

        return Result<ElementSyntax*, ParserError*> { .tag = Result<ElementSyntax*, ParserError*>::Ok, .ok = ret };
    }

    Result<BlockSyntax*, ParserError*> parse_block(Region& _pr, Page* _rp, Page* _ep) {
        auto _r = Region::create(_pr);
        auto start = this->lexer.previous_position;

        auto success_left_curly_1 = this->lexer.parse_punctuation(_r.page, *String::from_c_string(_r.page, "{"));
        if (!success_left_curly_1) {

                return Result<BlockSyntax*, ParserError*> { .tag = Result<BlockSyntax*, ParserError*>::Ok, .ok = nullptr };
        }

        auto statements_result = this->parse_statement_list(_r, _rp, _ep);
        if (statements_result.tag == Result<Vector<StatementSyntax>*, ParserError*>::Error)
            return Result<BlockSyntax*, ParserError*> { .tag = Result<BlockSyntax*, ParserError*>::Error, .error = statements_result.error };
        auto statements = statements_result.ok;

        auto success_right_curly_3 = this->lexer.parse_punctuation(_r.page, *String::from_c_string(_r.page, "}"));
        if (!success_right_curly_3) {

            return Result<BlockSyntax*, ParserError*> { .tag = Result<BlockSyntax*, ParserError*>::Error, .error = new(alignof(ParserError), _ep) ParserError {
                .file_name = file_name,
                .position = lexer.position,
            }};        }

        auto end = this->lexer.position;

        auto ret = new(alignof(BlockSyntax), _rp) BlockSyntax {
            .start = start,
            .end = end,
            .statements = statements,
        };

        return Result<BlockSyntax*, ParserError*> { .tag = Result<BlockSyntax*, ParserError*>::Ok, .ok = ret };
    }

    Result<IfSyntax*, ParserError*> parse_if(Region& _pr, Page* _rp, Page* _ep) {
        auto _r = Region::create(_pr);
        auto start = this->lexer.previous_position;

        auto success_if_1 = this->lexer.parse_keyword(_r.page, *String::from_c_string(_r.page, "if"));
        if (!success_if_1) {

                return Result<IfSyntax*, ParserError*> { .tag = Result<IfSyntax*, ParserError*>::Ok, .ok = nullptr };
        }

        auto condition_result = this->parse_operation(_r, _rp, _ep);
        if (condition_result.tag == Result<OperationSyntax*, ParserError*>::Error)
            return Result<IfSyntax*, ParserError*> { .tag = Result<IfSyntax*, ParserError*>::Error, .error = condition_result.error };
        auto condition = condition_result.ok;
        if (condition == nullptr) {
            return Result<IfSyntax*, ParserError*> { .tag = Result<IfSyntax*, ParserError*>::Error, .error = new(alignof(ParserError), _ep) ParserError {
                .file_name = file_name,
                .position = lexer.position,
            }};
        }

        auto match_result = this->parse_is(_r, _rp, _ep);
        if (match_result.tag == Result<IsSyntax*, ParserError*>::Error)
            return Result<IfSyntax*, ParserError*> { .tag = Result<IfSyntax*, ParserError*>::Error, .error = match_result.error };
        auto match = match_result.ok;

        auto alias_result = this->parse_as(_r, _rp, _ep);
        if (alias_result.tag == Result<AsSyntax*, ParserError*>::Error)
            return Result<IfSyntax*, ParserError*> { .tag = Result<IfSyntax*, ParserError*>::Error, .error = alias_result.error };
        auto alias = alias_result.ok;

        auto consequent_result = this->parse_action(_r, _rp, _ep);
        if (consequent_result.tag == Result<ActionSyntax*, ParserError*>::Error)
            return Result<IfSyntax*, ParserError*> { .tag = Result<IfSyntax*, ParserError*>::Error, .error = consequent_result.error };
        auto consequent = consequent_result.ok;
        if (consequent == nullptr) {
            return Result<IfSyntax*, ParserError*> { .tag = Result<IfSyntax*, ParserError*>::Error, .error = new(alignof(ParserError), _ep) ParserError {
                .file_name = file_name,
                .position = lexer.position,
            }};
        }

        auto alternative_result = this->parse_else(_r, _rp, _ep);
        if (alternative_result.tag == Result<ElseSyntax*, ParserError*>::Error)
            return Result<IfSyntax*, ParserError*> { .tag = Result<IfSyntax*, ParserError*>::Error, .error = alternative_result.error };
        auto alternative = alternative_result.ok;

        auto end = this->lexer.position;

        auto ret = new(alignof(IfSyntax), _rp) IfSyntax {
            .start = start,
            .end = end,
            .condition = condition,
            .match = match,
            .alias = alias,
            .consequent = consequent,
            .alternative = alternative,
        };

        return Result<IfSyntax*, ParserError*> { .tag = Result<IfSyntax*, ParserError*>::Ok, .ok = ret };
    }

    Result<IsSyntax*, ParserError*> parse_is(Region& _pr, Page* _rp, Page* _ep) {
        auto _r = Region::create(_pr);
        auto start = this->lexer.previous_position;

        auto success_is_1 = this->lexer.parse_keyword(_r.page, *String::from_c_string(_r.page, "is"));
        if (!success_is_1) {

                return Result<IsSyntax*, ParserError*> { .tag = Result<IsSyntax*, ParserError*>::Ok, .ok = nullptr };
        }

        auto type_result = this->parse_type(_r, _rp, _ep);
        if (type_result.tag == Result<TypeSyntax*, ParserError*>::Error)
            return Result<IsSyntax*, ParserError*> { .tag = Result<IsSyntax*, ParserError*>::Error, .error = type_result.error };
        auto type = type_result.ok;
        if (type == nullptr) {
            return Result<IsSyntax*, ParserError*> { .tag = Result<IsSyntax*, ParserError*>::Error, .error = new(alignof(ParserError), _ep) ParserError {
                .file_name = file_name,
                .position = lexer.position,
            }};
        }

        auto end = this->lexer.position;

        auto ret = new(alignof(IsSyntax), _rp) IsSyntax {
            .start = start,
            .end = end,
            .type = type,
        };

        return Result<IsSyntax*, ParserError*> { .tag = Result<IsSyntax*, ParserError*>::Ok, .ok = ret };
    }

    Result<AsSyntax*, ParserError*> parse_as(Region& _pr, Page* _rp, Page* _ep) {
        auto _r = Region::create(_pr);
        auto start = this->lexer.previous_position;

        auto success_as_1 = this->lexer.parse_keyword(_r.page, *String::from_c_string(_r.page, "as"));
        if (!success_as_1) {

                return Result<AsSyntax*, ParserError*> { .tag = Result<AsSyntax*, ParserError*>::Ok, .ok = nullptr };
        }

        auto name = this->lexer.parse_identifier(_r.page, this->keywords);
        if (name != nullptr) {
            if (!this->is_identifier(*name)) {
            return Result<AsSyntax*, ParserError*> { .tag = Result<AsSyntax*, ParserError*>::Error, .error = new(alignof(ParserError), _ep) ParserError {
                .file_name = file_name,
                .position = lexer.position,
            }};
            }
        }
        else {
            return Result<AsSyntax*, ParserError*> { .tag = Result<AsSyntax*, ParserError*>::Error, .error = new(alignof(ParserError), _ep) ParserError {
                .file_name = file_name,
                .position = lexer.position,
            }};
        }

        auto success_colon_3 = this->lexer.parse_colon(_r.page);
        if (!success_colon_3) {

        }

        auto end = this->lexer.position;

        auto ret = new(alignof(AsSyntax), _rp) AsSyntax {
            .start = start,
            .end = end,
            .name = name,
        };

        return Result<AsSyntax*, ParserError*> { .tag = Result<AsSyntax*, ParserError*>::Ok, .ok = ret };
    }

    Result<ElseSyntax*, ParserError*> parse_else(Region& _pr, Page* _rp, Page* _ep) {
        auto _r = Region::create(_pr);
        auto start = this->lexer.previous_position;

        auto success_else_1 = this->lexer.parse_keyword(_r.page, *String::from_c_string(_r.page, "else"));
        if (!success_else_1) {

                return Result<ElseSyntax*, ParserError*> { .tag = Result<ElseSyntax*, ParserError*>::Ok, .ok = nullptr };
        }

        auto success_colon_2 = this->lexer.parse_colon(_r.page);
        if (!success_colon_2) {

        }

        auto alternative_result = this->parse_action(_r, _rp, _ep);
        if (alternative_result.tag == Result<ActionSyntax*, ParserError*>::Error)
            return Result<ElseSyntax*, ParserError*> { .tag = Result<ElseSyntax*, ParserError*>::Error, .error = alternative_result.error };
        auto alternative = alternative_result.ok;
        if (alternative == nullptr) {
            return Result<ElseSyntax*, ParserError*> { .tag = Result<ElseSyntax*, ParserError*>::Error, .error = new(alignof(ParserError), _ep) ParserError {
                .file_name = file_name,
                .position = lexer.position,
            }};
        }

        auto end = this->lexer.position;

        auto ret = new(alignof(ElseSyntax), _rp) ElseSyntax {
            .start = start,
            .end = end,
            .alternative = alternative,
        };

        return Result<ElseSyntax*, ParserError*> { .tag = Result<ElseSyntax*, ParserError*>::Ok, .ok = ret };
    }

    Result<MatchSyntax*, ParserError*> parse_match(Region& _pr, Page* _rp, Page* _ep) {
        auto _r = Region::create(_pr);
        auto start = this->lexer.previous_position;

        auto success_match_1 = this->lexer.parse_keyword(_r.page, *String::from_c_string(_r.page, "match"));
        if (!success_match_1) {

                return Result<MatchSyntax*, ParserError*> { .tag = Result<MatchSyntax*, ParserError*>::Ok, .ok = nullptr };
        }

        auto scrutinee_result = this->parse_operation(_r, _rp, _ep);
        if (scrutinee_result.tag == Result<OperationSyntax*, ParserError*>::Error)
            return Result<MatchSyntax*, ParserError*> { .tag = Result<MatchSyntax*, ParserError*>::Error, .error = scrutinee_result.error };
        auto scrutinee = scrutinee_result.ok;
        if (scrutinee == nullptr) {
            return Result<MatchSyntax*, ParserError*> { .tag = Result<MatchSyntax*, ParserError*>::Error, .error = new(alignof(ParserError), _ep) ParserError {
                .file_name = file_name,
                .position = lexer.position,
            }};
        }

        auto cases_result = this->parse_case_list(_r, _rp, _ep);
        if (cases_result.tag == Result<Vector<CaseSyntax>*, ParserError*>::Error)
            return Result<MatchSyntax*, ParserError*> { .tag = Result<MatchSyntax*, ParserError*>::Error, .error = cases_result.error };
        auto cases = cases_result.ok;
        if (cases == nullptr) {
            return Result<MatchSyntax*, ParserError*> { .tag = Result<MatchSyntax*, ParserError*>::Error, .error = new(alignof(ParserError), _ep) ParserError {
                .file_name = file_name,
                .position = lexer.position,
            }};
        }

        auto alternative_result = this->parse_default(_r, _rp, _ep);
        if (alternative_result.tag == Result<DefaultSyntax*, ParserError*>::Error)
            return Result<MatchSyntax*, ParserError*> { .tag = Result<MatchSyntax*, ParserError*>::Error, .error = alternative_result.error };
        auto alternative = alternative_result.ok;

        auto end = this->lexer.position;

        auto ret = new(alignof(MatchSyntax), _rp) MatchSyntax {
            .start = start,
            .end = end,
            .scrutinee = scrutinee,
            .cases = cases,
            .alternative = alternative,
        };

        return Result<MatchSyntax*, ParserError*> { .tag = Result<MatchSyntax*, ParserError*>::Ok, .ok = ret };
    }

    Result<Vector<CaseSyntax>*, ParserError*> parse_case_list(Region& _pr, Page* _rp, Page* _ep) {
        auto _r = Region::create(_pr);
        {
            auto _r_1 = Region::create(_r);
            Array<CaseSyntax>* array = nullptr;
            while(true) {
                auto node_result = this->parse_case(_r_1, _rp, _ep);
                if (node_result.tag == Result<CaseSyntax*, ParserError*>::Error)
                    return Result<Vector<CaseSyntax>*, ParserError*> { .tag = Result<Vector<CaseSyntax>*, ParserError*>::Error, .error = node_result.error };
                auto node = node_result.ok;
                if (node != nullptr) {
                    if (array == nullptr)
                        array = Array<CaseSyntax>::create(_r_1.page);
                    array->add(*node);
                } else {
                    break;
                }
            }

            if (array == nullptr)
                return Result<Vector<CaseSyntax>*, ParserError*> { .tag = Result<Vector<CaseSyntax>*, ParserError*>::Ok, .ok = nullptr };
            
            return Result<Vector<CaseSyntax>*, ParserError*> { .tag = Result<Vector<CaseSyntax>*, ParserError*>::Ok, .ok = Vector<CaseSyntax>::from_array(_rp, *array) };
        }
    }

    Result<CaseSyntax*, ParserError*> parse_case(Region& _pr, Page* _rp, Page* _ep) {
        auto _r = Region::create(_pr);
        auto start = this->lexer.previous_position;

        auto success_case_1 = this->lexer.parse_keyword(_r.page, *String::from_c_string(_r.page, "case"));
        if (!success_case_1) {

                return Result<CaseSyntax*, ParserError*> { .tag = Result<CaseSyntax*, ParserError*>::Ok, .ok = nullptr };
        }

        auto condition_result = this->parse_operation(_r, _rp, _ep);
        if (condition_result.tag == Result<OperationSyntax*, ParserError*>::Error)
            return Result<CaseSyntax*, ParserError*> { .tag = Result<CaseSyntax*, ParserError*>::Error, .error = condition_result.error };
        auto condition = condition_result.ok;
        if (condition == nullptr) {
            return Result<CaseSyntax*, ParserError*> { .tag = Result<CaseSyntax*, ParserError*>::Error, .error = new(alignof(ParserError), _ep) ParserError {
                .file_name = file_name,
                .position = lexer.position,
            }};
        }

        auto consequent_result = this->parse_action(_r, _rp, _ep);
        if (consequent_result.tag == Result<ActionSyntax*, ParserError*>::Error)
            return Result<CaseSyntax*, ParserError*> { .tag = Result<CaseSyntax*, ParserError*>::Error, .error = consequent_result.error };
        auto consequent = consequent_result.ok;
        if (consequent == nullptr) {
            return Result<CaseSyntax*, ParserError*> { .tag = Result<CaseSyntax*, ParserError*>::Error, .error = new(alignof(ParserError), _ep) ParserError {
                .file_name = file_name,
                .position = lexer.position,
            }};
        }

        auto end = this->lexer.position;

        auto ret = new(alignof(CaseSyntax), _rp) CaseSyntax {
            .start = start,
            .end = end,
            .condition = condition,
            .consequent = consequent,
        };

        return Result<CaseSyntax*, ParserError*> { .tag = Result<CaseSyntax*, ParserError*>::Ok, .ok = ret };
    }

    Result<DefaultSyntax*, ParserError*> parse_default(Region& _pr, Page* _rp, Page* _ep) {
        auto _r = Region::create(_pr);
        auto start = this->lexer.previous_position;

        auto success_default_1 = this->lexer.parse_keyword(_r.page, *String::from_c_string(_r.page, "default"));
        if (!success_default_1) {

                return Result<DefaultSyntax*, ParserError*> { .tag = Result<DefaultSyntax*, ParserError*>::Ok, .ok = nullptr };
        }

        auto alternative_result = this->parse_action(_r, _rp, _ep);
        if (alternative_result.tag == Result<ActionSyntax*, ParserError*>::Error)
            return Result<DefaultSyntax*, ParserError*> { .tag = Result<DefaultSyntax*, ParserError*>::Error, .error = alternative_result.error };
        auto alternative = alternative_result.ok;

        auto end = this->lexer.position;

        auto ret = new(alignof(DefaultSyntax), _rp) DefaultSyntax {
            .start = start,
            .end = end,
            .alternative = alternative,
        };

        return Result<DefaultSyntax*, ParserError*> { .tag = Result<DefaultSyntax*, ParserError*>::Ok, .ok = ret };
    }

    Result<LambdaSyntax*, ParserError*> parse_lambda(Region& _pr, Page* _rp, Page* _ep) {
        auto _r = Region::create(_pr);
        auto start = this->lexer.previous_position;

        auto success_lambda_1 = this->lexer.parse_keyword(_r.page, *String::from_c_string(_r.page, "lambda"));
        if (!success_lambda_1) {

                return Result<LambdaSyntax*, ParserError*> { .tag = Result<LambdaSyntax*, ParserError*>::Ok, .ok = nullptr };
        }

        auto input_result = this->parse_operation(_r, _rp, _ep);
        if (input_result.tag == Result<OperationSyntax*, ParserError*>::Error)
            return Result<LambdaSyntax*, ParserError*> { .tag = Result<LambdaSyntax*, ParserError*>::Error, .error = input_result.error };
        auto input = input_result.ok;
        if (input == nullptr) {
            return Result<LambdaSyntax*, ParserError*> { .tag = Result<LambdaSyntax*, ParserError*>::Error, .error = new(alignof(ParserError), _ep) ParserError {
                .file_name = file_name,
                .position = lexer.position,
            }};
        }

        auto success_colon_3 = this->lexer.parse_colon(_r.page);
        if (!success_colon_3) {

            return Result<LambdaSyntax*, ParserError*> { .tag = Result<LambdaSyntax*, ParserError*>::Error, .error = new(alignof(ParserError), _ep) ParserError {
                .file_name = file_name,
                .position = lexer.position,
            }};        }

        auto block_result = this->parse_action(_r, _rp, _ep);
        if (block_result.tag == Result<ActionSyntax*, ParserError*>::Error)
            return Result<LambdaSyntax*, ParserError*> { .tag = Result<LambdaSyntax*, ParserError*>::Error, .error = block_result.error };
        auto block = block_result.ok;
        if (block == nullptr) {
            return Result<LambdaSyntax*, ParserError*> { .tag = Result<LambdaSyntax*, ParserError*>::Error, .error = new(alignof(ParserError), _ep) ParserError {
                .file_name = file_name,
                .position = lexer.position,
            }};
        }

        auto end = this->lexer.position;

        auto ret = new(alignof(LambdaSyntax), _rp) LambdaSyntax {
            .start = start,
            .end = end,
            .input = input,
            .block = block,
        };

        return Result<LambdaSyntax*, ParserError*> { .tag = Result<LambdaSyntax*, ParserError*>::Ok, .ok = ret };
    }

    Result<ForSyntax*, ParserError*> parse_for(Region& _pr, Page* _rp, Page* _ep) {
        auto _r = Region::create(_pr);
        auto start = this->lexer.previous_position;

        auto success_for_1 = this->lexer.parse_keyword(_r.page, *String::from_c_string(_r.page, "for"));
        if (!success_for_1) {

                return Result<ForSyntax*, ParserError*> { .tag = Result<ForSyntax*, ParserError*>::Ok, .ok = nullptr };
        }

        auto condition_result = this->parse_operation(_r, _rp, _ep);
        if (condition_result.tag == Result<OperationSyntax*, ParserError*>::Error)
            return Result<ForSyntax*, ParserError*> { .tag = Result<ForSyntax*, ParserError*>::Error, .error = condition_result.error };
        auto condition = condition_result.ok;
        if (condition == nullptr) {
            return Result<ForSyntax*, ParserError*> { .tag = Result<ForSyntax*, ParserError*>::Error, .error = new(alignof(ParserError), _ep) ParserError {
                .file_name = file_name,
                .position = lexer.position,
            }};
        }

        auto success_in_3 = this->lexer.parse_keyword(_r.page, *String::from_c_string(_r.page, "in"));
        if (!success_in_3) {

            return Result<ForSyntax*, ParserError*> { .tag = Result<ForSyntax*, ParserError*>::Error, .error = new(alignof(ParserError), _ep) ParserError {
                .file_name = file_name,
                .position = lexer.position,
            }};        }

        auto expression_result = this->parse_operation(_r, _rp, _ep);
        if (expression_result.tag == Result<OperationSyntax*, ParserError*>::Error)
            return Result<ForSyntax*, ParserError*> { .tag = Result<ForSyntax*, ParserError*>::Error, .error = expression_result.error };
        auto expression = expression_result.ok;
        if (expression == nullptr) {
            return Result<ForSyntax*, ParserError*> { .tag = Result<ForSyntax*, ParserError*>::Error, .error = new(alignof(ParserError), _ep) ParserError {
                .file_name = file_name,
                .position = lexer.position,
            }};
        }

        auto name_result = this->parse_label(_r, _rp, _ep);
        if (name_result.tag == Result<LabelSyntax*, ParserError*>::Error)
            return Result<ForSyntax*, ParserError*> { .tag = Result<ForSyntax*, ParserError*>::Error, .error = name_result.error };
        auto name = name_result.ok;

        auto action_result = this->parse_action(_r, _rp, _ep);
        if (action_result.tag == Result<ActionSyntax*, ParserError*>::Error)
            return Result<ForSyntax*, ParserError*> { .tag = Result<ForSyntax*, ParserError*>::Error, .error = action_result.error };
        auto action = action_result.ok;
        if (action == nullptr) {
            return Result<ForSyntax*, ParserError*> { .tag = Result<ForSyntax*, ParserError*>::Error, .error = new(alignof(ParserError), _ep) ParserError {
                .file_name = file_name,
                .position = lexer.position,
            }};
        }

        auto end = this->lexer.position;

        auto ret = new(alignof(ForSyntax), _rp) ForSyntax {
            .start = start,
            .end = end,
            .condition = condition,
            .expression = expression,
            .name = name,
            .action = action,
        };

        return Result<ForSyntax*, ParserError*> { .tag = Result<ForSyntax*, ParserError*>::Ok, .ok = ret };
    }

    Result<LabelSyntax*, ParserError*> parse_label(Region& _pr, Page* _rp, Page* _ep) {
        auto _r = Region::create(_pr);
        auto start = this->lexer.previous_position;

        auto success_label_1 = this->lexer.parse_keyword(_r.page, *String::from_c_string(_r.page, "label"));
        if (!success_label_1) {

                return Result<LabelSyntax*, ParserError*> { .tag = Result<LabelSyntax*, ParserError*>::Ok, .ok = nullptr };
        }

        auto name = this->lexer.parse_identifier(_r.page, this->keywords);
        if (name != nullptr) {
            if (!this->is_identifier(*name)) {
            return Result<LabelSyntax*, ParserError*> { .tag = Result<LabelSyntax*, ParserError*>::Error, .error = new(alignof(ParserError), _ep) ParserError {
                .file_name = file_name,
                .position = lexer.position,
            }};
            }
        }
        else {
            return Result<LabelSyntax*, ParserError*> { .tag = Result<LabelSyntax*, ParserError*>::Error, .error = new(alignof(ParserError), _ep) ParserError {
                .file_name = file_name,
                .position = lexer.position,
            }};
        }

        auto end = this->lexer.position;

        auto ret = new(alignof(LabelSyntax), _rp) LabelSyntax {
            .start = start,
            .end = end,
            .name = name,
        };

        return Result<LabelSyntax*, ParserError*> { .tag = Result<LabelSyntax*, ParserError*>::Ok, .ok = ret };
    }

    Result<WhileSyntax*, ParserError*> parse_while(Region& _pr, Page* _rp, Page* _ep) {
        auto _r = Region::create(_pr);
        auto start = this->lexer.previous_position;

        auto success_while_1 = this->lexer.parse_keyword(_r.page, *String::from_c_string(_r.page, "while"));
        if (!success_while_1) {

                return Result<WhileSyntax*, ParserError*> { .tag = Result<WhileSyntax*, ParserError*>::Ok, .ok = nullptr };
        }

        auto condition_result = this->parse_operation(_r, _rp, _ep);
        if (condition_result.tag == Result<OperationSyntax*, ParserError*>::Error)
            return Result<WhileSyntax*, ParserError*> { .tag = Result<WhileSyntax*, ParserError*>::Error, .error = condition_result.error };
        auto condition = condition_result.ok;
        if (condition == nullptr) {
            return Result<WhileSyntax*, ParserError*> { .tag = Result<WhileSyntax*, ParserError*>::Error, .error = new(alignof(ParserError), _ep) ParserError {
                .file_name = file_name,
                .position = lexer.position,
            }};
        }

        auto name_result = this->parse_label(_r, _rp, _ep);
        if (name_result.tag == Result<LabelSyntax*, ParserError*>::Error)
            return Result<WhileSyntax*, ParserError*> { .tag = Result<WhileSyntax*, ParserError*>::Error, .error = name_result.error };
        auto name = name_result.ok;

        auto action_result = this->parse_action(_r, _rp, _ep);
        if (action_result.tag == Result<ActionSyntax*, ParserError*>::Error)
            return Result<WhileSyntax*, ParserError*> { .tag = Result<WhileSyntax*, ParserError*>::Error, .error = action_result.error };
        auto action = action_result.ok;
        if (action == nullptr) {
            return Result<WhileSyntax*, ParserError*> { .tag = Result<WhileSyntax*, ParserError*>::Error, .error = new(alignof(ParserError), _ep) ParserError {
                .file_name = file_name,
                .position = lexer.position,
            }};
        }

        auto end = this->lexer.position;

        auto ret = new(alignof(WhileSyntax), _rp) WhileSyntax {
            .start = start,
            .end = end,
            .condition = condition,
            .name = name,
            .action = action,
        };

        return Result<WhileSyntax*, ParserError*> { .tag = Result<WhileSyntax*, ParserError*>::Ok, .ok = ret };
    }

    Result<RepeatSyntax*, ParserError*> parse_repeat(Region& _pr, Page* _rp, Page* _ep) {
        auto _r = Region::create(_pr);
        auto start = this->lexer.previous_position;

        auto success_repeat_1 = this->lexer.parse_keyword(_r.page, *String::from_c_string(_r.page, "repeat"));
        if (!success_repeat_1) {

                return Result<RepeatSyntax*, ParserError*> { .tag = Result<RepeatSyntax*, ParserError*>::Ok, .ok = nullptr };
        }

        auto name_result = this->parse_label(_r, _rp, _ep);
        if (name_result.tag == Result<LabelSyntax*, ParserError*>::Error)
            return Result<RepeatSyntax*, ParserError*> { .tag = Result<RepeatSyntax*, ParserError*>::Error, .error = name_result.error };
        auto name = name_result.ok;

        auto action_result = this->parse_action(_r, _rp, _ep);
        if (action_result.tag == Result<ActionSyntax*, ParserError*>::Error)
            return Result<RepeatSyntax*, ParserError*> { .tag = Result<RepeatSyntax*, ParserError*>::Error, .error = action_result.error };
        auto action = action_result.ok;
        if (action == nullptr) {
            return Result<RepeatSyntax*, ParserError*> { .tag = Result<RepeatSyntax*, ParserError*>::Error, .error = new(alignof(ParserError), _ep) ParserError {
                .file_name = file_name,
                .position = lexer.position,
            }};
        }

        auto end = this->lexer.position;

        auto ret = new(alignof(RepeatSyntax), _rp) RepeatSyntax {
            .start = start,
            .end = end,
            .name = name,
            .action = action,
        };

        return Result<RepeatSyntax*, ParserError*> { .tag = Result<RepeatSyntax*, ParserError*>::Ok, .ok = ret };
    }

    Result<SizeOfSyntax*, ParserError*> parse_sizeof(Region& _pr, Page* _rp, Page* _ep) {
        auto _r = Region::create(_pr);
        auto start = this->lexer.previous_position;

        auto success_sizeof_1 = this->lexer.parse_keyword(_r.page, *String::from_c_string(_r.page, "sizeof"));
        if (!success_sizeof_1) {

                return Result<SizeOfSyntax*, ParserError*> { .tag = Result<SizeOfSyntax*, ParserError*>::Ok, .ok = nullptr };
        }

        auto type_result = this->parse_type(_r, _rp, _ep);
        if (type_result.tag == Result<TypeSyntax*, ParserError*>::Error)
            return Result<SizeOfSyntax*, ParserError*> { .tag = Result<SizeOfSyntax*, ParserError*>::Error, .error = type_result.error };
        auto type = type_result.ok;
        if (type == nullptr) {
            return Result<SizeOfSyntax*, ParserError*> { .tag = Result<SizeOfSyntax*, ParserError*>::Error, .error = new(alignof(ParserError), _ep) ParserError {
                .file_name = file_name,
                .position = lexer.position,
            }};
        }

        auto end = this->lexer.position;

        auto ret = new(alignof(SizeOfSyntax), _rp) SizeOfSyntax {
            .start = start,
            .end = end,
            .type = type,
        };

        return Result<SizeOfSyntax*, ParserError*> { .tag = Result<SizeOfSyntax*, ParserError*>::Ok, .ok = ret };
    }

    bool is_at_end() {
        return this->lexer.is_at_end();
    }

    bool is_identifier(String& id) {
        if (this->keywords.contains(id))
            return false;
        else
            return true;
    }
};

}