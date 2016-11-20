#include "scalyc.h"
using namespace scaly;
namespace scalyc {

bool MyVisitor::openProgram(Program* program) {
    return true;
}

void MyVisitor::closeProgram(Program* program) {
}

bool MyVisitor::openCompilationUnit(CompilationUnit* compilationUnit) {
    return true;
}

void MyVisitor::closeCompilationUnit(CompilationUnit* compilationUnit) {
}

bool MyVisitor::openConstantDeclaration(ConstantDeclaration* constantDeclaration) {
    return true;
}

void MyVisitor::closeConstantDeclaration(ConstantDeclaration* constantDeclaration) {
}

bool MyVisitor::openVariableDeclaration(VariableDeclaration* variableDeclaration) {
    return true;
}

void MyVisitor::closeVariableDeclaration(VariableDeclaration* variableDeclaration) {
}

bool MyVisitor::openMutableDeclaration(MutableDeclaration* mutableDeclaration) {
    return true;
}

void MyVisitor::closeMutableDeclaration(MutableDeclaration* mutableDeclaration) {
}

bool MyVisitor::openFunctionDeclaration(FunctionDeclaration* functionDeclaration) {
    return true;
}

void MyVisitor::closeFunctionDeclaration(FunctionDeclaration* functionDeclaration) {
}

bool MyVisitor::openEnumDeclaration(EnumDeclaration* enumDeclaration) {
    return true;
}

void MyVisitor::closeEnumDeclaration(EnumDeclaration* enumDeclaration) {
}

bool MyVisitor::openClassDeclaration(ClassDeclaration* classDeclaration) {
    return true;
}

void MyVisitor::closeClassDeclaration(ClassDeclaration* classDeclaration) {
}

bool MyVisitor::openConstructorDeclaration(ConstructorDeclaration* constructorDeclaration) {
    return true;
}

void MyVisitor::closeConstructorDeclaration(ConstructorDeclaration* constructorDeclaration) {
}

bool MyVisitor::openCodeBlock(CodeBlock* codeBlock) {
    return true;
}

void MyVisitor::closeCodeBlock(CodeBlock* codeBlock) {
}

bool MyVisitor::openSimpleExpression(SimpleExpression* simpleExpression) {
    return true;
}

void MyVisitor::closeSimpleExpression(SimpleExpression* simpleExpression) {
}

bool MyVisitor::openInitializer(Initializer* initializer) {
    return true;
}

void MyVisitor::closeInitializer(Initializer* initializer) {
}

bool MyVisitor::openBindingInitializer(BindingInitializer* bindingInitializer) {
    return true;
}

void MyVisitor::closeBindingInitializer(BindingInitializer* bindingInitializer) {
}

bool MyVisitor::openPatternInitializer(PatternInitializer* patternInitializer) {
    return true;
}

void MyVisitor::closePatternInitializer(PatternInitializer* patternInitializer) {
}

bool MyVisitor::openAdditionalInitializer(AdditionalInitializer* additionalInitializer) {
    return true;
}

void MyVisitor::closeAdditionalInitializer(AdditionalInitializer* additionalInitializer) {
}

void MyVisitor::visitOverrideWord(OverrideWord* overrideWord) {
}

void MyVisitor::visitStaticWord(StaticWord* staticWord) {
}

bool MyVisitor::openFunctionSignature(FunctionSignature* functionSignature) {
    return true;
}

void MyVisitor::closeFunctionSignature(FunctionSignature* functionSignature) {
}

bool MyVisitor::openFunctionResult(FunctionResult* functionResult) {
    return true;
}

void MyVisitor::closeFunctionResult(FunctionResult* functionResult) {
}

void MyVisitor::visitExistingClause(ExistingClause* existingClause) {
}

bool MyVisitor::openParameterClause(ParameterClause* parameterClause) {
    return true;
}

void MyVisitor::closeParameterClause(ParameterClause* parameterClause) {
}

bool MyVisitor::openConstParameter(ConstParameter* constParameter) {
    return true;
}

void MyVisitor::closeConstParameter(ConstParameter* constParameter) {
}

bool MyVisitor::openVarParameter(VarParameter* varParameter) {
    return true;
}

void MyVisitor::closeVarParameter(VarParameter* varParameter) {
}

bool MyVisitor::openThrowsClause(ThrowsClause* throwsClause) {
    return true;
}

void MyVisitor::closeThrowsClause(ThrowsClause* throwsClause) {
}

bool MyVisitor::openEnumMember(EnumMember* enumMember) {
    return true;
}

void MyVisitor::closeEnumMember(EnumMember* enumMember) {
}

void MyVisitor::visitEnumCase(EnumCase* enumCase) {
}

bool MyVisitor::openAdditionalCase(AdditionalCase* additionalCase) {
    return true;
}

void MyVisitor::closeAdditionalCase(AdditionalCase* additionalCase) {
}

bool MyVisitor::openClassBody(ClassBody* classBody) {
    return true;
}

void MyVisitor::closeClassBody(ClassBody* classBody) {
}

bool MyVisitor::openClassMember(ClassMember* classMember) {
    return true;
}

void MyVisitor::closeClassMember(ClassMember* classMember) {
}

bool MyVisitor::openPrefixExpression(PrefixExpression* prefixExpression) {
    return true;
}

void MyVisitor::closePrefixExpression(PrefixExpression* prefixExpression) {
}

bool MyVisitor::openPostfixExpression(PostfixExpression* postfixExpression) {
    return true;
}

void MyVisitor::closePostfixExpression(PostfixExpression* postfixExpression) {
}

bool MyVisitor::openBinaryOperation(BinaryOperation* binaryOperation) {
    return true;
}

void MyVisitor::closeBinaryOperation(BinaryOperation* binaryOperation) {
}

bool MyVisitor::openAssignment(Assignment* assignment) {
    return true;
}

void MyVisitor::closeAssignment(Assignment* assignment) {
}

bool MyVisitor::openTypeQuery(TypeQuery* typeQuery) {
    return true;
}

void MyVisitor::closeTypeQuery(TypeQuery* typeQuery) {
}

bool MyVisitor::openTypeCast(TypeCast* typeCast) {
    return true;
}

void MyVisitor::closeTypeCast(TypeCast* typeCast) {
}

bool MyVisitor::openCatchClause(CatchClause* catchClause) {
    return true;
}

void MyVisitor::closeCatchClause(CatchClause* catchClause) {
}

bool MyVisitor::openWildCardCatchPattern(WildCardCatchPattern* wildCardCatchPattern) {
    return true;
}

void MyVisitor::closeWildCardCatchPattern(WildCardCatchPattern* wildCardCatchPattern) {
}

bool MyVisitor::openIdentifierCatchPattern(IdentifierCatchPattern* identifierCatchPattern) {
    return true;
}

void MyVisitor::closeIdentifierCatchPattern(IdentifierCatchPattern* identifierCatchPattern) {
}

void MyVisitor::visitOperatorPostfix(OperatorPostfix* operatorPostfix) {
}

bool MyVisitor::openFunctionCall(FunctionCall* functionCall) {
    return true;
}

void MyVisitor::closeFunctionCall(FunctionCall* functionCall) {
}

void MyVisitor::visitMemberExpression(MemberExpression* memberExpression) {
}

bool MyVisitor::openSubscript(Subscript* subscript) {
    return true;
}

void MyVisitor::closeSubscript(Subscript* subscript) {
}

bool MyVisitor::openExpressionElement(ExpressionElement* expressionElement) {
    return true;
}

void MyVisitor::closeExpressionElement(ExpressionElement* expressionElement) {
}

void MyVisitor::visitIdentifierExpression(IdentifierExpression* identifierExpression) {
}

void MyVisitor::visitLiteralExpression(LiteralExpression* literalExpression) {
}

bool MyVisitor::openIfExpression(IfExpression* ifExpression) {
    return true;
}

void MyVisitor::closeIfExpression(IfExpression* ifExpression) {
}

bool MyVisitor::openSwitchExpression(SwitchExpression* switchExpression) {
    return true;
}

void MyVisitor::closeSwitchExpression(SwitchExpression* switchExpression) {
}

bool MyVisitor::openForExpression(ForExpression* forExpression) {
    return true;
}

void MyVisitor::closeForExpression(ForExpression* forExpression) {
}

bool MyVisitor::openWhileExpression(WhileExpression* whileExpression) {
    return true;
}

void MyVisitor::closeWhileExpression(WhileExpression* whileExpression) {
}

bool MyVisitor::openDoExpression(DoExpression* doExpression) {
    return true;
}

void MyVisitor::closeDoExpression(DoExpression* doExpression) {
}

bool MyVisitor::openParenthesizedExpression(ParenthesizedExpression* parenthesizedExpression) {
    return true;
}

void MyVisitor::closeParenthesizedExpression(ParenthesizedExpression* parenthesizedExpression) {
}

bool MyVisitor::openReturnExpression(ReturnExpression* returnExpression) {
    return true;
}

void MyVisitor::closeReturnExpression(ReturnExpression* returnExpression) {
}

bool MyVisitor::openThrowExpression(ThrowExpression* throwExpression) {
    return true;
}

void MyVisitor::closeThrowExpression(ThrowExpression* throwExpression) {
}

bool MyVisitor::openBreakExpression(BreakExpression* breakExpression) {
    return true;
}

void MyVisitor::closeBreakExpression(BreakExpression* breakExpression) {
}

bool MyVisitor::openConstructorCall(ConstructorCall* constructorCall) {
    return true;
}

void MyVisitor::closeConstructorCall(ConstructorCall* constructorCall) {
}

void MyVisitor::visitThisExpression(ThisExpression* thisExpression) {
}

void MyVisitor::visitNullExpression(NullExpression* nullExpression) {
}

bool MyVisitor::openElseClause(ElseClause* elseClause) {
    return true;
}

void MyVisitor::closeElseClause(ElseClause* elseClause) {
}

bool MyVisitor::openCurliedSwitchBody(CurliedSwitchBody* curliedSwitchBody) {
    return true;
}

void MyVisitor::closeCurliedSwitchBody(CurliedSwitchBody* curliedSwitchBody) {
}

bool MyVisitor::openNakedSwitchBody(NakedSwitchBody* nakedSwitchBody) {
    return true;
}

void MyVisitor::closeNakedSwitchBody(NakedSwitchBody* nakedSwitchBody) {
}

bool MyVisitor::openSwitchCase(SwitchCase* switchCase) {
    return true;
}

void MyVisitor::closeSwitchCase(SwitchCase* switchCase) {
}

bool MyVisitor::openItemCaseLabel(ItemCaseLabel* itemCaseLabel) {
    return true;
}

void MyVisitor::closeItemCaseLabel(ItemCaseLabel* itemCaseLabel) {
}

void MyVisitor::visitDefaultCaseLabel(DefaultCaseLabel* defaultCaseLabel) {
}

bool MyVisitor::openCaseItem(CaseItem* caseItem) {
    return true;
}

void MyVisitor::closeCaseItem(CaseItem* caseItem) {
}

void MyVisitor::visitWildcardPattern(WildcardPattern* wildcardPattern) {
}

bool MyVisitor::openIdentifierPattern(IdentifierPattern* identifierPattern) {
    return true;
}

void MyVisitor::closeIdentifierPattern(IdentifierPattern* identifierPattern) {
}

bool MyVisitor::openTuplePattern(TuplePattern* tuplePattern) {
    return true;
}

void MyVisitor::closeTuplePattern(TuplePattern* tuplePattern) {
}

bool MyVisitor::openExpressionPattern(ExpressionPattern* expressionPattern) {
    return true;
}

void MyVisitor::closeExpressionPattern(ExpressionPattern* expressionPattern) {
}

bool MyVisitor::openTuplePatternElement(TuplePatternElement* tuplePatternElement) {
    return true;
}

void MyVisitor::closeTuplePatternElement(TuplePatternElement* tuplePatternElement) {
}

bool MyVisitor::openCaseContent(CaseContent* caseContent) {
    return true;
}

void MyVisitor::closeCaseContent(CaseContent* caseContent) {
}

bool MyVisitor::openType(Type* type) {
    return true;
}

void MyVisitor::closeType(Type* type) {
}

bool MyVisitor::openTypeAnnotation(TypeAnnotation* typeAnnotation) {
    return true;
}

void MyVisitor::closeTypeAnnotation(TypeAnnotation* typeAnnotation) {
}

bool MyVisitor::openSubtype(Subtype* subtype) {
    return true;
}

void MyVisitor::closeSubtype(Subtype* subtype) {
}

bool MyVisitor::openIndexedType(IndexedType* indexedType) {
    return true;
}

void MyVisitor::closeIndexedType(IndexedType* indexedType) {
}

void MyVisitor::visitAge(Age* age) {
}

bool MyVisitor::openTypeInheritanceClause(TypeInheritanceClause* typeInheritanceClause) {
    return true;
}

void MyVisitor::closeTypeInheritanceClause(TypeInheritanceClause* typeInheritanceClause) {
}

bool MyVisitor::openInheritance(Inheritance* inheritance) {
    return true;
}

void MyVisitor::closeInheritance(Inheritance* inheritance) {
}

bool MyVisitor::_isMyVisitor() { return (true); }


}
