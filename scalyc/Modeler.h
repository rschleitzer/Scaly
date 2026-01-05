// Modeler.h - Syntax to semantic model transformation
#pragma once

#include "Model.h"
#include "ModelError.h"
#include "Syntax.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/Support/Error.h"
#include <map>
#include <set>
#include <string>
#include <vector>

namespace scaly {

class Modeler {
public:
    explicit Modeler(llvm::StringRef FileName);

    // Add package search path (from -I flag)
    void addPackageSearchPath(llvm::StringRef Path);

    // Main entry point - build a program from syntax
    llvm::Expected<Program> buildProgram(const ProgramSyntax &Syntax);

    // Build a module from file syntax
    llvm::Expected<Module> buildModule(llvm::StringRef Path,
                                        llvm::StringRef FileName,
                                        llvm::StringRef Name,
                                        const FileSyntax &Syntax,
                                        bool Private);

private:
    std::string File;
    std::vector<std::string> PackageSearchPaths;
    std::set<std::string> LoadingPackages;  // For circular dependency detection
    std::map<std::string, Package> ResolvedPackages;  // Cache of resolved packages

    // Package resolution with multi-path search (recursive for transitive deps)
    llvm::Expected<Module> resolvePackage(llvm::StringRef Name, const Version &Ver);

    // Type handling
    Type nameToType(const NameSyntax &Syntax);
    llvm::Expected<std::unique_ptr<Type>> handleType(const TypeSyntax &Syntax);
    Lifetime handleLifetime(const LifetimeSyntax *Syntax);

    // Literal and constant handling
    llvm::Expected<Constant> handleLiteral(const LiteralSyntax &Syntax);

    // Expression handling
    llvm::Expected<Expression> handleExpression(const ExpressionSyntax &Syntax);
    llvm::Expected<Operand> handleOperand(const OperandSyntax &Syntax);
    llvm::Expected<std::vector<Operand>> handleOperands(
        const std::vector<OperandSyntax> *Syntax);
    llvm::Expected<std::vector<Operand>> handleOperation(
        const OperationSyntax &Syntax);

    // Tuple and matrix handling
    llvm::Expected<Tuple> handleObject(const ObjectSyntax &Syntax);
    llvm::Expected<Component> handleComponent(const ComponentSyntax &Syntax);
    llvm::Expected<Matrix> handleVector(const VectorSyntax &Syntax);

    // Block and statement handling
    llvm::Expected<Block> handleBlock(const BlockSyntax &Syntax);
    llvm::Expected<Statement> handleStatement(const StatementSyntax &Syntax);
    llvm::Expected<Statement> handleCommand(const CommandSyntax &Syntax);
    llvm::Expected<std::vector<Statement>> handleStatements(
        const std::vector<StatementSyntax> *Syntax);

    // Binding handling
    llvm::Expected<std::unique_ptr<Type>> handleBindingAnnotation(
        const BindingAnnotationSyntax &Syntax);
    llvm::Expected<Binding> handleCondition(const ConditionSyntax &Syntax);

    // Control flow handling
    llvm::Expected<If> handleIf(const IfSyntax &Syntax);
    llvm::Expected<Match> handleMatch(const MatchSyntax &Syntax);
    llvm::Expected<Choose> handleChoose(const ChooseSyntax &Syntax);
    llvm::Expected<When> handleWhen(const WhenSyntax &Syntax);
    llvm::Expected<For> handleFor(const ForSyntax &Syntax);
    llvm::Expected<While> handleWhile(const WhileSyntax &Syntax);
    llvm::Expected<Try> handleTry(const TrySyntax &Syntax);

    // Special expressions
    llvm::Expected<SizeOf> handleSizeOf(const SizeOfSyntax &Syntax);
    llvm::Expected<AlignOf> handleAlignOf(const AlignOfSyntax &Syntax);
    llvm::Expected<Is> handleIs(const IsSyntax &Syntax);
    llvm::Expected<As> handleAs(const AsSyntax &Syntax);

    // Action handling
    llvm::Expected<Action> handleAction(const ActionSyntax &Syntax);

    // Break, continue, return, throw
    llvm::Expected<Break> handleBreak(const BreakSyntax &Syntax);
    llvm::Expected<Continue> handleContinue(const ContinueSyntax &Syntax);
    llvm::Expected<Return> handleReturn(const ReturnSyntax &Syntax);
    llvm::Expected<Throw> handleThrow(const ThrowSyntax &Syntax);

    // Item and property handling
    llvm::Expected<Item> handleItem(bool Private, const ItemSyntax &Syntax);
    llvm::Expected<Property> handleProperty(bool Private,
                                             const PropertySyntax &Syntax);
    llvm::Expected<std::vector<Item>> handleParameterSet(
        const ParameterSetSyntax &Syntax);

    // Attribute handling
    llvm::Expected<Attribute> handleAttribute(const AttributeSyntax &Syntax);
    llvm::Expected<Model> handleModel(const ModelSyntax &Syntax);

    // Definition handling
    llvm::Expected<Concept> handleDefinition(llvm::StringRef Path,
                                              const DefinitionSyntax &Syntax,
                                              bool Private);
    llvm::Expected<GenericParameter> handleGenericParameter(
        const GenericParameterSyntax &Syntax);
    std::vector<GenericParameter> extractGenericParams(
        const GenericArgumentsSyntax &Syntax);

    // Structure, union, namespace handling
    llvm::Expected<Structure> handleClass(llvm::StringRef Name,
                                           llvm::StringRef Path,
                                           const ClassSyntax &Syntax,
                                           bool Private);
    llvm::Expected<Union> handleUnion(llvm::StringRef Name,
                                       llvm::StringRef Path,
                                       const UnionSyntax &Syntax,
                                       bool Private);
    llvm::Expected<Namespace> handleNamespace(llvm::StringRef Name,
                                               llvm::StringRef Path,
                                               const NamespaceSyntax &Syntax,
                                               bool Private);

    // Function and operator handling
    llvm::Expected<Function> buildFunction(size_t Start, size_t End,
                                            const TargetSyntax &Syntax,
                                            bool Private, bool Pure);
    llvm::Expected<Operator> handleOperator(const OperatorSyntax &Syntax,
                                             bool Private);
    llvm::Expected<Initializer> handleInitializer(const InitSyntax &Syntax,
                                                   bool Private);
    llvm::Expected<std::shared_ptr<DeInitializer>> handleDeInitializer(
        const DeInitSyntax &Syntax);

    // Use handling
    llvm::Expected<Use> handleUse(const UseSyntax &Syntax);

    // Module resolution
    llvm::Expected<Module> buildReferencedModule(llvm::StringRef Path,
                                                  llvm::StringRef Name,
                                                  bool Private);
    llvm::Expected<Module> handleModule(llvm::StringRef Path,
                                         const ModuleSyntax &Syntax,
                                         bool Private);

    // Body handling (for classes and unions)
    struct BodyResult {
        std::vector<Use> Uses;
        std::vector<Module> Modules;
        std::vector<Initializer> Initializers;
        std::shared_ptr<DeInitializer> Deinitializer;
        std::vector<StructMember> Members;
        std::map<std::string, std::shared_ptr<Nameable>> Symbols;
    };

    llvm::Expected<BodyResult> handleBody(llvm::StringRef Name,
                                           llvm::StringRef Path,
                                           const BodySyntax &Syntax);
};

} // namespace scaly
