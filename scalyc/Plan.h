// Plan.h - Resolved/monomorphized model for code generation
// The Plan is the "execution plan" - concrete types with generics resolved,
// mangled names ready for LLVM emission, and provenance tracking for debug info.
//
// Designed to support full Hindley-Milner type inference from the start.

#pragma once

#include "Model.h"
#include "llvm/Support/Error.h"
#include <map>
#include <memory>
#include <set>
#include <string>
#include <variant>
#include <vector>

namespace scaly {

// ============================================================================
// Type Inference Support - Part 1: Basic types
// ============================================================================

// Type variable - represents an unknown type during inference
struct TypeVariable {
    uint64_t Id;
    std::string DebugName;  // Optional: "T", "?1", etc. for error messages

    bool operator==(const TypeVariable &Other) const { return Id == Other.Id; }
    bool operator<(const TypeVariable &Other) const { return Id < Other.Id; }
};

// ============================================================================
// Provenance Tracking (for debug info)
// ============================================================================

// Tracks where a generic instantiation came from
struct InstantiationInfo {
    Span DefinitionLoc;                  // Where the generic was defined
    Span InstantiationLoc;               // Where it was instantiated
    std::vector<std::string> TypeArgs;   // The concrete type arguments (readable)
};

// ============================================================================
// Planned Types (resolved, concrete)
// ============================================================================

// A fully resolved type
struct PlannedType {
    Span Loc;
    std::string Name;                    // Readable: "List.int"
    std::string MangledName;             // Itanium: "_Z4ListIiE"
    std::vector<PlannedType> Generics;   // Resolved generic args (if any)
    Lifetime Life;
    std::shared_ptr<InstantiationInfo> Origin;  // null if not from generic

    // For inference: may contain unresolved type variables
    std::shared_ptr<TypeVariable> Variable;  // non-null if this is a type variable

    // Helper: is this type fully resolved (no variables)?
    bool isResolved() const { return Variable == nullptr; }
};

// ============================================================================
// Type Inference Support - Part 2: Constraints and schemes
// ============================================================================

// A type can be concrete or a variable during inference
using TypeOrVariable = std::variant<PlannedType, TypeVariable>;

// Substitution: TypeVariable → Type mapping from unification
using Substitution = std::map<uint64_t, PlannedType>;

// Type constraint kinds for inference
struct EqualityConstraint {
    TypeOrVariable Left;
    TypeOrVariable Right;
    Span Loc;  // Where the constraint originated (for error messages)
};

struct InstanceConstraint {
    TypeVariable Var;
    std::string TraitName;  // e.g., "Hashable", "Comparable"
    Span Loc;
};

using TypeConstraint = std::variant<EqualityConstraint, InstanceConstraint>;

// Type scheme for let-polymorphism: ∀T1,T2,...Tn. Type
// e.g., identity function: ∀T. T → T
struct TypeScheme {
    std::vector<TypeVariable> Quantified;  // Bound type variables
    std::shared_ptr<PlannedType> Type;
};

// ============================================================================
// Forward declarations for Plan types
// ============================================================================

struct PlannedFunction;
struct PlannedOperator;
struct PlannedConcept;
struct PlannedModule;
struct PlannedOperand;
struct PlannedAction;

// ============================================================================
// Planned Attributes and Items
// ============================================================================

using PlannedConstant = Constant;

struct PlannedAttribute {
    Span Loc;
    std::string Name;
    Model Value;
};

struct PlannedItem {
    Span Loc;
    bool Private;
    std::shared_ptr<std::string> Name;
    std::shared_ptr<PlannedType> ItemType;
    std::vector<PlannedAttribute> Attributes;
};

// ============================================================================
// Planned Properties and Variants
// ============================================================================

struct PlannedProperty {
    Span Loc;
    bool Private;
    std::string Name;
    std::string MangledName;
    PlannedType PropType;
    std::shared_ptr<std::vector<PlannedOperand>> Initializer;
    std::vector<PlannedAttribute> Attributes;
    size_t Offset;  // Byte offset in struct layout
};

struct PlannedVariant {
    Span Loc;
    std::string Name;
    std::string MangledName;
    std::shared_ptr<PlannedType> VarType;
    std::vector<PlannedAttribute> Attributes;
    int Tag;  // Discriminator value
};

// ============================================================================
// Planned Statements
// ============================================================================

struct PlannedAction {
    std::vector<PlannedOperand> Source;
    std::vector<PlannedOperand> Target;
    PlannedType ResultType;  // Computed type of the Source operation sequence
};

struct PlannedBinding {
    Span Loc;
    std::string BindingType;  // "const", "var", "mutable"
    PlannedItem BindingItem;
    std::vector<PlannedOperand> Operation;
};

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

// ============================================================================
// Planned Control Flow
// ============================================================================

struct PlannedBlock {
    Span Loc;
    std::vector<PlannedStatement> Statements;
};

struct PlannedIf {
    Span Loc;
    std::vector<PlannedOperand> Condition;
    std::shared_ptr<PlannedProperty> Prop;
    std::shared_ptr<PlannedStatement> Consequent;
    std::shared_ptr<PlannedStatement> Alternative;
};

struct PlannedCase {
    Span Loc;
    std::vector<PlannedOperand> Condition;
};

struct PlannedBranch {
    Span Loc;
    std::vector<PlannedCase> Cases;
    std::shared_ptr<PlannedStatement> Consequent;
};

struct PlannedMatch {
    Span Loc;
    std::vector<PlannedOperand> Condition;
    std::vector<PlannedBranch> Branches;
    std::shared_ptr<PlannedStatement> Alternative;
};

struct PlannedWhen {
    Span Loc;
    std::string Name;
    PlannedType VariantType;
    std::shared_ptr<PlannedStatement> Consequent;
};

struct PlannedChoose {
    Span Loc;
    std::vector<PlannedOperand> Condition;
    std::vector<PlannedWhen> Cases;
    std::shared_ptr<PlannedStatement> Alternative;
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
    std::shared_ptr<PlannedStatement> Alternative;
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

// Forward declaration for PlannedCall
struct PlannedOperand;

// Call to a function or operator
struct PlannedCall {
    Span Loc;
    std::string Name;           // Operator symbol or function name: "+", "print", etc.
    std::string MangledName;    // Mangled name for linking
    bool IsIntrinsic;           // True for stdlib intrinsic ops (emit LLVM instruction)
    bool IsOperator;            // True for operators, false for functions
    std::shared_ptr<std::vector<PlannedOperand>> Args;  // Arguments (shared_ptr to break cycle)
    PlannedType ResultType;     // Return type
};

// ============================================================================
// Planned Tuples and Matrices
// ============================================================================

struct PlannedComponent {
    Span Loc;
    std::shared_ptr<std::string> Name;
    std::vector<PlannedOperand> Value;
    std::vector<PlannedAttribute> Attributes;
};

struct PlannedTuple {
    Span Loc;
    std::vector<PlannedComponent> Components;
    PlannedType TupleType;
};

struct PlannedMatrix {
    Span Loc;
    std::vector<std::vector<PlannedOperand>> Operations;
    PlannedType ElementType;
};

// ============================================================================
// Planned Expressions and Operands
// ============================================================================

// Represents a reference to a local variable
struct PlannedVariable {
    Span Loc;
    std::string Name;           // The variable name
    PlannedType VariableType;   // The type of the variable
    bool IsMutable;             // True if it's a var/mutable binding
};

using PlannedExpression = std::variant<
    PlannedConstant,
    PlannedType,
    PlannedVariable,
    PlannedCall,
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

// Member access step with field index for code generation
struct PlannedMemberAccess {
    std::string Name;        // Field name
    size_t FieldIndex;       // Index in struct layout
    PlannedType ResultType;  // Type after this access
};

struct PlannedOperand {
    Span Loc;
    PlannedExpression Expr;
    std::shared_ptr<std::vector<PlannedMemberAccess>> MemberAccess;
    PlannedType ResultType;  // The computed/inferred result type
};

// ============================================================================
// Planned Functions and Operators
// ============================================================================

// Planned implementation variants (Action is already defined above as PlannedAction)
struct PlannedExternImpl {
    Span Loc;
};

struct PlannedInstructionImpl {
    Span Loc;
};

struct PlannedIntrinsicImpl {
    Span Loc;
};

using PlannedImplementation = std::variant<
    PlannedAction,
    PlannedExternImpl,
    PlannedInstructionImpl,
    PlannedIntrinsicImpl
>;

struct PlannedFunction {
    Span Loc;
    bool Private;
    bool Pure;
    std::string Name;           // Readable: "List.get"
    std::string MangledName;    // Itanium: "_ZN4ListIiE3getEi"
    std::vector<PlannedItem> Input;
    std::shared_ptr<PlannedType> Returns;
    std::shared_ptr<PlannedType> Throws;
    Lifetime Life;
    PlannedImplementation Impl;
    std::shared_ptr<InstantiationInfo> Origin;

    // For polymorphic functions: the type scheme before instantiation
    std::shared_ptr<TypeScheme> Scheme;
};

struct PlannedInitializer {
    Span Loc;
    bool Private;
    std::string MangledName;
    std::vector<PlannedItem> Input;
    PlannedImplementation Impl;
};

struct PlannedDeInitializer {
    Span Loc;
    std::string MangledName;
    PlannedImplementation Impl;
};

struct PlannedOperator {
    Span Loc;
    bool Private;
    std::string Name;
    std::string MangledName;
    std::vector<PlannedItem> Input;
    std::shared_ptr<PlannedType> Returns;
    std::shared_ptr<PlannedType> Throws;
    PlannedImplementation Impl;
    std::shared_ptr<InstantiationInfo> Origin;
    std::shared_ptr<TypeScheme> Scheme;
};

// ============================================================================
// Planned Globals and Definitions
// ============================================================================

struct PlannedGlobal {
    Span Loc;
    std::string Name;
    std::string MangledName;
    PlannedType GlobalType;
    std::vector<PlannedOperand> Value;
};

struct PlannedStructure {
    Span Loc;
    bool Private = false;
    bool IsGenericPlaceholder = false;  // True for uninstantiated generic types
    std::string Name;
    std::string MangledName;
    std::vector<PlannedProperty> Properties;
    std::vector<PlannedInitializer> Initializers;
    std::shared_ptr<PlannedDeInitializer> Deinitializer;
    std::vector<PlannedFunction> Methods;
    std::vector<PlannedOperator> Operators;
    size_t Size = 0;       // Total size in bytes
    size_t Alignment = 0;  // Required alignment
    std::shared_ptr<InstantiationInfo> Origin;
};

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
    std::shared_ptr<InstantiationInfo> Origin;
};

struct PlannedNamespace {
    Span Loc;
    std::string Name;
    std::vector<PlannedModule> Modules;
    std::vector<PlannedFunction> Functions;
    std::vector<PlannedOperator> Operators;
    std::vector<PlannedConcept> Concepts;
};

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
    std::shared_ptr<InstantiationInfo> Origin;
};

// ============================================================================
// Planned Module and Program
// ============================================================================

struct PlannedModule {
    std::string File;
    std::string Name;
    std::vector<PlannedModule> Modules;
    std::vector<PlannedConcept> Concepts;
    std::vector<PlannedFunction> Functions;
    std::vector<PlannedOperator> Operators;
};

// ============================================================================
// The Complete Plan
// ============================================================================

struct Plan {
    PlannedModule MainModule;
    std::vector<PlannedStatement> Statements;  // Top-level program statements

    // All instantiated types/functions (for quick lookup during emission)
    std::map<std::string, PlannedStructure*> Structures;
    std::map<std::string, PlannedUnion*> Unions;
    std::map<std::string, PlannedFunction*> Functions;

    // Type inference state (used during planning, cleared after)
    uint64_t NextTypeVarId = 0;
    std::vector<TypeConstraint> Constraints;
    Substitution Solution;

    // Generate a fresh type variable
    TypeVariable freshTypeVar(const std::string &DebugName = "") {
        return TypeVariable{NextTypeVarId++, DebugName};
    }

    // Add a constraint
    void addConstraint(TypeConstraint C) {
        Constraints.push_back(std::move(C));
    }

    // Add equality constraint (convenience)
    void constrain(const TypeOrVariable &Left, const TypeOrVariable &Right, Span Loc) {
        Constraints.push_back(EqualityConstraint{Left, Right, Loc});
    }
};

// ============================================================================
// Type Inference Utilities
// ============================================================================

// Apply a substitution to a type, resolving type variables
PlannedType applySubstitution(const PlannedType &Type, const Substitution &Subst);

// Unify two types, producing a substitution or error
llvm::Expected<Substitution> unify(const TypeOrVariable &Left,
                                    const TypeOrVariable &Right,
                                    Span Loc,
                                    llvm::StringRef File);

// Solve a set of constraints
llvm::Expected<Substitution> solveConstraints(const std::vector<TypeConstraint> &Constraints,
                                               llvm::StringRef File);

// Generalize a type to a type scheme (for let-polymorphism)
TypeScheme generalize(const PlannedType &Type,
                      const std::set<uint64_t> &FreeInEnvironment);

// Instantiate a type scheme with fresh type variables
PlannedType instantiate(const TypeScheme &Scheme, Plan &P);

// Get free type variables in a type
std::set<uint64_t> freeTypeVars(const PlannedType &Type);

// Check if a type contains a specific type variable (occurs check)
bool occursIn(uint64_t VarId, const PlannedType &Type);

} // namespace scaly
