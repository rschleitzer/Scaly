// Model.h - Semantic model types for Scaly compiler
#pragma once

#include "llvm/ADT/SmallVector.h"
#include "llvm/ADT/StringRef.h"
#include <cstddef>
#include <map>
#include <memory>
#include <optional>
#include <string>
#include <variant>
#include <vector>

namespace scaly {

// Span represents a source location range
struct Span {
    size_t Start;
    size_t End;
};

// Constants
struct BooleanConstant {
    Span Loc;
    bool Value;
};

struct IntegerConstant {
    Span Loc;
    int64_t Value;
};

struct HexConstant {
    Span Loc;
    uint64_t Value;
};

struct FloatingPointConstant {
    Span Loc;
    double Value;
};

struct StringConstant {
    Span Loc;
    std::string Value;
};

struct CharacterConstant {
    Span Loc;
    std::string Value;
};

// Package version (semantic versioning)
struct Version {
    unsigned Major;
    unsigned Minor;
    unsigned Patch;

    std::string toString() const {
        return std::to_string(Major) + "." +
               std::to_string(Minor) + "." +
               std::to_string(Patch);
    }

    // For mangled names: 2.0.1 -> "2_0_1"
    std::string toMangledString() const {
        return std::to_string(Major) + "_" +
               std::to_string(Minor) + "_" +
               std::to_string(Patch);
    }
};

struct FragmentConstant {
    Span Loc;
    std::string Value;
};

struct NullConstant {
    Span Loc;
};

using Constant = std::variant<
    BooleanConstant,
    IntegerConstant,
    HexConstant,
    FloatingPointConstant,
    StringConstant,
    CharacterConstant,
    FragmentConstant,
    NullConstant
>;

// Lifetime variants
struct UnspecifiedLifetime {};

struct CallLifetime {
    Span Loc;
};

struct LocalLifetime {
    Span Loc;
};

struct ReferenceLifetime {
    Span Loc;
    std::string Location;
};

struct ThrownLifetime {
    Span Loc;
};

using Lifetime = std::variant<
    UnspecifiedLifetime,
    CallLifetime,
    LocalLifetime,
    ReferenceLifetime,
    ThrownLifetime
>;

// Type represents a type reference
struct Type {
    Span Loc;
    std::vector<std::string> Name;
    std::shared_ptr<std::vector<Type>> Generics;
    Lifetime Life;
};

// Forward declarations for recursive types
struct Operand;
struct Tuple;
struct Matrix;
struct Component;
struct Attribute;

// Model for attribute values
struct ModelVariable {
    std::string Name;
};

using Model = std::variant<
    Constant,
    ModelVariable,
    std::shared_ptr<Tuple>,
    std::shared_ptr<Matrix>
>;

struct Attribute {
    Span Loc;
    std::string Name;
    Model Value;
};

// Item represents a parameter or binding target
struct Item {
    Span Loc;
    bool Private;
    std::shared_ptr<std::string> Name;
    std::shared_ptr<Type> ItemType;
    std::vector<Attribute> Attributes;
};

// Action represents an executable unit
struct Action {
    std::vector<Operand> Source;
    std::vector<Operand> Target;
};

// Binding represents a variable binding
struct Binding {
    Span Loc;
    std::string BindingType;  // "const", "var", "mutable"
    Item BindingItem;
    std::vector<Operand> Operation;
};

// Control flow statements
struct Break {
    Span Loc;
    std::vector<Operand> Result;
};

struct Continue {
    Span Loc;
};

struct Return {
    Span Loc;
    std::vector<Operand> Result;
};

struct Throw {
    Span Loc;
    std::vector<Operand> Result;
};

// Statement is a variant of executable statements
using Statement = std::variant<
    Action,
    Binding,
    Break,
    Continue,
    Return,
    Throw
>;

// Block of statements
struct Block {
    Span Loc;
    std::vector<Statement> Statements;
};

// Property represents a struct field
struct Property {
    Span Loc;
    bool Private;
    std::string Name;
    std::shared_ptr<Type> PropType;
    std::shared_ptr<std::vector<Operand>> Initializer;
    std::vector<Attribute> Attributes;
};

// Control flow expressions
struct If {
    Span Loc;
    std::vector<Operand> Condition;
    std::shared_ptr<Property> Prop;
    std::shared_ptr<Statement> Consequent;
    std::shared_ptr<Statement> Alternative;
};

struct Case {
    Span Loc;
    std::vector<Operand> Condition;
};

struct Branch {
    Span Loc;
    std::vector<Case> Cases;
    std::shared_ptr<Statement> Consequent;
};

struct Match {
    Span Loc;
    std::vector<Operand> Condition;
    std::vector<Branch> Branches;
    std::shared_ptr<Statement> Alternative;
};

struct When {
    Span Loc;
    std::string Name;
    std::vector<std::string> VariantPath;
    std::shared_ptr<Statement> Consequent;
};

struct Choose {
    Span Loc;
    std::vector<Operand> Condition;
    std::vector<When> Cases;
    std::shared_ptr<Statement> Alternative;
};

struct For {
    Span Loc;
    std::string Identifier;
    std::vector<Operand> Expr;
    Action Body;
};

struct While {
    Span Loc;
    Binding Cond;
    Action Body;
};

struct Try {
    Span Loc;
    Binding Cond;
    std::vector<When> Catches;
    std::shared_ptr<Statement> Alternative;
};

struct SizeOf {
    Span Loc;
    Type SizedType;
};

struct AlignOf {
    Span Loc;
    Type AlignedType;
};

struct Is {
    Span Loc;
    std::vector<std::string> Name;
};

// Component of a tuple
struct Component {
    Span Loc;
    std::shared_ptr<std::string> Name;
    std::vector<Operand> Value;
    std::vector<Attribute> Attributes;
};

// Tuple (object literal)
struct Tuple {
    Span Loc;
    std::vector<Component> Components;
};

// Matrix (vector literal)
struct Matrix {
    Span Loc;
    std::vector<std::vector<Operand>> Operations;
    Lifetime Life;  // Lifetime from vector syntax (e.g., [T]^lifetime)
};

struct New {
    Span Loc;
    Type NewType;
    Tuple Arguments;
};

// As expression for type casting: value as Type
struct As {
    Span Loc;
    Type TargetType;
};

// Expression is the main expression type
using Expression = std::variant<
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
    AlignOf,
    Is,
    As,
    New
>;

// Operand wraps an expression with optional member access
struct Operand {
    Span Loc;
    Expression Expr;
    std::shared_ptr<std::vector<Type>> MemberAccess;  // Full Type to preserve lifetime
};

// Implementation variants
struct ExternImpl {
    Span Loc;
};

struct InstructionImpl {
    Span Loc;
};

struct IntrinsicImpl {
    Span Loc;
};

using Implementation = std::variant<
    Action,
    ExternImpl,
    InstructionImpl,
    IntrinsicImpl
>;

// Generic parameter (for generic functions/operators)
struct GenericParameter {
    Span Loc;
    std::string Name;
    std::vector<Attribute> Attributes;
};

// Function represents a function or procedure
struct Function {
    Span Loc;
    bool Private;
    bool Pure;
    std::string Name;
    std::vector<GenericParameter> Parameters;  // Generic type parameters (if any)
    std::vector<Item> Input;
    std::shared_ptr<Type> Returns;
    std::shared_ptr<Type> Throws;
    Lifetime Life;
    Implementation Impl;
};

// Initializer for structures
struct Initializer {
    Span Loc;
    bool Private;
    std::optional<std::string> PageParameter;  // Name of page param if init#
    std::vector<Item> Input;
    Implementation Impl;
};

// DeInitializer for structures
struct DeInitializer {
    Span Loc;
    Implementation Impl;
};

// Operator definition
struct Operator {
    Span Loc;
    bool Private;
    std::string Name;
    std::vector<GenericParameter> Parameters;  // Generic type parameters (if any)
    std::vector<Item> Input;
    std::shared_ptr<Type> Returns;
    std::shared_ptr<Type> Throws;
    Implementation Impl;
};

// Variant for union types
struct Variant {
    Span Loc;
    std::string Name;
    std::shared_ptr<Type> VarType;
    std::vector<Attribute> Attributes;
};

// Forward declare for recursive types
struct Module;
struct Package;
struct Concept;
struct Nameable;

// Use statement
struct Use {
    Span Loc;
    std::vector<std::string> Path;
};

// Global constant
struct Global {
    Span Loc;
    Type GlobalType;
    std::vector<Operand> Value;
};

// Member variant used in Structure, Union, and Namespace
using StructMember = std::variant<Concept, Operator, Function>;

// Structure (class) definition
struct Structure {
    Span Loc;
    bool Private;
    std::vector<Property> Properties;
    std::vector<Module> Modules;
    std::vector<std::vector<std::string>> Uses;
    std::vector<Initializer> Initializers;
    std::shared_ptr<DeInitializer> Deinitializer;
    std::vector<StructMember> Members;
    std::map<std::string, std::shared_ptr<Nameable>> Symbols;
};

// Union definition
struct Union {
    Span Loc;
    bool Private;
    std::vector<Variant> Variants;
    std::vector<StructMember> Members;
    std::map<std::string, std::shared_ptr<Nameable>> Symbols;
};

// Namespace definition
struct Namespace {
    Span Loc;
    std::vector<Module> Modules;
    std::vector<StructMember> Members;
    std::map<std::string, std::shared_ptr<Nameable>> Symbols;
};

// Definition represents a type definition
using Definition = std::variant<
    IntrinsicImpl,
    Global,
    Namespace,
    Structure,
    Union,
    Type
>;

// Concept wraps a definition with generics
struct Concept {
    Span Loc;
    std::string Name;
    std::vector<GenericParameter> Parameters;
    std::vector<Attribute> Attributes;
    Definition Def;
};

// Package represents an external package dependency with version
// (defined before Member/Nameable because std::variant needs complete types)
struct Package {
    std::string Name;
    Version Ver;
    std::shared_ptr<Module> Root;  // The loaded package module
};

// Member can be a package, concept, operator, or function
using Member = std::variant<
    Package,
    Concept,
    Operator,
    Function
>;

// Nameable represents symbols in a symbol table
struct Nameable {
    std::variant<
        Package,
        Concept,
        Operator,
        std::vector<Function>,  // Function overloads
        Property,
        Variant
    > Value;
};

// Module represents a compiled module
struct Module {
    bool Private;
    std::string File;
    std::string Name;
    std::vector<Module> Modules;
    std::vector<Use> Uses;
    std::vector<Member> Members;
    std::map<std::string, std::shared_ptr<Nameable>> Symbols;
};

// Program represents a complete program
struct Program {
    std::vector<Package> Packages;
    Module MainModule;
    std::vector<Statement> Statements;
};

} // namespace scaly
