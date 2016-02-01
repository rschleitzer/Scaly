#include "scalyc.h"
using namespace scaly;
namespace scalyc {


void CppVisitor::OpenProgram(Program& program) {
    programName = program.name;
    programDirectory = program.directory;
}

void CppVisitor::CloseProgram(Program& program) {
    programName = 0;
    programDirectory = 0;
}

void CppVisitor::OpenCompilationUnit(CompilationUnit& compilationUnit) {
}

void CppVisitor::CloseCompilationUnit(CompilationUnit& compilationUnit) {
}

void CppVisitor::OpenStatementWithSemicolon(StatementWithSemicolon& statementWithSemicolon) {
}

void CppVisitor::CloseStatementWithSemicolon(StatementWithSemicolon& statementWithSemicolon) {
}

void CppVisitor::OpenUseDeclaration(UseDeclaration& useDeclaration) {
}

void CppVisitor::CloseUseDeclaration(UseDeclaration& useDeclaration) {
}

void CppVisitor::OpenPathIdentifier(PathIdentifier& pathIdentifier) {
}

void CppVisitor::ClosePathIdentifier(PathIdentifier& pathIdentifier) {
}

void CppVisitor::VisitPathItem(PathItem& pathItem) {
}

void CppVisitor::OpenInitializer(Initializer& initializer) {
}

void CppVisitor::CloseInitializer(Initializer& initializer) {
}

void CppVisitor::OpenConstantDeclaration(ConstantDeclaration& constantDeclaration) {
}

void CppVisitor::CloseConstantDeclaration(ConstantDeclaration& constantDeclaration) {
}

void CppVisitor::OpenVariableDeclaration(VariableDeclaration& variableDeclaration) {
}

void CppVisitor::CloseVariableDeclaration(VariableDeclaration& variableDeclaration) {
}

void CppVisitor::OpenBindingInitializer(BindingInitializer& bindingInitializer) {
}

void CppVisitor::CloseBindingInitializer(BindingInitializer& bindingInitializer) {
}

void CppVisitor::OpenPatternInitializer(PatternInitializer& patternInitializer) {
}

void CppVisitor::ClosePatternInitializer(PatternInitializer& patternInitializer) {
}

void CppVisitor::OpenAdditionalInitializer(AdditionalInitializer& additionalInitializer) {
}

void CppVisitor::CloseAdditionalInitializer(AdditionalInitializer& additionalInitializer) {
}

void CppVisitor::OpenFunctionDeclaration(FunctionDeclaration& functionDeclaration) {
}

void CppVisitor::CloseFunctionDeclaration(FunctionDeclaration& functionDeclaration) {
}

void CppVisitor::OpenInitializerDeclaration(InitializerDeclaration& initializerDeclaration) {
}

void CppVisitor::CloseInitializerDeclaration(InitializerDeclaration& initializerDeclaration) {
}

void CppVisitor::VisitOverride(Override& override) {
}

void CppVisitor::VisitStaticWord(StaticWord& staticWord) {
}

void CppVisitor::VisitIdentifierFunction(IdentifierFunction& identifierFunction) {
}

void CppVisitor::OpenFunctionSignature(FunctionSignature& functionSignature) {
}

void CppVisitor::CloseFunctionSignature(FunctionSignature& functionSignature) {
}

void CppVisitor::OpenFunctionResult(FunctionResult& functionResult) {
}

void CppVisitor::CloseFunctionResult(FunctionResult& functionResult) {
}

void CppVisitor::OpenParameterClause(ParameterClause& parameterClause) {
}

void CppVisitor::CloseParameterClause(ParameterClause& parameterClause) {
}

void CppVisitor::OpenConstParameter(ConstParameter& constParameter) {
    constParameterName = constParameter.name;
}

void CppVisitor::CloseConstParameter(ConstParameter& constParameter) {
    constParameterName = 0;
}

void CppVisitor::OpenVarParameter(VarParameter& varParameter) {
    varParameterName = varParameter.name;
}

void CppVisitor::CloseVarParameter(VarParameter& varParameter) {
    varParameterName = 0;
}

void CppVisitor::OpenThrowsClause(ThrowsClause& throwsClause) {
}

void CppVisitor::CloseThrowsClause(ThrowsClause& throwsClause) {
}

void CppVisitor::OpenEnumDeclaration(EnumDeclaration& enumDeclaration) {
    enumDeclarationName = enumDeclaration.name;
}

void CppVisitor::CloseEnumDeclaration(EnumDeclaration& enumDeclaration) {
    enumDeclarationName = 0;
}

void CppVisitor::OpenEnumMember(EnumMember& enumMember) {
}

void CppVisitor::CloseEnumMember(EnumMember& enumMember) {
}

void CppVisitor::OpenTupleType(TupleType& tupleType) {
}

void CppVisitor::CloseTupleType(TupleType& tupleType) {
}

void CppVisitor::OpenAdditionalType(AdditionalType& additionalType) {
}

void CppVisitor::CloseAdditionalType(AdditionalType& additionalType) {
}

void CppVisitor::VisitEnumCase(EnumCase& enumCase) {
}

void CppVisitor::OpenAdditionalCase(AdditionalCase& additionalCase) {
}

void CppVisitor::CloseAdditionalCase(AdditionalCase& additionalCase) {
}

void CppVisitor::OpenClassDeclaration(ClassDeclaration& classDeclaration) {
    classDeclarationName = classDeclaration.name;
}

void CppVisitor::CloseClassDeclaration(ClassDeclaration& classDeclaration) {
    classDeclarationName = 0;
}

void CppVisitor::OpenGenericArgumentClause(GenericArgumentClause& genericArgumentClause) {
}

void CppVisitor::CloseGenericArgumentClause(GenericArgumentClause& genericArgumentClause) {
}

void CppVisitor::VisitGenericParameter(GenericParameter& genericParameter) {
}

void CppVisitor::OpenClassMember(ClassMember& classMember) {
}

void CppVisitor::CloseClassMember(ClassMember& classMember) {
}

void CppVisitor::OpenCodeBlock(CodeBlock& codeBlock) {
}

void CppVisitor::CloseCodeBlock(CodeBlock& codeBlock) {
}

void CppVisitor::OpenSimpleExpression(SimpleExpression& simpleExpression) {
}

void CppVisitor::CloseSimpleExpression(SimpleExpression& simpleExpression) {
}

void CppVisitor::OpenPrefixExpression(PrefixExpression& prefixExpression) {
}

void CppVisitor::ClosePrefixExpression(PrefixExpression& prefixExpression) {
}

void CppVisitor::OpenPostfixExpression(PostfixExpression& postfixExpression) {
}

void CppVisitor::ClosePostfixExpression(PostfixExpression& postfixExpression) {
}

void CppVisitor::OpenBinaryOperation(BinaryOperation& binaryOperation) {
}

void CppVisitor::CloseBinaryOperation(BinaryOperation& binaryOperation) {
}

void CppVisitor::OpenAssignment(Assignment& assignment) {
}

void CppVisitor::CloseAssignment(Assignment& assignment) {
}

void CppVisitor::OpenTypeQuery(TypeQuery& typeQuery) {
}

void CppVisitor::CloseTypeQuery(TypeQuery& typeQuery) {
}

void CppVisitor::OpenTypeCast(TypeCast& typeCast) {
}

void CppVisitor::CloseTypeCast(TypeCast& typeCast) {
}

void CppVisitor::OpenCatchClause(CatchClause& catchClause) {
}

void CppVisitor::CloseCatchClause(CatchClause& catchClause) {
}

void CppVisitor::OpenWildCardCatchPattern(WildCardCatchPattern& wildCardCatchPattern) {
}

void CppVisitor::CloseWildCardCatchPattern(WildCardCatchPattern& wildCardCatchPattern) {
}

void CppVisitor::OpenPathItemCatchPattern(PathItemCatchPattern& pathItemCatchPattern) {
}

void CppVisitor::ClosePathItemCatchPattern(PathItemCatchPattern& pathItemCatchPattern) {
}

void CppVisitor::VisitOperatorPostfix(OperatorPostfix& operatorPostfix) {
}

void CppVisitor::OpenFunctionCall(FunctionCall& functionCall) {
}

void CppVisitor::CloseFunctionCall(FunctionCall& functionCall) {
}

void CppVisitor::OpenExplicitMemberExpression(ExplicitMemberExpression& explicitMemberExpression) {
}

void CppVisitor::CloseExplicitMemberExpression(ExplicitMemberExpression& explicitMemberExpression) {
}

void CppVisitor::OpenSubscript(Subscript& subscript) {
}

void CppVisitor::CloseSubscript(Subscript& subscript) {
}

void CppVisitor::OpenExpressionElement(ExpressionElement& expressionElement) {
}

void CppVisitor::CloseExpressionElement(ExpressionElement& expressionElement) {
}

void CppVisitor::OpenNamedMemberPostfix(NamedMemberPostfix& namedMemberPostfix) {
}

void CppVisitor::CloseNamedMemberPostfix(NamedMemberPostfix& namedMemberPostfix) {
}

void CppVisitor::OpenParenthesizedExpression(ParenthesizedExpression& parenthesizedExpression) {
}

void CppVisitor::CloseParenthesizedExpression(ParenthesizedExpression& parenthesizedExpression) {
}

void CppVisitor::VisitLiteralExpression(LiteralExpression& literalExpression) {
}

void CppVisitor::VisitIdentifierExpression(IdentifierExpression& identifierExpression) {
}

void CppVisitor::OpenIfExpression(IfExpression& ifExpression) {
}

void CppVisitor::CloseIfExpression(IfExpression& ifExpression) {
}

void CppVisitor::OpenElseClause(ElseClause& elseClause) {
}

void CppVisitor::CloseElseClause(ElseClause& elseClause) {
}

void CppVisitor::OpenSwitchExpression(SwitchExpression& switchExpression) {
}

void CppVisitor::CloseSwitchExpression(SwitchExpression& switchExpression) {
}

void CppVisitor::OpenCurliedSwitchBody(CurliedSwitchBody& curliedSwitchBody) {
}

void CppVisitor::CloseCurliedSwitchBody(CurliedSwitchBody& curliedSwitchBody) {
}

void CppVisitor::OpenNakedSwitchBody(NakedSwitchBody& nakedSwitchBody) {
}

void CppVisitor::CloseNakedSwitchBody(NakedSwitchBody& nakedSwitchBody) {
}

void CppVisitor::OpenSwitchCase(SwitchCase& switchCase) {
}

void CppVisitor::CloseSwitchCase(SwitchCase& switchCase) {
}

void CppVisitor::OpenItemCaseLabel(ItemCaseLabel& itemCaseLabel) {
}

void CppVisitor::CloseItemCaseLabel(ItemCaseLabel& itemCaseLabel) {
}

void CppVisitor::OpenCaseItem(CaseItem& caseItem) {
}

void CppVisitor::CloseCaseItem(CaseItem& caseItem) {
}

void CppVisitor::OpenForExpression(ForExpression& forExpression) {
}

void CppVisitor::CloseForExpression(ForExpression& forExpression) {
}

void CppVisitor::OpenForEach(ForEach& forEach) {
}

void CppVisitor::CloseForEach(ForEach& forEach) {
}

void CppVisitor::OpenPlainFor(PlainFor& plainFor) {
}

void CppVisitor::ClosePlainFor(PlainFor& plainFor) {
}

void CppVisitor::OpenReturnExpression(ReturnExpression& returnExpression) {
}

void CppVisitor::CloseReturnExpression(ReturnExpression& returnExpression) {
}

void CppVisitor::OpenThrowExpression(ThrowExpression& throwExpression) {
}

void CppVisitor::CloseThrowExpression(ThrowExpression& throwExpression) {
}

void CppVisitor::OpenBreakExpression(BreakExpression& breakExpression) {
}

void CppVisitor::CloseBreakExpression(BreakExpression& breakExpression) {
}

void CppVisitor::VisitWildcardPattern(WildcardPattern& wildcardPattern) {
}

void CppVisitor::OpenIdentifierPattern(IdentifierPattern& identifierPattern) {
    identifierPatternIdentifier = identifierPattern.identifier;
}

void CppVisitor::CloseIdentifierPattern(IdentifierPattern& identifierPattern) {
    identifierPatternIdentifier = 0;
}

void CppVisitor::OpenTuplePattern(TuplePattern& tuplePattern) {
}

void CppVisitor::CloseTuplePattern(TuplePattern& tuplePattern) {
}

void CppVisitor::OpenTuplePatternElement(TuplePatternElement& tuplePatternElement) {
}

void CppVisitor::CloseTuplePatternElement(TuplePatternElement& tuplePatternElement) {
}

void CppVisitor::OpenExpressionPattern(ExpressionPattern& expressionPattern) {
}

void CppVisitor::CloseExpressionPattern(ExpressionPattern& expressionPattern) {
}

void CppVisitor::VisitDefaultCaseLabel(DefaultCaseLabel& defaultCaseLabel) {
}

void CppVisitor::OpenBlockCaseContent(BlockCaseContent& blockCaseContent) {
}

void CppVisitor::CloseBlockCaseContent(BlockCaseContent& blockCaseContent) {
}

void CppVisitor::VisitEmptyCaseContent(EmptyCaseContent& emptyCaseContent) {
}

void CppVisitor::OpenInitializerCall(InitializerCall& initializerCall) {
}

void CppVisitor::CloseInitializerCall(InitializerCall& initializerCall) {
}

void CppVisitor::OpenThisDot(ThisDot& thisDot) {
}

void CppVisitor::CloseThisDot(ThisDot& thisDot) {
}

void CppVisitor::OpenThisSubscript(ThisSubscript& thisSubscript) {
}

void CppVisitor::CloseThisSubscript(ThisSubscript& thisSubscript) {
}

void CppVisitor::VisitThisWord(ThisWord& thisWord) {
}

void CppVisitor::VisitThisInit(ThisInit& thisInit) {
}

void CppVisitor::VisitThisMember(ThisMember& thisMember) {
}

void CppVisitor::OpenSuperDot(SuperDot& superDot) {
}

void CppVisitor::CloseSuperDot(SuperDot& superDot) {
}

void CppVisitor::OpenSuperSubscript(SuperSubscript& superSubscript) {
}

void CppVisitor::CloseSuperSubscript(SuperSubscript& superSubscript) {
}

void CppVisitor::VisitSuperInit(SuperInit& superInit) {
}

void CppVisitor::VisitSuperMember(SuperMember& superMember) {
}

void CppVisitor::OpenTypeAnnotation(TypeAnnotation& typeAnnotation) {
}

void CppVisitor::CloseTypeAnnotation(TypeAnnotation& typeAnnotation) {
}

void CppVisitor::OpenTypeIdentifier(TypeIdentifier& typeIdentifier) {
    typeIdentifierName = typeIdentifier.name;
}

void CppVisitor::CloseTypeIdentifier(TypeIdentifier& typeIdentifier) {
    typeIdentifierName = 0;
}

void CppVisitor::OpenSubtypeIdentifier(SubtypeIdentifier& subtypeIdentifier) {
}

void CppVisitor::CloseSubtypeIdentifier(SubtypeIdentifier& subtypeIdentifier) {
}

void CppVisitor::OpenArrayType(ArrayType& arrayType) {
}

void CppVisitor::CloseArrayType(ArrayType& arrayType) {
}

void CppVisitor::VisitOptionalType(OptionalType& optionalType) {
}

void CppVisitor::OpenTypeInheritanceClause(TypeInheritanceClause& typeInheritanceClause) {
}

void CppVisitor::CloseTypeInheritanceClause(TypeInheritanceClause& typeInheritanceClause) {
}

void CppVisitor::OpenInheritance(Inheritance& inheritance) {
}

void CppVisitor::CloseInheritance(Inheritance& inheritance) {
}

}
