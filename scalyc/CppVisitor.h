#ifndef __scalyc__CppVisitor__
#define __scalyc__CppVisitor__
#include "scalyc.h"
using namespace scaly;
namespace scalyc {

class CppVisitor : public SyntaxVisitor {
public:
    string* moduleName;
    VarString* sourceFile;
    VarString* headerFile;
    CppModule* module;
    VarString* mainHeaderFile;
    VarString* projectFile;
    _Array<CppModule>* modules;
    CppProgram* output;
    _Array<Inherits>* inherits;
    _Array<string>* classes;
    size_t headerIndentLevel;
    size_t sourceIndentLevel;
    bool firstParameter;
    bool firstBindingInitializer;
    bool declaringClassMember;
    bool inParameterClause;
    bool abstractFunction;
    bool staticFunction;
    bool inEnumMember;
    bool inFunctionReturn;
    bool constDeclaration;
    bool suppressSource;
    bool suppressHeader;
    CppVisitor();
    virtual void execute(Program* program);
    virtual bool openProgram(Program* program);
    virtual void closeProgram(Program* program);
    virtual bool openCompilationUnit(CompilationUnit* compilationUnit);
    virtual void closeCompilationUnit(CompilationUnit* compilationUnit);
    virtual bool isTopLevelFile(CompilationUnit* compilationUnit);
    virtual bool openConstantDeclaration(ConstantDeclaration* constantDeclaration);
    virtual void closeConstantDeclaration(ConstantDeclaration* constantDeclaration);
    virtual bool openVariableDeclaration(VariableDeclaration* variableDeclaration);
    virtual void closeVariableDeclaration(VariableDeclaration* variableDeclaration);
    virtual bool openMutableDeclaration(MutableDeclaration* mutableDeclaration);
    virtual void closeMutableDeclaration(MutableDeclaration* mutableDeclaration);
    virtual bool openFunctionDeclaration(FunctionDeclaration* functionDeclaration);
    virtual void closeFunctionDeclaration(FunctionDeclaration* functionDeclaration);
    virtual bool openEnumDeclaration(EnumDeclaration* enumDeclaration);
    virtual void closeEnumDeclaration(EnumDeclaration* enumDeclaration);
    virtual bool openClassDeclaration(ClassDeclaration* classDeclaration);
    virtual void closeClassDeclaration(ClassDeclaration* classDeclaration);
    virtual bool openConstructorDeclaration(ConstructorDeclaration* initializerDeclaration);
    virtual void closeConstructorDeclaration(ConstructorDeclaration* initializerDeclaration);
    virtual bool openCodeBlock(CodeBlock* codeBlock);
    virtual bool localAllocations(CodeBlock* codeBlock);
    virtual FunctionCall* getFunctionCall(PatternInitializer* patternInitializer);
    virtual bool isCatchingFunctionCall(PatternInitializer* patternInitializer);
    virtual void closeCodeBlock(CodeBlock* codeBlock);
    virtual bool openSimpleExpression(SimpleExpression* simpleExpression);
    virtual void prependReturn(SimpleExpression* simpleExpression);
    virtual void closeSimpleExpression(SimpleExpression* simpleExpression);
    virtual bool openInitializer(Initializer* initializer);
    virtual void closeInitializer(Initializer* initializer);
    virtual bool openBindingInitializer(BindingInitializer* bindingInitializer);
    virtual void closeBindingInitializer(BindingInitializer* bindingInitializer);
    virtual bool openPatternInitializer(PatternInitializer* patternInitializer);
    virtual void closePatternInitializer(PatternInitializer* patternInitializer);
    virtual bool openAdditionalInitializer(AdditionalInitializer* additionalInitializer);
    virtual void closeAdditionalInitializer(AdditionalInitializer* additionalInitializer);
    virtual void visitOverrideWord(OverrideWord* overrideWord);
    virtual void visitStaticWord(StaticWord* staticWord);
    virtual bool openFunctionSignature(FunctionSignature* functionSignature);
    virtual bool hasArrayPostfix(Type* type);
    virtual void closeFunctionSignature(FunctionSignature* functionSignature);
    virtual bool openFunctionResult(FunctionResult* functionResult);
    virtual void closeFunctionResult(FunctionResult* functionResult);
    virtual bool openParameterClause(ParameterClause* parameterClause);
    virtual void closeParameterClause(ParameterClause* parameterClause);
    virtual bool openConstParameter(ConstParameter* constParameter);
    virtual void writeParameter(string* name, Type* parameterType);
    virtual bool isClass(string* name);
    virtual void closeConstParameter(ConstParameter* constParameter);
    virtual bool openVarParameter(VarParameter* varParameter);
    virtual void closeVarParameter(VarParameter* varParameter);
    virtual bool openThrowsClause(ThrowsClause* throwsClause);
    virtual void closeThrowsClause(ThrowsClause* throwsClause);
    virtual bool openEnumMember(EnumMember* enumMember);
    virtual void closeEnumMember(EnumMember* enumMember);
    virtual void appendCppType(VarString* s, Type* type);
    virtual void visitEnumCase(EnumCase* enumCase);
    virtual bool openAdditionalCase(AdditionalCase* additionalCase);
    virtual void closeAdditionalCase(AdditionalCase* additionalCase);
    virtual bool openClassBody(ClassBody* classBody);
    virtual void closeClassBody(ClassBody* classBody);
    virtual void indentHeader();
    virtual void indentSource();
    virtual void collectDerivedClasses(_Array<string>* derivedClasses, string* className);
    virtual void appendDerivedClasses(_Array<string>* derivedClasses, _Array<string>* inheritors);
    virtual bool openClassMember(ClassMember* classMember);
    virtual void closeClassMember(ClassMember* classMember);
    virtual bool openPrefixExpression(PrefixExpression* prefixExpression);
    virtual void closePrefixExpression(PrefixExpression* prefixExpression);
    virtual bool openPostfixExpression(PostfixExpression* postfixExpression);
    virtual void closePostfixExpression(PostfixExpression* postfixExpression);
    virtual bool openBinaryOperation(BinaryOperation* binaryOperation);
    virtual void closeBinaryOperation(BinaryOperation* binaryOperation);
    virtual bool openAssignment(Assignment* assignment);
    virtual bool inConstructor(SyntaxNode* node);
    virtual bool inReturn(SyntaxNode* node);
    virtual bool inThrow(SyntaxNode* node);
    virtual string* getMemberIfCreatingObject(_Page* _rp, Assignment* assignment);
    virtual string* getFunctionName(_Page* _rp, Assignment* assignment);
    virtual bool isCreatingObject(string* functionName, SyntaxNode* node);
    virtual ClassDeclaration* getClassDeclaration(SyntaxNode* node);
    virtual bool isVariableMember(string* memberName, ClassDeclaration* classDeclaration);
    virtual void closeAssignment(Assignment* assignment);
    virtual bool openTypeQuery(TypeQuery* typeQuery);
    virtual void closeTypeQuery(TypeQuery* typeQuery);
    virtual bool openTypeCast(TypeCast* typeCast);
    virtual void closeTypeCast(TypeCast* typeCast);
    virtual bool openCatchClause(CatchClause* catchClause);
    virtual IdentifierExpression* getIdentifierExpression(PostfixExpression* postfixExpression);
    virtual string* getErrorType(CatchClause* catchClause);
    virtual void closeCatchClause(CatchClause* catchClause);
    virtual bool openWildCardCatchPattern(WildCardCatchPattern* wildCardCatchPattern);
    virtual void closeWildCardCatchPattern(WildCardCatchPattern* wildCardCatchPattern);
    virtual bool openIdentifierCatchPattern(IdentifierCatchPattern* identifierCatchPattern);
    virtual void closeIdentifierCatchPattern(IdentifierCatchPattern* identifierCatchPattern);
    virtual void visitOperatorPostfix(OperatorPostfix* operatorPostfix);
    virtual bool openFunctionCall(FunctionCall* functionCall);
    virtual void closeFunctionCall(FunctionCall* functionCall);
    virtual void visitMemberExpression(MemberExpression* memberExpression);
    virtual bool openSubscript(Subscript* subscript);
    virtual void closeSubscript(Subscript* subscript);
    virtual bool openExpressionElement(ExpressionElement* expressionElement);
    virtual void closeExpressionElement(ExpressionElement* expressionElement);
    virtual bool isLastExpressionElement(ExpressionElement* expressionElement);
    virtual void visitIdentifierExpression(IdentifierExpression* identifierExpression);
    virtual bool inAssignment(SyntaxNode* syntaxNode);
    virtual Assignment* getAssignment(SyntaxNode* syntaxNode);
    virtual bool inRetDeclaration(SyntaxNode* syntaxNode);
    virtual void visitLiteralExpression(LiteralExpression* literalExpression);
    virtual bool openIfExpression(IfExpression* ifExpression);
    virtual void closeIfExpression(IfExpression* ifExpression);
    virtual bool openSwitchExpression(SwitchExpression* switchExpression);
    virtual void closeSwitchExpression(SwitchExpression* switchExpression);
    virtual bool openForExpression(ForExpression* forExpression);
    virtual void closeForExpression(ForExpression* forExpression);
    virtual bool openWhileExpression(WhileExpression* whileExpression);
    virtual void closeWhileExpression(WhileExpression* whileExpression);
    virtual bool openDoExpression(DoExpression* doExpression);
    virtual void closeDoExpression(DoExpression* doExpression);
    virtual bool openParenthesizedExpression(ParenthesizedExpression* parenthesizedExpression);
    virtual bool assignedToRootObject(FunctionCall* functionCall);
    virtual string* getLocalPage(FunctionCall* functionCall);
    virtual bool isRootBinding(BindingInitializer* bindingInitializer);
    virtual bool assignedToReturnedObject(FunctionCall* functionCall);
    virtual bool boundToObject(BindingInitializer* bindingInitializer);
    virtual BindingInitializer* getBindingInitializer(FunctionCall* functionCall);
    virtual bool callsInitializer(FunctionCall* functionCall);
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
    virtual void closeThrowExpression(ThrowExpression* throwExpression);
    virtual bool openBreakExpression(BreakExpression* breakExpression);
    virtual void closeBreakExpression(BreakExpression* breakExpression);
    virtual bool openConstructorCall(ConstructorCall* constructorCall);
    virtual void closeConstructorCall(ConstructorCall* constructorCall);
    virtual bool initializerIsBoundOrAssigned(ConstructorCall* initializerCall);
    virtual void visitThisExpression(ThisExpression* thisExpression);
    virtual void visitNullExpression(NullExpression* nullExpression);
    virtual bool openElseClause(ElseClause* elseClause);
    virtual void closeElseClause(ElseClause* elseClause);
    virtual bool openCurliedSwitchBody(CurliedSwitchBody* curliedSwitchBody);
    virtual void closeCurliedSwitchBody(CurliedSwitchBody* curliedSwitchBody);
    virtual bool openNakedSwitchBody(NakedSwitchBody* nakedSwitchBody);
    virtual void closeNakedSwitchBody(NakedSwitchBody* nakedSwitchBody);
    virtual bool openSwitchCase(SwitchCase* switchCase);
    virtual void closeSwitchCase(SwitchCase* switchCase);
    virtual bool openItemCaseLabel(ItemCaseLabel* itemCaseLabel);
    virtual void closeItemCaseLabel(ItemCaseLabel* itemCaseLabel);
    virtual void visitDefaultCaseLabel(DefaultCaseLabel* defaultCaseLabel);
    virtual bool openCaseItem(CaseItem* caseItem);
    virtual void closeCaseItem(CaseItem* caseItem);
    virtual void visitWildcardPattern(WildcardPattern* wildcardPattern);
    virtual bool openIdentifierPattern(IdentifierPattern* identifierPattern);
    virtual bool isCatchingPatternInitializer(PatternInitializer* patternInitializer);
    virtual void closeIdentifierPattern(IdentifierPattern* identifierPattern);
    virtual bool openTuplePattern(TuplePattern* tuplePattern);
    virtual void closeTuplePattern(TuplePattern* tuplePattern);
    virtual bool openExpressionPattern(ExpressionPattern* expressionPattern);
    virtual void closeExpressionPattern(ExpressionPattern* expressionPattern);
    virtual bool openTuplePatternElement(TuplePatternElement* tuplePatternElement);
    virtual void closeTuplePatternElement(TuplePatternElement* tuplePatternElement);
    virtual bool openCaseContent(CaseContent* caseContent);
    virtual void closeCaseContent(CaseContent* caseContent);
    virtual bool openType(Type* type);
    virtual void closeType(Type* type);
    virtual bool inTypeQuery(Type* type);
    virtual void appendCppTypeName(VarString* s, Type* type);
    virtual bool openTypeAnnotation(TypeAnnotation* typeAnnotation);
    virtual void closeTypeAnnotation(TypeAnnotation* typeAnnotation);
    virtual bool openSubtype(Subtype* subtype);
    virtual void closeSubtype(Subtype* subtype);
    virtual bool openIndexedType(IndexedType* indexedType);
    virtual void closeIndexedType(IndexedType* indexedType);
    virtual void visitPointer(Pointer* pointer);
    virtual void visitRoot(Root* root);
    virtual void visitLocal(Local* local);
    virtual void visitReference(Reference* age);
    virtual void visitThrown(Thrown* thrown);
    virtual bool openTypeInheritanceClause(TypeInheritanceClause* typeInheritanceClause);
    virtual void closeTypeInheritanceClause(TypeInheritanceClause* typeInheritanceClause);
    virtual bool openInheritance(Inheritance* inheritance);
    virtual void closeInheritance(Inheritance* inheritance);
    virtual void buildMainHeaderFileString(Program* program);
    virtual void buildProjectFileString(Program* program);
    virtual void collectInheritances(Program* program);
    virtual void collectInheritancesInCompilationUnit(CompilationUnit* compilationUnit);
    virtual void registerInheritance(string* className, string* baseName);

    virtual bool _isCppVisitor();
};

}
#endif // __scalyc__CppVisitor__
