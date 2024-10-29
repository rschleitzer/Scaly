namespace scaly {
namespace compiler {

struct IdentifierLiteral {
    IdentifierLiteral(String name) : name(name) {}
    String name;
};

struct StringLiteral {
    StringLiteral(String value) : value(value) {}
    String value;
};

struct CharacterLiteral {
    CharacterLiteral(String value) : value(value) {}
    String value;
};

struct FragmentLiteral {
    FragmentLiteral(String value) : value(value) {}
    String value;
};

struct IntegerLiteral {
    IntegerLiteral(String value) : value(value) {}
    String value;
};

struct BooleanLiteral {
    BooleanLiteral(bool value) : value(value) {}
    bool value;
};

struct FloatingPointLiteral {
    FloatingPointLiteral(String value) : value(value) {}
    String value;
};

struct HexLiteral {
    HexLiteral(String value) : value(value) {}
    String value;
};

struct Literal : Object {
    Literal(StringLiteral string) : _tag(String), _String(string) {}
    Literal(CharacterLiteral string) : _tag(Character), _Character(string) {}
    Literal(FragmentLiteral fragment) : _tag(Fragment), _Fragment(fragment) {}
    Literal(IntegerLiteral integer) : _tag(Integer), _Integer(integer) {}
    Literal(BooleanLiteral boolean) : _tag(Boolean), _Boolean(boolean) {}
    Literal(FloatingPointLiteral floating_point) : _tag(FloatingPoint), _FloatingPoint(floating_point) {}
    Literal(HexLiteral hex) : _tag(Hex), _Hex(hex) {}
    enum {
        String,
        Character,
        Fragment,
        Integer,
        Boolean,
        FloatingPoint,
        Hex,
    } _tag;
    union {
        StringLiteral _String;
        CharacterLiteral _Character;
        FragmentLiteral _Fragment;
        IntegerLiteral _Integer;
        BooleanLiteral _Boolean;
        FloatingPointLiteral _FloatingPoint;
        HexLiteral _Hex;

    };
};

struct ProgramSyntax; 
struct FileSyntax; 
struct DeclarationSyntax; 
struct SymbolSyntax; 
struct PrivateSyntax; 
struct ExportSyntax; 
struct MemberSyntax; 
struct ConstituentSyntax; 
struct DefinitionSyntax; 
struct GenericParametersSyntax; 
struct GenericParameterSyntax; 
struct ConceptSyntax; 
struct ClassSyntax; 
struct BodySyntax; 
struct NamespaceSyntax; 
struct UnionSyntax; 
struct VariantSyntax; 
struct ConstantSyntax; 
struct DelegateSyntax; 
struct TypeAnnotationSyntax; 
struct GenericArgumentsSyntax; 
struct GenericArgumentSyntax; 
struct OptionalSyntax; 
struct ParameterSetSyntax; 
struct ParametersSyntax; 
struct ItemSyntax; 
struct ReturnsSyntax; 
struct ThrowsSyntax; 
struct UseSyntax; 
struct ImplementSyntax; 
struct TraitSyntax; 
struct MethodSyntax; 
struct InitSyntax; 
struct DeInitSyntax; 
struct FunctionSyntax; 
struct ProcedureSyntax; 
struct OperatorSyntax; 
struct TargetSyntax; 
struct NamedSyntax; 
struct RoutineSyntax; 
struct ImplementationSyntax; 
struct ExternSyntax; 
struct InstructionSyntax; 
struct IntrinsicSyntax; 
struct ExtendsSyntax; 
struct ExtendSyntax; 
struct MacroSyntax; 
struct AttributeSyntax; 
struct ModelSyntax; 
struct ModuleSyntax; 
struct PackageSyntax; 
struct InitializerSyntax; 
struct OperandSyntax; 
struct PostfixSyntax; 
struct MemberAccessSyntax; 
struct CatcherSyntax; 
struct CatchSyntax; 
struct DropSyntax; 
struct ExpressionSyntax; 
struct LiteralSyntax; 
struct ObjectSyntax; 
struct ComponentSyntax; 
struct ValueSyntax; 
struct VectorSyntax; 
struct ElementSyntax; 
struct BlockSyntax; 
struct IfSyntax; 
struct ElseSyntax; 
struct StatementSyntax; 
struct CommandSyntax; 
struct WhileSyntax; 
struct ConditionSyntax; 
struct LetSyntax; 
struct VarSyntax; 
struct MutableSyntax; 
struct BindingSyntax; 
struct BindingAnnotationSyntax; 
struct BindingSpecSyntax; 
struct ArraySyntax; 
struct StructureSyntax; 
struct PartSyntax; 
struct FieldSyntax; 
struct PropertySyntax; 
struct ContinueSyntax; 
struct BreakSyntax; 
struct LoopSyntax; 
struct ReturnSyntax; 
struct ThrowSyntax; 
struct MatchSyntax; 
struct CaseSyntax; 
struct DefaultSyntax; 
struct LambdaSyntax; 
struct ForSyntax; 
struct LabelSyntax; 
struct RepeatSyntax; 
struct ActionSyntax; 
struct OperationSyntax; 
struct SetSyntax; 
struct SizeOfSyntax; 
struct TypeSyntax; 
struct NameSyntax; 
struct ExtensionSyntax; 
struct LifetimeSyntax; 
struct CallSyntax; 
struct LocalSyntax; 
struct ReferenceSyntax; 
struct ThrownSyntax; 

struct ThrownSyntax : Object {
    ThrownSyntax(size_t start, size_t end) : start(start), end(end) {}
    size_t start;
    size_t end;
};

struct ReferenceSyntax : Object {
    ReferenceSyntax(size_t start, size_t end, String location) : start(start), end(end), location(location) {}
    size_t start;
    size_t end;
    String location;
};

struct LocalSyntax : Object {
    LocalSyntax(size_t start, size_t end) : start(start), end(end) {}
    size_t start;
    size_t end;
};

struct CallSyntax : Object {
    CallSyntax(size_t start, size_t end) : start(start), end(end) {}
    size_t start;
    size_t end;
};

struct LifetimeSyntax : Object {
    LifetimeSyntax(CallSyntax _CallSyntax) : _tag(Call) { _Call = _CallSyntax; }
    LifetimeSyntax(LocalSyntax _LocalSyntax) : _tag(Local) { _Local = _LocalSyntax; }
    LifetimeSyntax(ReferenceSyntax _ReferenceSyntax) : _tag(Reference) { _Reference = _ReferenceSyntax; }
    LifetimeSyntax(ThrownSyntax _ThrownSyntax) : _tag(Thrown) { _Thrown = _ThrownSyntax; }
    enum {
        Call,
        Local,
        Reference,
        Thrown,
    } _tag;
    union {
        CallSyntax _Call;
        LocalSyntax _Local;
        ReferenceSyntax _Reference;
        ThrownSyntax _Thrown;
    };
};

struct ExtensionSyntax : Object {
    ExtensionSyntax(size_t start, size_t end, String name) : start(start), end(end), name(name) {}
    size_t start;
    size_t end;
    String name;
};

struct NameSyntax : Object {
    NameSyntax(size_t start, size_t end, String name, Vector<ExtensionSyntax>* extensions) : start(start), end(end), name(name), extensions(extensions) {}
    size_t start;
    size_t end;
    String name;
    Vector<ExtensionSyntax>* extensions;
};

struct TypeSyntax : Object {
    TypeSyntax(size_t start, size_t end, NameSyntax name, GenericArgumentsSyntax* generics, OptionalSyntax* optional, LifetimeSyntax* lifetime) : start(start), end(end), name(name), generics(generics), optional(optional), lifetime(lifetime) {}
    size_t start;
    size_t end;
    NameSyntax name;
    GenericArgumentsSyntax* generics;
    OptionalSyntax* optional;
    LifetimeSyntax* lifetime;
};

struct SizeOfSyntax : Object {
    SizeOfSyntax(size_t start, size_t end, TypeSyntax type) : start(start), end(end), type(type) {}
    size_t start;
    size_t end;
    TypeSyntax type;
};

struct SetSyntax : Object {
    SetSyntax(size_t start, size_t end, Vector<OperandSyntax>* target, Vector<OperandSyntax>* source) : start(start), end(end), target(target), source(source) {}
    size_t start;
    size_t end;
    Vector<OperandSyntax>* target;
    Vector<OperandSyntax>* source;
};

struct OperationSyntax : Object {
    OperationSyntax(size_t start, size_t end, Vector<OperandSyntax>* operands) : start(start), end(end), operands(operands) {}
    size_t start;
    size_t end;
    Vector<OperandSyntax>* operands;
};

struct ActionSyntax : Object {
    ActionSyntax(OperationSyntax _OperationSyntax) : _tag(Operation) { _Operation = _OperationSyntax; }
    ActionSyntax(SetSyntax _SetSyntax) : _tag(Set) { _Set = _SetSyntax; }
    enum {
        Operation,
        Set,
    } _tag;
    union {
        OperationSyntax _Operation;
        SetSyntax _Set;
    };
};

struct RepeatSyntax : Object {
    RepeatSyntax(size_t start, size_t end, LabelSyntax* name, ActionSyntax action) : start(start), end(end), name(name), action(action) {}
    size_t start;
    size_t end;
    LabelSyntax* name;
    ActionSyntax action;
};

struct LabelSyntax : Object {
    LabelSyntax(size_t start, size_t end, String name) : start(start), end(end), name(name) {}
    size_t start;
    size_t end;
    String name;
};

struct ForSyntax : Object {
    ForSyntax(size_t start, size_t end, String variable, TypeAnnotationSyntax* annotation, Vector<OperandSyntax>* operation, LabelSyntax* name, ActionSyntax action) : start(start), end(end), variable(variable), annotation(annotation), operation(operation), name(name), action(action) {}
    size_t start;
    size_t end;
    String variable;
    TypeAnnotationSyntax* annotation;
    Vector<OperandSyntax>* operation;
    LabelSyntax* name;
    ActionSyntax action;
};

struct LambdaSyntax : Object {
    LambdaSyntax(size_t start, size_t end, Vector<OperandSyntax>* input, ActionSyntax block) : start(start), end(end), input(input), block(block) {}
    size_t start;
    size_t end;
    Vector<OperandSyntax>* input;
    ActionSyntax block;
};

struct DefaultSyntax : Object {
    DefaultSyntax(size_t start, size_t end, ActionSyntax* alternative) : start(start), end(end), alternative(alternative) {}
    size_t start;
    size_t end;
    ActionSyntax* alternative;
};

struct CaseSyntax : Object {
    CaseSyntax(size_t start, size_t end, Vector<OperandSyntax>* condition, ActionSyntax consequent) : start(start), end(end), condition(condition), consequent(consequent) {}
    size_t start;
    size_t end;
    Vector<OperandSyntax>* condition;
    ActionSyntax consequent;
};

struct MatchSyntax : Object {
    MatchSyntax(size_t start, size_t end, Vector<OperandSyntax>* scrutinee, Vector<CaseSyntax>* cases, DefaultSyntax* alternative) : start(start), end(end), scrutinee(scrutinee), cases(cases), alternative(alternative) {}
    size_t start;
    size_t end;
    Vector<OperandSyntax>* scrutinee;
    Vector<CaseSyntax>* cases;
    DefaultSyntax* alternative;
};

struct ThrowSyntax : Object {
    ThrowSyntax(size_t start, size_t end, Vector<OperandSyntax>* result) : start(start), end(end), result(result) {}
    size_t start;
    size_t end;
    Vector<OperandSyntax>* result;
};

struct ReturnSyntax : Object {
    ReturnSyntax(size_t start, size_t end, Vector<OperandSyntax>* result) : start(start), end(end), result(result) {}
    size_t start;
    size_t end;
    Vector<OperandSyntax>* result;
};

struct LoopSyntax : Object {
    LoopSyntax(size_t start, size_t end, String name) : start(start), end(end), name(name) {}
    size_t start;
    size_t end;
    String name;
};

struct BreakSyntax : Object {
    BreakSyntax(size_t start, size_t end, LoopSyntax* name, Vector<OperandSyntax>* result) : start(start), end(end), name(name), result(result) {}
    size_t start;
    size_t end;
    LoopSyntax* name;
    Vector<OperandSyntax>* result;
};

struct ContinueSyntax : Object {
    ContinueSyntax(size_t start, size_t end, LoopSyntax* name) : start(start), end(end), name(name) {}
    size_t start;
    size_t end;
    LoopSyntax* name;
};

struct PropertySyntax : Object {
    PropertySyntax(size_t start, size_t end, String name, TypeAnnotationSyntax* annotation, InitializerSyntax* initializer, Vector<AttributeSyntax>* attributes) : start(start), end(end), name(name), annotation(annotation), initializer(initializer), attributes(attributes) {}
    size_t start;
    size_t end;
    String name;
    TypeAnnotationSyntax* annotation;
    InitializerSyntax* initializer;
    Vector<AttributeSyntax>* attributes;
};

struct FieldSyntax : Object {
    FieldSyntax(size_t start, size_t end, PropertySyntax property) : start(start), end(end), property(property) {}
    size_t start;
    size_t end;
    PropertySyntax property;
};

struct PartSyntax : Object {
    PartSyntax(FieldSyntax _FieldSyntax) : _tag(Field) { _Field = _FieldSyntax; }
    PartSyntax(PropertySyntax _PropertySyntax) : _tag(Property) { _Property = _PropertySyntax; }
    enum {
        Field,
        Property,
    } _tag;
    union {
        FieldSyntax _Field;
        PropertySyntax _Property;
    };
};

struct StructureSyntax : Object {
    StructureSyntax(size_t start, size_t end, Vector<PartSyntax>* parts) : start(start), end(end), parts(parts) {}
    size_t start;
    size_t end;
    Vector<PartSyntax>* parts;
};

struct ArraySyntax : Object {
    ArraySyntax(size_t start, size_t end, Vector<TypeSyntax>* members) : start(start), end(end), members(members) {}
    size_t start;
    size_t end;
    Vector<TypeSyntax>* members;
};

struct BindingSpecSyntax : Object {
    BindingSpecSyntax(StructureSyntax _StructureSyntax) : _tag(Structure) { _Structure = _StructureSyntax; }
    BindingSpecSyntax(TypeSyntax _TypeSyntax) : _tag(Type) { _Type = _TypeSyntax; }
    BindingSpecSyntax(ArraySyntax _ArraySyntax) : _tag(Array) { _Array = _ArraySyntax; }
    enum {
        Structure,
        Type,
        Array,
    } _tag;
    union {
        StructureSyntax _Structure;
        TypeSyntax _Type;
        ArraySyntax _Array;
    };
};

struct BindingAnnotationSyntax : Object {
    BindingAnnotationSyntax(size_t start, size_t end, BindingSpecSyntax spec) : start(start), end(end), spec(spec) {}
    size_t start;
    size_t end;
    BindingSpecSyntax spec;
};

struct BindingSyntax : Object {
    BindingSyntax(size_t start, size_t end, String name, BindingAnnotationSyntax* annotation, Vector<OperandSyntax>* operation) : start(start), end(end), name(name), annotation(annotation), operation(operation) {}
    size_t start;
    size_t end;
    String name;
    BindingAnnotationSyntax* annotation;
    Vector<OperandSyntax>* operation;
};

struct MutableSyntax : Object {
    MutableSyntax(size_t start, size_t end, BindingSyntax binding) : start(start), end(end), binding(binding) {}
    size_t start;
    size_t end;
    BindingSyntax binding;
};

struct VarSyntax : Object {
    VarSyntax(size_t start, size_t end, BindingSyntax binding) : start(start), end(end), binding(binding) {}
    size_t start;
    size_t end;
    BindingSyntax binding;
};

struct LetSyntax : Object {
    LetSyntax(size_t start, size_t end, BindingSyntax binding) : start(start), end(end), binding(binding) {}
    size_t start;
    size_t end;
    BindingSyntax binding;
};

struct ConditionSyntax : Object {
    ConditionSyntax(OperationSyntax _OperationSyntax) : _tag(Operation) { _Operation = _OperationSyntax; }
    ConditionSyntax(LetSyntax _LetSyntax) : _tag(Let) { _Let = _LetSyntax; }
    enum {
        Operation,
        Let,
    } _tag;
    union {
        OperationSyntax _Operation;
        LetSyntax _Let;
    };
};

struct WhileSyntax : Object {
    WhileSyntax(size_t start, size_t end, ConditionSyntax condition, LabelSyntax* name, ActionSyntax action) : start(start), end(end), condition(condition), name(name), action(action) {}
    size_t start;
    size_t end;
    ConditionSyntax condition;
    LabelSyntax* name;
    ActionSyntax action;
};

struct CommandSyntax : Object {
    CommandSyntax(OperationSyntax _OperationSyntax) : _tag(Operation) { _Operation = _OperationSyntax; }
    CommandSyntax(LetSyntax _LetSyntax) : _tag(Let) { _Let = _LetSyntax; }
    CommandSyntax(VarSyntax _VarSyntax) : _tag(Var) { _Var = _VarSyntax; }
    CommandSyntax(MutableSyntax _MutableSyntax) : _tag(Mutable) { _Mutable = _MutableSyntax; }
    CommandSyntax(SetSyntax _SetSyntax) : _tag(Set) { _Set = _SetSyntax; }
    CommandSyntax(ContinueSyntax _ContinueSyntax) : _tag(Continue) { _Continue = _ContinueSyntax; }
    CommandSyntax(BreakSyntax _BreakSyntax) : _tag(Break) { _Break = _BreakSyntax; }
    CommandSyntax(ReturnSyntax _ReturnSyntax) : _tag(Return) { _Return = _ReturnSyntax; }
    CommandSyntax(ThrowSyntax _ThrowSyntax) : _tag(Throw) { _Throw = _ThrowSyntax; }
    enum {
        Operation,
        Let,
        Var,
        Mutable,
        Set,
        Continue,
        Break,
        Return,
        Throw,
    } _tag;
    union {
        OperationSyntax _Operation;
        LetSyntax _Let;
        VarSyntax _Var;
        MutableSyntax _Mutable;
        SetSyntax _Set;
        ContinueSyntax _Continue;
        BreakSyntax _Break;
        ReturnSyntax _Return;
        ThrowSyntax _Throw;
    };
};

struct StatementSyntax : Object {
    StatementSyntax(size_t start, size_t end, CommandSyntax command) : start(start), end(end), command(command) {}
    size_t start;
    size_t end;
    CommandSyntax command;
};

struct ElseSyntax : Object {
    ElseSyntax(size_t start, size_t end, CommandSyntax alternative) : start(start), end(end), alternative(alternative) {}
    size_t start;
    size_t end;
    CommandSyntax alternative;
};

struct IfSyntax : Object {
    IfSyntax(size_t start, size_t end, Vector<OperandSyntax>* condition, CommandSyntax consequent, ElseSyntax* alternative) : start(start), end(end), condition(condition), consequent(consequent), alternative(alternative) {}
    size_t start;
    size_t end;
    Vector<OperandSyntax>* condition;
    CommandSyntax consequent;
    ElseSyntax* alternative;
};

struct BlockSyntax : Object {
    BlockSyntax(size_t start, size_t end, Vector<UseSyntax>* uses, Vector<StatementSyntax>* statements) : start(start), end(end), uses(uses), statements(statements) {}
    size_t start;
    size_t end;
    Vector<UseSyntax>* uses;
    Vector<StatementSyntax>* statements;
};

struct ElementSyntax : Object {
    ElementSyntax(size_t start, size_t end, Vector<OperandSyntax>* operation, Vector<AttributeSyntax>* attributes) : start(start), end(end), operation(operation), attributes(attributes) {}
    size_t start;
    size_t end;
    Vector<OperandSyntax>* operation;
    Vector<AttributeSyntax>* attributes;
};

struct VectorSyntax : Object {
    VectorSyntax(size_t start, size_t end, Vector<ElementSyntax>* elements, LifetimeSyntax* lifetime) : start(start), end(end), elements(elements), lifetime(lifetime) {}
    size_t start;
    size_t end;
    Vector<ElementSyntax>* elements;
    LifetimeSyntax* lifetime;
};

struct ValueSyntax : Object {
    ValueSyntax(size_t start, size_t end, Vector<OperandSyntax>* value, Vector<AttributeSyntax>* attributes) : start(start), end(end), value(value), attributes(attributes) {}
    size_t start;
    size_t end;
    Vector<OperandSyntax>* value;
    Vector<AttributeSyntax>* attributes;
};

struct ComponentSyntax : Object {
    ComponentSyntax(size_t start, size_t end, Vector<OperandSyntax>* operands, Vector<AttributeSyntax>* attributes, ValueSyntax* value) : start(start), end(end), operands(operands), attributes(attributes), value(value) {}
    size_t start;
    size_t end;
    Vector<OperandSyntax>* operands;
    Vector<AttributeSyntax>* attributes;
    ValueSyntax* value;
};

struct ObjectSyntax : Object {
    ObjectSyntax(size_t start, size_t end, Vector<ComponentSyntax>* components) : start(start), end(end), components(components) {}
    size_t start;
    size_t end;
    Vector<ComponentSyntax>* components;
};

struct LiteralSyntax : Object {
    LiteralSyntax(size_t start, size_t end, Literal literal) : start(start), end(end), literal(literal) {}
    size_t start;
    size_t end;
    Literal literal;
};

struct ExpressionSyntax : Object {
    ExpressionSyntax(LiteralSyntax _LiteralSyntax) : _tag(Literal) { _Literal = _LiteralSyntax; }
    ExpressionSyntax(TypeSyntax _TypeSyntax) : _tag(Type) { _Type = _TypeSyntax; }
    ExpressionSyntax(ObjectSyntax _ObjectSyntax) : _tag(Object) { _Object = _ObjectSyntax; }
    ExpressionSyntax(VectorSyntax _VectorSyntax) : _tag(Vector) { _Vector = _VectorSyntax; }
    ExpressionSyntax(BlockSyntax _BlockSyntax) : _tag(Block) { _Block = _BlockSyntax; }
    ExpressionSyntax(IfSyntax _IfSyntax) : _tag(If) { _If = _IfSyntax; }
    ExpressionSyntax(MatchSyntax _MatchSyntax) : _tag(Match) { _Match = _MatchSyntax; }
    ExpressionSyntax(LambdaSyntax _LambdaSyntax) : _tag(Lambda) { _Lambda = _LambdaSyntax; }
    ExpressionSyntax(ForSyntax _ForSyntax) : _tag(For) { _For = _ForSyntax; }
    ExpressionSyntax(WhileSyntax _WhileSyntax) : _tag(While) { _While = _WhileSyntax; }
    ExpressionSyntax(RepeatSyntax _RepeatSyntax) : _tag(Repeat) { _Repeat = _RepeatSyntax; }
    ExpressionSyntax(SizeOfSyntax _SizeOfSyntax) : _tag(SizeOf) { _SizeOf = _SizeOfSyntax; }
    enum {
        Literal,
        Type,
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
    } _tag;
    union {
        LiteralSyntax _Literal;
        TypeSyntax _Type;
        ObjectSyntax _Object;
        VectorSyntax _Vector;
        BlockSyntax _Block;
        IfSyntax _If;
        MatchSyntax _Match;
        LambdaSyntax _Lambda;
        ForSyntax _For;
        WhileSyntax _While;
        RepeatSyntax _Repeat;
        SizeOfSyntax _SizeOf;
    };
};

struct DropSyntax : Object {
    DropSyntax(size_t start, size_t end, Vector<OperandSyntax>* handler) : start(start), end(end), handler(handler) {}
    size_t start;
    size_t end;
    Vector<OperandSyntax>* handler;
};

struct CatchSyntax : Object {
    CatchSyntax(size_t start, size_t end, Vector<OperandSyntax>* condition, Vector<OperandSyntax>* handler) : start(start), end(end), condition(condition), handler(handler) {}
    size_t start;
    size_t end;
    Vector<OperandSyntax>* condition;
    Vector<OperandSyntax>* handler;
};

struct CatcherSyntax : Object {
    CatcherSyntax(size_t start, size_t end, Vector<CatchSyntax>* catches, DropSyntax* dropper) : start(start), end(end), catches(catches), dropper(dropper) {}
    size_t start;
    size_t end;
    Vector<CatchSyntax>* catches;
    DropSyntax* dropper;
};

struct MemberAccessSyntax : Object {
    MemberAccessSyntax(size_t start, size_t end, NameSyntax member) : start(start), end(end), member(member) {}
    size_t start;
    size_t end;
    NameSyntax member;
};

struct PostfixSyntax : Object {
    PostfixSyntax(MemberAccessSyntax _MemberAccessSyntax) : _tag(MemberAccess) { _MemberAccess = _MemberAccessSyntax; }
    PostfixSyntax(CatcherSyntax _CatcherSyntax) : _tag(Catcher) { _Catcher = _CatcherSyntax; }
    enum {
        MemberAccess,
        Catcher,
    } _tag;
    union {
        MemberAccessSyntax _MemberAccess;
        CatcherSyntax _Catcher;
    };
};

struct OperandSyntax : Object {
    OperandSyntax(size_t start, size_t end, ExpressionSyntax expression, Vector<PostfixSyntax>* postfixes) : start(start), end(end), expression(expression), postfixes(postfixes) {}
    size_t start;
    size_t end;
    ExpressionSyntax expression;
    Vector<PostfixSyntax>* postfixes;
};

struct InitializerSyntax : Object {
    InitializerSyntax(size_t start, size_t end, Vector<OperandSyntax>* operands) : start(start), end(end), operands(operands) {}
    size_t start;
    size_t end;
    Vector<OperandSyntax>* operands;
};

struct PackageSyntax : Object {
    PackageSyntax(size_t start, size_t end, NameSyntax name) : start(start), end(end), name(name) {}
    size_t start;
    size_t end;
    NameSyntax name;
};

struct ModuleSyntax : Object {
    ModuleSyntax(size_t start, size_t end, String name) : start(start), end(end), name(name) {}
    size_t start;
    size_t end;
    String name;
};

struct ModelSyntax : Object {
    ModelSyntax(LiteralSyntax _LiteralSyntax) : _tag(Literal) { _Literal = _LiteralSyntax; }
    ModelSyntax(NameSyntax _NameSyntax) : _tag(Name) { _Name = _NameSyntax; }
    ModelSyntax(ObjectSyntax _ObjectSyntax) : _tag(Object) { _Object = _ObjectSyntax; }
    ModelSyntax(VectorSyntax _VectorSyntax) : _tag(Vector) { _Vector = _VectorSyntax; }
    enum {
        Literal,
        Name,
        Object,
        Vector,
    } _tag;
    union {
        LiteralSyntax _Literal;
        NameSyntax _Name;
        ObjectSyntax _Object;
        VectorSyntax _Vector;
    };
};

struct AttributeSyntax : Object {
    AttributeSyntax(size_t start, size_t end, String name, ModelSyntax model) : start(start), end(end), name(name), model(model) {}
    size_t start;
    size_t end;
    String name;
    ModelSyntax model;
};

struct MacroSyntax : Object {
    MacroSyntax(size_t start, size_t end, String name, ModelSyntax model, Vector<OperandSyntax>* rule) : start(start), end(end), name(name), model(model), rule(rule) {}
    size_t start;
    size_t end;
    String name;
    ModelSyntax model;
    Vector<OperandSyntax>* rule;
};

struct ExtendSyntax : Object {
    ExtendSyntax(size_t start, size_t end, TypeSyntax type) : start(start), end(end), type(type) {}
    size_t start;
    size_t end;
    TypeSyntax type;
};

struct ExtendsSyntax : Object {
    ExtendsSyntax(size_t start, size_t end, Vector<ExtendSyntax>* extensions) : start(start), end(end), extensions(extensions) {}
    size_t start;
    size_t end;
    Vector<ExtendSyntax>* extensions;
};

struct IntrinsicSyntax : Object {
    IntrinsicSyntax(size_t start, size_t end) : start(start), end(end) {}
    size_t start;
    size_t end;
};

struct InstructionSyntax : Object {
    InstructionSyntax(size_t start, size_t end) : start(start), end(end) {}
    size_t start;
    size_t end;
};

struct ExternSyntax : Object {
    ExternSyntax(size_t start, size_t end) : start(start), end(end) {}
    size_t start;
    size_t end;
};

struct ImplementationSyntax : Object {
    ImplementationSyntax(ActionSyntax _ActionSyntax) : _tag(Action) { _Action = _ActionSyntax; }
    ImplementationSyntax(ExternSyntax _ExternSyntax) : _tag(Extern) { _Extern = _ExternSyntax; }
    ImplementationSyntax(InstructionSyntax _InstructionSyntax) : _tag(Instruction) { _Instruction = _InstructionSyntax; }
    ImplementationSyntax(IntrinsicSyntax _IntrinsicSyntax) : _tag(Intrinsic) { _Intrinsic = _IntrinsicSyntax; }
    enum {
        Action,
        Extern,
        Instruction,
        Intrinsic,
    } _tag;
    union {
        ActionSyntax _Action;
        ExternSyntax _Extern;
        InstructionSyntax _Instruction;
        IntrinsicSyntax _Intrinsic;
    };
};

struct RoutineSyntax : Object {
    RoutineSyntax(size_t start, size_t end, GenericArgumentsSyntax* generics, LifetimeSyntax* lifetime, ParameterSetSyntax* parameters, Vector<AttributeSyntax>* attributes, ReturnsSyntax* returns_, ThrowsSyntax* throws_, ImplementationSyntax implementation) : start(start), end(end), generics(generics), lifetime(lifetime), parameters(parameters), attributes(attributes), returns_(returns_), throws_(throws_), implementation(implementation) {}
    size_t start;
    size_t end;
    GenericArgumentsSyntax* generics;
    LifetimeSyntax* lifetime;
    ParameterSetSyntax* parameters;
    Vector<AttributeSyntax>* attributes;
    ReturnsSyntax* returns_;
    ThrowsSyntax* throws_;
    ImplementationSyntax implementation;
};

struct NamedSyntax : Object {
    NamedSyntax(size_t start, size_t end, String name, RoutineSyntax routine) : start(start), end(end), name(name), routine(routine) {}
    size_t start;
    size_t end;
    String name;
    RoutineSyntax routine;
};

struct TargetSyntax : Object {
    TargetSyntax(NamedSyntax _NamedSyntax) : _tag(Named) { _Named = _NamedSyntax; }
    TargetSyntax(RoutineSyntax _RoutineSyntax) : _tag(Routine) { _Routine = _RoutineSyntax; }
    enum {
        Named,
        Routine,
    } _tag;
    union {
        NamedSyntax _Named;
        RoutineSyntax _Routine;
    };
};

struct OperatorSyntax : Object {
    OperatorSyntax(size_t start, size_t end, TargetSyntax target) : start(start), end(end), target(target) {}
    size_t start;
    size_t end;
    TargetSyntax target;
};

struct ProcedureSyntax : Object {
    ProcedureSyntax(size_t start, size_t end, TargetSyntax target) : start(start), end(end), target(target) {}
    size_t start;
    size_t end;
    TargetSyntax target;
};

struct FunctionSyntax : Object {
    FunctionSyntax(size_t start, size_t end, TargetSyntax target) : start(start), end(end), target(target) {}
    size_t start;
    size_t end;
    TargetSyntax target;
};

struct DeInitSyntax : Object {
    DeInitSyntax(size_t start, size_t end, ActionSyntax action) : start(start), end(end), action(action) {}
    size_t start;
    size_t end;
    ActionSyntax action;
};

struct InitSyntax : Object {
    InitSyntax(size_t start, size_t end, ParameterSetSyntax* parameters, ActionSyntax action) : start(start), end(end), parameters(parameters), action(action) {}
    size_t start;
    size_t end;
    ParameterSetSyntax* parameters;
    ActionSyntax action;
};

struct MethodSyntax : Object {
    MethodSyntax(FunctionSyntax _FunctionSyntax) : _tag(Function) { _Function = _FunctionSyntax; }
    MethodSyntax(ProcedureSyntax _ProcedureSyntax) : _tag(Procedure) { _Procedure = _ProcedureSyntax; }
    MethodSyntax(OperatorSyntax _OperatorSyntax) : _tag(Operator) { _Operator = _OperatorSyntax; }
    enum {
        Function,
        Procedure,
        Operator,
    } _tag;
    union {
        FunctionSyntax _Function;
        ProcedureSyntax _Procedure;
        OperatorSyntax _Operator;
    };
};

struct TraitSyntax : Object {
    TraitSyntax(size_t start, size_t end, NameSyntax name, ExtendsSyntax* extension, Vector<AttributeSyntax>* attributes, Vector<UseSyntax>* uses, Vector<MethodSyntax>* functions) : start(start), end(end), name(name), extension(extension), attributes(attributes), uses(uses), functions(functions) {}
    size_t start;
    size_t end;
    NameSyntax name;
    ExtendsSyntax* extension;
    Vector<AttributeSyntax>* attributes;
    Vector<UseSyntax>* uses;
    Vector<MethodSyntax>* functions;
};

struct ImplementSyntax : Object {
    ImplementSyntax(size_t start, size_t end, TypeSyntax type, Vector<AttributeSyntax>* attributes, Vector<UseSyntax>* uses, Vector<MethodSyntax>* methods) : start(start), end(end), type(type), attributes(attributes), uses(uses), methods(methods) {}
    size_t start;
    size_t end;
    TypeSyntax type;
    Vector<AttributeSyntax>* attributes;
    Vector<UseSyntax>* uses;
    Vector<MethodSyntax>* methods;
};

struct UseSyntax : Object {
    UseSyntax(size_t start, size_t end, NameSyntax name) : start(start), end(end), name(name) {}
    size_t start;
    size_t end;
    NameSyntax name;
};

struct ThrowsSyntax : Object {
    ThrowsSyntax(size_t start, size_t end, TypeSyntax type, Vector<AttributeSyntax>* attributes) : start(start), end(end), type(type), attributes(attributes) {}
    size_t start;
    size_t end;
    TypeSyntax type;
    Vector<AttributeSyntax>* attributes;
};

struct ReturnsSyntax : Object {
    ReturnsSyntax(size_t start, size_t end, TypeSyntax type, Vector<AttributeSyntax>* attributes) : start(start), end(end), type(type), attributes(attributes) {}
    size_t start;
    size_t end;
    TypeSyntax type;
    Vector<AttributeSyntax>* attributes;
};

struct ItemSyntax : Object {
    ItemSyntax(size_t start, size_t end, String name, TypeAnnotationSyntax* annotation, Vector<AttributeSyntax>* attributes) : start(start), end(end), name(name), annotation(annotation), attributes(attributes) {}
    size_t start;
    size_t end;
    String name;
    TypeAnnotationSyntax* annotation;
    Vector<AttributeSyntax>* attributes;
};

struct ParametersSyntax : Object {
    ParametersSyntax(size_t start, size_t end, Vector<ItemSyntax>* items) : start(start), end(end), items(items) {}
    size_t start;
    size_t end;
    Vector<ItemSyntax>* items;
};

struct ParameterSetSyntax : Object {
    ParameterSetSyntax(ParametersSyntax _ParametersSyntax) : _tag(Parameters) { _Parameters = _ParametersSyntax; }
    ParameterSetSyntax(TypeSyntax _TypeSyntax) : _tag(Type) { _Type = _TypeSyntax; }
    enum {
        Parameters,
        Type,
    } _tag;
    union {
        ParametersSyntax _Parameters;
        TypeSyntax _Type;
    };
};

struct OptionalSyntax : Object {
    OptionalSyntax(size_t start, size_t end) : start(start), end(end) {}
    size_t start;
    size_t end;
};

struct GenericArgumentSyntax : Object {
    GenericArgumentSyntax(size_t start, size_t end, TypeSyntax type) : start(start), end(end), type(type) {}
    size_t start;
    size_t end;
    TypeSyntax type;
};

struct GenericArgumentsSyntax : Object {
    GenericArgumentsSyntax(size_t start, size_t end, Vector<GenericArgumentSyntax>* generics) : start(start), end(end), generics(generics) {}
    size_t start;
    size_t end;
    Vector<GenericArgumentSyntax>* generics;
};

struct TypeAnnotationSyntax : Object {
    TypeAnnotationSyntax(size_t start, size_t end, TypeSyntax type) : start(start), end(end), type(type) {}
    size_t start;
    size_t end;
    TypeSyntax type;
};

struct DelegateSyntax : Object {
    DelegateSyntax(size_t start, size_t end, ParameterSetSyntax* parameters, Vector<AttributeSyntax>* attributes, ReturnsSyntax* result, ThrowsSyntax* error) : start(start), end(end), parameters(parameters), attributes(attributes), result(result), error(error) {}
    size_t start;
    size_t end;
    ParameterSetSyntax* parameters;
    Vector<AttributeSyntax>* attributes;
    ReturnsSyntax* result;
    ThrowsSyntax* error;
};

struct ConstantSyntax : Object {
    ConstantSyntax(size_t start, size_t end, TypeSyntax type, Vector<OperandSyntax>* operation) : start(start), end(end), type(type), operation(operation) {}
    size_t start;
    size_t end;
    TypeSyntax type;
    Vector<OperandSyntax>* operation;
};

struct VariantSyntax : Object {
    VariantSyntax(size_t start, size_t end, String name, Vector<AttributeSyntax>* attributes, TypeAnnotationSyntax* annotation) : start(start), end(end), name(name), attributes(attributes), annotation(annotation) {}
    size_t start;
    size_t end;
    String name;
    Vector<AttributeSyntax>* attributes;
    TypeAnnotationSyntax* annotation;
};

struct UnionSyntax : Object {
    UnionSyntax(size_t start, size_t end, Vector<VariantSyntax>* variants) : start(start), end(end), variants(variants) {}
    size_t start;
    size_t end;
    Vector<VariantSyntax>* variants;
};

struct NamespaceSyntax : Object {
    NamespaceSyntax(size_t start, size_t end, Vector<UseSyntax>* uses, Vector<DeclarationSyntax>* declarations) : start(start), end(end), uses(uses), declarations(declarations) {}
    size_t start;
    size_t end;
    Vector<UseSyntax>* uses;
    Vector<DeclarationSyntax>* declarations;
};

struct BodySyntax : Object {
    BodySyntax(size_t start, size_t end, Vector<UseSyntax>* uses, Vector<InitSyntax>* inits, DeInitSyntax* deInit, Vector<MemberSyntax>* members) : start(start), end(end), uses(uses), inits(inits), deInit(deInit), members(members) {}
    size_t start;
    size_t end;
    Vector<UseSyntax>* uses;
    Vector<InitSyntax>* inits;
    DeInitSyntax* deInit;
    Vector<MemberSyntax>* members;
};

struct ClassSyntax : Object {
    ClassSyntax(size_t start, size_t end, StructureSyntax structure, BodySyntax* body) : start(start), end(end), structure(structure), body(body) {}
    size_t start;
    size_t end;
    StructureSyntax structure;
    BodySyntax* body;
};

struct ConceptSyntax : Object {
    ConceptSyntax(ClassSyntax _ClassSyntax) : _tag(Class) { _Class = _ClassSyntax; }
    ConceptSyntax(NamespaceSyntax _NamespaceSyntax) : _tag(Namespace) { _Namespace = _NamespaceSyntax; }
    ConceptSyntax(UnionSyntax _UnionSyntax) : _tag(Union) { _Union = _UnionSyntax; }
    ConceptSyntax(ConstantSyntax _ConstantSyntax) : _tag(Constant) { _Constant = _ConstantSyntax; }
    ConceptSyntax(DelegateSyntax _DelegateSyntax) : _tag(Delegate) { _Delegate = _DelegateSyntax; }
    ConceptSyntax(IntrinsicSyntax _IntrinsicSyntax) : _tag(Intrinsic) { _Intrinsic = _IntrinsicSyntax; }
    enum {
        Class,
        Namespace,
        Union,
        Constant,
        Delegate,
        Intrinsic,
    } _tag;
    union {
        ClassSyntax _Class;
        NamespaceSyntax _Namespace;
        UnionSyntax _Union;
        ConstantSyntax _Constant;
        DelegateSyntax _Delegate;
        IntrinsicSyntax _Intrinsic;
    };
};

struct GenericParameterSyntax : Object {
    GenericParameterSyntax(size_t start, size_t end, String name, Vector<AttributeSyntax>* attributes) : start(start), end(end), name(name), attributes(attributes) {}
    size_t start;
    size_t end;
    String name;
    Vector<AttributeSyntax>* attributes;
};

struct GenericParametersSyntax : Object {
    GenericParametersSyntax(size_t start, size_t end, Vector<GenericParameterSyntax>* parameters) : start(start), end(end), parameters(parameters) {}
    size_t start;
    size_t end;
    Vector<GenericParameterSyntax>* parameters;
};

struct DefinitionSyntax : Object {
    DefinitionSyntax(size_t start, size_t end, String name, GenericParametersSyntax* parameters, Vector<AttributeSyntax>* attributes, ConceptSyntax concept_) : start(start), end(end), name(name), parameters(parameters), attributes(attributes), concept_(concept_) {}
    size_t start;
    size_t end;
    String name;
    GenericParametersSyntax* parameters;
    Vector<AttributeSyntax>* attributes;
    ConceptSyntax concept_;
};

struct ConstituentSyntax : Object {
    ConstituentSyntax(DefinitionSyntax _DefinitionSyntax) : _tag(Definition) { _Definition = _DefinitionSyntax; }
    ConstituentSyntax(FunctionSyntax _FunctionSyntax) : _tag(Function) { _Function = _FunctionSyntax; }
    ConstituentSyntax(ProcedureSyntax _ProcedureSyntax) : _tag(Procedure) { _Procedure = _ProcedureSyntax; }
    ConstituentSyntax(OperatorSyntax _OperatorSyntax) : _tag(Operator) { _Operator = _OperatorSyntax; }
    ConstituentSyntax(ImplementSyntax _ImplementSyntax) : _tag(Implement) { _Implement = _ImplementSyntax; }
    ConstituentSyntax(TraitSyntax _TraitSyntax) : _tag(Trait) { _Trait = _TraitSyntax; }
    ConstituentSyntax(MacroSyntax _MacroSyntax) : _tag(Macro) { _Macro = _MacroSyntax; }
    ConstituentSyntax(ModuleSyntax _ModuleSyntax) : _tag(Module) { _Module = _ModuleSyntax; }
    enum {
        Definition,
        Function,
        Procedure,
        Operator,
        Implement,
        Trait,
        Macro,
        Module,
    } _tag;
    union {
        DefinitionSyntax _Definition;
        FunctionSyntax _Function;
        ProcedureSyntax _Procedure;
        OperatorSyntax _Operator;
        ImplementSyntax _Implement;
        TraitSyntax _Trait;
        MacroSyntax _Macro;
        ModuleSyntax _Module;
    };
};

struct MemberSyntax : Object {
    MemberSyntax(size_t start, size_t end, ConstituentSyntax constituent) : start(start), end(end), constituent(constituent) {}
    size_t start;
    size_t end;
    ConstituentSyntax constituent;
};

struct ExportSyntax : Object {
    ExportSyntax(DefinitionSyntax _DefinitionSyntax) : _tag(Definition) { _Definition = _DefinitionSyntax; }
    ExportSyntax(FunctionSyntax _FunctionSyntax) : _tag(Function) { _Function = _FunctionSyntax; }
    ExportSyntax(OperatorSyntax _OperatorSyntax) : _tag(Operator) { _Operator = _OperatorSyntax; }
    ExportSyntax(TraitSyntax _TraitSyntax) : _tag(Trait) { _Trait = _TraitSyntax; }
    ExportSyntax(ModuleSyntax _ModuleSyntax) : _tag(Module) { _Module = _ModuleSyntax; }
    enum {
        Definition,
        Function,
        Operator,
        Trait,
        Module,
    } _tag;
    union {
        DefinitionSyntax _Definition;
        FunctionSyntax _Function;
        OperatorSyntax _Operator;
        TraitSyntax _Trait;
        ModuleSyntax _Module;
    };
};

struct PrivateSyntax : Object {
    PrivateSyntax(size_t start, size_t end, ExportSyntax export_) : start(start), end(end), export_(export_) {}
    size_t start;
    size_t end;
    ExportSyntax export_;
};

struct SymbolSyntax : Object {
    SymbolSyntax(PrivateSyntax _PrivateSyntax) : _tag(Private) { _Private = _PrivateSyntax; }
    SymbolSyntax(DefinitionSyntax _DefinitionSyntax) : _tag(Definition) { _Definition = _DefinitionSyntax; }
    SymbolSyntax(FunctionSyntax _FunctionSyntax) : _tag(Function) { _Function = _FunctionSyntax; }
    SymbolSyntax(OperatorSyntax _OperatorSyntax) : _tag(Operator) { _Operator = _OperatorSyntax; }
    SymbolSyntax(TraitSyntax _TraitSyntax) : _tag(Trait) { _Trait = _TraitSyntax; }
    SymbolSyntax(MacroSyntax _MacroSyntax) : _tag(Macro) { _Macro = _MacroSyntax; }
    SymbolSyntax(ModuleSyntax _ModuleSyntax) : _tag(Module) { _Module = _ModuleSyntax; }
    enum {
        Private,
        Definition,
        Function,
        Operator,
        Trait,
        Macro,
        Module,
    } _tag;
    union {
        PrivateSyntax _Private;
        DefinitionSyntax _Definition;
        FunctionSyntax _Function;
        OperatorSyntax _Operator;
        TraitSyntax _Trait;
        MacroSyntax _Macro;
        ModuleSyntax _Module;
    };
};

struct DeclarationSyntax : Object {
    DeclarationSyntax(size_t start, size_t end, SymbolSyntax symbol) : start(start), end(end), symbol(symbol) {}
    size_t start;
    size_t end;
    SymbolSyntax symbol;
};

struct FileSyntax : Object {
    FileSyntax(size_t start, size_t end, Vector<PackageSyntax>* packages, Vector<UseSyntax>* uses, Vector<DeclarationSyntax>* declarations) : start(start), end(end), packages(packages), uses(uses), declarations(declarations) {}
    size_t start;
    size_t end;
    Vector<PackageSyntax>* packages;
    Vector<UseSyntax>* uses;
    Vector<DeclarationSyntax>* declarations;
};

struct ProgramSyntax : Object {
    ProgramSyntax(size_t start, size_t end, FileSyntax file, Vector<StatementSyntax>* statements) : start(start), end(end), file(file), statements(statements) {}
    size_t start;
    size_t end;
    FileSyntax file;
    Vector<StatementSyntax>* statements;
};

using namespace scaly::containers;

struct Parser : Object {
    Lexer lexer;
    Vector<String> keywords_index;
    HashSet<String> keywords;

    Parser(String text)
      : lexer(Lexer(text)),
        keywords_index(initialize_keywords_index(Page::get(this))),
        keywords(initialize_keywords(Page::get(this))) {}

    Vector<String> initialize_keywords_index(Page* _rp) {
        Region _r;
        Array<String>& keywords_builder = *new(alignof(Array<String>), _r.get_page()) Array<String>();
        auto p = Page::get(this);
        keywords_builder.add(String(p, "break"));
        keywords_builder.add(String(p, "catch"));
        keywords_builder.add(String(p, "case"));
        keywords_builder.add(String(p, "continue"));
        keywords_builder.add(String(p, "define"));
        keywords_builder.add(String(p, "default"));
        keywords_builder.add(String(p, "deinit"));
        keywords_builder.add(String(p, "delegate"));
        keywords_builder.add(String(p, "drop"));
        keywords_builder.add(String(p, "else"));
        keywords_builder.add(String(p, "extends"));
        keywords_builder.add(String(p, "extern"));
        keywords_builder.add(String(p, "for"));
        keywords_builder.add(String(p, "function"));
        keywords_builder.add(String(p, "if"));
        keywords_builder.add(String(p, "init"));
        keywords_builder.add(String(p, "implement"));
        keywords_builder.add(String(p, "in"));
        keywords_builder.add(String(p, "instruction"));
        keywords_builder.add(String(p, "intrinsic"));
        keywords_builder.add(String(p, "label"));
        keywords_builder.add(String(p, "lambda"));
        keywords_builder.add(String(p, "let"));
        keywords_builder.add(String(p, "loop"));
        keywords_builder.add(String(p, "macro"));
        keywords_builder.add(String(p, "match"));
        keywords_builder.add(String(p, "module"));
        keywords_builder.add(String(p, "mutable"));
        keywords_builder.add(String(p, "operator"));
        keywords_builder.add(String(p, "procedure"));
        keywords_builder.add(String(p, "private"));
        keywords_builder.add(String(p, "return"));
        keywords_builder.add(String(p, "returns"));
        keywords_builder.add(String(p, "repeat"));
        keywords_builder.add(String(p, "set"));
        keywords_builder.add(String(p, "sizeof"));
        keywords_builder.add(String(p, "throw"));
        keywords_builder.add(String(p, "throws"));
        keywords_builder.add(String(p, "trait"));
        keywords_builder.add(String(p, "union"));
        keywords_builder.add(String(p, "use"));
        keywords_builder.add(String(p, "var"));
        keywords_builder.add(String(p, "while"));
        keywords_builder.add(String(p, "package"));
        return Vector<String>(_rp, keywords_builder);
    }

    HashSet<String> initialize_keywords(Page* _rp) {
        Region _r;
        HashSetBuilder<String>& hash_set_builder = *new(alignof(HashSetBuilder<String>), _r.get_page()) HashSetBuilder<String>(this->keywords_index);
        return HashSet<String>(_rp, hash_set_builder);
    }

    Result<Literal, ParserError> parse_literal_token(Page* _rp) {
        if (this->lexer.token._tag == Token::Empty)
            lexer.advance();

        switch (this->lexer.token._tag)
        {
            case Token::Literal:
                switch (this->lexer.token._Literal._tag)
                {
                    case LiteralToken::String:
                    {
                        auto ret = Literal(StringLiteral(String(_rp, this->lexer.token._Literal._String.value)));
                        this->lexer.empty();
                        return Result<Literal, ParserError> { ._tag = Result<Literal, ParserError>::Ok, ._Ok = ret };
                    }

                    case LiteralToken::Character:
                    {
                        auto ret = Literal(CharacterLiteral(String(_rp, this->lexer.token._Literal._String.value)));
                        this->lexer.empty();
                        return Result<Literal, ParserError> { ._tag = Result<Literal, ParserError>::Ok, ._Ok = ret };
                    }

                    case LiteralToken::Integer:
                    {
                        auto ret = Literal(IntegerLiteral(String(_rp, this->lexer.token._Literal._String.value)));
                        this->lexer.empty();
                        return Result<Literal, ParserError> { ._tag = Result<Literal, ParserError>::Ok, ._Ok = ret };
                    }

                    case LiteralToken::FloatingPoint:
                    {
                        auto ret = Literal(FloatingPointLiteral(String(_rp, this->lexer.token._Literal._String.value)));
                        this->lexer.empty();
                        return Result<Literal, ParserError> { ._tag = Result<Literal, ParserError>::Ok, ._Ok = ret };
                    }

                    case LiteralToken::Hex:
                    {
                        auto ret = Literal(HexLiteral(String(_rp, this->lexer.token._Literal._String.value)));
                        this->lexer.empty();
                        return Result<Literal, ParserError> { ._tag = Result<Literal, ParserError>::Ok, ._Ok = ret };
                    }

                    case LiteralToken::Boolean:
                    {
                        auto ret = Literal(BooleanLiteral(this->lexer.token._Literal._Boolean.value));
                        this->lexer.empty();
                        return Result<Literal, ParserError> { ._tag = Result<Literal, ParserError>::Ok, ._Ok = ret };
                    }

                    case LiteralToken::Fragment:
                    {
                        auto ret = Literal(FragmentLiteral(String(_rp, this->lexer.token._Literal._String.value)));
                        this->lexer.empty();
                        return Result<Literal, ParserError> { ._tag = Result<Literal, ParserError>::Ok, ._Ok = ret };
                    }
                }
            default:
                return Result<Literal, ParserError> { ._tag = Result<Literal, ParserError>::Error, ._Error = ParserError(OtherSyntax()) };
        }
    }

    Result<ProgramSyntax, ParserError> parse_program(Page* _rp, Page* _ep) {
        auto start = this->lexer.previous_position;

        auto file_start = this->lexer.position;
        auto file_result = this->parse_file(_rp, _ep);
        if (file_result._tag == Result<FileSyntax, ParserError>::Error)
        {
            return Result<ProgramSyntax, ParserError> { ._tag = Result<ProgramSyntax, ParserError>::Error, ._Error = file_result._Error };
        }

        auto file = file_result._Ok;

        auto statements_start = this->lexer.position;
        auto statements_result = this->parse_statement_list(_rp, _ep);
        if (statements_result._tag == Result<Vector<StatementSyntax>, ParserError>::Error)
        {
            switch (statements_result._Error._tag) {
                case ParserError::OtherSyntax:
                    break;
                case ParserError::InvalidSyntax:
                    return Result<ProgramSyntax, ParserError> { ._tag = Result<ProgramSyntax, ParserError>::Error, ._Error = statements_result._Error };
            }
        }

        auto statements = statements_result._tag == Result<Vector<StatementSyntax>, ParserError>::Error ? nullptr : statements_result._Ok;

        auto end = this->lexer.position;

        auto ret = ProgramSyntax(start, end, file, statements);

        return Result<ProgramSyntax, ParserError> { ._tag = Result<ProgramSyntax, ParserError>::Ok, ._Ok = ret };
    }

    Result<FileSyntax, ParserError> parse_file(Page* _rp, Page* _ep) {
        auto start = this->lexer.previous_position;

        auto packages_start = this->lexer.position;
        auto packages_result = this->parse_package_list(_rp, _ep);
        if (packages_result._tag == Result<Vector<PackageSyntax>, ParserError>::Error)
        {
            switch (packages_result._Error._tag) {
                case ParserError::OtherSyntax:
                    break;
                case ParserError::InvalidSyntax:
                    return Result<FileSyntax, ParserError> { ._tag = Result<FileSyntax, ParserError>::Error, ._Error = packages_result._Error };
            }
        }

        auto packages = packages_result._tag == Result<Vector<PackageSyntax>, ParserError>::Error ? nullptr : packages_result._Ok;

        auto uses_start = this->lexer.position;
        auto uses_result = this->parse_use_list(_rp, _ep);
        if (uses_result._tag == Result<Vector<UseSyntax>, ParserError>::Error)
        {
            switch (uses_result._Error._tag) {
                case ParserError::OtherSyntax:
                    break;
                case ParserError::InvalidSyntax:
                    return Result<FileSyntax, ParserError> { ._tag = Result<FileSyntax, ParserError>::Error, ._Error = uses_result._Error };
            }
        }

        auto uses = uses_result._tag == Result<Vector<UseSyntax>, ParserError>::Error ? nullptr : uses_result._Ok;

        auto declarations_start = this->lexer.position;
        auto declarations_result = this->parse_declaration_list(_rp, _ep);
        if (declarations_result._tag == Result<Vector<DeclarationSyntax>, ParserError>::Error)
        {
            switch (declarations_result._Error._tag) {
                case ParserError::OtherSyntax:
                    break;
                case ParserError::InvalidSyntax:
                    return Result<FileSyntax, ParserError> { ._tag = Result<FileSyntax, ParserError>::Error, ._Error = declarations_result._Error };
            }
        }

        auto declarations = declarations_result._tag == Result<Vector<DeclarationSyntax>, ParserError>::Error ? nullptr : declarations_result._Ok;

        auto end = this->lexer.position;

        auto ret = FileSyntax(start, end, packages, uses, declarations);

        return Result<FileSyntax, ParserError> { ._tag = Result<FileSyntax, ParserError>::Ok, ._Ok = ret };
    }

    Result<Vector<DeclarationSyntax>*, ParserError> parse_declaration_list(Page* _rp, Page* _ep) {
        Region _r;
        List<DeclarationSyntax>& list = *new(alignof(List<DeclarationSyntax>), _r.get_page()) List<DeclarationSyntax>();;
        while(true) {
            auto node_result = this->parse_declaration(_rp, _ep);
            if ((node_result._tag == Result<DeclarationSyntax, ParserError>::Error) && (node_result._Error._tag == ParserError::InvalidSyntax))
                return Result<Vector<DeclarationSyntax>*, ParserError> { ._tag = Result<Vector<DeclarationSyntax>*, ParserError>::Error, ._Error = node_result._Error };
            if (node_result._tag == Result<DeclarationSyntax, ParserError>::Ok) {
                auto node = node_result._Ok;
                list.add(node);
            } else {
                if ((list.count() == 0) && (node_result._tag == Result<DeclarationSyntax, ParserError>::Error) && (node_result._Error._tag == ParserError::OtherSyntax))
                    return Result<Vector<DeclarationSyntax>*, ParserError> { ._tag = Result<Vector<DeclarationSyntax>*, ParserError>::Error, ._Error = node_result._Error };
                break;
            }
        }

        if (list.count() == 0)
            return Result<Vector<DeclarationSyntax>*, ParserError> { ._tag = Result<Vector<DeclarationSyntax>*, ParserError>::Ok, ._Ok = nullptr };
        
        return Result<Vector<DeclarationSyntax>*, ParserError> {
            ._tag = Result<Vector<DeclarationSyntax>*, ParserError>::Ok,
            ._Ok = new(alignof(Vector<DeclarationSyntax>), _rp) Vector<DeclarationSyntax>(_rp, list) };
    }

    Result<DeclarationSyntax, ParserError> parse_declaration(Page* _rp, Page* _ep) {
        auto start = this->lexer.previous_position;

        auto symbol_start = this->lexer.position;
        auto symbol_result = this->parse_symbol(_rp, _ep);
        if (symbol_result._tag == Result<SymbolSyntax, ParserError>::Error)
        {
            return Result<DeclarationSyntax, ParserError> { ._tag = Result<DeclarationSyntax, ParserError>::Error, ._Error = symbol_result._Error };
        }

        auto symbol = symbol_result._Ok;

        auto start_colon_2 = this->lexer.previous_position;
        auto success_colon_2 = this->lexer.parse_colon(_rp);
        if (!success_colon_2) {
        }

        auto end = this->lexer.position;

        auto ret = DeclarationSyntax(start, end, symbol);

        return Result<DeclarationSyntax, ParserError> { ._tag = Result<DeclarationSyntax, ParserError>::Ok, ._Ok = ret };
    }

    Result<SymbolSyntax, ParserError> parse_symbol(Page* _rp, Page* _ep) {
        {
            auto node_result = this->parse_private(_rp, _ep);
            if (node_result._tag == Result<PrivateSyntax, ParserError>::Error)
            {
                if (node_result._Error._tag == ParserError::InvalidSyntax)
                    return Result<SymbolSyntax, ParserError> { ._tag = Result<SymbolSyntax, ParserError>::Error, ._Error = node_result._Error };
            }
            else
            {
                auto node = node_result._Ok;
                return Result<SymbolSyntax, ParserError> { ._tag = Result<SymbolSyntax, ParserError>::Ok, ._Ok = 
                    SymbolSyntax(PrivateSyntax(node))
                };
            }
        }
        {
            auto node_result = this->parse_definition(_rp, _ep);
            if (node_result._tag == Result<DefinitionSyntax, ParserError>::Error)
            {
                if (node_result._Error._tag == ParserError::InvalidSyntax)
                    return Result<SymbolSyntax, ParserError> { ._tag = Result<SymbolSyntax, ParserError>::Error, ._Error = node_result._Error };
            }
            else
            {
                auto node = node_result._Ok;
                return Result<SymbolSyntax, ParserError> { ._tag = Result<SymbolSyntax, ParserError>::Ok, ._Ok = 
                    SymbolSyntax(DefinitionSyntax(node))
                };
            }
        }
        {
            auto node_result = this->parse_function(_rp, _ep);
            if (node_result._tag == Result<FunctionSyntax, ParserError>::Error)
            {
                if (node_result._Error._tag == ParserError::InvalidSyntax)
                    return Result<SymbolSyntax, ParserError> { ._tag = Result<SymbolSyntax, ParserError>::Error, ._Error = node_result._Error };
            }
            else
            {
                auto node = node_result._Ok;
                return Result<SymbolSyntax, ParserError> { ._tag = Result<SymbolSyntax, ParserError>::Ok, ._Ok = 
                    SymbolSyntax(FunctionSyntax(node))
                };
            }
        }
        {
            auto node_result = this->parse_operator(_rp, _ep);
            if (node_result._tag == Result<OperatorSyntax, ParserError>::Error)
            {
                if (node_result._Error._tag == ParserError::InvalidSyntax)
                    return Result<SymbolSyntax, ParserError> { ._tag = Result<SymbolSyntax, ParserError>::Error, ._Error = node_result._Error };
            }
            else
            {
                auto node = node_result._Ok;
                return Result<SymbolSyntax, ParserError> { ._tag = Result<SymbolSyntax, ParserError>::Ok, ._Ok = 
                    SymbolSyntax(OperatorSyntax(node))
                };
            }
        }
        {
            auto node_result = this->parse_trait(_rp, _ep);
            if (node_result._tag == Result<TraitSyntax, ParserError>::Error)
            {
                if (node_result._Error._tag == ParserError::InvalidSyntax)
                    return Result<SymbolSyntax, ParserError> { ._tag = Result<SymbolSyntax, ParserError>::Error, ._Error = node_result._Error };
            }
            else
            {
                auto node = node_result._Ok;
                return Result<SymbolSyntax, ParserError> { ._tag = Result<SymbolSyntax, ParserError>::Ok, ._Ok = 
                    SymbolSyntax(TraitSyntax(node))
                };
            }
        }
        {
            auto node_result = this->parse_macro(_rp, _ep);
            if (node_result._tag == Result<MacroSyntax, ParserError>::Error)
            {
                if (node_result._Error._tag == ParserError::InvalidSyntax)
                    return Result<SymbolSyntax, ParserError> { ._tag = Result<SymbolSyntax, ParserError>::Error, ._Error = node_result._Error };
            }
            else
            {
                auto node = node_result._Ok;
                return Result<SymbolSyntax, ParserError> { ._tag = Result<SymbolSyntax, ParserError>::Ok, ._Ok = 
                    SymbolSyntax(MacroSyntax(node))
                };
            }
        }
        {
            auto node_result = this->parse_module(_rp, _ep);
            if (node_result._tag == Result<ModuleSyntax, ParserError>::Error)
            {
                if (node_result._Error._tag == ParserError::InvalidSyntax)
                    return Result<SymbolSyntax, ParserError> { ._tag = Result<SymbolSyntax, ParserError>::Error, ._Error = node_result._Error };
            }
            else
            {
                auto node = node_result._Ok;
                return Result<SymbolSyntax, ParserError> { ._tag = Result<SymbolSyntax, ParserError>::Ok, ._Ok = 
                    SymbolSyntax(ModuleSyntax(node))
                };
            }
        }
        return Result<SymbolSyntax, ParserError> { ._tag = Result<SymbolSyntax, ParserError>::Error, ._Error = ParserError(OtherSyntax()) };
    }

    Result<PrivateSyntax, ParserError> parse_private(Page* _rp, Page* _ep) {
        auto start = this->lexer.previous_position;

        auto start_private_1 = this->lexer.previous_position;
        auto success_private_1 = this->lexer.parse_keyword(_rp, *this->keywords_index[30]);
        if (!success_private_1) {
            return Result<PrivateSyntax, ParserError> { ._tag = Result<PrivateSyntax, ParserError>::Error, ._Error = ParserError(OtherSyntax()) };
        }

        auto export__start = this->lexer.position;
        auto export__result = this->parse_export(_rp, _ep);
        if (export__result._tag == Result<ExportSyntax, ParserError>::Error)
        {
            switch (export__result._Error._tag) {
                case ParserError::OtherSyntax:
                    return Result<PrivateSyntax, ParserError> { ._tag = Result<PrivateSyntax, ParserError>::Error, ._Error = ParserError(InvalidSyntax(export__start, lexer.position, String(_ep, "a valid Export syntax"))) };
                case ParserError::InvalidSyntax:
                    return Result<PrivateSyntax, ParserError> { ._tag = Result<PrivateSyntax, ParserError>::Error, ._Error = export__result._Error };
            }
        }

        auto export_ = export__result._Ok;

        auto end = this->lexer.position;

        auto ret = PrivateSyntax(start, end, export_);

        return Result<PrivateSyntax, ParserError> { ._tag = Result<PrivateSyntax, ParserError>::Ok, ._Ok = ret };
    }

    Result<ExportSyntax, ParserError> parse_export(Page* _rp, Page* _ep) {
        {
            auto node_result = this->parse_definition(_rp, _ep);
            if (node_result._tag == Result<DefinitionSyntax, ParserError>::Error)
            {
                if (node_result._Error._tag == ParserError::InvalidSyntax)
                    return Result<ExportSyntax, ParserError> { ._tag = Result<ExportSyntax, ParserError>::Error, ._Error = node_result._Error };
            }
            else
            {
                auto node = node_result._Ok;
                return Result<ExportSyntax, ParserError> { ._tag = Result<ExportSyntax, ParserError>::Ok, ._Ok = 
                    ExportSyntax(DefinitionSyntax(node))
                };
            }
        }
        {
            auto node_result = this->parse_function(_rp, _ep);
            if (node_result._tag == Result<FunctionSyntax, ParserError>::Error)
            {
                if (node_result._Error._tag == ParserError::InvalidSyntax)
                    return Result<ExportSyntax, ParserError> { ._tag = Result<ExportSyntax, ParserError>::Error, ._Error = node_result._Error };
            }
            else
            {
                auto node = node_result._Ok;
                return Result<ExportSyntax, ParserError> { ._tag = Result<ExportSyntax, ParserError>::Ok, ._Ok = 
                    ExportSyntax(FunctionSyntax(node))
                };
            }
        }
        {
            auto node_result = this->parse_operator(_rp, _ep);
            if (node_result._tag == Result<OperatorSyntax, ParserError>::Error)
            {
                if (node_result._Error._tag == ParserError::InvalidSyntax)
                    return Result<ExportSyntax, ParserError> { ._tag = Result<ExportSyntax, ParserError>::Error, ._Error = node_result._Error };
            }
            else
            {
                auto node = node_result._Ok;
                return Result<ExportSyntax, ParserError> { ._tag = Result<ExportSyntax, ParserError>::Ok, ._Ok = 
                    ExportSyntax(OperatorSyntax(node))
                };
            }
        }
        {
            auto node_result = this->parse_trait(_rp, _ep);
            if (node_result._tag == Result<TraitSyntax, ParserError>::Error)
            {
                if (node_result._Error._tag == ParserError::InvalidSyntax)
                    return Result<ExportSyntax, ParserError> { ._tag = Result<ExportSyntax, ParserError>::Error, ._Error = node_result._Error };
            }
            else
            {
                auto node = node_result._Ok;
                return Result<ExportSyntax, ParserError> { ._tag = Result<ExportSyntax, ParserError>::Ok, ._Ok = 
                    ExportSyntax(TraitSyntax(node))
                };
            }
        }
        {
            auto node_result = this->parse_module(_rp, _ep);
            if (node_result._tag == Result<ModuleSyntax, ParserError>::Error)
            {
                if (node_result._Error._tag == ParserError::InvalidSyntax)
                    return Result<ExportSyntax, ParserError> { ._tag = Result<ExportSyntax, ParserError>::Error, ._Error = node_result._Error };
            }
            else
            {
                auto node = node_result._Ok;
                return Result<ExportSyntax, ParserError> { ._tag = Result<ExportSyntax, ParserError>::Ok, ._Ok = 
                    ExportSyntax(ModuleSyntax(node))
                };
            }
        }
        return Result<ExportSyntax, ParserError> { ._tag = Result<ExportSyntax, ParserError>::Error, ._Error = ParserError(OtherSyntax()) };
    }

    Result<Vector<MemberSyntax>*, ParserError> parse_member_list(Page* _rp, Page* _ep) {
        Region _r;
        List<MemberSyntax>& list = *new(alignof(List<MemberSyntax>), _r.get_page()) List<MemberSyntax>();;
        while(true) {
            auto node_result = this->parse_member(_rp, _ep);
            if ((node_result._tag == Result<MemberSyntax, ParserError>::Error) && (node_result._Error._tag == ParserError::InvalidSyntax))
                return Result<Vector<MemberSyntax>*, ParserError> { ._tag = Result<Vector<MemberSyntax>*, ParserError>::Error, ._Error = node_result._Error };
            if (node_result._tag == Result<MemberSyntax, ParserError>::Ok) {
                auto node = node_result._Ok;
                list.add(node);
            } else {
                if ((list.count() == 0) && (node_result._tag == Result<MemberSyntax, ParserError>::Error) && (node_result._Error._tag == ParserError::OtherSyntax))
                    return Result<Vector<MemberSyntax>*, ParserError> { ._tag = Result<Vector<MemberSyntax>*, ParserError>::Error, ._Error = node_result._Error };
                break;
            }
        }

        if (list.count() == 0)
            return Result<Vector<MemberSyntax>*, ParserError> { ._tag = Result<Vector<MemberSyntax>*, ParserError>::Ok, ._Ok = nullptr };
        
        return Result<Vector<MemberSyntax>*, ParserError> {
            ._tag = Result<Vector<MemberSyntax>*, ParserError>::Ok,
            ._Ok = new(alignof(Vector<MemberSyntax>), _rp) Vector<MemberSyntax>(_rp, list) };
    }

    Result<MemberSyntax, ParserError> parse_member(Page* _rp, Page* _ep) {
        auto start = this->lexer.previous_position;

        auto constituent_start = this->lexer.position;
        auto constituent_result = this->parse_constituent(_rp, _ep);
        if (constituent_result._tag == Result<ConstituentSyntax, ParserError>::Error)
        {
            return Result<MemberSyntax, ParserError> { ._tag = Result<MemberSyntax, ParserError>::Error, ._Error = constituent_result._Error };
        }

        auto constituent = constituent_result._Ok;

        auto start_colon_2 = this->lexer.previous_position;
        auto success_colon_2 = this->lexer.parse_colon(_rp);
        if (!success_colon_2) {
        }

        auto end = this->lexer.position;

        auto ret = MemberSyntax(start, end, constituent);

        return Result<MemberSyntax, ParserError> { ._tag = Result<MemberSyntax, ParserError>::Ok, ._Ok = ret };
    }

    Result<ConstituentSyntax, ParserError> parse_constituent(Page* _rp, Page* _ep) {
        {
            auto node_result = this->parse_definition(_rp, _ep);
            if (node_result._tag == Result<DefinitionSyntax, ParserError>::Error)
            {
                if (node_result._Error._tag == ParserError::InvalidSyntax)
                    return Result<ConstituentSyntax, ParserError> { ._tag = Result<ConstituentSyntax, ParserError>::Error, ._Error = node_result._Error };
            }
            else
            {
                auto node = node_result._Ok;
                return Result<ConstituentSyntax, ParserError> { ._tag = Result<ConstituentSyntax, ParserError>::Ok, ._Ok = 
                    ConstituentSyntax(DefinitionSyntax(node))
                };
            }
        }
        {
            auto node_result = this->parse_function(_rp, _ep);
            if (node_result._tag == Result<FunctionSyntax, ParserError>::Error)
            {
                if (node_result._Error._tag == ParserError::InvalidSyntax)
                    return Result<ConstituentSyntax, ParserError> { ._tag = Result<ConstituentSyntax, ParserError>::Error, ._Error = node_result._Error };
            }
            else
            {
                auto node = node_result._Ok;
                return Result<ConstituentSyntax, ParserError> { ._tag = Result<ConstituentSyntax, ParserError>::Ok, ._Ok = 
                    ConstituentSyntax(FunctionSyntax(node))
                };
            }
        }
        {
            auto node_result = this->parse_procedure(_rp, _ep);
            if (node_result._tag == Result<ProcedureSyntax, ParserError>::Error)
            {
                if (node_result._Error._tag == ParserError::InvalidSyntax)
                    return Result<ConstituentSyntax, ParserError> { ._tag = Result<ConstituentSyntax, ParserError>::Error, ._Error = node_result._Error };
            }
            else
            {
                auto node = node_result._Ok;
                return Result<ConstituentSyntax, ParserError> { ._tag = Result<ConstituentSyntax, ParserError>::Ok, ._Ok = 
                    ConstituentSyntax(ProcedureSyntax(node))
                };
            }
        }
        {
            auto node_result = this->parse_operator(_rp, _ep);
            if (node_result._tag == Result<OperatorSyntax, ParserError>::Error)
            {
                if (node_result._Error._tag == ParserError::InvalidSyntax)
                    return Result<ConstituentSyntax, ParserError> { ._tag = Result<ConstituentSyntax, ParserError>::Error, ._Error = node_result._Error };
            }
            else
            {
                auto node = node_result._Ok;
                return Result<ConstituentSyntax, ParserError> { ._tag = Result<ConstituentSyntax, ParserError>::Ok, ._Ok = 
                    ConstituentSyntax(OperatorSyntax(node))
                };
            }
        }
        {
            auto node_result = this->parse_implement(_rp, _ep);
            if (node_result._tag == Result<ImplementSyntax, ParserError>::Error)
            {
                if (node_result._Error._tag == ParserError::InvalidSyntax)
                    return Result<ConstituentSyntax, ParserError> { ._tag = Result<ConstituentSyntax, ParserError>::Error, ._Error = node_result._Error };
            }
            else
            {
                auto node = node_result._Ok;
                return Result<ConstituentSyntax, ParserError> { ._tag = Result<ConstituentSyntax, ParserError>::Ok, ._Ok = 
                    ConstituentSyntax(ImplementSyntax(node))
                };
            }
        }
        {
            auto node_result = this->parse_trait(_rp, _ep);
            if (node_result._tag == Result<TraitSyntax, ParserError>::Error)
            {
                if (node_result._Error._tag == ParserError::InvalidSyntax)
                    return Result<ConstituentSyntax, ParserError> { ._tag = Result<ConstituentSyntax, ParserError>::Error, ._Error = node_result._Error };
            }
            else
            {
                auto node = node_result._Ok;
                return Result<ConstituentSyntax, ParserError> { ._tag = Result<ConstituentSyntax, ParserError>::Ok, ._Ok = 
                    ConstituentSyntax(TraitSyntax(node))
                };
            }
        }
        {
            auto node_result = this->parse_macro(_rp, _ep);
            if (node_result._tag == Result<MacroSyntax, ParserError>::Error)
            {
                if (node_result._Error._tag == ParserError::InvalidSyntax)
                    return Result<ConstituentSyntax, ParserError> { ._tag = Result<ConstituentSyntax, ParserError>::Error, ._Error = node_result._Error };
            }
            else
            {
                auto node = node_result._Ok;
                return Result<ConstituentSyntax, ParserError> { ._tag = Result<ConstituentSyntax, ParserError>::Ok, ._Ok = 
                    ConstituentSyntax(MacroSyntax(node))
                };
            }
        }
        {
            auto node_result = this->parse_module(_rp, _ep);
            if (node_result._tag == Result<ModuleSyntax, ParserError>::Error)
            {
                if (node_result._Error._tag == ParserError::InvalidSyntax)
                    return Result<ConstituentSyntax, ParserError> { ._tag = Result<ConstituentSyntax, ParserError>::Error, ._Error = node_result._Error };
            }
            else
            {
                auto node = node_result._Ok;
                return Result<ConstituentSyntax, ParserError> { ._tag = Result<ConstituentSyntax, ParserError>::Ok, ._Ok = 
                    ConstituentSyntax(ModuleSyntax(node))
                };
            }
        }
        return Result<ConstituentSyntax, ParserError> { ._tag = Result<ConstituentSyntax, ParserError>::Error, ._Error = ParserError(OtherSyntax()) };
    }

    Result<DefinitionSyntax, ParserError> parse_definition(Page* _rp, Page* _ep) {
        auto start = this->lexer.previous_position;

        auto start_define_1 = this->lexer.previous_position;
        auto success_define_1 = this->lexer.parse_keyword(_rp, *this->keywords_index[4]);
        if (!success_define_1) {
            return Result<DefinitionSyntax, ParserError> { ._tag = Result<DefinitionSyntax, ParserError>::Error, ._Error = ParserError(OtherSyntax()) };
        }

        auto start_name = this->lexer.previous_position;
        auto name = this->lexer.parse_identifier(_rp, this->keywords);
        if (name == nullptr)
        {
            return Result<DefinitionSyntax, ParserError> { ._tag = Result<DefinitionSyntax, ParserError>::Error, ._Error = ParserError(InvalidSyntax(start_name, lexer.position, String(_ep, "an identifier"))) };
        }

        auto parameters_start = this->lexer.position;
        auto parameters_result = this->parse_genericparameters(_rp, _ep);
        if (parameters_result._tag == Result<GenericParametersSyntax, ParserError>::Error)
        {
            switch (parameters_result._Error._tag) {
                case ParserError::OtherSyntax:
                    break;
                case ParserError::InvalidSyntax:
                    return Result<DefinitionSyntax, ParserError> { ._tag = Result<DefinitionSyntax, ParserError>::Error, ._Error = parameters_result._Error };
            }
        }

        GenericParametersSyntax* parameters = parameters_result._tag == Result<GenericParametersSyntax, ParserError>::Error ? nullptr : new(alignof(GenericParametersSyntax), _rp) GenericParametersSyntax(parameters_result._Ok);

        auto start_colon_4 = this->lexer.previous_position;
        auto success_colon_4 = this->lexer.parse_colon(_rp);
        if (!success_colon_4) {
        }

        auto attributes_start = this->lexer.position;
        auto attributes_result = this->parse_attribute_list(_rp, _ep);
        if (attributes_result._tag == Result<Vector<AttributeSyntax>, ParserError>::Error)
        {
            switch (attributes_result._Error._tag) {
                case ParserError::OtherSyntax:
                    break;
                case ParserError::InvalidSyntax:
                    return Result<DefinitionSyntax, ParserError> { ._tag = Result<DefinitionSyntax, ParserError>::Error, ._Error = attributes_result._Error };
            }
        }

        auto attributes = attributes_result._tag == Result<Vector<AttributeSyntax>, ParserError>::Error ? nullptr : attributes_result._Ok;

        auto concept__start = this->lexer.position;
        auto concept__result = this->parse_concept(_rp, _ep);
        if (concept__result._tag == Result<ConceptSyntax, ParserError>::Error)
        {
            switch (concept__result._Error._tag) {
                case ParserError::OtherSyntax:
                    return Result<DefinitionSyntax, ParserError> { ._tag = Result<DefinitionSyntax, ParserError>::Error, ._Error = ParserError(InvalidSyntax(concept__start, lexer.position, String(_ep, "a valid Concept syntax"))) };
                case ParserError::InvalidSyntax:
                    return Result<DefinitionSyntax, ParserError> { ._tag = Result<DefinitionSyntax, ParserError>::Error, ._Error = concept__result._Error };
            }
        }

        auto concept_ = concept__result._Ok;

        auto end = this->lexer.position;

        auto ret = DefinitionSyntax(start, end, *name, parameters, attributes, concept_);

        return Result<DefinitionSyntax, ParserError> { ._tag = Result<DefinitionSyntax, ParserError>::Ok, ._Ok = ret };
    }

    Result<GenericParametersSyntax, ParserError> parse_genericparameters(Page* _rp, Page* _ep) {
        auto start = this->lexer.previous_position;

        auto start_left_bracket_1 = this->lexer.previous_position;
        auto success_left_bracket_1 = this->lexer.parse_punctuation('[');
        if (!success_left_bracket_1) {
            return Result<GenericParametersSyntax, ParserError> { ._tag = Result<GenericParametersSyntax, ParserError>::Error, ._Error = ParserError(OtherSyntax()) };
        }

        auto parameters_start = this->lexer.position;
        auto parameters_result = this->parse_genericparameter_list(_rp, _ep);
        if (parameters_result._tag == Result<Vector<GenericParameterSyntax>, ParserError>::Error)
        {
            switch (parameters_result._Error._tag) {
                case ParserError::OtherSyntax:
                    return Result<GenericParametersSyntax, ParserError> { ._tag = Result<GenericParametersSyntax, ParserError>::Error, ._Error = ParserError(InvalidSyntax(parameters_start, lexer.position, String(_ep, "a valid GenericParameter syntax"))) };
                case ParserError::InvalidSyntax:
                    return Result<GenericParametersSyntax, ParserError> { ._tag = Result<GenericParametersSyntax, ParserError>::Error, ._Error = parameters_result._Error };
            }
        }

        auto parameters = parameters_result._Ok;

        auto start_right_bracket_3 = this->lexer.previous_position;
        auto success_right_bracket_3 = this->lexer.parse_punctuation(']');
        if (!success_right_bracket_3) {
            return Result<GenericParametersSyntax, ParserError> { ._tag = Result<GenericParametersSyntax, ParserError>::Error, ._Error = ParserError(InvalidSyntax(start_right_bracket_3, lexer.position, String(_ep, "]"))) };        }

        auto end = this->lexer.position;

        auto ret = GenericParametersSyntax(start, end, parameters);

        return Result<GenericParametersSyntax, ParserError> { ._tag = Result<GenericParametersSyntax, ParserError>::Ok, ._Ok = ret };
    }

    Result<Vector<GenericParameterSyntax>*, ParserError> parse_genericparameter_list(Page* _rp, Page* _ep) {
        Region _r;
        List<GenericParameterSyntax>& list = *new(alignof(List<GenericParameterSyntax>), _r.get_page()) List<GenericParameterSyntax>();;
        while(true) {
            auto node_result = this->parse_genericparameter(_rp, _ep);
            if ((node_result._tag == Result<GenericParameterSyntax, ParserError>::Error) && (node_result._Error._tag == ParserError::InvalidSyntax))
                return Result<Vector<GenericParameterSyntax>*, ParserError> { ._tag = Result<Vector<GenericParameterSyntax>*, ParserError>::Error, ._Error = node_result._Error };
            if (node_result._tag == Result<GenericParameterSyntax, ParserError>::Ok) {
                auto node = node_result._Ok;
                list.add(node);
            } else {
                if ((list.count() == 0) && (node_result._tag == Result<GenericParameterSyntax, ParserError>::Error) && (node_result._Error._tag == ParserError::OtherSyntax))
                    return Result<Vector<GenericParameterSyntax>*, ParserError> { ._tag = Result<Vector<GenericParameterSyntax>*, ParserError>::Error, ._Error = node_result._Error };
                break;
            }
        }

        if (list.count() == 0)
            return Result<Vector<GenericParameterSyntax>*, ParserError> { ._tag = Result<Vector<GenericParameterSyntax>*, ParserError>::Ok, ._Ok = nullptr };
        
        return Result<Vector<GenericParameterSyntax>*, ParserError> {
            ._tag = Result<Vector<GenericParameterSyntax>*, ParserError>::Ok,
            ._Ok = new(alignof(Vector<GenericParameterSyntax>), _rp) Vector<GenericParameterSyntax>(_rp, list) };
    }

    Result<GenericParameterSyntax, ParserError> parse_genericparameter(Page* _rp, Page* _ep) {
        auto start = this->lexer.previous_position;

        auto start_name = this->lexer.previous_position;
        auto name = this->lexer.parse_identifier(_rp, this->keywords);
        if (name == nullptr)
        {
            return Result<GenericParameterSyntax, ParserError> { ._tag = Result<GenericParameterSyntax, ParserError>::Error, ._Error = ParserError(OtherSyntax()) };

        }

        auto attributes_start = this->lexer.position;
        auto attributes_result = this->parse_attribute_list(_rp, _ep);
        if (attributes_result._tag == Result<Vector<AttributeSyntax>, ParserError>::Error)
        {
            switch (attributes_result._Error._tag) {
                case ParserError::OtherSyntax:
                    break;
                case ParserError::InvalidSyntax:
                    return Result<GenericParameterSyntax, ParserError> { ._tag = Result<GenericParameterSyntax, ParserError>::Error, ._Error = attributes_result._Error };
            }
        }

        auto attributes = attributes_result._tag == Result<Vector<AttributeSyntax>, ParserError>::Error ? nullptr : attributes_result._Ok;

        auto start_comma_3 = this->lexer.previous_position;
        auto success_comma_3 = this->lexer.parse_punctuation(',');
        if (!success_comma_3) {
        }

        auto end = this->lexer.position;

        auto ret = GenericParameterSyntax(start, end, *name, attributes);

        return Result<GenericParameterSyntax, ParserError> { ._tag = Result<GenericParameterSyntax, ParserError>::Ok, ._Ok = ret };
    }

    Result<ConceptSyntax, ParserError> parse_concept(Page* _rp, Page* _ep) {
        {
            auto node_result = this->parse_class(_rp, _ep);
            if (node_result._tag == Result<ClassSyntax, ParserError>::Error)
            {
                if (node_result._Error._tag == ParserError::InvalidSyntax)
                    return Result<ConceptSyntax, ParserError> { ._tag = Result<ConceptSyntax, ParserError>::Error, ._Error = node_result._Error };
            }
            else
            {
                auto node = node_result._Ok;
                return Result<ConceptSyntax, ParserError> { ._tag = Result<ConceptSyntax, ParserError>::Ok, ._Ok = 
                    ConceptSyntax(ClassSyntax(node))
                };
            }
        }
        {
            auto node_result = this->parse_namespace(_rp, _ep);
            if (node_result._tag == Result<NamespaceSyntax, ParserError>::Error)
            {
                if (node_result._Error._tag == ParserError::InvalidSyntax)
                    return Result<ConceptSyntax, ParserError> { ._tag = Result<ConceptSyntax, ParserError>::Error, ._Error = node_result._Error };
            }
            else
            {
                auto node = node_result._Ok;
                return Result<ConceptSyntax, ParserError> { ._tag = Result<ConceptSyntax, ParserError>::Ok, ._Ok = 
                    ConceptSyntax(NamespaceSyntax(node))
                };
            }
        }
        {
            auto node_result = this->parse_union(_rp, _ep);
            if (node_result._tag == Result<UnionSyntax, ParserError>::Error)
            {
                if (node_result._Error._tag == ParserError::InvalidSyntax)
                    return Result<ConceptSyntax, ParserError> { ._tag = Result<ConceptSyntax, ParserError>::Error, ._Error = node_result._Error };
            }
            else
            {
                auto node = node_result._Ok;
                return Result<ConceptSyntax, ParserError> { ._tag = Result<ConceptSyntax, ParserError>::Ok, ._Ok = 
                    ConceptSyntax(UnionSyntax(node))
                };
            }
        }
        {
            auto node_result = this->parse_constant(_rp, _ep);
            if (node_result._tag == Result<ConstantSyntax, ParserError>::Error)
            {
                if (node_result._Error._tag == ParserError::InvalidSyntax)
                    return Result<ConceptSyntax, ParserError> { ._tag = Result<ConceptSyntax, ParserError>::Error, ._Error = node_result._Error };
            }
            else
            {
                auto node = node_result._Ok;
                return Result<ConceptSyntax, ParserError> { ._tag = Result<ConceptSyntax, ParserError>::Ok, ._Ok = 
                    ConceptSyntax(ConstantSyntax(node))
                };
            }
        }
        {
            auto node_result = this->parse_delegate(_rp, _ep);
            if (node_result._tag == Result<DelegateSyntax, ParserError>::Error)
            {
                if (node_result._Error._tag == ParserError::InvalidSyntax)
                    return Result<ConceptSyntax, ParserError> { ._tag = Result<ConceptSyntax, ParserError>::Error, ._Error = node_result._Error };
            }
            else
            {
                auto node = node_result._Ok;
                return Result<ConceptSyntax, ParserError> { ._tag = Result<ConceptSyntax, ParserError>::Ok, ._Ok = 
                    ConceptSyntax(DelegateSyntax(node))
                };
            }
        }
        {
            auto node_result = this->parse_intrinsic(_rp, _ep);
            if (node_result._tag == Result<IntrinsicSyntax, ParserError>::Error)
            {
                if (node_result._Error._tag == ParserError::InvalidSyntax)
                    return Result<ConceptSyntax, ParserError> { ._tag = Result<ConceptSyntax, ParserError>::Error, ._Error = node_result._Error };
            }
            else
            {
                auto node = node_result._Ok;
                return Result<ConceptSyntax, ParserError> { ._tag = Result<ConceptSyntax, ParserError>::Ok, ._Ok = 
                    ConceptSyntax(IntrinsicSyntax(node))
                };
            }
        }
        return Result<ConceptSyntax, ParserError> { ._tag = Result<ConceptSyntax, ParserError>::Error, ._Error = ParserError(OtherSyntax()) };
    }

    Result<ClassSyntax, ParserError> parse_class(Page* _rp, Page* _ep) {
        auto start = this->lexer.previous_position;

        auto structure_start = this->lexer.position;
        auto structure_result = this->parse_structure(_rp, _ep);
        if (structure_result._tag == Result<StructureSyntax, ParserError>::Error)
        {
            return Result<ClassSyntax, ParserError> { ._tag = Result<ClassSyntax, ParserError>::Error, ._Error = structure_result._Error };
        }

        auto structure = structure_result._Ok;

        auto start_colon_2 = this->lexer.previous_position;
        auto success_colon_2 = this->lexer.parse_colon(_rp);
        if (!success_colon_2) {
        }

        auto body_start = this->lexer.position;
        auto body_result = this->parse_body(_rp, _ep);
        if (body_result._tag == Result<BodySyntax, ParserError>::Error)
        {
            switch (body_result._Error._tag) {
                case ParserError::OtherSyntax:
                    break;
                case ParserError::InvalidSyntax:
                    return Result<ClassSyntax, ParserError> { ._tag = Result<ClassSyntax, ParserError>::Error, ._Error = body_result._Error };
            }
        }

        BodySyntax* body = body_result._tag == Result<BodySyntax, ParserError>::Error ? nullptr : new(alignof(BodySyntax), _rp) BodySyntax(body_result._Ok);

        auto start_colon_4 = this->lexer.previous_position;
        auto success_colon_4 = this->lexer.parse_colon(_rp);
        if (!success_colon_4) {
        }

        auto end = this->lexer.position;

        auto ret = ClassSyntax(start, end, structure, body);

        return Result<ClassSyntax, ParserError> { ._tag = Result<ClassSyntax, ParserError>::Ok, ._Ok = ret };
    }

    Result<BodySyntax, ParserError> parse_body(Page* _rp, Page* _ep) {
        auto start = this->lexer.previous_position;

        auto start_left_curly_1 = this->lexer.previous_position;
        auto success_left_curly_1 = this->lexer.parse_punctuation('{');
        if (!success_left_curly_1) {
            return Result<BodySyntax, ParserError> { ._tag = Result<BodySyntax, ParserError>::Error, ._Error = ParserError(OtherSyntax()) };
        }

        auto uses_start = this->lexer.position;
        auto uses_result = this->parse_use_list(_rp, _ep);
        if (uses_result._tag == Result<Vector<UseSyntax>, ParserError>::Error)
        {
            switch (uses_result._Error._tag) {
                case ParserError::OtherSyntax:
                    break;
                case ParserError::InvalidSyntax:
                    return Result<BodySyntax, ParserError> { ._tag = Result<BodySyntax, ParserError>::Error, ._Error = uses_result._Error };
            }
        }

        auto uses = uses_result._tag == Result<Vector<UseSyntax>, ParserError>::Error ? nullptr : uses_result._Ok;

        auto inits_start = this->lexer.position;
        auto inits_result = this->parse_init_list(_rp, _ep);
        if (inits_result._tag == Result<Vector<InitSyntax>, ParserError>::Error)
        {
            switch (inits_result._Error._tag) {
                case ParserError::OtherSyntax:
                    break;
                case ParserError::InvalidSyntax:
                    return Result<BodySyntax, ParserError> { ._tag = Result<BodySyntax, ParserError>::Error, ._Error = inits_result._Error };
            }
        }

        auto inits = inits_result._tag == Result<Vector<InitSyntax>, ParserError>::Error ? nullptr : inits_result._Ok;

        auto deInit_start = this->lexer.position;
        auto deInit_result = this->parse_deinit(_rp, _ep);
        if (deInit_result._tag == Result<DeInitSyntax, ParserError>::Error)
        {
            switch (deInit_result._Error._tag) {
                case ParserError::OtherSyntax:
                    break;
                case ParserError::InvalidSyntax:
                    return Result<BodySyntax, ParserError> { ._tag = Result<BodySyntax, ParserError>::Error, ._Error = deInit_result._Error };
            }
        }

        DeInitSyntax* deInit = deInit_result._tag == Result<DeInitSyntax, ParserError>::Error ? nullptr : new(alignof(DeInitSyntax), _rp) DeInitSyntax(deInit_result._Ok);

        auto members_start = this->lexer.position;
        auto members_result = this->parse_member_list(_rp, _ep);
        if (members_result._tag == Result<Vector<MemberSyntax>, ParserError>::Error)
        {
            switch (members_result._Error._tag) {
                case ParserError::OtherSyntax:
                    break;
                case ParserError::InvalidSyntax:
                    return Result<BodySyntax, ParserError> { ._tag = Result<BodySyntax, ParserError>::Error, ._Error = members_result._Error };
            }
        }

        auto members = members_result._tag == Result<Vector<MemberSyntax>, ParserError>::Error ? nullptr : members_result._Ok;

        auto start_right_curly_6 = this->lexer.previous_position;
        auto success_right_curly_6 = this->lexer.parse_punctuation('}');
        if (!success_right_curly_6) {
            return Result<BodySyntax, ParserError> { ._tag = Result<BodySyntax, ParserError>::Error, ._Error = ParserError(InvalidSyntax(start_right_curly_6, lexer.position, String(_ep, "}"))) };        }

        auto start_colon_7 = this->lexer.previous_position;
        auto success_colon_7 = this->lexer.parse_colon(_rp);
        if (!success_colon_7) {
        }

        auto end = this->lexer.position;

        auto ret = BodySyntax(start, end, uses, inits, deInit, members);

        return Result<BodySyntax, ParserError> { ._tag = Result<BodySyntax, ParserError>::Ok, ._Ok = ret };
    }

    Result<NamespaceSyntax, ParserError> parse_namespace(Page* _rp, Page* _ep) {
        auto start = this->lexer.previous_position;

        auto start_left_curly_1 = this->lexer.previous_position;
        auto success_left_curly_1 = this->lexer.parse_punctuation('{');
        if (!success_left_curly_1) {
            return Result<NamespaceSyntax, ParserError> { ._tag = Result<NamespaceSyntax, ParserError>::Error, ._Error = ParserError(OtherSyntax()) };
        }

        auto uses_start = this->lexer.position;
        auto uses_result = this->parse_use_list(_rp, _ep);
        if (uses_result._tag == Result<Vector<UseSyntax>, ParserError>::Error)
        {
            switch (uses_result._Error._tag) {
                case ParserError::OtherSyntax:
                    break;
                case ParserError::InvalidSyntax:
                    return Result<NamespaceSyntax, ParserError> { ._tag = Result<NamespaceSyntax, ParserError>::Error, ._Error = uses_result._Error };
            }
        }

        auto uses = uses_result._tag == Result<Vector<UseSyntax>, ParserError>::Error ? nullptr : uses_result._Ok;

        auto declarations_start = this->lexer.position;
        auto declarations_result = this->parse_declaration_list(_rp, _ep);
        if (declarations_result._tag == Result<Vector<DeclarationSyntax>, ParserError>::Error)
        {
            switch (declarations_result._Error._tag) {
                case ParserError::OtherSyntax:
                    break;
                case ParserError::InvalidSyntax:
                    return Result<NamespaceSyntax, ParserError> { ._tag = Result<NamespaceSyntax, ParserError>::Error, ._Error = declarations_result._Error };
            }
        }

        auto declarations = declarations_result._tag == Result<Vector<DeclarationSyntax>, ParserError>::Error ? nullptr : declarations_result._Ok;

        auto start_right_curly_4 = this->lexer.previous_position;
        auto success_right_curly_4 = this->lexer.parse_punctuation('}');
        if (!success_right_curly_4) {
            return Result<NamespaceSyntax, ParserError> { ._tag = Result<NamespaceSyntax, ParserError>::Error, ._Error = ParserError(InvalidSyntax(start_right_curly_4, lexer.position, String(_ep, "}"))) };        }

        auto start_colon_5 = this->lexer.previous_position;
        auto success_colon_5 = this->lexer.parse_colon(_rp);
        if (!success_colon_5) {
        }

        auto end = this->lexer.position;

        auto ret = NamespaceSyntax(start, end, uses, declarations);

        return Result<NamespaceSyntax, ParserError> { ._tag = Result<NamespaceSyntax, ParserError>::Ok, ._Ok = ret };
    }

    Result<UnionSyntax, ParserError> parse_union(Page* _rp, Page* _ep) {
        auto start = this->lexer.previous_position;

        auto start_union_1 = this->lexer.previous_position;
        auto success_union_1 = this->lexer.parse_keyword(_rp, *this->keywords_index[39]);
        if (!success_union_1) {
            return Result<UnionSyntax, ParserError> { ._tag = Result<UnionSyntax, ParserError>::Error, ._Error = ParserError(OtherSyntax()) };
        }

        auto start_colon_2 = this->lexer.previous_position;
        auto success_colon_2 = this->lexer.parse_colon(_rp);
        if (!success_colon_2) {
        }

        auto start_left_paren_3 = this->lexer.previous_position;
        auto success_left_paren_3 = this->lexer.parse_punctuation('(');
        if (!success_left_paren_3) {
            return Result<UnionSyntax, ParserError> { ._tag = Result<UnionSyntax, ParserError>::Error, ._Error = ParserError(InvalidSyntax(start_left_paren_3, lexer.position, String(_ep, "("))) };        }

        auto variants_start = this->lexer.position;
        auto variants_result = this->parse_variant_list(_rp, _ep);
        if (variants_result._tag == Result<Vector<VariantSyntax>, ParserError>::Error)
        {
            switch (variants_result._Error._tag) {
                case ParserError::OtherSyntax:
                    return Result<UnionSyntax, ParserError> { ._tag = Result<UnionSyntax, ParserError>::Error, ._Error = ParserError(InvalidSyntax(variants_start, lexer.position, String(_ep, "a valid Variant syntax"))) };
                case ParserError::InvalidSyntax:
                    return Result<UnionSyntax, ParserError> { ._tag = Result<UnionSyntax, ParserError>::Error, ._Error = variants_result._Error };
            }
        }

        auto variants = variants_result._Ok;

        auto start_right_paren_5 = this->lexer.previous_position;
        auto success_right_paren_5 = this->lexer.parse_punctuation(')');
        if (!success_right_paren_5) {
            return Result<UnionSyntax, ParserError> { ._tag = Result<UnionSyntax, ParserError>::Error, ._Error = ParserError(InvalidSyntax(start_right_paren_5, lexer.position, String(_ep, ")"))) };        }

        auto start_colon_6 = this->lexer.previous_position;
        auto success_colon_6 = this->lexer.parse_colon(_rp);
        if (!success_colon_6) {
        }

        auto end = this->lexer.position;

        auto ret = UnionSyntax(start, end, variants);

        return Result<UnionSyntax, ParserError> { ._tag = Result<UnionSyntax, ParserError>::Ok, ._Ok = ret };
    }

    Result<Vector<VariantSyntax>*, ParserError> parse_variant_list(Page* _rp, Page* _ep) {
        Region _r;
        List<VariantSyntax>& list = *new(alignof(List<VariantSyntax>), _r.get_page()) List<VariantSyntax>();;
        while(true) {
            auto node_result = this->parse_variant(_rp, _ep);
            if ((node_result._tag == Result<VariantSyntax, ParserError>::Error) && (node_result._Error._tag == ParserError::InvalidSyntax))
                return Result<Vector<VariantSyntax>*, ParserError> { ._tag = Result<Vector<VariantSyntax>*, ParserError>::Error, ._Error = node_result._Error };
            if (node_result._tag == Result<VariantSyntax, ParserError>::Ok) {
                auto node = node_result._Ok;
                list.add(node);
            } else {
                if ((list.count() == 0) && (node_result._tag == Result<VariantSyntax, ParserError>::Error) && (node_result._Error._tag == ParserError::OtherSyntax))
                    return Result<Vector<VariantSyntax>*, ParserError> { ._tag = Result<Vector<VariantSyntax>*, ParserError>::Error, ._Error = node_result._Error };
                break;
            }
        }

        if (list.count() == 0)
            return Result<Vector<VariantSyntax>*, ParserError> { ._tag = Result<Vector<VariantSyntax>*, ParserError>::Ok, ._Ok = nullptr };
        
        return Result<Vector<VariantSyntax>*, ParserError> {
            ._tag = Result<Vector<VariantSyntax>*, ParserError>::Ok,
            ._Ok = new(alignof(Vector<VariantSyntax>), _rp) Vector<VariantSyntax>(_rp, list) };
    }

    Result<VariantSyntax, ParserError> parse_variant(Page* _rp, Page* _ep) {
        auto start = this->lexer.previous_position;

        auto start_name = this->lexer.previous_position;
        auto name = this->lexer.parse_identifier(_rp, this->keywords);
        if (name == nullptr)
        {
            return Result<VariantSyntax, ParserError> { ._tag = Result<VariantSyntax, ParserError>::Error, ._Error = ParserError(OtherSyntax()) };

        }

        auto attributes_start = this->lexer.position;
        auto attributes_result = this->parse_attribute_list(_rp, _ep);
        if (attributes_result._tag == Result<Vector<AttributeSyntax>, ParserError>::Error)
        {
            switch (attributes_result._Error._tag) {
                case ParserError::OtherSyntax:
                    break;
                case ParserError::InvalidSyntax:
                    return Result<VariantSyntax, ParserError> { ._tag = Result<VariantSyntax, ParserError>::Error, ._Error = attributes_result._Error };
            }
        }

        auto attributes = attributes_result._tag == Result<Vector<AttributeSyntax>, ParserError>::Error ? nullptr : attributes_result._Ok;

        auto annotation_start = this->lexer.position;
        auto annotation_result = this->parse_typeannotation(_rp, _ep);
        if (annotation_result._tag == Result<TypeAnnotationSyntax, ParserError>::Error)
        {
            switch (annotation_result._Error._tag) {
                case ParserError::OtherSyntax:
                    break;
                case ParserError::InvalidSyntax:
                    return Result<VariantSyntax, ParserError> { ._tag = Result<VariantSyntax, ParserError>::Error, ._Error = annotation_result._Error };
            }
        }

        TypeAnnotationSyntax* annotation = annotation_result._tag == Result<TypeAnnotationSyntax, ParserError>::Error ? nullptr : new(alignof(TypeAnnotationSyntax), _rp) TypeAnnotationSyntax(annotation_result._Ok);

        auto start_comma_4 = this->lexer.previous_position;
        auto success_comma_4 = this->lexer.parse_punctuation(',');
        if (!success_comma_4) {
        }

        auto start_colon_5 = this->lexer.previous_position;
        auto success_colon_5 = this->lexer.parse_colon(_rp);
        if (!success_colon_5) {
        }

        auto end = this->lexer.position;

        auto ret = VariantSyntax(start, end, *name, attributes, annotation);

        return Result<VariantSyntax, ParserError> { ._tag = Result<VariantSyntax, ParserError>::Ok, ._Ok = ret };
    }

    Result<ConstantSyntax, ParserError> parse_constant(Page* _rp, Page* _ep) {
        auto start = this->lexer.previous_position;

        auto type_start = this->lexer.position;
        auto type_result = this->parse_type(_rp, _ep);
        if (type_result._tag == Result<TypeSyntax, ParserError>::Error)
        {
            return Result<ConstantSyntax, ParserError> { ._tag = Result<ConstantSyntax, ParserError>::Error, ._Error = type_result._Error };
        }

        auto type = type_result._Ok;

        auto operation_start = this->lexer.position;
        auto operation_result = this->parse_operand_list(_rp, _ep);
        if (operation_result._tag == Result<Vector<OperandSyntax>, ParserError>::Error)
        {
            switch (operation_result._Error._tag) {
                case ParserError::OtherSyntax:
                    return Result<ConstantSyntax, ParserError> { ._tag = Result<ConstantSyntax, ParserError>::Error, ._Error = ParserError(InvalidSyntax(operation_start, lexer.position, String(_ep, "a valid Operand syntax"))) };
                case ParserError::InvalidSyntax:
                    return Result<ConstantSyntax, ParserError> { ._tag = Result<ConstantSyntax, ParserError>::Error, ._Error = operation_result._Error };
            }
        }

        auto operation = operation_result._Ok;

        auto start_colon_3 = this->lexer.previous_position;
        auto success_colon_3 = this->lexer.parse_colon(_rp);
        if (!success_colon_3) {
        }

        auto end = this->lexer.position;

        auto ret = ConstantSyntax(start, end, type, operation);

        return Result<ConstantSyntax, ParserError> { ._tag = Result<ConstantSyntax, ParserError>::Ok, ._Ok = ret };
    }

    Result<DelegateSyntax, ParserError> parse_delegate(Page* _rp, Page* _ep) {
        auto start = this->lexer.previous_position;

        auto start_delegate_1 = this->lexer.previous_position;
        auto success_delegate_1 = this->lexer.parse_keyword(_rp, *this->keywords_index[7]);
        if (!success_delegate_1) {
            return Result<DelegateSyntax, ParserError> { ._tag = Result<DelegateSyntax, ParserError>::Error, ._Error = ParserError(OtherSyntax()) };
        }

        auto parameters_start = this->lexer.position;
        auto parameters_result = this->parse_parameterset(_rp, _ep);
        if (parameters_result._tag == Result<ParameterSetSyntax, ParserError>::Error)
        {
            switch (parameters_result._Error._tag) {
                case ParserError::OtherSyntax:
                    break;
                case ParserError::InvalidSyntax:
                    return Result<DelegateSyntax, ParserError> { ._tag = Result<DelegateSyntax, ParserError>::Error, ._Error = parameters_result._Error };
            }
        }

        ParameterSetSyntax* parameters = parameters_result._tag == Result<ParameterSetSyntax, ParserError>::Error ? nullptr : new(alignof(ParameterSetSyntax), _rp) ParameterSetSyntax(parameters_result._Ok);

        auto attributes_start = this->lexer.position;
        auto attributes_result = this->parse_attribute_list(_rp, _ep);
        if (attributes_result._tag == Result<Vector<AttributeSyntax>, ParserError>::Error)
        {
            switch (attributes_result._Error._tag) {
                case ParserError::OtherSyntax:
                    break;
                case ParserError::InvalidSyntax:
                    return Result<DelegateSyntax, ParserError> { ._tag = Result<DelegateSyntax, ParserError>::Error, ._Error = attributes_result._Error };
            }
        }

        auto attributes = attributes_result._tag == Result<Vector<AttributeSyntax>, ParserError>::Error ? nullptr : attributes_result._Ok;

        auto result_start = this->lexer.position;
        auto result_result = this->parse_returns(_rp, _ep);
        if (result_result._tag == Result<ReturnsSyntax, ParserError>::Error)
        {
            switch (result_result._Error._tag) {
                case ParserError::OtherSyntax:
                    break;
                case ParserError::InvalidSyntax:
                    return Result<DelegateSyntax, ParserError> { ._tag = Result<DelegateSyntax, ParserError>::Error, ._Error = result_result._Error };
            }
        }

        ReturnsSyntax* result = result_result._tag == Result<ReturnsSyntax, ParserError>::Error ? nullptr : new(alignof(ReturnsSyntax), _rp) ReturnsSyntax(result_result._Ok);

        auto error_start = this->lexer.position;
        auto error_result = this->parse_throws(_rp, _ep);
        if (error_result._tag == Result<ThrowsSyntax, ParserError>::Error)
        {
            switch (error_result._Error._tag) {
                case ParserError::OtherSyntax:
                    break;
                case ParserError::InvalidSyntax:
                    return Result<DelegateSyntax, ParserError> { ._tag = Result<DelegateSyntax, ParserError>::Error, ._Error = error_result._Error };
            }
        }

        ThrowsSyntax* error = error_result._tag == Result<ThrowsSyntax, ParserError>::Error ? nullptr : new(alignof(ThrowsSyntax), _rp) ThrowsSyntax(error_result._Ok);

        auto end = this->lexer.position;

        auto ret = DelegateSyntax(start, end, parameters, attributes, result, error);

        return Result<DelegateSyntax, ParserError> { ._tag = Result<DelegateSyntax, ParserError>::Ok, ._Ok = ret };
    }

    Result<TypeAnnotationSyntax, ParserError> parse_typeannotation(Page* _rp, Page* _ep) {
        auto start = this->lexer.previous_position;

        auto start_colon_1 = this->lexer.previous_position;
        auto success_colon_1 = this->lexer.parse_colon(_rp);
        if (!success_colon_1) {
            return Result<TypeAnnotationSyntax, ParserError> { ._tag = Result<TypeAnnotationSyntax, ParserError>::Error, ._Error = ParserError(OtherSyntax()) };
        }

        auto type_start = this->lexer.position;
        auto type_result = this->parse_type(_rp, _ep);
        if (type_result._tag == Result<TypeSyntax, ParserError>::Error)
        {
            switch (type_result._Error._tag) {
                case ParserError::OtherSyntax:
                    return Result<TypeAnnotationSyntax, ParserError> { ._tag = Result<TypeAnnotationSyntax, ParserError>::Error, ._Error = ParserError(InvalidSyntax(type_start, lexer.position, String(_ep, "a valid Type syntax"))) };
                case ParserError::InvalidSyntax:
                    return Result<TypeAnnotationSyntax, ParserError> { ._tag = Result<TypeAnnotationSyntax, ParserError>::Error, ._Error = type_result._Error };
            }
        }

        auto type = type_result._Ok;

        auto end = this->lexer.position;

        auto ret = TypeAnnotationSyntax(start, end, type);

        return Result<TypeAnnotationSyntax, ParserError> { ._tag = Result<TypeAnnotationSyntax, ParserError>::Ok, ._Ok = ret };
    }

    Result<GenericArgumentsSyntax, ParserError> parse_genericarguments(Page* _rp, Page* _ep) {
        auto start = this->lexer.previous_position;

        auto start_left_bracket_1 = this->lexer.previous_position;
        auto success_left_bracket_1 = this->lexer.parse_punctuation('[');
        if (!success_left_bracket_1) {
            return Result<GenericArgumentsSyntax, ParserError> { ._tag = Result<GenericArgumentsSyntax, ParserError>::Error, ._Error = ParserError(OtherSyntax()) };
        }

        auto generics_start = this->lexer.position;
        auto generics_result = this->parse_genericargument_list(_rp, _ep);
        if (generics_result._tag == Result<Vector<GenericArgumentSyntax>, ParserError>::Error)
        {
            switch (generics_result._Error._tag) {
                case ParserError::OtherSyntax:
                    break;
                case ParserError::InvalidSyntax:
                    return Result<GenericArgumentsSyntax, ParserError> { ._tag = Result<GenericArgumentsSyntax, ParserError>::Error, ._Error = generics_result._Error };
            }
        }

        auto generics = generics_result._tag == Result<Vector<GenericArgumentSyntax>, ParserError>::Error ? nullptr : generics_result._Ok;

        auto start_right_bracket_3 = this->lexer.previous_position;
        auto success_right_bracket_3 = this->lexer.parse_punctuation(']');
        if (!success_right_bracket_3) {
            return Result<GenericArgumentsSyntax, ParserError> { ._tag = Result<GenericArgumentsSyntax, ParserError>::Error, ._Error = ParserError(InvalidSyntax(start_right_bracket_3, lexer.position, String(_ep, "]"))) };        }

        auto end = this->lexer.position;

        auto ret = GenericArgumentsSyntax(start, end, generics);

        return Result<GenericArgumentsSyntax, ParserError> { ._tag = Result<GenericArgumentsSyntax, ParserError>::Ok, ._Ok = ret };
    }

    Result<Vector<GenericArgumentSyntax>*, ParserError> parse_genericargument_list(Page* _rp, Page* _ep) {
        Region _r;
        List<GenericArgumentSyntax>& list = *new(alignof(List<GenericArgumentSyntax>), _r.get_page()) List<GenericArgumentSyntax>();;
        while(true) {
            auto node_result = this->parse_genericargument(_rp, _ep);
            if ((node_result._tag == Result<GenericArgumentSyntax, ParserError>::Error) && (node_result._Error._tag == ParserError::InvalidSyntax))
                return Result<Vector<GenericArgumentSyntax>*, ParserError> { ._tag = Result<Vector<GenericArgumentSyntax>*, ParserError>::Error, ._Error = node_result._Error };
            if (node_result._tag == Result<GenericArgumentSyntax, ParserError>::Ok) {
                auto node = node_result._Ok;
                list.add(node);
            } else {
                if ((list.count() == 0) && (node_result._tag == Result<GenericArgumentSyntax, ParserError>::Error) && (node_result._Error._tag == ParserError::OtherSyntax))
                    return Result<Vector<GenericArgumentSyntax>*, ParserError> { ._tag = Result<Vector<GenericArgumentSyntax>*, ParserError>::Error, ._Error = node_result._Error };
                break;
            }
        }

        if (list.count() == 0)
            return Result<Vector<GenericArgumentSyntax>*, ParserError> { ._tag = Result<Vector<GenericArgumentSyntax>*, ParserError>::Ok, ._Ok = nullptr };
        
        return Result<Vector<GenericArgumentSyntax>*, ParserError> {
            ._tag = Result<Vector<GenericArgumentSyntax>*, ParserError>::Ok,
            ._Ok = new(alignof(Vector<GenericArgumentSyntax>), _rp) Vector<GenericArgumentSyntax>(_rp, list) };
    }

    Result<GenericArgumentSyntax, ParserError> parse_genericargument(Page* _rp, Page* _ep) {
        auto start = this->lexer.previous_position;

        auto type_start = this->lexer.position;
        auto type_result = this->parse_type(_rp, _ep);
        if (type_result._tag == Result<TypeSyntax, ParserError>::Error)
        {
            return Result<GenericArgumentSyntax, ParserError> { ._tag = Result<GenericArgumentSyntax, ParserError>::Error, ._Error = type_result._Error };
        }

        auto type = type_result._Ok;

        auto start_comma_2 = this->lexer.previous_position;
        auto success_comma_2 = this->lexer.parse_punctuation(',');
        if (!success_comma_2) {
        }

        auto end = this->lexer.position;

        auto ret = GenericArgumentSyntax(start, end, type);

        return Result<GenericArgumentSyntax, ParserError> { ._tag = Result<GenericArgumentSyntax, ParserError>::Ok, ._Ok = ret };
    }

    Result<OptionalSyntax, ParserError> parse_optional(Page* _rp, Page* _ep) {
        auto start = this->lexer.previous_position;

        auto start_question_1 = this->lexer.previous_position;
        auto success_question_1 = this->lexer.parse_punctuation('?');
        if (!success_question_1) {
            return Result<OptionalSyntax, ParserError> { ._tag = Result<OptionalSyntax, ParserError>::Error, ._Error = ParserError(OtherSyntax()) };
        }

        auto end = this->lexer.position;

        auto ret = OptionalSyntax(start, end);

        return Result<OptionalSyntax, ParserError> { ._tag = Result<OptionalSyntax, ParserError>::Ok, ._Ok = ret };
    }

    Result<ParameterSetSyntax, ParserError> parse_parameterset(Page* _rp, Page* _ep) {
        {
            auto node_result = this->parse_parameters(_rp, _ep);
            if (node_result._tag == Result<ParametersSyntax, ParserError>::Error)
            {
                if (node_result._Error._tag == ParserError::InvalidSyntax)
                    return Result<ParameterSetSyntax, ParserError> { ._tag = Result<ParameterSetSyntax, ParserError>::Error, ._Error = node_result._Error };
            }
            else
            {
                auto node = node_result._Ok;
                return Result<ParameterSetSyntax, ParserError> { ._tag = Result<ParameterSetSyntax, ParserError>::Ok, ._Ok = 
                    ParameterSetSyntax(ParametersSyntax(node))
                };
            }
        }
        {
            auto node_result = this->parse_type(_rp, _ep);
            if (node_result._tag == Result<TypeSyntax, ParserError>::Error)
            {
                if (node_result._Error._tag == ParserError::InvalidSyntax)
                    return Result<ParameterSetSyntax, ParserError> { ._tag = Result<ParameterSetSyntax, ParserError>::Error, ._Error = node_result._Error };
            }
            else
            {
                auto node = node_result._Ok;
                return Result<ParameterSetSyntax, ParserError> { ._tag = Result<ParameterSetSyntax, ParserError>::Ok, ._Ok = 
                    ParameterSetSyntax(TypeSyntax(node))
                };
            }
        }
        return Result<ParameterSetSyntax, ParserError> { ._tag = Result<ParameterSetSyntax, ParserError>::Error, ._Error = ParserError(OtherSyntax()) };
    }

    Result<ParametersSyntax, ParserError> parse_parameters(Page* _rp, Page* _ep) {
        auto start = this->lexer.previous_position;

        auto start_left_paren_1 = this->lexer.previous_position;
        auto success_left_paren_1 = this->lexer.parse_punctuation('(');
        if (!success_left_paren_1) {
            return Result<ParametersSyntax, ParserError> { ._tag = Result<ParametersSyntax, ParserError>::Error, ._Error = ParserError(OtherSyntax()) };
        }

        auto items_start = this->lexer.position;
        auto items_result = this->parse_item_list(_rp, _ep);
        if (items_result._tag == Result<Vector<ItemSyntax>, ParserError>::Error)
        {
            switch (items_result._Error._tag) {
                case ParserError::OtherSyntax:
                    break;
                case ParserError::InvalidSyntax:
                    return Result<ParametersSyntax, ParserError> { ._tag = Result<ParametersSyntax, ParserError>::Error, ._Error = items_result._Error };
            }
        }

        auto items = items_result._tag == Result<Vector<ItemSyntax>, ParserError>::Error ? nullptr : items_result._Ok;

        auto start_right_paren_3 = this->lexer.previous_position;
        auto success_right_paren_3 = this->lexer.parse_punctuation(')');
        if (!success_right_paren_3) {
            return Result<ParametersSyntax, ParserError> { ._tag = Result<ParametersSyntax, ParserError>::Error, ._Error = ParserError(InvalidSyntax(start_right_paren_3, lexer.position, String(_ep, ")"))) };        }

        auto end = this->lexer.position;

        auto ret = ParametersSyntax(start, end, items);

        return Result<ParametersSyntax, ParserError> { ._tag = Result<ParametersSyntax, ParserError>::Ok, ._Ok = ret };
    }

    Result<Vector<ItemSyntax>*, ParserError> parse_item_list(Page* _rp, Page* _ep) {
        Region _r;
        List<ItemSyntax>& list = *new(alignof(List<ItemSyntax>), _r.get_page()) List<ItemSyntax>();;
        while(true) {
            auto node_result = this->parse_item(_rp, _ep);
            if ((node_result._tag == Result<ItemSyntax, ParserError>::Error) && (node_result._Error._tag == ParserError::InvalidSyntax))
                return Result<Vector<ItemSyntax>*, ParserError> { ._tag = Result<Vector<ItemSyntax>*, ParserError>::Error, ._Error = node_result._Error };
            if (node_result._tag == Result<ItemSyntax, ParserError>::Ok) {
                auto node = node_result._Ok;
                list.add(node);
            } else {
                if ((list.count() == 0) && (node_result._tag == Result<ItemSyntax, ParserError>::Error) && (node_result._Error._tag == ParserError::OtherSyntax))
                    return Result<Vector<ItemSyntax>*, ParserError> { ._tag = Result<Vector<ItemSyntax>*, ParserError>::Error, ._Error = node_result._Error };
                break;
            }
        }

        if (list.count() == 0)
            return Result<Vector<ItemSyntax>*, ParserError> { ._tag = Result<Vector<ItemSyntax>*, ParserError>::Ok, ._Ok = nullptr };
        
        return Result<Vector<ItemSyntax>*, ParserError> {
            ._tag = Result<Vector<ItemSyntax>*, ParserError>::Ok,
            ._Ok = new(alignof(Vector<ItemSyntax>), _rp) Vector<ItemSyntax>(_rp, list) };
    }

    Result<ItemSyntax, ParserError> parse_item(Page* _rp, Page* _ep) {
        auto start = this->lexer.previous_position;

        auto start_name = this->lexer.previous_position;
        auto name = this->lexer.parse_identifier(_rp, this->keywords);
        if (name == nullptr)
        {
            return Result<ItemSyntax, ParserError> { ._tag = Result<ItemSyntax, ParserError>::Error, ._Error = ParserError(OtherSyntax()) };

        }

        auto annotation_start = this->lexer.position;
        auto annotation_result = this->parse_typeannotation(_rp, _ep);
        if (annotation_result._tag == Result<TypeAnnotationSyntax, ParserError>::Error)
        {
            switch (annotation_result._Error._tag) {
                case ParserError::OtherSyntax:
                    break;
                case ParserError::InvalidSyntax:
                    return Result<ItemSyntax, ParserError> { ._tag = Result<ItemSyntax, ParserError>::Error, ._Error = annotation_result._Error };
            }
        }

        TypeAnnotationSyntax* annotation = annotation_result._tag == Result<TypeAnnotationSyntax, ParserError>::Error ? nullptr : new(alignof(TypeAnnotationSyntax), _rp) TypeAnnotationSyntax(annotation_result._Ok);

        auto start_comma_3 = this->lexer.previous_position;
        auto success_comma_3 = this->lexer.parse_punctuation(',');
        if (!success_comma_3) {
        }

        auto start_colon_4 = this->lexer.previous_position;
        auto success_colon_4 = this->lexer.parse_colon(_rp);
        if (!success_colon_4) {
        }

        auto attributes_start = this->lexer.position;
        auto attributes_result = this->parse_attribute_list(_rp, _ep);
        if (attributes_result._tag == Result<Vector<AttributeSyntax>, ParserError>::Error)
        {
            switch (attributes_result._Error._tag) {
                case ParserError::OtherSyntax:
                    break;
                case ParserError::InvalidSyntax:
                    return Result<ItemSyntax, ParserError> { ._tag = Result<ItemSyntax, ParserError>::Error, ._Error = attributes_result._Error };
            }
        }

        auto attributes = attributes_result._tag == Result<Vector<AttributeSyntax>, ParserError>::Error ? nullptr : attributes_result._Ok;

        auto start_colon_6 = this->lexer.previous_position;
        auto success_colon_6 = this->lexer.parse_colon(_rp);
        if (!success_colon_6) {
        }

        auto end = this->lexer.position;

        auto ret = ItemSyntax(start, end, *name, annotation, attributes);

        return Result<ItemSyntax, ParserError> { ._tag = Result<ItemSyntax, ParserError>::Ok, ._Ok = ret };
    }

    Result<ReturnsSyntax, ParserError> parse_returns(Page* _rp, Page* _ep) {
        auto start = this->lexer.previous_position;

        auto start_returns_1 = this->lexer.previous_position;
        auto success_returns_1 = this->lexer.parse_keyword(_rp, *this->keywords_index[32]);
        if (!success_returns_1) {
            return Result<ReturnsSyntax, ParserError> { ._tag = Result<ReturnsSyntax, ParserError>::Error, ._Error = ParserError(OtherSyntax()) };
        }

        auto type_start = this->lexer.position;
        auto type_result = this->parse_type(_rp, _ep);
        if (type_result._tag == Result<TypeSyntax, ParserError>::Error)
        {
            switch (type_result._Error._tag) {
                case ParserError::OtherSyntax:
                    return Result<ReturnsSyntax, ParserError> { ._tag = Result<ReturnsSyntax, ParserError>::Error, ._Error = ParserError(InvalidSyntax(type_start, lexer.position, String(_ep, "a valid Type syntax"))) };
                case ParserError::InvalidSyntax:
                    return Result<ReturnsSyntax, ParserError> { ._tag = Result<ReturnsSyntax, ParserError>::Error, ._Error = type_result._Error };
            }
        }

        auto type = type_result._Ok;

        auto attributes_start = this->lexer.position;
        auto attributes_result = this->parse_attribute_list(_rp, _ep);
        if (attributes_result._tag == Result<Vector<AttributeSyntax>, ParserError>::Error)
        {
            switch (attributes_result._Error._tag) {
                case ParserError::OtherSyntax:
                    break;
                case ParserError::InvalidSyntax:
                    return Result<ReturnsSyntax, ParserError> { ._tag = Result<ReturnsSyntax, ParserError>::Error, ._Error = attributes_result._Error };
            }
        }

        auto attributes = attributes_result._tag == Result<Vector<AttributeSyntax>, ParserError>::Error ? nullptr : attributes_result._Ok;

        auto end = this->lexer.position;

        auto ret = ReturnsSyntax(start, end, type, attributes);

        return Result<ReturnsSyntax, ParserError> { ._tag = Result<ReturnsSyntax, ParserError>::Ok, ._Ok = ret };
    }

    Result<ThrowsSyntax, ParserError> parse_throws(Page* _rp, Page* _ep) {
        auto start = this->lexer.previous_position;

        auto start_throws_1 = this->lexer.previous_position;
        auto success_throws_1 = this->lexer.parse_keyword(_rp, *this->keywords_index[37]);
        if (!success_throws_1) {
            return Result<ThrowsSyntax, ParserError> { ._tag = Result<ThrowsSyntax, ParserError>::Error, ._Error = ParserError(OtherSyntax()) };
        }

        auto type_start = this->lexer.position;
        auto type_result = this->parse_type(_rp, _ep);
        if (type_result._tag == Result<TypeSyntax, ParserError>::Error)
        {
            switch (type_result._Error._tag) {
                case ParserError::OtherSyntax:
                    return Result<ThrowsSyntax, ParserError> { ._tag = Result<ThrowsSyntax, ParserError>::Error, ._Error = ParserError(InvalidSyntax(type_start, lexer.position, String(_ep, "a valid Type syntax"))) };
                case ParserError::InvalidSyntax:
                    return Result<ThrowsSyntax, ParserError> { ._tag = Result<ThrowsSyntax, ParserError>::Error, ._Error = type_result._Error };
            }
        }

        auto type = type_result._Ok;

        auto attributes_start = this->lexer.position;
        auto attributes_result = this->parse_attribute_list(_rp, _ep);
        if (attributes_result._tag == Result<Vector<AttributeSyntax>, ParserError>::Error)
        {
            switch (attributes_result._Error._tag) {
                case ParserError::OtherSyntax:
                    break;
                case ParserError::InvalidSyntax:
                    return Result<ThrowsSyntax, ParserError> { ._tag = Result<ThrowsSyntax, ParserError>::Error, ._Error = attributes_result._Error };
            }
        }

        auto attributes = attributes_result._tag == Result<Vector<AttributeSyntax>, ParserError>::Error ? nullptr : attributes_result._Ok;

        auto end = this->lexer.position;

        auto ret = ThrowsSyntax(start, end, type, attributes);

        return Result<ThrowsSyntax, ParserError> { ._tag = Result<ThrowsSyntax, ParserError>::Ok, ._Ok = ret };
    }

    Result<Vector<UseSyntax>*, ParserError> parse_use_list(Page* _rp, Page* _ep) {
        Region _r;
        List<UseSyntax>& list = *new(alignof(List<UseSyntax>), _r.get_page()) List<UseSyntax>();;
        while(true) {
            auto node_result = this->parse_use(_rp, _ep);
            if ((node_result._tag == Result<UseSyntax, ParserError>::Error) && (node_result._Error._tag == ParserError::InvalidSyntax))
                return Result<Vector<UseSyntax>*, ParserError> { ._tag = Result<Vector<UseSyntax>*, ParserError>::Error, ._Error = node_result._Error };
            if (node_result._tag == Result<UseSyntax, ParserError>::Ok) {
                auto node = node_result._Ok;
                list.add(node);
            } else {
                if ((list.count() == 0) && (node_result._tag == Result<UseSyntax, ParserError>::Error) && (node_result._Error._tag == ParserError::OtherSyntax))
                    return Result<Vector<UseSyntax>*, ParserError> { ._tag = Result<Vector<UseSyntax>*, ParserError>::Error, ._Error = node_result._Error };
                break;
            }
        }

        if (list.count() == 0)
            return Result<Vector<UseSyntax>*, ParserError> { ._tag = Result<Vector<UseSyntax>*, ParserError>::Ok, ._Ok = nullptr };
        
        return Result<Vector<UseSyntax>*, ParserError> {
            ._tag = Result<Vector<UseSyntax>*, ParserError>::Ok,
            ._Ok = new(alignof(Vector<UseSyntax>), _rp) Vector<UseSyntax>(_rp, list) };
    }

    Result<UseSyntax, ParserError> parse_use(Page* _rp, Page* _ep) {
        auto start = this->lexer.previous_position;

        auto start_use_1 = this->lexer.previous_position;
        auto success_use_1 = this->lexer.parse_keyword(_rp, *this->keywords_index[40]);
        if (!success_use_1) {
            return Result<UseSyntax, ParserError> { ._tag = Result<UseSyntax, ParserError>::Error, ._Error = ParserError(OtherSyntax()) };
        }

        auto name_start = this->lexer.position;
        auto name_result = this->parse_name(_rp, _ep);
        if (name_result._tag == Result<NameSyntax, ParserError>::Error)
        {
            switch (name_result._Error._tag) {
                case ParserError::OtherSyntax:
                    return Result<UseSyntax, ParserError> { ._tag = Result<UseSyntax, ParserError>::Error, ._Error = ParserError(InvalidSyntax(name_start, lexer.position, String(_ep, "a valid Name syntax"))) };
                case ParserError::InvalidSyntax:
                    return Result<UseSyntax, ParserError> { ._tag = Result<UseSyntax, ParserError>::Error, ._Error = name_result._Error };
            }
        }

        auto name = name_result._Ok;

        auto start_colon_3 = this->lexer.previous_position;
        auto success_colon_3 = this->lexer.parse_colon(_rp);
        if (!success_colon_3) {
        }

        auto end = this->lexer.position;

        auto ret = UseSyntax(start, end, name);

        return Result<UseSyntax, ParserError> { ._tag = Result<UseSyntax, ParserError>::Ok, ._Ok = ret };
    }

    Result<ImplementSyntax, ParserError> parse_implement(Page* _rp, Page* _ep) {
        auto start = this->lexer.previous_position;

        auto start_implement_1 = this->lexer.previous_position;
        auto success_implement_1 = this->lexer.parse_keyword(_rp, *this->keywords_index[16]);
        if (!success_implement_1) {
            return Result<ImplementSyntax, ParserError> { ._tag = Result<ImplementSyntax, ParserError>::Error, ._Error = ParserError(OtherSyntax()) };
        }

        auto type_start = this->lexer.position;
        auto type_result = this->parse_type(_rp, _ep);
        if (type_result._tag == Result<TypeSyntax, ParserError>::Error)
        {
            switch (type_result._Error._tag) {
                case ParserError::OtherSyntax:
                    return Result<ImplementSyntax, ParserError> { ._tag = Result<ImplementSyntax, ParserError>::Error, ._Error = ParserError(InvalidSyntax(type_start, lexer.position, String(_ep, "a valid Type syntax"))) };
                case ParserError::InvalidSyntax:
                    return Result<ImplementSyntax, ParserError> { ._tag = Result<ImplementSyntax, ParserError>::Error, ._Error = type_result._Error };
            }
        }

        auto type = type_result._Ok;

        auto attributes_start = this->lexer.position;
        auto attributes_result = this->parse_attribute_list(_rp, _ep);
        if (attributes_result._tag == Result<Vector<AttributeSyntax>, ParserError>::Error)
        {
            switch (attributes_result._Error._tag) {
                case ParserError::OtherSyntax:
                    break;
                case ParserError::InvalidSyntax:
                    return Result<ImplementSyntax, ParserError> { ._tag = Result<ImplementSyntax, ParserError>::Error, ._Error = attributes_result._Error };
            }
        }

        auto attributes = attributes_result._tag == Result<Vector<AttributeSyntax>, ParserError>::Error ? nullptr : attributes_result._Ok;

        auto start_colon_4 = this->lexer.previous_position;
        auto success_colon_4 = this->lexer.parse_colon(_rp);
        if (!success_colon_4) {
        }

        auto start_left_curly_5 = this->lexer.previous_position;
        auto success_left_curly_5 = this->lexer.parse_punctuation('{');
        if (!success_left_curly_5) {
            return Result<ImplementSyntax, ParserError> { ._tag = Result<ImplementSyntax, ParserError>::Error, ._Error = ParserError(InvalidSyntax(start_left_curly_5, lexer.position, String(_ep, "{"))) };        }

        auto uses_start = this->lexer.position;
        auto uses_result = this->parse_use_list(_rp, _ep);
        if (uses_result._tag == Result<Vector<UseSyntax>, ParserError>::Error)
        {
            switch (uses_result._Error._tag) {
                case ParserError::OtherSyntax:
                    break;
                case ParserError::InvalidSyntax:
                    return Result<ImplementSyntax, ParserError> { ._tag = Result<ImplementSyntax, ParserError>::Error, ._Error = uses_result._Error };
            }
        }

        auto uses = uses_result._tag == Result<Vector<UseSyntax>, ParserError>::Error ? nullptr : uses_result._Ok;

        auto methods_start = this->lexer.position;
        auto methods_result = this->parse_method_list(_rp, _ep);
        if (methods_result._tag == Result<Vector<MethodSyntax>, ParserError>::Error)
        {
            switch (methods_result._Error._tag) {
                case ParserError::OtherSyntax:
                    break;
                case ParserError::InvalidSyntax:
                    return Result<ImplementSyntax, ParserError> { ._tag = Result<ImplementSyntax, ParserError>::Error, ._Error = methods_result._Error };
            }
        }

        auto methods = methods_result._tag == Result<Vector<MethodSyntax>, ParserError>::Error ? nullptr : methods_result._Ok;

        auto start_right_curly_8 = this->lexer.previous_position;
        auto success_right_curly_8 = this->lexer.parse_punctuation('}');
        if (!success_right_curly_8) {
            return Result<ImplementSyntax, ParserError> { ._tag = Result<ImplementSyntax, ParserError>::Error, ._Error = ParserError(InvalidSyntax(start_right_curly_8, lexer.position, String(_ep, "}"))) };        }

        auto start_colon_9 = this->lexer.previous_position;
        auto success_colon_9 = this->lexer.parse_colon(_rp);
        if (!success_colon_9) {
        }

        auto end = this->lexer.position;

        auto ret = ImplementSyntax(start, end, type, attributes, uses, methods);

        return Result<ImplementSyntax, ParserError> { ._tag = Result<ImplementSyntax, ParserError>::Ok, ._Ok = ret };
    }

    Result<TraitSyntax, ParserError> parse_trait(Page* _rp, Page* _ep) {
        auto start = this->lexer.previous_position;

        auto start_trait_1 = this->lexer.previous_position;
        auto success_trait_1 = this->lexer.parse_keyword(_rp, *this->keywords_index[38]);
        if (!success_trait_1) {
            return Result<TraitSyntax, ParserError> { ._tag = Result<TraitSyntax, ParserError>::Error, ._Error = ParserError(OtherSyntax()) };
        }

        auto name_start = this->lexer.position;
        auto name_result = this->parse_name(_rp, _ep);
        if (name_result._tag == Result<NameSyntax, ParserError>::Error)
        {
            switch (name_result._Error._tag) {
                case ParserError::OtherSyntax:
                    return Result<TraitSyntax, ParserError> { ._tag = Result<TraitSyntax, ParserError>::Error, ._Error = ParserError(InvalidSyntax(name_start, lexer.position, String(_ep, "a valid Name syntax"))) };
                case ParserError::InvalidSyntax:
                    return Result<TraitSyntax, ParserError> { ._tag = Result<TraitSyntax, ParserError>::Error, ._Error = name_result._Error };
            }
        }

        auto name = name_result._Ok;

        auto extension_start = this->lexer.position;
        auto extension_result = this->parse_extends(_rp, _ep);
        if (extension_result._tag == Result<ExtendsSyntax, ParserError>::Error)
        {
            switch (extension_result._Error._tag) {
                case ParserError::OtherSyntax:
                    break;
                case ParserError::InvalidSyntax:
                    return Result<TraitSyntax, ParserError> { ._tag = Result<TraitSyntax, ParserError>::Error, ._Error = extension_result._Error };
            }
        }

        ExtendsSyntax* extension = extension_result._tag == Result<ExtendsSyntax, ParserError>::Error ? nullptr : new(alignof(ExtendsSyntax), _rp) ExtendsSyntax(extension_result._Ok);

        auto attributes_start = this->lexer.position;
        auto attributes_result = this->parse_attribute_list(_rp, _ep);
        if (attributes_result._tag == Result<Vector<AttributeSyntax>, ParserError>::Error)
        {
            switch (attributes_result._Error._tag) {
                case ParserError::OtherSyntax:
                    break;
                case ParserError::InvalidSyntax:
                    return Result<TraitSyntax, ParserError> { ._tag = Result<TraitSyntax, ParserError>::Error, ._Error = attributes_result._Error };
            }
        }

        auto attributes = attributes_result._tag == Result<Vector<AttributeSyntax>, ParserError>::Error ? nullptr : attributes_result._Ok;

        auto start_left_curly_5 = this->lexer.previous_position;
        auto success_left_curly_5 = this->lexer.parse_punctuation('{');
        if (!success_left_curly_5) {
            return Result<TraitSyntax, ParserError> { ._tag = Result<TraitSyntax, ParserError>::Error, ._Error = ParserError(InvalidSyntax(start_left_curly_5, lexer.position, String(_ep, "{"))) };        }

        auto uses_start = this->lexer.position;
        auto uses_result = this->parse_use_list(_rp, _ep);
        if (uses_result._tag == Result<Vector<UseSyntax>, ParserError>::Error)
        {
            switch (uses_result._Error._tag) {
                case ParserError::OtherSyntax:
                    break;
                case ParserError::InvalidSyntax:
                    return Result<TraitSyntax, ParserError> { ._tag = Result<TraitSyntax, ParserError>::Error, ._Error = uses_result._Error };
            }
        }

        auto uses = uses_result._tag == Result<Vector<UseSyntax>, ParserError>::Error ? nullptr : uses_result._Ok;

        auto functions_start = this->lexer.position;
        auto functions_result = this->parse_method_list(_rp, _ep);
        if (functions_result._tag == Result<Vector<MethodSyntax>, ParserError>::Error)
        {
            switch (functions_result._Error._tag) {
                case ParserError::OtherSyntax:
                    break;
                case ParserError::InvalidSyntax:
                    return Result<TraitSyntax, ParserError> { ._tag = Result<TraitSyntax, ParserError>::Error, ._Error = functions_result._Error };
            }
        }

        auto functions = functions_result._tag == Result<Vector<MethodSyntax>, ParserError>::Error ? nullptr : functions_result._Ok;

        auto start_right_curly_8 = this->lexer.previous_position;
        auto success_right_curly_8 = this->lexer.parse_punctuation('}');
        if (!success_right_curly_8) {
            return Result<TraitSyntax, ParserError> { ._tag = Result<TraitSyntax, ParserError>::Error, ._Error = ParserError(InvalidSyntax(start_right_curly_8, lexer.position, String(_ep, "}"))) };        }

        auto start_colon_9 = this->lexer.previous_position;
        auto success_colon_9 = this->lexer.parse_colon(_rp);
        if (!success_colon_9) {
        }

        auto end = this->lexer.position;

        auto ret = TraitSyntax(start, end, name, extension, attributes, uses, functions);

        return Result<TraitSyntax, ParserError> { ._tag = Result<TraitSyntax, ParserError>::Ok, ._Ok = ret };
    }

    Result<Vector<MethodSyntax>*, ParserError> parse_method_list(Page* _rp, Page* _ep) {
        Region _r;
        List<MethodSyntax>& list = *new(alignof(List<MethodSyntax>), _r.get_page()) List<MethodSyntax>();;
        while(true) {
            auto node_result = this->parse_method(_rp, _ep);
            if ((node_result._tag == Result<MethodSyntax, ParserError>::Error) && (node_result._Error._tag == ParserError::InvalidSyntax))
                return Result<Vector<MethodSyntax>*, ParserError> { ._tag = Result<Vector<MethodSyntax>*, ParserError>::Error, ._Error = node_result._Error };
            if (node_result._tag == Result<MethodSyntax, ParserError>::Ok) {
                auto node = node_result._Ok;
                list.add(node);
            } else {
                if ((list.count() == 0) && (node_result._tag == Result<MethodSyntax, ParserError>::Error) && (node_result._Error._tag == ParserError::OtherSyntax))
                    return Result<Vector<MethodSyntax>*, ParserError> { ._tag = Result<Vector<MethodSyntax>*, ParserError>::Error, ._Error = node_result._Error };
                break;
            }
        }

        if (list.count() == 0)
            return Result<Vector<MethodSyntax>*, ParserError> { ._tag = Result<Vector<MethodSyntax>*, ParserError>::Ok, ._Ok = nullptr };
        
        return Result<Vector<MethodSyntax>*, ParserError> {
            ._tag = Result<Vector<MethodSyntax>*, ParserError>::Ok,
            ._Ok = new(alignof(Vector<MethodSyntax>), _rp) Vector<MethodSyntax>(_rp, list) };
    }

    Result<MethodSyntax, ParserError> parse_method(Page* _rp, Page* _ep) {
        {
            auto node_result = this->parse_function(_rp, _ep);
            if (node_result._tag == Result<FunctionSyntax, ParserError>::Error)
            {
                if (node_result._Error._tag == ParserError::InvalidSyntax)
                    return Result<MethodSyntax, ParserError> { ._tag = Result<MethodSyntax, ParserError>::Error, ._Error = node_result._Error };
            }
            else
            {
                auto node = node_result._Ok;
                return Result<MethodSyntax, ParserError> { ._tag = Result<MethodSyntax, ParserError>::Ok, ._Ok = 
                    MethodSyntax(FunctionSyntax(node))
                };
            }
        }
        {
            auto node_result = this->parse_procedure(_rp, _ep);
            if (node_result._tag == Result<ProcedureSyntax, ParserError>::Error)
            {
                if (node_result._Error._tag == ParserError::InvalidSyntax)
                    return Result<MethodSyntax, ParserError> { ._tag = Result<MethodSyntax, ParserError>::Error, ._Error = node_result._Error };
            }
            else
            {
                auto node = node_result._Ok;
                return Result<MethodSyntax, ParserError> { ._tag = Result<MethodSyntax, ParserError>::Ok, ._Ok = 
                    MethodSyntax(ProcedureSyntax(node))
                };
            }
        }
        {
            auto node_result = this->parse_operator(_rp, _ep);
            if (node_result._tag == Result<OperatorSyntax, ParserError>::Error)
            {
                if (node_result._Error._tag == ParserError::InvalidSyntax)
                    return Result<MethodSyntax, ParserError> { ._tag = Result<MethodSyntax, ParserError>::Error, ._Error = node_result._Error };
            }
            else
            {
                auto node = node_result._Ok;
                return Result<MethodSyntax, ParserError> { ._tag = Result<MethodSyntax, ParserError>::Ok, ._Ok = 
                    MethodSyntax(OperatorSyntax(node))
                };
            }
        }
        return Result<MethodSyntax, ParserError> { ._tag = Result<MethodSyntax, ParserError>::Error, ._Error = ParserError(OtherSyntax()) };
    }

    Result<Vector<InitSyntax>*, ParserError> parse_init_list(Page* _rp, Page* _ep) {
        Region _r;
        List<InitSyntax>& list = *new(alignof(List<InitSyntax>), _r.get_page()) List<InitSyntax>();;
        while(true) {
            auto node_result = this->parse_init(_rp, _ep);
            if ((node_result._tag == Result<InitSyntax, ParserError>::Error) && (node_result._Error._tag == ParserError::InvalidSyntax))
                return Result<Vector<InitSyntax>*, ParserError> { ._tag = Result<Vector<InitSyntax>*, ParserError>::Error, ._Error = node_result._Error };
            if (node_result._tag == Result<InitSyntax, ParserError>::Ok) {
                auto node = node_result._Ok;
                list.add(node);
            } else {
                if ((list.count() == 0) && (node_result._tag == Result<InitSyntax, ParserError>::Error) && (node_result._Error._tag == ParserError::OtherSyntax))
                    return Result<Vector<InitSyntax>*, ParserError> { ._tag = Result<Vector<InitSyntax>*, ParserError>::Error, ._Error = node_result._Error };
                break;
            }
        }

        if (list.count() == 0)
            return Result<Vector<InitSyntax>*, ParserError> { ._tag = Result<Vector<InitSyntax>*, ParserError>::Ok, ._Ok = nullptr };
        
        return Result<Vector<InitSyntax>*, ParserError> {
            ._tag = Result<Vector<InitSyntax>*, ParserError>::Ok,
            ._Ok = new(alignof(Vector<InitSyntax>), _rp) Vector<InitSyntax>(_rp, list) };
    }

    Result<InitSyntax, ParserError> parse_init(Page* _rp, Page* _ep) {
        auto start = this->lexer.previous_position;

        auto start_init_1 = this->lexer.previous_position;
        auto success_init_1 = this->lexer.parse_keyword(_rp, *this->keywords_index[15]);
        if (!success_init_1) {
            return Result<InitSyntax, ParserError> { ._tag = Result<InitSyntax, ParserError>::Error, ._Error = ParserError(OtherSyntax()) };
        }

        auto parameters_start = this->lexer.position;
        auto parameters_result = this->parse_parameterset(_rp, _ep);
        if (parameters_result._tag == Result<ParameterSetSyntax, ParserError>::Error)
        {
            switch (parameters_result._Error._tag) {
                case ParserError::OtherSyntax:
                    break;
                case ParserError::InvalidSyntax:
                    return Result<InitSyntax, ParserError> { ._tag = Result<InitSyntax, ParserError>::Error, ._Error = parameters_result._Error };
            }
        }

        ParameterSetSyntax* parameters = parameters_result._tag == Result<ParameterSetSyntax, ParserError>::Error ? nullptr : new(alignof(ParameterSetSyntax), _rp) ParameterSetSyntax(parameters_result._Ok);

        auto start_colon_3 = this->lexer.previous_position;
        auto success_colon_3 = this->lexer.parse_colon(_rp);
        if (!success_colon_3) {
        }

        auto action_start = this->lexer.position;
        auto action_result = this->parse_action(_rp, _ep);
        if (action_result._tag == Result<ActionSyntax, ParserError>::Error)
        {
            switch (action_result._Error._tag) {
                case ParserError::OtherSyntax:
                    return Result<InitSyntax, ParserError> { ._tag = Result<InitSyntax, ParserError>::Error, ._Error = ParserError(InvalidSyntax(action_start, lexer.position, String(_ep, "a valid Action syntax"))) };
                case ParserError::InvalidSyntax:
                    return Result<InitSyntax, ParserError> { ._tag = Result<InitSyntax, ParserError>::Error, ._Error = action_result._Error };
            }
        }

        auto action = action_result._Ok;

        auto start_colon_5 = this->lexer.previous_position;
        auto success_colon_5 = this->lexer.parse_colon(_rp);
        if (!success_colon_5) {
        }

        auto end = this->lexer.position;

        auto ret = InitSyntax(start, end, parameters, action);

        return Result<InitSyntax, ParserError> { ._tag = Result<InitSyntax, ParserError>::Ok, ._Ok = ret };
    }

    Result<DeInitSyntax, ParserError> parse_deinit(Page* _rp, Page* _ep) {
        auto start = this->lexer.previous_position;

        auto start_deinit_1 = this->lexer.previous_position;
        auto success_deinit_1 = this->lexer.parse_keyword(_rp, *this->keywords_index[6]);
        if (!success_deinit_1) {
            return Result<DeInitSyntax, ParserError> { ._tag = Result<DeInitSyntax, ParserError>::Error, ._Error = ParserError(OtherSyntax()) };
        }

        auto start_colon_2 = this->lexer.previous_position;
        auto success_colon_2 = this->lexer.parse_colon(_rp);
        if (!success_colon_2) {
        }

        auto action_start = this->lexer.position;
        auto action_result = this->parse_action(_rp, _ep);
        if (action_result._tag == Result<ActionSyntax, ParserError>::Error)
        {
            switch (action_result._Error._tag) {
                case ParserError::OtherSyntax:
                    return Result<DeInitSyntax, ParserError> { ._tag = Result<DeInitSyntax, ParserError>::Error, ._Error = ParserError(InvalidSyntax(action_start, lexer.position, String(_ep, "a valid Action syntax"))) };
                case ParserError::InvalidSyntax:
                    return Result<DeInitSyntax, ParserError> { ._tag = Result<DeInitSyntax, ParserError>::Error, ._Error = action_result._Error };
            }
        }

        auto action = action_result._Ok;

        auto start_colon_4 = this->lexer.previous_position;
        auto success_colon_4 = this->lexer.parse_colon(_rp);
        if (!success_colon_4) {
        }

        auto end = this->lexer.position;

        auto ret = DeInitSyntax(start, end, action);

        return Result<DeInitSyntax, ParserError> { ._tag = Result<DeInitSyntax, ParserError>::Ok, ._Ok = ret };
    }

    Result<FunctionSyntax, ParserError> parse_function(Page* _rp, Page* _ep) {
        auto start = this->lexer.previous_position;

        auto start_function_1 = this->lexer.previous_position;
        auto success_function_1 = this->lexer.parse_keyword(_rp, *this->keywords_index[13]);
        if (!success_function_1) {
            return Result<FunctionSyntax, ParserError> { ._tag = Result<FunctionSyntax, ParserError>::Error, ._Error = ParserError(OtherSyntax()) };
        }

        auto target_start = this->lexer.position;
        auto target_result = this->parse_target(_rp, _ep);
        if (target_result._tag == Result<TargetSyntax, ParserError>::Error)
        {
            switch (target_result._Error._tag) {
                case ParserError::OtherSyntax:
                    return Result<FunctionSyntax, ParserError> { ._tag = Result<FunctionSyntax, ParserError>::Error, ._Error = ParserError(InvalidSyntax(target_start, lexer.position, String(_ep, "a valid Target syntax"))) };
                case ParserError::InvalidSyntax:
                    return Result<FunctionSyntax, ParserError> { ._tag = Result<FunctionSyntax, ParserError>::Error, ._Error = target_result._Error };
            }
        }

        auto target = target_result._Ok;

        auto end = this->lexer.position;

        auto ret = FunctionSyntax(start, end, target);

        return Result<FunctionSyntax, ParserError> { ._tag = Result<FunctionSyntax, ParserError>::Ok, ._Ok = ret };
    }

    Result<ProcedureSyntax, ParserError> parse_procedure(Page* _rp, Page* _ep) {
        auto start = this->lexer.previous_position;

        auto start_procedure_1 = this->lexer.previous_position;
        auto success_procedure_1 = this->lexer.parse_keyword(_rp, *this->keywords_index[29]);
        if (!success_procedure_1) {
            return Result<ProcedureSyntax, ParserError> { ._tag = Result<ProcedureSyntax, ParserError>::Error, ._Error = ParserError(OtherSyntax()) };
        }

        auto target_start = this->lexer.position;
        auto target_result = this->parse_target(_rp, _ep);
        if (target_result._tag == Result<TargetSyntax, ParserError>::Error)
        {
            switch (target_result._Error._tag) {
                case ParserError::OtherSyntax:
                    return Result<ProcedureSyntax, ParserError> { ._tag = Result<ProcedureSyntax, ParserError>::Error, ._Error = ParserError(InvalidSyntax(target_start, lexer.position, String(_ep, "a valid Target syntax"))) };
                case ParserError::InvalidSyntax:
                    return Result<ProcedureSyntax, ParserError> { ._tag = Result<ProcedureSyntax, ParserError>::Error, ._Error = target_result._Error };
            }
        }

        auto target = target_result._Ok;

        auto end = this->lexer.position;

        auto ret = ProcedureSyntax(start, end, target);

        return Result<ProcedureSyntax, ParserError> { ._tag = Result<ProcedureSyntax, ParserError>::Ok, ._Ok = ret };
    }

    Result<OperatorSyntax, ParserError> parse_operator(Page* _rp, Page* _ep) {
        auto start = this->lexer.previous_position;

        auto start_operator_1 = this->lexer.previous_position;
        auto success_operator_1 = this->lexer.parse_keyword(_rp, *this->keywords_index[28]);
        if (!success_operator_1) {
            return Result<OperatorSyntax, ParserError> { ._tag = Result<OperatorSyntax, ParserError>::Error, ._Error = ParserError(OtherSyntax()) };
        }

        auto target_start = this->lexer.position;
        auto target_result = this->parse_target(_rp, _ep);
        if (target_result._tag == Result<TargetSyntax, ParserError>::Error)
        {
            switch (target_result._Error._tag) {
                case ParserError::OtherSyntax:
                    return Result<OperatorSyntax, ParserError> { ._tag = Result<OperatorSyntax, ParserError>::Error, ._Error = ParserError(InvalidSyntax(target_start, lexer.position, String(_ep, "a valid Target syntax"))) };
                case ParserError::InvalidSyntax:
                    return Result<OperatorSyntax, ParserError> { ._tag = Result<OperatorSyntax, ParserError>::Error, ._Error = target_result._Error };
            }
        }

        auto target = target_result._Ok;

        auto end = this->lexer.position;

        auto ret = OperatorSyntax(start, end, target);

        return Result<OperatorSyntax, ParserError> { ._tag = Result<OperatorSyntax, ParserError>::Ok, ._Ok = ret };
    }

    Result<TargetSyntax, ParserError> parse_target(Page* _rp, Page* _ep) {
        {
            auto node_result = this->parse_named(_rp, _ep);
            if (node_result._tag == Result<NamedSyntax, ParserError>::Error)
            {
                if (node_result._Error._tag == ParserError::InvalidSyntax)
                    return Result<TargetSyntax, ParserError> { ._tag = Result<TargetSyntax, ParserError>::Error, ._Error = node_result._Error };
            }
            else
            {
                auto node = node_result._Ok;
                return Result<TargetSyntax, ParserError> { ._tag = Result<TargetSyntax, ParserError>::Ok, ._Ok = 
                    TargetSyntax(NamedSyntax(node))
                };
            }
        }
        {
            auto node_result = this->parse_routine(_rp, _ep);
            if (node_result._tag == Result<RoutineSyntax, ParserError>::Error)
            {
                if (node_result._Error._tag == ParserError::InvalidSyntax)
                    return Result<TargetSyntax, ParserError> { ._tag = Result<TargetSyntax, ParserError>::Error, ._Error = node_result._Error };
            }
            else
            {
                auto node = node_result._Ok;
                return Result<TargetSyntax, ParserError> { ._tag = Result<TargetSyntax, ParserError>::Ok, ._Ok = 
                    TargetSyntax(RoutineSyntax(node))
                };
            }
        }
        return Result<TargetSyntax, ParserError> { ._tag = Result<TargetSyntax, ParserError>::Error, ._Error = ParserError(OtherSyntax()) };
    }

    Result<NamedSyntax, ParserError> parse_named(Page* _rp, Page* _ep) {
        auto start = this->lexer.previous_position;

        auto start_name = this->lexer.previous_position;
        auto name = this->lexer.parse_identifier(_rp, this->keywords);
        if (name == nullptr)
        {
            return Result<NamedSyntax, ParserError> { ._tag = Result<NamedSyntax, ParserError>::Error, ._Error = ParserError(OtherSyntax()) };

        }

        auto routine_start = this->lexer.position;
        auto routine_result = this->parse_routine(_rp, _ep);
        if (routine_result._tag == Result<RoutineSyntax, ParserError>::Error)
        {
            switch (routine_result._Error._tag) {
                case ParserError::OtherSyntax:
                    return Result<NamedSyntax, ParserError> { ._tag = Result<NamedSyntax, ParserError>::Error, ._Error = ParserError(InvalidSyntax(routine_start, lexer.position, String(_ep, "a valid Routine syntax"))) };
                case ParserError::InvalidSyntax:
                    return Result<NamedSyntax, ParserError> { ._tag = Result<NamedSyntax, ParserError>::Error, ._Error = routine_result._Error };
            }
        }

        auto routine = routine_result._Ok;

        auto end = this->lexer.position;

        auto ret = NamedSyntax(start, end, *name, routine);

        return Result<NamedSyntax, ParserError> { ._tag = Result<NamedSyntax, ParserError>::Ok, ._Ok = ret };
    }

    Result<RoutineSyntax, ParserError> parse_routine(Page* _rp, Page* _ep) {
        auto start = this->lexer.previous_position;

        auto generics_start = this->lexer.position;
        auto generics_result = this->parse_genericarguments(_rp, _ep);
        if (generics_result._tag == Result<GenericArgumentsSyntax, ParserError>::Error)
        {
            switch (generics_result._Error._tag) {
                case ParserError::OtherSyntax:
                    break;
                case ParserError::InvalidSyntax:
                    return Result<RoutineSyntax, ParserError> { ._tag = Result<RoutineSyntax, ParserError>::Error, ._Error = generics_result._Error };
            }
        }

        GenericArgumentsSyntax* generics = generics_result._tag == Result<GenericArgumentsSyntax, ParserError>::Error ? nullptr : new(alignof(GenericArgumentsSyntax), _rp) GenericArgumentsSyntax(generics_result._Ok);

        auto lifetime_start = this->lexer.position;
        auto lifetime_result = this->parse_lifetime(_rp, _ep);
        if (lifetime_result._tag == Result<LifetimeSyntax, ParserError>::Error)
        {
            switch (lifetime_result._Error._tag) {
                case ParserError::OtherSyntax:
                    break;
                case ParserError::InvalidSyntax:
                    return Result<RoutineSyntax, ParserError> { ._tag = Result<RoutineSyntax, ParserError>::Error, ._Error = lifetime_result._Error };
            }
        }

        LifetimeSyntax* lifetime = lifetime_result._tag == Result<LifetimeSyntax, ParserError>::Error ? nullptr : new(alignof(LifetimeSyntax), _rp) LifetimeSyntax(lifetime_result._Ok);

        auto parameters_start = this->lexer.position;
        auto parameters_result = this->parse_parameterset(_rp, _ep);
        if (parameters_result._tag == Result<ParameterSetSyntax, ParserError>::Error)
        {
            switch (parameters_result._Error._tag) {
                case ParserError::OtherSyntax:
                    break;
                case ParserError::InvalidSyntax:
                    return Result<RoutineSyntax, ParserError> { ._tag = Result<RoutineSyntax, ParserError>::Error, ._Error = parameters_result._Error };
            }
        }

        ParameterSetSyntax* parameters = parameters_result._tag == Result<ParameterSetSyntax, ParserError>::Error ? nullptr : new(alignof(ParameterSetSyntax), _rp) ParameterSetSyntax(parameters_result._Ok);

        auto attributes_start = this->lexer.position;
        auto attributes_result = this->parse_attribute_list(_rp, _ep);
        if (attributes_result._tag == Result<Vector<AttributeSyntax>, ParserError>::Error)
        {
            switch (attributes_result._Error._tag) {
                case ParserError::OtherSyntax:
                    break;
                case ParserError::InvalidSyntax:
                    return Result<RoutineSyntax, ParserError> { ._tag = Result<RoutineSyntax, ParserError>::Error, ._Error = attributes_result._Error };
            }
        }

        auto attributes = attributes_result._tag == Result<Vector<AttributeSyntax>, ParserError>::Error ? nullptr : attributes_result._Ok;

        auto returns__start = this->lexer.position;
        auto returns__result = this->parse_returns(_rp, _ep);
        if (returns__result._tag == Result<ReturnsSyntax, ParserError>::Error)
        {
            switch (returns__result._Error._tag) {
                case ParserError::OtherSyntax:
                    break;
                case ParserError::InvalidSyntax:
                    return Result<RoutineSyntax, ParserError> { ._tag = Result<RoutineSyntax, ParserError>::Error, ._Error = returns__result._Error };
            }
        }

        ReturnsSyntax* returns_ = returns__result._tag == Result<ReturnsSyntax, ParserError>::Error ? nullptr : new(alignof(ReturnsSyntax), _rp) ReturnsSyntax(returns__result._Ok);

        auto start_colon_6 = this->lexer.previous_position;
        auto success_colon_6 = this->lexer.parse_colon(_rp);
        if (!success_colon_6) {
        }

        auto throws__start = this->lexer.position;
        auto throws__result = this->parse_throws(_rp, _ep);
        if (throws__result._tag == Result<ThrowsSyntax, ParserError>::Error)
        {
            switch (throws__result._Error._tag) {
                case ParserError::OtherSyntax:
                    break;
                case ParserError::InvalidSyntax:
                    return Result<RoutineSyntax, ParserError> { ._tag = Result<RoutineSyntax, ParserError>::Error, ._Error = throws__result._Error };
            }
        }

        ThrowsSyntax* throws_ = throws__result._tag == Result<ThrowsSyntax, ParserError>::Error ? nullptr : new(alignof(ThrowsSyntax), _rp) ThrowsSyntax(throws__result._Ok);

        auto start_colon_8 = this->lexer.previous_position;
        auto success_colon_8 = this->lexer.parse_colon(_rp);
        if (!success_colon_8) {
        }

        auto implementation_start = this->lexer.position;
        auto implementation_result = this->parse_implementation(_rp, _ep);
        if (implementation_result._tag == Result<ImplementationSyntax, ParserError>::Error)
        {
            switch (implementation_result._Error._tag) {
                case ParserError::OtherSyntax:
                    return Result<RoutineSyntax, ParserError> { ._tag = Result<RoutineSyntax, ParserError>::Error, ._Error = ParserError(InvalidSyntax(implementation_start, lexer.position, String(_ep, "a valid Implementation syntax"))) };
                case ParserError::InvalidSyntax:
                    return Result<RoutineSyntax, ParserError> { ._tag = Result<RoutineSyntax, ParserError>::Error, ._Error = implementation_result._Error };
            }
        }

        auto implementation = implementation_result._Ok;

        auto end = this->lexer.position;

        auto ret = RoutineSyntax(start, end, generics, lifetime, parameters, attributes, returns_, throws_, implementation);

        return Result<RoutineSyntax, ParserError> { ._tag = Result<RoutineSyntax, ParserError>::Ok, ._Ok = ret };
    }

    Result<ImplementationSyntax, ParserError> parse_implementation(Page* _rp, Page* _ep) {
        {
            auto node_result = this->parse_action(_rp, _ep);
            if (node_result._tag == Result<ActionSyntax, ParserError>::Error)
            {
                if (node_result._Error._tag == ParserError::InvalidSyntax)
                    return Result<ImplementationSyntax, ParserError> { ._tag = Result<ImplementationSyntax, ParserError>::Error, ._Error = node_result._Error };
            }
            else
            {
                auto node = node_result._Ok;
                return Result<ImplementationSyntax, ParserError> { ._tag = Result<ImplementationSyntax, ParserError>::Ok, ._Ok = 
                    ImplementationSyntax(ActionSyntax(node))
                };
            }
        }
        {
            auto node_result = this->parse_extern(_rp, _ep);
            if (node_result._tag == Result<ExternSyntax, ParserError>::Error)
            {
                if (node_result._Error._tag == ParserError::InvalidSyntax)
                    return Result<ImplementationSyntax, ParserError> { ._tag = Result<ImplementationSyntax, ParserError>::Error, ._Error = node_result._Error };
            }
            else
            {
                auto node = node_result._Ok;
                return Result<ImplementationSyntax, ParserError> { ._tag = Result<ImplementationSyntax, ParserError>::Ok, ._Ok = 
                    ImplementationSyntax(ExternSyntax(node))
                };
            }
        }
        {
            auto node_result = this->parse_instruction(_rp, _ep);
            if (node_result._tag == Result<InstructionSyntax, ParserError>::Error)
            {
                if (node_result._Error._tag == ParserError::InvalidSyntax)
                    return Result<ImplementationSyntax, ParserError> { ._tag = Result<ImplementationSyntax, ParserError>::Error, ._Error = node_result._Error };
            }
            else
            {
                auto node = node_result._Ok;
                return Result<ImplementationSyntax, ParserError> { ._tag = Result<ImplementationSyntax, ParserError>::Ok, ._Ok = 
                    ImplementationSyntax(InstructionSyntax(node))
                };
            }
        }
        {
            auto node_result = this->parse_intrinsic(_rp, _ep);
            if (node_result._tag == Result<IntrinsicSyntax, ParserError>::Error)
            {
                if (node_result._Error._tag == ParserError::InvalidSyntax)
                    return Result<ImplementationSyntax, ParserError> { ._tag = Result<ImplementationSyntax, ParserError>::Error, ._Error = node_result._Error };
            }
            else
            {
                auto node = node_result._Ok;
                return Result<ImplementationSyntax, ParserError> { ._tag = Result<ImplementationSyntax, ParserError>::Ok, ._Ok = 
                    ImplementationSyntax(IntrinsicSyntax(node))
                };
            }
        }
        return Result<ImplementationSyntax, ParserError> { ._tag = Result<ImplementationSyntax, ParserError>::Error, ._Error = ParserError(OtherSyntax()) };
    }

    Result<ExternSyntax, ParserError> parse_extern(Page* _rp, Page* _ep) {
        auto start = this->lexer.previous_position;

        auto start_extern_1 = this->lexer.previous_position;
        auto success_extern_1 = this->lexer.parse_keyword(_rp, *this->keywords_index[11]);
        if (!success_extern_1) {
            return Result<ExternSyntax, ParserError> { ._tag = Result<ExternSyntax, ParserError>::Error, ._Error = ParserError(OtherSyntax()) };
        }

        auto end = this->lexer.position;

        auto ret = ExternSyntax(start, end);

        return Result<ExternSyntax, ParserError> { ._tag = Result<ExternSyntax, ParserError>::Ok, ._Ok = ret };
    }

    Result<InstructionSyntax, ParserError> parse_instruction(Page* _rp, Page* _ep) {
        auto start = this->lexer.previous_position;

        auto start_instruction_1 = this->lexer.previous_position;
        auto success_instruction_1 = this->lexer.parse_keyword(_rp, *this->keywords_index[18]);
        if (!success_instruction_1) {
            return Result<InstructionSyntax, ParserError> { ._tag = Result<InstructionSyntax, ParserError>::Error, ._Error = ParserError(OtherSyntax()) };
        }

        auto start_colon_2 = this->lexer.previous_position;
        auto success_colon_2 = this->lexer.parse_colon(_rp);
        if (!success_colon_2) {
        }

        auto end = this->lexer.position;

        auto ret = InstructionSyntax(start, end);

        return Result<InstructionSyntax, ParserError> { ._tag = Result<InstructionSyntax, ParserError>::Ok, ._Ok = ret };
    }

    Result<IntrinsicSyntax, ParserError> parse_intrinsic(Page* _rp, Page* _ep) {
        auto start = this->lexer.previous_position;

        auto start_intrinsic_1 = this->lexer.previous_position;
        auto success_intrinsic_1 = this->lexer.parse_keyword(_rp, *this->keywords_index[19]);
        if (!success_intrinsic_1) {
            return Result<IntrinsicSyntax, ParserError> { ._tag = Result<IntrinsicSyntax, ParserError>::Error, ._Error = ParserError(OtherSyntax()) };
        }

        auto start_colon_2 = this->lexer.previous_position;
        auto success_colon_2 = this->lexer.parse_colon(_rp);
        if (!success_colon_2) {
        }

        auto end = this->lexer.position;

        auto ret = IntrinsicSyntax(start, end);

        return Result<IntrinsicSyntax, ParserError> { ._tag = Result<IntrinsicSyntax, ParserError>::Ok, ._Ok = ret };
    }

    Result<ExtendsSyntax, ParserError> parse_extends(Page* _rp, Page* _ep) {
        auto start = this->lexer.previous_position;

        auto start_extends_1 = this->lexer.previous_position;
        auto success_extends_1 = this->lexer.parse_keyword(_rp, *this->keywords_index[10]);
        if (!success_extends_1) {
            return Result<ExtendsSyntax, ParserError> { ._tag = Result<ExtendsSyntax, ParserError>::Error, ._Error = ParserError(OtherSyntax()) };
        }

        auto extensions_start = this->lexer.position;
        auto extensions_result = this->parse_extend_list(_rp, _ep);
        if (extensions_result._tag == Result<Vector<ExtendSyntax>, ParserError>::Error)
        {
            switch (extensions_result._Error._tag) {
                case ParserError::OtherSyntax:
                    break;
                case ParserError::InvalidSyntax:
                    return Result<ExtendsSyntax, ParserError> { ._tag = Result<ExtendsSyntax, ParserError>::Error, ._Error = extensions_result._Error };
            }
        }

        auto extensions = extensions_result._tag == Result<Vector<ExtendSyntax>, ParserError>::Error ? nullptr : extensions_result._Ok;

        auto end = this->lexer.position;

        auto ret = ExtendsSyntax(start, end, extensions);

        return Result<ExtendsSyntax, ParserError> { ._tag = Result<ExtendsSyntax, ParserError>::Ok, ._Ok = ret };
    }

    Result<Vector<ExtendSyntax>*, ParserError> parse_extend_list(Page* _rp, Page* _ep) {
        Region _r;
        List<ExtendSyntax>& list = *new(alignof(List<ExtendSyntax>), _r.get_page()) List<ExtendSyntax>();;
        while(true) {
            auto node_result = this->parse_extend(_rp, _ep);
            if ((node_result._tag == Result<ExtendSyntax, ParserError>::Error) && (node_result._Error._tag == ParserError::InvalidSyntax))
                return Result<Vector<ExtendSyntax>*, ParserError> { ._tag = Result<Vector<ExtendSyntax>*, ParserError>::Error, ._Error = node_result._Error };
            if (node_result._tag == Result<ExtendSyntax, ParserError>::Ok) {
                auto node = node_result._Ok;
                list.add(node);
            } else {
                if ((list.count() == 0) && (node_result._tag == Result<ExtendSyntax, ParserError>::Error) && (node_result._Error._tag == ParserError::OtherSyntax))
                    return Result<Vector<ExtendSyntax>*, ParserError> { ._tag = Result<Vector<ExtendSyntax>*, ParserError>::Error, ._Error = node_result._Error };
                break;
            }
        }

        if (list.count() == 0)
            return Result<Vector<ExtendSyntax>*, ParserError> { ._tag = Result<Vector<ExtendSyntax>*, ParserError>::Ok, ._Ok = nullptr };
        
        return Result<Vector<ExtendSyntax>*, ParserError> {
            ._tag = Result<Vector<ExtendSyntax>*, ParserError>::Ok,
            ._Ok = new(alignof(Vector<ExtendSyntax>), _rp) Vector<ExtendSyntax>(_rp, list) };
    }

    Result<ExtendSyntax, ParserError> parse_extend(Page* _rp, Page* _ep) {
        auto start = this->lexer.previous_position;

        auto type_start = this->lexer.position;
        auto type_result = this->parse_type(_rp, _ep);
        if (type_result._tag == Result<TypeSyntax, ParserError>::Error)
        {
            return Result<ExtendSyntax, ParserError> { ._tag = Result<ExtendSyntax, ParserError>::Error, ._Error = type_result._Error };
        }

        auto type = type_result._Ok;

        auto start_comma_2 = this->lexer.previous_position;
        auto success_comma_2 = this->lexer.parse_punctuation(',');
        if (!success_comma_2) {
        }

        auto end = this->lexer.position;

        auto ret = ExtendSyntax(start, end, type);

        return Result<ExtendSyntax, ParserError> { ._tag = Result<ExtendSyntax, ParserError>::Ok, ._Ok = ret };
    }

    Result<MacroSyntax, ParserError> parse_macro(Page* _rp, Page* _ep) {
        auto start = this->lexer.previous_position;

        auto start_macro_1 = this->lexer.previous_position;
        auto success_macro_1 = this->lexer.parse_keyword(_rp, *this->keywords_index[24]);
        if (!success_macro_1) {
            return Result<MacroSyntax, ParserError> { ._tag = Result<MacroSyntax, ParserError>::Error, ._Error = ParserError(OtherSyntax()) };
        }

        auto start_name = this->lexer.previous_position;
        auto name = this->lexer.parse_identifier(_rp, this->keywords);
        if (name == nullptr)
        {
            return Result<MacroSyntax, ParserError> { ._tag = Result<MacroSyntax, ParserError>::Error, ._Error = ParserError(InvalidSyntax(start_name, lexer.position, String(_ep, "an identifier"))) };
        }

        auto model_start = this->lexer.position;
        auto model_result = this->parse_model(_rp, _ep);
        if (model_result._tag == Result<ModelSyntax, ParserError>::Error)
        {
            switch (model_result._Error._tag) {
                case ParserError::OtherSyntax:
                    return Result<MacroSyntax, ParserError> { ._tag = Result<MacroSyntax, ParserError>::Error, ._Error = ParserError(InvalidSyntax(model_start, lexer.position, String(_ep, "a valid Model syntax"))) };
                case ParserError::InvalidSyntax:
                    return Result<MacroSyntax, ParserError> { ._tag = Result<MacroSyntax, ParserError>::Error, ._Error = model_result._Error };
            }
        }

        auto model = model_result._Ok;

        auto rule_start = this->lexer.position;
        auto rule_result = this->parse_operand_list(_rp, _ep);
        if (rule_result._tag == Result<Vector<OperandSyntax>, ParserError>::Error)
        {
            switch (rule_result._Error._tag) {
                case ParserError::OtherSyntax:
                    return Result<MacroSyntax, ParserError> { ._tag = Result<MacroSyntax, ParserError>::Error, ._Error = ParserError(InvalidSyntax(rule_start, lexer.position, String(_ep, "a valid Operand syntax"))) };
                case ParserError::InvalidSyntax:
                    return Result<MacroSyntax, ParserError> { ._tag = Result<MacroSyntax, ParserError>::Error, ._Error = rule_result._Error };
            }
        }

        auto rule = rule_result._Ok;

        auto end = this->lexer.position;

        auto ret = MacroSyntax(start, end, *name, model, rule);

        return Result<MacroSyntax, ParserError> { ._tag = Result<MacroSyntax, ParserError>::Ok, ._Ok = ret };
    }

    Result<Vector<AttributeSyntax>*, ParserError> parse_attribute_list(Page* _rp, Page* _ep) {
        Region _r;
        List<AttributeSyntax>& list = *new(alignof(List<AttributeSyntax>), _r.get_page()) List<AttributeSyntax>();;
        while(true) {
            auto node_result = this->parse_attribute(_rp, _ep);
            if ((node_result._tag == Result<AttributeSyntax, ParserError>::Error) && (node_result._Error._tag == ParserError::InvalidSyntax))
                return Result<Vector<AttributeSyntax>*, ParserError> { ._tag = Result<Vector<AttributeSyntax>*, ParserError>::Error, ._Error = node_result._Error };
            if (node_result._tag == Result<AttributeSyntax, ParserError>::Ok) {
                auto node = node_result._Ok;
                list.add(node);
            } else {
                if ((list.count() == 0) && (node_result._tag == Result<AttributeSyntax, ParserError>::Error) && (node_result._Error._tag == ParserError::OtherSyntax))
                    return Result<Vector<AttributeSyntax>*, ParserError> { ._tag = Result<Vector<AttributeSyntax>*, ParserError>::Error, ._Error = node_result._Error };
                break;
            }
        }

        if (list.count() == 0)
            return Result<Vector<AttributeSyntax>*, ParserError> { ._tag = Result<Vector<AttributeSyntax>*, ParserError>::Ok, ._Ok = nullptr };
        
        return Result<Vector<AttributeSyntax>*, ParserError> {
            ._tag = Result<Vector<AttributeSyntax>*, ParserError>::Ok,
            ._Ok = new(alignof(Vector<AttributeSyntax>), _rp) Vector<AttributeSyntax>(_rp, list) };
    }

    Result<AttributeSyntax, ParserError> parse_attribute(Page* _rp, Page* _ep) {
        auto start = this->lexer.previous_position;

        auto start_name = this->lexer.previous_position;
        auto name = this->lexer.parse_attribute(_rp);
        if (name == nullptr) {

            return Result<AttributeSyntax, ParserError> { ._tag = Result<AttributeSyntax, ParserError>::Error, ._Error = ParserError(OtherSyntax()) };
        }

        auto model_start = this->lexer.position;
        auto model_result = this->parse_model(_rp, _ep);
        if (model_result._tag == Result<ModelSyntax, ParserError>::Error)
        {
            switch (model_result._Error._tag) {
                case ParserError::OtherSyntax:
                    return Result<AttributeSyntax, ParserError> { ._tag = Result<AttributeSyntax, ParserError>::Error, ._Error = ParserError(InvalidSyntax(model_start, lexer.position, String(_ep, "a valid Model syntax"))) };
                case ParserError::InvalidSyntax:
                    return Result<AttributeSyntax, ParserError> { ._tag = Result<AttributeSyntax, ParserError>::Error, ._Error = model_result._Error };
            }
        }

        auto model = model_result._Ok;

        auto start_colon_3 = this->lexer.previous_position;
        auto success_colon_3 = this->lexer.parse_colon(_rp);
        if (!success_colon_3) {
        }

        auto end = this->lexer.position;

        auto ret = AttributeSyntax(start, end, *name, model);

        return Result<AttributeSyntax, ParserError> { ._tag = Result<AttributeSyntax, ParserError>::Ok, ._Ok = ret };
    }

    Result<ModelSyntax, ParserError> parse_model(Page* _rp, Page* _ep) {
        {
            auto node_result = this->parse_literal(_rp, _ep);
            if (node_result._tag == Result<LiteralSyntax, ParserError>::Error)
            {
                if (node_result._Error._tag == ParserError::InvalidSyntax)
                    return Result<ModelSyntax, ParserError> { ._tag = Result<ModelSyntax, ParserError>::Error, ._Error = node_result._Error };
            }
            else
            {
                auto node = node_result._Ok;
                return Result<ModelSyntax, ParserError> { ._tag = Result<ModelSyntax, ParserError>::Ok, ._Ok = 
                    ModelSyntax(LiteralSyntax(node))
                };
            }
        }
        {
            auto node_result = this->parse_name(_rp, _ep);
            if (node_result._tag == Result<NameSyntax, ParserError>::Error)
            {
                if (node_result._Error._tag == ParserError::InvalidSyntax)
                    return Result<ModelSyntax, ParserError> { ._tag = Result<ModelSyntax, ParserError>::Error, ._Error = node_result._Error };
            }
            else
            {
                auto node = node_result._Ok;
                return Result<ModelSyntax, ParserError> { ._tag = Result<ModelSyntax, ParserError>::Ok, ._Ok = 
                    ModelSyntax(NameSyntax(node))
                };
            }
        }
        {
            auto node_result = this->parse_object(_rp, _ep);
            if (node_result._tag == Result<ObjectSyntax, ParserError>::Error)
            {
                if (node_result._Error._tag == ParserError::InvalidSyntax)
                    return Result<ModelSyntax, ParserError> { ._tag = Result<ModelSyntax, ParserError>::Error, ._Error = node_result._Error };
            }
            else
            {
                auto node = node_result._Ok;
                return Result<ModelSyntax, ParserError> { ._tag = Result<ModelSyntax, ParserError>::Ok, ._Ok = 
                    ModelSyntax(ObjectSyntax(node))
                };
            }
        }
        {
            auto node_result = this->parse_vector(_rp, _ep);
            if (node_result._tag == Result<VectorSyntax, ParserError>::Error)
            {
                if (node_result._Error._tag == ParserError::InvalidSyntax)
                    return Result<ModelSyntax, ParserError> { ._tag = Result<ModelSyntax, ParserError>::Error, ._Error = node_result._Error };
            }
            else
            {
                auto node = node_result._Ok;
                return Result<ModelSyntax, ParserError> { ._tag = Result<ModelSyntax, ParserError>::Ok, ._Ok = 
                    ModelSyntax(VectorSyntax(node))
                };
            }
        }
        return Result<ModelSyntax, ParserError> { ._tag = Result<ModelSyntax, ParserError>::Error, ._Error = ParserError(OtherSyntax()) };
    }

    Result<ModuleSyntax, ParserError> parse_module(Page* _rp, Page* _ep) {
        auto start = this->lexer.previous_position;

        auto start_module_1 = this->lexer.previous_position;
        auto success_module_1 = this->lexer.parse_keyword(_rp, *this->keywords_index[26]);
        if (!success_module_1) {
            return Result<ModuleSyntax, ParserError> { ._tag = Result<ModuleSyntax, ParserError>::Error, ._Error = ParserError(OtherSyntax()) };
        }

        auto start_name = this->lexer.previous_position;
        auto name = this->lexer.parse_identifier(_rp, this->keywords);
        if (name == nullptr)
        {
            return Result<ModuleSyntax, ParserError> { ._tag = Result<ModuleSyntax, ParserError>::Error, ._Error = ParserError(InvalidSyntax(start_name, lexer.position, String(_ep, "an identifier"))) };
        }

        auto start_colon_3 = this->lexer.previous_position;
        auto success_colon_3 = this->lexer.parse_colon(_rp);
        if (!success_colon_3) {
        }

        auto end = this->lexer.position;

        auto ret = ModuleSyntax(start, end, *name);

        return Result<ModuleSyntax, ParserError> { ._tag = Result<ModuleSyntax, ParserError>::Ok, ._Ok = ret };
    }

    Result<Vector<PackageSyntax>*, ParserError> parse_package_list(Page* _rp, Page* _ep) {
        Region _r;
        List<PackageSyntax>& list = *new(alignof(List<PackageSyntax>), _r.get_page()) List<PackageSyntax>();;
        while(true) {
            auto node_result = this->parse_package(_rp, _ep);
            if ((node_result._tag == Result<PackageSyntax, ParserError>::Error) && (node_result._Error._tag == ParserError::InvalidSyntax))
                return Result<Vector<PackageSyntax>*, ParserError> { ._tag = Result<Vector<PackageSyntax>*, ParserError>::Error, ._Error = node_result._Error };
            if (node_result._tag == Result<PackageSyntax, ParserError>::Ok) {
                auto node = node_result._Ok;
                list.add(node);
            } else {
                if ((list.count() == 0) && (node_result._tag == Result<PackageSyntax, ParserError>::Error) && (node_result._Error._tag == ParserError::OtherSyntax))
                    return Result<Vector<PackageSyntax>*, ParserError> { ._tag = Result<Vector<PackageSyntax>*, ParserError>::Error, ._Error = node_result._Error };
                break;
            }
        }

        if (list.count() == 0)
            return Result<Vector<PackageSyntax>*, ParserError> { ._tag = Result<Vector<PackageSyntax>*, ParserError>::Ok, ._Ok = nullptr };
        
        return Result<Vector<PackageSyntax>*, ParserError> {
            ._tag = Result<Vector<PackageSyntax>*, ParserError>::Ok,
            ._Ok = new(alignof(Vector<PackageSyntax>), _rp) Vector<PackageSyntax>(_rp, list) };
    }

    Result<PackageSyntax, ParserError> parse_package(Page* _rp, Page* _ep) {
        auto start = this->lexer.previous_position;

        auto start_package_1 = this->lexer.previous_position;
        auto success_package_1 = this->lexer.parse_keyword(_rp, *this->keywords_index[43]);
        if (!success_package_1) {
            return Result<PackageSyntax, ParserError> { ._tag = Result<PackageSyntax, ParserError>::Error, ._Error = ParserError(OtherSyntax()) };
        }

        auto name_start = this->lexer.position;
        auto name_result = this->parse_name(_rp, _ep);
        if (name_result._tag == Result<NameSyntax, ParserError>::Error)
        {
            switch (name_result._Error._tag) {
                case ParserError::OtherSyntax:
                    return Result<PackageSyntax, ParserError> { ._tag = Result<PackageSyntax, ParserError>::Error, ._Error = ParserError(InvalidSyntax(name_start, lexer.position, String(_ep, "a valid Name syntax"))) };
                case ParserError::InvalidSyntax:
                    return Result<PackageSyntax, ParserError> { ._tag = Result<PackageSyntax, ParserError>::Error, ._Error = name_result._Error };
            }
        }

        auto name = name_result._Ok;

        auto start_colon_3 = this->lexer.previous_position;
        auto success_colon_3 = this->lexer.parse_colon(_rp);
        if (!success_colon_3) {
        }

        auto end = this->lexer.position;

        auto ret = PackageSyntax(start, end, name);

        return Result<PackageSyntax, ParserError> { ._tag = Result<PackageSyntax, ParserError>::Ok, ._Ok = ret };
    }

    Result<InitializerSyntax, ParserError> parse_initializer(Page* _rp, Page* _ep) {
        auto start = this->lexer.previous_position;

        auto start_left_paren_1 = this->lexer.previous_position;
        auto success_left_paren_1 = this->lexer.parse_punctuation('(');
        if (!success_left_paren_1) {
            return Result<InitializerSyntax, ParserError> { ._tag = Result<InitializerSyntax, ParserError>::Error, ._Error = ParserError(OtherSyntax()) };
        }

        auto operands_start = this->lexer.position;
        auto operands_result = this->parse_operand_list(_rp, _ep);
        if (operands_result._tag == Result<Vector<OperandSyntax>, ParserError>::Error)
        {
            switch (operands_result._Error._tag) {
                case ParserError::OtherSyntax:
                    return Result<InitializerSyntax, ParserError> { ._tag = Result<InitializerSyntax, ParserError>::Error, ._Error = ParserError(InvalidSyntax(operands_start, lexer.position, String(_ep, "a valid Operand syntax"))) };
                case ParserError::InvalidSyntax:
                    return Result<InitializerSyntax, ParserError> { ._tag = Result<InitializerSyntax, ParserError>::Error, ._Error = operands_result._Error };
            }
        }

        auto operands = operands_result._Ok;

        auto start_right_paren_3 = this->lexer.previous_position;
        auto success_right_paren_3 = this->lexer.parse_punctuation(')');
        if (!success_right_paren_3) {
            return Result<InitializerSyntax, ParserError> { ._tag = Result<InitializerSyntax, ParserError>::Error, ._Error = ParserError(InvalidSyntax(start_right_paren_3, lexer.position, String(_ep, ")"))) };        }

        auto end = this->lexer.position;

        auto ret = InitializerSyntax(start, end, operands);

        return Result<InitializerSyntax, ParserError> { ._tag = Result<InitializerSyntax, ParserError>::Ok, ._Ok = ret };
    }

    Result<Vector<OperandSyntax>*, ParserError> parse_operand_list(Page* _rp, Page* _ep) {
        Region _r;
        List<OperandSyntax>& list = *new(alignof(List<OperandSyntax>), _r.get_page()) List<OperandSyntax>();;
        while(true) {
            auto node_result = this->parse_operand(_rp, _ep);
            if ((node_result._tag == Result<OperandSyntax, ParserError>::Error) && (node_result._Error._tag == ParserError::InvalidSyntax))
                return Result<Vector<OperandSyntax>*, ParserError> { ._tag = Result<Vector<OperandSyntax>*, ParserError>::Error, ._Error = node_result._Error };
            if (node_result._tag == Result<OperandSyntax, ParserError>::Ok) {
                auto node = node_result._Ok;
                list.add(node);
            } else {
                if ((list.count() == 0) && (node_result._tag == Result<OperandSyntax, ParserError>::Error) && (node_result._Error._tag == ParserError::OtherSyntax))
                    return Result<Vector<OperandSyntax>*, ParserError> { ._tag = Result<Vector<OperandSyntax>*, ParserError>::Error, ._Error = node_result._Error };
                break;
            }
        }

        if (list.count() == 0)
            return Result<Vector<OperandSyntax>*, ParserError> { ._tag = Result<Vector<OperandSyntax>*, ParserError>::Ok, ._Ok = nullptr };
        
        return Result<Vector<OperandSyntax>*, ParserError> {
            ._tag = Result<Vector<OperandSyntax>*, ParserError>::Ok,
            ._Ok = new(alignof(Vector<OperandSyntax>), _rp) Vector<OperandSyntax>(_rp, list) };
    }

    Result<OperandSyntax, ParserError> parse_operand(Page* _rp, Page* _ep) {
        auto start = this->lexer.previous_position;

        auto expression_start = this->lexer.position;
        auto expression_result = this->parse_expression(_rp, _ep);
        if (expression_result._tag == Result<ExpressionSyntax, ParserError>::Error)
        {
            return Result<OperandSyntax, ParserError> { ._tag = Result<OperandSyntax, ParserError>::Error, ._Error = expression_result._Error };
        }

        auto expression = expression_result._Ok;

        auto postfixes_start = this->lexer.position;
        auto postfixes_result = this->parse_postfix_list(_rp, _ep);
        if (postfixes_result._tag == Result<Vector<PostfixSyntax>, ParserError>::Error)
        {
            switch (postfixes_result._Error._tag) {
                case ParserError::OtherSyntax:
                    break;
                case ParserError::InvalidSyntax:
                    return Result<OperandSyntax, ParserError> { ._tag = Result<OperandSyntax, ParserError>::Error, ._Error = postfixes_result._Error };
            }
        }

        auto postfixes = postfixes_result._tag == Result<Vector<PostfixSyntax>, ParserError>::Error ? nullptr : postfixes_result._Ok;

        auto end = this->lexer.position;

        auto ret = OperandSyntax(start, end, expression, postfixes);

        return Result<OperandSyntax, ParserError> { ._tag = Result<OperandSyntax, ParserError>::Ok, ._Ok = ret };
    }

    Result<Vector<PostfixSyntax>*, ParserError> parse_postfix_list(Page* _rp, Page* _ep) {
        Region _r;
        List<PostfixSyntax>& list = *new(alignof(List<PostfixSyntax>), _r.get_page()) List<PostfixSyntax>();;
        while(true) {
            auto node_result = this->parse_postfix(_rp, _ep);
            if ((node_result._tag == Result<PostfixSyntax, ParserError>::Error) && (node_result._Error._tag == ParserError::InvalidSyntax))
                return Result<Vector<PostfixSyntax>*, ParserError> { ._tag = Result<Vector<PostfixSyntax>*, ParserError>::Error, ._Error = node_result._Error };
            if (node_result._tag == Result<PostfixSyntax, ParserError>::Ok) {
                auto node = node_result._Ok;
                list.add(node);
            } else {
                if ((list.count() == 0) && (node_result._tag == Result<PostfixSyntax, ParserError>::Error) && (node_result._Error._tag == ParserError::OtherSyntax))
                    return Result<Vector<PostfixSyntax>*, ParserError> { ._tag = Result<Vector<PostfixSyntax>*, ParserError>::Error, ._Error = node_result._Error };
                break;
            }
        }

        if (list.count() == 0)
            return Result<Vector<PostfixSyntax>*, ParserError> { ._tag = Result<Vector<PostfixSyntax>*, ParserError>::Ok, ._Ok = nullptr };
        
        return Result<Vector<PostfixSyntax>*, ParserError> {
            ._tag = Result<Vector<PostfixSyntax>*, ParserError>::Ok,
            ._Ok = new(alignof(Vector<PostfixSyntax>), _rp) Vector<PostfixSyntax>(_rp, list) };
    }

    Result<PostfixSyntax, ParserError> parse_postfix(Page* _rp, Page* _ep) {
        {
            auto node_result = this->parse_memberaccess(_rp, _ep);
            if (node_result._tag == Result<MemberAccessSyntax, ParserError>::Error)
            {
                if (node_result._Error._tag == ParserError::InvalidSyntax)
                    return Result<PostfixSyntax, ParserError> { ._tag = Result<PostfixSyntax, ParserError>::Error, ._Error = node_result._Error };
            }
            else
            {
                auto node = node_result._Ok;
                return Result<PostfixSyntax, ParserError> { ._tag = Result<PostfixSyntax, ParserError>::Ok, ._Ok = 
                    PostfixSyntax(MemberAccessSyntax(node))
                };
            }
        }
        {
            auto node_result = this->parse_catcher(_rp, _ep);
            if (node_result._tag == Result<CatcherSyntax, ParserError>::Error)
            {
                if (node_result._Error._tag == ParserError::InvalidSyntax)
                    return Result<PostfixSyntax, ParserError> { ._tag = Result<PostfixSyntax, ParserError>::Error, ._Error = node_result._Error };
            }
            else
            {
                auto node = node_result._Ok;
                return Result<PostfixSyntax, ParserError> { ._tag = Result<PostfixSyntax, ParserError>::Ok, ._Ok = 
                    PostfixSyntax(CatcherSyntax(node))
                };
            }
        }
        return Result<PostfixSyntax, ParserError> { ._tag = Result<PostfixSyntax, ParserError>::Error, ._Error = ParserError(OtherSyntax()) };
    }

    Result<MemberAccessSyntax, ParserError> parse_memberaccess(Page* _rp, Page* _ep) {
        auto start = this->lexer.previous_position;

        auto start_dot_1 = this->lexer.previous_position;
        auto success_dot_1 = this->lexer.parse_punctuation('.');
        if (!success_dot_1) {
            return Result<MemberAccessSyntax, ParserError> { ._tag = Result<MemberAccessSyntax, ParserError>::Error, ._Error = ParserError(OtherSyntax()) };
        }

        auto member_start = this->lexer.position;
        auto member_result = this->parse_name(_rp, _ep);
        if (member_result._tag == Result<NameSyntax, ParserError>::Error)
        {
            switch (member_result._Error._tag) {
                case ParserError::OtherSyntax:
                    return Result<MemberAccessSyntax, ParserError> { ._tag = Result<MemberAccessSyntax, ParserError>::Error, ._Error = ParserError(InvalidSyntax(member_start, lexer.position, String(_ep, "a valid Name syntax"))) };
                case ParserError::InvalidSyntax:
                    return Result<MemberAccessSyntax, ParserError> { ._tag = Result<MemberAccessSyntax, ParserError>::Error, ._Error = member_result._Error };
            }
        }

        auto member = member_result._Ok;

        auto end = this->lexer.position;

        auto ret = MemberAccessSyntax(start, end, member);

        return Result<MemberAccessSyntax, ParserError> { ._tag = Result<MemberAccessSyntax, ParserError>::Ok, ._Ok = ret };
    }

    Result<CatcherSyntax, ParserError> parse_catcher(Page* _rp, Page* _ep) {
        auto start = this->lexer.previous_position;

        auto catches_start = this->lexer.position;
        auto catches_result = this->parse_catch_list(_rp, _ep);
        if (catches_result._tag == Result<Vector<CatchSyntax>, ParserError>::Error)
        {
            return Result<CatcherSyntax, ParserError> { ._tag = Result<CatcherSyntax, ParserError>::Error, ._Error = catches_result._Error };
        }

        auto catches = catches_result._Ok;

        auto dropper_start = this->lexer.position;
        auto dropper_result = this->parse_drop(_rp, _ep);
        if (dropper_result._tag == Result<DropSyntax, ParserError>::Error)
        {
            switch (dropper_result._Error._tag) {
                case ParserError::OtherSyntax:
                    break;
                case ParserError::InvalidSyntax:
                    return Result<CatcherSyntax, ParserError> { ._tag = Result<CatcherSyntax, ParserError>::Error, ._Error = dropper_result._Error };
            }
        }

        DropSyntax* dropper = dropper_result._tag == Result<DropSyntax, ParserError>::Error ? nullptr : new(alignof(DropSyntax), _rp) DropSyntax(dropper_result._Ok);

        auto end = this->lexer.position;

        auto ret = CatcherSyntax(start, end, catches, dropper);

        return Result<CatcherSyntax, ParserError> { ._tag = Result<CatcherSyntax, ParserError>::Ok, ._Ok = ret };
    }

    Result<Vector<CatchSyntax>*, ParserError> parse_catch_list(Page* _rp, Page* _ep) {
        Region _r;
        List<CatchSyntax>& list = *new(alignof(List<CatchSyntax>), _r.get_page()) List<CatchSyntax>();;
        while(true) {
            auto node_result = this->parse_catch(_rp, _ep);
            if ((node_result._tag == Result<CatchSyntax, ParserError>::Error) && (node_result._Error._tag == ParserError::InvalidSyntax))
                return Result<Vector<CatchSyntax>*, ParserError> { ._tag = Result<Vector<CatchSyntax>*, ParserError>::Error, ._Error = node_result._Error };
            if (node_result._tag == Result<CatchSyntax, ParserError>::Ok) {
                auto node = node_result._Ok;
                list.add(node);
            } else {
                if ((list.count() == 0) && (node_result._tag == Result<CatchSyntax, ParserError>::Error) && (node_result._Error._tag == ParserError::OtherSyntax))
                    return Result<Vector<CatchSyntax>*, ParserError> { ._tag = Result<Vector<CatchSyntax>*, ParserError>::Error, ._Error = node_result._Error };
                break;
            }
        }

        if (list.count() == 0)
            return Result<Vector<CatchSyntax>*, ParserError> { ._tag = Result<Vector<CatchSyntax>*, ParserError>::Ok, ._Ok = nullptr };
        
        return Result<Vector<CatchSyntax>*, ParserError> {
            ._tag = Result<Vector<CatchSyntax>*, ParserError>::Ok,
            ._Ok = new(alignof(Vector<CatchSyntax>), _rp) Vector<CatchSyntax>(_rp, list) };
    }

    Result<CatchSyntax, ParserError> parse_catch(Page* _rp, Page* _ep) {
        auto start = this->lexer.previous_position;

        auto start_catch_1 = this->lexer.previous_position;
        auto success_catch_1 = this->lexer.parse_keyword(_rp, *this->keywords_index[1]);
        if (!success_catch_1) {
            return Result<CatchSyntax, ParserError> { ._tag = Result<CatchSyntax, ParserError>::Error, ._Error = ParserError(OtherSyntax()) };
        }

        auto condition_start = this->lexer.position;
        auto condition_result = this->parse_operand_list(_rp, _ep);
        if (condition_result._tag == Result<Vector<OperandSyntax>, ParserError>::Error)
        {
            switch (condition_result._Error._tag) {
                case ParserError::OtherSyntax:
                    return Result<CatchSyntax, ParserError> { ._tag = Result<CatchSyntax, ParserError>::Error, ._Error = ParserError(InvalidSyntax(condition_start, lexer.position, String(_ep, "a valid Operand syntax"))) };
                case ParserError::InvalidSyntax:
                    return Result<CatchSyntax, ParserError> { ._tag = Result<CatchSyntax, ParserError>::Error, ._Error = condition_result._Error };
            }
        }

        auto condition = condition_result._Ok;

        auto start_colon_3 = this->lexer.previous_position;
        auto success_colon_3 = this->lexer.parse_colon(_rp);
        if (!success_colon_3) {
            return Result<CatchSyntax, ParserError> { ._tag = Result<CatchSyntax, ParserError>::Error, ._Error = ParserError(InvalidSyntax(start_colon_3, lexer.position, String(_ep, "a colon or a line feed"))) };        }

        auto handler_start = this->lexer.position;
        auto handler_result = this->parse_operand_list(_rp, _ep);
        if (handler_result._tag == Result<Vector<OperandSyntax>, ParserError>::Error)
        {
            switch (handler_result._Error._tag) {
                case ParserError::OtherSyntax:
                    return Result<CatchSyntax, ParserError> { ._tag = Result<CatchSyntax, ParserError>::Error, ._Error = ParserError(InvalidSyntax(handler_start, lexer.position, String(_ep, "a valid Operand syntax"))) };
                case ParserError::InvalidSyntax:
                    return Result<CatchSyntax, ParserError> { ._tag = Result<CatchSyntax, ParserError>::Error, ._Error = handler_result._Error };
            }
        }

        auto handler = handler_result._Ok;

        auto end = this->lexer.position;

        auto ret = CatchSyntax(start, end, condition, handler);

        return Result<CatchSyntax, ParserError> { ._tag = Result<CatchSyntax, ParserError>::Ok, ._Ok = ret };
    }

    Result<DropSyntax, ParserError> parse_drop(Page* _rp, Page* _ep) {
        auto start = this->lexer.previous_position;

        auto start_drop_1 = this->lexer.previous_position;
        auto success_drop_1 = this->lexer.parse_keyword(_rp, *this->keywords_index[8]);
        if (!success_drop_1) {
            return Result<DropSyntax, ParserError> { ._tag = Result<DropSyntax, ParserError>::Error, ._Error = ParserError(OtherSyntax()) };
        }

        auto handler_start = this->lexer.position;
        auto handler_result = this->parse_operand_list(_rp, _ep);
        if (handler_result._tag == Result<Vector<OperandSyntax>, ParserError>::Error)
        {
            switch (handler_result._Error._tag) {
                case ParserError::OtherSyntax:
                    return Result<DropSyntax, ParserError> { ._tag = Result<DropSyntax, ParserError>::Error, ._Error = ParserError(InvalidSyntax(handler_start, lexer.position, String(_ep, "a valid Operand syntax"))) };
                case ParserError::InvalidSyntax:
                    return Result<DropSyntax, ParserError> { ._tag = Result<DropSyntax, ParserError>::Error, ._Error = handler_result._Error };
            }
        }

        auto handler = handler_result._Ok;

        auto end = this->lexer.position;

        auto ret = DropSyntax(start, end, handler);

        return Result<DropSyntax, ParserError> { ._tag = Result<DropSyntax, ParserError>::Ok, ._Ok = ret };
    }

    Result<ExpressionSyntax, ParserError> parse_expression(Page* _rp, Page* _ep) {
        {
            auto node_result = this->parse_literal(_rp, _ep);
            if (node_result._tag == Result<LiteralSyntax, ParserError>::Error)
            {
                if (node_result._Error._tag == ParserError::InvalidSyntax)
                    return Result<ExpressionSyntax, ParserError> { ._tag = Result<ExpressionSyntax, ParserError>::Error, ._Error = node_result._Error };
            }
            else
            {
                auto node = node_result._Ok;
                return Result<ExpressionSyntax, ParserError> { ._tag = Result<ExpressionSyntax, ParserError>::Ok, ._Ok = 
                    ExpressionSyntax(LiteralSyntax(node))
                };
            }
        }
        {
            auto node_result = this->parse_type(_rp, _ep);
            if (node_result._tag == Result<TypeSyntax, ParserError>::Error)
            {
                if (node_result._Error._tag == ParserError::InvalidSyntax)
                    return Result<ExpressionSyntax, ParserError> { ._tag = Result<ExpressionSyntax, ParserError>::Error, ._Error = node_result._Error };
            }
            else
            {
                auto node = node_result._Ok;
                return Result<ExpressionSyntax, ParserError> { ._tag = Result<ExpressionSyntax, ParserError>::Ok, ._Ok = 
                    ExpressionSyntax(TypeSyntax(node))
                };
            }
        }
        {
            auto node_result = this->parse_object(_rp, _ep);
            if (node_result._tag == Result<ObjectSyntax, ParserError>::Error)
            {
                if (node_result._Error._tag == ParserError::InvalidSyntax)
                    return Result<ExpressionSyntax, ParserError> { ._tag = Result<ExpressionSyntax, ParserError>::Error, ._Error = node_result._Error };
            }
            else
            {
                auto node = node_result._Ok;
                return Result<ExpressionSyntax, ParserError> { ._tag = Result<ExpressionSyntax, ParserError>::Ok, ._Ok = 
                    ExpressionSyntax(ObjectSyntax(node))
                };
            }
        }
        {
            auto node_result = this->parse_vector(_rp, _ep);
            if (node_result._tag == Result<VectorSyntax, ParserError>::Error)
            {
                if (node_result._Error._tag == ParserError::InvalidSyntax)
                    return Result<ExpressionSyntax, ParserError> { ._tag = Result<ExpressionSyntax, ParserError>::Error, ._Error = node_result._Error };
            }
            else
            {
                auto node = node_result._Ok;
                return Result<ExpressionSyntax, ParserError> { ._tag = Result<ExpressionSyntax, ParserError>::Ok, ._Ok = 
                    ExpressionSyntax(VectorSyntax(node))
                };
            }
        }
        {
            auto node_result = this->parse_block(_rp, _ep);
            if (node_result._tag == Result<BlockSyntax, ParserError>::Error)
            {
                if (node_result._Error._tag == ParserError::InvalidSyntax)
                    return Result<ExpressionSyntax, ParserError> { ._tag = Result<ExpressionSyntax, ParserError>::Error, ._Error = node_result._Error };
            }
            else
            {
                auto node = node_result._Ok;
                return Result<ExpressionSyntax, ParserError> { ._tag = Result<ExpressionSyntax, ParserError>::Ok, ._Ok = 
                    ExpressionSyntax(BlockSyntax(node))
                };
            }
        }
        {
            auto node_result = this->parse_if(_rp, _ep);
            if (node_result._tag == Result<IfSyntax, ParserError>::Error)
            {
                if (node_result._Error._tag == ParserError::InvalidSyntax)
                    return Result<ExpressionSyntax, ParserError> { ._tag = Result<ExpressionSyntax, ParserError>::Error, ._Error = node_result._Error };
            }
            else
            {
                auto node = node_result._Ok;
                return Result<ExpressionSyntax, ParserError> { ._tag = Result<ExpressionSyntax, ParserError>::Ok, ._Ok = 
                    ExpressionSyntax(IfSyntax(node))
                };
            }
        }
        {
            auto node_result = this->parse_match(_rp, _ep);
            if (node_result._tag == Result<MatchSyntax, ParserError>::Error)
            {
                if (node_result._Error._tag == ParserError::InvalidSyntax)
                    return Result<ExpressionSyntax, ParserError> { ._tag = Result<ExpressionSyntax, ParserError>::Error, ._Error = node_result._Error };
            }
            else
            {
                auto node = node_result._Ok;
                return Result<ExpressionSyntax, ParserError> { ._tag = Result<ExpressionSyntax, ParserError>::Ok, ._Ok = 
                    ExpressionSyntax(MatchSyntax(node))
                };
            }
        }
        {
            auto node_result = this->parse_lambda(_rp, _ep);
            if (node_result._tag == Result<LambdaSyntax, ParserError>::Error)
            {
                if (node_result._Error._tag == ParserError::InvalidSyntax)
                    return Result<ExpressionSyntax, ParserError> { ._tag = Result<ExpressionSyntax, ParserError>::Error, ._Error = node_result._Error };
            }
            else
            {
                auto node = node_result._Ok;
                return Result<ExpressionSyntax, ParserError> { ._tag = Result<ExpressionSyntax, ParserError>::Ok, ._Ok = 
                    ExpressionSyntax(LambdaSyntax(node))
                };
            }
        }
        {
            auto node_result = this->parse_for(_rp, _ep);
            if (node_result._tag == Result<ForSyntax, ParserError>::Error)
            {
                if (node_result._Error._tag == ParserError::InvalidSyntax)
                    return Result<ExpressionSyntax, ParserError> { ._tag = Result<ExpressionSyntax, ParserError>::Error, ._Error = node_result._Error };
            }
            else
            {
                auto node = node_result._Ok;
                return Result<ExpressionSyntax, ParserError> { ._tag = Result<ExpressionSyntax, ParserError>::Ok, ._Ok = 
                    ExpressionSyntax(ForSyntax(node))
                };
            }
        }
        {
            auto node_result = this->parse_while(_rp, _ep);
            if (node_result._tag == Result<WhileSyntax, ParserError>::Error)
            {
                if (node_result._Error._tag == ParserError::InvalidSyntax)
                    return Result<ExpressionSyntax, ParserError> { ._tag = Result<ExpressionSyntax, ParserError>::Error, ._Error = node_result._Error };
            }
            else
            {
                auto node = node_result._Ok;
                return Result<ExpressionSyntax, ParserError> { ._tag = Result<ExpressionSyntax, ParserError>::Ok, ._Ok = 
                    ExpressionSyntax(WhileSyntax(node))
                };
            }
        }
        {
            auto node_result = this->parse_repeat(_rp, _ep);
            if (node_result._tag == Result<RepeatSyntax, ParserError>::Error)
            {
                if (node_result._Error._tag == ParserError::InvalidSyntax)
                    return Result<ExpressionSyntax, ParserError> { ._tag = Result<ExpressionSyntax, ParserError>::Error, ._Error = node_result._Error };
            }
            else
            {
                auto node = node_result._Ok;
                return Result<ExpressionSyntax, ParserError> { ._tag = Result<ExpressionSyntax, ParserError>::Ok, ._Ok = 
                    ExpressionSyntax(RepeatSyntax(node))
                };
            }
        }
        {
            auto node_result = this->parse_sizeof(_rp, _ep);
            if (node_result._tag == Result<SizeOfSyntax, ParserError>::Error)
            {
                if (node_result._Error._tag == ParserError::InvalidSyntax)
                    return Result<ExpressionSyntax, ParserError> { ._tag = Result<ExpressionSyntax, ParserError>::Error, ._Error = node_result._Error };
            }
            else
            {
                auto node = node_result._Ok;
                return Result<ExpressionSyntax, ParserError> { ._tag = Result<ExpressionSyntax, ParserError>::Ok, ._Ok = 
                    ExpressionSyntax(SizeOfSyntax(node))
                };
            }
        }
        return Result<ExpressionSyntax, ParserError> { ._tag = Result<ExpressionSyntax, ParserError>::Error, ._Error = ParserError(OtherSyntax()) };
    }

    Result<LiteralSyntax, ParserError> parse_literal(Page* _rp, Page* _ep) {
        auto start = this->lexer.previous_position;

        auto literal_start = this->lexer.previous_position;
        auto literal_result = this->parse_literal_token(_rp);
        if (literal_result._tag == Result<Literal, ParserError>::Error)
        {
            if (literal_result._Error._tag == ParserError::OtherSyntax)
               return Result<LiteralSyntax, ParserError> { ._tag = Result<LiteralSyntax, ParserError>::Error, ._Error = ParserError(OtherSyntax()) };
        }

        auto literal = literal_result._Ok;

        auto end = this->lexer.position;

        auto ret = LiteralSyntax(start, end, literal);

        return Result<LiteralSyntax, ParserError> { ._tag = Result<LiteralSyntax, ParserError>::Ok, ._Ok = ret };
    }

    Result<ObjectSyntax, ParserError> parse_object(Page* _rp, Page* _ep) {
        auto start = this->lexer.previous_position;

        auto start_left_paren_1 = this->lexer.previous_position;
        auto success_left_paren_1 = this->lexer.parse_punctuation('(');
        if (!success_left_paren_1) {
            return Result<ObjectSyntax, ParserError> { ._tag = Result<ObjectSyntax, ParserError>::Error, ._Error = ParserError(OtherSyntax()) };
        }

        auto components_start = this->lexer.position;
        auto components_result = this->parse_component_list(_rp, _ep);
        if (components_result._tag == Result<Vector<ComponentSyntax>, ParserError>::Error)
        {
            switch (components_result._Error._tag) {
                case ParserError::OtherSyntax:
                    break;
                case ParserError::InvalidSyntax:
                    return Result<ObjectSyntax, ParserError> { ._tag = Result<ObjectSyntax, ParserError>::Error, ._Error = components_result._Error };
            }
        }

        auto components = components_result._tag == Result<Vector<ComponentSyntax>, ParserError>::Error ? nullptr : components_result._Ok;

        auto start_right_paren_3 = this->lexer.previous_position;
        auto success_right_paren_3 = this->lexer.parse_punctuation(')');
        if (!success_right_paren_3) {
            return Result<ObjectSyntax, ParserError> { ._tag = Result<ObjectSyntax, ParserError>::Error, ._Error = ParserError(InvalidSyntax(start_right_paren_3, lexer.position, String(_ep, ")"))) };        }

        auto end = this->lexer.position;

        auto ret = ObjectSyntax(start, end, components);

        return Result<ObjectSyntax, ParserError> { ._tag = Result<ObjectSyntax, ParserError>::Ok, ._Ok = ret };
    }

    Result<Vector<ComponentSyntax>*, ParserError> parse_component_list(Page* _rp, Page* _ep) {
        Region _r;
        List<ComponentSyntax>& list = *new(alignof(List<ComponentSyntax>), _r.get_page()) List<ComponentSyntax>();;
        while(true) {
            auto node_result = this->parse_component(_rp, _ep);
            if ((node_result._tag == Result<ComponentSyntax, ParserError>::Error) && (node_result._Error._tag == ParserError::InvalidSyntax))
                return Result<Vector<ComponentSyntax>*, ParserError> { ._tag = Result<Vector<ComponentSyntax>*, ParserError>::Error, ._Error = node_result._Error };
            if (node_result._tag == Result<ComponentSyntax, ParserError>::Ok) {
                auto node = node_result._Ok;
                list.add(node);
            } else {
                if ((list.count() == 0) && (node_result._tag == Result<ComponentSyntax, ParserError>::Error) && (node_result._Error._tag == ParserError::OtherSyntax))
                    return Result<Vector<ComponentSyntax>*, ParserError> { ._tag = Result<Vector<ComponentSyntax>*, ParserError>::Error, ._Error = node_result._Error };
                break;
            }
        }

        if (list.count() == 0)
            return Result<Vector<ComponentSyntax>*, ParserError> { ._tag = Result<Vector<ComponentSyntax>*, ParserError>::Ok, ._Ok = nullptr };
        
        return Result<Vector<ComponentSyntax>*, ParserError> {
            ._tag = Result<Vector<ComponentSyntax>*, ParserError>::Ok,
            ._Ok = new(alignof(Vector<ComponentSyntax>), _rp) Vector<ComponentSyntax>(_rp, list) };
    }

    Result<ComponentSyntax, ParserError> parse_component(Page* _rp, Page* _ep) {
        auto start = this->lexer.previous_position;

        auto operands_start = this->lexer.position;
        auto operands_result = this->parse_operand_list(_rp, _ep);
        if (operands_result._tag == Result<Vector<OperandSyntax>, ParserError>::Error)
        {
            return Result<ComponentSyntax, ParserError> { ._tag = Result<ComponentSyntax, ParserError>::Error, ._Error = operands_result._Error };
        }

        auto operands = operands_result._Ok;

        auto attributes_start = this->lexer.position;
        auto attributes_result = this->parse_attribute_list(_rp, _ep);
        if (attributes_result._tag == Result<Vector<AttributeSyntax>, ParserError>::Error)
        {
            switch (attributes_result._Error._tag) {
                case ParserError::OtherSyntax:
                    break;
                case ParserError::InvalidSyntax:
                    return Result<ComponentSyntax, ParserError> { ._tag = Result<ComponentSyntax, ParserError>::Error, ._Error = attributes_result._Error };
            }
        }

        auto attributes = attributes_result._tag == Result<Vector<AttributeSyntax>, ParserError>::Error ? nullptr : attributes_result._Ok;

        auto value_start = this->lexer.position;
        auto value_result = this->parse_value(_rp, _ep);
        if (value_result._tag == Result<ValueSyntax, ParserError>::Error)
        {
            switch (value_result._Error._tag) {
                case ParserError::OtherSyntax:
                    break;
                case ParserError::InvalidSyntax:
                    return Result<ComponentSyntax, ParserError> { ._tag = Result<ComponentSyntax, ParserError>::Error, ._Error = value_result._Error };
            }
        }

        ValueSyntax* value = value_result._tag == Result<ValueSyntax, ParserError>::Error ? nullptr : new(alignof(ValueSyntax), _rp) ValueSyntax(value_result._Ok);

        auto start_comma_4 = this->lexer.previous_position;
        auto success_comma_4 = this->lexer.parse_punctuation(',');
        if (!success_comma_4) {
        }

        auto end = this->lexer.position;

        auto ret = ComponentSyntax(start, end, operands, attributes, value);

        return Result<ComponentSyntax, ParserError> { ._tag = Result<ComponentSyntax, ParserError>::Ok, ._Ok = ret };
    }

    Result<ValueSyntax, ParserError> parse_value(Page* _rp, Page* _ep) {
        auto start = this->lexer.previous_position;

        auto start_colon_1 = this->lexer.previous_position;
        auto success_colon_1 = this->lexer.parse_colon(_rp);
        if (!success_colon_1) {
            return Result<ValueSyntax, ParserError> { ._tag = Result<ValueSyntax, ParserError>::Error, ._Error = ParserError(OtherSyntax()) };
        }

        auto value_start = this->lexer.position;
        auto value_result = this->parse_operand_list(_rp, _ep);
        if (value_result._tag == Result<Vector<OperandSyntax>, ParserError>::Error)
        {
            switch (value_result._Error._tag) {
                case ParserError::OtherSyntax:
                    return Result<ValueSyntax, ParserError> { ._tag = Result<ValueSyntax, ParserError>::Error, ._Error = ParserError(InvalidSyntax(value_start, lexer.position, String(_ep, "a valid Operand syntax"))) };
                case ParserError::InvalidSyntax:
                    return Result<ValueSyntax, ParserError> { ._tag = Result<ValueSyntax, ParserError>::Error, ._Error = value_result._Error };
            }
        }

        auto value = value_result._Ok;

        auto attributes_start = this->lexer.position;
        auto attributes_result = this->parse_attribute_list(_rp, _ep);
        if (attributes_result._tag == Result<Vector<AttributeSyntax>, ParserError>::Error)
        {
            switch (attributes_result._Error._tag) {
                case ParserError::OtherSyntax:
                    break;
                case ParserError::InvalidSyntax:
                    return Result<ValueSyntax, ParserError> { ._tag = Result<ValueSyntax, ParserError>::Error, ._Error = attributes_result._Error };
            }
        }

        auto attributes = attributes_result._tag == Result<Vector<AttributeSyntax>, ParserError>::Error ? nullptr : attributes_result._Ok;

        auto end = this->lexer.position;

        auto ret = ValueSyntax(start, end, value, attributes);

        return Result<ValueSyntax, ParserError> { ._tag = Result<ValueSyntax, ParserError>::Ok, ._Ok = ret };
    }

    Result<VectorSyntax, ParserError> parse_vector(Page* _rp, Page* _ep) {
        auto start = this->lexer.previous_position;

        auto start_left_bracket_1 = this->lexer.previous_position;
        auto success_left_bracket_1 = this->lexer.parse_punctuation('[');
        if (!success_left_bracket_1) {
            return Result<VectorSyntax, ParserError> { ._tag = Result<VectorSyntax, ParserError>::Error, ._Error = ParserError(OtherSyntax()) };
        }

        auto elements_start = this->lexer.position;
        auto elements_result = this->parse_element_list(_rp, _ep);
        if (elements_result._tag == Result<Vector<ElementSyntax>, ParserError>::Error)
        {
            switch (elements_result._Error._tag) {
                case ParserError::OtherSyntax:
                    return Result<VectorSyntax, ParserError> { ._tag = Result<VectorSyntax, ParserError>::Error, ._Error = ParserError(InvalidSyntax(elements_start, lexer.position, String(_ep, "a valid Element syntax"))) };
                case ParserError::InvalidSyntax:
                    return Result<VectorSyntax, ParserError> { ._tag = Result<VectorSyntax, ParserError>::Error, ._Error = elements_result._Error };
            }
        }

        auto elements = elements_result._Ok;

        auto start_right_bracket_3 = this->lexer.previous_position;
        auto success_right_bracket_3 = this->lexer.parse_punctuation(']');
        if (!success_right_bracket_3) {
            return Result<VectorSyntax, ParserError> { ._tag = Result<VectorSyntax, ParserError>::Error, ._Error = ParserError(InvalidSyntax(start_right_bracket_3, lexer.position, String(_ep, "]"))) };        }

        auto lifetime_start = this->lexer.position;
        auto lifetime_result = this->parse_lifetime(_rp, _ep);
        if (lifetime_result._tag == Result<LifetimeSyntax, ParserError>::Error)
        {
            switch (lifetime_result._Error._tag) {
                case ParserError::OtherSyntax:
                    break;
                case ParserError::InvalidSyntax:
                    return Result<VectorSyntax, ParserError> { ._tag = Result<VectorSyntax, ParserError>::Error, ._Error = lifetime_result._Error };
            }
        }

        LifetimeSyntax* lifetime = lifetime_result._tag == Result<LifetimeSyntax, ParserError>::Error ? nullptr : new(alignof(LifetimeSyntax), _rp) LifetimeSyntax(lifetime_result._Ok);

        auto end = this->lexer.position;

        auto ret = VectorSyntax(start, end, elements, lifetime);

        return Result<VectorSyntax, ParserError> { ._tag = Result<VectorSyntax, ParserError>::Ok, ._Ok = ret };
    }

    Result<Vector<ElementSyntax>*, ParserError> parse_element_list(Page* _rp, Page* _ep) {
        Region _r;
        List<ElementSyntax>& list = *new(alignof(List<ElementSyntax>), _r.get_page()) List<ElementSyntax>();;
        while(true) {
            auto node_result = this->parse_element(_rp, _ep);
            if ((node_result._tag == Result<ElementSyntax, ParserError>::Error) && (node_result._Error._tag == ParserError::InvalidSyntax))
                return Result<Vector<ElementSyntax>*, ParserError> { ._tag = Result<Vector<ElementSyntax>*, ParserError>::Error, ._Error = node_result._Error };
            if (node_result._tag == Result<ElementSyntax, ParserError>::Ok) {
                auto node = node_result._Ok;
                list.add(node);
            } else {
                if ((list.count() == 0) && (node_result._tag == Result<ElementSyntax, ParserError>::Error) && (node_result._Error._tag == ParserError::OtherSyntax))
                    return Result<Vector<ElementSyntax>*, ParserError> { ._tag = Result<Vector<ElementSyntax>*, ParserError>::Error, ._Error = node_result._Error };
                break;
            }
        }

        if (list.count() == 0)
            return Result<Vector<ElementSyntax>*, ParserError> { ._tag = Result<Vector<ElementSyntax>*, ParserError>::Ok, ._Ok = nullptr };
        
        return Result<Vector<ElementSyntax>*, ParserError> {
            ._tag = Result<Vector<ElementSyntax>*, ParserError>::Ok,
            ._Ok = new(alignof(Vector<ElementSyntax>), _rp) Vector<ElementSyntax>(_rp, list) };
    }

    Result<ElementSyntax, ParserError> parse_element(Page* _rp, Page* _ep) {
        auto start = this->lexer.previous_position;

        auto operation_start = this->lexer.position;
        auto operation_result = this->parse_operand_list(_rp, _ep);
        if (operation_result._tag == Result<Vector<OperandSyntax>, ParserError>::Error)
        {
            return Result<ElementSyntax, ParserError> { ._tag = Result<ElementSyntax, ParserError>::Error, ._Error = operation_result._Error };
        }

        auto operation = operation_result._Ok;

        auto attributes_start = this->lexer.position;
        auto attributes_result = this->parse_attribute_list(_rp, _ep);
        if (attributes_result._tag == Result<Vector<AttributeSyntax>, ParserError>::Error)
        {
            switch (attributes_result._Error._tag) {
                case ParserError::OtherSyntax:
                    break;
                case ParserError::InvalidSyntax:
                    return Result<ElementSyntax, ParserError> { ._tag = Result<ElementSyntax, ParserError>::Error, ._Error = attributes_result._Error };
            }
        }

        auto attributes = attributes_result._tag == Result<Vector<AttributeSyntax>, ParserError>::Error ? nullptr : attributes_result._Ok;

        auto start_comma_3 = this->lexer.previous_position;
        auto success_comma_3 = this->lexer.parse_punctuation(',');
        if (!success_comma_3) {
        }

        auto end = this->lexer.position;

        auto ret = ElementSyntax(start, end, operation, attributes);

        return Result<ElementSyntax, ParserError> { ._tag = Result<ElementSyntax, ParserError>::Ok, ._Ok = ret };
    }

    Result<BlockSyntax, ParserError> parse_block(Page* _rp, Page* _ep) {
        auto start = this->lexer.previous_position;

        auto start_left_curly_1 = this->lexer.previous_position;
        auto success_left_curly_1 = this->lexer.parse_punctuation('{');
        if (!success_left_curly_1) {
            return Result<BlockSyntax, ParserError> { ._tag = Result<BlockSyntax, ParserError>::Error, ._Error = ParserError(OtherSyntax()) };
        }

        auto uses_start = this->lexer.position;
        auto uses_result = this->parse_use_list(_rp, _ep);
        if (uses_result._tag == Result<Vector<UseSyntax>, ParserError>::Error)
        {
            switch (uses_result._Error._tag) {
                case ParserError::OtherSyntax:
                    break;
                case ParserError::InvalidSyntax:
                    return Result<BlockSyntax, ParserError> { ._tag = Result<BlockSyntax, ParserError>::Error, ._Error = uses_result._Error };
            }
        }

        auto uses = uses_result._tag == Result<Vector<UseSyntax>, ParserError>::Error ? nullptr : uses_result._Ok;

        auto statements_start = this->lexer.position;
        auto statements_result = this->parse_statement_list(_rp, _ep);
        if (statements_result._tag == Result<Vector<StatementSyntax>, ParserError>::Error)
        {
            switch (statements_result._Error._tag) {
                case ParserError::OtherSyntax:
                    break;
                case ParserError::InvalidSyntax:
                    return Result<BlockSyntax, ParserError> { ._tag = Result<BlockSyntax, ParserError>::Error, ._Error = statements_result._Error };
            }
        }

        auto statements = statements_result._tag == Result<Vector<StatementSyntax>, ParserError>::Error ? nullptr : statements_result._Ok;

        auto start_right_curly_4 = this->lexer.previous_position;
        auto success_right_curly_4 = this->lexer.parse_punctuation('}');
        if (!success_right_curly_4) {
            return Result<BlockSyntax, ParserError> { ._tag = Result<BlockSyntax, ParserError>::Error, ._Error = ParserError(InvalidSyntax(start_right_curly_4, lexer.position, String(_ep, "}"))) };        }

        auto end = this->lexer.position;

        auto ret = BlockSyntax(start, end, uses, statements);

        return Result<BlockSyntax, ParserError> { ._tag = Result<BlockSyntax, ParserError>::Ok, ._Ok = ret };
    }

    Result<IfSyntax, ParserError> parse_if(Page* _rp, Page* _ep) {
        auto start = this->lexer.previous_position;

        auto start_if_1 = this->lexer.previous_position;
        auto success_if_1 = this->lexer.parse_keyword(_rp, *this->keywords_index[14]);
        if (!success_if_1) {
            return Result<IfSyntax, ParserError> { ._tag = Result<IfSyntax, ParserError>::Error, ._Error = ParserError(OtherSyntax()) };
        }

        auto condition_start = this->lexer.position;
        auto condition_result = this->parse_operand_list(_rp, _ep);
        if (condition_result._tag == Result<Vector<OperandSyntax>, ParserError>::Error)
        {
            switch (condition_result._Error._tag) {
                case ParserError::OtherSyntax:
                    return Result<IfSyntax, ParserError> { ._tag = Result<IfSyntax, ParserError>::Error, ._Error = ParserError(InvalidSyntax(condition_start, lexer.position, String(_ep, "a valid Operand syntax"))) };
                case ParserError::InvalidSyntax:
                    return Result<IfSyntax, ParserError> { ._tag = Result<IfSyntax, ParserError>::Error, ._Error = condition_result._Error };
            }
        }

        auto condition = condition_result._Ok;

        auto start_colon_3 = this->lexer.previous_position;
        auto success_colon_3 = this->lexer.parse_colon(_rp);
        if (!success_colon_3) {
            return Result<IfSyntax, ParserError> { ._tag = Result<IfSyntax, ParserError>::Error, ._Error = ParserError(InvalidSyntax(start_colon_3, lexer.position, String(_ep, "a colon or a line feed"))) };        }

        auto consequent_start = this->lexer.position;
        auto consequent_result = this->parse_command(_rp, _ep);
        if (consequent_result._tag == Result<CommandSyntax, ParserError>::Error)
        {
            switch (consequent_result._Error._tag) {
                case ParserError::OtherSyntax:
                    return Result<IfSyntax, ParserError> { ._tag = Result<IfSyntax, ParserError>::Error, ._Error = ParserError(InvalidSyntax(consequent_start, lexer.position, String(_ep, "a valid Command syntax"))) };
                case ParserError::InvalidSyntax:
                    return Result<IfSyntax, ParserError> { ._tag = Result<IfSyntax, ParserError>::Error, ._Error = consequent_result._Error };
            }
        }

        auto consequent = consequent_result._Ok;

        auto alternative_start = this->lexer.position;
        auto alternative_result = this->parse_else(_rp, _ep);
        if (alternative_result._tag == Result<ElseSyntax, ParserError>::Error)
        {
            switch (alternative_result._Error._tag) {
                case ParserError::OtherSyntax:
                    break;
                case ParserError::InvalidSyntax:
                    return Result<IfSyntax, ParserError> { ._tag = Result<IfSyntax, ParserError>::Error, ._Error = alternative_result._Error };
            }
        }

        ElseSyntax* alternative = alternative_result._tag == Result<ElseSyntax, ParserError>::Error ? nullptr : new(alignof(ElseSyntax), _rp) ElseSyntax(alternative_result._Ok);

        auto end = this->lexer.position;

        auto ret = IfSyntax(start, end, condition, consequent, alternative);

        return Result<IfSyntax, ParserError> { ._tag = Result<IfSyntax, ParserError>::Ok, ._Ok = ret };
    }

    Result<ElseSyntax, ParserError> parse_else(Page* _rp, Page* _ep) {
        auto start = this->lexer.previous_position;

        auto start_else_1 = this->lexer.previous_position;
        auto success_else_1 = this->lexer.parse_keyword(_rp, *this->keywords_index[9]);
        if (!success_else_1) {
            return Result<ElseSyntax, ParserError> { ._tag = Result<ElseSyntax, ParserError>::Error, ._Error = ParserError(OtherSyntax()) };
        }

        auto start_colon_2 = this->lexer.previous_position;
        auto success_colon_2 = this->lexer.parse_colon(_rp);
        if (!success_colon_2) {
        }

        auto alternative_start = this->lexer.position;
        auto alternative_result = this->parse_command(_rp, _ep);
        if (alternative_result._tag == Result<CommandSyntax, ParserError>::Error)
        {
            switch (alternative_result._Error._tag) {
                case ParserError::OtherSyntax:
                    return Result<ElseSyntax, ParserError> { ._tag = Result<ElseSyntax, ParserError>::Error, ._Error = ParserError(InvalidSyntax(alternative_start, lexer.position, String(_ep, "a valid Command syntax"))) };
                case ParserError::InvalidSyntax:
                    return Result<ElseSyntax, ParserError> { ._tag = Result<ElseSyntax, ParserError>::Error, ._Error = alternative_result._Error };
            }
        }

        auto alternative = alternative_result._Ok;

        auto end = this->lexer.position;

        auto ret = ElseSyntax(start, end, alternative);

        return Result<ElseSyntax, ParserError> { ._tag = Result<ElseSyntax, ParserError>::Ok, ._Ok = ret };
    }

    Result<Vector<StatementSyntax>*, ParserError> parse_statement_list(Page* _rp, Page* _ep) {
        Region _r;
        List<StatementSyntax>& list = *new(alignof(List<StatementSyntax>), _r.get_page()) List<StatementSyntax>();;
        while(true) {
            auto node_result = this->parse_statement(_rp, _ep);
            if ((node_result._tag == Result<StatementSyntax, ParserError>::Error) && (node_result._Error._tag == ParserError::InvalidSyntax))
                return Result<Vector<StatementSyntax>*, ParserError> { ._tag = Result<Vector<StatementSyntax>*, ParserError>::Error, ._Error = node_result._Error };
            if (node_result._tag == Result<StatementSyntax, ParserError>::Ok) {
                auto node = node_result._Ok;
                list.add(node);
            } else {
                if ((list.count() == 0) && (node_result._tag == Result<StatementSyntax, ParserError>::Error) && (node_result._Error._tag == ParserError::OtherSyntax))
                    return Result<Vector<StatementSyntax>*, ParserError> { ._tag = Result<Vector<StatementSyntax>*, ParserError>::Error, ._Error = node_result._Error };
                break;
            }
        }

        if (list.count() == 0)
            return Result<Vector<StatementSyntax>*, ParserError> { ._tag = Result<Vector<StatementSyntax>*, ParserError>::Ok, ._Ok = nullptr };
        
        return Result<Vector<StatementSyntax>*, ParserError> {
            ._tag = Result<Vector<StatementSyntax>*, ParserError>::Ok,
            ._Ok = new(alignof(Vector<StatementSyntax>), _rp) Vector<StatementSyntax>(_rp, list) };
    }

    Result<StatementSyntax, ParserError> parse_statement(Page* _rp, Page* _ep) {
        auto start = this->lexer.previous_position;

        auto command_start = this->lexer.position;
        auto command_result = this->parse_command(_rp, _ep);
        if (command_result._tag == Result<CommandSyntax, ParserError>::Error)
        {
            return Result<StatementSyntax, ParserError> { ._tag = Result<StatementSyntax, ParserError>::Error, ._Error = command_result._Error };
        }

        auto command = command_result._Ok;

        auto start_colon_2 = this->lexer.previous_position;
        auto success_colon_2 = this->lexer.parse_colon(_rp);
        if (!success_colon_2) {
        }

        auto end = this->lexer.position;

        auto ret = StatementSyntax(start, end, command);

        return Result<StatementSyntax, ParserError> { ._tag = Result<StatementSyntax, ParserError>::Ok, ._Ok = ret };
    }

    Result<CommandSyntax, ParserError> parse_command(Page* _rp, Page* _ep) {
        {
            auto node_result = this->parse_operation(_rp, _ep);
            if (node_result._tag == Result<OperationSyntax, ParserError>::Error)
            {
                if (node_result._Error._tag == ParserError::InvalidSyntax)
                    return Result<CommandSyntax, ParserError> { ._tag = Result<CommandSyntax, ParserError>::Error, ._Error = node_result._Error };
            }
            else
            {
                auto node = node_result._Ok;
                return Result<CommandSyntax, ParserError> { ._tag = Result<CommandSyntax, ParserError>::Ok, ._Ok = 
                    CommandSyntax(OperationSyntax(node))
                };
            }
        }
        {
            auto node_result = this->parse_let(_rp, _ep);
            if (node_result._tag == Result<LetSyntax, ParserError>::Error)
            {
                if (node_result._Error._tag == ParserError::InvalidSyntax)
                    return Result<CommandSyntax, ParserError> { ._tag = Result<CommandSyntax, ParserError>::Error, ._Error = node_result._Error };
            }
            else
            {
                auto node = node_result._Ok;
                return Result<CommandSyntax, ParserError> { ._tag = Result<CommandSyntax, ParserError>::Ok, ._Ok = 
                    CommandSyntax(LetSyntax(node))
                };
            }
        }
        {
            auto node_result = this->parse_var(_rp, _ep);
            if (node_result._tag == Result<VarSyntax, ParserError>::Error)
            {
                if (node_result._Error._tag == ParserError::InvalidSyntax)
                    return Result<CommandSyntax, ParserError> { ._tag = Result<CommandSyntax, ParserError>::Error, ._Error = node_result._Error };
            }
            else
            {
                auto node = node_result._Ok;
                return Result<CommandSyntax, ParserError> { ._tag = Result<CommandSyntax, ParserError>::Ok, ._Ok = 
                    CommandSyntax(VarSyntax(node))
                };
            }
        }
        {
            auto node_result = this->parse_mutable(_rp, _ep);
            if (node_result._tag == Result<MutableSyntax, ParserError>::Error)
            {
                if (node_result._Error._tag == ParserError::InvalidSyntax)
                    return Result<CommandSyntax, ParserError> { ._tag = Result<CommandSyntax, ParserError>::Error, ._Error = node_result._Error };
            }
            else
            {
                auto node = node_result._Ok;
                return Result<CommandSyntax, ParserError> { ._tag = Result<CommandSyntax, ParserError>::Ok, ._Ok = 
                    CommandSyntax(MutableSyntax(node))
                };
            }
        }
        {
            auto node_result = this->parse_set(_rp, _ep);
            if (node_result._tag == Result<SetSyntax, ParserError>::Error)
            {
                if (node_result._Error._tag == ParserError::InvalidSyntax)
                    return Result<CommandSyntax, ParserError> { ._tag = Result<CommandSyntax, ParserError>::Error, ._Error = node_result._Error };
            }
            else
            {
                auto node = node_result._Ok;
                return Result<CommandSyntax, ParserError> { ._tag = Result<CommandSyntax, ParserError>::Ok, ._Ok = 
                    CommandSyntax(SetSyntax(node))
                };
            }
        }
        {
            auto node_result = this->parse_continue(_rp, _ep);
            if (node_result._tag == Result<ContinueSyntax, ParserError>::Error)
            {
                if (node_result._Error._tag == ParserError::InvalidSyntax)
                    return Result<CommandSyntax, ParserError> { ._tag = Result<CommandSyntax, ParserError>::Error, ._Error = node_result._Error };
            }
            else
            {
                auto node = node_result._Ok;
                return Result<CommandSyntax, ParserError> { ._tag = Result<CommandSyntax, ParserError>::Ok, ._Ok = 
                    CommandSyntax(ContinueSyntax(node))
                };
            }
        }
        {
            auto node_result = this->parse_break(_rp, _ep);
            if (node_result._tag == Result<BreakSyntax, ParserError>::Error)
            {
                if (node_result._Error._tag == ParserError::InvalidSyntax)
                    return Result<CommandSyntax, ParserError> { ._tag = Result<CommandSyntax, ParserError>::Error, ._Error = node_result._Error };
            }
            else
            {
                auto node = node_result._Ok;
                return Result<CommandSyntax, ParserError> { ._tag = Result<CommandSyntax, ParserError>::Ok, ._Ok = 
                    CommandSyntax(BreakSyntax(node))
                };
            }
        }
        {
            auto node_result = this->parse_return(_rp, _ep);
            if (node_result._tag == Result<ReturnSyntax, ParserError>::Error)
            {
                if (node_result._Error._tag == ParserError::InvalidSyntax)
                    return Result<CommandSyntax, ParserError> { ._tag = Result<CommandSyntax, ParserError>::Error, ._Error = node_result._Error };
            }
            else
            {
                auto node = node_result._Ok;
                return Result<CommandSyntax, ParserError> { ._tag = Result<CommandSyntax, ParserError>::Ok, ._Ok = 
                    CommandSyntax(ReturnSyntax(node))
                };
            }
        }
        {
            auto node_result = this->parse_throw(_rp, _ep);
            if (node_result._tag == Result<ThrowSyntax, ParserError>::Error)
            {
                if (node_result._Error._tag == ParserError::InvalidSyntax)
                    return Result<CommandSyntax, ParserError> { ._tag = Result<CommandSyntax, ParserError>::Error, ._Error = node_result._Error };
            }
            else
            {
                auto node = node_result._Ok;
                return Result<CommandSyntax, ParserError> { ._tag = Result<CommandSyntax, ParserError>::Ok, ._Ok = 
                    CommandSyntax(ThrowSyntax(node))
                };
            }
        }
        return Result<CommandSyntax, ParserError> { ._tag = Result<CommandSyntax, ParserError>::Error, ._Error = ParserError(OtherSyntax()) };
    }

    Result<WhileSyntax, ParserError> parse_while(Page* _rp, Page* _ep) {
        auto start = this->lexer.previous_position;

        auto start_while_1 = this->lexer.previous_position;
        auto success_while_1 = this->lexer.parse_keyword(_rp, *this->keywords_index[42]);
        if (!success_while_1) {
            return Result<WhileSyntax, ParserError> { ._tag = Result<WhileSyntax, ParserError>::Error, ._Error = ParserError(OtherSyntax()) };
        }

        auto condition_start = this->lexer.position;
        auto condition_result = this->parse_condition(_rp, _ep);
        if (condition_result._tag == Result<ConditionSyntax, ParserError>::Error)
        {
            switch (condition_result._Error._tag) {
                case ParserError::OtherSyntax:
                    return Result<WhileSyntax, ParserError> { ._tag = Result<WhileSyntax, ParserError>::Error, ._Error = ParserError(InvalidSyntax(condition_start, lexer.position, String(_ep, "a valid Condition syntax"))) };
                case ParserError::InvalidSyntax:
                    return Result<WhileSyntax, ParserError> { ._tag = Result<WhileSyntax, ParserError>::Error, ._Error = condition_result._Error };
            }
        }

        auto condition = condition_result._Ok;

        auto start_colon_3 = this->lexer.previous_position;
        auto success_colon_3 = this->lexer.parse_colon(_rp);
        if (!success_colon_3) {
            return Result<WhileSyntax, ParserError> { ._tag = Result<WhileSyntax, ParserError>::Error, ._Error = ParserError(InvalidSyntax(start_colon_3, lexer.position, String(_ep, "a colon or a line feed"))) };        }

        auto name_start = this->lexer.position;
        auto name_result = this->parse_label(_rp, _ep);
        if (name_result._tag == Result<LabelSyntax, ParserError>::Error)
        {
            switch (name_result._Error._tag) {
                case ParserError::OtherSyntax:
                    break;
                case ParserError::InvalidSyntax:
                    return Result<WhileSyntax, ParserError> { ._tag = Result<WhileSyntax, ParserError>::Error, ._Error = name_result._Error };
            }
        }

        LabelSyntax* name = name_result._tag == Result<LabelSyntax, ParserError>::Error ? nullptr : new(alignof(LabelSyntax), _rp) LabelSyntax(name_result._Ok);

        auto action_start = this->lexer.position;
        auto action_result = this->parse_action(_rp, _ep);
        if (action_result._tag == Result<ActionSyntax, ParserError>::Error)
        {
            switch (action_result._Error._tag) {
                case ParserError::OtherSyntax:
                    return Result<WhileSyntax, ParserError> { ._tag = Result<WhileSyntax, ParserError>::Error, ._Error = ParserError(InvalidSyntax(action_start, lexer.position, String(_ep, "a valid Action syntax"))) };
                case ParserError::InvalidSyntax:
                    return Result<WhileSyntax, ParserError> { ._tag = Result<WhileSyntax, ParserError>::Error, ._Error = action_result._Error };
            }
        }

        auto action = action_result._Ok;

        auto end = this->lexer.position;

        auto ret = WhileSyntax(start, end, condition, name, action);

        return Result<WhileSyntax, ParserError> { ._tag = Result<WhileSyntax, ParserError>::Ok, ._Ok = ret };
    }

    Result<ConditionSyntax, ParserError> parse_condition(Page* _rp, Page* _ep) {
        {
            auto node_result = this->parse_operation(_rp, _ep);
            if (node_result._tag == Result<OperationSyntax, ParserError>::Error)
            {
                if (node_result._Error._tag == ParserError::InvalidSyntax)
                    return Result<ConditionSyntax, ParserError> { ._tag = Result<ConditionSyntax, ParserError>::Error, ._Error = node_result._Error };
            }
            else
            {
                auto node = node_result._Ok;
                return Result<ConditionSyntax, ParserError> { ._tag = Result<ConditionSyntax, ParserError>::Ok, ._Ok = 
                    ConditionSyntax(OperationSyntax(node))
                };
            }
        }
        {
            auto node_result = this->parse_let(_rp, _ep);
            if (node_result._tag == Result<LetSyntax, ParserError>::Error)
            {
                if (node_result._Error._tag == ParserError::InvalidSyntax)
                    return Result<ConditionSyntax, ParserError> { ._tag = Result<ConditionSyntax, ParserError>::Error, ._Error = node_result._Error };
            }
            else
            {
                auto node = node_result._Ok;
                return Result<ConditionSyntax, ParserError> { ._tag = Result<ConditionSyntax, ParserError>::Ok, ._Ok = 
                    ConditionSyntax(LetSyntax(node))
                };
            }
        }
        return Result<ConditionSyntax, ParserError> { ._tag = Result<ConditionSyntax, ParserError>::Error, ._Error = ParserError(OtherSyntax()) };
    }

    Result<LetSyntax, ParserError> parse_let(Page* _rp, Page* _ep) {
        auto start = this->lexer.previous_position;

        auto start_let_1 = this->lexer.previous_position;
        auto success_let_1 = this->lexer.parse_keyword(_rp, *this->keywords_index[22]);
        if (!success_let_1) {
            return Result<LetSyntax, ParserError> { ._tag = Result<LetSyntax, ParserError>::Error, ._Error = ParserError(OtherSyntax()) };
        }

        auto binding_start = this->lexer.position;
        auto binding_result = this->parse_binding(_rp, _ep);
        if (binding_result._tag == Result<BindingSyntax, ParserError>::Error)
        {
            switch (binding_result._Error._tag) {
                case ParserError::OtherSyntax:
                    return Result<LetSyntax, ParserError> { ._tag = Result<LetSyntax, ParserError>::Error, ._Error = ParserError(InvalidSyntax(binding_start, lexer.position, String(_ep, "a valid Binding syntax"))) };
                case ParserError::InvalidSyntax:
                    return Result<LetSyntax, ParserError> { ._tag = Result<LetSyntax, ParserError>::Error, ._Error = binding_result._Error };
            }
        }

        auto binding = binding_result._Ok;

        auto end = this->lexer.position;

        auto ret = LetSyntax(start, end, binding);

        return Result<LetSyntax, ParserError> { ._tag = Result<LetSyntax, ParserError>::Ok, ._Ok = ret };
    }

    Result<VarSyntax, ParserError> parse_var(Page* _rp, Page* _ep) {
        auto start = this->lexer.previous_position;

        auto start_var_1 = this->lexer.previous_position;
        auto success_var_1 = this->lexer.parse_keyword(_rp, *this->keywords_index[41]);
        if (!success_var_1) {
            return Result<VarSyntax, ParserError> { ._tag = Result<VarSyntax, ParserError>::Error, ._Error = ParserError(OtherSyntax()) };
        }

        auto binding_start = this->lexer.position;
        auto binding_result = this->parse_binding(_rp, _ep);
        if (binding_result._tag == Result<BindingSyntax, ParserError>::Error)
        {
            switch (binding_result._Error._tag) {
                case ParserError::OtherSyntax:
                    return Result<VarSyntax, ParserError> { ._tag = Result<VarSyntax, ParserError>::Error, ._Error = ParserError(InvalidSyntax(binding_start, lexer.position, String(_ep, "a valid Binding syntax"))) };
                case ParserError::InvalidSyntax:
                    return Result<VarSyntax, ParserError> { ._tag = Result<VarSyntax, ParserError>::Error, ._Error = binding_result._Error };
            }
        }

        auto binding = binding_result._Ok;

        auto end = this->lexer.position;

        auto ret = VarSyntax(start, end, binding);

        return Result<VarSyntax, ParserError> { ._tag = Result<VarSyntax, ParserError>::Ok, ._Ok = ret };
    }

    Result<MutableSyntax, ParserError> parse_mutable(Page* _rp, Page* _ep) {
        auto start = this->lexer.previous_position;

        auto start_mutable_1 = this->lexer.previous_position;
        auto success_mutable_1 = this->lexer.parse_keyword(_rp, *this->keywords_index[27]);
        if (!success_mutable_1) {
            return Result<MutableSyntax, ParserError> { ._tag = Result<MutableSyntax, ParserError>::Error, ._Error = ParserError(OtherSyntax()) };
        }

        auto binding_start = this->lexer.position;
        auto binding_result = this->parse_binding(_rp, _ep);
        if (binding_result._tag == Result<BindingSyntax, ParserError>::Error)
        {
            switch (binding_result._Error._tag) {
                case ParserError::OtherSyntax:
                    return Result<MutableSyntax, ParserError> { ._tag = Result<MutableSyntax, ParserError>::Error, ._Error = ParserError(InvalidSyntax(binding_start, lexer.position, String(_ep, "a valid Binding syntax"))) };
                case ParserError::InvalidSyntax:
                    return Result<MutableSyntax, ParserError> { ._tag = Result<MutableSyntax, ParserError>::Error, ._Error = binding_result._Error };
            }
        }

        auto binding = binding_result._Ok;

        auto end = this->lexer.position;

        auto ret = MutableSyntax(start, end, binding);

        return Result<MutableSyntax, ParserError> { ._tag = Result<MutableSyntax, ParserError>::Ok, ._Ok = ret };
    }

    Result<BindingSyntax, ParserError> parse_binding(Page* _rp, Page* _ep) {
        auto start = this->lexer.previous_position;

        auto start_name = this->lexer.previous_position;
        auto name = this->lexer.parse_identifier(_rp, this->keywords);
        if (name == nullptr)
        {
            return Result<BindingSyntax, ParserError> { ._tag = Result<BindingSyntax, ParserError>::Error, ._Error = ParserError(OtherSyntax()) };

        }

        auto annotation_start = this->lexer.position;
        auto annotation_result = this->parse_bindingannotation(_rp, _ep);
        if (annotation_result._tag == Result<BindingAnnotationSyntax, ParserError>::Error)
        {
            switch (annotation_result._Error._tag) {
                case ParserError::OtherSyntax:
                    break;
                case ParserError::InvalidSyntax:
                    return Result<BindingSyntax, ParserError> { ._tag = Result<BindingSyntax, ParserError>::Error, ._Error = annotation_result._Error };
            }
        }

        BindingAnnotationSyntax* annotation = annotation_result._tag == Result<BindingAnnotationSyntax, ParserError>::Error ? nullptr : new(alignof(BindingAnnotationSyntax), _rp) BindingAnnotationSyntax(annotation_result._Ok);

        auto operation_start = this->lexer.position;
        auto operation_result = this->parse_operand_list(_rp, _ep);
        if (operation_result._tag == Result<Vector<OperandSyntax>, ParserError>::Error)
        {
            switch (operation_result._Error._tag) {
                case ParserError::OtherSyntax:
                    return Result<BindingSyntax, ParserError> { ._tag = Result<BindingSyntax, ParserError>::Error, ._Error = ParserError(InvalidSyntax(operation_start, lexer.position, String(_ep, "a valid Operand syntax"))) };
                case ParserError::InvalidSyntax:
                    return Result<BindingSyntax, ParserError> { ._tag = Result<BindingSyntax, ParserError>::Error, ._Error = operation_result._Error };
            }
        }

        auto operation = operation_result._Ok;

        auto end = this->lexer.position;

        auto ret = BindingSyntax(start, end, *name, annotation, operation);

        return Result<BindingSyntax, ParserError> { ._tag = Result<BindingSyntax, ParserError>::Ok, ._Ok = ret };
    }

    Result<BindingAnnotationSyntax, ParserError> parse_bindingannotation(Page* _rp, Page* _ep) {
        auto start = this->lexer.previous_position;

        auto start_colon_1 = this->lexer.previous_position;
        auto success_colon_1 = this->lexer.parse_colon(_rp);
        if (!success_colon_1) {
            return Result<BindingAnnotationSyntax, ParserError> { ._tag = Result<BindingAnnotationSyntax, ParserError>::Error, ._Error = ParserError(OtherSyntax()) };
        }

        auto spec_start = this->lexer.position;
        auto spec_result = this->parse_bindingspec(_rp, _ep);
        if (spec_result._tag == Result<BindingSpecSyntax, ParserError>::Error)
        {
            switch (spec_result._Error._tag) {
                case ParserError::OtherSyntax:
                    return Result<BindingAnnotationSyntax, ParserError> { ._tag = Result<BindingAnnotationSyntax, ParserError>::Error, ._Error = ParserError(InvalidSyntax(spec_start, lexer.position, String(_ep, "a valid BindingSpec syntax"))) };
                case ParserError::InvalidSyntax:
                    return Result<BindingAnnotationSyntax, ParserError> { ._tag = Result<BindingAnnotationSyntax, ParserError>::Error, ._Error = spec_result._Error };
            }
        }

        auto spec = spec_result._Ok;

        auto end = this->lexer.position;

        auto ret = BindingAnnotationSyntax(start, end, spec);

        return Result<BindingAnnotationSyntax, ParserError> { ._tag = Result<BindingAnnotationSyntax, ParserError>::Ok, ._Ok = ret };
    }

    Result<Vector<BindingSpecSyntax>*, ParserError> parse_bindingspec_list(Page* _rp, Page* _ep) {
        Region _r;
        List<BindingSpecSyntax>& list = *new(alignof(List<BindingSpecSyntax>), _r.get_page()) List<BindingSpecSyntax>();;
        while(true) {
            auto node_result = this->parse_bindingspec(_rp, _ep);
            if ((node_result._tag == Result<BindingSpecSyntax, ParserError>::Error) && (node_result._Error._tag == ParserError::InvalidSyntax))
                return Result<Vector<BindingSpecSyntax>*, ParserError> { ._tag = Result<Vector<BindingSpecSyntax>*, ParserError>::Error, ._Error = node_result._Error };
            if (node_result._tag == Result<BindingSpecSyntax, ParserError>::Ok) {
                auto node = node_result._Ok;
                list.add(node);
            } else {
                if ((list.count() == 0) && (node_result._tag == Result<BindingSpecSyntax, ParserError>::Error) && (node_result._Error._tag == ParserError::OtherSyntax))
                    return Result<Vector<BindingSpecSyntax>*, ParserError> { ._tag = Result<Vector<BindingSpecSyntax>*, ParserError>::Error, ._Error = node_result._Error };
                break;
            }
        }

        if (list.count() == 0)
            return Result<Vector<BindingSpecSyntax>*, ParserError> { ._tag = Result<Vector<BindingSpecSyntax>*, ParserError>::Ok, ._Ok = nullptr };
        
        return Result<Vector<BindingSpecSyntax>*, ParserError> {
            ._tag = Result<Vector<BindingSpecSyntax>*, ParserError>::Ok,
            ._Ok = new(alignof(Vector<BindingSpecSyntax>), _rp) Vector<BindingSpecSyntax>(_rp, list) };
    }

    Result<BindingSpecSyntax, ParserError> parse_bindingspec(Page* _rp, Page* _ep) {
        {
            auto node_result = this->parse_structure(_rp, _ep);
            if (node_result._tag == Result<StructureSyntax, ParserError>::Error)
            {
                if (node_result._Error._tag == ParserError::InvalidSyntax)
                    return Result<BindingSpecSyntax, ParserError> { ._tag = Result<BindingSpecSyntax, ParserError>::Error, ._Error = node_result._Error };
            }
            else
            {
                auto node = node_result._Ok;
                return Result<BindingSpecSyntax, ParserError> { ._tag = Result<BindingSpecSyntax, ParserError>::Ok, ._Ok = 
                    BindingSpecSyntax(StructureSyntax(node))
                };
            }
        }
        {
            auto node_result = this->parse_type(_rp, _ep);
            if (node_result._tag == Result<TypeSyntax, ParserError>::Error)
            {
                if (node_result._Error._tag == ParserError::InvalidSyntax)
                    return Result<BindingSpecSyntax, ParserError> { ._tag = Result<BindingSpecSyntax, ParserError>::Error, ._Error = node_result._Error };
            }
            else
            {
                auto node = node_result._Ok;
                return Result<BindingSpecSyntax, ParserError> { ._tag = Result<BindingSpecSyntax, ParserError>::Ok, ._Ok = 
                    BindingSpecSyntax(TypeSyntax(node))
                };
            }
        }
        {
            auto node_result = this->parse_array(_rp, _ep);
            if (node_result._tag == Result<ArraySyntax, ParserError>::Error)
            {
                if (node_result._Error._tag == ParserError::InvalidSyntax)
                    return Result<BindingSpecSyntax, ParserError> { ._tag = Result<BindingSpecSyntax, ParserError>::Error, ._Error = node_result._Error };
            }
            else
            {
                auto node = node_result._Ok;
                return Result<BindingSpecSyntax, ParserError> { ._tag = Result<BindingSpecSyntax, ParserError>::Ok, ._Ok = 
                    BindingSpecSyntax(ArraySyntax(node))
                };
            }
        }
        return Result<BindingSpecSyntax, ParserError> { ._tag = Result<BindingSpecSyntax, ParserError>::Error, ._Error = ParserError(OtherSyntax()) };
    }

    Result<ArraySyntax, ParserError> parse_array(Page* _rp, Page* _ep) {
        auto start = this->lexer.previous_position;

        auto start_left_bracket_1 = this->lexer.previous_position;
        auto success_left_bracket_1 = this->lexer.parse_punctuation('[');
        if (!success_left_bracket_1) {
            return Result<ArraySyntax, ParserError> { ._tag = Result<ArraySyntax, ParserError>::Error, ._Error = ParserError(OtherSyntax()) };
        }

        auto members_start = this->lexer.position;
        auto members_result = this->parse_type_list(_rp, _ep);
        if (members_result._tag == Result<Vector<TypeSyntax>, ParserError>::Error)
        {
            switch (members_result._Error._tag) {
                case ParserError::OtherSyntax:
                    break;
                case ParserError::InvalidSyntax:
                    return Result<ArraySyntax, ParserError> { ._tag = Result<ArraySyntax, ParserError>::Error, ._Error = members_result._Error };
            }
        }

        auto members = members_result._tag == Result<Vector<TypeSyntax>, ParserError>::Error ? nullptr : members_result._Ok;

        auto start_right_bracket_3 = this->lexer.previous_position;
        auto success_right_bracket_3 = this->lexer.parse_punctuation(']');
        if (!success_right_bracket_3) {
            return Result<ArraySyntax, ParserError> { ._tag = Result<ArraySyntax, ParserError>::Error, ._Error = ParserError(InvalidSyntax(start_right_bracket_3, lexer.position, String(_ep, "]"))) };        }

        auto end = this->lexer.position;

        auto ret = ArraySyntax(start, end, members);

        return Result<ArraySyntax, ParserError> { ._tag = Result<ArraySyntax, ParserError>::Ok, ._Ok = ret };
    }

    Result<StructureSyntax, ParserError> parse_structure(Page* _rp, Page* _ep) {
        auto start = this->lexer.previous_position;

        auto start_left_paren_1 = this->lexer.previous_position;
        auto success_left_paren_1 = this->lexer.parse_punctuation('(');
        if (!success_left_paren_1) {
            return Result<StructureSyntax, ParserError> { ._tag = Result<StructureSyntax, ParserError>::Error, ._Error = ParserError(OtherSyntax()) };
        }

        auto parts_start = this->lexer.position;
        auto parts_result = this->parse_part_list(_rp, _ep);
        if (parts_result._tag == Result<Vector<PartSyntax>, ParserError>::Error)
        {
            switch (parts_result._Error._tag) {
                case ParserError::OtherSyntax:
                    break;
                case ParserError::InvalidSyntax:
                    return Result<StructureSyntax, ParserError> { ._tag = Result<StructureSyntax, ParserError>::Error, ._Error = parts_result._Error };
            }
        }

        auto parts = parts_result._tag == Result<Vector<PartSyntax>, ParserError>::Error ? nullptr : parts_result._Ok;

        auto start_right_paren_3 = this->lexer.previous_position;
        auto success_right_paren_3 = this->lexer.parse_punctuation(')');
        if (!success_right_paren_3) {
            return Result<StructureSyntax, ParserError> { ._tag = Result<StructureSyntax, ParserError>::Error, ._Error = ParserError(InvalidSyntax(start_right_paren_3, lexer.position, String(_ep, ")"))) };        }

        auto end = this->lexer.position;

        auto ret = StructureSyntax(start, end, parts);

        return Result<StructureSyntax, ParserError> { ._tag = Result<StructureSyntax, ParserError>::Ok, ._Ok = ret };
    }

    Result<Vector<PartSyntax>*, ParserError> parse_part_list(Page* _rp, Page* _ep) {
        Region _r;
        List<PartSyntax>& list = *new(alignof(List<PartSyntax>), _r.get_page()) List<PartSyntax>();;
        while(true) {
            auto node_result = this->parse_part(_rp, _ep);
            if ((node_result._tag == Result<PartSyntax, ParserError>::Error) && (node_result._Error._tag == ParserError::InvalidSyntax))
                return Result<Vector<PartSyntax>*, ParserError> { ._tag = Result<Vector<PartSyntax>*, ParserError>::Error, ._Error = node_result._Error };
            if (node_result._tag == Result<PartSyntax, ParserError>::Ok) {
                auto node = node_result._Ok;
                list.add(node);
            } else {
                if ((list.count() == 0) && (node_result._tag == Result<PartSyntax, ParserError>::Error) && (node_result._Error._tag == ParserError::OtherSyntax))
                    return Result<Vector<PartSyntax>*, ParserError> { ._tag = Result<Vector<PartSyntax>*, ParserError>::Error, ._Error = node_result._Error };
                break;
            }
        }

        if (list.count() == 0)
            return Result<Vector<PartSyntax>*, ParserError> { ._tag = Result<Vector<PartSyntax>*, ParserError>::Ok, ._Ok = nullptr };
        
        return Result<Vector<PartSyntax>*, ParserError> {
            ._tag = Result<Vector<PartSyntax>*, ParserError>::Ok,
            ._Ok = new(alignof(Vector<PartSyntax>), _rp) Vector<PartSyntax>(_rp, list) };
    }

    Result<PartSyntax, ParserError> parse_part(Page* _rp, Page* _ep) {
        {
            auto node_result = this->parse_field(_rp, _ep);
            if (node_result._tag == Result<FieldSyntax, ParserError>::Error)
            {
                if (node_result._Error._tag == ParserError::InvalidSyntax)
                    return Result<PartSyntax, ParserError> { ._tag = Result<PartSyntax, ParserError>::Error, ._Error = node_result._Error };
            }
            else
            {
                auto node = node_result._Ok;
                return Result<PartSyntax, ParserError> { ._tag = Result<PartSyntax, ParserError>::Ok, ._Ok = 
                    PartSyntax(FieldSyntax(node))
                };
            }
        }
        {
            auto node_result = this->parse_property(_rp, _ep);
            if (node_result._tag == Result<PropertySyntax, ParserError>::Error)
            {
                if (node_result._Error._tag == ParserError::InvalidSyntax)
                    return Result<PartSyntax, ParserError> { ._tag = Result<PartSyntax, ParserError>::Error, ._Error = node_result._Error };
            }
            else
            {
                auto node = node_result._Ok;
                return Result<PartSyntax, ParserError> { ._tag = Result<PartSyntax, ParserError>::Ok, ._Ok = 
                    PartSyntax(PropertySyntax(node))
                };
            }
        }
        return Result<PartSyntax, ParserError> { ._tag = Result<PartSyntax, ParserError>::Error, ._Error = ParserError(OtherSyntax()) };
    }

    Result<FieldSyntax, ParserError> parse_field(Page* _rp, Page* _ep) {
        auto start = this->lexer.previous_position;

        auto start_private_1 = this->lexer.previous_position;
        auto success_private_1 = this->lexer.parse_keyword(_rp, *this->keywords_index[30]);
        if (!success_private_1) {
            return Result<FieldSyntax, ParserError> { ._tag = Result<FieldSyntax, ParserError>::Error, ._Error = ParserError(OtherSyntax()) };
        }

        auto property_start = this->lexer.position;
        auto property_result = this->parse_property(_rp, _ep);
        if (property_result._tag == Result<PropertySyntax, ParserError>::Error)
        {
            switch (property_result._Error._tag) {
                case ParserError::OtherSyntax:
                    return Result<FieldSyntax, ParserError> { ._tag = Result<FieldSyntax, ParserError>::Error, ._Error = ParserError(InvalidSyntax(property_start, lexer.position, String(_ep, "a valid Property syntax"))) };
                case ParserError::InvalidSyntax:
                    return Result<FieldSyntax, ParserError> { ._tag = Result<FieldSyntax, ParserError>::Error, ._Error = property_result._Error };
            }
        }

        auto property = property_result._Ok;

        auto end = this->lexer.position;

        auto ret = FieldSyntax(start, end, property);

        return Result<FieldSyntax, ParserError> { ._tag = Result<FieldSyntax, ParserError>::Ok, ._Ok = ret };
    }

    Result<Vector<PropertySyntax>*, ParserError> parse_property_list(Page* _rp, Page* _ep) {
        Region _r;
        List<PropertySyntax>& list = *new(alignof(List<PropertySyntax>), _r.get_page()) List<PropertySyntax>();;
        while(true) {
            auto node_result = this->parse_property(_rp, _ep);
            if ((node_result._tag == Result<PropertySyntax, ParserError>::Error) && (node_result._Error._tag == ParserError::InvalidSyntax))
                return Result<Vector<PropertySyntax>*, ParserError> { ._tag = Result<Vector<PropertySyntax>*, ParserError>::Error, ._Error = node_result._Error };
            if (node_result._tag == Result<PropertySyntax, ParserError>::Ok) {
                auto node = node_result._Ok;
                list.add(node);
            } else {
                if ((list.count() == 0) && (node_result._tag == Result<PropertySyntax, ParserError>::Error) && (node_result._Error._tag == ParserError::OtherSyntax))
                    return Result<Vector<PropertySyntax>*, ParserError> { ._tag = Result<Vector<PropertySyntax>*, ParserError>::Error, ._Error = node_result._Error };
                break;
            }
        }

        if (list.count() == 0)
            return Result<Vector<PropertySyntax>*, ParserError> { ._tag = Result<Vector<PropertySyntax>*, ParserError>::Ok, ._Ok = nullptr };
        
        return Result<Vector<PropertySyntax>*, ParserError> {
            ._tag = Result<Vector<PropertySyntax>*, ParserError>::Ok,
            ._Ok = new(alignof(Vector<PropertySyntax>), _rp) Vector<PropertySyntax>(_rp, list) };
    }

    Result<PropertySyntax, ParserError> parse_property(Page* _rp, Page* _ep) {
        auto start = this->lexer.previous_position;

        auto start_name = this->lexer.previous_position;
        auto name = this->lexer.parse_identifier(_rp, this->keywords);
        if (name == nullptr)
        {
            return Result<PropertySyntax, ParserError> { ._tag = Result<PropertySyntax, ParserError>::Error, ._Error = ParserError(OtherSyntax()) };

        }

        auto annotation_start = this->lexer.position;
        auto annotation_result = this->parse_typeannotation(_rp, _ep);
        if (annotation_result._tag == Result<TypeAnnotationSyntax, ParserError>::Error)
        {
            switch (annotation_result._Error._tag) {
                case ParserError::OtherSyntax:
                    break;
                case ParserError::InvalidSyntax:
                    return Result<PropertySyntax, ParserError> { ._tag = Result<PropertySyntax, ParserError>::Error, ._Error = annotation_result._Error };
            }
        }

        TypeAnnotationSyntax* annotation = annotation_result._tag == Result<TypeAnnotationSyntax, ParserError>::Error ? nullptr : new(alignof(TypeAnnotationSyntax), _rp) TypeAnnotationSyntax(annotation_result._Ok);

        auto initializer_start = this->lexer.position;
        auto initializer_result = this->parse_initializer(_rp, _ep);
        if (initializer_result._tag == Result<InitializerSyntax, ParserError>::Error)
        {
            switch (initializer_result._Error._tag) {
                case ParserError::OtherSyntax:
                    break;
                case ParserError::InvalidSyntax:
                    return Result<PropertySyntax, ParserError> { ._tag = Result<PropertySyntax, ParserError>::Error, ._Error = initializer_result._Error };
            }
        }

        InitializerSyntax* initializer = initializer_result._tag == Result<InitializerSyntax, ParserError>::Error ? nullptr : new(alignof(InitializerSyntax), _rp) InitializerSyntax(initializer_result._Ok);

        auto start_comma_4 = this->lexer.previous_position;
        auto success_comma_4 = this->lexer.parse_punctuation(',');
        if (!success_comma_4) {
        }

        auto start_colon_5 = this->lexer.previous_position;
        auto success_colon_5 = this->lexer.parse_colon(_rp);
        if (!success_colon_5) {
        }

        auto attributes_start = this->lexer.position;
        auto attributes_result = this->parse_attribute_list(_rp, _ep);
        if (attributes_result._tag == Result<Vector<AttributeSyntax>, ParserError>::Error)
        {
            switch (attributes_result._Error._tag) {
                case ParserError::OtherSyntax:
                    break;
                case ParserError::InvalidSyntax:
                    return Result<PropertySyntax, ParserError> { ._tag = Result<PropertySyntax, ParserError>::Error, ._Error = attributes_result._Error };
            }
        }

        auto attributes = attributes_result._tag == Result<Vector<AttributeSyntax>, ParserError>::Error ? nullptr : attributes_result._Ok;

        auto start_colon_7 = this->lexer.previous_position;
        auto success_colon_7 = this->lexer.parse_colon(_rp);
        if (!success_colon_7) {
        }

        auto end = this->lexer.position;

        auto ret = PropertySyntax(start, end, *name, annotation, initializer, attributes);

        return Result<PropertySyntax, ParserError> { ._tag = Result<PropertySyntax, ParserError>::Ok, ._Ok = ret };
    }

    Result<ContinueSyntax, ParserError> parse_continue(Page* _rp, Page* _ep) {
        auto start = this->lexer.previous_position;

        auto start_continue_1 = this->lexer.previous_position;
        auto success_continue_1 = this->lexer.parse_keyword(_rp, *this->keywords_index[3]);
        if (!success_continue_1) {
            return Result<ContinueSyntax, ParserError> { ._tag = Result<ContinueSyntax, ParserError>::Error, ._Error = ParserError(OtherSyntax()) };
        }

        auto name_start = this->lexer.position;
        auto name_result = this->parse_loop(_rp, _ep);
        if (name_result._tag == Result<LoopSyntax, ParserError>::Error)
        {
            switch (name_result._Error._tag) {
                case ParserError::OtherSyntax:
                    break;
                case ParserError::InvalidSyntax:
                    return Result<ContinueSyntax, ParserError> { ._tag = Result<ContinueSyntax, ParserError>::Error, ._Error = name_result._Error };
            }
        }

        LoopSyntax* name = name_result._tag == Result<LoopSyntax, ParserError>::Error ? nullptr : new(alignof(LoopSyntax), _rp) LoopSyntax(name_result._Ok);

        auto start_colon_3 = this->lexer.previous_position;
        auto success_colon_3 = this->lexer.parse_colon(_rp);
        if (!success_colon_3) {
        }

        auto end = this->lexer.position;

        auto ret = ContinueSyntax(start, end, name);

        return Result<ContinueSyntax, ParserError> { ._tag = Result<ContinueSyntax, ParserError>::Ok, ._Ok = ret };
    }

    Result<BreakSyntax, ParserError> parse_break(Page* _rp, Page* _ep) {
        auto start = this->lexer.previous_position;

        auto start_break_1 = this->lexer.previous_position;
        auto success_break_1 = this->lexer.parse_keyword(_rp, *this->keywords_index[0]);
        if (!success_break_1) {
            return Result<BreakSyntax, ParserError> { ._tag = Result<BreakSyntax, ParserError>::Error, ._Error = ParserError(OtherSyntax()) };
        }

        auto name_start = this->lexer.position;
        auto name_result = this->parse_loop(_rp, _ep);
        if (name_result._tag == Result<LoopSyntax, ParserError>::Error)
        {
            switch (name_result._Error._tag) {
                case ParserError::OtherSyntax:
                    break;
                case ParserError::InvalidSyntax:
                    return Result<BreakSyntax, ParserError> { ._tag = Result<BreakSyntax, ParserError>::Error, ._Error = name_result._Error };
            }
        }

        LoopSyntax* name = name_result._tag == Result<LoopSyntax, ParserError>::Error ? nullptr : new(alignof(LoopSyntax), _rp) LoopSyntax(name_result._Ok);

        auto result_start = this->lexer.position;
        auto result_result = this->parse_operand_list(_rp, _ep);
        if (result_result._tag == Result<Vector<OperandSyntax>, ParserError>::Error)
        {
            switch (result_result._Error._tag) {
                case ParserError::OtherSyntax:
                    break;
                case ParserError::InvalidSyntax:
                    return Result<BreakSyntax, ParserError> { ._tag = Result<BreakSyntax, ParserError>::Error, ._Error = result_result._Error };
            }
        }

        auto result = result_result._tag == Result<Vector<OperandSyntax>, ParserError>::Error ? nullptr : result_result._Ok;

        auto start_colon_4 = this->lexer.previous_position;
        auto success_colon_4 = this->lexer.parse_colon(_rp);
        if (!success_colon_4) {
        }

        auto end = this->lexer.position;

        auto ret = BreakSyntax(start, end, name, result);

        return Result<BreakSyntax, ParserError> { ._tag = Result<BreakSyntax, ParserError>::Ok, ._Ok = ret };
    }

    Result<LoopSyntax, ParserError> parse_loop(Page* _rp, Page* _ep) {
        auto start = this->lexer.previous_position;

        auto start_loop_1 = this->lexer.previous_position;
        auto success_loop_1 = this->lexer.parse_keyword(_rp, *this->keywords_index[23]);
        if (!success_loop_1) {
            return Result<LoopSyntax, ParserError> { ._tag = Result<LoopSyntax, ParserError>::Error, ._Error = ParserError(OtherSyntax()) };
        }

        auto start_name = this->lexer.previous_position;
        auto name = this->lexer.parse_identifier(_rp, this->keywords);
        if (name == nullptr)
        {
            return Result<LoopSyntax, ParserError> { ._tag = Result<LoopSyntax, ParserError>::Error, ._Error = ParserError(InvalidSyntax(start_name, lexer.position, String(_ep, "an identifier"))) };
        }

        auto end = this->lexer.position;

        auto ret = LoopSyntax(start, end, *name);

        return Result<LoopSyntax, ParserError> { ._tag = Result<LoopSyntax, ParserError>::Ok, ._Ok = ret };
    }

    Result<ReturnSyntax, ParserError> parse_return(Page* _rp, Page* _ep) {
        auto start = this->lexer.previous_position;

        auto start_return_1 = this->lexer.previous_position;
        auto success_return_1 = this->lexer.parse_keyword(_rp, *this->keywords_index[31]);
        if (!success_return_1) {
            return Result<ReturnSyntax, ParserError> { ._tag = Result<ReturnSyntax, ParserError>::Error, ._Error = ParserError(OtherSyntax()) };
        }

        auto result_start = this->lexer.position;
        auto result_result = this->parse_operand_list(_rp, _ep);
        if (result_result._tag == Result<Vector<OperandSyntax>, ParserError>::Error)
        {
            switch (result_result._Error._tag) {
                case ParserError::OtherSyntax:
                    break;
                case ParserError::InvalidSyntax:
                    return Result<ReturnSyntax, ParserError> { ._tag = Result<ReturnSyntax, ParserError>::Error, ._Error = result_result._Error };
            }
        }

        auto result = result_result._tag == Result<Vector<OperandSyntax>, ParserError>::Error ? nullptr : result_result._Ok;

        auto end = this->lexer.position;

        auto ret = ReturnSyntax(start, end, result);

        return Result<ReturnSyntax, ParserError> { ._tag = Result<ReturnSyntax, ParserError>::Ok, ._Ok = ret };
    }

    Result<ThrowSyntax, ParserError> parse_throw(Page* _rp, Page* _ep) {
        auto start = this->lexer.previous_position;

        auto start_throw_1 = this->lexer.previous_position;
        auto success_throw_1 = this->lexer.parse_keyword(_rp, *this->keywords_index[36]);
        if (!success_throw_1) {
            return Result<ThrowSyntax, ParserError> { ._tag = Result<ThrowSyntax, ParserError>::Error, ._Error = ParserError(OtherSyntax()) };
        }

        auto result_start = this->lexer.position;
        auto result_result = this->parse_operand_list(_rp, _ep);
        if (result_result._tag == Result<Vector<OperandSyntax>, ParserError>::Error)
        {
            switch (result_result._Error._tag) {
                case ParserError::OtherSyntax:
                    break;
                case ParserError::InvalidSyntax:
                    return Result<ThrowSyntax, ParserError> { ._tag = Result<ThrowSyntax, ParserError>::Error, ._Error = result_result._Error };
            }
        }

        auto result = result_result._tag == Result<Vector<OperandSyntax>, ParserError>::Error ? nullptr : result_result._Ok;

        auto end = this->lexer.position;

        auto ret = ThrowSyntax(start, end, result);

        return Result<ThrowSyntax, ParserError> { ._tag = Result<ThrowSyntax, ParserError>::Ok, ._Ok = ret };
    }

    Result<MatchSyntax, ParserError> parse_match(Page* _rp, Page* _ep) {
        auto start = this->lexer.previous_position;

        auto start_match_1 = this->lexer.previous_position;
        auto success_match_1 = this->lexer.parse_keyword(_rp, *this->keywords_index[25]);
        if (!success_match_1) {
            return Result<MatchSyntax, ParserError> { ._tag = Result<MatchSyntax, ParserError>::Error, ._Error = ParserError(OtherSyntax()) };
        }

        auto scrutinee_start = this->lexer.position;
        auto scrutinee_result = this->parse_operand_list(_rp, _ep);
        if (scrutinee_result._tag == Result<Vector<OperandSyntax>, ParserError>::Error)
        {
            switch (scrutinee_result._Error._tag) {
                case ParserError::OtherSyntax:
                    return Result<MatchSyntax, ParserError> { ._tag = Result<MatchSyntax, ParserError>::Error, ._Error = ParserError(InvalidSyntax(scrutinee_start, lexer.position, String(_ep, "a valid Operand syntax"))) };
                case ParserError::InvalidSyntax:
                    return Result<MatchSyntax, ParserError> { ._tag = Result<MatchSyntax, ParserError>::Error, ._Error = scrutinee_result._Error };
            }
        }

        auto scrutinee = scrutinee_result._Ok;

        auto start_colon_3 = this->lexer.previous_position;
        auto success_colon_3 = this->lexer.parse_colon(_rp);
        if (!success_colon_3) {
            return Result<MatchSyntax, ParserError> { ._tag = Result<MatchSyntax, ParserError>::Error, ._Error = ParserError(InvalidSyntax(start_colon_3, lexer.position, String(_ep, "a colon or a line feed"))) };        }

        auto cases_start = this->lexer.position;
        auto cases_result = this->parse_case_list(_rp, _ep);
        if (cases_result._tag == Result<Vector<CaseSyntax>, ParserError>::Error)
        {
            switch (cases_result._Error._tag) {
                case ParserError::OtherSyntax:
                    return Result<MatchSyntax, ParserError> { ._tag = Result<MatchSyntax, ParserError>::Error, ._Error = ParserError(InvalidSyntax(cases_start, lexer.position, String(_ep, "a valid Case syntax"))) };
                case ParserError::InvalidSyntax:
                    return Result<MatchSyntax, ParserError> { ._tag = Result<MatchSyntax, ParserError>::Error, ._Error = cases_result._Error };
            }
        }

        auto cases = cases_result._Ok;

        auto alternative_start = this->lexer.position;
        auto alternative_result = this->parse_default(_rp, _ep);
        if (alternative_result._tag == Result<DefaultSyntax, ParserError>::Error)
        {
            switch (alternative_result._Error._tag) {
                case ParserError::OtherSyntax:
                    break;
                case ParserError::InvalidSyntax:
                    return Result<MatchSyntax, ParserError> { ._tag = Result<MatchSyntax, ParserError>::Error, ._Error = alternative_result._Error };
            }
        }

        DefaultSyntax* alternative = alternative_result._tag == Result<DefaultSyntax, ParserError>::Error ? nullptr : new(alignof(DefaultSyntax), _rp) DefaultSyntax(alternative_result._Ok);

        auto end = this->lexer.position;

        auto ret = MatchSyntax(start, end, scrutinee, cases, alternative);

        return Result<MatchSyntax, ParserError> { ._tag = Result<MatchSyntax, ParserError>::Ok, ._Ok = ret };
    }

    Result<Vector<CaseSyntax>*, ParserError> parse_case_list(Page* _rp, Page* _ep) {
        Region _r;
        List<CaseSyntax>& list = *new(alignof(List<CaseSyntax>), _r.get_page()) List<CaseSyntax>();;
        while(true) {
            auto node_result = this->parse_case(_rp, _ep);
            if ((node_result._tag == Result<CaseSyntax, ParserError>::Error) && (node_result._Error._tag == ParserError::InvalidSyntax))
                return Result<Vector<CaseSyntax>*, ParserError> { ._tag = Result<Vector<CaseSyntax>*, ParserError>::Error, ._Error = node_result._Error };
            if (node_result._tag == Result<CaseSyntax, ParserError>::Ok) {
                auto node = node_result._Ok;
                list.add(node);
            } else {
                if ((list.count() == 0) && (node_result._tag == Result<CaseSyntax, ParserError>::Error) && (node_result._Error._tag == ParserError::OtherSyntax))
                    return Result<Vector<CaseSyntax>*, ParserError> { ._tag = Result<Vector<CaseSyntax>*, ParserError>::Error, ._Error = node_result._Error };
                break;
            }
        }

        if (list.count() == 0)
            return Result<Vector<CaseSyntax>*, ParserError> { ._tag = Result<Vector<CaseSyntax>*, ParserError>::Ok, ._Ok = nullptr };
        
        return Result<Vector<CaseSyntax>*, ParserError> {
            ._tag = Result<Vector<CaseSyntax>*, ParserError>::Ok,
            ._Ok = new(alignof(Vector<CaseSyntax>), _rp) Vector<CaseSyntax>(_rp, list) };
    }

    Result<CaseSyntax, ParserError> parse_case(Page* _rp, Page* _ep) {
        auto start = this->lexer.previous_position;

        auto start_case_1 = this->lexer.previous_position;
        auto success_case_1 = this->lexer.parse_keyword(_rp, *this->keywords_index[2]);
        if (!success_case_1) {
            return Result<CaseSyntax, ParserError> { ._tag = Result<CaseSyntax, ParserError>::Error, ._Error = ParserError(OtherSyntax()) };
        }

        auto condition_start = this->lexer.position;
        auto condition_result = this->parse_operand_list(_rp, _ep);
        if (condition_result._tag == Result<Vector<OperandSyntax>, ParserError>::Error)
        {
            switch (condition_result._Error._tag) {
                case ParserError::OtherSyntax:
                    return Result<CaseSyntax, ParserError> { ._tag = Result<CaseSyntax, ParserError>::Error, ._Error = ParserError(InvalidSyntax(condition_start, lexer.position, String(_ep, "a valid Operand syntax"))) };
                case ParserError::InvalidSyntax:
                    return Result<CaseSyntax, ParserError> { ._tag = Result<CaseSyntax, ParserError>::Error, ._Error = condition_result._Error };
            }
        }

        auto condition = condition_result._Ok;

        auto start_colon_3 = this->lexer.previous_position;
        auto success_colon_3 = this->lexer.parse_colon(_rp);
        if (!success_colon_3) {
            return Result<CaseSyntax, ParserError> { ._tag = Result<CaseSyntax, ParserError>::Error, ._Error = ParserError(InvalidSyntax(start_colon_3, lexer.position, String(_ep, "a colon or a line feed"))) };        }

        auto consequent_start = this->lexer.position;
        auto consequent_result = this->parse_action(_rp, _ep);
        if (consequent_result._tag == Result<ActionSyntax, ParserError>::Error)
        {
            switch (consequent_result._Error._tag) {
                case ParserError::OtherSyntax:
                    return Result<CaseSyntax, ParserError> { ._tag = Result<CaseSyntax, ParserError>::Error, ._Error = ParserError(InvalidSyntax(consequent_start, lexer.position, String(_ep, "a valid Action syntax"))) };
                case ParserError::InvalidSyntax:
                    return Result<CaseSyntax, ParserError> { ._tag = Result<CaseSyntax, ParserError>::Error, ._Error = consequent_result._Error };
            }
        }

        auto consequent = consequent_result._Ok;

        auto end = this->lexer.position;

        auto ret = CaseSyntax(start, end, condition, consequent);

        return Result<CaseSyntax, ParserError> { ._tag = Result<CaseSyntax, ParserError>::Ok, ._Ok = ret };
    }

    Result<DefaultSyntax, ParserError> parse_default(Page* _rp, Page* _ep) {
        auto start = this->lexer.previous_position;

        auto start_default_1 = this->lexer.previous_position;
        auto success_default_1 = this->lexer.parse_keyword(_rp, *this->keywords_index[5]);
        if (!success_default_1) {
            return Result<DefaultSyntax, ParserError> { ._tag = Result<DefaultSyntax, ParserError>::Error, ._Error = ParserError(OtherSyntax()) };
        }

        auto alternative_start = this->lexer.position;
        auto alternative_result = this->parse_action(_rp, _ep);
        if (alternative_result._tag == Result<ActionSyntax, ParserError>::Error)
        {
            switch (alternative_result._Error._tag) {
                case ParserError::OtherSyntax:
                    break;
                case ParserError::InvalidSyntax:
                    return Result<DefaultSyntax, ParserError> { ._tag = Result<DefaultSyntax, ParserError>::Error, ._Error = alternative_result._Error };
            }
        }

        ActionSyntax* alternative = alternative_result._tag == Result<ActionSyntax, ParserError>::Error ? nullptr : new(alignof(ActionSyntax), _rp) ActionSyntax(alternative_result._Ok);

        auto end = this->lexer.position;

        auto ret = DefaultSyntax(start, end, alternative);

        return Result<DefaultSyntax, ParserError> { ._tag = Result<DefaultSyntax, ParserError>::Ok, ._Ok = ret };
    }

    Result<LambdaSyntax, ParserError> parse_lambda(Page* _rp, Page* _ep) {
        auto start = this->lexer.previous_position;

        auto start_lambda_1 = this->lexer.previous_position;
        auto success_lambda_1 = this->lexer.parse_keyword(_rp, *this->keywords_index[21]);
        if (!success_lambda_1) {
            return Result<LambdaSyntax, ParserError> { ._tag = Result<LambdaSyntax, ParserError>::Error, ._Error = ParserError(OtherSyntax()) };
        }

        auto input_start = this->lexer.position;
        auto input_result = this->parse_operand_list(_rp, _ep);
        if (input_result._tag == Result<Vector<OperandSyntax>, ParserError>::Error)
        {
            switch (input_result._Error._tag) {
                case ParserError::OtherSyntax:
                    return Result<LambdaSyntax, ParserError> { ._tag = Result<LambdaSyntax, ParserError>::Error, ._Error = ParserError(InvalidSyntax(input_start, lexer.position, String(_ep, "a valid Operand syntax"))) };
                case ParserError::InvalidSyntax:
                    return Result<LambdaSyntax, ParserError> { ._tag = Result<LambdaSyntax, ParserError>::Error, ._Error = input_result._Error };
            }
        }

        auto input = input_result._Ok;

        auto start_colon_3 = this->lexer.previous_position;
        auto success_colon_3 = this->lexer.parse_colon(_rp);
        if (!success_colon_3) {
            return Result<LambdaSyntax, ParserError> { ._tag = Result<LambdaSyntax, ParserError>::Error, ._Error = ParserError(InvalidSyntax(start_colon_3, lexer.position, String(_ep, "a colon or a line feed"))) };        }

        auto block_start = this->lexer.position;
        auto block_result = this->parse_action(_rp, _ep);
        if (block_result._tag == Result<ActionSyntax, ParserError>::Error)
        {
            switch (block_result._Error._tag) {
                case ParserError::OtherSyntax:
                    return Result<LambdaSyntax, ParserError> { ._tag = Result<LambdaSyntax, ParserError>::Error, ._Error = ParserError(InvalidSyntax(block_start, lexer.position, String(_ep, "a valid Action syntax"))) };
                case ParserError::InvalidSyntax:
                    return Result<LambdaSyntax, ParserError> { ._tag = Result<LambdaSyntax, ParserError>::Error, ._Error = block_result._Error };
            }
        }

        auto block = block_result._Ok;

        auto end = this->lexer.position;

        auto ret = LambdaSyntax(start, end, input, block);

        return Result<LambdaSyntax, ParserError> { ._tag = Result<LambdaSyntax, ParserError>::Ok, ._Ok = ret };
    }

    Result<ForSyntax, ParserError> parse_for(Page* _rp, Page* _ep) {
        auto start = this->lexer.previous_position;

        auto start_for_1 = this->lexer.previous_position;
        auto success_for_1 = this->lexer.parse_keyword(_rp, *this->keywords_index[12]);
        if (!success_for_1) {
            return Result<ForSyntax, ParserError> { ._tag = Result<ForSyntax, ParserError>::Error, ._Error = ParserError(OtherSyntax()) };
        }

        auto start_variable = this->lexer.previous_position;
        auto variable = this->lexer.parse_identifier(_rp, this->keywords);
        if (variable == nullptr)
        {
            return Result<ForSyntax, ParserError> { ._tag = Result<ForSyntax, ParserError>::Error, ._Error = ParserError(InvalidSyntax(start_variable, lexer.position, String(_ep, "an identifier"))) };
        }

        auto annotation_start = this->lexer.position;
        auto annotation_result = this->parse_typeannotation(_rp, _ep);
        if (annotation_result._tag == Result<TypeAnnotationSyntax, ParserError>::Error)
        {
            switch (annotation_result._Error._tag) {
                case ParserError::OtherSyntax:
                    break;
                case ParserError::InvalidSyntax:
                    return Result<ForSyntax, ParserError> { ._tag = Result<ForSyntax, ParserError>::Error, ._Error = annotation_result._Error };
            }
        }

        TypeAnnotationSyntax* annotation = annotation_result._tag == Result<TypeAnnotationSyntax, ParserError>::Error ? nullptr : new(alignof(TypeAnnotationSyntax), _rp) TypeAnnotationSyntax(annotation_result._Ok);

        auto start_in_4 = this->lexer.previous_position;
        auto success_in_4 = this->lexer.parse_keyword(_rp, *this->keywords_index[17]);
        if (!success_in_4) {
            return Result<ForSyntax, ParserError> { ._tag = Result<ForSyntax, ParserError>::Error, ._Error = ParserError(InvalidSyntax(start_in_4, lexer.position, String(_ep, "in"))) };        }

        auto operation_start = this->lexer.position;
        auto operation_result = this->parse_operand_list(_rp, _ep);
        if (operation_result._tag == Result<Vector<OperandSyntax>, ParserError>::Error)
        {
            switch (operation_result._Error._tag) {
                case ParserError::OtherSyntax:
                    return Result<ForSyntax, ParserError> { ._tag = Result<ForSyntax, ParserError>::Error, ._Error = ParserError(InvalidSyntax(operation_start, lexer.position, String(_ep, "a valid Operand syntax"))) };
                case ParserError::InvalidSyntax:
                    return Result<ForSyntax, ParserError> { ._tag = Result<ForSyntax, ParserError>::Error, ._Error = operation_result._Error };
            }
        }

        auto operation = operation_result._Ok;

        auto start_colon_6 = this->lexer.previous_position;
        auto success_colon_6 = this->lexer.parse_colon(_rp);
        if (!success_colon_6) {
            return Result<ForSyntax, ParserError> { ._tag = Result<ForSyntax, ParserError>::Error, ._Error = ParserError(InvalidSyntax(start_colon_6, lexer.position, String(_ep, "a colon or a line feed"))) };        }

        auto name_start = this->lexer.position;
        auto name_result = this->parse_label(_rp, _ep);
        if (name_result._tag == Result<LabelSyntax, ParserError>::Error)
        {
            switch (name_result._Error._tag) {
                case ParserError::OtherSyntax:
                    break;
                case ParserError::InvalidSyntax:
                    return Result<ForSyntax, ParserError> { ._tag = Result<ForSyntax, ParserError>::Error, ._Error = name_result._Error };
            }
        }

        LabelSyntax* name = name_result._tag == Result<LabelSyntax, ParserError>::Error ? nullptr : new(alignof(LabelSyntax), _rp) LabelSyntax(name_result._Ok);

        auto action_start = this->lexer.position;
        auto action_result = this->parse_action(_rp, _ep);
        if (action_result._tag == Result<ActionSyntax, ParserError>::Error)
        {
            switch (action_result._Error._tag) {
                case ParserError::OtherSyntax:
                    return Result<ForSyntax, ParserError> { ._tag = Result<ForSyntax, ParserError>::Error, ._Error = ParserError(InvalidSyntax(action_start, lexer.position, String(_ep, "a valid Action syntax"))) };
                case ParserError::InvalidSyntax:
                    return Result<ForSyntax, ParserError> { ._tag = Result<ForSyntax, ParserError>::Error, ._Error = action_result._Error };
            }
        }

        auto action = action_result._Ok;

        auto end = this->lexer.position;

        auto ret = ForSyntax(start, end, *variable, annotation, operation, name, action);

        return Result<ForSyntax, ParserError> { ._tag = Result<ForSyntax, ParserError>::Ok, ._Ok = ret };
    }

    Result<LabelSyntax, ParserError> parse_label(Page* _rp, Page* _ep) {
        auto start = this->lexer.previous_position;

        auto start_label_1 = this->lexer.previous_position;
        auto success_label_1 = this->lexer.parse_keyword(_rp, *this->keywords_index[20]);
        if (!success_label_1) {
            return Result<LabelSyntax, ParserError> { ._tag = Result<LabelSyntax, ParserError>::Error, ._Error = ParserError(OtherSyntax()) };
        }

        auto start_name = this->lexer.previous_position;
        auto name = this->lexer.parse_identifier(_rp, this->keywords);
        if (name == nullptr)
        {
            return Result<LabelSyntax, ParserError> { ._tag = Result<LabelSyntax, ParserError>::Error, ._Error = ParserError(InvalidSyntax(start_name, lexer.position, String(_ep, "an identifier"))) };
        }

        auto start_colon_3 = this->lexer.previous_position;
        auto success_colon_3 = this->lexer.parse_colon(_rp);
        if (!success_colon_3) {
        }

        auto end = this->lexer.position;

        auto ret = LabelSyntax(start, end, *name);

        return Result<LabelSyntax, ParserError> { ._tag = Result<LabelSyntax, ParserError>::Ok, ._Ok = ret };
    }

    Result<RepeatSyntax, ParserError> parse_repeat(Page* _rp, Page* _ep) {
        auto start = this->lexer.previous_position;

        auto start_repeat_1 = this->lexer.previous_position;
        auto success_repeat_1 = this->lexer.parse_keyword(_rp, *this->keywords_index[33]);
        if (!success_repeat_1) {
            return Result<RepeatSyntax, ParserError> { ._tag = Result<RepeatSyntax, ParserError>::Error, ._Error = ParserError(OtherSyntax()) };
        }

        auto name_start = this->lexer.position;
        auto name_result = this->parse_label(_rp, _ep);
        if (name_result._tag == Result<LabelSyntax, ParserError>::Error)
        {
            switch (name_result._Error._tag) {
                case ParserError::OtherSyntax:
                    break;
                case ParserError::InvalidSyntax:
                    return Result<RepeatSyntax, ParserError> { ._tag = Result<RepeatSyntax, ParserError>::Error, ._Error = name_result._Error };
            }
        }

        LabelSyntax* name = name_result._tag == Result<LabelSyntax, ParserError>::Error ? nullptr : new(alignof(LabelSyntax), _rp) LabelSyntax(name_result._Ok);

        auto action_start = this->lexer.position;
        auto action_result = this->parse_action(_rp, _ep);
        if (action_result._tag == Result<ActionSyntax, ParserError>::Error)
        {
            switch (action_result._Error._tag) {
                case ParserError::OtherSyntax:
                    return Result<RepeatSyntax, ParserError> { ._tag = Result<RepeatSyntax, ParserError>::Error, ._Error = ParserError(InvalidSyntax(action_start, lexer.position, String(_ep, "a valid Action syntax"))) };
                case ParserError::InvalidSyntax:
                    return Result<RepeatSyntax, ParserError> { ._tag = Result<RepeatSyntax, ParserError>::Error, ._Error = action_result._Error };
            }
        }

        auto action = action_result._Ok;

        auto end = this->lexer.position;

        auto ret = RepeatSyntax(start, end, name, action);

        return Result<RepeatSyntax, ParserError> { ._tag = Result<RepeatSyntax, ParserError>::Ok, ._Ok = ret };
    }

    Result<Vector<ActionSyntax>*, ParserError> parse_action_list(Page* _rp, Page* _ep) {
        Region _r;
        List<ActionSyntax>& list = *new(alignof(List<ActionSyntax>), _r.get_page()) List<ActionSyntax>();;
        while(true) {
            auto node_result = this->parse_action(_rp, _ep);
            if ((node_result._tag == Result<ActionSyntax, ParserError>::Error) && (node_result._Error._tag == ParserError::InvalidSyntax))
                return Result<Vector<ActionSyntax>*, ParserError> { ._tag = Result<Vector<ActionSyntax>*, ParserError>::Error, ._Error = node_result._Error };
            if (node_result._tag == Result<ActionSyntax, ParserError>::Ok) {
                auto node = node_result._Ok;
                list.add(node);
            } else {
                if ((list.count() == 0) && (node_result._tag == Result<ActionSyntax, ParserError>::Error) && (node_result._Error._tag == ParserError::OtherSyntax))
                    return Result<Vector<ActionSyntax>*, ParserError> { ._tag = Result<Vector<ActionSyntax>*, ParserError>::Error, ._Error = node_result._Error };
                break;
            }
        }

        if (list.count() == 0)
            return Result<Vector<ActionSyntax>*, ParserError> { ._tag = Result<Vector<ActionSyntax>*, ParserError>::Ok, ._Ok = nullptr };
        
        return Result<Vector<ActionSyntax>*, ParserError> {
            ._tag = Result<Vector<ActionSyntax>*, ParserError>::Ok,
            ._Ok = new(alignof(Vector<ActionSyntax>), _rp) Vector<ActionSyntax>(_rp, list) };
    }

    Result<ActionSyntax, ParserError> parse_action(Page* _rp, Page* _ep) {
        {
            auto node_result = this->parse_operation(_rp, _ep);
            if (node_result._tag == Result<OperationSyntax, ParserError>::Error)
            {
                if (node_result._Error._tag == ParserError::InvalidSyntax)
                    return Result<ActionSyntax, ParserError> { ._tag = Result<ActionSyntax, ParserError>::Error, ._Error = node_result._Error };
            }
            else
            {
                auto node = node_result._Ok;
                return Result<ActionSyntax, ParserError> { ._tag = Result<ActionSyntax, ParserError>::Ok, ._Ok = 
                    ActionSyntax(OperationSyntax(node))
                };
            }
        }
        {
            auto node_result = this->parse_set(_rp, _ep);
            if (node_result._tag == Result<SetSyntax, ParserError>::Error)
            {
                if (node_result._Error._tag == ParserError::InvalidSyntax)
                    return Result<ActionSyntax, ParserError> { ._tag = Result<ActionSyntax, ParserError>::Error, ._Error = node_result._Error };
            }
            else
            {
                auto node = node_result._Ok;
                return Result<ActionSyntax, ParserError> { ._tag = Result<ActionSyntax, ParserError>::Ok, ._Ok = 
                    ActionSyntax(SetSyntax(node))
                };
            }
        }
        return Result<ActionSyntax, ParserError> { ._tag = Result<ActionSyntax, ParserError>::Error, ._Error = ParserError(OtherSyntax()) };
    }

    Result<OperationSyntax, ParserError> parse_operation(Page* _rp, Page* _ep) {
        auto start = this->lexer.previous_position;

        auto operands_start = this->lexer.position;
        auto operands_result = this->parse_operand_list(_rp, _ep);
        if (operands_result._tag == Result<Vector<OperandSyntax>, ParserError>::Error)
        {
            return Result<OperationSyntax, ParserError> { ._tag = Result<OperationSyntax, ParserError>::Error, ._Error = operands_result._Error };
        }

        auto operands = operands_result._Ok;

        auto end = this->lexer.position;

        auto ret = OperationSyntax(start, end, operands);

        return Result<OperationSyntax, ParserError> { ._tag = Result<OperationSyntax, ParserError>::Ok, ._Ok = ret };
    }

    Result<SetSyntax, ParserError> parse_set(Page* _rp, Page* _ep) {
        auto start = this->lexer.previous_position;

        auto start_set_1 = this->lexer.previous_position;
        auto success_set_1 = this->lexer.parse_keyword(_rp, *this->keywords_index[34]);
        if (!success_set_1) {
            return Result<SetSyntax, ParserError> { ._tag = Result<SetSyntax, ParserError>::Error, ._Error = ParserError(OtherSyntax()) };
        }

        auto target_start = this->lexer.position;
        auto target_result = this->parse_operand_list(_rp, _ep);
        if (target_result._tag == Result<Vector<OperandSyntax>, ParserError>::Error)
        {
            switch (target_result._Error._tag) {
                case ParserError::OtherSyntax:
                    return Result<SetSyntax, ParserError> { ._tag = Result<SetSyntax, ParserError>::Error, ._Error = ParserError(InvalidSyntax(target_start, lexer.position, String(_ep, "a valid Operand syntax"))) };
                case ParserError::InvalidSyntax:
                    return Result<SetSyntax, ParserError> { ._tag = Result<SetSyntax, ParserError>::Error, ._Error = target_result._Error };
            }
        }

        auto target = target_result._Ok;

        auto start_colon_3 = this->lexer.previous_position;
        auto success_colon_3 = this->lexer.parse_colon(_rp);
        if (!success_colon_3) {
            return Result<SetSyntax, ParserError> { ._tag = Result<SetSyntax, ParserError>::Error, ._Error = ParserError(InvalidSyntax(start_colon_3, lexer.position, String(_ep, "a colon or a line feed"))) };        }

        auto source_start = this->lexer.position;
        auto source_result = this->parse_operand_list(_rp, _ep);
        if (source_result._tag == Result<Vector<OperandSyntax>, ParserError>::Error)
        {
            switch (source_result._Error._tag) {
                case ParserError::OtherSyntax:
                    return Result<SetSyntax, ParserError> { ._tag = Result<SetSyntax, ParserError>::Error, ._Error = ParserError(InvalidSyntax(source_start, lexer.position, String(_ep, "a valid Operand syntax"))) };
                case ParserError::InvalidSyntax:
                    return Result<SetSyntax, ParserError> { ._tag = Result<SetSyntax, ParserError>::Error, ._Error = source_result._Error };
            }
        }

        auto source = source_result._Ok;

        auto end = this->lexer.position;

        auto ret = SetSyntax(start, end, target, source);

        return Result<SetSyntax, ParserError> { ._tag = Result<SetSyntax, ParserError>::Ok, ._Ok = ret };
    }

    Result<SizeOfSyntax, ParserError> parse_sizeof(Page* _rp, Page* _ep) {
        auto start = this->lexer.previous_position;

        auto start_sizeof_1 = this->lexer.previous_position;
        auto success_sizeof_1 = this->lexer.parse_keyword(_rp, *this->keywords_index[35]);
        if (!success_sizeof_1) {
            return Result<SizeOfSyntax, ParserError> { ._tag = Result<SizeOfSyntax, ParserError>::Error, ._Error = ParserError(OtherSyntax()) };
        }

        auto type_start = this->lexer.position;
        auto type_result = this->parse_type(_rp, _ep);
        if (type_result._tag == Result<TypeSyntax, ParserError>::Error)
        {
            switch (type_result._Error._tag) {
                case ParserError::OtherSyntax:
                    return Result<SizeOfSyntax, ParserError> { ._tag = Result<SizeOfSyntax, ParserError>::Error, ._Error = ParserError(InvalidSyntax(type_start, lexer.position, String(_ep, "a valid Type syntax"))) };
                case ParserError::InvalidSyntax:
                    return Result<SizeOfSyntax, ParserError> { ._tag = Result<SizeOfSyntax, ParserError>::Error, ._Error = type_result._Error };
            }
        }

        auto type = type_result._Ok;

        auto end = this->lexer.position;

        auto ret = SizeOfSyntax(start, end, type);

        return Result<SizeOfSyntax, ParserError> { ._tag = Result<SizeOfSyntax, ParserError>::Ok, ._Ok = ret };
    }

    Result<Vector<TypeSyntax>*, ParserError> parse_type_list(Page* _rp, Page* _ep) {
        Region _r;
        List<TypeSyntax>& list = *new(alignof(List<TypeSyntax>), _r.get_page()) List<TypeSyntax>();;
        while(true) {
            auto node_result = this->parse_type(_rp, _ep);
            if ((node_result._tag == Result<TypeSyntax, ParserError>::Error) && (node_result._Error._tag == ParserError::InvalidSyntax))
                return Result<Vector<TypeSyntax>*, ParserError> { ._tag = Result<Vector<TypeSyntax>*, ParserError>::Error, ._Error = node_result._Error };
            if (node_result._tag == Result<TypeSyntax, ParserError>::Ok) {
                auto node = node_result._Ok;
                list.add(node);
            } else {
                if ((list.count() == 0) && (node_result._tag == Result<TypeSyntax, ParserError>::Error) && (node_result._Error._tag == ParserError::OtherSyntax))
                    return Result<Vector<TypeSyntax>*, ParserError> { ._tag = Result<Vector<TypeSyntax>*, ParserError>::Error, ._Error = node_result._Error };
                break;
            }
        }

        if (list.count() == 0)
            return Result<Vector<TypeSyntax>*, ParserError> { ._tag = Result<Vector<TypeSyntax>*, ParserError>::Ok, ._Ok = nullptr };
        
        return Result<Vector<TypeSyntax>*, ParserError> {
            ._tag = Result<Vector<TypeSyntax>*, ParserError>::Ok,
            ._Ok = new(alignof(Vector<TypeSyntax>), _rp) Vector<TypeSyntax>(_rp, list) };
    }

    Result<TypeSyntax, ParserError> parse_type(Page* _rp, Page* _ep) {
        auto start = this->lexer.previous_position;

        auto name_start = this->lexer.position;
        auto name_result = this->parse_name(_rp, _ep);
        if (name_result._tag == Result<NameSyntax, ParserError>::Error)
        {
            return Result<TypeSyntax, ParserError> { ._tag = Result<TypeSyntax, ParserError>::Error, ._Error = name_result._Error };
        }

        auto name = name_result._Ok;

        auto generics_start = this->lexer.position;
        auto generics_result = this->parse_genericarguments(_rp, _ep);
        if (generics_result._tag == Result<GenericArgumentsSyntax, ParserError>::Error)
        {
            switch (generics_result._Error._tag) {
                case ParserError::OtherSyntax:
                    break;
                case ParserError::InvalidSyntax:
                    return Result<TypeSyntax, ParserError> { ._tag = Result<TypeSyntax, ParserError>::Error, ._Error = generics_result._Error };
            }
        }

        GenericArgumentsSyntax* generics = generics_result._tag == Result<GenericArgumentsSyntax, ParserError>::Error ? nullptr : new(alignof(GenericArgumentsSyntax), _rp) GenericArgumentsSyntax(generics_result._Ok);

        auto optional_start = this->lexer.position;
        auto optional_result = this->parse_optional(_rp, _ep);
        if (optional_result._tag == Result<OptionalSyntax, ParserError>::Error)
        {
            switch (optional_result._Error._tag) {
                case ParserError::OtherSyntax:
                    break;
                case ParserError::InvalidSyntax:
                    return Result<TypeSyntax, ParserError> { ._tag = Result<TypeSyntax, ParserError>::Error, ._Error = optional_result._Error };
            }
        }

        OptionalSyntax* optional = optional_result._tag == Result<OptionalSyntax, ParserError>::Error ? nullptr : new(alignof(OptionalSyntax), _rp) OptionalSyntax(optional_result._Ok);

        auto lifetime_start = this->lexer.position;
        auto lifetime_result = this->parse_lifetime(_rp, _ep);
        if (lifetime_result._tag == Result<LifetimeSyntax, ParserError>::Error)
        {
            switch (lifetime_result._Error._tag) {
                case ParserError::OtherSyntax:
                    break;
                case ParserError::InvalidSyntax:
                    return Result<TypeSyntax, ParserError> { ._tag = Result<TypeSyntax, ParserError>::Error, ._Error = lifetime_result._Error };
            }
        }

        LifetimeSyntax* lifetime = lifetime_result._tag == Result<LifetimeSyntax, ParserError>::Error ? nullptr : new(alignof(LifetimeSyntax), _rp) LifetimeSyntax(lifetime_result._Ok);

        auto end = this->lexer.position;

        auto ret = TypeSyntax(start, end, name, generics, optional, lifetime);

        return Result<TypeSyntax, ParserError> { ._tag = Result<TypeSyntax, ParserError>::Ok, ._Ok = ret };
    }

    Result<NameSyntax, ParserError> parse_name(Page* _rp, Page* _ep) {
        auto start = this->lexer.previous_position;

        auto start_name = this->lexer.previous_position;
        auto name = this->lexer.parse_identifier(_rp, this->keywords);
        if (name == nullptr)
        {
            return Result<NameSyntax, ParserError> { ._tag = Result<NameSyntax, ParserError>::Error, ._Error = ParserError(OtherSyntax()) };

        }

        auto extensions_start = this->lexer.position;
        auto extensions_result = this->parse_extension_list(_rp, _ep);
        if (extensions_result._tag == Result<Vector<ExtensionSyntax>, ParserError>::Error)
        {
            switch (extensions_result._Error._tag) {
                case ParserError::OtherSyntax:
                    break;
                case ParserError::InvalidSyntax:
                    return Result<NameSyntax, ParserError> { ._tag = Result<NameSyntax, ParserError>::Error, ._Error = extensions_result._Error };
            }
        }

        auto extensions = extensions_result._tag == Result<Vector<ExtensionSyntax>, ParserError>::Error ? nullptr : extensions_result._Ok;

        auto end = this->lexer.position;

        auto ret = NameSyntax(start, end, *name, extensions);

        return Result<NameSyntax, ParserError> { ._tag = Result<NameSyntax, ParserError>::Ok, ._Ok = ret };
    }

    Result<Vector<ExtensionSyntax>*, ParserError> parse_extension_list(Page* _rp, Page* _ep) {
        Region _r;
        List<ExtensionSyntax>& list = *new(alignof(List<ExtensionSyntax>), _r.get_page()) List<ExtensionSyntax>();;
        while(true) {
            auto node_result = this->parse_extension(_rp, _ep);
            if ((node_result._tag == Result<ExtensionSyntax, ParserError>::Error) && (node_result._Error._tag == ParserError::InvalidSyntax))
                return Result<Vector<ExtensionSyntax>*, ParserError> { ._tag = Result<Vector<ExtensionSyntax>*, ParserError>::Error, ._Error = node_result._Error };
            if (node_result._tag == Result<ExtensionSyntax, ParserError>::Ok) {
                auto node = node_result._Ok;
                list.add(node);
            } else {
                if ((list.count() == 0) && (node_result._tag == Result<ExtensionSyntax, ParserError>::Error) && (node_result._Error._tag == ParserError::OtherSyntax))
                    return Result<Vector<ExtensionSyntax>*, ParserError> { ._tag = Result<Vector<ExtensionSyntax>*, ParserError>::Error, ._Error = node_result._Error };
                break;
            }
        }

        if (list.count() == 0)
            return Result<Vector<ExtensionSyntax>*, ParserError> { ._tag = Result<Vector<ExtensionSyntax>*, ParserError>::Ok, ._Ok = nullptr };
        
        return Result<Vector<ExtensionSyntax>*, ParserError> {
            ._tag = Result<Vector<ExtensionSyntax>*, ParserError>::Ok,
            ._Ok = new(alignof(Vector<ExtensionSyntax>), _rp) Vector<ExtensionSyntax>(_rp, list) };
    }

    Result<ExtensionSyntax, ParserError> parse_extension(Page* _rp, Page* _ep) {
        auto start = this->lexer.previous_position;

        auto start_dot_1 = this->lexer.previous_position;
        auto success_dot_1 = this->lexer.parse_punctuation('.');
        if (!success_dot_1) {
            return Result<ExtensionSyntax, ParserError> { ._tag = Result<ExtensionSyntax, ParserError>::Error, ._Error = ParserError(OtherSyntax()) };
        }

        auto start_name = this->lexer.previous_position;
        auto name = this->lexer.parse_identifier(_rp, this->keywords);
        if (name == nullptr)
        {
            return Result<ExtensionSyntax, ParserError> { ._tag = Result<ExtensionSyntax, ParserError>::Error, ._Error = ParserError(InvalidSyntax(start_name, lexer.position, String(_ep, "an identifier"))) };
        }

        auto end = this->lexer.position;

        auto ret = ExtensionSyntax(start, end, *name);

        return Result<ExtensionSyntax, ParserError> { ._tag = Result<ExtensionSyntax, ParserError>::Ok, ._Ok = ret };
    }

    Result<LifetimeSyntax, ParserError> parse_lifetime(Page* _rp, Page* _ep) {
        {
            auto node_result = this->parse_call(_rp, _ep);
            if (node_result._tag == Result<CallSyntax, ParserError>::Error)
            {
                if (node_result._Error._tag == ParserError::InvalidSyntax)
                    return Result<LifetimeSyntax, ParserError> { ._tag = Result<LifetimeSyntax, ParserError>::Error, ._Error = node_result._Error };
            }
            else
            {
                auto node = node_result._Ok;
                return Result<LifetimeSyntax, ParserError> { ._tag = Result<LifetimeSyntax, ParserError>::Ok, ._Ok = 
                    LifetimeSyntax(CallSyntax(node))
                };
            }
        }
        {
            auto node_result = this->parse_local(_rp, _ep);
            if (node_result._tag == Result<LocalSyntax, ParserError>::Error)
            {
                if (node_result._Error._tag == ParserError::InvalidSyntax)
                    return Result<LifetimeSyntax, ParserError> { ._tag = Result<LifetimeSyntax, ParserError>::Error, ._Error = node_result._Error };
            }
            else
            {
                auto node = node_result._Ok;
                return Result<LifetimeSyntax, ParserError> { ._tag = Result<LifetimeSyntax, ParserError>::Ok, ._Ok = 
                    LifetimeSyntax(LocalSyntax(node))
                };
            }
        }
        {
            auto node_result = this->parse_reference(_rp, _ep);
            if (node_result._tag == Result<ReferenceSyntax, ParserError>::Error)
            {
                if (node_result._Error._tag == ParserError::InvalidSyntax)
                    return Result<LifetimeSyntax, ParserError> { ._tag = Result<LifetimeSyntax, ParserError>::Error, ._Error = node_result._Error };
            }
            else
            {
                auto node = node_result._Ok;
                return Result<LifetimeSyntax, ParserError> { ._tag = Result<LifetimeSyntax, ParserError>::Ok, ._Ok = 
                    LifetimeSyntax(ReferenceSyntax(node))
                };
            }
        }
        {
            auto node_result = this->parse_thrown(_rp, _ep);
            if (node_result._tag == Result<ThrownSyntax, ParserError>::Error)
            {
                if (node_result._Error._tag == ParserError::InvalidSyntax)
                    return Result<LifetimeSyntax, ParserError> { ._tag = Result<LifetimeSyntax, ParserError>::Error, ._Error = node_result._Error };
            }
            else
            {
                auto node = node_result._Ok;
                return Result<LifetimeSyntax, ParserError> { ._tag = Result<LifetimeSyntax, ParserError>::Ok, ._Ok = 
                    LifetimeSyntax(ThrownSyntax(node))
                };
            }
        }
        return Result<LifetimeSyntax, ParserError> { ._tag = Result<LifetimeSyntax, ParserError>::Error, ._Error = ParserError(OtherSyntax()) };
    }

    Result<CallSyntax, ParserError> parse_call(Page* _rp, Page* _ep) {
        auto start = this->lexer.previous_position;

        auto start_hash_1 = this->lexer.previous_position;
        auto success_hash_1 = this->lexer.parse_punctuation('#');
        if (!success_hash_1) {
            return Result<CallSyntax, ParserError> { ._tag = Result<CallSyntax, ParserError>::Error, ._Error = ParserError(OtherSyntax()) };
        }

        auto end = this->lexer.position;

        auto ret = CallSyntax(start, end);

        return Result<CallSyntax, ParserError> { ._tag = Result<CallSyntax, ParserError>::Ok, ._Ok = ret };
    }

    Result<LocalSyntax, ParserError> parse_local(Page* _rp, Page* _ep) {
        auto start = this->lexer.previous_position;

        auto start_dollar_1 = this->lexer.previous_position;
        auto success_dollar_1 = this->lexer.parse_punctuation('$');
        if (!success_dollar_1) {
            return Result<LocalSyntax, ParserError> { ._tag = Result<LocalSyntax, ParserError>::Error, ._Error = ParserError(OtherSyntax()) };
        }

        auto end = this->lexer.position;

        auto ret = LocalSyntax(start, end);

        return Result<LocalSyntax, ParserError> { ._tag = Result<LocalSyntax, ParserError>::Ok, ._Ok = ret };
    }

    Result<ReferenceSyntax, ParserError> parse_reference(Page* _rp, Page* _ep) {
        auto start = this->lexer.previous_position;

        auto start_caret_1 = this->lexer.previous_position;
        auto success_caret_1 = this->lexer.parse_punctuation('^');
        if (!success_caret_1) {
            return Result<ReferenceSyntax, ParserError> { ._tag = Result<ReferenceSyntax, ParserError>::Error, ._Error = ParserError(OtherSyntax()) };
        }

        auto start_location = this->lexer.previous_position;
        auto location = this->lexer.parse_identifier(_rp, this->keywords);
        if (location == nullptr)
        {
            return Result<ReferenceSyntax, ParserError> { ._tag = Result<ReferenceSyntax, ParserError>::Error, ._Error = ParserError(InvalidSyntax(start_location, lexer.position, String(_ep, "an identifier"))) };
        }

        auto end = this->lexer.position;

        auto ret = ReferenceSyntax(start, end, *location);

        return Result<ReferenceSyntax, ParserError> { ._tag = Result<ReferenceSyntax, ParserError>::Ok, ._Ok = ret };
    }

    Result<ThrownSyntax, ParserError> parse_thrown(Page* _rp, Page* _ep) {
        auto start = this->lexer.previous_position;

        auto start_exclamation_1 = this->lexer.previous_position;
        auto success_exclamation_1 = this->lexer.parse_punctuation('!');
        if (!success_exclamation_1) {
            return Result<ThrownSyntax, ParserError> { ._tag = Result<ThrownSyntax, ParserError>::Error, ._Error = ParserError(OtherSyntax()) };
        }

        auto end = this->lexer.position;

        auto ret = ThrownSyntax(start, end);

        return Result<ThrownSyntax, ParserError> { ._tag = Result<ThrownSyntax, ParserError>::Ok, ._Ok = ret };
    }

    bool is_at_end() {
        return this->lexer.is_at_end();
    }
};

}
}
