// Planner.h - Generic resolution and name mangling
// Transforms Model (with generics) → Plan (concrete types, mangled names)

#pragma once

#include "Model.h"
#include "Plan.h"
#include "PlannerError.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/Support/Error.h"
#include <map>
#include <optional>
#include <set>
#include <string>
#include <vector>

namespace scaly {

class Planner {
public:
    explicit Planner(llvm::StringRef FileName);

    // Main entry point - resolve a program to a plan
    llvm::Expected<Plan> plan(const Program &Prog);

    // Add a sibling program (for multi-file compilation)
    void addSiblingProgram(std::shared_ptr<Program> Prog);

private:
    std::string File;

    // Sibling programs for multi-file compilation
    std::vector<std::shared_ptr<Program>> SiblingPrograms;

    // Symbol tables for resolution (flat cache, populated during planning)
    std::map<std::string, const Concept*> Concepts;
    std::map<std::string, std::vector<const Function*>> Functions;
    std::map<std::string, std::vector<const Operator*>> Operators;

    // Loaded packages (name -> root module)
    std::map<std::string, const Module*> LoadedPackages;

    // Built-in runtime functions for RBMM (aligned_alloc, free, exit)
    std::vector<std::unique_ptr<Function>> BuiltinFunctions;
    bool RuntimeFunctionsRegistered = false;
    void registerRuntimeFunctions();

    // Module context for cross-module resolution
    std::vector<const Module*> ModuleStack;

    // Lookup a concept by name, searching module hierarchy
    const Concept* lookupConcept(llvm::StringRef Name);

    // Look up a function by qualified package path (e.g., ["scaly", "containers", "test"])
    const Function* lookupPackageFunction(const std::vector<std::string>& Path);

    // Instantiation cache - avoid duplicate monomorphization
    std::map<std::string, PlannedStructure> InstantiatedStructures;
    std::map<std::string, PlannedUnion> InstantiatedUnions;
    std::map<std::string, PlannedFunction> InstantiatedFunctions;

    // Global constants cache - for constant lookup during expression planning
    std::map<std::string, PlannedGlobal> PlannedGlobals;

    // Current generic substitution context
    std::map<std::string, PlannedType> TypeSubstitutions;

    // Information about a local binding
    struct LocalBinding {
        PlannedType Type;
        bool IsMutable;
        bool IsOnPage;  // true if allocated with $, #, or ^name (not stack)
    };

    // Scope stack for local variables
    std::vector<std::map<std::string, LocalBinding>> Scopes;

    // Current structure being planned (for property access in methods)
    std::string CurrentStructureName;
    std::vector<PlannedProperty>* CurrentStructureProperties = nullptr;
    PlannedStructure* CurrentStructure = nullptr;  // For method lookup during planning

    // Current namespace being planned (for sibling function calls in define blocks)
    std::string CurrentNamespaceName;
    const Namespace* CurrentNamespace = nullptr;  // Pointer to namespace for function lookup

    // Track if current function uses $ allocations (needs local page)
    bool CurrentFunctionUsesLocalLifetime = false;

    // Track per-scope $ allocations for block-scoped cleanup
    struct ScopeInfo {
        bool HasLocalAllocations = false;  // True if this scope has $ allocations
    };
    std::vector<ScopeInfo> ScopeInfoStack;

    // ========== Type Inference State ==========

    // Current plan being built (for constraint collection)
    Plan* CurrentPlan = nullptr;

    // Create a fresh type variable
    PlannedType freshTypeVar(const std::string &DebugName = "");

    // Add an equality constraint
    void addConstraint(const PlannedType &Left, const PlannedType &Right, Span Loc);

    // Solve collected constraints and apply solution
    llvm::Expected<Substitution> solveAndApply();

    // Apply substitution to a planned type (resolving type variables)
    PlannedType applyCurrentSubstitution(const PlannedType &Type);

    // ========== Name Mangling (Itanium ABI inspired) ==========

    // Mangle a type name
    std::string mangleType(const PlannedType &Type);

    // Mangle a function name
    std::string mangleFunction(llvm::StringRef Name,
                               const std::vector<PlannedItem> &Params,
                               const PlannedType *Parent = nullptr);

    // Mangle an operator name
    std::string mangleOperator(llvm::StringRef Name,
                               const std::vector<PlannedItem> &Params,
                               const PlannedType *Parent = nullptr);

    // Mangle a structure/union name with generic args
    std::string mangleStructure(llvm::StringRef Name,
                                const std::vector<PlannedType> &GenericArgs);

    // Encode a type for mangling (i=int, d=double, etc.)
    std::string encodeType(const PlannedType &Type);

    // Encode a name with length prefix
    std::string encodeName(llvm::StringRef Name);

    // ========== Type Resolution ==========

    // Resolve a Model::Type to a PlannedType
    llvm::Expected<PlannedType> resolveType(const Type &T, Span InstantiationLoc);

    // Resolve a type name path (e.g., ["List", "Node"])
    llvm::Expected<PlannedType> resolveTypePath(const std::vector<std::string> &Path,
                                                 Span Loc);

    // Instantiate a generic type with concrete arguments
    llvm::Expected<PlannedType> instantiateGeneric(const Concept &Generic,
                                                    const std::vector<PlannedType> &Args,
                                                    Span InstantiationLoc);

    // Instantiate a generic function with concrete type arguments
    llvm::Expected<PlannedFunction> instantiateGenericFunction(
        const Function &Func,
        const std::vector<PlannedType> &TypeArgs,
        Span InstantiationLoc);

    // Infer type arguments for a generic function call from argument types
    llvm::Expected<std::vector<PlannedType>> inferTypeArguments(
        const Function &Func,
        const std::vector<PlannedType> &ArgTypes,
        Span Loc);

    // ========== Expression/Statement Planning ==========

    llvm::Expected<PlannedOperand> planOperand(const Operand &Op);
    llvm::Expected<std::vector<PlannedOperand>> planOperands(
        const std::vector<Operand> &Ops);

    llvm::Expected<PlannedExpression> planExpression(const Expression &Expr);
    llvm::Expected<PlannedStatement> planStatement(const Statement &Stmt);
    llvm::Expected<std::vector<PlannedStatement>> planStatements(
        const std::vector<Statement> &Stmts);

    llvm::Expected<PlannedAction> planAction(const Action &Act);
    llvm::Expected<PlannedBinding> planBinding(const Binding &Bind);
    llvm::Expected<PlannedBlock> planBlock(const Block &Blk);

    // Control flow
    llvm::Expected<PlannedIf> planIf(const If &IfExpr);
    llvm::Expected<PlannedMatch> planMatch(const Match &MatchExpr);
    llvm::Expected<PlannedChoose> planChoose(const Choose &ChooseExpr);
    llvm::Expected<PlannedFor> planFor(const For &ForExpr);
    llvm::Expected<PlannedWhile> planWhile(const While &WhileExpr);
    llvm::Expected<PlannedTry> planTry(const Try &TryExpr);

    // Literals and constructors
    llvm::Expected<PlannedTuple> planTuple(const Tuple &Tup);
    llvm::Expected<PlannedMatrix> planMatrix(const Matrix &Mat);

    // ========== Definition Planning ==========

    llvm::Expected<PlannedImplementation> planImplementation(const Implementation &Impl);

    llvm::Expected<PlannedFunction> planFunction(const Function &Func,
                                                  const PlannedType *Parent = nullptr);
    llvm::Expected<PlannedOperator> planOperator(const Operator &Op,
                                                  const PlannedType *Parent = nullptr);
    llvm::Expected<PlannedConcept> planConcept(const Concept &Conc);

    llvm::Expected<PlannedStructure> planStructure(const Structure &Struct,
                                                    llvm::StringRef Name,
                                                    const std::vector<PlannedType> &GenericArgs);
    llvm::Expected<PlannedUnion> planUnion(const Union &Un,
                                            llvm::StringRef Name,
                                            const std::vector<PlannedType> &GenericArgs);
    llvm::Expected<PlannedNamespace> planNamespace(const Namespace &NS,
                                                    llvm::StringRef Name);

    llvm::Expected<PlannedProperty> planProperty(const Property &Prop);
    llvm::Expected<PlannedVariant> planVariant(const Variant &Var, int Tag);
    llvm::Expected<PlannedItem> planItem(const Item &It);

    llvm::Expected<PlannedInitializer> planInitializer(const Initializer &Init,
                                                        const PlannedType &Parent);
    llvm::Expected<PlannedDeInitializer> planDeInitializer(const DeInitializer &DeInit,
                                                            const PlannedType &Parent);

    // ========== Module Planning ==========

    llvm::Expected<PlannedModule> planModule(const Module &Mod);

    // ========== Scope Management ==========

    void pushScope();
    void popScope();
    void defineLocal(llvm::StringRef Name, const PlannedType &Type, bool IsMutable = false, bool IsOnPage = false);
    std::optional<PlannedType> lookupLocal(llvm::StringRef Name);
    std::optional<LocalBinding> lookupLocalBinding(llvm::StringRef Name);

    // ========== Type Utilities ==========

    // Get the readable name for a type (for error messages, debug info)
    std::string getReadableName(const PlannedType &Type);

    // Check if two types are equal
    bool typesEqual(const PlannedType &A, const PlannedType &B);

    // Check if ArgType can be implicitly converted to ParamType
    bool typesCompatible(const PlannedType &ParamType, const PlannedType &ArgType);

    // ========== Symbol Lookup ==========

    // Look up a function by name, returning candidates for overload resolution
    std::vector<const Function*> lookupFunction(llvm::StringRef Name);

    // Look up operators by name (returns all overloads for resolution)
    std::vector<const Operator*> lookupOperator(llvm::StringRef Name);

    // Check if a name refers to a function (vs variable/type)
    bool isFunction(llvm::StringRef Name);

    // Check if a name refers to an operator
    bool isOperatorName(llvm::StringRef Name);

    // ========== Operation Resolution ==========

    // Resolve an operation sequence (function calls and operators)
    // Returns the result type of the entire operation
    llvm::Expected<PlannedType> resolveOperationSequence(
        const std::vector<PlannedOperand> &Ops);

    // Collapse an operand sequence into a single operand with PlannedCall
    // Returns a single operand containing nested PlannedCall structures
    llvm::Expected<PlannedOperand> collapseOperandSequence(
        std::vector<PlannedOperand> Ops);

    // Find matching operator and check if it's intrinsic
    struct OperatorMatch {
        const Operator* Op;
        std::string MangledName;
        bool IsIntrinsic;
        PlannedType ResultType;  // For intrinsic ops like pointer arithmetic
    };
    std::optional<OperatorMatch> findOperator(
        llvm::StringRef Name, const PlannedType &Left, const PlannedType &Right);
    std::optional<OperatorMatch> findSubscriptOperator(
        const PlannedType &ContainerType, const PlannedType &IndexType, Span Loc);

    // Resolve a function call with given arguments
    llvm::Expected<PlannedType> resolveFunctionCall(
        llvm::StringRef Name, Span Loc,
        const std::vector<PlannedType> &ArgTypes);

    // Resolve an operator application
    llvm::Expected<PlannedType> resolveOperatorCall(
        llvm::StringRef Name, Span Loc,
        const PlannedType &Left, const PlannedType &Right);

    // Resolve a prefix (unary) operator application
    llvm::Expected<PlannedType> resolvePrefixOperatorCall(
        llvm::StringRef Name, Span Loc,
        const PlannedType &Operand);

    // Extract the element type from pointer[T], returning T
    std::optional<PlannedType> getPointerElementType(const PlannedType &PtrType);

    // ========== Member Access Resolution ==========

    // Resolve member access on a type, returning the member's type
    llvm::Expected<PlannedType> resolveMemberAccess(
        const PlannedType &BaseType,
        const std::vector<std::string> &Members,
        Span Loc);

    // Resolve member access chain, returning full PlannedMemberAccess info with field indices
    llvm::Expected<std::vector<PlannedMemberAccess>> resolveMemberAccessChain(
        const PlannedType &BaseType,
        const std::vector<std::string> &Members,
        Span Loc);

    // Overload for Type-based member access (extracts names from Types)
    llvm::Expected<std::vector<PlannedMemberAccess>> resolveMemberAccessChain(
        const PlannedType &BaseType,
        const std::vector<Type> &Members,
        Span Loc);

    // Look up a method on a struct type by name
    struct MethodMatch {
        const Function* Method;
        std::string MangledName;
        PlannedType ReturnType;
        std::vector<PlannedType> ParameterTypes;  // For overload resolution
    };
    std::optional<MethodMatch> lookupMethod(
        const PlannedType &StructType,
        llvm::StringRef MethodName,
        Span Loc);

    // Look up a method with overload resolution based on argument types
    std::optional<MethodMatch> lookupMethod(
        const PlannedType &StructType,
        llvm::StringRef MethodName,
        const std::vector<PlannedType> &ArgTypes,
        Span Loc);

    // Look up an initializer on a struct type matching the given argument types
    struct InitializerMatch {
        const PlannedInitializer* Init;
        std::string MangledName;
        PlannedType StructType;
    };
    std::optional<InitializerMatch> findInitializer(
        const PlannedType &StructType,
        const std::vector<PlannedType> &ArgTypes);

    // ========== Type Inference ==========

    // Infer the type of a constant
    PlannedType inferConstantType(const Constant &C);

    // Infer the type of an expression
    llvm::Expected<PlannedType> inferExpressionType(const PlannedExpression &Expr);

    // Resolve a name that could be a variable or type
    llvm::Expected<PlannedType> resolveNameOrVariable(const Type &T);

    // Compute struct layout (size, alignment, field offsets)
    void computeStructLayout(PlannedStructure &Struct);

    // Compute union layout (size = max variant, plus tag)
    void computeUnionLayout(PlannedUnion &Union);
};

} // namespace scaly
