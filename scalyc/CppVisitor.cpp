#include "scalyc.h"
using namespace scaly;
namespace scalyc {

void CppVisitor::execute(Program* program) {
    program->accept(this);
}

bool CppVisitor::openProgram(Program* program) {
    return true;
}

void CppVisitor::closeProgram(Program* program) {
}

bool CppVisitor::openCompilationUnit(CompilationUnit* compilationUnit) {
    return true;
}

void CppVisitor::closeCompilationUnit(CompilationUnit* compilationUnit) {
}

bool CppVisitor::openConstantDeclaration(ConstantDeclaration* constantDeclaration) {
    return true;
}

void CppVisitor::closeConstantDeclaration(ConstantDeclaration* constantDeclaration) {
}

bool CppVisitor::openVariableDeclaration(VariableDeclaration* variableDeclaration) {
    return true;
}

void CppVisitor::closeVariableDeclaration(VariableDeclaration* variableDeclaration) {
}

bool CppVisitor::openMutableDeclaration(MutableDeclaration* mutableDeclaration) {
    return true;
}

void CppVisitor::closeMutableDeclaration(MutableDeclaration* mutableDeclaration) {
}

bool CppVisitor::openFunctionDeclaration(FunctionDeclaration* functionDeclaration) {
    return true;
}

void CppVisitor::closeFunctionDeclaration(FunctionDeclaration* functionDeclaration) {
}

bool CppVisitor::openEnumDeclaration(EnumDeclaration* enumDeclaration) {
    return true;
}

void CppVisitor::closeEnumDeclaration(EnumDeclaration* enumDeclaration) {
}

bool CppVisitor::openClassDeclaration(ClassDeclaration* classDeclaration) {
    return true;
}

void CppVisitor::closeClassDeclaration(ClassDeclaration* classDeclaration) {
}

bool CppVisitor::openConstructorDeclaration(ConstructorDeclaration* constructorDeclaration) {
    return true;
}

void CppVisitor::closeConstructorDeclaration(ConstructorDeclaration* constructorDeclaration) {
}

bool CppVisitor::openCodeBlock(CodeBlock* codeBlock) {
    return true;
}

void CppVisitor::closeCodeBlock(CodeBlock* codeBlock) {
}

bool CppVisitor::openSimpleExpression(SimpleExpression* simpleExpression) {
    return true;
}

void CppVisitor::closeSimpleExpression(SimpleExpression* simpleExpression) {
}

bool CppVisitor::openInitializer(Initializer* initializer) {
    return true;
}

void CppVisitor::closeInitializer(Initializer* initializer) {
}

bool CppVisitor::openBindingInitializer(BindingInitializer* bindingInitializer) {
    return true;
}

void CppVisitor::closeBindingInitializer(BindingInitializer* bindingInitializer) {
}

bool CppVisitor::openPatternInitializer(PatternInitializer* patternInitializer) {
    return true;
}

void CppVisitor::closePatternInitializer(PatternInitializer* patternInitializer) {
}

bool CppVisitor::openAdditionalInitializer(AdditionalInitializer* additionalInitializer) {
    return true;
}

void CppVisitor::closeAdditionalInitializer(AdditionalInitializer* additionalInitializer) {
}

void CppVisitor::visitOverrideWord(OverrideWord* overrideWord) {
}

void CppVisitor::visitStaticWord(StaticWord* staticWord) {
}

bool CppVisitor::openFunctionSignature(FunctionSignature* functionSignature) {
    return true;
}

void CppVisitor::closeFunctionSignature(FunctionSignature* functionSignature) {
}

bool CppVisitor::openFunctionResult(FunctionResult* functionResult) {
    return true;
}

void CppVisitor::closeFunctionResult(FunctionResult* functionResult) {
}

bool CppVisitor::openParameterClause(ParameterClause* parameterClause) {
    return true;
}

void CppVisitor::closeParameterClause(ParameterClause* parameterClause) {
}

bool CppVisitor::openConstParameter(ConstParameter* constParameter) {
    return true;
}

void CppVisitor::closeConstParameter(ConstParameter* constParameter) {
}

bool CppVisitor::openVarParameter(VarParameter* varParameter) {
    return true;
}

void CppVisitor::closeVarParameter(VarParameter* varParameter) {
}

bool CppVisitor::openThrowsClause(ThrowsClause* throwsClause) {
    return true;
}

void CppVisitor::closeThrowsClause(ThrowsClause* throwsClause) {
}

bool CppVisitor::openEnumMember(EnumMember* enumMember) {
    return true;
}

void CppVisitor::closeEnumMember(EnumMember* enumMember) {
}

void CppVisitor::visitEnumCase(EnumCase* enumCase) {
}

bool CppVisitor::openAdditionalCase(AdditionalCase* additionalCase) {
    return true;
}

void CppVisitor::closeAdditionalCase(AdditionalCase* additionalCase) {
}

bool CppVisitor::openClassBody(ClassBody* classBody) {
    return true;
}

void CppVisitor::closeClassBody(ClassBody* classBody) {
}

bool CppVisitor::openClassMember(ClassMember* classMember) {
    return true;
}

void CppVisitor::closeClassMember(ClassMember* classMember) {
}

bool CppVisitor::openPrefixExpression(PrefixExpression* prefixExpression) {
    return true;
}

void CppVisitor::closePrefixExpression(PrefixExpression* prefixExpression) {
}

bool CppVisitor::openPostfixExpression(PostfixExpression* postfixExpression) {
    return true;
}

void CppVisitor::closePostfixExpression(PostfixExpression* postfixExpression) {
}

bool CppVisitor::openBinaryOperation(BinaryOperation* binaryOperation) {
    return true;
}

void CppVisitor::closeBinaryOperation(BinaryOperation* binaryOperation) {
}

bool CppVisitor::openAssignment(Assignment* assignment) {
    return true;
}

void CppVisitor::closeAssignment(Assignment* assignment) {
}

bool CppVisitor::openTypeQuery(TypeQuery* typeQuery) {
    return true;
}

void CppVisitor::closeTypeQuery(TypeQuery* typeQuery) {
}

bool CppVisitor::openTypeCast(TypeCast* typeCast) {
    return true;
}

void CppVisitor::closeTypeCast(TypeCast* typeCast) {
}

bool CppVisitor::openCatchClause(CatchClause* catchClause) {
    return true;
}

void CppVisitor::closeCatchClause(CatchClause* catchClause) {
}

bool CppVisitor::openWildCardCatchPattern(WildCardCatchPattern* wildCardCatchPattern) {
    return true;
}

void CppVisitor::closeWildCardCatchPattern(WildCardCatchPattern* wildCardCatchPattern) {
}

bool CppVisitor::openIdentifierCatchPattern(IdentifierCatchPattern* identifierCatchPattern) {
    return true;
}

void CppVisitor::closeIdentifierCatchPattern(IdentifierCatchPattern* identifierCatchPattern) {
}

void CppVisitor::visitOperatorPostfix(OperatorPostfix* operatorPostfix) {
}

bool CppVisitor::openFunctionCall(FunctionCall* functionCall) {
    return true;
}

void CppVisitor::closeFunctionCall(FunctionCall* functionCall) {
}

void CppVisitor::visitMemberExpression(MemberExpression* memberExpression) {
}

bool CppVisitor::openSubscript(Subscript* subscript) {
    return true;
}

void CppVisitor::closeSubscript(Subscript* subscript) {
}

bool CppVisitor::openExpressionElement(ExpressionElement* expressionElement) {
    return true;
}

void CppVisitor::closeExpressionElement(ExpressionElement* expressionElement) {
}

void CppVisitor::visitIdentifierExpression(IdentifierExpression* identifierExpression) {
}

void CppVisitor::visitLiteralExpression(LiteralExpression* literalExpression) {
}

bool CppVisitor::openIfExpression(IfExpression* ifExpression) {
    return true;
}

void CppVisitor::closeIfExpression(IfExpression* ifExpression) {
}

bool CppVisitor::openSwitchExpression(SwitchExpression* switchExpression) {
    return true;
}

void CppVisitor::closeSwitchExpression(SwitchExpression* switchExpression) {
}

bool CppVisitor::openForExpression(ForExpression* forExpression) {
    return true;
}

void CppVisitor::closeForExpression(ForExpression* forExpression) {
}

bool CppVisitor::openWhileExpression(WhileExpression* whileExpression) {
    return true;
}

void CppVisitor::closeWhileExpression(WhileExpression* whileExpression) {
}

bool CppVisitor::openDoExpression(DoExpression* doExpression) {
    return true;
}

void CppVisitor::closeDoExpression(DoExpression* doExpression) {
}

bool CppVisitor::openParenthesizedExpression(ParenthesizedExpression* parenthesizedExpression) {
    return true;
}

void CppVisitor::closeParenthesizedExpression(ParenthesizedExpression* parenthesizedExpression) {
}

bool CppVisitor::openReturnExpression(ReturnExpression* returnExpression) {
    return true;
}

void CppVisitor::closeReturnExpression(ReturnExpression* returnExpression) {
}

bool CppVisitor::openThrowExpression(ThrowExpression* throwExpression) {
    return true;
}

void CppVisitor::closeThrowExpression(ThrowExpression* throwExpression) {
}

bool CppVisitor::openBreakExpression(BreakExpression* breakExpression) {
    return true;
}

void CppVisitor::closeBreakExpression(BreakExpression* breakExpression) {
}

bool CppVisitor::openConstructorCall(ConstructorCall* constructorCall) {
    return true;
}

void CppVisitor::closeConstructorCall(ConstructorCall* constructorCall) {
}

void CppVisitor::visitThisExpression(ThisExpression* thisExpression) {
}

void CppVisitor::visitNullExpression(NullExpression* nullExpression) {
}

bool CppVisitor::openElseClause(ElseClause* elseClause) {
    return true;
}

void CppVisitor::closeElseClause(ElseClause* elseClause) {
}

bool CppVisitor::openCurliedSwitchBody(CurliedSwitchBody* curliedSwitchBody) {
    return true;
}

void CppVisitor::closeCurliedSwitchBody(CurliedSwitchBody* curliedSwitchBody) {
}

bool CppVisitor::openNakedSwitchBody(NakedSwitchBody* nakedSwitchBody) {
    return true;
}

void CppVisitor::closeNakedSwitchBody(NakedSwitchBody* nakedSwitchBody) {
}

bool CppVisitor::openSwitchCase(SwitchCase* switchCase) {
    return true;
}

void CppVisitor::closeSwitchCase(SwitchCase* switchCase) {
}

bool CppVisitor::openItemCaseLabel(ItemCaseLabel* itemCaseLabel) {
    return true;
}

void CppVisitor::closeItemCaseLabel(ItemCaseLabel* itemCaseLabel) {
}

void CppVisitor::visitDefaultCaseLabel(DefaultCaseLabel* defaultCaseLabel) {
}

bool CppVisitor::openCaseItem(CaseItem* caseItem) {
    return true;
}

void CppVisitor::closeCaseItem(CaseItem* caseItem) {
}

void CppVisitor::visitWildcardPattern(WildcardPattern* wildcardPattern) {
}

bool CppVisitor::openIdentifierPattern(IdentifierPattern* identifierPattern) {
    return true;
}

void CppVisitor::closeIdentifierPattern(IdentifierPattern* identifierPattern) {
}

bool CppVisitor::openTuplePattern(TuplePattern* tuplePattern) {
    return true;
}

void CppVisitor::closeTuplePattern(TuplePattern* tuplePattern) {
}

bool CppVisitor::openExpressionPattern(ExpressionPattern* expressionPattern) {
    return true;
}

void CppVisitor::closeExpressionPattern(ExpressionPattern* expressionPattern) {
}

bool CppVisitor::openTuplePatternElement(TuplePatternElement* tuplePatternElement) {
    return true;
}

void CppVisitor::closeTuplePatternElement(TuplePatternElement* tuplePatternElement) {
}

bool CppVisitor::openCaseContent(CaseContent* caseContent) {
    return true;
}

void CppVisitor::closeCaseContent(CaseContent* caseContent) {
}

bool CppVisitor::openType(Type* type) {
    return true;
}

void CppVisitor::closeType(Type* type) {
}

bool CppVisitor::openTypeAnnotation(TypeAnnotation* typeAnnotation) {
    return true;
}

void CppVisitor::closeTypeAnnotation(TypeAnnotation* typeAnnotation) {
}

bool CppVisitor::openSubtype(Subtype* subtype) {
    return true;
}

void CppVisitor::closeSubtype(Subtype* subtype) {
}

bool CppVisitor::openIndexedType(IndexedType* indexedType) {
    return true;
}

void CppVisitor::closeIndexedType(IndexedType* indexedType) {
}

void CppVisitor::visitPointer(Pointer* pointer) {
}

void CppVisitor::visitRoot(Root* root) {
}

void CppVisitor::visitLocal(Local* local) {
}

void CppVisitor::visitReference(Reference* reference) {
}

void CppVisitor::visitThrown(Thrown* thrown) {
}

bool CppVisitor::openTypeInheritanceClause(TypeInheritanceClause* typeInheritanceClause) {
    return true;
}

void CppVisitor::closeTypeInheritanceClause(TypeInheritanceClause* typeInheritanceClause) {
}

bool CppVisitor::openInheritance(Inheritance* inheritance) {
    return true;
}

void CppVisitor::closeInheritance(Inheritance* inheritance) {
}

bool CppVisitor::_isCppVisitor() { return (true); }

bool CppVisitor::_isHeaderVisitor() { return (false); }
bool CppVisitor::_isSourceVisitor() { return (false); }


}
