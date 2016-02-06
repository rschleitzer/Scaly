#include "scalyc.h"
using namespace scaly;
namespace scalyc {

CppError* CppVisitor::Execute(Program& program) {
    program.Accept(*this);
    return cppError;
}

bool CppVisitor::OpenProgram(Program& program) {
    programName = program.name;
    programDirectory = program.directory;
    
    if (programDirectory == 0 || *programDirectory == "")
        programDirectory = new(this->getPage()) String(".");
        
    if (!Directory::exists(*programDirectory)) {
        _Region _region; _Page* _p = _region.get();
        if (Directory::create(_p, *programDirectory))
            return false;
    }
    
    {
        _Region _region; _Page* _p = _region.get();
        String& outputFilePath = *new(_p) String(*programDirectory);
        outputFilePath += "/";
        outputFilePath += *programName;
        
        // Build and write the project file
        {
            _Region _region; _Page* _p = _region.get();
            String& projectFile = *new(_p) String();
            buildProjectFileString(projectFile, program);
            {
                _Region _region; _Page* _p = _region.get();
                String& projectFilePath = *new(_p) String(outputFilePath) + ".project";
                if (File::writeFromString(_p, projectFilePath , projectFile))
                    return false;
            }
        }
        
        // Build and write the main header file
        {
            _Region _region; _Page* _p = _region.get();
            String& headerFile = *new(_p) String();
            buildMainHeaderFileString(headerFile, program);
            {
                _Region _region; _Page* _p = _region.get();
                String& headerFilePath = *new(_p) String(outputFilePath) + ".h";
                if (File::writeFromString(_p, headerFilePath, headerFile))
                    return false;
            }
        }
    }
    
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

void CppVisitor::buildProjectFileString(String& projectFile, Program& program) {
    projectFile += "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
    projectFile += "<CodeLite_Project Name=\"";
    projectFile +=  *programName;
    projectFile +=  "\" InternalType=\"Console\">\n  <Plugins>\n    <Plugin Name=\"qmake\">\n";
    projectFile +=  "      <![CDATA[00020001N0005Debug0000000000000001N0007Release000000000000]]>\n    </Plugin>\n";
    projectFile +=  "    <Plugin Name=\"CMakePlugin\">\n      <![CDATA[[{\n";
    projectFile +=  "  \"name\": \"Debug\",\n  \"enabled\": false,\n  \"buildDirectory\": \"build\",\n";
    projectFile +=  "  \"sourceDirectory\": \"$(ProjectPath)\",\n  \"generator\": \"\",\n  \"buildType\": \"\",\n";
    projectFile +=  "  \"arguments\": [],\n  \"parentProject\": \"\"\n";
    projectFile +=  " }, {\n";
    projectFile +=  "  \"name\": \"Release\",\n";
    projectFile +=  "  \"enabled\": false,\n";
    projectFile +=  "  \"buildDirectory\": \"build\",\n";
    projectFile +=  "  \"sourceDirectory\": \"$(ProjectPath)\",\n";
    projectFile +=  "  \"generator\": \"\",\n";
    projectFile +=  "  \"buildType\": \"\",\n";
    projectFile +=  "  \"arguments\": [],\n";
    projectFile +=  "  \"parentProject\": \"\"\n";
    projectFile +=  " }]]]>\n    </Plugin>\n  </Plugins>\n";
    projectFile +=  "  <Description/>\n  <Dependencies/>\n";
    projectFile +=  "  <VirtualDirectory Name=\"src\">\n    <File Name=\"main.cpp\"/>\n";
    size_t noOfCompilationUnits = program.compilationUnits->length();
    for (size_t i = 0; i < noOfCompilationUnits; i++) {
        _Region _region; _Page* _p = _region.get();
        projectFile +=  "    <File Name=\"";
        projectFile +=  *Path::getFileNameWithoutExtension(_p, *(*(*program.compilationUnits)[i])->fileName);
        projectFile +=  ".cpp\"/>\n";
    }
    projectFile +=  "  </VirtualDirectory>\n  <VirtualDirectory Name=\"include\">\n";
    for (size_t i = 0; i < noOfCompilationUnits; i++) {
        _Region _region; _Page* _p = _region.get();
        projectFile +=  "    <File Name=\"";
        projectFile +=  *Path::getFileNameWithoutExtension(_p, *(*(*program.compilationUnits)[i])->fileName);
        projectFile +=  ".h\"/>\n";
    }
    projectFile +=  "  </VirtualDirectory>\n  <Settings Type=\"Executable\">\n    <GlobalSettings>\n";
    projectFile +=  "      <Compiler Options=\"\" C_Options=\"\" Assembler=\"\">\n";
    projectFile +=  "        <IncludePath Value=\".\"/>\n      </Compiler>\n      <Linker Options=\"\">\n";
    projectFile +=  "        <LibraryPath Value=\".\"/>\n      </Linker>\n      <ResourceCompiler Options=\"\"/>\n";
    projectFile +=  "    </GlobalSettings>\n";
    projectFile +=  "    <Configuration Name=\"Debug\" CompilerType=\"GCC\" DebuggerType=\"GNU gdb debugger\"";
    projectFile +=  " Type=\"Executable\" BuildCmpWithGlobalSettings=\"append\" BuildLnkWithGlobalSettings=\"append\"";
    projectFile +=  " BuildResWithGlobalSettings=\"append\">\n";
    projectFile +=  "      <Compiler Options=\"-g;-O0;-std=c++11;-Wall\" C_Options=\"-g;-O0;-Wall\" Assembler=\"\"";
    projectFile +=  " Required=\"yes\" PreCompiledHeader=\"\" PCHInCommandLine=\"no\" PCHFlags=\"\" PCHFlagsPolicy=\"0\">\n";
    projectFile +=  "        <IncludePath Value=\".\"/>\n        <IncludePath Value=\"../scaly\"/>\n      </Compiler>\n";
    projectFile +=  "      <Linker Options=\"\" Required=\"yes\">\n        <LibraryPath Value=\"../Debug\"/>\n";
    projectFile +=  "        <Library Value=\"libscaly\"/>\n      </Linker>\n      <ResourceCompiler Options=\"\" Required=\"no\"/>\n";    
    projectFile +=  "      <General OutputFile=\"$(IntermediateDirectory)/$(ProjectName)\" IntermediateDirectory=\"../Debug\" ";
    projectFile +=  "Command=\"./$(ProjectName)\" CommandArguments=\"-o ";
    projectFile +=  *programName;
    projectFile +=  " -d output";
    for (size_t i = 0; i < noOfCompilationUnits; i++) {
        _Region _region; _Page* _p = _region.get();
        projectFile +=  " ../"; projectFile +=  *programName; projectFile += "/";
        projectFile +=  *Path::getFileNameWithoutExtension(_p, *(*(*program.compilationUnits)[i])->fileName);
        projectFile +=  ".scaly";
    }
    projectFile += "\" UseSeparateDebugArgs=\"no\" DebugArguments=\"\" WorkingDirectory=\"$(IntermediateDirectory)\"";
    projectFile += " PauseExecWhenProcTerminates=\"yes\" IsGUIProgram=\"no\" IsEnabled=\"yes\"/>\n";
    projectFile += "      <Environment EnvVarSetName=\"&lt;Use Defaults&gt;\" DbgSetName=\"&lt;Use Defaults&gt;\">\n";
    projectFile += "        <![CDATA[]]>\n      </Environment>\n";
    projectFile += "      <Debugger IsRemote=\"no\" RemoteHostName=\"\" RemoteHostPort=\"\" DebuggerPath=\"\" IsExtended=\"yes\">\n";
    projectFile += "        <DebuggerSearchPaths/>\n        <PostConnectCommands/>\n        <StartupCommands/>\n      </Debugger>\n";
    projectFile += "      <PreBuild/>\n      <PostBuild/>\n      <CustomBuild Enabled=\"no\">\n        <RebuildCommand/>\n";
    projectFile += "        <CleanCommand/>\n        <BuildCommand/>\n        <PreprocessFileCommand/>\n        <SingleFileCommand/>\n";
    projectFile += "        <MakefileGenerationCommand/>\n        <ThirdPartyToolName>None</ThirdPartyToolName>\n";
    projectFile += "        <WorkingDirectory/>\n      </CustomBuild>\n      <AdditionalRules>\n";
    projectFile += "        <CustomPostBuild/>\n        <CustomPreBuild/>\n      </AdditionalRules>\n";
    projectFile += "      <Completion EnableCpp11=\"no\" EnableCpp14=\"no\">\n        <ClangCmpFlagsC/>\n        <ClangCmpFlags/>\n";
    projectFile += "        <ClangPP/>\n        <SearchPaths/>\n      </Completion>\n";
    projectFile += "    </Configuration>\n";
    projectFile += "    <Configuration Name=\"Release\" CompilerType=\"GCC\" DebuggerType=\"GNU gdb debugger\" Type=\"Executable\"";
    projectFile += " BuildCmpWithGlobalSettings=\"append\" BuildLnkWithGlobalSettings=\"append\" BuildResWithGlobalSettings=\"append\">\n";
    projectFile += "      <Compiler Options=\"-O2;-Wall\" C_Options=\"-O2;-Wall\" Assembler=\"\" Required=\"yes\" PreCompiledHeader=\"\"";
    projectFile += " PCHInCommandLine=\"no\" PCHFlags=\"\" PCHFlagsPolicy=\"0\">\n";
    projectFile += "        <IncludePath Value=\".\"/>\n        <Preprocessor Value=\"NDEBUG\"/>\n";
    projectFile += "      </Compiler>\n      <Linker Options=\"\" Required=\"yes\">\n";
    projectFile += "        <LibraryPath Value=\"../Release\"/>\n        <Library Value=\"libscaly\"/>\n      </Linker>\n";
    projectFile += "      <ResourceCompiler Options=\"\" Required=\"no\"/>\n";
    projectFile += "      <General OutputFile=\"$(IntermediateDirectory)/$(ProjectName)\" IntermediateDirectory=\"../Release\"";
    projectFile += " Command=\"./$(ProjectName)\" CommandArguments=\"\" UseSeparateDebugArgs=\"no\" DebugArguments=\"\"";
    projectFile += " WorkingDirectory=\"$(IntermediateDirectory)\" PauseExecWhenProcTerminates=\"yes\" IsGUIProgram=\"no\" IsEnabled=\"yes\"/>\n";
    projectFile += "      <Environment EnvVarSetName=\"&lt;Use Defaults&gt;\" DbgSetName=\"&lt;Use Defaults&gt;\">\n";
    projectFile += "        <![CDATA[]]>\n      </Environment>\n";
    projectFile += "      <Debugger IsRemote=\"no\" RemoteHostName=\"\" RemoteHostPort=\"\" DebuggerPath=\"\" IsExtended=\"yes\">\n";
    projectFile += "        <DebuggerSearchPaths/>\n        <PostConnectCommands/>\n        <StartupCommands/>\n";
    projectFile += "      </Debugger>\n      <PreBuild/>\n      <PostBuild/>\n      <CustomBuild Enabled=\"no\">\n";
    projectFile += "        <RebuildCommand/>\n        <CleanCommand/>\n        <BuildCommand/>\n        <PreprocessFileCommand/>\n";
    projectFile += "        <SingleFileCommand/>\n        <MakefileGenerationCommand/>\n";
    projectFile += "        <ThirdPartyToolName>None</ThirdPartyToolName>\n        <WorkingDirectory/>\n";
    projectFile += "      </CustomBuild>\n      <AdditionalRules>\n        <CustomPostBuild/>\n";
    projectFile += "        <CustomPreBuild/>\n      </AdditionalRules>\n      <Completion EnableCpp11=\"no\" EnableCpp14=\"no\">\n";
    projectFile += "        <ClangCmpFlagsC/>\n        <ClangCmpFlags/>\n        <ClangPP/>\n";
    projectFile += "        <SearchPaths/>\n      </Completion>\n    </Configuration>\n  </Settings>\n</CodeLite_Project>\n";    
}

void CppVisitor::buildMainHeaderFileString(String& headerFile, Program& program) {
    headerFile += "#ifndef __scaly__scalyc__\n#define __scaly__scalyc__\n\n#include \"Scaly.h\"\n";
    size_t noOfCompilationUnits = program.compilationUnits->length();
    for (size_t i = 0; i < noOfCompilationUnits; i++) {
        _Region _region; _Page* _p = _region.get();
        headerFile +=  "#include \"";
        headerFile += *Path::getFileNameWithoutExtension(_p, *(*(*program.compilationUnits)[i])->fileName) + ".h\n";
    }
    headerFile += "\nusing namespace scaly;\nnamespace scalyc {\nint _main(Array<String>& arguments);\n}\n\n#endif // __scaly__scalyc__\n";
}

}
