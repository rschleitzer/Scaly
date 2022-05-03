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
 };

}