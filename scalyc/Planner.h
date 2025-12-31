// Planner.h - Generic resolution and name mangling
// Transforms Model (with generics) → Plan (concrete types, mangled names)

#pragma once

#include "Model.h"
#include "Plan.h"
#include "PlannerError.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/Support/Error.h"
#include <map>
#include <set>
#include <string>
#include <vector>

namespace scaly {

class Planner {
public:
    explicit Planner(llvm::StringRef FileName);

    // Main entry point - resolve a program to a plan
    llvm::Expected<Plan> plan(const Program &Prog);

private:
    std::string File;

    // Symbol tables for resolution (flat cache, populated during planning)
    std::map<std::string, const Concept*> Concepts;
    std::map<std::string, std::vector<const Function*>> Functions;
    std::map<std::string, const Operator*> Operators;

    // Module context for cross-module resolution
    std::vector<const Module*> ModuleStack;

    // Lookup a concept by name, searching module hierarchy
    const Concept* lookupConcept(llvm::StringRef Name);

    // Instantiation cache - avoid duplicate monomorphization
    std::map<std::string, PlannedStructure> InstantiatedStructures;
    std::map<std::string, PlannedUnion> InstantiatedUnions;
    std::map<std::string, PlannedFunction> InstantiatedFunctions;

    // Current generic substitution context
    std::map<std::string, PlannedType> TypeSubstitutions;

    // Scope stack for local variables
    std::vector<std::map<std::string, PlannedType>> Scopes;

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
    void defineLocal(llvm::StringRef Name, const PlannedType &Type);
    std::optional<PlannedType> lookupLocal(llvm::StringRef Name);

    // ========== Type Utilities ==========

    // Get the readable name for a type (for error messages, debug info)
    std::string getReadableName(const PlannedType &Type);

    // Check if two types are equal
    bool typesEqual(const PlannedType &A, const PlannedType &B);

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
