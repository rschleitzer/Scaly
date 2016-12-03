#ifndef __scalyc__HeaderVisitor__
#define __scalyc__HeaderVisitor__
#include "scalyc.h"
using namespace scaly;
namespace scalyc {

class HeaderVisitor : public CppVisitor {
public:
    VarString* headerFile;
    VarString* mainHeaderFile;
    _Array<Inherits>* inherits;
    _Array<string>* classes;
    size_t headerIndentLevel;
    bool firstParameter;
    bool firstBindingInitializer;
    bool staticFunction;
    HeaderVisitor();
    virtual bool openProgram(Program* program);
    virtual bool openCompilationUnit(CompilationUnit* compilationUnit);
    virtual void closeCompilationUnit(CompilationUnit* compilationUnit);
    virtual bool openConstantDeclaration(ConstantDeclaration* constantDeclaration);
    virtual bool openVariableDeclaration(VariableDeclaration* variableDeclaration);
    virtual bool openMutableDeclaration(MutableDeclaration* mutableDeclaration);
    virtual bool openFunctionDeclaration(FunctionDeclaration* functionDeclaration);
    virtual void closeFunctionDeclaration(FunctionDeclaration* functionDeclaration);
    virtual bool openEnumDeclaration(EnumDeclaration* enumDeclaration);
    virtual void closeEnumDeclaration(EnumDeclaration* enumDeclaration);
    virtual bool openClassDeclaration(ClassDeclaration* classDeclaration);
    virtual void closeClassDeclaration(ClassDeclaration* classDeclaration);
    virtual bool openConstructorDeclaration(ConstructorDeclaration* constructorDeclaration);
    virtual bool openCodeBlock(CodeBlock* codeBlock);
    virtual bool openBindingInitializer(BindingInitializer* bindingInitializer);
    virtual bool openPatternInitializer(PatternInitializer* patternInitializer);
    virtual bool openFunctionSignature(FunctionSignature* functionSignature);
    virtual bool openParameterClause(ParameterClause* parameterClause);
    virtual void closeParameterClause(ParameterClause* parameterClause);
    virtual bool openConstParameter(ConstParameter* constParameter);
    virtual void writeParameter(string* name, Type* parameterType);
    virtual bool isClass(string* name);
    virtual void closeConstParameter(ConstParameter* constParameter);
    virtual bool openVarParameter(VarParameter* varParameter);
    virtual void closeVarParameter(VarParameter* varParameter);
    virtual bool openEnumMember(EnumMember* enumMember);
    virtual void closeEnumMember(EnumMember* enumMember);
    virtual void indentHeader();
    virtual void collectDerivedClasses(_Array<string>* derivedClasses, string* className);
    virtual void appendDerivedClasses(_Array<string>* derivedClasses, _Array<string>* inheritors);
    virtual bool openClassMember(ClassMember* classMember);
    virtual void closeClassMember(ClassMember* classMember);
    virtual bool openIdentifierPattern(IdentifierPattern* identifierPattern);
    virtual bool openType(Type* type);
    virtual void closeType(Type* type);
    virtual void buildMainHeaderFileString(Program* program);
    virtual void collectInheritances(Program* program);
    virtual void collectInheritancesInCompilationUnit(CompilationUnit* compilationUnit);
    virtual void registerInheritance(string* className, string* baseName);

    virtual bool _isHeaderVisitor();
};

}
#endif // __scalyc__HeaderVisitor__
