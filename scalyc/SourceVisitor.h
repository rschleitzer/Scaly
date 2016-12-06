#ifndef __scalyc__SourceVisitor__
#define __scalyc__SourceVisitor__
#include "scalyc.h"
using namespace scaly;
namespace scalyc {

class SourceVisitor : public CppVisitor {
public:
    VarString* sourceFile;
    VarString* projectFile;
    SourceVisitor();
    virtual bool openProgram(Program* program);
    virtual bool openCompilationUnit(CompilationUnit* compilationUnit);
    virtual void closeCompilationUnit(CompilationUnit* compilationUnit);
    virtual bool isTopLevelFile(CompilationUnit* compilationUnit);
    virtual bool openConstantDeclaration(ConstantDeclaration* constantDeclaration);
    virtual bool openVariableDeclaration(VariableDeclaration* variableDeclaration);
    virtual bool openMutableDeclaration(MutableDeclaration* mutableDeclaration);
    virtual bool openFunctionDeclaration(FunctionDeclaration* functionDeclaration);
    virtual bool openEnumDeclaration(EnumDeclaration* enumDeclaration);
    virtual void closeClassDeclaration(ClassDeclaration* classDeclaration);
    virtual bool openConstructorDeclaration(ConstructorDeclaration* constructorDeclaration);
    virtual void closeConstructorDeclaration(ConstructorDeclaration* constructorDeclaration);
    virtual bool openCodeBlock(CodeBlock* codeBlock);
    virtual bool localAllocations(CodeBlock* codeBlock);
    virtual FunctionCall* getFunctionCall(PatternInitializer* patternInitializer);
    virtual bool isCatchingFunctionCall(PatternInitializer* patternInitializer);
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
    virtual bool isCatchingPatternInitializer(PatternInitializer* patternInitializer);
    virtual void closeExpressionPattern(ExpressionPattern* expressionPattern);
    virtual bool openCaseContent(CaseContent* caseContent);
    virtual void closeCaseContent(CaseContent* caseContent);
    virtual bool openType(Type* type);
    virtual void closeType(Type* type);
    virtual bool inTypeQuery(Type* type);
    virtual void visitPointer(Pointer* pointer);
    virtual bool openInheritance(Inheritance* inheritance);
    virtual void buildProjectFileString(Program* program);
    virtual void collectInheritances(Program* program);
    virtual void collectInheritancesInCompilationUnit(CompilationUnit* compilationUnit);
    virtual void registerInheritance(string* className, string* baseName);

    virtual bool _isSourceVisitor();
};

}
#endif // __scalyc__SourceVisitor__
