#include "scalyc.h"
using namespace scaly;
namespace scalyc {


bool MyVisitor::OpenProgram(Program& program) {
    programName = program.name;
    programDirectory = program.directory;
    return true;
}

void MyVisitor::CloseProgram(Program& program) {
    programName = 0;
    programDirectory = 0;
}

bool MyVisitor::OpenCompilationUnit(CompilationUnit& compilationUnit) {
    return true;
}

void MyVisitor::CloseCompilationUnit(CompilationUnit& compilationUnit) {
}

bool MyVisitor::OpenStatementWithSemicolon(StatementWithSemicolon& statementWithSemicolon) {
    return true;
}

void MyVisitor::CloseStatementWithSemicolon(StatementWithSemicolon& statementWithSemicolon) {
}

bool MyVisitor::OpenUseDeclaration(UseDeclaration& useDeclaration) {
    return true;
}

void MyVisitor::CloseUseDeclaration(UseDeclaration& useDeclaration) {
}

bool MyVisitor::OpenPathIdentifier(PathIdentifier& pathIdentifier) {
    return true;
}

void MyVisitor::ClosePathIdentifier(PathIdentifier& pathIdentifier) {
}

void MyVisitor::VisitPathItem(PathItem& pathItem) {
}

bool MyVisitor::OpenInitializer(Initializer& initializer) {
    return true;
}

void MyVisitor::CloseInitializer(Initializer& initializer) {
}

bool MyVisitor::OpenConstantDeclaration(ConstantDeclaration& constantDeclaration) {
    return true;
}

void MyVisitor::CloseConstantDeclaration(ConstantDeclaration& constantDeclaration) {
}

bool MyVisitor::OpenVariableDeclaration(VariableDeclaration& variableDeclaration) {
    return true;
}

void MyVisitor::CloseVariableDeclaration(VariableDeclaration& variableDeclaration) {
}

bool MyVisitor::OpenBindingInitializer(BindingInitializer& bindingInitializer) {
    return true;
}

void MyVisitor::CloseBindingInitializer(BindingInitializer& bindingInitializer) {
}

bool MyVisitor::OpenPatternInitializer(PatternInitializer& patternInitializer) {
    return true;
}

void MyVisitor::ClosePatternInitializer(PatternInitializer& patternInitializer) {
}

bool MyVisitor::OpenAdditionalInitializer(AdditionalInitializer& additionalInitializer) {
    return true;
}

void MyVisitor::CloseAdditionalInitializer(AdditionalInitializer& additionalInitializer) {
}

bool MyVisitor::OpenFunctionDeclaration(FunctionDeclaration& functionDeclaration) {
    return true;
}

void MyVisitor::CloseFunctionDeclaration(FunctionDeclaration& functionDeclaration) {
}

bool MyVisitor::OpenInitializerDeclaration(InitializerDeclaration& initializerDeclaration) {
    return true;
}

void MyVisitor::CloseInitializerDeclaration(InitializerDeclaration& initializerDeclaration) {
}

void MyVisitor::VisitOverride(Override& override) {
}

void MyVisitor::VisitStaticWord(StaticWord& staticWord) {
}

void MyVisitor::VisitIdentifierFunction(IdentifierFunction& identifierFunction) {
}

bool MyVisitor::OpenFunctionSignature(FunctionSignature& functionSignature) {
    return true;
}

void MyVisitor::CloseFunctionSignature(FunctionSignature& functionSignature) {
}

bool MyVisitor::OpenFunctionResult(FunctionResult& functionResult) {
    return true;
}

void MyVisitor::CloseFunctionResult(FunctionResult& functionResult) {
}

bool MyVisitor::OpenParameterClause(ParameterClause& parameterClause) {
    return true;
}

void MyVisitor::CloseParameterClause(ParameterClause& parameterClause) {
}

bool MyVisitor::OpenConstParameter(ConstParameter& constParameter) {
    constParameterName = constParameter.name;
    return true;
}

void MyVisitor::CloseConstParameter(ConstParameter& constParameter) {
    constParameterName = 0;
}

bool MyVisitor::OpenVarParameter(VarParameter& varParameter) {
    varParameterName = varParameter.name;
    return true;
}

void MyVisitor::CloseVarParameter(VarParameter& varParameter) {
    varParameterName = 0;
}

bool MyVisitor::OpenThrowsClause(ThrowsClause& throwsClause) {
    return true;
}

void MyVisitor::CloseThrowsClause(ThrowsClause& throwsClause) {
}

bool MyVisitor::OpenEnumDeclaration(EnumDeclaration& enumDeclaration) {
    enumDeclarationName = enumDeclaration.name;
    return true;
}

void MyVisitor::CloseEnumDeclaration(EnumDeclaration& enumDeclaration) {
    enumDeclarationName = 0;
}

bool MyVisitor::OpenEnumMember(EnumMember& enumMember) {
    return true;
}

void MyVisitor::CloseEnumMember(EnumMember& enumMember) {
}

bool MyVisitor::OpenTupleType(TupleType& tupleType) {
    return true;
}

void MyVisitor::CloseTupleType(TupleType& tupleType) {
}

bool MyVisitor::OpenAdditionalType(AdditionalType& additionalType) {
    return true;
}

void MyVisitor::CloseAdditionalType(AdditionalType& additionalType) {
}

void MyVisitor::VisitEnumCase(EnumCase& enumCase) {
}

bool MyVisitor::OpenAdditionalCase(AdditionalCase& additionalCase) {
    return true;
}

void MyVisitor::CloseAdditionalCase(AdditionalCase& additionalCase) {
}

bool MyVisitor::OpenClassDeclaration(ClassDeclaration& classDeclaration) {
    classDeclarationName = classDeclaration.name;
    return true;
}

void MyVisitor::CloseClassDeclaration(ClassDeclaration& classDeclaration) {
    classDeclarationName = 0;
}

bool MyVisitor::OpenGenericArgumentClause(GenericArgumentClause& genericArgumentClause) {
    return true;
}

void MyVisitor::CloseGenericArgumentClause(GenericArgumentClause& genericArgumentClause) {
}

void MyVisitor::VisitGenericParameter(GenericParameter& genericParameter) {
}

bool MyVisitor::OpenClassMember(ClassMember& classMember) {
    return true;
}

void MyVisitor::CloseClassMember(ClassMember& classMember) {
}

bool MyVisitor::OpenCodeBlock(CodeBlock& codeBlock) {
    return true;
}

void MyVisitor::CloseCodeBlock(CodeBlock& codeBlock) {
}

bool MyVisitor::OpenSimpleExpression(SimpleExpression& simpleExpression) {
    return true;
}

void MyVisitor::CloseSimpleExpression(SimpleExpression& simpleExpression) {
}

bool MyVisitor::OpenPrefixExpression(PrefixExpression& prefixExpression) {
    return true;
}

void MyVisitor::ClosePrefixExpression(PrefixExpression& prefixExpression) {
}

bool MyVisitor::OpenPostfixExpression(PostfixExpression& postfixExpression) {
    return true;
}

void MyVisitor::ClosePostfixExpression(PostfixExpression& postfixExpression) {
}

bool MyVisitor::OpenBinaryOperation(BinaryOperation& binaryOperation) {
    return true;
}

void MyVisitor::CloseBinaryOperation(BinaryOperation& binaryOperation) {
}

bool MyVisitor::OpenAssignment(Assignment& assignment) {
    return true;
}

void MyVisitor::CloseAssignment(Assignment& assignment) {
}

bool MyVisitor::OpenTypeQuery(TypeQuery& typeQuery) {
    return true;
}

void MyVisitor::CloseTypeQuery(TypeQuery& typeQuery) {
}

bool MyVisitor::OpenTypeCast(TypeCast& typeCast) {
    return true;
}

void MyVisitor::CloseTypeCast(TypeCast& typeCast) {
}

bool MyVisitor::OpenCatchClause(CatchClause& catchClause) {
    return true;
}

void MyVisitor::CloseCatchClause(CatchClause& catchClause) {
}

bool MyVisitor::OpenWildCardCatchPattern(WildCardCatchPattern& wildCardCatchPattern) {
    return true;
}

void MyVisitor::CloseWildCardCatchPattern(WildCardCatchPattern& wildCardCatchPattern) {
}

bool MyVisitor::OpenPathItemCatchPattern(PathItemCatchPattern& pathItemCatchPattern) {
    return true;
}

void MyVisitor::ClosePathItemCatchPattern(PathItemCatchPattern& pathItemCatchPattern) {
}

void MyVisitor::VisitOperatorPostfix(OperatorPostfix& operatorPostfix) {
}

bool MyVisitor::OpenFunctionCall(FunctionCall& functionCall) {
    return true;
}

void MyVisitor::CloseFunctionCall(FunctionCall& functionCall) {
}

bool MyVisitor::OpenExplicitMemberExpression(ExplicitMemberExpression& explicitMemberExpression) {
    return true;
}

void MyVisitor::CloseExplicitMemberExpression(ExplicitMemberExpression& explicitMemberExpression) {
}

bool MyVisitor::OpenSubscript(Subscript& subscript) {
    return true;
}

void MyVisitor::CloseSubscript(Subscript& subscript) {
}

bool MyVisitor::OpenExpressionElement(ExpressionElement& expressionElement) {
    return true;
}

void MyVisitor::CloseExpressionElement(ExpressionElement& expressionElement) {
}

bool MyVisitor::OpenNamedMemberPostfix(NamedMemberPostfix& namedMemberPostfix) {
    return true;
}

void MyVisitor::CloseNamedMemberPostfix(NamedMemberPostfix& namedMemberPostfix) {
}

bool MyVisitor::OpenParenthesizedExpression(ParenthesizedExpression& parenthesizedExpression) {
    return true;
}

void MyVisitor::CloseParenthesizedExpression(ParenthesizedExpression& parenthesizedExpression) {
}

void MyVisitor::VisitLiteralExpression(LiteralExpression& literalExpression) {
}

void MyVisitor::VisitIdentifierExpression(IdentifierExpression& identifierExpression) {
}

bool MyVisitor::OpenIfExpression(IfExpression& ifExpression) {
    return true;
}

void MyVisitor::CloseIfExpression(IfExpression& ifExpression) {
}

bool MyVisitor::OpenElseClause(ElseClause& elseClause) {
    return true;
}

void MyVisitor::CloseElseClause(ElseClause& elseClause) {
}

bool MyVisitor::OpenSwitchExpression(SwitchExpression& switchExpression) {
    return true;
}

void MyVisitor::CloseSwitchExpression(SwitchExpression& switchExpression) {
}

bool MyVisitor::OpenCurliedSwitchBody(CurliedSwitchBody& curliedSwitchBody) {
    return true;
}

void MyVisitor::CloseCurliedSwitchBody(CurliedSwitchBody& curliedSwitchBody) {
}

bool MyVisitor::OpenNakedSwitchBody(NakedSwitchBody& nakedSwitchBody) {
    return true;
}

void MyVisitor::CloseNakedSwitchBody(NakedSwitchBody& nakedSwitchBody) {
}

bool MyVisitor::OpenSwitchCase(SwitchCase& switchCase) {
    return true;
}

void MyVisitor::CloseSwitchCase(SwitchCase& switchCase) {
}

bool MyVisitor::OpenItemCaseLabel(ItemCaseLabel& itemCaseLabel) {
    return true;
}

void MyVisitor::CloseItemCaseLabel(ItemCaseLabel& itemCaseLabel) {
}

bool MyVisitor::OpenCaseItem(CaseItem& caseItem) {
    return true;
}

void MyVisitor::CloseCaseItem(CaseItem& caseItem) {
}

bool MyVisitor::OpenForExpression(ForExpression& forExpression) {
    return true;
}

void MyVisitor::CloseForExpression(ForExpression& forExpression) {
}

bool MyVisitor::OpenForEach(ForEach& forEach) {
    return true;
}

void MyVisitor::CloseForEach(ForEach& forEach) {
}

bool MyVisitor::OpenPlainFor(PlainFor& plainFor) {
    return true;
}

void MyVisitor::ClosePlainFor(PlainFor& plainFor) {
}

bool MyVisitor::OpenReturnExpression(ReturnExpression& returnExpression) {
    return true;
}

void MyVisitor::CloseReturnExpression(ReturnExpression& returnExpression) {
}

bool MyVisitor::OpenThrowExpression(ThrowExpression& throwExpression) {
    return true;
}

void MyVisitor::CloseThrowExpression(ThrowExpression& throwExpression) {
}

bool MyVisitor::OpenBreakExpression(BreakExpression& breakExpression) {
    return true;
}

void MyVisitor::CloseBreakExpression(BreakExpression& breakExpression) {
}

void MyVisitor::VisitWildcardPattern(WildcardPattern& wildcardPattern) {
}

bool MyVisitor::OpenIdentifierPattern(IdentifierPattern& identifierPattern) {
    identifierPatternIdentifier = identifierPattern.identifier;
    return true;
}

void MyVisitor::CloseIdentifierPattern(IdentifierPattern& identifierPattern) {
    identifierPatternIdentifier = 0;
}

bool MyVisitor::OpenTuplePattern(TuplePattern& tuplePattern) {
    return true;
}

void MyVisitor::CloseTuplePattern(TuplePattern& tuplePattern) {
}

bool MyVisitor::OpenTuplePatternElement(TuplePatternElement& tuplePatternElement) {
    return true;
}

void MyVisitor::CloseTuplePatternElement(TuplePatternElement& tuplePatternElement) {
}

bool MyVisitor::OpenExpressionPattern(ExpressionPattern& expressionPattern) {
    return true;
}

void MyVisitor::CloseExpressionPattern(ExpressionPattern& expressionPattern) {
}

void MyVisitor::VisitDefaultCaseLabel(DefaultCaseLabel& defaultCaseLabel) {
}

bool MyVisitor::OpenBlockCaseContent(BlockCaseContent& blockCaseContent) {
    return true;
}

void MyVisitor::CloseBlockCaseContent(BlockCaseContent& blockCaseContent) {
}

void MyVisitor::VisitEmptyCaseContent(EmptyCaseContent& emptyCaseContent) {
}

bool MyVisitor::OpenInitializerCall(InitializerCall& initializerCall) {
    return true;
}

void MyVisitor::CloseInitializerCall(InitializerCall& initializerCall) {
}

bool MyVisitor::OpenThisDot(ThisDot& thisDot) {
    return true;
}

void MyVisitor::CloseThisDot(ThisDot& thisDot) {
}

bool MyVisitor::OpenThisSubscript(ThisSubscript& thisSubscript) {
    return true;
}

void MyVisitor::CloseThisSubscript(ThisSubscript& thisSubscript) {
}

void MyVisitor::VisitThisWord(ThisWord& thisWord) {
}

void MyVisitor::VisitThisInit(ThisInit& thisInit) {
}

void MyVisitor::VisitThisMember(ThisMember& thisMember) {
}

bool MyVisitor::OpenSuperDot(SuperDot& superDot) {
    return true;
}

void MyVisitor::CloseSuperDot(SuperDot& superDot) {
}

bool MyVisitor::OpenSuperSubscript(SuperSubscript& superSubscript) {
    return true;
}

void MyVisitor::CloseSuperSubscript(SuperSubscript& superSubscript) {
}

void MyVisitor::VisitSuperInit(SuperInit& superInit) {
}

void MyVisitor::VisitSuperMember(SuperMember& superMember) {
}

bool MyVisitor::OpenTypeAnnotation(TypeAnnotation& typeAnnotation) {
    return true;
}

void MyVisitor::CloseTypeAnnotation(TypeAnnotation& typeAnnotation) {
}

bool MyVisitor::OpenTypeIdentifier(TypeIdentifier& typeIdentifier) {
    typeIdentifierName = typeIdentifier.name;
    return true;
}

void MyVisitor::CloseTypeIdentifier(TypeIdentifier& typeIdentifier) {
    typeIdentifierName = 0;
}

bool MyVisitor::OpenSubtypeIdentifier(SubtypeIdentifier& subtypeIdentifier) {
    return true;
}

void MyVisitor::CloseSubtypeIdentifier(SubtypeIdentifier& subtypeIdentifier) {
}

bool MyVisitor::OpenArrayType(ArrayType& arrayType) {
    return true;
}

void MyVisitor::CloseArrayType(ArrayType& arrayType) {
}

void MyVisitor::VisitOptionalType(OptionalType& optionalType) {
}

bool MyVisitor::OpenTypeInheritanceClause(TypeInheritanceClause& typeInheritanceClause) {
    return true;
}

void MyVisitor::CloseTypeInheritanceClause(TypeInheritanceClause& typeInheritanceClause) {
}

bool MyVisitor::OpenInheritance(Inheritance& inheritance) {
    return true;
}

void MyVisitor::CloseInheritance(Inheritance& inheritance) {
}

}
