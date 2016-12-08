#ifndef __scalyc__HeaderVisitor__
#define __scalyc__HeaderVisitor__
#include "scalyc.h"
using namespace scaly;
namespace scalyc {

class HeaderVisitor : public CppVisitor {
public:
    string* directory;
    VarString* headerFile;
    VarString* mainHeaderFile;
    HeaderVisitor(string* outputDirectory);
    virtual bool openProgram(Program* program);
    virtual bool openCompilationUnit(CompilationUnit* compilationUnit);
    virtual void closeCompilationUnit(CompilationUnit* compilationUnit);
    virtual bool openConstantDeclaration(ConstantDeclaration* constantDeclaration);
    virtual bool openMutableDeclaration(MutableDeclaration* mutableDeclaration);
    virtual void closeFunctionDeclaration(FunctionDeclaration* functionDeclaration);
    virtual bool openEnumDeclaration(EnumDeclaration* enumDeclaration);
    virtual void closeEnumDeclaration(EnumDeclaration* enumDeclaration);
    virtual bool openClassDeclaration(ClassDeclaration* classDeclaration);
    virtual void closeClassDeclaration(ClassDeclaration* classDeclaration);
    virtual bool openConstructorDeclaration(ConstructorDeclaration* constructorDeclaration);
    virtual bool openCodeBlock(CodeBlock* codeBlock);
    virtual bool openPatternInitializer(PatternInitializer* patternInitializer);
    virtual bool openFunctionSignature(FunctionSignature* functionSignature);
    virtual void closeParameterClause(ParameterClause* parameterClause);
    virtual bool openConstParameter(ConstParameter* constParameter);
    virtual void writeParameter(string* name, Type* parameterType);
    virtual bool isClass(string* name);
    virtual void closeConstParameter(ConstParameter* constParameter);
    virtual bool openVarParameter(VarParameter* varParameter);
    virtual void closeVarParameter(VarParameter* varParameter);
    virtual bool openEnumMember(EnumMember* enumMember);
    virtual void closeEnumMember(EnumMember* enumMember);
    virtual bool openClassMember(ClassMember* classMember);
    virtual void closeClassMember(ClassMember* classMember);
    virtual bool openIdentifierPattern(IdentifierPattern* identifierPattern);
    virtual bool openType(Type* type);
    virtual void closeType(Type* type);
    virtual void buildMainHeaderFileString(Program* program);
    virtual void collectInheritances(Program* program);

    virtual bool _isHeaderVisitor();
};

}
#endif // __scalyc__HeaderVisitor__
