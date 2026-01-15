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

    // Add a package search path for on-demand loading
    void addPackageSearchPath(llvm::StringRef Path);

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

    // On-demand loaded packages (owns the Program objects)
    std::vector<std::unique_ptr<Program>> OnDemandPackages;

    // Cache of loaded module paths (to prevent infinite recursion)
    std::map<std::string, const Module*> LoadedModulePaths;

    // Modules accessed via on-demand loading (for sibling concept lookup)
    // When we find Page in scaly.memory.Page, we cache the Page module
    // so later lookups for PAGE_SIZE can find it as a sibling
    std::vector<const Module*> AccessedPackageModules;

    // Package search paths for on-demand loading
    std::vector<std::string> PackageSearchPaths;

    // Built-in runtime functions for RBMM (aligned_alloc, free, exit)
    std::vector<std::unique_ptr<Function>> BuiltinFunctions;
    bool RuntimeFunctionsRegistered = false;
    void registerRuntimeFunctions();

    // Module context for cross-module resolution
    std::vector<const Module*> ModuleStack;

    // Lookup a concept by name, searching module hierarchy
    const Concept* lookupConcept(llvm::StringRef Name);

    // Load a package on demand from search paths
    // Returns the root module of the loaded package, or nullptr if not found
    const Module* loadPackageOnDemand(llvm::StringRef PackageName);

    // Load an intra-package module on demand (e.g., "containers" from within scaly package)
    // BasePath: path to the parent directory (e.g., "packages/scaly/0.1.0/scaly")
    // ModuleName: name of the module to load (e.g., "containers")
    const Module* loadIntraPackageModule(llvm::StringRef BasePath, llvm::StringRef ModuleName);

    // Look up a function by qualified package path (e.g., ["scaly", "containers", "test"])
    const Function* lookupPackageFunction(const std::vector<std::string>& Path);

    // Instantiation cache - avoid duplicate monomorphization
    std::map<std::string, PlannedStructure> InstantiatedStructures;
    std::map<std::string, PlannedUnion> InstantiatedUnions;
    std::map<std::string, PlannedFunction> InstantiatedFunctions;

    // Track functions currently being planned to handle recursive calls
    std::set<const Function*> FunctionsBeingPlanned;

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

    // ========== Planning Context ==========
    // Grouped state for current planning scope

    // Structure context - active when planning methods/properties of a struct
    struct StructureContext {
        std::string Name;
        std::vector<PlannedProperty>* Properties = nullptr;
        PlannedStructure* Structure = nullptr;

        bool isActive() const { return !Name.empty(); }
    };
    StructureContext CurrentStructureCtx;

    // Namespace context - active when planning namespace members
    struct NamespaceContext {
        std::string Name;
        const Namespace* NS = nullptr;
        std::vector<const Module*> Modules;

        bool isActive() const { return !Name.empty(); }
    };
    NamespaceContext CurrentNamespaceCtx;

    // Legacy accessors for gradual migration
    std::string& CurrentStructureName = CurrentStructureCtx.Name;
    std::vector<PlannedProperty>*& CurrentStructureProperties = CurrentStructureCtx.Properties;
    PlannedStructure*& CurrentStructure = CurrentStructureCtx.Structure;
    std::string& CurrentNamespaceName = CurrentNamespaceCtx.Name;
    const Namespace*& CurrentNamespace = CurrentNamespaceCtx.NS;
    std::vector<const Module*>& CurrentNamespaceModules = CurrentNamespaceCtx.Modules;

    // Track if current function uses $ allocations (needs local page)
    bool CurrentFunctionUsesLocalLifetime = false;

    // Recursion depth tracking for stack overflow prevention
    // Note: Each planning frame is ~200KB due to std::variant and locals
    // Max depth of 200 requires ~40MB stack - use ulimit -s unlimited for large files
    size_t PlanningDepth = 0;
    static constexpr size_t MaxPlanningDepth = 300;  // Conservative limit
    size_t MaxDepthSeen = 0;  // For debugging

    // Track per-scope $ allocations for block-scoped cleanup
    struct ScopeInfo {
        bool HasLocalAllocations = false;  // True if this scope has $ allocations
    };
    std::vector<ScopeInfo> ScopeInfoStack;

    // ========== Iterative Processing Infrastructure ==========
    // Work queue for converting recursive planning to iterative processing.
    // This prevents stack overflow on deeply nested code.

    // Result storage location for deferred work items
    struct ResultSlot {
        enum class Kind {
            StatementVec,      // Append to std::vector<PlannedStatement>*
            Statement,         // Store in PlannedStatement*
            Block,             // Store in PlannedBlock*
            If,                // Store in PlannedIf*
            Match,             // Store in PlannedMatch*
            Choose,            // Store in PlannedChoose*
            For,               // Store in PlannedFor*
            While,             // Store in PlannedWhile*
        };
        Kind SlotKind;
        void* Storage;
        size_t Index = 0;  // For vector slots
    };

    // A pending work item for iterative processing
    struct WorkItem {
        enum class Kind {
            PlanStatementList,   // Plan a list of statements
            PlanSingleStatement, // Plan one statement
            PlanBlock,           // Plan a block
            PlanIfConsequent,    // Plan if consequent
            PlanIfAlternative,   // Plan if alternative
            PlanMatchBranch,     // Plan match branch
            PlanChooseWhen,      // Plan choose when branch
            PlanForBody,         // Plan for loop body
            PlanWhileBody,       // Plan while loop body
            PopScope,            // Pop a scope after block processing
            FinalizeBlock,       // Finalize block after statements planned
            FinalizeIf,          // Finalize if after branches planned
        };
        Kind ItemKind;

        // Input data (depending on ItemKind)
        const std::vector<Statement>* Statements = nullptr;
        const Statement* Stmt = nullptr;
        const Block* Blk = nullptr;
        const If* IfExpr = nullptr;
        const Match* MatchExpr = nullptr;
        const Choose* ChooseExpr = nullptr;
        const For* ForExpr = nullptr;
        const While* WhileExpr = nullptr;
        size_t Index = 0;  // Current position in list processing

        // Output location
        ResultSlot Result;

        // Context for continuation (what to do after this item completes)
        PlannedBlock* BlockResult = nullptr;
        PlannedIf* IfResult = nullptr;
        PlannedMatch* MatchResult = nullptr;
        PlannedChoose* ChooseResult = nullptr;
        PlannedFor* ForResult = nullptr;
        PlannedWhile* WhileResult = nullptr;
    };

    // Work queue for iterative processing
    std::vector<WorkItem> WorkQueue;

    // Process work items iteratively until queue is empty
    llvm::Expected<bool> processWorkQueue();

    // Push a work item onto the queue
    void pushWork(WorkItem Item);

    // Helper to plan statements iteratively (entry point for iterative mode)
    llvm::Expected<std::vector<PlannedStatement>> planStatementsIterative(
        const std::vector<Statement>& Stmts);

    // ========== Type Inference State ==========

    // Current plan being built (for constraint collection)
    Plan* CurrentPlan = nullptr;

    // Expected type context for generic type inference in set statements
    // When set, allows generic types to infer their type arguments from the target
    std::optional<PlannedType> ExpectedTypeContext;

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
                                                    const std::vector<PlannedType> &GenericArgs,
                                                    bool lazy = false);
    llvm::Expected<PlannedUnion> planUnion(const Union &Un,
                                            llvm::StringRef Name,
                                            const std::vector<PlannedType> &GenericArgs,
                                            bool lazy = false);

    // On-demand method planning for lazy instantiation
    llvm::Expected<PlannedFunction*> planDeferredMethod(PlannedStructure &Struct,
                                                         const std::string &MethodName);
    llvm::Expected<PlannedFunction*> planDeferredMethod(PlannedUnion &Un,
                                                         const std::string &MethodName);
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

    // Get operator precedence (higher = binds tighter, 0 = unknown)
    int getOperatorPrecedence(llvm::StringRef Op);

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
        bool RequiresPageParam = false;  // True if function# - method needs page parameter
        bool CanThrow = false;  // True if method can throw
        std::shared_ptr<PlannedType> ThrowsType;  // Exception type if CanThrow is true
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
        bool RequiresPageParam = false;  // True if init# - initializer needs page parameter
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

    // ========== Method Call Helpers ==========

    // Generate a Page.get(this) call for ^this lifetime annotation
    // Returns a PlannedOperand with result type pointer[Page]
    llvm::Expected<PlannedOperand> generatePageGetThis(Span Loc);

    // Extract argument types from a planned operand containing a tuple
    std::vector<PlannedType> extractArgTypes(const PlannedOperand &ArgsOp);

    // Create a PlannedCall for a method invocation
    PlannedCall createMethodCall(
        const MethodMatch &Match,
        PlannedOperand InstanceOp,
        const PlannedOperand &ArgsOp,
        Span Loc);

    // ========== Pattern Detection Helpers ==========

    // Check if a name refers to a local variable or property in current struct
    struct BindingInfo {
        bool IsLocal = false;      // True if it's a local variable
        bool IsProperty = false;   // True if it's a property of current struct
        std::optional<LocalBinding> Binding;  // The binding if IsLocal
    };
    BindingInfo checkLocalOrProperty(llvm::StringRef Name);

    // Result of trying to plan a method call
    struct MethodCallResult {
        bool Matched = false;
        PlannedOperand CallOp;
        size_t ConsumedOperands = 0;  // How many operands were consumed (for index advancement)
    };

    // Process chained method calls on an operand (e.g., expr.method1().method2().method3())
    // Returns the final operand and number of additional operands consumed
    llvm::Expected<MethodCallResult> processChainedMethodCalls(
        PlannedOperand InitialOp,
        const std::vector<Operand> &Ops,
        size_t StartIndex);  // Index of the first tuple (args for initial call)
};

} // namespace scaly
