#include "scalyc.h"
using namespace scaly;
namespace scalyc {


bool CppVisitor::OpenProgram(Program& program) {
    programName = program.name;
    programDirectory = program.directory;
    
    if (programDirectory == 0 || *programDirectory == "")
        programDirectory = new(this->getPage()) String(".");
        
//    if (!Directory::exists(programDirectory)) {
//        Directory.create(programDirectory);
//    }
    
    return true;
}

void CppVisitor::CloseProgram(Program& program) {
    programName = 0;
    programDirectory = 0;    
}


bool CppVisitor::OpenCompilationUnit(CompilationUnit& compilationUnit) {
    return true;
}

void CppVisitor::CloseCompilationUnit(CompilationUnit& compilationUnit) {
}

bool CppVisitor::OpenStatementWithSemicolon(StatementWithSemicolon& statementWithSemicolon) {
    return true;
}

void CppVisitor::CloseStatementWithSemicolon(StatementWithSemicolon& statementWithSemicolon) {
}

bool CppVisitor::OpenUseDeclaration(UseDeclaration& useDeclaration) {
    return true;
}

void CppVisitor::CloseUseDeclaration(UseDeclaration& useDeclaration) {
}

bool CppVisitor::OpenPathIdentifier(PathIdentifier& pathIdentifier) {
    return true;
}

void CppVisitor::ClosePathIdentifier(PathIdentifier& pathIdentifier) {
}

void CppVisitor::VisitPathItem(PathItem& pathItem) {
}

bool CppVisitor::OpenInitializer(Initializer& initializer) {
    return true;
}

void CppVisitor::CloseInitializer(Initializer& initializer) {
}

bool CppVisitor::OpenConstantDeclaration(ConstantDeclaration& constantDeclaration) {
    return true;
}

void CppVisitor::CloseConstantDeclaration(ConstantDeclaration& constantDeclaration) {
}

bool CppVisitor::OpenVariableDeclaration(VariableDeclaration& variableDeclaration) {
    return true;
}

void CppVisitor::CloseVariableDeclaration(VariableDeclaration& variableDeclaration) {
}

bool CppVisitor::OpenBindingInitializer(BindingInitializer& bindingInitializer) {
    return true;
}

void CppVisitor::CloseBindingInitializer(BindingInitializer& bindingInitializer) {
}

bool CppVisitor::OpenPatternInitializer(PatternInitializer& patternInitializer) {
    return true;
}

void CppVisitor::ClosePatternInitializer(PatternInitializer& patternInitializer) {
}

bool CppVisitor::OpenAdditionalInitializer(AdditionalInitializer& additionalInitializer) {
    return true;
}

void CppVisitor::CloseAdditionalInitializer(AdditionalInitializer& additionalInitializer) {
}

bool CppVisitor::OpenFunctionDeclaration(FunctionDeclaration& functionDeclaration) {
    return true;
}

void CppVisitor::CloseFunctionDeclaration(FunctionDeclaration& functionDeclaration) {
}

bool CppVisitor::OpenInitializerDeclaration(InitializerDeclaration& initializerDeclaration) {
    return true;
}

void CppVisitor::CloseInitializerDeclaration(InitializerDeclaration& initializerDeclaration) {
}

void CppVisitor::VisitOverride(Override& override) {
}

void CppVisitor::VisitStaticWord(StaticWord& staticWord) {
}

void CppVisitor::VisitIdentifierFunction(IdentifierFunction& identifierFunction) {
}

bool CppVisitor::OpenFunctionSignature(FunctionSignature& functionSignature) {
    return true;
}

void CppVisitor::CloseFunctionSignature(FunctionSignature& functionSignature) {
}

bool CppVisitor::OpenFunctionResult(FunctionResult& functionResult) {
    return true;
}

void CppVisitor::CloseFunctionResult(FunctionResult& functionResult) {
}

bool CppVisitor::OpenParameterClause(ParameterClause& parameterClause) {
    return true;
}

void CppVisitor::CloseParameterClause(ParameterClause& parameterClause) {
}

bool CppVisitor::OpenConstParameter(ConstParameter& constParameter) {
    constParameterName = constParameter.name;
    return true;
}

void CppVisitor::CloseConstParameter(ConstParameter& constParameter) {
    constParameterName = 0;
}

bool CppVisitor::OpenVarParameter(VarParameter& varParameter) {
    varParameterName = varParameter.name;
    return true;
}

void CppVisitor::CloseVarParameter(VarParameter& varParameter) {
    varParameterName = 0;
}

bool CppVisitor::OpenThrowsClause(ThrowsClause& throwsClause) {
    return true;
}

void CppVisitor::CloseThrowsClause(ThrowsClause& throwsClause) {
}

bool CppVisitor::OpenEnumDeclaration(EnumDeclaration& enumDeclaration) {
    enumDeclarationName = enumDeclaration.name;
    return true;
}

void CppVisitor::CloseEnumDeclaration(EnumDeclaration& enumDeclaration) {
    enumDeclarationName = 0;
}

bool CppVisitor::OpenEnumMember(EnumMember& enumMember) {
    return true;
}

void CppVisitor::CloseEnumMember(EnumMember& enumMember) {
}

bool CppVisitor::OpenTupleType(TupleType& tupleType) {
    return true;
}

void CppVisitor::CloseTupleType(TupleType& tupleType) {
}

bool CppVisitor::OpenAdditionalType(AdditionalType& additionalType) {
    return true;
}

void CppVisitor::CloseAdditionalType(AdditionalType& additionalType) {
}

void CppVisitor::VisitEnumCase(EnumCase& enumCase) {
}

bool CppVisitor::OpenAdditionalCase(AdditionalCase& additionalCase) {
    return true;
}

void CppVisitor::CloseAdditionalCase(AdditionalCase& additionalCase) {
}

bool CppVisitor::OpenClassDeclaration(ClassDeclaration& classDeclaration) {
    classDeclarationName = classDeclaration.name;
    return true;
}

void CppVisitor::CloseClassDeclaration(ClassDeclaration& classDeclaration) {
    classDeclarationName = 0;
}

bool CppVisitor::OpenGenericArgumentClause(GenericArgumentClause& genericArgumentClause) {
    return true;
}

void CppVisitor::CloseGenericArgumentClause(GenericArgumentClause& genericArgumentClause) {
}

void CppVisitor::VisitGenericParameter(GenericParameter& genericParameter) {
}

bool CppVisitor::OpenClassMember(ClassMember& classMember) {
    return true;
}

void CppVisitor::CloseClassMember(ClassMember& classMember) {
}

bool CppVisitor::OpenCodeBlock(CodeBlock& codeBlock) {
    return true;
}

void CppVisitor::CloseCodeBlock(CodeBlock& codeBlock) {
}

bool CppVisitor::OpenSimpleExpression(SimpleExpression& simpleExpression) {
    return true;
}

void CppVisitor::CloseSimpleExpression(SimpleExpression& simpleExpression) {
}

bool CppVisitor::OpenPrefixExpression(PrefixExpression& prefixExpression) {
    return true;
}

void CppVisitor::ClosePrefixExpression(PrefixExpression& prefixExpression) {
}

bool CppVisitor::OpenPostfixExpression(PostfixExpression& postfixExpression) {
    return true;
}

void CppVisitor::ClosePostfixExpression(PostfixExpression& postfixExpression) {
}

bool CppVisitor::OpenBinaryOperation(BinaryOperation& binaryOperation) {
    return true;
}

void CppVisitor::CloseBinaryOperation(BinaryOperation& binaryOperation) {
}

bool CppVisitor::OpenAssignment(Assignment& assignment) {
    return true;
}

void CppVisitor::CloseAssignment(Assignment& assignment) {
}

bool CppVisitor::OpenTypeQuery(TypeQuery& typeQuery) {
    return true;
}

void CppVisitor::CloseTypeQuery(TypeQuery& typeQuery) {
}

bool CppVisitor::OpenTypeCast(TypeCast& typeCast) {
    return true;
}

void CppVisitor::CloseTypeCast(TypeCast& typeCast) {
}

bool CppVisitor::OpenCatchClause(CatchClause& catchClause) {
    return true;
}

void CppVisitor::CloseCatchClause(CatchClause& catchClause) {
}

bool CppVisitor::OpenWildCardCatchPattern(WildCardCatchPattern& wildCardCatchPattern) {
    return true;
}

void CppVisitor::CloseWildCardCatchPattern(WildCardCatchPattern& wildCardCatchPattern) {
}

bool CppVisitor::OpenPathItemCatchPattern(PathItemCatchPattern& pathItemCatchPattern) {
    return true;
}

void CppVisitor::ClosePathItemCatchPattern(PathItemCatchPattern& pathItemCatchPattern) {
}

void CppVisitor::VisitOperatorPostfix(OperatorPostfix& operatorPostfix) {
}

bool CppVisitor::OpenFunctionCall(FunctionCall& functionCall) {
    return true;
}

void CppVisitor::CloseFunctionCall(FunctionCall& functionCall) {
}

bool CppVisitor::OpenExplicitMemberExpression(ExplicitMemberExpression& explicitMemberExpression) {
    return true;
}

void CppVisitor::CloseExplicitMemberExpression(ExplicitMemberExpression& explicitMemberExpression) {
}

bool CppVisitor::OpenSubscript(Subscript& subscript) {
    return true;
}

void CppVisitor::CloseSubscript(Subscript& subscript) {
}

bool CppVisitor::OpenExpressionElement(ExpressionElement& expressionElement) {
    return true;
}

void CppVisitor::CloseExpressionElement(ExpressionElement& expressionElement) {
}

bool CppVisitor::OpenNamedMemberPostfix(NamedMemberPostfix& namedMemberPostfix) {
    return true;
}

void CppVisitor::CloseNamedMemberPostfix(NamedMemberPostfix& namedMemberPostfix) {
}

bool CppVisitor::OpenParenthesizedExpression(ParenthesizedExpression& parenthesizedExpression) {
    return true;
}

void CppVisitor::CloseParenthesizedExpression(ParenthesizedExpression& parenthesizedExpression) {
}

void CppVisitor::VisitLiteralExpression(LiteralExpression& literalExpression) {
}

void CppVisitor::VisitIdentifierExpression(IdentifierExpression& identifierExpression) {
}

bool CppVisitor::OpenIfExpression(IfExpression& ifExpression) {
    return true;
}

void CppVisitor::CloseIfExpression(IfExpression& ifExpression) {
}

bool CppVisitor::OpenElseClause(ElseClause& elseClause) {
    return true;
}

void CppVisitor::CloseElseClause(ElseClause& elseClause) {
}

bool CppVisitor::OpenSwitchExpression(SwitchExpression& switchExpression) {
    return true;
}

void CppVisitor::CloseSwitchExpression(SwitchExpression& switchExpression) {
}

bool CppVisitor::OpenCurliedSwitchBody(CurliedSwitchBody& curliedSwitchBody) {
    return true;
}

void CppVisitor::CloseCurliedSwitchBody(CurliedSwitchBody& curliedSwitchBody) {
}

bool CppVisitor::OpenNakedSwitchBody(NakedSwitchBody& nakedSwitchBody) {
    return true;
}

void CppVisitor::CloseNakedSwitchBody(NakedSwitchBody& nakedSwitchBody) {
}

bool CppVisitor::OpenSwitchCase(SwitchCase& switchCase) {
    return true;
}

void CppVisitor::CloseSwitchCase(SwitchCase& switchCase) {
}

bool CppVisitor::OpenItemCaseLabel(ItemCaseLabel& itemCaseLabel) {
    return true;
}

void CppVisitor::CloseItemCaseLabel(ItemCaseLabel& itemCaseLabel) {
}

bool CppVisitor::OpenCaseItem(CaseItem& caseItem) {
    return true;
}

void CppVisitor::CloseCaseItem(CaseItem& caseItem) {
}

bool CppVisitor::OpenForExpression(ForExpression& forExpression) {
    return true;
}

void CppVisitor::CloseForExpression(ForExpression& forExpression) {
}

bool CppVisitor::OpenForEach(ForEach& forEach) {
    return true;
}

void CppVisitor::CloseForEach(ForEach& forEach) {
}

bool CppVisitor::OpenPlainFor(PlainFor& plainFor) {
    return true;
}

void CppVisitor::ClosePlainFor(PlainFor& plainFor) {
}

bool CppVisitor::OpenReturnExpression(ReturnExpression& returnExpression) {
    return true;
}

void CppVisitor::CloseReturnExpression(ReturnExpression& returnExpression) {
}

bool CppVisitor::OpenThrowExpression(ThrowExpression& throwExpression) {
    return true;
}

void CppVisitor::CloseThrowExpression(ThrowExpression& throwExpression) {
}

bool CppVisitor::OpenBreakExpression(BreakExpression& breakExpression) {
    return true;
}

void CppVisitor::CloseBreakExpression(BreakExpression& breakExpression) {
}

void CppVisitor::VisitWildcardPattern(WildcardPattern& wildcardPattern) {
}

bool CppVisitor::OpenIdentifierPattern(IdentifierPattern& identifierPattern) {
    identifierPatternIdentifier = identifierPattern.identifier;
    return true;
}

void CppVisitor::CloseIdentifierPattern(IdentifierPattern& identifierPattern) {
    identifierPatternIdentifier = 0;
}

bool CppVisitor::OpenTuplePattern(TuplePattern& tuplePattern) {
    return true;
}

void CppVisitor::CloseTuplePattern(TuplePattern& tuplePattern) {
}

bool CppVisitor::OpenTuplePatternElement(TuplePatternElement& tuplePatternElement) {
    return true;
}

void CppVisitor::CloseTuplePatternElement(TuplePatternElement& tuplePatternElement) {
}

bool CppVisitor::OpenExpressionPattern(ExpressionPattern& expressionPattern) {
    return true;
}

void CppVisitor::CloseExpressionPattern(ExpressionPattern& expressionPattern) {
}

void CppVisitor::VisitDefaultCaseLabel(DefaultCaseLabel& defaultCaseLabel) {
}

bool CppVisitor::OpenBlockCaseContent(BlockCaseContent& blockCaseContent) {
    return true;
}

void CppVisitor::CloseBlockCaseContent(BlockCaseContent& blockCaseContent) {
}

void CppVisitor::VisitEmptyCaseContent(EmptyCaseContent& emptyCaseContent) {
}

bool CppVisitor::OpenInitializerCall(InitializerCall& initializerCall) {
    return true;
}

void CppVisitor::CloseInitializerCall(InitializerCall& initializerCall) {
}

bool CppVisitor::OpenThisDot(ThisDot& thisDot) {
    return true;
}

void CppVisitor::CloseThisDot(ThisDot& thisDot) {
}

bool CppVisitor::OpenThisSubscript(ThisSubscript& thisSubscript) {
    return true;
}

void CppVisitor::CloseThisSubscript(ThisSubscript& thisSubscript) {
}

void CppVisitor::VisitThisWord(ThisWord& thisWord) {
}

void CppVisitor::VisitThisInit(ThisInit& thisInit) {
}

void CppVisitor::VisitThisMember(ThisMember& thisMember) {
}

bool CppVisitor::OpenSuperDot(SuperDot& superDot) {
    return true;
}

void CppVisitor::CloseSuperDot(SuperDot& superDot) {
}

bool CppVisitor::OpenSuperSubscript(SuperSubscript& superSubscript) {
    return true;
}

void CppVisitor::CloseSuperSubscript(SuperSubscript& superSubscript) {
}

void CppVisitor::VisitSuperInit(SuperInit& superInit) {
}

void CppVisitor::VisitSuperMember(SuperMember& superMember) {
}

bool CppVisitor::OpenTypeAnnotation(TypeAnnotation& typeAnnotation) {
    return true;
}

void CppVisitor::CloseTypeAnnotation(TypeAnnotation& typeAnnotation) {
}

bool CppVisitor::OpenTypeIdentifier(TypeIdentifier& typeIdentifier) {
    typeIdentifierName = typeIdentifier.name;
    return true;
}

void CppVisitor::CloseTypeIdentifier(TypeIdentifier& typeIdentifier) {
    typeIdentifierName = 0;
}

bool CppVisitor::OpenSubtypeIdentifier(SubtypeIdentifier& subtypeIdentifier) {
    return true;
}

void CppVisitor::CloseSubtypeIdentifier(SubtypeIdentifier& subtypeIdentifier) {
}

bool CppVisitor::OpenArrayType(ArrayType& arrayType) {
    return true;
}

void CppVisitor::CloseArrayType(ArrayType& arrayType) {
}

void CppVisitor::VisitOptionalType(OptionalType& optionalType) {
}

bool CppVisitor::OpenTypeInheritanceClause(TypeInheritanceClause& typeInheritanceClause) {
    return true;
}

void CppVisitor::CloseTypeInheritanceClause(TypeInheritanceClause& typeInheritanceClause) {
}

bool CppVisitor::OpenInheritance(Inheritance& inheritance) {
    return true;
}

void CppVisitor::CloseInheritance(Inheritance& inheritance) {
}

}
