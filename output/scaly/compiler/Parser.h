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

struct Literal : Object {
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
        struct StringLiteral _String;
        struct CharacterLiteral _Character;
        struct FragmentLiteral _Fragment;
        struct IntegerLiteral _Integer;
        struct BooleanLiteral _Boolean;
        struct FloatingPointLiteral _FloatingPoint;
        struct HexLiteral _Hex;
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

struct LifetimeSyntax : Object {
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
        struct CallSyntax _Call;
        struct LocalSyntax _Local;
        struct ReferenceSyntax _Reference;
        struct ThrownSyntax _Thrown;
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

struct ActionSyntax : Object {
    ActionSyntax(OperationSyntax);
    ActionSyntax(SetSyntax);
    enum {
        Operation,
        Set,
    } _tag;
    union {
        struct OperationSyntax _Operation;
        struct SetSyntax _Set;
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

struct PartSyntax : Object {
    PartSyntax(FieldSyntax);
    PartSyntax(PropertySyntax);
    enum {
        Field,
        Property,
    } _tag;
    union {
        struct FieldSyntax _Field;
        struct PropertySyntax _Property;
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

struct BindingSpecSyntax : Object {
    BindingSpecSyntax(StructureSyntax);
    BindingSpecSyntax(TypeSyntax);
    BindingSpecSyntax(ArraySyntax);
    enum {
        Structure,
        Type,
        Array,
    } _tag;
    union {
        struct StructureSyntax _Structure;
        struct TypeSyntax _Type;
        struct ArraySyntax _Array;
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

struct CommandSyntax : Object {
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
        struct OperationSyntax _Operation;
        struct LetSyntax _Let;
        struct VarSyntax _Var;
        struct MutableSyntax _Mutable;
        struct SetSyntax _Set;
        struct ContinueSyntax _Continue;
        struct BreakSyntax _Break;
        struct ReturnSyntax _Return;
        struct ThrowSyntax _Throw;
    };
};
struct WhenSyntax : Object {
    size_t start;
    size_t end;
    String name;
    NameSyntax variant;
    CommandSyntax command;

    WhenSyntax(size_t start, size_t end, String name, NameSyntax variant, CommandSyntax command);
};

struct ConditionSyntax : Object {
    ConditionSyntax(OperationSyntax);
    ConditionSyntax(LetSyntax);
    enum {
        Operation,
        Let,
    } _tag;
    union {
        struct OperationSyntax _Operation;
        struct LetSyntax _Let;
    };
};
struct TrySyntax : Object {
    size_t start;
    size_t end;
    ConditionSyntax condition;
    Vector<WhenSyntax>* cases;
    ElseSyntax* dropper;

    TrySyntax(size_t start, size_t end, ConditionSyntax condition, Vector<WhenSyntax>* cases, ElseSyntax* dropper);
};

struct ChooseSyntax : Object {
    size_t start;
    size_t end;
    Vector<OperandSyntax> condition;
    Vector<WhenSyntax>* cases;
    ElseSyntax* alternative;

    ChooseSyntax(size_t start, size_t end, Vector<OperandSyntax> condition, Vector<WhenSyntax>* cases, ElseSyntax* alternative);
};

struct WhileSyntax : Object {
    size_t start;
    size_t end;
    ConditionSyntax condition;
    LabelSyntax* name;
    ActionSyntax action;

    WhileSyntax(size_t start, size_t end, ConditionSyntax condition, LabelSyntax* name, ActionSyntax action);
};

struct StatementSyntax : Object {
    size_t start;
    size_t end;
    CommandSyntax command;

    StatementSyntax(size_t start, size_t end, CommandSyntax command);
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
    ElseSyntax* alternative;

    MatchSyntax(size_t start, size_t end, Vector<OperandSyntax> scrutinee, Vector<BranchSyntax> branches, ElseSyntax* alternative);
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

struct ExpressionSyntax : Object {
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
        struct LiteralSyntax _Literal;
        struct TypeSyntax _Type;
        struct ObjectSyntax _Object;
        struct VectorSyntax _Vector;
        struct BlockSyntax _Block;
        struct IfSyntax _If;
        struct MatchSyntax _Match;
        struct LambdaSyntax _Lambda;
        struct ForSyntax _For;
        struct WhileSyntax _While;
        struct ChooseSyntax _Choose;
        struct TrySyntax _Try;
        struct RepeatSyntax _Repeat;
        struct SizeOfSyntax _SizeOf;
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

struct ModelSyntax : Object {
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
        struct LiteralSyntax _Literal;
        struct NameSyntax _Name;
        struct ObjectSyntax _Object;
        struct VectorSyntax _Vector;
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

struct ImplementationSyntax : Object {
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
        struct ActionSyntax _Action;
        struct ExternSyntax _Extern;
        struct InstructionSyntax _Instruction;
        struct IntrinsicSyntax _Intrinsic;
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

struct TargetSyntax : Object {
    TargetSyntax(NamedSyntax);
    TargetSyntax(RoutineSyntax);
    enum {
        Named,
        Routine,
    } _tag;
    union {
        struct NamedSyntax _Named;
        struct RoutineSyntax _Routine;
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

struct MethodSyntax : Object {
    MethodSyntax(FunctionSyntax);
    MethodSyntax(ProcedureSyntax);
    MethodSyntax(OperatorSyntax);
    enum {
        Function,
        Procedure,
        Operator,
    } _tag;
    union {
        struct FunctionSyntax _Function;
        struct ProcedureSyntax _Procedure;
        struct OperatorSyntax _Operator;
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

struct ParameterSetSyntax : Object {
    ParameterSetSyntax(ParametersSyntax);
    ParameterSetSyntax(TypeSyntax);
    enum {
        Parameters,
        Type,
    } _tag;
    union {
        struct ParametersSyntax _Parameters;
        struct TypeSyntax _Type;
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

struct ConceptSyntax : Object {
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
        struct ClassSyntax _Class;
        struct NamespaceSyntax _Namespace;
        struct UnionSyntax _Union;
        struct ConstantSyntax _Constant;
        struct DelegateSyntax _Delegate;
        struct IntrinsicSyntax _Intrinsic;
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

struct ConstituentSyntax : Object {
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
        struct DefinitionSyntax _Definition;
        struct FunctionSyntax _Function;
        struct ProcedureSyntax _Procedure;
        struct OperatorSyntax _Operator;
        struct ImplementSyntax _Implement;
        struct TraitSyntax _Trait;
        struct MacroSyntax _Macro;
        struct ModuleSyntax _Module;
    };
};
struct MemberSyntax : Object {
    size_t start;
    size_t end;
    ConstituentSyntax constituent;

    MemberSyntax(size_t start, size_t end, ConstituentSyntax constituent);
};

struct ExportSyntax : Object {
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
        struct DefinitionSyntax _Definition;
        struct FunctionSyntax _Function;
        struct OperatorSyntax _Operator;
        struct TraitSyntax _Trait;
        struct ModuleSyntax _Module;
    };
};
struct PrivateSyntax : Object {
    size_t start;
    size_t end;
    ExportSyntax export_;

    PrivateSyntax(size_t start, size_t end, ExportSyntax export_);
};

struct SymbolSyntax : Object {
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
        struct PrivateSyntax _Private;
        struct DefinitionSyntax _Definition;
        struct FunctionSyntax _Function;
        struct OperatorSyntax _Operator;
        struct TraitSyntax _Trait;
        struct MacroSyntax _Macro;
        struct ModuleSyntax _Module;
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
    Result<ProgramSyntax, ParserError> parse_program(Page* rp, Page* ep);
    Result<FileSyntax, ParserError> parse_file(Page* rp, Page* ep);
    Result<Vector<DeclarationSyntax>*, ParserError> parse_declaration_list(Page* rp, Page* ep);
    Result<DeclarationSyntax, ParserError> parse_declaration(Page* rp, Page* ep);
    Result<SymbolSyntax, ParserError> parse_symbol(Page* rp, Page* ep);
    Result<PrivateSyntax, ParserError> parse_private(Page* rp, Page* ep);
    Result<ExportSyntax, ParserError> parse_export(Page* rp, Page* ep);
    Result<Vector<MemberSyntax>*, ParserError> parse_member_list(Page* rp, Page* ep);
    Result<MemberSyntax, ParserError> parse_member(Page* rp, Page* ep);
    Result<ConstituentSyntax, ParserError> parse_constituent(Page* rp, Page* ep);
    Result<DefinitionSyntax, ParserError> parse_definition(Page* rp, Page* ep);
    Result<GenericParametersSyntax, ParserError> parse_genericparameters(Page* rp, Page* ep);
    Result<Vector<GenericParameterSyntax>*, ParserError> parse_genericparameter_list(Page* rp, Page* ep);
    Result<GenericParameterSyntax, ParserError> parse_genericparameter(Page* rp, Page* ep);
    Result<ConceptSyntax, ParserError> parse_concept(Page* rp, Page* ep);
    Result<ClassSyntax, ParserError> parse_class(Page* rp, Page* ep);
    Result<BodySyntax, ParserError> parse_body(Page* rp, Page* ep);
    Result<NamespaceSyntax, ParserError> parse_namespace(Page* rp, Page* ep);
    Result<UnionSyntax, ParserError> parse_union(Page* rp, Page* ep);
    Result<Vector<VariantSyntax>*, ParserError> parse_variant_list(Page* rp, Page* ep);
    Result<VariantSyntax, ParserError> parse_variant(Page* rp, Page* ep);
    Result<ConstantSyntax, ParserError> parse_constant(Page* rp, Page* ep);
    Result<DelegateSyntax, ParserError> parse_delegate(Page* rp, Page* ep);
    Result<GenericArgumentsSyntax, ParserError> parse_genericarguments(Page* rp, Page* ep);
    Result<Vector<GenericArgumentSyntax>*, ParserError> parse_genericargument_list(Page* rp, Page* ep);
    Result<GenericArgumentSyntax, ParserError> parse_genericargument(Page* rp, Page* ep);
    Result<OptionalSyntax, ParserError> parse_optional(Page* rp, Page* ep);
    Result<ParameterSetSyntax, ParserError> parse_parameterset(Page* rp, Page* ep);
    Result<ParametersSyntax, ParserError> parse_parameters(Page* rp, Page* ep);
    Result<Vector<ItemSyntax>*, ParserError> parse_item_list(Page* rp, Page* ep);
    Result<ItemSyntax, ParserError> parse_item(Page* rp, Page* ep);
    Result<ReturnsSyntax, ParserError> parse_returns(Page* rp, Page* ep);
    Result<ThrowsSyntax, ParserError> parse_throws(Page* rp, Page* ep);
    Result<Vector<UseSyntax>*, ParserError> parse_use_list(Page* rp, Page* ep);
    Result<UseSyntax, ParserError> parse_use(Page* rp, Page* ep);
    Result<ImplementSyntax, ParserError> parse_implement(Page* rp, Page* ep);
    Result<TraitSyntax, ParserError> parse_trait(Page* rp, Page* ep);
    Result<Vector<MethodSyntax>*, ParserError> parse_method_list(Page* rp, Page* ep);
    Result<MethodSyntax, ParserError> parse_method(Page* rp, Page* ep);
    Result<Vector<InitSyntax>*, ParserError> parse_init_list(Page* rp, Page* ep);
    Result<InitSyntax, ParserError> parse_init(Page* rp, Page* ep);
    Result<DeInitSyntax, ParserError> parse_deinit(Page* rp, Page* ep);
    Result<FunctionSyntax, ParserError> parse_function(Page* rp, Page* ep);
    Result<ProcedureSyntax, ParserError> parse_procedure(Page* rp, Page* ep);
    Result<OperatorSyntax, ParserError> parse_operator(Page* rp, Page* ep);
    Result<TargetSyntax, ParserError> parse_target(Page* rp, Page* ep);
    Result<NamedSyntax, ParserError> parse_named(Page* rp, Page* ep);
    Result<RoutineSyntax, ParserError> parse_routine(Page* rp, Page* ep);
    Result<ImplementationSyntax, ParserError> parse_implementation(Page* rp, Page* ep);
    Result<ExternSyntax, ParserError> parse_extern(Page* rp, Page* ep);
    Result<InstructionSyntax, ParserError> parse_instruction(Page* rp, Page* ep);
    Result<IntrinsicSyntax, ParserError> parse_intrinsic(Page* rp, Page* ep);
    Result<ExtendsSyntax, ParserError> parse_extends(Page* rp, Page* ep);
    Result<Vector<ExtendSyntax>*, ParserError> parse_extend_list(Page* rp, Page* ep);
    Result<ExtendSyntax, ParserError> parse_extend(Page* rp, Page* ep);
    Result<MacroSyntax, ParserError> parse_macro(Page* rp, Page* ep);
    Result<Vector<AttributeSyntax>*, ParserError> parse_attribute_list(Page* rp, Page* ep);
    Result<AttributeSyntax, ParserError> parse_attribute(Page* rp, Page* ep);
    Result<ModelSyntax, ParserError> parse_model(Page* rp, Page* ep);
    Result<ModuleSyntax, ParserError> parse_module(Page* rp, Page* ep);
    Result<Vector<PackageSyntax>*, ParserError> parse_package_list(Page* rp, Page* ep);
    Result<PackageSyntax, ParserError> parse_package(Page* rp, Page* ep);
    Result<InitializerSyntax, ParserError> parse_initializer(Page* rp, Page* ep);
    Result<Vector<OperandSyntax>*, ParserError> parse_operand_list(Page* rp, Page* ep);
    Result<OperandSyntax, ParserError> parse_operand(Page* rp, Page* ep);
    Result<Vector<MemberAccessSyntax>*, ParserError> parse_memberaccess_list(Page* rp, Page* ep);
    Result<MemberAccessSyntax, ParserError> parse_memberaccess(Page* rp, Page* ep);
    Result<ExpressionSyntax, ParserError> parse_expression(Page* rp, Page* ep);
    Result<LiteralSyntax, ParserError> parse_literal(Page* rp, Page* ep);
    Result<ObjectSyntax, ParserError> parse_object(Page* rp, Page* ep);
    Result<Vector<ComponentSyntax>*, ParserError> parse_component_list(Page* rp, Page* ep);
    Result<ComponentSyntax, ParserError> parse_component(Page* rp, Page* ep);
    Result<ValueSyntax, ParserError> parse_value(Page* rp, Page* ep);
    Result<VectorSyntax, ParserError> parse_vector(Page* rp, Page* ep);
    Result<Vector<ElementSyntax>*, ParserError> parse_element_list(Page* rp, Page* ep);
    Result<ElementSyntax, ParserError> parse_element(Page* rp, Page* ep);
    Result<BlockSyntax, ParserError> parse_block(Page* rp, Page* ep);
    Result<IfSyntax, ParserError> parse_if(Page* rp, Page* ep);
    Result<ElseSyntax, ParserError> parse_else(Page* rp, Page* ep);
    Result<MatchSyntax, ParserError> parse_match(Page* rp, Page* ep);
    Result<Vector<BranchSyntax>*, ParserError> parse_branch_list(Page* rp, Page* ep);
    Result<BranchSyntax, ParserError> parse_branch(Page* rp, Page* ep);
    Result<Vector<CaseSyntax>*, ParserError> parse_case_list(Page* rp, Page* ep);
    Result<CaseSyntax, ParserError> parse_case(Page* rp, Page* ep);
    Result<Vector<StatementSyntax>*, ParserError> parse_statement_list(Page* rp, Page* ep);
    Result<StatementSyntax, ParserError> parse_statement(Page* rp, Page* ep);
    Result<WhileSyntax, ParserError> parse_while(Page* rp, Page* ep);
    Result<ChooseSyntax, ParserError> parse_choose(Page* rp, Page* ep);
    Result<TrySyntax, ParserError> parse_try(Page* rp, Page* ep);
    Result<ConditionSyntax, ParserError> parse_condition(Page* rp, Page* ep);
    Result<Vector<WhenSyntax>*, ParserError> parse_when_list(Page* rp, Page* ep);
    Result<WhenSyntax, ParserError> parse_when(Page* rp, Page* ep);
    Result<CommandSyntax, ParserError> parse_command(Page* rp, Page* ep);
    Result<LetSyntax, ParserError> parse_let(Page* rp, Page* ep);
    Result<VarSyntax, ParserError> parse_var(Page* rp, Page* ep);
    Result<MutableSyntax, ParserError> parse_mutable(Page* rp, Page* ep);
    Result<BindingSyntax, ParserError> parse_binding(Page* rp, Page* ep);
    Result<BindingAnnotationSyntax, ParserError> parse_bindingannotation(Page* rp, Page* ep);
    Result<Vector<BindingSpecSyntax>*, ParserError> parse_bindingspec_list(Page* rp, Page* ep);
    Result<BindingSpecSyntax, ParserError> parse_bindingspec(Page* rp, Page* ep);
    Result<ArraySyntax, ParserError> parse_array(Page* rp, Page* ep);
    Result<StructureSyntax, ParserError> parse_structure(Page* rp, Page* ep);
    Result<Vector<PartSyntax>*, ParserError> parse_part_list(Page* rp, Page* ep);
    Result<PartSyntax, ParserError> parse_part(Page* rp, Page* ep);
    Result<FieldSyntax, ParserError> parse_field(Page* rp, Page* ep);
    Result<Vector<PropertySyntax>*, ParserError> parse_property_list(Page* rp, Page* ep);
    Result<PropertySyntax, ParserError> parse_property(Page* rp, Page* ep);
    Result<TypeAnnotationSyntax, ParserError> parse_typeannotation(Page* rp, Page* ep);
    Result<ContinueSyntax, ParserError> parse_continue(Page* rp, Page* ep);
    Result<BreakSyntax, ParserError> parse_break(Page* rp, Page* ep);
    Result<LoopSyntax, ParserError> parse_loop(Page* rp, Page* ep);
    Result<ReturnSyntax, ParserError> parse_return(Page* rp, Page* ep);
    Result<ThrowSyntax, ParserError> parse_throw(Page* rp, Page* ep);
    Result<LambdaSyntax, ParserError> parse_lambda(Page* rp, Page* ep);
    Result<ForSyntax, ParserError> parse_for(Page* rp, Page* ep);
    Result<LabelSyntax, ParserError> parse_label(Page* rp, Page* ep);
    Result<RepeatSyntax, ParserError> parse_repeat(Page* rp, Page* ep);
    Result<Vector<ActionSyntax>*, ParserError> parse_action_list(Page* rp, Page* ep);
    Result<ActionSyntax, ParserError> parse_action(Page* rp, Page* ep);
    Result<OperationSyntax, ParserError> parse_operation(Page* rp, Page* ep);
    Result<SetSyntax, ParserError> parse_set(Page* rp, Page* ep);
    Result<SizeOfSyntax, ParserError> parse_sizeof(Page* rp, Page* ep);
    Result<Vector<TypeSyntax>*, ParserError> parse_type_list(Page* rp, Page* ep);
    Result<TypeSyntax, ParserError> parse_type(Page* rp, Page* ep);
    Result<NameSyntax, ParserError> parse_name(Page* rp, Page* ep);
    Result<Vector<ExtensionSyntax>*, ParserError> parse_extension_list(Page* rp, Page* ep);
    Result<ExtensionSyntax, ParserError> parse_extension(Page* rp, Page* ep);
    Result<LifetimeSyntax, ParserError> parse_lifetime(Page* rp, Page* ep);
    Result<CallSyntax, ParserError> parse_call(Page* rp, Page* ep);
    Result<LocalSyntax, ParserError> parse_local(Page* rp, Page* ep);
    Result<ReferenceSyntax, ParserError> parse_reference(Page* rp, Page* ep);
    Result<ThrownSyntax, ParserError> parse_thrown(Page* rp, Page* ep);
    bool is_at_end();
};

#endif