#ifndef _Model_h
#define _Model_h
#include "../../scaly.h"
using namespace scaly;
using namespace scaly::memory;

using namespace scaly::containers;

using namespace scaly::io;

struct Span : Object {
    size_t start;
    size_t end;

    Span(size_t start, size_t end);
};

struct Component : Object {
    Span span;
    String* name;
    Vector<Operand> value;
    Vector<Attribute> attributes;

    Component(Span span, String* name, Vector<Operand> value, Vector<Attribute> attributes);
};

struct Name : Object {
    Span span;
    Vector<String> path;

    Name(Span span, Vector<String> path);
};

struct BooleanConstant : Object {
    bool value;

    BooleanConstant(bool value);
};

struct IntegerConstant : Object {
    size_t value;

    IntegerConstant(size_t value);
};

struct HexConstant : Object {
    size_t value;

    HexConstant(size_t value);
};

struct FloatingPointConstant : Object {
    double value;

    FloatingPointConstant(double value);
};

struct StringConstant : Object {
    String value;

    StringConstant(String value);
};

struct CharacterConstant : Object {
    String value;

    CharacterConstant(String value);
};

struct FragmentConstant : Object {
    String value;

    FragmentConstant(String value);
};

struct Constant : Object {
    Constant(BooleanConstant);
    Constant(IntegerConstant);
    Constant(HexConstant);
    Constant(FloatingPointConstant);
    Constant(StringConstant);
    Constant(CharacterConstant);
    Constant(FragmentConstant);
    enum {
        Boolean,
        Integer,
        Hex,
        FloatingPoint,
        String,
        Character,
        Fragment,
    } _tag;
    union {
        struct BooleanConstant _Boolean;
        struct IntegerConstant _Integer;
        struct HexConstant _Hex;
        struct FloatingPointConstant _FloatingPoint;
        struct StringConstant _String;
        struct CharacterConstant _Character;
        struct FragmentConstant _Fragment;
    };
};
struct Tuple : Object {
    Span span;
    Vector<Component> components;

    Tuple(Span span, Vector<Component> components);
};

struct Matrix : Object {
    Span span;
    Vector<Component> operations;

    Matrix(Span span, Vector<Component> operations);
};

struct Block : Object {
    Span span;
    Vector<Statement> statements;

    Block(Span span, Vector<Statement> statements);
};

struct Unspecified : Object {

};

struct Call : Object {

};

struct Local : Object {

};

struct Reference : Object {
    String location;

    Reference(String location);
};

struct Thrown : Object {

};

struct Lifetime : Object {
    Lifetime(Unspecified);
    Lifetime(Call);
    Lifetime(Local);
    Lifetime(Reference);
    Lifetime(Thrown);
    enum {
        Unspecified,
        Call,
        Local,
        Reference,
        Thrown,
    } _tag;
    union {
        struct Unspecified _Unspecified;
        struct Call _Call;
        struct Local _Local;
        struct Reference _Reference;
        struct Thrown _Thrown;
    };
};
struct Action : Object {
    Vector<Operand> source;
    Vector<Operand> target;

    Action(Vector<Operand> source, Vector<Operand> target);
};

struct Item : Object {
    Span span;
    bool private_;
    String* name;
    Type* type;
    Vector<Attribute> attributes;

    Item(Span span, bool private_, String* name, Type* type, Vector<Attribute> attributes);
};

struct Binding : Object {
    String binding_type;
    Item item;
    Vector<Operand> operation;

    Binding(String binding_type, Item item, Vector<Operand> operation);
};

struct Break : Object {
    Span span;
    Vector<Operand> result;

    Break(Span span, Vector<Operand> result);
};

struct Continue : Object {
    Span span;

    Continue(Span span);
};

struct Return : Object {
    Span span;
    Vector<Operand> result;

    Return(Span span, Vector<Operand> result);
};

struct Throw : Object {
    Span span;
    Vector<Operand> result;

    Throw(Span span, Vector<Operand> result);
};

struct Statement : Object {
    Statement(Action);
    Statement(Binding);
    Statement(Break);
    Statement(Continue);
    Statement(Return);
    Statement(Throw);
    enum {
        Action,
        Binding,
        Break,
        Continue,
        Return,
        Throw,
    } _tag;
    union {
        struct Action _Action;
        struct Binding _Binding;
        struct Break _Break;
        struct Continue _Continue;
        struct Return _Return;
        struct Throw _Throw;
    };
};
struct Type : Object {
    Span span;
    Vector<String> name;
    Vector<Type>* generics;
    Lifetime lifetime;

    Type(Span span, Vector<String> name, Vector<Type>* generics, Lifetime lifetime);
};

struct Property : Object {
    Span span;
    bool private_;
    String name;
    Type* type;
    Vector<Operand>* initializer;
    Vector<Attribute> attributes;

    Property(Span span, bool private_, String name, Type* type, Vector<Operand>* initializer, Vector<Attribute> attributes);
};

struct If : Object {
    Span span;
    Vector<Operand> condition;
    Property* property;
    Statement consequent;
    Statement* alternative;

    If(Span span, Vector<Operand> condition, Property* property, Statement consequent, Statement* alternative);
};

struct Case : Object {
    Span span;
    Vector<Operand> condition;

    Case(Span span, Vector<Operand> condition);
};

struct Branch : Object {
    Span span;
    Vector<Case> cases;
    Statement consequent;

    Branch(Span span, Vector<Case> cases, Statement consequent);
};

struct Match : Object {
    Span span;
    Vector<Operand> condition;
    Vector<Branch> branches;
    Statement* alternative;

    Match(Span span, Vector<Operand> condition, Vector<Branch> branches, Statement* alternative);
};

struct When : Object {
    Span span;
    String name;
    Vector<String> variant;
    Statement consequent;

    When(Span span, String name, Vector<String> variant, Statement consequent);
};

struct Choose : Object {
    Span span;
    Vector<Operand> condition;
    Vector<When> cases;
    Statement* alternative;

    Choose(Span span, Vector<Operand> condition, Vector<When> cases, Statement* alternative);
};

struct For : Object {
    Span span;
    String identifier;
    Vector<Operand> expression;
    Action action;

    For(Span span, String identifier, Vector<Operand> expression, Action action);
};

struct While : Object {
    Span span;
    Binding condition;
    Action action;

    While(Span span, Binding condition, Action action);
};

struct Drop : Object {
    Span span;
    Action action;

    Drop(Span span, Action action);
};

struct Try : Object {
    Span span;
    Binding condition;
    Vector<When> catches;
    Statement* alternative;

    Try(Span span, Binding condition, Vector<When> catches, Statement* alternative);
};

struct SizeOf : Object {
    Span span;
    Type type;

    SizeOf(Span span, Type type);
};

struct Expression : Object {
    Expression(Constant);
    Expression(Type);
    Expression(Tuple);
    Expression(Matrix);
    Expression(Block);
    Expression(If);
    Expression(Match);
    Expression(Choose);
    Expression(For);
    Expression(While);
    Expression(Try);
    Expression(SizeOf);
    Expression(Return);
    enum {
        Constant,
        Type,
        Tuple,
        Matrix,
        Block,
        If,
        Match,
        Choose,
        For,
        While,
        Try,
        SizeOf,
        Return,
    } _tag;
    union {
        struct Constant _Constant;
        struct Type _Type;
        struct Tuple _Tuple;
        struct Matrix _Matrix;
        struct Block _Block;
        struct If _If;
        struct Match _Match;
        struct Choose _Choose;
        struct For _For;
        struct While _While;
        struct Try _Try;
        struct SizeOf _SizeOf;
        struct Return _Return;
    };
};
struct Operand : Object {
    Span span;
    Expression expression;
    Vector<String>* member_access;

    Operand(Span span, Expression expression, Vector<String>* member_access);
};

struct Extern : Object {

};

struct Instruction : Object {

};

struct Intrinsic : Object {

};

struct Implementation : Object {
    Implementation(Action);
    Implementation(Extern);
    Implementation(Instruction);
    Implementation(Intrinsic);
    enum {
        Action,
        Extern,
        Instruction,
        Intrinsic,
    } _tag;
    union {
        struct Action _Action;
        struct Extern _Extern;
        struct Instruction _Instruction;
        struct Intrinsic _Intrinsic;
    };
};
struct Function : Object {
    Span span;
    bool private_;
    bool pure;
    String name;
    Vector<Item> input;
    Type* returns_;
    Type* throws_;
    Lifetime lifetime;
    Implementation implementation;

    Function(Span span, bool private_, bool pure, String name, Vector<Item> input, Type* returns_, Type* throws_, Lifetime lifetime, Implementation implementation);
};

struct Model : Object {
    Model(Constant);
    Model(String);
    Model(Tuple);
    Model(Matrix);
    enum {
        Constant,
        Variable,
        Tuple,
        Matrix,
    } _tag;
    union {
        struct Constant _Constant;
        struct String _Variable;
        struct Tuple _Tuple;
        struct Matrix _Matrix;
    };
};
struct Attribute : Object {
    Span span;
    String name;
    Model model;

    Attribute(Span span, String name, Model model);
};

struct Initializer : Object {
    Span span;
    bool private_;
    Vector<Item> input;
    Implementation implementation;

    Initializer(Span span, bool private_, Vector<Item> input, Implementation implementation);
};

struct DeInitializer : Object {
    Span span;
    Implementation implementation;

    DeInitializer(Span span, Implementation implementation);
};

struct Operator : Object {
    Span span;
    bool private_;
    String name;
    Vector<Item> input;
    Type* returns_;
    Type* throws_;
    Implementation implementation;

    Operator(Span span, bool private_, String name, Vector<Item> input, Type* returns_, Type* throws_, Implementation implementation);
};

struct Structure : Object {
    Span span;
    bool private_;
    Vector<Property> properties;
    Vector<Module> modules;
    Vector<Use> uses;
    Vector<Initializer> initializers;
    DeInitializer* deinitializer;
    Vector<Member> members;
    HashMap<String, Nameable> symbols;

    Structure(Span span, bool private_, Vector<Property> properties, Vector<Module> modules, Vector<Use> uses, Vector<Initializer> initializers, DeInitializer* deinitializer, Vector<Member> members, HashMap<String, Nameable> symbols);
};

struct Variant : Object {
    Span span;
    String name;
    Type* type;
    Vector<Attribute> attributes;

    Variant(Span span, String name, Type* type, Vector<Attribute> attributes);
};

struct Union : Object {
    Span span;
    bool private_;
    Vector<Variant> variants;
    HashMap<String, Variant> symbols;

    Union(Span span, bool private_, Vector<Variant> variants, HashMap<String, Variant> symbols);
};

struct Namespace : Object {
    Span span;
    Vector<Module> modules;
    Vector<Member> members;
    HashMap<String, Nameable> symbols;

    Namespace(Span span, Vector<Module> modules, Vector<Member> members, HashMap<String, Nameable> symbols);
};

struct Global : Object {
    Span span;
    Type type;
    Vector<Operand> value;

    Global(Span span, Type type, Vector<Operand> value);
};

struct Definition : Object {
    Definition(Intrinsic);
    Definition(Global);
    Definition(Namespace);
    Definition(Structure);
    Definition(Union);
    enum {
        Intrinsic,
        Global,
        Namespace,
        Structure,
        Union,
    } _tag;
    union {
        struct Intrinsic _Intrinsic;
        struct Global _Global;
        struct Namespace _Namespace;
        struct Structure _Structure;
        struct Union _Union;
    };
};
struct Use : Object {
    Span span;
    Vector<String> path;

    Use(Span span, Vector<String> path);
};

struct GenericParameter : Object {
    Span span;
    String name;
    Vector<Attribute> attributes;

    GenericParameter(Span span, String name, Vector<Attribute> attributes);
};

struct Concept : Object {
    Span span;
    String name;
    Vector<GenericParameter> parameters;
    Vector<Attribute> attributes;
    Definition definition;

    Concept(Span span, String name, Vector<GenericParameter> parameters, Vector<Attribute> attributes, Definition definition);
};

struct Module : Object {
    bool private_;
    String file;
    String name;
    Vector<Module> modules;
    Vector<Use> uses;
    Vector<Member> mambers;
    HashMap<String, Nameable> symbols;

    Module(bool private_, String file, String name, Vector<Module> modules, Vector<Use> uses, Vector<Member> mambers, HashMap<String, Nameable> symbols);
};

struct Program : Object {
    Vector<Module> packages;
    Module module_;
    Vector<Statement> statements;

    Program(Vector<Module> packages, Module module_, Vector<Statement> statements);
};

struct Member : Object {
    Member(Concept);
    Member(Operator);
    Member(Function);
    enum {
        Concept,
        Operator,
        Function,
    } _tag;
    union {
        struct Concept _Concept;
        struct Operator _Operator;
        struct Function _Function;
    };
};
struct Nameable : Object {
    Nameable(Concept);
    Nameable(Operator);
    Nameable(Vector<Function>);
    Nameable(Property);
    enum {
        Concept,
        Operator,
        Functions,
        Property,
    } _tag;
    union {
        struct Concept _Concept;
        struct Operator _Operator;
        struct Vector<Function> _Functions;
        struct Property _Property;
    };
};
#endif