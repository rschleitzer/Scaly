#include "scalyc.h"
using namespace scaly;
namespace scalyc {

bool HeaderVisitor::openProgram(Program* program) {
    return true;
}

void HeaderVisitor::closeProgram(Program* program) {
}

bool HeaderVisitor::openCompilationUnit(CompilationUnit* compilationUnit) {
    return true;
}

void HeaderVisitor::closeCompilationUnit(CompilationUnit* compilationUnit) {
}

bool HeaderVisitor::openConstantDeclaration(ConstantDeclaration* constantDeclaration) {
    return true;
}

void HeaderVisitor::closeConstantDeclaration(ConstantDeclaration* constantDeclaration) {
}

bool HeaderVisitor::openVariableDeclaration(VariableDeclaration* variableDeclaration) {
    return true;
}

void HeaderVisitor::closeVariableDeclaration(VariableDeclaration* variableDeclaration) {
}

bool HeaderVisitor::openMutableDeclaration(MutableDeclaration* mutableDeclaration) {
    return true;
}

void HeaderVisitor::closeMutableDeclaration(MutableDeclaration* mutableDeclaration) {
}

bool HeaderVisitor::openFunctionDeclaration(FunctionDeclaration* functionDeclaration) {
    return true;
}

void HeaderVisitor::closeFunctionDeclaration(FunctionDeclaration* functionDeclaration) {
}

bool HeaderVisitor::openEnumDeclaration(EnumDeclaration* enumDeclaration) {
    return true;
}

void HeaderVisitor::closeEnumDeclaration(EnumDeclaration* enumDeclaration) {
}

bool HeaderVisitor::openClassDeclaration(ClassDeclaration* classDeclaration) {
    return true;
}

void HeaderVisitor::closeClassDeclaration(ClassDeclaration* classDeclaration) {
}

bool HeaderVisitor::openConstructorDeclaration(ConstructorDeclaration* constructorDeclaration) {
    return true;
}

void HeaderVisitor::closeConstructorDeclaration(ConstructorDeclaration* constructorDeclaration) {
}

bool HeaderVisitor::openCodeBlock(CodeBlock* codeBlock) {
    return true;
}

void HeaderVisitor::closeCodeBlock(CodeBlock* codeBlock) {
}

bool HeaderVisitor::openSimpleExpression(SimpleExpression* simpleExpression) {
    return true;
}

void HeaderVisitor::closeSimpleExpression(SimpleExpression* simpleExpression) {
}

bool HeaderVisitor::openInitializer(Initializer* initializer) {
    return true;
}

void HeaderVisitor::closeInitializer(Initializer* initializer) {
}

bool HeaderVisitor::openBindingInitializer(BindingInitializer* bindingInitializer) {
    return true;
}

void HeaderVisitor::closeBindingInitializer(BindingInitializer* bindingInitializer) {
}

bool HeaderVisitor::openPatternInitializer(PatternInitializer* patternInitializer) {
    return true;
}

void HeaderVisitor::closePatternInitializer(PatternInitializer* patternInitializer) {
}

bool HeaderVisitor::openAdditionalInitializer(AdditionalInitializer* additionalInitializer) {
    return true;
}

void HeaderVisitor::closeAdditionalInitializer(AdditionalInitializer* additionalInitializer) {
}

void HeaderVisitor::visitOverrideWord(OverrideWord* overrideWord) {
}

void HeaderVisitor::visitStaticWord(StaticWord* staticWord) {
}

bool HeaderVisitor::openFunctionSignature(FunctionSignature* functionSignature) {
    return true;
}

void HeaderVisitor::closeFunctionSignature(FunctionSignature* functionSignature) {
}

bool HeaderVisitor::openFunctionResult(FunctionResult* functionResult) {
    return true;
}

void HeaderVisitor::closeFunctionResult(FunctionResult* functionResult) {
}

bool HeaderVisitor::openParameterClause(ParameterClause* parameterClause) {
    return true;
}

void HeaderVisitor::closeParameterClause(ParameterClause* parameterClause) {
}

bool HeaderVisitor::openConstParameter(ConstParameter* constParameter) {
    return true;
}

void HeaderVisitor::closeConstParameter(ConstParameter* constParameter) {
}

bool HeaderVisitor::openVarParameter(VarParameter* varParameter) {
    return true;
}

void HeaderVisitor::closeVarParameter(VarParameter* varParameter) {
}

bool HeaderVisitor::openThrowsClause(ThrowsClause* throwsClause) {
    return true;
}

void HeaderVisitor::closeThrowsClause(ThrowsClause* throwsClause) {
}

bool HeaderVisitor::openEnumMember(EnumMember* enumMember) {
    return true;
}

void HeaderVisitor::closeEnumMember(EnumMember* enumMember) {
}

void HeaderVisitor::visitEnumCase(EnumCase* enumCase) {
}

bool HeaderVisitor::openAdditionalCase(AdditionalCase* additionalCase) {
    return true;
}

void HeaderVisitor::closeAdditionalCase(AdditionalCase* additionalCase) {
}

bool HeaderVisitor::openClassBody(ClassBody* classBody) {
    return true;
}

void HeaderVisitor::closeClassBody(ClassBody* classBody) {
}

bool HeaderVisitor::openClassMember(ClassMember* classMember) {
    return true;
}

void HeaderVisitor::closeClassMember(ClassMember* classMember) {
}

bool HeaderVisitor::openPrefixExpression(PrefixExpression* prefixExpression) {
    return true;
}

void HeaderVisitor::closePrefixExpression(PrefixExpression* prefixExpression) {
}

bool HeaderVisitor::openPostfixExpression(PostfixExpression* postfixExpression) {
    return true;
}

void HeaderVisitor::closePostfixExpression(PostfixExpression* postfixExpression) {
}

bool HeaderVisitor::openBinaryOperation(BinaryOperation* binaryOperation) {
    return true;
}

void HeaderVisitor::closeBinaryOperation(BinaryOperation* binaryOperation) {
}

bool HeaderVisitor::openAssignment(Assignment* assignment) {
    return true;
}

void HeaderVisitor::closeAssignment(Assignment* assignment) {
}

bool HeaderVisitor::openTypeQuery(TypeQuery* typeQuery) {
    return true;
}

void HeaderVisitor::closeTypeQuery(TypeQuery* typeQuery) {
}

bool HeaderVisitor::openTypeCast(TypeCast* typeCast) {
    return true;
}

void HeaderVisitor::closeTypeCast(TypeCast* typeCast) {
}

bool HeaderVisitor::openCatchClause(CatchClause* catchClause) {
    return true;
}

void HeaderVisitor::closeCatchClause(CatchClause* catchClause) {
}

bool HeaderVisitor::openWildCardCatchPattern(WildCardCatchPattern* wildCardCatchPattern) {
    return true;
}

void HeaderVisitor::closeWildCardCatchPattern(WildCardCatchPattern* wildCardCatchPattern) {
}

bool HeaderVisitor::openIdentifierCatchPattern(IdentifierCatchPattern* identifierCatchPattern) {
    return true;
}

void HeaderVisitor::closeIdentifierCatchPattern(IdentifierCatchPattern* identifierCatchPattern) {
}

void HeaderVisitor::visitOperatorPostfix(OperatorPostfix* operatorPostfix) {
}

bool HeaderVisitor::openFunctionCall(FunctionCall* functionCall) {
    return true;
}

void HeaderVisitor::closeFunctionCall(FunctionCall* functionCall) {
}

void HeaderVisitor::visitMemberExpression(MemberExpression* memberExpression) {
}

bool HeaderVisitor::openSubscript(Subscript* subscript) {
    return true;
}

void HeaderVisitor::closeSubscript(Subscript* subscript) {
}

bool HeaderVisitor::openExpressionElement(ExpressionElement* expressionElement) {
    return true;
}

void HeaderVisitor::closeExpressionElement(ExpressionElement* expressionElement) {
}

void HeaderVisitor::visitIdentifierExpression(IdentifierExpression* identifierExpression) {
}

void HeaderVisitor::visitLiteralExpression(LiteralExpression* literalExpression) {
}

bool HeaderVisitor::openIfExpression(IfExpression* ifExpression) {
    return true;
}

void HeaderVisitor::closeIfExpression(IfExpression* ifExpression) {
}

bool HeaderVisitor::openSwitchExpression(SwitchExpression* switchExpression) {
    return true;
}

void HeaderVisitor::closeSwitchExpression(SwitchExpression* switchExpression) {
}

bool HeaderVisitor::openForExpression(ForExpression* forExpression) {
    return true;
}

void HeaderVisitor::closeForExpression(ForExpression* forExpression) {
}

bool HeaderVisitor::openWhileExpression(WhileExpression* whileExpression) {
    return true;
}

void HeaderVisitor::closeWhileExpression(WhileExpression* whileExpression) {
}

bool HeaderVisitor::openDoExpression(DoExpression* doExpression) {
    return true;
}

void HeaderVisitor::closeDoExpression(DoExpression* doExpression) {
}

bool HeaderVisitor::openParenthesizedExpression(ParenthesizedExpression* parenthesizedExpression) {
    return true;
}

void HeaderVisitor::closeParenthesizedExpression(ParenthesizedExpression* parenthesizedExpression) {
}

bool HeaderVisitor::openReturnExpression(ReturnExpression* returnExpression) {
    return true;
}

void HeaderVisitor::closeReturnExpression(ReturnExpression* returnExpression) {
}

bool HeaderVisitor::openThrowExpression(ThrowExpression* throwExpression) {
    return true;
}

void HeaderVisitor::closeThrowExpression(ThrowExpression* throwExpression) {
}

bool HeaderVisitor::openBreakExpression(BreakExpression* breakExpression) {
    return true;
}

void HeaderVisitor::closeBreakExpression(BreakExpression* breakExpression) {
}

bool HeaderVisitor::openConstructorCall(ConstructorCall* constructorCall) {
    return true;
}

void HeaderVisitor::closeConstructorCall(ConstructorCall* constructorCall) {
}

void HeaderVisitor::visitThisExpression(ThisExpression* thisExpression) {
}

void HeaderVisitor::visitNullExpression(NullExpression* nullExpression) {
}

bool HeaderVisitor::openElseClause(ElseClause* elseClause) {
    return true;
}

void HeaderVisitor::closeElseClause(ElseClause* elseClause) {
}

bool HeaderVisitor::openCurliedSwitchBody(CurliedSwitchBody* curliedSwitchBody) {
    return true;
}

void HeaderVisitor::closeCurliedSwitchBody(CurliedSwitchBody* curliedSwitchBody) {
}

bool HeaderVisitor::openNakedSwitchBody(NakedSwitchBody* nakedSwitchBody) {
    return true;
}

void HeaderVisitor::closeNakedSwitchBody(NakedSwitchBody* nakedSwitchBody) {
}

bool HeaderVisitor::openSwitchCase(SwitchCase* switchCase) {
    return true;
}

void HeaderVisitor::closeSwitchCase(SwitchCase* switchCase) {
}

bool HeaderVisitor::openItemCaseLabel(ItemCaseLabel* itemCaseLabel) {
    return true;
}

void HeaderVisitor::closeItemCaseLabel(ItemCaseLabel* itemCaseLabel) {
}

void HeaderVisitor::visitDefaultCaseLabel(DefaultCaseLabel* defaultCaseLabel) {
}

bool HeaderVisitor::openCaseItem(CaseItem* caseItem) {
    return true;
}

void HeaderVisitor::closeCaseItem(CaseItem* caseItem) {
}

void HeaderVisitor::visitWildcardPattern(WildcardPattern* wildcardPattern) {
}

bool HeaderVisitor::openIdentifierPattern(IdentifierPattern* identifierPattern) {
    return true;
}

void HeaderVisitor::closeIdentifierPattern(IdentifierPattern* identifierPattern) {
}

bool HeaderVisitor::openTuplePattern(TuplePattern* tuplePattern) {
    return true;
}

void HeaderVisitor::closeTuplePattern(TuplePattern* tuplePattern) {
}

bool HeaderVisitor::openExpressionPattern(ExpressionPattern* expressionPattern) {
    return true;
}

void HeaderVisitor::closeExpressionPattern(ExpressionPattern* expressionPattern) {
}

bool HeaderVisitor::openTuplePatternElement(TuplePatternElement* tuplePatternElement) {
    return true;
}

void HeaderVisitor::closeTuplePatternElement(TuplePatternElement* tuplePatternElement) {
}

bool HeaderVisitor::openCaseContent(CaseContent* caseContent) {
    return true;
}

void HeaderVisitor::closeCaseContent(CaseContent* caseContent) {
}

bool HeaderVisitor::openType(Type* type) {
    return true;
}

void HeaderVisitor::closeType(Type* type) {
}

bool HeaderVisitor::openTypeAnnotation(TypeAnnotation* typeAnnotation) {
    return true;
}

void HeaderVisitor::closeTypeAnnotation(TypeAnnotation* typeAnnotation) {
}

bool HeaderVisitor::openSubtype(Subtype* subtype) {
    return true;
}

void HeaderVisitor::closeSubtype(Subtype* subtype) {
}

bool HeaderVisitor::openIndexedType(IndexedType* indexedType) {
    return true;
}

void HeaderVisitor::closeIndexedType(IndexedType* indexedType) {
}

void HeaderVisitor::visitPointer(Pointer* pointer) {
}

void HeaderVisitor::visitRoot(Root* root) {
}

void HeaderVisitor::visitLocal(Local* local) {
}

void HeaderVisitor::visitReference(Reference* reference) {
}

void HeaderVisitor::visitThrown(Thrown* thrown) {
}

bool HeaderVisitor::openTypeInheritanceClause(TypeInheritanceClause* typeInheritanceClause) {
    return true;
}

void HeaderVisitor::closeTypeInheritanceClause(TypeInheritanceClause* typeInheritanceClause) {
}

bool HeaderVisitor::openInheritance(Inheritance* inheritance) {
    return true;
}

void HeaderVisitor::closeInheritance(Inheritance* inheritance) {
}

bool HeaderVisitor::_isHeaderVisitor() { return (true); }


}
