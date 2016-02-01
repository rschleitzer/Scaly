#include "scalyc.h"
using namespace scaly;
namespace scalyc {


void MyVisitor::OpenProgram(Program& program) {
    programName = program.name;
    programDirectory = program.directory;
}

void MyVisitor::CloseProgram(Program& program) {
    programName = 0;
    programDirectory = 0;
}

void MyVisitor::OpenCompilationUnit(CompilationUnit& compilationUnit) {
}

void MyVisitor::CloseCompilationUnit(CompilationUnit& compilationUnit) {
}

void MyVisitor::OpenStatementWithSemicolon(StatementWithSemicolon& statementWithSemicolon) {
}

void MyVisitor::CloseStatementWithSemicolon(StatementWithSemicolon& statementWithSemicolon) {
}

void MyVisitor::OpenUseDeclaration(UseDeclaration& useDeclaration) {
}

void MyVisitor::CloseUseDeclaration(UseDeclaration& useDeclaration) {
}

void MyVisitor::OpenPathIdentifier(PathIdentifier& pathIdentifier) {
}

void MyVisitor::ClosePathIdentifier(PathIdentifier& pathIdentifier) {
}

void MyVisitor::VisitPathItem(PathItem& pathItem) {
}

void MyVisitor::OpenInitializer(Initializer& initializer) {
}

void MyVisitor::CloseInitializer(Initializer& initializer) {
}

void MyVisitor::OpenConstantDeclaration(ConstantDeclaration& constantDeclaration) {
}

void MyVisitor::CloseConstantDeclaration(ConstantDeclaration& constantDeclaration) {
}

void MyVisitor::OpenVariableDeclaration(VariableDeclaration& variableDeclaration) {
}

void MyVisitor::CloseVariableDeclaration(VariableDeclaration& variableDeclaration) {
}

void MyVisitor::OpenBindingInitializer(BindingInitializer& bindingInitializer) {
}

void MyVisitor::CloseBindingInitializer(BindingInitializer& bindingInitializer) {
}

void MyVisitor::OpenPatternInitializer(PatternInitializer& patternInitializer) {
}

void MyVisitor::ClosePatternInitializer(PatternInitializer& patternInitializer) {
}

void MyVisitor::OpenAdditionalInitializer(AdditionalInitializer& additionalInitializer) {
}

void MyVisitor::CloseAdditionalInitializer(AdditionalInitializer& additionalInitializer) {
}

void MyVisitor::OpenFunctionDeclaration(FunctionDeclaration& functionDeclaration) {
}

void MyVisitor::CloseFunctionDeclaration(FunctionDeclaration& functionDeclaration) {
}

void MyVisitor::OpenInitializerDeclaration(InitializerDeclaration& initializerDeclaration) {
}

void MyVisitor::CloseInitializerDeclaration(InitializerDeclaration& initializerDeclaration) {
}

void MyVisitor::VisitOverride(Override& override) {
}

void MyVisitor::VisitStaticWord(StaticWord& staticWord) {
}

void MyVisitor::VisitIdentifierFunction(IdentifierFunction& identifierFunction) {
}

void MyVisitor::OpenFunctionSignature(FunctionSignature& functionSignature) {
}

void MyVisitor::CloseFunctionSignature(FunctionSignature& functionSignature) {
}

void MyVisitor::OpenFunctionResult(FunctionResult& functionResult) {
}

void MyVisitor::CloseFunctionResult(FunctionResult& functionResult) {
}

void MyVisitor::OpenParameterClause(ParameterClause& parameterClause) {
}

void MyVisitor::CloseParameterClause(ParameterClause& parameterClause) {
}

void MyVisitor::OpenConstParameter(ConstParameter& constParameter) {
    constParameterName = constParameter.name;
}

void MyVisitor::CloseConstParameter(ConstParameter& constParameter) {
    constParameterName = 0;
}

void MyVisitor::OpenVarParameter(VarParameter& varParameter) {
    varParameterName = varParameter.name;
}

void MyVisitor::CloseVarParameter(VarParameter& varParameter) {
    varParameterName = 0;
}

void MyVisitor::OpenThrowsClause(ThrowsClause& throwsClause) {
}

void MyVisitor::CloseThrowsClause(ThrowsClause& throwsClause) {
}

void MyVisitor::OpenEnumDeclaration(EnumDeclaration& enumDeclaration) {
    enumDeclarationName = enumDeclaration.name;
}

void MyVisitor::CloseEnumDeclaration(EnumDeclaration& enumDeclaration) {
    enumDeclarationName = 0;
}

void MyVisitor::OpenEnumMember(EnumMember& enumMember) {
}

void MyVisitor::CloseEnumMember(EnumMember& enumMember) {
}

void MyVisitor::OpenTupleType(TupleType& tupleType) {
}

void MyVisitor::CloseTupleType(TupleType& tupleType) {
}

void MyVisitor::OpenAdditionalType(AdditionalType& additionalType) {
}

void MyVisitor::CloseAdditionalType(AdditionalType& additionalType) {
}

void MyVisitor::VisitEnumCase(EnumCase& enumCase) {
}

void MyVisitor::OpenAdditionalCase(AdditionalCase& additionalCase) {
}

void MyVisitor::CloseAdditionalCase(AdditionalCase& additionalCase) {
}

void MyVisitor::OpenClassDeclaration(ClassDeclaration& classDeclaration) {
    classDeclarationName = classDeclaration.name;
}

void MyVisitor::CloseClassDeclaration(ClassDeclaration& classDeclaration) {
    classDeclarationName = 0;
}

void MyVisitor::OpenGenericArgumentClause(GenericArgumentClause& genericArgumentClause) {
}

void MyVisitor::CloseGenericArgumentClause(GenericArgumentClause& genericArgumentClause) {
}

void MyVisitor::VisitGenericParameter(GenericParameter& genericParameter) {
}

void MyVisitor::OpenClassMember(ClassMember& classMember) {
}

void MyVisitor::CloseClassMember(ClassMember& classMember) {
}

void MyVisitor::OpenCodeBlock(CodeBlock& codeBlock) {
}

void MyVisitor::CloseCodeBlock(CodeBlock& codeBlock) {
}

void MyVisitor::OpenSimpleExpression(SimpleExpression& simpleExpression) {
}

void MyVisitor::CloseSimpleExpression(SimpleExpression& simpleExpression) {
}

void MyVisitor::OpenPrefixExpression(PrefixExpression& prefixExpression) {
}

void MyVisitor::ClosePrefixExpression(PrefixExpression& prefixExpression) {
}

void MyVisitor::OpenPostfixExpression(PostfixExpression& postfixExpression) {
}

void MyVisitor::ClosePostfixExpression(PostfixExpression& postfixExpression) {
}

void MyVisitor::OpenBinaryOperation(BinaryOperation& binaryOperation) {
}

void MyVisitor::CloseBinaryOperation(BinaryOperation& binaryOperation) {
}

void MyVisitor::OpenAssignment(Assignment& assignment) {
}

void MyVisitor::CloseAssignment(Assignment& assignment) {
}

void MyVisitor::OpenTypeQuery(TypeQuery& typeQuery) {
}

void MyVisitor::CloseTypeQuery(TypeQuery& typeQuery) {
}

void MyVisitor::OpenTypeCast(TypeCast& typeCast) {
}

void MyVisitor::CloseTypeCast(TypeCast& typeCast) {
}

void MyVisitor::OpenCatchClause(CatchClause& catchClause) {
}

void MyVisitor::CloseCatchClause(CatchClause& catchClause) {
}

void MyVisitor::OpenWildCardCatchPattern(WildCardCatchPattern& wildCardCatchPattern) {
}

void MyVisitor::CloseWildCardCatchPattern(WildCardCatchPattern& wildCardCatchPattern) {
}

void MyVisitor::OpenPathItemCatchPattern(PathItemCatchPattern& pathItemCatchPattern) {
}

void MyVisitor::ClosePathItemCatchPattern(PathItemCatchPattern& pathItemCatchPattern) {
}

void MyVisitor::VisitOperatorPostfix(OperatorPostfix& operatorPostfix) {
}

void MyVisitor::OpenFunctionCall(FunctionCall& functionCall) {
}

void MyVisitor::CloseFunctionCall(FunctionCall& functionCall) {
}

void MyVisitor::OpenExplicitMemberExpression(ExplicitMemberExpression& explicitMemberExpression) {
}

void MyVisitor::CloseExplicitMemberExpression(ExplicitMemberExpression& explicitMemberExpression) {
}

void MyVisitor::OpenSubscript(Subscript& subscript) {
}

void MyVisitor::CloseSubscript(Subscript& subscript) {
}

void MyVisitor::OpenExpressionElement(ExpressionElement& expressionElement) {
}

void MyVisitor::CloseExpressionElement(ExpressionElement& expressionElement) {
}

void MyVisitor::OpenNamedMemberPostfix(NamedMemberPostfix& namedMemberPostfix) {
}

void MyVisitor::CloseNamedMemberPostfix(NamedMemberPostfix& namedMemberPostfix) {
}

void MyVisitor::OpenParenthesizedExpression(ParenthesizedExpression& parenthesizedExpression) {
}

void MyVisitor::CloseParenthesizedExpression(ParenthesizedExpression& parenthesizedExpression) {
}

void MyVisitor::VisitLiteralExpression(LiteralExpression& literalExpression) {
}

void MyVisitor::VisitIdentifierExpression(IdentifierExpression& identifierExpression) {
}

void MyVisitor::OpenIfExpression(IfExpression& ifExpression) {
}

void MyVisitor::CloseIfExpression(IfExpression& ifExpression) {
}

void MyVisitor::OpenElseClause(ElseClause& elseClause) {
}

void MyVisitor::CloseElseClause(ElseClause& elseClause) {
}

void MyVisitor::OpenSwitchExpression(SwitchExpression& switchExpression) {
}

void MyVisitor::CloseSwitchExpression(SwitchExpression& switchExpression) {
}

void MyVisitor::OpenCurliedSwitchBody(CurliedSwitchBody& curliedSwitchBody) {
}

void MyVisitor::CloseCurliedSwitchBody(CurliedSwitchBody& curliedSwitchBody) {
}

void MyVisitor::OpenNakedSwitchBody(NakedSwitchBody& nakedSwitchBody) {
}

void MyVisitor::CloseNakedSwitchBody(NakedSwitchBody& nakedSwitchBody) {
}

void MyVisitor::OpenSwitchCase(SwitchCase& switchCase) {
}

void MyVisitor::CloseSwitchCase(SwitchCase& switchCase) {
}

void MyVisitor::OpenItemCaseLabel(ItemCaseLabel& itemCaseLabel) {
}

void MyVisitor::CloseItemCaseLabel(ItemCaseLabel& itemCaseLabel) {
}

void MyVisitor::OpenCaseItem(CaseItem& caseItem) {
}

void MyVisitor::CloseCaseItem(CaseItem& caseItem) {
}

void MyVisitor::OpenForExpression(ForExpression& forExpression) {
}

void MyVisitor::CloseForExpression(ForExpression& forExpression) {
}

void MyVisitor::OpenForEach(ForEach& forEach) {
}

void MyVisitor::CloseForEach(ForEach& forEach) {
}

void MyVisitor::OpenPlainFor(PlainFor& plainFor) {
}

void MyVisitor::ClosePlainFor(PlainFor& plainFor) {
}

void MyVisitor::OpenReturnExpression(ReturnExpression& returnExpression) {
}

void MyVisitor::CloseReturnExpression(ReturnExpression& returnExpression) {
}

void MyVisitor::OpenThrowExpression(ThrowExpression& throwExpression) {
}

void MyVisitor::CloseThrowExpression(ThrowExpression& throwExpression) {
}

void MyVisitor::OpenBreakExpression(BreakExpression& breakExpression) {
}

void MyVisitor::CloseBreakExpression(BreakExpression& breakExpression) {
}

void MyVisitor::VisitWildcardPattern(WildcardPattern& wildcardPattern) {
}

void MyVisitor::OpenIdentifierPattern(IdentifierPattern& identifierPattern) {
    identifierPatternIdentifier = identifierPattern.identifier;
}

void MyVisitor::CloseIdentifierPattern(IdentifierPattern& identifierPattern) {
    identifierPatternIdentifier = 0;
}

void MyVisitor::OpenTuplePattern(TuplePattern& tuplePattern) {
}

void MyVisitor::CloseTuplePattern(TuplePattern& tuplePattern) {
}

void MyVisitor::OpenTuplePatternElement(TuplePatternElement& tuplePatternElement) {
}

void MyVisitor::CloseTuplePatternElement(TuplePatternElement& tuplePatternElement) {
}

void MyVisitor::OpenExpressionPattern(ExpressionPattern& expressionPattern) {
}

void MyVisitor::CloseExpressionPattern(ExpressionPattern& expressionPattern) {
}

void MyVisitor::VisitDefaultCaseLabel(DefaultCaseLabel& defaultCaseLabel) {
}

void MyVisitor::OpenBlockCaseContent(BlockCaseContent& blockCaseContent) {
}

void MyVisitor::CloseBlockCaseContent(BlockCaseContent& blockCaseContent) {
}

void MyVisitor::VisitEmptyCaseContent(EmptyCaseContent& emptyCaseContent) {
}

void MyVisitor::OpenInitializerCall(InitializerCall& initializerCall) {
}

void MyVisitor::CloseInitializerCall(InitializerCall& initializerCall) {
}

void MyVisitor::OpenThisDot(ThisDot& thisDot) {
}

void MyVisitor::CloseThisDot(ThisDot& thisDot) {
}

void MyVisitor::OpenThisSubscript(ThisSubscript& thisSubscript) {
}

void MyVisitor::CloseThisSubscript(ThisSubscript& thisSubscript) {
}

void MyVisitor::VisitThisWord(ThisWord& thisWord) {
}

void MyVisitor::VisitThisInit(ThisInit& thisInit) {
}

void MyVisitor::VisitThisMember(ThisMember& thisMember) {
}

void MyVisitor::OpenSuperDot(SuperDot& superDot) {
}

void MyVisitor::CloseSuperDot(SuperDot& superDot) {
}

void MyVisitor::OpenSuperSubscript(SuperSubscript& superSubscript) {
}

void MyVisitor::CloseSuperSubscript(SuperSubscript& superSubscript) {
}

void MyVisitor::VisitSuperInit(SuperInit& superInit) {
}

void MyVisitor::VisitSuperMember(SuperMember& superMember) {
}

void MyVisitor::OpenTypeAnnotation(TypeAnnotation& typeAnnotation) {
}

void MyVisitor::CloseTypeAnnotation(TypeAnnotation& typeAnnotation) {
}

void MyVisitor::OpenTypeIdentifier(TypeIdentifier& typeIdentifier) {
    typeIdentifierName = typeIdentifier.name;
}

void MyVisitor::CloseTypeIdentifier(TypeIdentifier& typeIdentifier) {
    typeIdentifierName = 0;
}

void MyVisitor::OpenSubtypeIdentifier(SubtypeIdentifier& subtypeIdentifier) {
}

void MyVisitor::CloseSubtypeIdentifier(SubtypeIdentifier& subtypeIdentifier) {
}

void MyVisitor::OpenArrayType(ArrayType& arrayType) {
}

void MyVisitor::CloseArrayType(ArrayType& arrayType) {
}

void MyVisitor::VisitOptionalType(OptionalType& optionalType) {
}

void MyVisitor::OpenTypeInheritanceClause(TypeInheritanceClause& typeInheritanceClause) {
}

void MyVisitor::CloseTypeInheritanceClause(TypeInheritanceClause& typeInheritanceClause) {
}

void MyVisitor::OpenInheritance(Inheritance& inheritance) {
}

void MyVisitor::CloseInheritance(Inheritance& inheritance) {
}

}
