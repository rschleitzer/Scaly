#ifndef __scalyc__CppVisitor__
#define __scalyc__CppVisitor__
#include "scalyc.h"
using namespace scaly;
namespace scalyc {

class Inherits;

class CppVisitor : public CommonVisitor {
public:
    _Array<Inherits>* inherits;
    _Array<string>* classes;
    virtual bool hasArrayPostfix(Type* type);
    virtual bool isClass(string* name) = 0;
    virtual void appendCppType(VarString* s, Type* type);
    virtual void appendCppTypeName(VarString* s, Type* type);
    virtual void collectDerivedClasses(_Array<string>* derivedClasses, string* className);
    virtual void appendDerivedClasses(_Array<string>* derivedClasses, _Array<string>* inheritors);
    virtual void collectInheritancesInCompilationUnit(CompilationUnit* compilationUnit);
    virtual void registerInheritance(string* className, string* baseName);
    virtual string* getFileName(_Page* _rp, CompilationUnit* compilationUnit);

    virtual bool _isCppVisitor();
    virtual bool _isHeaderVisitor();
    virtual bool _isSourceVisitor();
};

class Inherits : public Object {
public:
    string* name;
    _Array<string>* inheritors;
    Inherits(string* className);

};

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
    virtual bool openIdentifierInitializer(IdentifierInitializer* identifierInitializer);
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

class SourceVisitor : public CppVisitor {
public:
    string* directory;
    VarString* sourceFile;
    SourceVisitor(string* outputDirectory);
    virtual bool openProgram(Program* program);
    virtual bool openCompilationUnit(CompilationUnit* compilationUnit);
    virtual void closeCompilationUnit(CompilationUnit* compilationUnit);
    virtual bool openConstantDeclaration(ConstantDeclaration* constantDeclaration);
    virtual bool openMutableDeclaration(MutableDeclaration* mutableDeclaration);
    virtual bool openFunctionDeclaration(FunctionDeclaration* functionDeclaration);
    virtual bool openEnumDeclaration(EnumDeclaration* enumDeclaration);
    virtual void closeClassDeclaration(ClassDeclaration* classDeclaration);
    virtual bool openConstructorDeclaration(ConstructorDeclaration* constructorDeclaration);
    virtual void closeConstructorDeclaration(ConstructorDeclaration* constructorDeclaration);
    virtual bool openCodeBlock(CodeBlock* codeBlock);
    virtual bool localAllocations(CodeBlock* codeBlock);
    virtual FunctionCall* getFunctionCall(IdentifierInitializer* identifierInitializer);
    virtual bool isCatchingFunctionCall(IdentifierInitializer* identifierInitializer);
    virtual void closeCodeBlock(CodeBlock* codeBlock);
    virtual bool openSimpleExpression(SimpleExpression* simpleExpression);
    virtual void prependReturn(SimpleExpression* simpleExpression);
    virtual void closeSimpleExpression(SimpleExpression* simpleExpression);
    virtual bool openInitializer(Initializer* initializer);
    virtual bool openBindingInitializer(BindingInitializer* bindingInitializer);
    virtual void closeBindingInitializer(BindingInitializer* bindingInitializer);
    virtual bool openFunctionSignature(FunctionSignature* functionSignature);
    virtual bool openFunctionResult(FunctionResult* functionResult);
    virtual void closeParameterClause(ParameterClause* parameterClause);
    virtual bool openConstParameter(ConstParameter* constParameter);
    virtual void writeParameter(string* name, Type* parameterType);
    virtual bool isClass(string* name);
    virtual bool openVarParameter(VarParameter* varParameter);
    virtual bool openThrowsClause(ThrowsClause* throwsClause);
    virtual bool openEnumMember(EnumMember* enumMember);
    virtual void closeEnumMember(EnumMember* enumMember);
    virtual size_t level(SyntaxNode* syntaxNode);
    virtual void indent(size_t level);
    virtual bool openPrefixExpression(PrefixExpression* prefixExpression);
    virtual bool openPostfixExpression(PostfixExpression* postfixExpression);
    virtual bool openBinaryOperation(BinaryOperation* binaryOperation);
    virtual bool openAssignment(Assignment* assignment);
    virtual bool inConstructor(SyntaxNode* node);
    virtual bool inThrow(SyntaxNode* node);
    virtual ClassDeclaration* getClassDeclaration(SyntaxNode* node);
    virtual bool isObjectField(string* memberName, ClassDeclaration* classDeclaration);
    virtual bool isVariableObjectField(string* memberName, ClassDeclaration* classDeclaration);
    virtual ClassDeclaration* findClassDeclaration(SyntaxNode* node, string* name);
    virtual bool openTypeQuery(TypeQuery* typeQuery);
    virtual bool openCatchClause(CatchClause* catchClause);
    virtual IdentifierExpression* getIdentifierExpression(PostfixExpression* postfixExpression);
    virtual string* getErrorType(CatchClause* catchClause);
    virtual void visitOperatorPostfix(OperatorPostfix* operatorPostfix);
    virtual void visitMemberExpression(MemberExpression* memberExpression);
    virtual bool openSubscript(Subscript* subscript);
    virtual void closeSubscript(Subscript* subscript);
    virtual void closeExpressionElement(ExpressionElement* expressionElement);
    virtual bool isLastExpressionElement(ExpressionElement* expressionElement);
    virtual void visitIdentifierExpression(IdentifierExpression* identifierExpression);
    virtual bool inAssignment(SyntaxNode* syntaxNode);
    virtual Assignment* getAssignment(SyntaxNode* syntaxNode);
    virtual Initializer* getInitializer(SyntaxNode* syntaxNode);
    virtual void visitLiteralExpression(LiteralExpression* literalExpression);
    virtual bool openIfExpression(IfExpression* ifExpression);
    virtual bool openSwitchExpression(SwitchExpression* switchExpression);
    virtual void closeSwitchExpression(SwitchExpression* switchExpression);
    virtual bool openForExpression(ForExpression* forExpression);
    virtual bool openWhileExpression(WhileExpression* whileExpression);
    virtual bool openDoExpression(DoExpression* doExpression);
    virtual bool openParenthesizedExpression(ParenthesizedExpression* parenthesizedExpression);
    virtual bool assignedToRootObject(FunctionCall* functionCall);
    virtual string* getLocalPage(FunctionCall* functionCall);
    virtual bool isRootBinding(BindingInitializer* bindingInitializer);
    virtual bool assignedToReturnedObject(FunctionCall* functionCall);
    virtual bool boundToObject(BindingInitializer* bindingInitializer);
    virtual BindingInitializer* getBindingInitializer(FunctionCall* functionCall);
    virtual bool catchesError(FunctionCall* functionCall);
    virtual void closeParenthesizedExpression(ParenthesizedExpression* parenthesizedExpression);
    virtual bool openReturnExpression(ReturnExpression* returnExpression);
    virtual bool inTopLevelCode(SyntaxNode* syntaxNode);
    virtual void closeReturnExpression(ReturnExpression* returnExpression);
    virtual bool openThrowExpression(ThrowExpression* throwExpression);
    virtual bool inWildcardCatchClause(ThrowExpression* throwExpression);
    virtual CatchClause* getCatchClause(SyntaxNode* syntaxNode);
    virtual string* getReturnType(_Page* _rp, SyntaxNode* syntaxNode);
    virtual string* getThrownType(_Page* _rp, SyntaxNode* syntaxNode);
    virtual bool returnsArray(SyntaxNode* syntaxNode);
    virtual FunctionDeclaration* getFunctionDeclaration(SyntaxNode* syntaxNode);
    virtual bool openBreakExpression(BreakExpression* breakExpression);
    virtual string* getPageOfVariable(_Page* _rp, string* name, CodeBlock* codeBlock);
    virtual string* getPage(_Page* _rp, SyntaxNode* node);
    virtual bool openConstructorCall(ConstructorCall* constructorCall);
    virtual void visitThisExpression(ThisExpression* thisExpression);
    virtual void visitNullExpression(NullExpression* nullExpression);
    virtual bool openElseClause(ElseClause* elseClause);
    virtual bool openCurliedSwitchBody(CurliedSwitchBody* curliedSwitchBody);
    virtual void closeCurliedSwitchBody(CurliedSwitchBody* curliedSwitchBody);
    virtual bool openSwitchCase(SwitchCase* switchCase);
    virtual bool openItemCaseLabel(ItemCaseLabel* itemCaseLabel);
    virtual void visitDefaultCaseLabel(DefaultCaseLabel* defaultCaseLabel);
    virtual bool openCaseItem(CaseItem* caseItem);
    virtual void closeCaseItem(CaseItem* caseItem);
    virtual bool openIdentifierPattern(IdentifierPattern* identifierPattern);
    virtual bool isCatchingIdentifierInitializer(IdentifierInitializer* identifierInitializer);
    virtual void closeExpressionPattern(ExpressionPattern* expressionPattern);
    virtual bool openCaseContent(CaseContent* caseContent);
    virtual void closeCaseContent(CaseContent* caseContent);
    virtual bool openType(Type* type);
    virtual void closeType(Type* type);
    virtual bool inTypeQuery(Type* type);
    virtual void visitPointer(Pointer* pointer);
    virtual bool openInheritance(Inheritance* inheritance);
    virtual VarString* buildProjectFileString(_Page* _rp, Program* program);
    virtual void collectInheritances(Program* program);

    virtual bool _isSourceVisitor();
};

}
#endif // __scalyc__CppVisitor__
