// Plan.h - Resolved/monomorphized model for code generation
// The Plan is the "execution plan" - concrete types with generics resolved,
// mangled names ready for LLVM emission, and provenance tracking for debug info.

#pragma once

#include "Model.h"
#include <map>
#include <memory>
#include <string>
#include <variant>
#include <vector>

namespace scaly {

// Tracks where a generic instantiation came from (for debug info)
struct InstantiationInfo {
    Span DefinitionLoc;                  // Where the generic was defined
    Span InstantiationLoc;               // Where it was instantiated
    std::vector<std::string> TypeArgs;   // The concrete type arguments
};

// A resolved type - concrete, with optional provenance if from generic
struct PlannedType {
    Span Loc;
    std::string Name;                    // Readable: "List.int"
    std::string MangledName;             // Itanium: "_Z4ListIiE"
    std::vector<PlannedType> Generics;   // Resolved generic args (if any)
    Lifetime Life;
    std::unique_ptr<InstantiationInfo> Origin;  // null if not from generic
};

// Forward declarations
struct PlannedFunction;
struct PlannedOperator;
struct PlannedConcept;
struct PlannedModule;

// Resolved constant (same as Model, but with resolved types)
using PlannedConstant = Constant;

// Resolved attribute
struct PlannedAttribute {
    Span Loc;
    std::string Name;
    Model Value;
};

// Resolved item (parameter/binding target)
struct PlannedItem {
    Span Loc;
    bool Private;
    std::unique_ptr<std::string> Name;
    std::unique_ptr<PlannedType> ItemType;
    std::vector<PlannedAttribute> Attributes;
};

// Resolved property (struct field)
struct PlannedProperty {
    Span Loc;
    bool Private;
    std::string Name;
    std::string MangledName;
    PlannedType PropType;
    std::unique_ptr<std::vector<PlannedOperand>> Initializer;
    std::vector<PlannedAttribute> Attributes;
    size_t Offset;  // Byte offset in struct layout
};

// Resolved variant (union case)
struct PlannedVariant {
    Span Loc;
    std::string Name;
    std::string MangledName;
    std::unique_ptr<PlannedType> VarType;
    std::vector<PlannedAttribute> Attributes;
    int Tag;  // Discriminator value
};

// Forward declare for recursive types
struct PlannedOperand;
struct PlannedStatement;
struct PlannedAction;

// Resolved action
struct PlannedAction {
    std::vector<PlannedOperand> Source;
    std::vector<PlannedOperand> Target;
};

// Resolved binding
struct PlannedBinding {
    Span Loc;
    std::string BindingType;  // "const", "var", "mutable"
    PlannedItem BindingItem;
    std::vector<PlannedOperand> Operation;
};

// Resolved control flow
struct PlannedBreak {
    Span Loc;
    std::vector<PlannedOperand> Result;
};

struct PlannedContinue {
    Span Loc;
};

struct PlannedReturn {
    Span Loc;
    std::vector<PlannedOperand> Result;
};

struct PlannedThrow {
    Span Loc;
    std::vector<PlannedOperand> Result;
};

using PlannedStatement = std::variant<
    PlannedAction,
    PlannedBinding,
    PlannedBreak,
    PlannedContinue,
    PlannedReturn,
    PlannedThrow
>;

// Resolved block
struct PlannedBlock {
    Span Loc;
    std::vector<PlannedStatement> Statements;
};

// Resolved control flow expressions
struct PlannedIf {
    Span Loc;
    std::vector<PlannedOperand> Condition;
    std::unique_ptr<PlannedProperty> Prop;
    std::unique_ptr<PlannedStatement> Consequent;
    std::unique_ptr<PlannedStatement> Alternative;
};

struct PlannedCase {
    Span Loc;
    std::vector<PlannedOperand> Condition;
};

struct PlannedBranch {
    Span Loc;
    std::vector<PlannedCase> Cases;
    std::unique_ptr<PlannedStatement> Consequent;
};

struct PlannedMatch {
    Span Loc;
    std::vector<PlannedOperand> Condition;
    std::vector<PlannedBranch> Branches;
    std::unique_ptr<PlannedStatement> Alternative;
};

struct PlannedWhen {
    Span Loc;
    std::string Name;
    PlannedType VariantType;
    std::unique_ptr<PlannedStatement> Consequent;
};

struct PlannedChoose {
    Span Loc;
    std::vector<PlannedOperand> Condition;
    std::vector<PlannedWhen> Cases;
    std::unique_ptr<PlannedStatement> Alternative;
};

struct PlannedFor {
    Span Loc;
    std::string Identifier;
    std::vector<PlannedOperand> Expr;
    PlannedAction Body;
};

struct PlannedWhile {
    Span Loc;
    PlannedBinding Cond;
    PlannedAction Body;
};

struct PlannedTry {
    Span Loc;
    PlannedBinding Cond;
    std::vector<PlannedWhen> Catches;
    std::unique_ptr<PlannedStatement> Alternative;
};

struct PlannedSizeOf {
    Span Loc;
    PlannedType SizedType;
    size_t Size;  // Computed size in bytes
};

struct PlannedIs {
    Span Loc;
    PlannedType TestType;
};

// Resolved tuple component
struct PlannedComponent {
    Span Loc;
    std::unique_ptr<std::string> Name;
    std::vector<PlannedOperand> Value;
    std::vector<PlannedAttribute> Attributes;
};

// Resolved tuple
struct PlannedTuple {
    Span Loc;
    std::vector<PlannedComponent> Components;
    PlannedType TupleType;  // The resolved constructor type
};

// Resolved matrix
struct PlannedMatrix {
    Span Loc;
    std::vector<std::vector<PlannedOperand>> Operations;
    PlannedType ElementType;
};

// Resolved expression
using PlannedExpression = std::variant<
    PlannedConstant,
    PlannedType,
    PlannedTuple,
    PlannedMatrix,
    PlannedBlock,
    PlannedIf,
    PlannedMatch,
    PlannedChoose,
    PlannedFor,
    PlannedWhile,
    PlannedTry,
    PlannedSizeOf,
    PlannedIs
>;

// Resolved operand
struct PlannedOperand {
    Span Loc;
    PlannedExpression Expr;
    std::unique_ptr<std::vector<std::string>> MemberAccess;
    PlannedType ResultType;  // The computed result type
};

// Implementation variants (same as Model)
using PlannedImplementation = Implementation;

// Resolved function
struct PlannedFunction {
    Span Loc;
    bool Private;
    bool Pure;
    std::string Name;           // Readable: "List.get"
    std::string MangledName;    // Itanium: "_ZN4ListIiE3getEi"
    std::vector<PlannedItem> Input;
    std::unique_ptr<PlannedType> Returns;
    std::unique_ptr<PlannedType> Throws;
    Lifetime Life;
    PlannedImplementation Impl;
    std::unique_ptr<InstantiationInfo> Origin;
};

// Resolved initializer
struct PlannedInitializer {
    Span Loc;
    bool Private;
    std::string MangledName;
    std::vector<PlannedItem> Input;
    PlannedImplementation Impl;
};

// Resolved deinitializer
struct PlannedDeInitializer {
    Span Loc;
    std::string MangledName;
    PlannedImplementation Impl;
};

// Resolved operator
struct PlannedOperator {
    Span Loc;
    bool Private;
    std::string Name;
    std::string MangledName;
    std::vector<PlannedItem> Input;
    std::unique_ptr<PlannedType> Returns;
    std::unique_ptr<PlannedType> Throws;
    PlannedImplementation Impl;
    std::unique_ptr<InstantiationInfo> Origin;
};

// Resolved global constant
struct PlannedGlobal {
    Span Loc;
    std::string Name;
    std::string MangledName;
    PlannedType GlobalType;
    std::vector<PlannedOperand> Value;
};

// Resolved structure
struct PlannedStructure {
    Span Loc;
    bool Private;
    std::string Name;
    std::string MangledName;
    std::vector<PlannedProperty> Properties;
    std::vector<PlannedInitializer> Initializers;
    std::unique_ptr<PlannedDeInitializer> Deinitializer;
    std::vector<PlannedFunction> Methods;
    std::vector<PlannedOperator> Operators;
    size_t Size;       // Total size in bytes
    size_t Alignment;  // Required alignment
    std::unique_ptr<InstantiationInfo> Origin;
};

// Resolved union
struct PlannedUnion {
    Span Loc;
    bool Private;
    std::string Name;
    std::string MangledName;
    std::vector<PlannedVariant> Variants;
    std::vector<PlannedFunction> Methods;
    std::vector<PlannedOperator> Operators;
    size_t Size;       // Max variant size + tag
    size_t Alignment;
    std::unique_ptr<InstantiationInfo> Origin;
};

// Resolved namespace (no mangling needed - just a container)
struct PlannedNamespace {
    Span Loc;
    std::string Name;
    std::vector<PlannedFunction> Functions;
    std::vector<PlannedOperator> Operators;
    std::vector<PlannedConcept> Concepts;
};

// Resolved concept (type definition)
struct PlannedConcept {
    Span Loc;
    std::string Name;
    std::string MangledName;
    std::vector<PlannedAttribute> Attributes;
    std::variant<
        PlannedStructure,
        PlannedUnion,
        PlannedNamespace,
        PlannedGlobal,
        PlannedType  // Type alias
    > Definition;
    std::unique_ptr<InstantiationInfo> Origin;
};

// Resolved module
struct PlannedModule {
    std::string File;
    std::string Name;
    std::vector<PlannedModule> Modules;
    std::vector<PlannedConcept> Concepts;
    std::vector<PlannedFunction> Functions;
    std::vector<PlannedOperator> Operators;
};

// The complete execution plan
struct Plan {
    PlannedModule MainModule;
    std::vector<PlannedStatement> Statements;  // Top-level program statements

    // All instantiated types/functions (for quick lookup during emission)
    std::map<std::string, PlannedStructure*> Structures;
    std::map<std::string, PlannedUnion*> Unions;
    std::map<std::string, PlannedFunction*> Functions;
};

} // namespace scaly
