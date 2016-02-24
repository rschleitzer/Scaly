#include "scalyc.h"
using namespace scaly;
namespace scalyc {

CppError* CppVisitor::execute(Program* program) {
    program->accept(*this);
    return cppError;
}

bool CppVisitor::openProgram(Program* program) {
    programName = program->name;
    programDirectory = program->directory;

    if (programDirectory == 0 || *programDirectory == "")
        programDirectory = &_LetString::create(this->getPage(), ".");

    if (!Directory::exists(*programDirectory)) {
        _Region _region; _Page* _p = _region.get();
        if (Directory::create(_p, *programDirectory))
            return false;
    }

    {
        _Region _region; _Page* _p = _region.get();
        _VarString& outputFilePath = *new(_p) _VarString(*programDirectory);
        outputFilePath += "/";
        outputFilePath += *programName;

        // Build and write the project file
        {
            _Region _region; _Page* _p = _region.get();
            _VarString* projectFile = new(_p) _VarString();
            buildProjectFileString(projectFile, program);
            {
                _Region _region; _Page* _p = _region.get();
                _VarString* projectFilePath = new(_p) _VarString(outputFilePath);
                (*projectFilePath) += ".project";
                if (File::writeFromString(_p, *projectFilePath , *projectFile))
                    return false;
            }
        }

        // Build and write the main header file
        {
            _Region _region; _Page* _p = _region.get();
            _VarString* headerFile = new(_p) _VarString(0, _pageSize);
            buildMainHeaderFileString(headerFile, program);
            {
                _Region _region; _Page* _p = _region.get();
                _VarString* headerFilePath = new(_p) _VarString(outputFilePath);
                (*headerFilePath) += ".h";
                if (File::writeFromString(_p, *headerFilePath, *headerFile))
                    return false;
            }
        }

        inherits = new(getPage()) _Array<Inherits>();
        classes = new(getPage()) _Array<_LetString>();
        collectInheritances(program);
    }

    return true;
}

void CppVisitor::collectInheritances(Program* program) {
    inherits = new(getPage()) _Array<Inherits>();
    _Vector<CompilationUnit>& compilationUnits = *program->compilationUnits;
    size_t _compilationUnits_length = compilationUnits.length();
    for (size_t _i = 0; _i < _compilationUnits_length; _i++) {
        collectInheritancesInCompilationUnit(*compilationUnits[_i]);}}

void CppVisitor::collectInheritancesInCompilationUnit(CompilationUnit* compilationUnit) {
    if (compilationUnit->statements) {
        _Vector<StatementWithSemicolon>& statements = *compilationUnit->statements;
        size_t _statements_length = statements.length();
        for (size_t _i = 0; _i < _statements_length; _i++) {
            StatementWithSemicolon& statementWithSemicolon = **statements[_i];
            if (statementWithSemicolon.statement) {
                if (statementWithSemicolon.statement->_isClassDeclaration()) {
                    ClassDeclaration& classDeclaration = *(ClassDeclaration*)statementWithSemicolon.statement;
                    classes->push(classDeclaration.name);
                    if (classDeclaration.typeInheritanceClause) {
                        TypeInheritanceClause& inheritanceClause = *classDeclaration.typeInheritanceClause;
                        _Vector<Inheritance>& inheritances = *inheritanceClause.inheritances;
                        size_t _inheritances_length = inheritances.length();
                        for (size_t _j = 0; _j < _inheritances_length; _j++) {
                            Inheritance& inheritance = **inheritances[_j];
                            registerInheritance(classDeclaration.name, inheritance.typeIdentifier->name);}}}}}}}

void CppVisitor::registerInheritance(_LetString* className, _LetString* baseName) {
    size_t _inherits_length = inherits->length();
    Inherits* inherit = 0;
    for (size_t _i = 0; _i < _inherits_length; _i++) {
        Inherits* inh = *(*inherits)[_i];
        if (*inh->name == *baseName) {
            inherit = inh;
        }
    }
    if (!inherit) {
        inherit = new(getPage()) Inherits(baseName);
        inherits->push(inherit);
    }
    inherit->inheritors->push(className);
}


void CppVisitor::closeProgram(Program* program) {
    programName = 0;
    programDirectory = 0;
}


bool CppVisitor::openCompilationUnit(CompilationUnit* compilationUnit) {
    moduleName = Path::getFileNameWithoutExtension(getPage(), *compilationUnit->fileName);
    headerIndentLevel = 0;
    sourceIndentLevel = 0;
    declaringClassMember = false;
    inParameterClause = false;

    // Build and write the header file
    if (*moduleName != *programName) {
        headerFile = new(getPage()) _VarString(0, _pageSize);
        (*headerFile) += "#ifndef __";
        (*headerFile) += *programName;
        (*headerFile) += "__";
        (*headerFile) += *moduleName;
        (*headerFile) += "__\n";
        (*headerFile) += "#define __";
        (*headerFile) += *programName;
        (*headerFile) += "__";
        (*headerFile) += *moduleName;
        (*headerFile) += "__\n#include \"";
        (*headerFile) += *programName;
        (*headerFile) += ".h\"\nusing namespace scaly;\nnamespace ";
        (*headerFile) += *programName;
        (*headerFile) += " {";
    }

    // Begin cpp file
    sourceFile = new(getPage()) _VarString(0, _pageSize);
    (*sourceFile) += "#include \"";
    (*sourceFile) += *programName;
    (*sourceFile) += ".h\"\nusing namespace scaly;\nnamespace ";
    (*sourceFile) += *programName;
    (*sourceFile) += " {\n\n";

    return true;
}

void CppVisitor::closeCompilationUnit(CompilationUnit* compilationUnit) {
    // Close and write cpp file
    _Region _region; _Page* _p = _region.get();

    _VarString& outputFilePath = *new(_p) _VarString(*programDirectory);
    outputFilePath += "/";
    outputFilePath += *Path::getFileNameWithoutExtension(_p, *compilationUnit->fileName);

    if (*moduleName != *programName) {
        (*headerFile) += "\n\n}\n#endif // __scalyc__";
        (*headerFile) += *moduleName;
        (*headerFile) += "__\n";
        _VarString& headerFilePath = *new(_p) _VarString(outputFilePath);
        headerFilePath += ".h";
        File::writeFromString(_p, headerFilePath , *headerFile);
    }

    (*sourceFile) += "\n}\n";
    _VarString& sourceFilePath = *new(_p) _VarString(outputFilePath);
    sourceFilePath += ".cpp";
    File::writeFromString(_p, sourceFilePath , *sourceFile);
}

bool CppVisitor::openStatementWithSemicolon(StatementWithSemicolon* statementWithSemicolon) {
    return true;
}

void CppVisitor::closeStatementWithSemicolon(StatementWithSemicolon* statementWithSemicolon) {
    (*sourceFile) += ";\n";
}

bool CppVisitor::openUseDeclaration(UseDeclaration* useDeclaration) {
    return true;
}

void CppVisitor::closeUseDeclaration(UseDeclaration* useDeclaration) {
}

bool CppVisitor::openPathIdentifier(PathIdentifier* pathIdentifier) {
    return true;
}

void CppVisitor::closePathIdentifier(PathIdentifier* pathIdentifier) {
}

void CppVisitor::visitPathItem(PathItem* pathItem) {
}

bool CppVisitor::openInitializer(Initializer* initializer) {
    return true;
}

void CppVisitor::closeInitializer(Initializer* initializer) {
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

bool CppVisitor::openBindingInitializer(BindingInitializer* bindingInitializer) {
    firstBindingInitializer = true;
    return true;
}

void CppVisitor::closeBindingInitializer(BindingInitializer* bindingInitializer) {
}

bool CppVisitor::openPatternInitializer(PatternInitializer* patternInitializer) {
    if (!firstBindingInitializer)
        (*headerFile) += ", ";
    else
        firstBindingInitializer = false;

    return true;
}

void CppVisitor::closePatternInitializer(PatternInitializer* patternInitializer) {
}

bool CppVisitor::openAdditionalInitializer(AdditionalInitializer* additionalInitializer) {
    return true;
}

void CppVisitor::closeAdditionalInitializer(AdditionalInitializer* additionalInitializer) {
}

bool CppVisitor::openFunctionDeclaration(FunctionDeclaration* functionDeclaration) {
    if (!functionDeclaration->body)
        virtualFunction = true;
    else
        virtualFunction = false;
    return true;
}

void CppVisitor::closeFunctionDeclaration(FunctionDeclaration* functionDeclaration) {
    if (virtualFunction) {
        (*headerFile) += " = 0";
        virtualFunction = false; } }

bool CppVisitor::openInitializerDeclaration(InitializerDeclaration* initializerDeclaration) {
    (*headerFile) += *classDeclarationName;
    (*headerFile) += "(";
    return true;
}

void CppVisitor::closeInitializerDeclaration(InitializerDeclaration* initializerDeclaration) {
}

void CppVisitor::visitOverrideWord(OverrideWord* overrideWord) {
}

void CppVisitor::visitStaticWord(StaticWord* staticWord) {
}

void CppVisitor::visitIdentifierFunction(IdentifierFunction* identifierFunction) {
    this->identifierFunctionName = identifierFunction->name; }

bool CppVisitor::openFunctionSignature(FunctionSignature* functionSignature) {
    if (virtualFunction)
        (*headerFile) += "virtual ";
    if (!functionSignature->result)
        (*headerFile) += "void";
    else {
        if (functionSignature->result->resultType->_isTypeIdentifier()) {
            TypeIdentifier* typeId = (TypeIdentifier*)functionSignature->result->resultType;
            (*headerFile) += getCppType(typeId->name);
            if (isClass(typeId->name)) {
                (*headerFile) += "*"; } } }
    (*headerFile) += " ";
    (*headerFile) += *identifierFunctionName;
    (*headerFile) += "(";
    if ((functionSignature->result) && (functionSignature->result->resultType->_isTypeIdentifier())) {
        TypeIdentifier* typeId = (TypeIdentifier*)functionSignature->result->resultType;
        if (isClass(typeId->name)) {
            (*headerFile) += "_Page* _rp";
            if (functionSignature->parameterClause->parameters)
                (*headerFile) += ", ";

            } }
   return true;
}

void CppVisitor::closeFunctionSignature(FunctionSignature* functionSignature) {
}

bool CppVisitor::openFunctionResult(FunctionResult* functionResult) {
    return false;
}

void CppVisitor::closeFunctionResult(FunctionResult* functionResult) {
}

bool CppVisitor::openParameterClause(ParameterClause* parameterClause) {
    firstParameter = true;
    inParameterClause = true;
    return true;
}

void CppVisitor::closeParameterClause(ParameterClause* parameterClause) {
    (*headerFile) += ")";
    inParameterClause = false;
}

bool CppVisitor::openConstParameter(ConstParameter* constParameter) {
    constParameterName = constParameter->name;
    if (!firstParameter)
        (*headerFile) += ", ";
    else
        firstParameter = false;

    constParameter->type->accept(*this);

    (*headerFile) += " ";
    (*headerFile) += *constParameter->name;

    return false;
}

bool CppVisitor::isClass(_LetString* name) {
    if (*name == "String")
        return true;

    size_t _classes_length = classes->length();
    for (size_t _i = 0; _i < _classes_length; _i++) {
        if (**(*classes)[_i] == *name)
            return true;
    }

    return false;
}

void CppVisitor::closeConstParameter(ConstParameter* constParameter) {
    (*headerFile) += *constParameterName;
    constParameterName = 0;
}

bool CppVisitor::openVarParameter(VarParameter* varParameter) {
    varParameterName = varParameter->name;
    return true;
}

void CppVisitor::closeVarParameter(VarParameter* varParameter) {
    varParameterName = 0;
}

bool CppVisitor::openThrowsClause(ThrowsClause* throwsClause) {
    return true;
}

void CppVisitor::closeThrowsClause(ThrowsClause* throwsClause) {
}

bool CppVisitor::openEnumDeclaration(EnumDeclaration* enumDeclaration) {
    enumDeclarationName = enumDeclaration->name;
    return true;
}

void CppVisitor::closeEnumDeclaration(EnumDeclaration* enumDeclaration) {
    enumDeclarationName = 0;
}

bool CppVisitor::openEnumMember(EnumMember* enumMember) {
    return true;
}

void CppVisitor::closeEnumMember(EnumMember* enumMember) {
}

bool CppVisitor::openTupleType(TupleType* tupleType) {
    return true;
}

void CppVisitor::closeTupleType(TupleType* tupleType) {
}

bool CppVisitor::openAdditionalType(AdditionalType* additionalType) {
    return true;
}

void CppVisitor::closeAdditionalType(AdditionalType* additionalType) {
}

void CppVisitor::visitEnumCase(EnumCase* enumCase) {
}

bool CppVisitor::openAdditionalCase(AdditionalCase* additionalCase) {
    return true;
}

void CppVisitor::closeAdditionalCase(AdditionalCase* additionalCase) {
}

bool CppVisitor::openClassDeclaration(ClassDeclaration* classDeclaration) {
    classDeclarationName = classDeclaration->name;

    (*headerFile) += "\n\nclass ";
    (*headerFile) += *classDeclarationName;
    if (!classDeclaration->body) {
        (*headerFile) += ";";
        return false; }
    (*headerFile) += " : public ";
    if (classDeclaration->typeInheritanceClause) {
        size_t noOfInheritanceClauses = classDeclaration->typeInheritanceClause->inheritances->length();
        for (size_t _i = 0; _i < noOfInheritanceClauses; _i++) {
            if (_i > 0)
                (*headerFile) += ", ";
            TypeIdentifier* typeIdentifier = (*(*classDeclaration->typeInheritanceClause->inheritances)[_i])->typeIdentifier;
            (*headerFile) += *typeIdentifier->name;
        }
    }
    else {
        (*headerFile) += "Object";
    }
    (*headerFile) += " {\n";
    (*headerFile) += "public:";

    headerIndentLevel++;

    return true;
}

void CppVisitor::closeClassDeclaration(ClassDeclaration* classDeclaration) {
    if (classDeclaration->typeInheritanceClause) {
        (*headerFile) += "\n"; indentHeader(); (*headerFile) += "virtual bool _is"; (*headerFile) += *classDeclarationName; (*headerFile) += "();";
    }
    else {
        _Region _region; _Page* _p = _region.get();
        _Array<_LetString>& derivedClasses = *new(_p) _Array<_LetString>();
        collectDerivedClasses(&derivedClasses, classDeclarationName);
        size_t _derivedClasses_length = derivedClasses.length();
        for (size_t _i = 0; _i < _derivedClasses_length; _i++) {
            (*headerFile) += "\n"; indentHeader(); (*headerFile) += "virtual bool _is"; (*headerFile) += **derivedClasses[_i]; (*headerFile) += "();";
        }
    }

    headerIndentLevel--;
    classDeclarationName = 0;
    (*headerFile) += "\n};";
}

bool CppVisitor::openClassBody(ClassBody* classBody) {
    return true;
}

void CppVisitor::closeClassBody(ClassBody* classBody) {
}

void CppVisitor::indentHeader() {
    for (size_t i = 0; i < headerIndentLevel; i++)
        (*headerFile) += "    ";
}

void CppVisitor::collectDerivedClasses(_Array<_LetString>* derivedClasses, _LetString* className) {
    size_t _inherits_length = inherits->length();
    for (size_t _i = 0; _i < _inherits_length; _i++) {
        if (*(*(*inherits)[_i])->name == *className)
            appendDerivedClasses(derivedClasses, (*(*inherits)[_i])->inheritors);
    }
}

void CppVisitor::appendDerivedClasses(_Array<_LetString>* derivedClasses, _Array<_LetString>* inheritors) {
    size_t _inheritors_length = inheritors->length();
    for (size_t _i = 0; _i < _inheritors_length; _i++) {
        _LetString* derivedClass = *((*inheritors)[_i]);
        derivedClasses->push(derivedClass);
        collectDerivedClasses(derivedClasses, derivedClass);
    }
}

bool CppVisitor::openGenericArgumentClause(GenericArgumentClause* genericArgumentClause) {
    return true;
}

void CppVisitor::closeGenericArgumentClause(GenericArgumentClause* genericArgumentClause) {
}

void CppVisitor::visitGenericParameter(GenericParameter* genericParameter) {
}

bool CppVisitor::openClassMember(ClassMember* classMember) {
    (*headerFile) += "\n";
    indentHeader();
    declaringClassMember = true;
    return true;
}

void CppVisitor::closeClassMember(ClassMember* classMember) {
    (*headerFile) += ";";
    declaringClassMember = false;
}

bool CppVisitor::openCodeBlock(CodeBlock* codeBlock) {
    codeBlockLevel++;
    return true;
}

void CppVisitor::closeCodeBlock(CodeBlock* codeBlock) {
    codeBlockLevel--;
}

bool CppVisitor::openSimpleExpression(SimpleExpression* simpleExpression) {
    return true;
}

void CppVisitor::closeSimpleExpression(SimpleExpression* simpleExpression) {
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

bool CppVisitor::openPathItemCatchPattern(PathItemCatchPattern* pathItemCatchPattern) {
    return true;
}

void CppVisitor::closePathItemCatchPattern(PathItemCatchPattern* pathItemCatchPattern) {
}

void CppVisitor::visitOperatorPostfix(OperatorPostfix* operatorPostfix) {
}

bool CppVisitor::openFunctionCall(FunctionCall* functionCall) {
    return true;
}

void CppVisitor::closeFunctionCall(FunctionCall* functionCall) {
}

bool CppVisitor::openExplicitMemberExpression(ExplicitMemberExpression* explicitMemberExpression) {
    return true;
}

void CppVisitor::closeExplicitMemberExpression(ExplicitMemberExpression* explicitMemberExpression) {
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

bool CppVisitor::openNamedMemberPostfix(NamedMemberPostfix* namedMemberPostfix) {
    return true;
}

void CppVisitor::closeNamedMemberPostfix(NamedMemberPostfix* namedMemberPostfix) {
}

bool CppVisitor::openParenthesizedExpression(ParenthesizedExpression* parenthesizedExpression) {
    return true;
}

void CppVisitor::closeParenthesizedExpression(ParenthesizedExpression* parenthesizedExpression) {
}

void CppVisitor::visitLiteralExpression(LiteralExpression* literalExpression) {
}

void CppVisitor::visitIdentifierExpression(IdentifierExpression* identifierExpression) {
}

bool CppVisitor::openIfExpression(IfExpression* ifExpression) {
    return true;
}

void CppVisitor::closeIfExpression(IfExpression* ifExpression) {
}

bool CppVisitor::openElseClause(ElseClause* elseClause) {
    return true;
}

void CppVisitor::closeElseClause(ElseClause* elseClause) {
}

bool CppVisitor::openSwitchExpression(SwitchExpression* switchExpression) {
    return true;
}

void CppVisitor::closeSwitchExpression(SwitchExpression* switchExpression) {
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

bool CppVisitor::openCaseItem(CaseItem* caseItem) {
    return true;
}

void CppVisitor::closeCaseItem(CaseItem* caseItem) {
}

bool CppVisitor::openForExpression(ForExpression* forExpression) {
    return true;
}

void CppVisitor::closeForExpression(ForExpression* forExpression) {
}

bool CppVisitor::openForEach(ForEach* forEach) {
    return true;
}

void CppVisitor::closeForEach(ForEach* forEach) {
}

bool CppVisitor::openPlainFor(PlainFor* plainFor) {
    return true;
}

void CppVisitor::closePlainFor(PlainFor* plainFor) {
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

void CppVisitor::visitWildcardPattern(WildcardPattern* wildcardPattern) {
}

bool CppVisitor::openIdentifierPattern(IdentifierPattern* identifierPattern) {
    if (!codeBlockLevel) {
        identifierPatternIdentifier = identifierPattern->identifier;
        if (identifierPattern->annotationForType) {
            identifierPattern->annotationForType->accept(*this);
            (*headerFile) += " ";
        }
        (*headerFile) += *identifierPattern->identifier; }
    return false; }

void CppVisitor::closeIdentifierPattern(IdentifierPattern* identifierPattern) {
    identifierPatternIdentifier = 0;
}

bool CppVisitor::openTuplePattern(TuplePattern* tuplePattern) {
    return true;
}

void CppVisitor::closeTuplePattern(TuplePattern* tuplePattern) {
}

bool CppVisitor::openTuplePatternElement(TuplePatternElement* tuplePatternElement) {
    return true;
}

void CppVisitor::closeTuplePatternElement(TuplePatternElement* tuplePatternElement) {
}

bool CppVisitor::openExpressionPattern(ExpressionPattern* expressionPattern) {
    return true;
}

void CppVisitor::closeExpressionPattern(ExpressionPattern* expressionPattern) {
}

void CppVisitor::visitDefaultCaseLabel(DefaultCaseLabel* defaultCaseLabel) {
}

bool CppVisitor::openBlockCaseContent(BlockCaseContent* blockCaseContent) {
    return true;
}

void CppVisitor::closeBlockCaseContent(BlockCaseContent* blockCaseContent) {
}

void CppVisitor::visitEmptyCaseContent(EmptyCaseContent* emptyCaseContent) {
}

bool CppVisitor::openInitializerCall(InitializerCall* initializerCall) {
    return true;
}

void CppVisitor::closeInitializerCall(InitializerCall* initializerCall) {
}

bool CppVisitor::openThisDot(ThisDot* thisDot) {
    return true;
}

void CppVisitor::closeThisDot(ThisDot* thisDot) {
}

bool CppVisitor::openThisSubscript(ThisSubscript* thisSubscript) {
    return true;
}

void CppVisitor::closeThisSubscript(ThisSubscript* thisSubscript) {
}

void CppVisitor::visitThisWord(ThisWord* thisWord) {
}

void CppVisitor::visitThisInit(ThisInit* thisInit) {
}

void CppVisitor::visitThisMember(ThisMember* thisMember) {
}

bool CppVisitor::openSuperDot(SuperDot* superDot) {
    return true;
}

void CppVisitor::closeSuperDot(SuperDot* superDot) {
}

bool CppVisitor::openSuperSubscript(SuperSubscript* superSubscript) {
    return true;
}

void CppVisitor::closeSuperSubscript(SuperSubscript* superSubscript) {
}

void CppVisitor::visitSuperInit(SuperInit* superInit) {
}

void CppVisitor::visitSuperMember(SuperMember* superMember) {
}

bool CppVisitor::openTypeAnnotation(TypeAnnotation* annotationForType) {
    return true;
}

void CppVisitor::closeTypeAnnotation(TypeAnnotation* annotationForType) {
}

bool CppVisitor::openTypeIdentifier(TypeIdentifier* typeIdentifier) {
    typeIdentifierName = typeIdentifier->name;

    if (!codeBlockLevel) {
        (*headerFile) += getCppType(typeIdentifierName);
        if (isClass(typeIdentifierName)) {
            (*headerFile) += "*"; } }
    return true;
}

const char* CppVisitor::getCppType(_LetString* typeIdentifierName) {
    const char* typeIdentifier = typeIdentifierName->getNativeString();

    if ((*typeIdentifierName) == "unsigned")
        return "size_t";
    else if ((*typeIdentifierName) == "String")
        return "_LetString";

    return typeIdentifier;
}

void CppVisitor::closeTypeIdentifier(TypeIdentifier* typeIdentifier) {
    typeIdentifierName = 0;
}

bool CppVisitor::openSubtypeIdentifier(SubtypeIdentifier* subtypeIdentifier) {
    return true;
}

void CppVisitor::closeSubtypeIdentifier(SubtypeIdentifier* subtypeIdentifier) {
}

bool CppVisitor::openArrayType(ArrayType* arrayType) {
    return true;
}

void CppVisitor::closeArrayType(ArrayType* arrayType) {
}

void CppVisitor::visitOptionalType(OptionalType* optionalType) {
}

bool CppVisitor::openTypeInheritanceClause(TypeInheritanceClause* typeInheritanceClause) {
    return true;
}

void CppVisitor::closeTypeInheritanceClause(TypeInheritanceClause* typeInheritanceClause) {
}

bool CppVisitor::openInheritance(Inheritance* inheritance) {
    return false;
}

void CppVisitor::closeInheritance(Inheritance* inheritance) {
}

void CppVisitor::buildMainHeaderFileString(_VarString* mainHeaderFile, Program* program) {
    (*mainHeaderFile) += "#ifndef __scaly__";
    (*mainHeaderFile) += *programName;
    (*mainHeaderFile) += "__\n#define __scaly__";
    (*mainHeaderFile) += *programName;
    (*mainHeaderFile) += "__\n\n#include \"Scaly.h\"\n";
    size_t noOfCompilationUnits = program->compilationUnits->length();
    for (size_t i = 0; i < noOfCompilationUnits; i++) {
        _Region _region; _Page* _p = _region.get();
        (*mainHeaderFile) +=  "#include \"";
        (*mainHeaderFile) += *new(mainHeaderFile->getPage()) _VarString(*Path::getFileNameWithoutExtension(_p, *(*(*program->compilationUnits)[i])->fileName)) + ".h\"\n";
    }
    (*mainHeaderFile) += "\nusing namespace scaly;\nnamespace ";
    (*mainHeaderFile) += *programName;
    (*mainHeaderFile) += " {\nint _main(_Array<String>& arguments);\n}\n\n#endif // __scaly__scalyc__\n";
}

void CppVisitor::buildProjectFileString(_VarString* projectFile, Program* program) {
    (*projectFile) += "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
    (*projectFile) += "<CodeLite_Project Name=\"";
    (*projectFile) +=  *programName;
    (*projectFile) +=  "\" InternalType=\"Console\">\n  <Plugins>\n    <Plugin Name=\"qmake\">\n";
    (*projectFile) +=  "      <![CDATA[00020001N0005Debug0000000000000001N0007Release000000000000]]>\n    </Plugin>\n";
    (*projectFile) +=  "    <Plugin Name=\"CMakePlugin\">\n      <![CDATA[[{\n";
    (*projectFile) +=  "  \"name\": \"Debug\",\n  \"enabled\": false,\n  \"buildDirectory\": \"build\",\n";
    (*projectFile) +=  "  \"sourceDirectory\": \"$(ProjectPath)\",\n  \"generator\": \"\",\n  \"buildType\": \"\",\n";
    (*projectFile) +=  "  \"arguments\": [],\n  \"parentProject\": \"\"\n";
    (*projectFile) +=  " }, {\n";
    (*projectFile) +=  "  \"name\": \"Release\",\n";
    (*projectFile) +=  "  \"enabled\": false,\n";
    (*projectFile) +=  "  \"buildDirectory\": \"build\",\n";
    (*projectFile) +=  "  \"sourceDirectory\": \"$(ProjectPath)\",\n";
    (*projectFile) +=  "  \"generator\": \"\",\n";
    (*projectFile) +=  "  \"buildType\": \"\",\n";
    (*projectFile) +=  "  \"arguments\": [],\n";
    (*projectFile) +=  "  \"parentProject\": \"\"\n";
    (*projectFile) +=  " }]]]>\n    </Plugin>\n  </Plugins>\n";
    (*projectFile) +=  "  <Description/>\n  <Dependencies/>\n";
    (*projectFile) +=  "  <VirtualDirectory Name=\"src\">\n    <File Name=\"main.cpp\"/>\n";
    size_t noOfCompilationUnits = program->compilationUnits->length();
    for (size_t i = 0; i < noOfCompilationUnits; i++) {
        _Region _region; _Page* _p = _region.get();
        (*projectFile) +=  "    <File Name=\"";
        (*projectFile) +=  *Path::getFileNameWithoutExtension(_p, *(*(*program->compilationUnits)[i])->fileName);
        (*projectFile) +=  ".cpp\"/>\n";
    }
    (*projectFile) +=  "  </VirtualDirectory>\n  <VirtualDirectory Name=\"include\">\n";
    for (size_t i = 0; i < noOfCompilationUnits; i++) {
        _Region _region; _Page* _p = _region.get();
        (*projectFile) +=  "    <File Name=\"";
        (*projectFile) +=  *Path::getFileNameWithoutExtension(_p, *(*(*program->compilationUnits)[i])->fileName);
        (*projectFile) +=  ".h\"/>\n";
    }
    (*projectFile) +=  "  </VirtualDirectory>\n  <Settings Type=\"Executable\">\n    <GlobalSettings>\n";
    (*projectFile) +=  "      <Compiler Options=\"\" C_Options=\"\" Assembler=\"\">\n";
    (*projectFile) +=  "        <IncludePath Value=\".\"/>\n      </Compiler>\n      <Linker Options=\"\">\n";
    (*projectFile) +=  "        <LibraryPath Value=\".\"/>\n      </Linker>\n      <ResourceCompiler Options=\"\"/>\n";
    (*projectFile) +=  "    </GlobalSettings>\n";
    (*projectFile) +=  "    <Configuration Name=\"Debug\" CompilerType=\"GCC\" DebuggerType=\"GNU gdb debugger\"";
    (*projectFile) +=  " Type=\"Executable\" BuildCmpWithGlobalSettings=\"push\" BuildLnkWithGlobalSettings=\"push\"";
    (*projectFile) +=  " BuildResWithGlobalSettings=\"push\">\n";
    (*projectFile) +=  "      <Compiler Options=\"-g;-O0;-std=c++11;-Wall\" C_Options=\"-g;-O0;-Wall\" Assembler=\"\"";
    (*projectFile) +=  " Required=\"yes\" PreCompiledHeader=\"\" PCHInCommandLine=\"no\" PCHFlags=\"\" PCHFlagsPolicy=\"0\">\n";
    (*projectFile) +=  "        <IncludePath Value=\".\"/>\n        <IncludePath Value=\"../scaly\"/>\n      </Compiler>\n";
    (*projectFile) +=  "      <Linker Options=\"\" Required=\"yes\">\n        <LibraryPath Value=\"../Debug\"/>\n";
    (*projectFile) +=  "        <Library Value=\"libscaly\"/>\n      </Linker>\n      <ResourceCompiler Options=\"\" Required=\"no\"/>\n";
    (*projectFile) +=  "      <General OutputFile=\"$(IntermediateDirectory)/$(ProjectName)\" IntermediateDirectory=\"../Debug\" ";
    (*projectFile) +=  "Command=\"./$(ProjectName)\" CommandArguments=\"-o ";
    (*projectFile) +=  *programName;
    (*projectFile) +=  " -d output";
    for (size_t i = 0; i < noOfCompilationUnits; i++) {
        _Region _region; _Page* _p = _region.get();
        (*projectFile) +=  " ../"; (*projectFile) +=  *programName; (*projectFile) += "/";
        (*projectFile) +=  *Path::getFileNameWithoutExtension(_p, *(*(*program->compilationUnits)[i])->fileName);
        (*projectFile) +=  ".scaly";
    }
    (*projectFile) += "\" UseSeparateDebugArgs=\"no\" DebugArguments=\"\" WorkingDirectory=\"$(IntermediateDirectory)\"";
    (*projectFile) += " PauseExecWhenProcTerminates=\"yes\" IsGUIProgram=\"no\" IsEnabled=\"yes\"/>\n";
    (*projectFile) += "      <Environment EnvVarSetName=\"&lt;Use Defaults&gt;\" DbgSetName=\"&lt;Use Defaults&gt;\">\n";
    (*projectFile) += "        <![CDATA[]]>\n      </Environment>\n";
    (*projectFile) += "      <Debugger IsRemote=\"no\" RemoteHostName=\"\" RemoteHostPort=\"\" DebuggerPath=\"\" IsExtended=\"yes\">\n";
    (*projectFile) += "        <DebuggerSearchPaths/>\n        <PostConnectCommands/>\n        <StartupCommands/>\n      </Debugger>\n";
    (*projectFile) += "      <PreBuild/>\n      <PostBuild/>\n      <CustomBuild Enabled=\"no\">\n        <RebuildCommand/>\n";
    (*projectFile) += "        <CleanCommand/>\n        <BuildCommand/>\n        <PreprocessFileCommand/>\n        <SingleFileCommand/>\n";
    (*projectFile) += "        <MakefileGenerationCommand/>\n        <ThirdPartyToolName>None</ThirdPartyToolName>\n";
    (*projectFile) += "        <WorkingDirectory/>\n      </CustomBuild>\n      <AdditionalRules>\n";
    (*projectFile) += "        <CustomPostBuild/>\n        <CustomPreBuild/>\n      </AdditionalRules>\n";
    (*projectFile) += "      <Completion EnableCpp11=\"no\" EnableCpp14=\"no\">\n        <ClangCmpFlagsC/>\n        <ClangCmpFlags/>\n";
    (*projectFile) += "        <ClangPP/>\n        <SearchPaths/>\n      </Completion>\n";
    (*projectFile) += "    </Configuration>\n";
    (*projectFile) += "    <Configuration Name=\"Release\" CompilerType=\"GCC\" DebuggerType=\"GNU gdb debugger\" Type=\"Executable\"";
    (*projectFile) += " BuildCmpWithGlobalSettings=\"push\" BuildLnkWithGlobalSettings=\"push\" BuildResWithGlobalSettings=\"push\">\n";
    (*projectFile) += "      <Compiler Options=\"-O2;-Wall\" C_Options=\"-O2;-Wall\" Assembler=\"\" Required=\"yes\" PreCompiledHeader=\"\"";
    (*projectFile) += " PCHInCommandLine=\"no\" PCHFlags=\"\" PCHFlagsPolicy=\"0\">\n";
    (*projectFile) += "        <IncludePath Value=\".\"/>\n        <Preprocessor Value=\"NDEBUG\"/>\n";
    (*projectFile) += "      </Compiler>\n      <Linker Options=\"\" Required=\"yes\">\n";
    (*projectFile) += "        <LibraryPath Value=\"../Release\"/>\n        <Library Value=\"libscaly\"/>\n      </Linker>\n";
    (*projectFile) += "      <ResourceCompiler Options=\"\" Required=\"no\"/>\n";
    (*projectFile) += "      <General OutputFile=\"$(IntermediateDirectory)/$(ProjectName)\" IntermediateDirectory=\"../Release\"";
    (*projectFile) += " Command=\"./$(ProjectName)\" CommandArguments=\"\" UseSeparateDebugArgs=\"no\" DebugArguments=\"\"";
    (*projectFile) += " WorkingDirectory=\"$(IntermediateDirectory)\" PauseExecWhenProcTerminates=\"yes\" IsGUIProgram=\"no\" IsEnabled=\"yes\"/>\n";
    (*projectFile) += "      <Environment EnvVarSetName=\"&lt;Use Defaults&gt;\" DbgSetName=\"&lt;Use Defaults&gt;\">\n";
    (*projectFile) += "        <![CDATA[]]>\n      </Environment>\n";
    (*projectFile) += "      <Debugger IsRemote=\"no\" RemoteHostName=\"\" RemoteHostPort=\"\" DebuggerPath=\"\" IsExtended=\"yes\">\n";
    (*projectFile) += "        <DebuggerSearchPaths/>\n        <PostConnectCommands/>\n        <StartupCommands/>\n";
    (*projectFile) += "      </Debugger>\n      <PreBuild/>\n      <PostBuild/>\n      <CustomBuild Enabled=\"no\">\n";
    (*projectFile) += "        <RebuildCommand/>\n        <CleanCommand/>\n        <BuildCommand/>\n        <PreprocessFileCommand/>\n";
    (*projectFile) += "        <SingleFileCommand/>\n        <MakefileGenerationCommand/>\n";
    (*projectFile) += "        <ThirdPartyToolName>None</ThirdPartyToolName>\n        <WorkingDirectory/>\n";
    (*projectFile) += "      </CustomBuild>\n      <AdditionalRules>\n        <CustomPostBuild/>\n";
    (*projectFile) += "        <CustomPreBuild/>\n      </AdditionalRules>\n      <Completion EnableCpp11=\"no\" EnableCpp14=\"no\">\n";
    (*projectFile) += "        <ClangCmpFlagsC/>\n        <ClangCmpFlags/>\n        <ClangPP/>\n";
    (*projectFile) += "        <SearchPaths/>\n      </Completion>\n    </Configuration>\n  </Settings>\n</CodeLite_Project>\n";
}

Inherits::Inherits(_LetString* className) {
    name = &_LetString::create(getPage(), *className);
    inheritors = new(getPage()) _Array<_LetString>();
}

}
