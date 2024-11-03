#ifndef _Parser_h
#define _Parser_h
#include "../../scaly.h"
using namespace scaly;
using namespace scaly::memory;

using namespace scaly::containers;

struct IdentifierLiteral : Object {
    String name;

    IdentifierLiteral(String name);
};

struct StringLiteral : Object {
    String value;

    StringLiteral(String value);
};

struct CharacterLiteral : Object {
    String value;

    CharacterLiteral(String value);
};

struct FragmentLiteral : Object {
    String value;

    FragmentLiteral(String value);
};

struct IntegerLiteral : Object {
    String value;

    IntegerLiteral(String value);
};

struct BooleanLiteral : Object {
    bool value;

    BooleanLiteral(bool value);
};

struct FloatingPointLiteral : Object {
    String value;

    FloatingPointLiteral(String value);
};

struct HexLiteral : Object {
    String value;

    HexLiteral(String value);
};

struct Literal {
    Literal(StringLiteral);
    Literal(CharacterLiteral);
    Literal(FragmentLiteral);
    Literal(IntegerLiteral);
    Literal(BooleanLiteral);
    Literal(FloatingPointLiteral);
    Literal(HexLiteral);
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
struct ThrownSyntax : Object {
    size_t start;
    size_t end;

    ThrownSyntax(size_t start, size_t end);
};

struct ReferenceSyntax : Object {
    size_t start;
    size_t end;
    String location;

    ReferenceSyntax(size_t start, size_t end, String location);
};

struct LocalSyntax : Object {
    size_t start;
    size_t end;

    LocalSyntax(size_t start, size_t end);
};

struct CallSyntax : Object {
    size_t start;
    size_t end;

    CallSyntax(size_t start, size_t end);
};

struct LifetimeSyntax {
    LifetimeSyntax(CallSyntax);
    LifetimeSyntax(LocalSyntax);
    LifetimeSyntax(ReferenceSyntax);
    LifetimeSyntax(ThrownSyntax);
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
    size_t start;
    size_t end;
    String name;

    ExtensionSyntax(size_t start, size_t end, String name);
};

struct NameSyntax : Object {
    size_t start;
    size_t end;
    String name;
    Vector<ExtensionSyntax>* extensions;

    NameSyntax(size_t start, size_t end, String name, Vector<ExtensionSyntax>* extensions);
};

struct TypeSyntax : Object {
    size_t start;
    size_t end;
    NameSyntax name;
    GenericArgumentsSyntax* generics;
    OptionalSyntax* optional;
    LifetimeSyntax* lifetime;

    TypeSyntax(size_t start, size_t end, NameSyntax name, GenericArgumentsSyntax* generics, OptionalSyntax* optional, LifetimeSyntax* lifetime);
};

struct SizeOfSyntax : Object {
    size_t start;
    size_t end;
    TypeSyntax type;

    SizeOfSyntax(size_t start, size_t end, TypeSyntax type);
};

struct SetSyntax : Object {
    size_t start;
    size_t end;
    Vector<OperandSyntax> target;
    Vector<OperandSyntax> source;

    SetSyntax(size_t start, size_t end, Vector<OperandSyntax> target, Vector<OperandSyntax> source);
};

struct OperationSyntax : Object {
    size_t start;
    size_t end;
    Vector<OperandSyntax> operands;

    OperationSyntax(size_t start, size_t end, Vector<OperandSyntax> operands);
};

struct ActionSyntax {
    ActionSyntax(OperationSyntax);
    ActionSyntax(SetSyntax);
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
    size_t start;
    size_t end;
    LabelSyntax* name;
    ActionSyntax action;

    RepeatSyntax(size_t start, size_t end, LabelSyntax* name, ActionSyntax action);
};

struct LabelSyntax : Object {
    size_t start;
    size_t end;
    String name;

    LabelSyntax(size_t start, size_t end, String name);
};

struct ForSyntax : Object {
    size_t start;
    size_t end;
    String variable;
    TypeAnnotationSyntax* annotation;
    Vector<OperandSyntax> operation;
    LabelSyntax* name;
    ActionSyntax action;

    ForSyntax(size_t start, size_t end, String variable, TypeAnnotationSyntax* annotation, Vector<OperandSyntax> operation, LabelSyntax* name, ActionSyntax action);
};

struct LambdaSyntax : Object {
    size_t start;
    size_t end;
    Vector<OperandSyntax> input;
    ActionSyntax block;

    LambdaSyntax(size_t start, size_t end, Vector<OperandSyntax> input, ActionSyntax block);
};

struct ThrowSyntax : Object {
    size_t start;
    size_t end;
    Vector<OperandSyntax>* result;

    ThrowSyntax(size_t start, size_t end, Vector<OperandSyntax>* result);
};

struct ReturnSyntax : Object {
    size_t start;
    size_t end;
    Vector<OperandSyntax>* result;

    ReturnSyntax(size_t start, size_t end, Vector<OperandSyntax>* result);
};

struct LoopSyntax : Object {
    size_t start;
    size_t end;
    String name;

    LoopSyntax(size_t start, size_t end, String name);
};

struct BreakSyntax : Object {
    size_t start;
    size_t end;
    LoopSyntax* name;
    Vector<OperandSyntax>* result;

    BreakSyntax(size_t start, size_t end, LoopSyntax* name, Vector<OperandSyntax>* result);
};

struct ContinueSyntax : Object {
    size_t start;
    size_t end;
    LoopSyntax* name;

    ContinueSyntax(size_t start, size_t end, LoopSyntax* name);
};

struct TypeAnnotationSyntax : Object {
    size_t start;
    size_t end;
    TypeSyntax type;

    TypeAnnotationSyntax(size_t start, size_t end, TypeSyntax type);
};

struct PropertySyntax : Object {
    size_t start;
    size_t end;
    String name;
    TypeAnnotationSyntax annotation;
    InitializerSyntax* initializer;
    Vector<AttributeSyntax>* attributes;

    PropertySyntax(size_t start, size_t end, String name, TypeAnnotationSyntax annotation, InitializerSyntax* initializer, Vector<AttributeSyntax>* attributes);
};

struct FieldSyntax : Object {
    size_t start;
    size_t end;
    PropertySyntax property;

    FieldSyntax(size_t start, size_t end, PropertySyntax property);
};

struct PartSyntax {
    PartSyntax(FieldSyntax);
    PartSyntax(PropertySyntax);
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
    size_t start;
    size_t end;
    Vector<PartSyntax>* parts;

    StructureSyntax(size_t start, size_t end, Vector<PartSyntax>* parts);
};

struct ArraySyntax : Object {
    size_t start;
    size_t end;
    Vector<TypeSyntax>* members;

    ArraySyntax(size_t start, size_t end, Vector<TypeSyntax>* members);
};

struct BindingSpecSyntax {
    BindingSpecSyntax(StructureSyntax);
    BindingSpecSyntax(TypeSyntax);
    BindingSpecSyntax(ArraySyntax);
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
    size_t start;
    size_t end;
    BindingSpecSyntax spec;

    BindingAnnotationSyntax(size_t start, size_t end, BindingSpecSyntax spec);
};

struct BindingSyntax : Object {
    size_t start;
    size_t end;
    String name;
    BindingAnnotationSyntax* annotation;
    Vector<OperandSyntax> operation;

    BindingSyntax(size_t start, size_t end, String name, BindingAnnotationSyntax* annotation, Vector<OperandSyntax> operation);
};

struct MutableSyntax : Object {
    size_t start;
    size_t end;
    BindingSyntax binding;

    MutableSyntax(size_t start, size_t end, BindingSyntax binding);
};

struct VarSyntax : Object {
    size_t start;
    size_t end;
    BindingSyntax binding;

    VarSyntax(size_t start, size_t end, BindingSyntax binding);
};

struct LetSyntax : Object {
    size_t start;
    size_t end;
    BindingSyntax binding;

    LetSyntax(size_t start, size_t end, BindingSyntax binding);
};

struct WhenSyntax : Object {
    size_t start;
    size_t end;
    String name;
    NameSyntax variant;
    ActionSyntax action;

    WhenSyntax(size_t start, size_t end, String name, NameSyntax variant, ActionSyntax action);
};

struct ConditionSyntax {
    ConditionSyntax(OperationSyntax);
    ConditionSyntax(LetSyntax);
    enum {
        Operation,
        Let,
    } _tag;
    union {
        OperationSyntax _Operation;
        LetSyntax _Let;
    };
};
struct TrySyntax : Object {
    size_t start;
    size_t end;
    ConditionSyntax condition;
    Vector<WhenSyntax>* cases;
    DefaultSyntax* dropper;

    TrySyntax(size_t start, size_t end, ConditionSyntax condition, Vector<WhenSyntax>* cases, DefaultSyntax* dropper);
};

struct ChooseSyntax : Object {
    size_t start;
    size_t end;
    Vector<OperandSyntax> condition;
    Vector<WhenSyntax>* cases;
    DefaultSyntax* alternative;

    ChooseSyntax(size_t start, size_t end, Vector<OperandSyntax> condition, Vector<WhenSyntax>* cases, DefaultSyntax* alternative);
};

struct WhileSyntax : Object {
    size_t start;
    size_t end;
    ConditionSyntax condition;
    LabelSyntax* name;
    ActionSyntax action;

    WhileSyntax(size_t start, size_t end, ConditionSyntax condition, LabelSyntax* name, ActionSyntax action);
};

struct CommandSyntax {
    CommandSyntax(OperationSyntax);
    CommandSyntax(LetSyntax);
    CommandSyntax(VarSyntax);
    CommandSyntax(MutableSyntax);
    CommandSyntax(SetSyntax);
    CommandSyntax(ContinueSyntax);
    CommandSyntax(BreakSyntax);
    CommandSyntax(ReturnSyntax);
    CommandSyntax(ThrowSyntax);
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
    size_t start;
    size_t end;
    CommandSyntax command;

    StatementSyntax(size_t start, size_t end, CommandSyntax command);
};

struct DefaultSyntax : Object {
    size_t start;
    size_t end;
    CommandSyntax alternative;

    DefaultSyntax(size_t start, size_t end, CommandSyntax alternative);
};

struct CaseSyntax : Object {
    size_t start;
    size_t end;
    Vector<OperandSyntax> condition;

    CaseSyntax(size_t start, size_t end, Vector<OperandSyntax> condition);
};

struct BranchSyntax : Object {
    size_t start;
    size_t end;
    Vector<CaseSyntax> cases;
    StatementSyntax consequent;

    BranchSyntax(size_t start, size_t end, Vector<CaseSyntax> cases, StatementSyntax consequent);
};

struct MatchSyntax : Object {
    size_t start;
    size_t end;
    Vector<OperandSyntax> scrutinee;
    Vector<BranchSyntax> branches;
    DefaultSyntax* alternative;

    MatchSyntax(size_t start, size_t end, Vector<OperandSyntax> scrutinee, Vector<BranchSyntax> branches, DefaultSyntax* alternative);
};

struct ElseSyntax : Object {
    size_t start;
    size_t end;
    CommandSyntax alternative;

    ElseSyntax(size_t start, size_t end, CommandSyntax alternative);
};

struct IfSyntax : Object {
    size_t start;
    size_t end;
    Vector<OperandSyntax> condition;
    CommandSyntax consequent;
    ElseSyntax* alternative;

    IfSyntax(size_t start, size_t end, Vector<OperandSyntax> condition, CommandSyntax consequent, ElseSyntax* alternative);
};

struct BlockSyntax : Object {
    size_t start;
    size_t end;
    Vector<UseSyntax>* uses;
    Vector<StatementSyntax>* statements;

    BlockSyntax(size_t start, size_t end, Vector<UseSyntax>* uses, Vector<StatementSyntax>* statements);
};

struct ElementSyntax : Object {
    size_t start;
    size_t end;
    Vector<OperandSyntax> operation;
    Vector<AttributeSyntax>* attributes;

    ElementSyntax(size_t start, size_t end, Vector<OperandSyntax> operation, Vector<AttributeSyntax>* attributes);
};

struct VectorSyntax : Object {
    size_t start;
    size_t end;
    Vector<ElementSyntax> elements;
    LifetimeSyntax* lifetime;

    VectorSyntax(size_t start, size_t end, Vector<ElementSyntax> elements, LifetimeSyntax* lifetime);
};

struct ValueSyntax : Object {
    size_t start;
    size_t end;
    Vector<OperandSyntax> value;
    Vector<AttributeSyntax>* attributes;

    ValueSyntax(size_t start, size_t end, Vector<OperandSyntax> value, Vector<AttributeSyntax>* attributes);
};

struct ComponentSyntax : Object {
    size_t start;
    size_t end;
    Vector<OperandSyntax> operands;
    Vector<AttributeSyntax>* attributes;
    ValueSyntax* value;

    ComponentSyntax(size_t start, size_t end, Vector<OperandSyntax> operands, Vector<AttributeSyntax>* attributes, ValueSyntax* value);
};

struct ObjectSyntax : Object {
    size_t start;
    size_t end;
    Vector<ComponentSyntax>* components;

    ObjectSyntax(size_t start, size_t end, Vector<ComponentSyntax>* components);
};

struct LiteralSyntax : Object {
    size_t start;
    size_t end;
    Literal literal;

    LiteralSyntax(size_t start, size_t end, Literal literal);
};

struct ExpressionSyntax {
    ExpressionSyntax(LiteralSyntax);
    ExpressionSyntax(TypeSyntax);
    ExpressionSyntax(ObjectSyntax);
    ExpressionSyntax(VectorSyntax);
    ExpressionSyntax(BlockSyntax);
    ExpressionSyntax(IfSyntax);
    ExpressionSyntax(MatchSyntax);
    ExpressionSyntax(LambdaSyntax);
    ExpressionSyntax(ForSyntax);
    ExpressionSyntax(WhileSyntax);
    ExpressionSyntax(ChooseSyntax);
    ExpressionSyntax(TrySyntax);
    ExpressionSyntax(RepeatSyntax);
    ExpressionSyntax(SizeOfSyntax);
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
        Choose,
        Try,
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
        ChooseSyntax _Choose;
        TrySyntax _Try;
        RepeatSyntax _Repeat;
        SizeOfSyntax _SizeOf;
    };
};
struct MemberAccessSyntax : Object {
    size_t start;
    size_t end;
    NameSyntax name;

    MemberAccessSyntax(size_t start, size_t end, NameSyntax name);
};

struct OperandSyntax : Object {
    size_t start;
    size_t end;
    ExpressionSyntax expression;
    Vector<MemberAccessSyntax>* members;

    OperandSyntax(size_t start, size_t end, ExpressionSyntax expression, Vector<MemberAccessSyntax>* members);
};

struct InitializerSyntax : Object {
    size_t start;
    size_t end;
    Vector<OperandSyntax> operands;

    InitializerSyntax(size_t start, size_t end, Vector<OperandSyntax> operands);
};

struct PackageSyntax : Object {
    size_t start;
    size_t end;
    NameSyntax name;

    PackageSyntax(size_t start, size_t end, NameSyntax name);
};

struct ModuleSyntax : Object {
    size_t start;
    size_t end;
    String name;

    ModuleSyntax(size_t start, size_t end, String name);
};

struct ModelSyntax {
    ModelSyntax(LiteralSyntax);
    ModelSyntax(NameSyntax);
    ModelSyntax(ObjectSyntax);
    ModelSyntax(VectorSyntax);
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
    size_t start;
    size_t end;
    String name;
    ModelSyntax model;

    AttributeSyntax(size_t start, size_t end, String name, ModelSyntax model);
};

struct MacroSyntax : Object {
    size_t start;
    size_t end;
    String name;
    ModelSyntax model;
    Vector<OperandSyntax> rule;

    MacroSyntax(size_t start, size_t end, String name, ModelSyntax model, Vector<OperandSyntax> rule);
};

struct ExtendSyntax : Object {
    size_t start;
    size_t end;
    TypeSyntax type;

    ExtendSyntax(size_t start, size_t end, TypeSyntax type);
};

struct ExtendsSyntax : Object {
    size_t start;
    size_t end;
    Vector<ExtendSyntax>* extensions;

    ExtendsSyntax(size_t start, size_t end, Vector<ExtendSyntax>* extensions);
};

struct IntrinsicSyntax : Object {
    size_t start;
    size_t end;

    IntrinsicSyntax(size_t start, size_t end);
};

struct InstructionSyntax : Object {
    size_t start;
    size_t end;

    InstructionSyntax(size_t start, size_t end);
};

struct ExternSyntax : Object {
    size_t start;
    size_t end;

    ExternSyntax(size_t start, size_t end);
};

struct ImplementationSyntax {
    ImplementationSyntax(ActionSyntax);
    ImplementationSyntax(ExternSyntax);
    ImplementationSyntax(InstructionSyntax);
    ImplementationSyntax(IntrinsicSyntax);
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
    size_t start;
    size_t end;
    GenericArgumentsSyntax* generics;
    LifetimeSyntax* lifetime;
    ParameterSetSyntax* parameters;
    Vector<AttributeSyntax>* attributes;
    ReturnsSyntax* returns_;
    ThrowsSyntax* throws_;
    ImplementationSyntax implementation;

    RoutineSyntax(size_t start, size_t end, GenericArgumentsSyntax* generics, LifetimeSyntax* lifetime, ParameterSetSyntax* parameters, Vector<AttributeSyntax>* attributes, ReturnsSyntax* returns_, ThrowsSyntax* throws_, ImplementationSyntax implementation);
};

struct NamedSyntax : Object {
    size_t start;
    size_t end;
    String name;
    RoutineSyntax routine;

    NamedSyntax(size_t start, size_t end, String name, RoutineSyntax routine);
};

struct TargetSyntax {
    TargetSyntax(NamedSyntax);
    TargetSyntax(RoutineSyntax);
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
    size_t start;
    size_t end;
    TargetSyntax target;

    OperatorSyntax(size_t start, size_t end, TargetSyntax target);
};

struct ProcedureSyntax : Object {
    size_t start;
    size_t end;
    TargetSyntax target;

    ProcedureSyntax(size_t start, size_t end, TargetSyntax target);
};

struct FunctionSyntax : Object {
    size_t start;
    size_t end;
    TargetSyntax target;

    FunctionSyntax(size_t start, size_t end, TargetSyntax target);
};

struct DeInitSyntax : Object {
    size_t start;
    size_t end;
    ActionSyntax action;

    DeInitSyntax(size_t start, size_t end, ActionSyntax action);
};

struct InitSyntax : Object {
    size_t start;
    size_t end;
    ParameterSetSyntax* parameters;
    ActionSyntax action;

    InitSyntax(size_t start, size_t end, ParameterSetSyntax* parameters, ActionSyntax action);
};

struct MethodSyntax {
    MethodSyntax(FunctionSyntax);
    MethodSyntax(ProcedureSyntax);
    MethodSyntax(OperatorSyntax);
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
    size_t start;
    size_t end;
    NameSyntax name;
    ExtendsSyntax* extension;
    Vector<AttributeSyntax>* attributes;
    Vector<UseSyntax>* uses;
    Vector<MethodSyntax>* functions;

    TraitSyntax(size_t start, size_t end, NameSyntax name, ExtendsSyntax* extension, Vector<AttributeSyntax>* attributes, Vector<UseSyntax>* uses, Vector<MethodSyntax>* functions);
};

struct ImplementSyntax : Object {
    size_t start;
    size_t end;
    TypeSyntax type;
    Vector<AttributeSyntax>* attributes;
    Vector<UseSyntax>* uses;
    Vector<MethodSyntax>* methods;

    ImplementSyntax(size_t start, size_t end, TypeSyntax type, Vector<AttributeSyntax>* attributes, Vector<UseSyntax>* uses, Vector<MethodSyntax>* methods);
};

struct UseSyntax : Object {
    size_t start;
    size_t end;
    NameSyntax name;

    UseSyntax(size_t start, size_t end, NameSyntax name);
};

struct ThrowsSyntax : Object {
    size_t start;
    size_t end;
    TypeSyntax type;
    Vector<AttributeSyntax>* attributes;

    ThrowsSyntax(size_t start, size_t end, TypeSyntax type, Vector<AttributeSyntax>* attributes);
};

struct ReturnsSyntax : Object {
    size_t start;
    size_t end;
    TypeSyntax type;
    Vector<AttributeSyntax>* attributes;

    ReturnsSyntax(size_t start, size_t end, TypeSyntax type, Vector<AttributeSyntax>* attributes);
};

struct ItemSyntax : Object {
    size_t start;
    size_t end;
    String name;
    TypeAnnotationSyntax* annotation;
    Vector<AttributeSyntax>* attributes;

    ItemSyntax(size_t start, size_t end, String name, TypeAnnotationSyntax* annotation, Vector<AttributeSyntax>* attributes);
};

struct ParametersSyntax : Object {
    size_t start;
    size_t end;
    Vector<ItemSyntax>* items;

    ParametersSyntax(size_t start, size_t end, Vector<ItemSyntax>* items);
};

struct ParameterSetSyntax {
    ParameterSetSyntax(ParametersSyntax);
    ParameterSetSyntax(TypeSyntax);
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
    size_t start;
    size_t end;

    OptionalSyntax(size_t start, size_t end);
};

struct GenericArgumentSyntax : Object {
    size_t start;
    size_t end;
    TypeSyntax type;

    GenericArgumentSyntax(size_t start, size_t end, TypeSyntax type);
};

struct GenericArgumentsSyntax : Object {
    size_t start;
    size_t end;
    Vector<GenericArgumentSyntax>* generics;

    GenericArgumentsSyntax(size_t start, size_t end, Vector<GenericArgumentSyntax>* generics);
};

struct DelegateSyntax : Object {
    size_t start;
    size_t end;
    ParameterSetSyntax* parameters;
    Vector<AttributeSyntax>* attributes;
    ReturnsSyntax* result;
    ThrowsSyntax* error;

    DelegateSyntax(size_t start, size_t end, ParameterSetSyntax* parameters, Vector<AttributeSyntax>* attributes, ReturnsSyntax* result, ThrowsSyntax* error);
};

struct ConstantSyntax : Object {
    size_t start;
    size_t end;
    TypeSyntax type;
    Vector<OperandSyntax> operation;

    ConstantSyntax(size_t start, size_t end, TypeSyntax type, Vector<OperandSyntax> operation);
};

struct VariantSyntax : Object {
    size_t start;
    size_t end;
    String name;
    Vector<AttributeSyntax>* attributes;
    TypeAnnotationSyntax* annotation;

    VariantSyntax(size_t start, size_t end, String name, Vector<AttributeSyntax>* attributes, TypeAnnotationSyntax* annotation);
};

struct UnionSyntax : Object {
    size_t start;
    size_t end;
    Vector<VariantSyntax> variants;

    UnionSyntax(size_t start, size_t end, Vector<VariantSyntax> variants);
};

struct NamespaceSyntax : Object {
    size_t start;
    size_t end;
    Vector<UseSyntax>* uses;
    Vector<DeclarationSyntax>* declarations;

    NamespaceSyntax(size_t start, size_t end, Vector<UseSyntax>* uses, Vector<DeclarationSyntax>* declarations);
};

struct BodySyntax : Object {
    size_t start;
    size_t end;
    Vector<UseSyntax>* uses;
    Vector<InitSyntax>* inits;
    DeInitSyntax* deInit;
    Vector<MemberSyntax>* members;

    BodySyntax(size_t start, size_t end, Vector<UseSyntax>* uses, Vector<InitSyntax>* inits, DeInitSyntax* deInit, Vector<MemberSyntax>* members);
};

struct ClassSyntax : Object {
    size_t start;
    size_t end;
    StructureSyntax structure;
    BodySyntax* body;

    ClassSyntax(size_t start, size_t end, StructureSyntax structure, BodySyntax* body);
};

struct ConceptSyntax {
    ConceptSyntax(ClassSyntax);
    ConceptSyntax(NamespaceSyntax);
    ConceptSyntax(UnionSyntax);
    ConceptSyntax(ConstantSyntax);
    ConceptSyntax(DelegateSyntax);
    ConceptSyntax(IntrinsicSyntax);
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
    size_t start;
    size_t end;
    String name;
    Vector<AttributeSyntax>* attributes;

    GenericParameterSyntax(size_t start, size_t end, String name, Vector<AttributeSyntax>* attributes);
};

struct GenericParametersSyntax : Object {
    size_t start;
    size_t end;
    Vector<GenericParameterSyntax> parameters;

    GenericParametersSyntax(size_t start, size_t end, Vector<GenericParameterSyntax> parameters);
};

struct DefinitionSyntax : Object {
    size_t start;
    size_t end;
    String name;
    GenericParametersSyntax* parameters;
    Vector<AttributeSyntax>* attributes;
    ConceptSyntax concept_;

    DefinitionSyntax(size_t start, size_t end, String name, GenericParametersSyntax* parameters, Vector<AttributeSyntax>* attributes, ConceptSyntax concept_);
};

struct ConstituentSyntax {
    ConstituentSyntax(DefinitionSyntax);
    ConstituentSyntax(FunctionSyntax);
    ConstituentSyntax(ProcedureSyntax);
    ConstituentSyntax(OperatorSyntax);
    ConstituentSyntax(ImplementSyntax);
    ConstituentSyntax(TraitSyntax);
    ConstituentSyntax(MacroSyntax);
    ConstituentSyntax(ModuleSyntax);
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
    size_t start;
    size_t end;
    ConstituentSyntax constituent;

    MemberSyntax(size_t start, size_t end, ConstituentSyntax constituent);
};

struct ExportSyntax {
    ExportSyntax(DefinitionSyntax);
    ExportSyntax(FunctionSyntax);
    ExportSyntax(OperatorSyntax);
    ExportSyntax(TraitSyntax);
    ExportSyntax(ModuleSyntax);
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
    size_t start;
    size_t end;
    ExportSyntax export_;

    PrivateSyntax(size_t start, size_t end, ExportSyntax export_);
};

struct SymbolSyntax {
    SymbolSyntax(PrivateSyntax);
    SymbolSyntax(DefinitionSyntax);
    SymbolSyntax(FunctionSyntax);
    SymbolSyntax(OperatorSyntax);
    SymbolSyntax(TraitSyntax);
    SymbolSyntax(MacroSyntax);
    SymbolSyntax(ModuleSyntax);
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
    size_t start;
    size_t end;
    SymbolSyntax symbol;

    DeclarationSyntax(size_t start, size_t end, SymbolSyntax symbol);
};

struct FileSyntax : Object {
    size_t start;
    size_t end;
    Vector<PackageSyntax>* packages;
    Vector<UseSyntax>* uses;
    Vector<DeclarationSyntax>* declarations;

    FileSyntax(size_t start, size_t end, Vector<PackageSyntax>* packages, Vector<UseSyntax>* uses, Vector<DeclarationSyntax>* declarations);
};

struct ProgramSyntax : Object {
    size_t start;
    size_t end;
    FileSyntax file;
    Vector<StatementSyntax>* statements;

    ProgramSyntax(size_t start, size_t end, FileSyntax file, Vector<StatementSyntax>* statements);
};

struct Parser : Object {
    Lexer lexer;
    Vector<String> keywords_index;
    HashSet<String> keywords;

    Parser(Lexer lexer, Vector<String> keywords_index, HashSet<String> keywords);
    Parser(String text);
    Vector<String> initialize_keywords_index();
    HashSet<String> initialize_keywords();
    Result<Literal, ParserError> parse_literal_token(Page* rp, Page* ep);
};

#endif