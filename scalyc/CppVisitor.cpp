#include "scalyc.h"
using namespace scaly;
namespace scalyc {

CppVisitor::CppVisitor() {
    this->cppError = 0;
}

CppError* CppVisitor::execute(_Page* _rp, Program* program) {
    program->accept(this);
    return cppError;
}

bool CppVisitor::openProgram(Program* program) {
    _Region _region; _Page* _p = _region.get();
    _VarString* programDirectory = new(_p) _VarString(*program->directory);

    if (programDirectory == 0 || *programDirectory == "") {
        programDirectory->getPage()->clear();
        programDirectory = new(programDirectory->getPage()) _VarString(_LetString::create(this->getPage(), "."));
    }

    {
        _Region _region; _Page* _p = _region.get();
        if (!Directory::exists(_LetString::create(_p, *programDirectory))) {
            DirectoryError* dirError = Directory::create(getPage(), _LetString::create(_p, *programDirectory));
            if (dirError) {
                cppError = new(getPage()) CppError(new(getPage()) _CppError_unableToCreateOutputDirectory(&_LetString::create(getPage(), *programDirectory), dirError));
                return false; } } }

    {
        _Region _region; _Page* _p = _region.get();
        _VarString& outputFilePath = *new(_p) _VarString(*programDirectory);
        outputFilePath += "/";
        outputFilePath += *program->name;

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
        _Vector<TerminatedStatement>& statements = *compilationUnit->statements;
        size_t _statements_length = statements.length();
        for (size_t _i = 0; _i < _statements_length; _i++) {
            TerminatedStatement& terminatedStatement = **statements[_i];
            if (terminatedStatement.statement) {
                if (terminatedStatement.statement->_isClassDeclaration()) {
                    ClassDeclaration& classDeclaration = *(ClassDeclaration*)terminatedStatement.statement;
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
}


bool CppVisitor::openCompilationUnit(CompilationUnit* compilationUnit) {
    moduleName = Path::getFileNameWithoutExtension(getPage(), *compilationUnit->fileName);
    headerIndentLevel = 0;
    sourceIndentLevel = 0;
    declaringClassMember = false;
    inParameterClause = false;

    if (!compilationUnit->parent->_isProgram())
        return false;

    _LetString* programName = ((Program*)(compilationUnit->parent))->name;
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
    if (!compilationUnit->parent->_isProgram())
        return;

    _LetString* programName = ((Program*)(compilationUnit->parent))->name;
    _LetString* programDirectory = ((Program*)(compilationUnit->parent))->directory;

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

bool CppVisitor::openTerminatedStatement(TerminatedStatement* terminatedStatement) {
    this->indentSource();
    if (terminatedStatement->statement->_isSimpleExpression()) {
        SimpleExpression* simpleExpression = (SimpleExpression*)terminatedStatement->statement;
        if (simpleExpression->prefixExpression->expression->primaryExpression->_isIdentifierExpression()) {
            _Vector<BinaryOp>* binaryOps = simpleExpression->binaryOps;
            if (binaryOps != nullptr) {
                if (binaryOps->length() == 1) {
                    BinaryOp* binaryOp = *(*binaryOps)[0];
                    if (binaryOp->_isAssignment()) {
                        Assignment* assignment = (Assignment*)binaryOp;
                        _LetString* memberName = getMemberIfCreatingObject(assignment);
                        if ((memberName != nullptr) && (!assignmentIsInInitializer(assignment))) {
                            (*sourceFile) += *memberName;
                            (*sourceFile) += "->getPage()->clear();\n";
                            this->indentSource();
                        }
                    }
                }
            }
        }
    }
    return true;
}

void CppVisitor::closeTerminatedStatement(TerminatedStatement* terminatedStatement) {
    if (terminatedStatement->statement->_isClassDeclaration())
        return;

    if (terminatedStatement->statement->_isSimpleExpression()) {
        SimpleExpression* expression = (SimpleExpression*) terminatedStatement->statement;
        if (expression->prefixExpression->expression->primaryExpression->_isIfExpression())
            return;
    }
        
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
    (*sourceFile) += " = ";
    return true;
}

void CppVisitor::closeInitializer(Initializer* initializer) {
}

bool CppVisitor::openConstantDeclaration(ConstantDeclaration* constantDeclaration) {
    constDeclaration = true;
    if (constantDeclaration->parent->parent->parent->_isClassDeclaration()) {
        suppressSource = true;
    }
    return true;
}

void CppVisitor::closeConstantDeclaration(ConstantDeclaration* constantDeclaration) {
    constDeclaration = false;
    suppressSource = false;
}

bool CppVisitor::openVariableDeclaration(VariableDeclaration* variableDeclaration) {
    if (variableDeclaration->parent->parent->parent->_isClassDeclaration())
        suppressSource = true;
    return true;
}

void CppVisitor::closeVariableDeclaration(VariableDeclaration* variableDeclaration) {
    suppressSource = false;
}

bool CppVisitor::openMutableDeclaration(MutableDeclaration* mutableDeclaration) {
    if (mutableDeclaration->parent->parent->parent->_isClassDeclaration())
        suppressSource = true;
    return true;
}

void CppVisitor::closeMutableDeclaration(MutableDeclaration* mutableDeclaration) {
    suppressSource = false;
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
        abstractFunction = true;
    else
        abstractFunction = false;
    if (functionDeclaration->modifiers) {
        size_t _modifiers_length = functionDeclaration->modifiers->length();
        for (size_t _i = 0; _i < _modifiers_length; _i++) {
            Modifier* modifier = *(*functionDeclaration->modifiers)[_i];
            if (modifier->_isStaticWord())
                staticFunction = true;
        }
    }
    return true;
}

void CppVisitor::closeFunctionDeclaration(FunctionDeclaration* functionDeclaration) {
    if (abstractFunction) {
        (*headerFile) += " = 0";
        abstractFunction = false;
    }
    staticFunction = false;
    suppressHeader = false;
}

bool CppVisitor::openInitializerDeclaration(InitializerDeclaration* initializerDeclaration) {
    if (!initializerDeclaration->parent->parent->parent->_isClassDeclaration())
        return false;

    _LetString* classDeclarationName = ((Program*)(initializerDeclaration->parent->parent->parent))->name;

    (*headerFile) += *classDeclarationName;
    (*headerFile) += "(";
    
    (*sourceFile) += *classDeclarationName;
    (*sourceFile) += "::";
    (*sourceFile) += *classDeclarationName;
    (*sourceFile) += "(";
    
    return true;
}

void CppVisitor::closeInitializerDeclaration(InitializerDeclaration* initializerDeclaration) {
    (*sourceFile) += "\n";
}

void CppVisitor::visitOverrideWord(OverrideWord* overrideWord) {
}

void CppVisitor::visitStaticWord(StaticWord* staticWord) {
}

void CppVisitor::visitIdentifierFunction(IdentifierFunction* identifierFunction) {
}

bool CppVisitor::openFunctionSignature(FunctionSignature* functionSignature) {
    if (!functionSignature->parent->_isFunctionDeclaration())
        return false;

    FunctionName* functionName = ((FunctionDeclaration*)(functionSignature->parent))->name;
    if (!functionName->_isIdentifierFunction())
        return false;
    
    _LetString* identifierFunctionName = ((IdentifierFunction*)functionName)->name;
        
    if (staticFunction)
        (*headerFile) += "static ";
    else
        (*headerFile) += "virtual ";
    inFunctionReturn = true;
    if (!functionSignature->result) {
        if (!functionSignature->throwsClause) {
            (*headerFile) += "void";
            (*sourceFile) += "void";
        }
        else {
            appendCppType(headerFile, functionSignature->throwsClause->throwsType);
            appendCppType(sourceFile, functionSignature->throwsClause->throwsType);
        }
    }
    else {
        if (functionSignature->throwsClause) {
            (*headerFile) += "_Result<";
            if (functionSignature->result->resultType->_isTypeIdentifier()) {
                TypeIdentifier* typeId = (TypeIdentifier*)functionSignature->result->resultType;
                appendCppTypeName(headerFile, typeId->name);
                appendCppTypeName(sourceFile, typeId->name);
            }
            else if (functionSignature->result->resultType->_isArrayType()) {
                ArrayType* arrayType = (ArrayType*)functionSignature->result->resultType;
                if (arrayType->elementType->_isTypeIdentifier()) {
                    (*headerFile) += "_Vector<";
                    (*sourceFile) += "_Vector<";
                    TypeIdentifier* typeId = (TypeIdentifier*)arrayType->elementType;
                    appendCppTypeName(headerFile, typeId->name);
                    appendCppTypeName(sourceFile, typeId->name);
                    (*headerFile) += ">";
                    (*sourceFile) += ">";
                }
            }
            (*headerFile) += ", ";
            (*sourceFile) += ", ";
            appendCppTypeName(headerFile, ((TypeIdentifier*)(functionSignature->throwsClause->throwsType))->name);
            appendCppTypeName(sourceFile, ((TypeIdentifier*)(functionSignature->throwsClause->throwsType))->name);
            (*headerFile) += ">";
            (*sourceFile) += ">";
        }
        else {
            if (functionSignature->result->resultType->_isTypeIdentifier()) {
                TypeIdentifier* typeId = (TypeIdentifier*)functionSignature->result->resultType;
                appendCppTypeName(headerFile, typeId->name);
                appendCppTypeName(sourceFile, typeId->name);
                if (isClass(typeId->name)) {
                    (*headerFile) += "*";
                    (*sourceFile) += "*";
                }
            }
            else if (functionSignature->result->resultType->_isArrayType()) {
                ArrayType* arrayType = (ArrayType*)functionSignature->result->resultType;
                if (arrayType->elementType->_isTypeIdentifier()) {
                    (*headerFile) += "_Vector<";
                    (*sourceFile) += "_Vector<";
                    TypeIdentifier* typeId = (TypeIdentifier*)arrayType->elementType;
                    appendCppTypeName(headerFile, typeId->name);
                    appendCppTypeName(sourceFile, typeId->name);
                    (*headerFile) += ">";
                    (*sourceFile) += ">";
                }
            }
        }
    }
    inFunctionReturn = false;
    (*headerFile) += " ";
    (*sourceFile) += " ";
    
    if (functionSignature->parent->parent->parent->parent->_isClassDeclaration()) {
        ClassDeclaration* classDeclaration = (ClassDeclaration*)functionSignature->parent->parent->parent->parent;
        (*sourceFile) += *classDeclaration->name;
        (*sourceFile) += "::";
    }
    (*headerFile) += *identifierFunctionName;
    (*sourceFile) += *identifierFunctionName;
    (*headerFile) += "(";
    (*sourceFile) += "(";
    if (functionSignature->result != nullptr) {
        if (functionSignature->result->resultType->_isTypeIdentifier()) {
            TypeIdentifier* typeId = (TypeIdentifier*)functionSignature->result->resultType;
            if (isClass(typeId->name)) {
                if (functionSignature->result->existingObject == nullptr) {
                    (*headerFile) += "_Page* _rp";
                    (*sourceFile) += "_Page* _rp";
                    if ((functionSignature->parameterClause->parameters) || (functionSignature->throwsClause)) {
                        (*headerFile) += ", ";
                        (*sourceFile) += ", ";
                    }
                }
            }
        }
        else if (functionSignature->result->resultType->_isArrayType()) {
            if (functionSignature->result->existingObject == nullptr) {
                (*headerFile) += "_Page* _rp";
                (*sourceFile) += "_Page* _rp";
                if ((functionSignature->parameterClause->parameters) || (functionSignature->throwsClause)) {
                    (*headerFile) += ", ";
                    (*sourceFile) += ", ";
                }
            }
        }
    }
    if (functionSignature->throwsClause) {
        (*headerFile) += "_Page* _ep";
        (*sourceFile) += "_Page* _ep";
        if (functionSignature->parameterClause->parameters) {
            (*headerFile) += ", ";
            (*sourceFile) += ", ";
        }
    }

    return true;
}

void CppVisitor::closeFunctionSignature(FunctionSignature* functionSignature) {
    suppressHeader = true;
}

bool CppVisitor::openFunctionResult(FunctionResult* functionResult) {
    return false;
}

void CppVisitor::closeFunctionResult(FunctionResult* functionResult) {
}

void CppVisitor::visitExistingClause(ExistingClause* existingClause) {
}

bool CppVisitor::openParameterClause(ParameterClause* parameterClause) {
    firstParameter = true;
    inParameterClause = true;
    return true;
}

void CppVisitor::closeParameterClause(ParameterClause* parameterClause) {
    (*headerFile) += ")";
    (*sourceFile) += ") ";
    inParameterClause = false;
}

bool CppVisitor::openConstParameter(ConstParameter* constParameter) {
    _LetString* constParameterName = constParameter->name;
    constDeclaration = true;
    writeParameter(constParameterName, constParameter->parameterType);
    constDeclaration = false;
    return false;
}

void CppVisitor::writeParameter(_LetString* name, Type* parameterType) {
    if (!firstParameter) {
        (*headerFile) += ", ";
        (*sourceFile) += ", ";
    }
    else {
        firstParameter = false;
    }

    parameterType->accept(this);
    (*headerFile) += " ";
    (*sourceFile) += " ";
    (*headerFile) += *name;
    (*sourceFile) += *name;
}

bool CppVisitor::isClass(_LetString* name) {
    if (    (*name == "String")
        ||  (*name == "DirectoryError")
        ||  (*name == "FileError")
        ||  (*name == "ParserError")
        ||  (*name == "CppError")
        ||  (*name == "CompilerError")
       )
        return true;

    size_t _classes_length = classes->length();
    for (size_t _i = 0; _i < _classes_length; _i++) {
        if (**(*classes)[_i] == *name)
            return true;
    }

    return false;
}

void CppVisitor::closeConstParameter(ConstParameter* constParameter) {
    (*headerFile) += *constParameter->name;
}

bool CppVisitor::openVarParameter(VarParameter* varParameter) {
    writeParameter(varParameter->name, varParameter->parameterType);
    return false;
}

void CppVisitor::closeVarParameter(VarParameter* varParameter) {
    _LetString* varParameterName = varParameter->name;
    (*headerFile) += *varParameterName;
}

bool CppVisitor::openThrowsClause(ThrowsClause* throwsClause) {
    return false;
}

void CppVisitor::closeThrowsClause(ThrowsClause* throwsClause) {
}

bool CppVisitor::openEnumDeclaration(EnumDeclaration* enumDeclaration) {
    _LetString* enumDeclarationName = enumDeclaration->name;
    (*headerFile) += "\n\nclass ";
    (*headerFile) += *enumDeclarationName;
    (*headerFile) += ";\n";
    return true;
}

void CppVisitor::closeEnumDeclaration(EnumDeclaration* enumDeclaration) {
    _LetString* enumDeclarationName = enumDeclaration->name;
    _Vector<EnumMember>* members = enumDeclaration->members;
    if (members) {
        (*headerFile) += "enum _";
        (*headerFile) += *enumDeclarationName;
        (*headerFile) += "Code {\n";
        size_t _members_length = members->length();
        for (size_t _i = 0; _i < _members_length; _i++) {
            (*headerFile) += "    _";
            (*headerFile) += *enumDeclarationName;
            (*headerFile) += "Code_";
            (*headerFile) += *(*(*members)[_i])->enumCase->name;
            if (!_i)
                (*headerFile) += " = 1";
            (*headerFile) += ",\n";
        }
        (*headerFile) += "};\n\n";
    }
    (*headerFile) += "class ";
    (*headerFile) += *enumDeclarationName;
    (*headerFile) += " : public Object\n{\npublic:\n    ";
    (*headerFile) += *enumDeclarationName;
    (*headerFile) += "(_";
    (*headerFile) += *enumDeclarationName;
    (*headerFile) += "Code errorCode)\n    : errorCode(errorCode), errorInfo(0) {}\n\n";
    if (members) {
        size_t _members_length = members->length();
        for (size_t _i = 0; _i < _members_length; _i++) {
            EnumMember* member = *(*members)[_i];
            if (member->parameterClause) {
                (*headerFile) += "    ";
                (*headerFile) += *enumDeclarationName;
                (*headerFile) += "(_";
                (*headerFile) += *enumDeclarationName;
                (*headerFile) += "_";
                (*headerFile) += *member->enumCase->name;
                (*headerFile) += "* ";
                (*headerFile) += *member->enumCase->name;
                (*headerFile) += ")\n    : errorCode(_";
                (*headerFile) += *enumDeclarationName;
                (*headerFile) += "Code_";
                (*headerFile) += *member->enumCase->name;
                (*headerFile) += "), errorInfo(";
                (*headerFile) += *member->enumCase->name;
                (*headerFile) += ") {}\n\n";
            }
        }
    }
    (*headerFile) += "    long getErrorCode();\n    void* getErrorInfo();\n\n";
    if (members) {
        size_t _members_length = members->length();
        for (size_t _i = 0; _i < _members_length; _i++) {
            EnumMember* member = *(*members)[_i];
            if (member->parameterClause) {
                (*headerFile) += "    _";
                (*headerFile) += *enumDeclarationName;
                (*headerFile) += "_";
                (*headerFile) += *member->enumCase->name;
                (*headerFile) += "* get_";
                (*headerFile) += *member->enumCase->name;
                (*headerFile) += "();\n";
            }
        }
    }
    (*headerFile) += "\nprivate:\n    _";
    (*headerFile) += *enumDeclarationName;
    (*headerFile) +="Code errorCode;\n    void* errorInfo;\n};";
}

bool CppVisitor::openEnumMember(EnumMember* enumMember) {
    if (!enumMember->parent->_isEnumDeclaration())
        return false;

    _LetString* enumDeclarationName = ((EnumDeclaration*)(enumMember->parent))->name;
    if (enumMember->parameterClause) {
        (*headerFile) += "\nclass _";
        (*headerFile) += *enumDeclarationName;
        (*headerFile) += "_";
        (*headerFile) += *enumMember->enumCase->name;
        (*headerFile) += " : public Object {\npublic:\n    _";
        (*headerFile) += *enumDeclarationName;
        (*headerFile) += "_";
        (*headerFile) += *enumMember->enumCase->name;
        (*headerFile) += "(";
    }
    inEnumMember = true;
    return true;
}

void CppVisitor::closeEnumMember(EnumMember* enumMember) {
    if (enumMember->parameterClause) {
        (*headerFile) += ";\n\n";
        _Vector<Parameter>* parameters = enumMember->parameterClause->parameters;
        if (parameters) {
            size_t _parameters_length = parameters->length();
            for (size_t _i = 0; _i < _parameters_length; _i++) {
                Parameter* parameter = *(*parameters)[_i];
                if (parameter->_isConstParameter()) {
                    ConstParameter* constParameter = (ConstParameter*)parameter;
                    (*headerFile) += "    ";
                    appendCppType(headerFile, constParameter->parameterType);
                    (*headerFile) += " ";
                    (*headerFile) += *constParameter->name;
                    (*headerFile) += ";\n";
                }
            }
        }
        (*headerFile) += "};\n";
    }
    inEnumMember = false;
}

void CppVisitor::appendCppType(_VarString* s, Type* type) {
    if (type->_isTypeIdentifier()) {
        TypeIdentifier* typeId = (TypeIdentifier*)type;
        appendCppTypeName(s, typeId->name);
        if (isClass(typeId->name)) {
            (*s) += "*"; }
    }
    else if (type->_isArrayType()) {
        ArrayType* arrayType = (ArrayType*)type;
        Type* type = arrayType->elementType;
        if (type->_isTypeIdentifier()) {
            TypeIdentifier* typeId = (TypeIdentifier*)type;
            (*s) += "_Vector<";
            appendCppTypeName(s, typeId->name);
            (*s) += ">*";
        }
    }
}

void CppVisitor::visitEnumCase(EnumCase* enumCase) {
}

bool CppVisitor::openAdditionalCase(AdditionalCase* additionalCase) {
    return true;
}

void CppVisitor::closeAdditionalCase(AdditionalCase* additionalCase) {
}

bool CppVisitor::openClassDeclaration(ClassDeclaration* classDeclaration) {
    (*headerFile) += "\n\nclass ";
    (*headerFile) += *classDeclaration->name;
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
    (*headerFile) += "\n";
    if (classDeclaration->typeInheritanceClause) {
        (*headerFile) += "\n"; indentHeader(); (*headerFile) += "virtual bool _is"; (*headerFile) += *classDeclaration->name; (*headerFile) += "();";
        (*sourceFile) += "bool "; 
        (*sourceFile) += *classDeclaration->name; 
        (*sourceFile) += "::_is"; 
        (*sourceFile) += *classDeclaration->name; 
        (*sourceFile) += "() { return true; }\n\n";
    }
    {
        _Region _region; _Page* _p = _region.get();
        _Array<_LetString>& derivedClasses = *new(_p) _Array<_LetString>();
        collectDerivedClasses(&derivedClasses, &_LetString::create(_p, *classDeclaration->name));
        size_t _derivedClasses_length = derivedClasses.length();
        for (size_t _i = 0; _i < _derivedClasses_length; _i++) {
            (*headerFile) += "\n"; indentHeader(); (*headerFile) += "virtual bool _is"; (*headerFile) += **derivedClasses[_i]; (*headerFile) += "();";
            (*sourceFile) += "bool "; 
            (*sourceFile) += *classDeclaration->name; 
            (*sourceFile) += "::_is"; 
            (*sourceFile) += **derivedClasses[_i]; 
            (*sourceFile) += "() { return false; }\n";
        }
        if (derivedClasses.length() > 0)
            (*sourceFile) += "\n";
    }
    

    headerIndentLevel--;
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

void CppVisitor::indentSource() {
    for (size_t i = 0; i < sourceIndentLevel; i++)
        (*sourceFile) += "    ";
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
    (*sourceFile) += "{\n";
    sourceIndentLevel++;
    return true;
}

void CppVisitor::closeCodeBlock(CodeBlock* codeBlock) {
    sourceIndentLevel--;
    indentSource();
    (*sourceFile) += "}\n";
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
    if (postfixExpression->postfixes != nullptr) {
        if ((*(*postfixExpression->postfixes)[0])->_isSubscript()) {
            (*sourceFile) += "(*";
        }
    }
    return true;
}

void CppVisitor::closePostfixExpression(PostfixExpression* postfixExpression) {
}

bool CppVisitor::openBinaryOperation(BinaryOperation* binaryOperation) {
    (*sourceFile) += " ";
    (*sourceFile) += *binaryOperation->binaryOperator;
    (*sourceFile) += " ";
    return true;
}

void CppVisitor::closeBinaryOperation(BinaryOperation* binaryOperation) {
}

bool CppVisitor::openAssignment(Assignment* assignment) {
    (*sourceFile) += " = ";
    _LetString* memberName = getMemberIfCreatingObject(assignment);
    if (memberName != nullptr) {
        if (isClass(getFunctionName(assignment))) {
            (*sourceFile) += "new(";
            ClassDeclaration* classDeclaration = getClassDeclaration(assignment);
            if (isVariableMember(memberName, classDeclaration)) {
                if (!assignmentIsInInitializer(assignment)) {
                    (*sourceFile) += *memberName;
                    (*sourceFile) += "->";
                }
                (*sourceFile) += "getPage()";
                if (assignmentIsInInitializer(assignment)) {
                    (*sourceFile) += "->allocateExclusivePage()";
                }
            }
            (*sourceFile) += ") ";
        }
    }

    return true;
}

bool CppVisitor::assignmentIsInInitializer(Assignment* assignment) {
    if (assignment->parent->parent->parent->parent->_isInitializerDeclaration()) {
        return true;
    }
        
    
    return false;
}

_LetString* CppVisitor::getMemberIfCreatingObject(Assignment* assignment) {
     _LetString* functionName = getFunctionName(assignment);
    if (functionName == nullptr) {
        return nullptr;
    }
    PostfixExpression* rightSide = assignment->expression->expression;
    if (((isClass(functionName) || isCreatingObject(functionName, assignment)) && (rightSide->postfixes->length() == 1))) {
        if ((*(*rightSide->postfixes)[0])->_isFunctionCall()) {
            if (assignment->parent->_isSimpleExpression()) {
                SimpleExpression* simpleExpression = (SimpleExpression*)(assignment->parent);
                if (simpleExpression->prefixExpression->prefixOperator == 0) {
                    PostfixExpression* leftSide = simpleExpression->prefixExpression->expression;
                    if ((leftSide->postfixes == 0) && (leftSide->primaryExpression->_isIdentifierExpression())) {
                        IdentifierExpression* memberExpression = (IdentifierExpression*)(leftSide->primaryExpression);
                        _LetString* memberName = memberExpression->name;
                        ClassDeclaration* classDeclaration = getClassDeclaration(assignment);
                        if (classDeclaration != nullptr) {
                            return memberName;
                        }
                    }
                }
            }
        }
    }
    
    return nullptr;
}

_LetString* CppVisitor::getFunctionName(Assignment* assignment) {
   if (assignment->expression->prefixOperator == 0) {
        PostfixExpression* rightSide = assignment->expression->expression;
        if (rightSide->primaryExpression->_isIdentifierExpression()) {
            IdentifierExpression* classExpression = (IdentifierExpression*)(rightSide->primaryExpression);
            return classExpression->name;
        }
   }
   
   return nullptr;
}

bool CppVisitor::isCreatingObject(_LetString* functionName, SyntaxNode* node) {
    ClassDeclaration* classDeclaration = getClassDeclaration(node);
    if (classDeclaration == nullptr)
        return false;
    _Vector<ClassMember>* members = classDeclaration->body->members;
    if (members == nullptr)
        return false;
    size_t _members_length = members->length();
    for (size_t _i = 0; _i < _members_length; _i++) {
        ClassMember* member = *(*members)[_i];
        if (member->declaration->_isFunctionDeclaration()) {
            FunctionDeclaration* functionDeclaration = (FunctionDeclaration*)member->declaration;
            if (functionDeclaration->name->_isIdentifierFunction()) {
                IdentifierFunction* identifierFunction = (IdentifierFunction*)functionDeclaration->name;
                if ((*identifierFunction->name) == (*functionName))
                    return true;
            }
        } 
    }

    return false;
}

ClassDeclaration* CppVisitor::getClassDeclaration(SyntaxNode* node) {
    if (node->_isClassDeclaration())
        return (ClassDeclaration*)node;
    if (node->parent != nullptr)
        return getClassDeclaration(node->parent);
    
    return nullptr;
}

bool CppVisitor::isVariableMember(_LetString* memberName, ClassDeclaration* classDeclaration) {
    _Vector<ClassMember>* classMembers = classDeclaration->body->members;
    size_t _classMembers_length = classMembers->length();
    for (size_t _i = 0; _i < _classMembers_length; _i++) {
        BindingInitializer* bindingInitializer = 0;
        if ((*(*classMembers)[_i])->declaration->_isMutableDeclaration()) {
            MutableDeclaration* mutableDeclaration = (MutableDeclaration*)(*(*classMembers)[_i])->declaration;
            bindingInitializer = mutableDeclaration->initializer;
        }
        if ((*(*classMembers)[_i])->declaration->_isVariableDeclaration()) {
            VariableDeclaration* variableDeclaration = (VariableDeclaration*)(*(*classMembers)[_i])->declaration;
            bindingInitializer = variableDeclaration->initializer;
        }
        
        if (bindingInitializer == nullptr)
            continue;
            
        PatternInitializer* patternInitializer = bindingInitializer->initializer;
        if (patternInitializer->pattern->_isIdentifierPattern()) {
            IdentifierPattern* identifierPattern = (IdentifierPattern*)patternInitializer->pattern;
            if (*identifierPattern->identifier == *memberName)
                return true;
        }
    }

    return false;
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
    (*sourceFile) += "->";
    return true;
}

void CppVisitor::closeExplicitMemberExpression(ExplicitMemberExpression* explicitMemberExpression) {
}

bool CppVisitor::openSubscript(Subscript* subscript) {
    (*sourceFile) += "[";
    return true;
}

void CppVisitor::closeSubscript(Subscript* subscript) {
    (*sourceFile) += "]";
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
    bool outputParen = true;
    if (parenthesizedExpression->parent->_isFunctionCall()) {
        FunctionCall* functionCall = (FunctionCall*)parenthesizedExpression->parent;
        if (functionCall->parent->_isPostfixExpression()) {
            PostfixExpression* postfixExpression = (PostfixExpression*)functionCall->parent;
            if (postfixExpression->primaryExpression->_isIdentifierExpression()) {
                IdentifierExpression* identifierExpression = (IdentifierExpression*)postfixExpression->primaryExpression;
                if (!isClass(identifierExpression->name)) {
                    if (postfixExpression->parent->parent->_isAssignment()) {
                        Assignment* assignment = (Assignment*)postfixExpression->parent->parent;
                        _LetString* member = getMemberIfCreatingObject(assignment);
                        if (member != nullptr) {
                            ClassDeclaration* classDeclaration = getClassDeclaration(assignment);
                            if (isVariableMember(member, classDeclaration)) {
                                (*sourceFile) += "(";
                                (*sourceFile) += *member;
                                (*sourceFile) += "->getPage()";
                                outputParen = false;
                            }
                        }
                    }
                }
                else {
                    if (isClass(identifierExpression->name)) {
                        _LetString* className = identifierExpression->name;
                        if ((*className) == "String") {
                            outputParen = false;
                        }
                    }
                }
                    
            }
        }
    }

    if (outputParen)
        (*sourceFile) += "(";

    
    return true;
}

void CppVisitor::closeParenthesizedExpression(ParenthesizedExpression* parenthesizedExpression) {
    (*sourceFile) += ")";
}

void CppVisitor::visitLiteralExpression(LiteralExpression* literalExpression) {
    Literal* literal = literalExpression->literal;
    if (literal->_isNumericLiteral()) {
        NumericLiteral* numericLiteral = (NumericLiteral*)literal;
        (*sourceFile) += *numericLiteral->value;
    }
    else if (literal->_isStringLiteral()) {
        StringLiteral* stringLiteral = (StringLiteral*)literal;
        (*sourceFile) += "\"";
        (*sourceFile) += *stringLiteral->string;
        (*sourceFile) += "\"";
    }
    else if (literal->_isCharacterLiteral()) {
        CharacterLiteral* characterLiteral = (CharacterLiteral*)literal;
        (*sourceFile) += "\'";
        if (characterLiteral->value->getLength() > 0) {
            if (((*characterLiteral->value)[0] == '"') || ((*characterLiteral->value)[0] == '\''))
                (*sourceFile) += "\\";
        }
        (*sourceFile) += *characterLiteral->value;
        (*sourceFile) += "\'";
    }
}

void CppVisitor::visitIdentifierExpression(IdentifierExpression* identifierExpression) {
    if (isClass(identifierExpression->name)) {
        _LetString* className = identifierExpression->name;
        if ((*className) == "String") {
            (*sourceFile) += "&_LetString::create(token->getPage(), ";
        }
        else
            (*sourceFile) += *identifierExpression->name;
    }
    else
        (*sourceFile) += *identifierExpression->name;
    if (identifierExpression->parent->_isPostfixExpression()) {
        PostfixExpression* postfixExpression = (PostfixExpression*)identifierExpression->parent;
        if (postfixExpression->postfixes != nullptr) {
            if ((*(*postfixExpression->postfixes)[0])->_isSubscript()) {
                (*sourceFile) += ")";
            }
        }
    }
}

bool CppVisitor::openIfExpression(IfExpression* ifExpression) {
    (*sourceFile) += "if (";
    ifExpression->condition->accept(this);
    (*sourceFile) += ") ";
    ifExpression->consequent->accept(this);
    if (ifExpression->elseClause)
        ifExpression->elseClause->accept(this);

    return false;
}

void CppVisitor::closeIfExpression(IfExpression* ifExpression) {
}

bool CppVisitor::openElseClause(ElseClause* elseClause) {
    return true;
}

void CppVisitor::closeElseClause(ElseClause* elseClause) {
}

bool CppVisitor::openSwitchExpression(SwitchExpression* switchExpression) {
    (*sourceFile) += "switch (";
    return true;
}

void CppVisitor::closeSwitchExpression(SwitchExpression* switchExpression) {
}

bool CppVisitor::openCurliedSwitchBody(CurliedSwitchBody* curliedSwitchBody) {
    (*sourceFile) += ") {\n";
    sourceIndentLevel++;
    return true;
}

void CppVisitor::closeCurliedSwitchBody(CurliedSwitchBody* curliedSwitchBody) {
    sourceIndentLevel--;
    indentSource();
    (*sourceFile) += "}\n";
}

bool CppVisitor::openNakedSwitchBody(NakedSwitchBody* nakedSwitchBody) {
    return true;
}

void CppVisitor::closeNakedSwitchBody(NakedSwitchBody* nakedSwitchBody) {
}

bool CppVisitor::openSwitchCase(SwitchCase* switchCase) {
    indentSource();
    (*sourceFile) += "case ";
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
    (*sourceFile) += "case ";
    return true;
}

void CppVisitor::closeCaseItem(CaseItem* caseItem) {
    (*sourceFile) += ": ";
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

bool CppVisitor::openRepeatExpression(RepeatExpression* repeatExpression) {
    return true;
}

void CppVisitor::closeRepeatExpression(RepeatExpression* repeatExpression) {
}

bool CppVisitor::openReturnExpression(ReturnExpression* returnExpression) {
    (*sourceFile) += "return";
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
    if (identifierPattern->annotationForType) {
        identifierPattern->annotationForType->accept(this);
        if (!suppressHeader) {
            (*headerFile) += " ";
        }
        if (!suppressSource)
            (*sourceFile) += " ";
    }
    if (!suppressHeader) {
        (*headerFile) += *identifierPattern->identifier;
    }
    if (!suppressSource) {
        (*sourceFile) += *identifierPattern->identifier;
    }
    return false;
 }

void CppVisitor::closeIdentifierPattern(IdentifierPattern* identifierPattern) {
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
    if (expressionPattern->parent->_isItemCaseLabel())
        (*sourceFile) += ": ";
}

void CppVisitor::visitDefaultCaseLabel(DefaultCaseLabel* defaultCaseLabel) {
}

bool CppVisitor::openBlockCaseContent(BlockCaseContent* blockCaseContent) {
    (*sourceFile) += "{\n";
    sourceIndentLevel++;
    return true;
}

void CppVisitor::closeBlockCaseContent(BlockCaseContent* blockCaseContent) {
    indentSource();
    (*sourceFile) += "break;\n";
    sourceIndentLevel--;
    indentSource();
    (*sourceFile) += "}\n\n";
}

void CppVisitor::visitEmptyCaseContent(EmptyCaseContent* emptyCaseContent) {
}

bool CppVisitor::openInitializerCall(InitializerCall* initializerCall) {
    return true;
}

void CppVisitor::closeInitializerCall(InitializerCall* initializerCall) {
}

bool CppVisitor::openThisDot(ThisDot* thisDot) {
    (*sourceFile) += "this->";
    if(thisDot->member->_isThisMember()) {
        ThisMember* thisMember = (ThisMember*)(thisDot->member);
        (*sourceFile) += *thisMember->name;
    }
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
    inArrayType = false;
    return true;
}

void CppVisitor::closeTypeAnnotation(TypeAnnotation* annotationForType) {
}

bool CppVisitor::openTypeIdentifier(TypeIdentifier* typeIdentifier) {
    if (!suppressHeader) {
        appendCppTypeName(headerFile, typeIdentifier->name);
    }
    if (!suppressSource) {
        appendCppTypeName(sourceFile, typeIdentifier->name);
    }
    if (isClass(typeIdentifier->name) && (!inArrayType)) {
        if (!suppressHeader) {
            (*headerFile) += "*";
        }
        if (!suppressSource) {
            (*sourceFile) += "*";
        }
    }
    return true;
}

void CppVisitor::appendCppTypeName(_VarString* s, _LetString* typeIdentifierName) {
    if ((*typeIdentifierName) == "unsigned") {
        (*s) += "size_t";
        return;
    }
    else if ((*typeIdentifierName) == "character") {
        (*s) += "char";
        return;
    }
    else {
        if ((*typeIdentifierName) == "String") {
            if ((constDeclaration)||(inArrayType)||(inEnumMember)||(inFunctionReturn)) {
                (*s) += "_LetString";
                return;
            }
            else {
                (*s) += "_VarString";
                return;
            }
        }
    }

    (*s) += *typeIdentifierName;
}

void CppVisitor::closeTypeIdentifier(TypeIdentifier* typeIdentifier) {
}

bool CppVisitor::openSubtypeIdentifier(SubtypeIdentifier* subtypeIdentifier) {
    return true;
}

void CppVisitor::closeSubtypeIdentifier(SubtypeIdentifier* subtypeIdentifier) {
}

bool CppVisitor::openArrayType(ArrayType* arrayType) {
    if (!sourceIndentLevel) {
        if (constDeclaration)
            (*headerFile) += "_Vector<";
        else
            (*headerFile) += "_Array<";
        inArrayType = true;
        arrayType->elementType->accept(this);
        inArrayType = false;
        (*headerFile) += ">*"; }
    return false;
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
    (*mainHeaderFile) += *program->name;
    (*mainHeaderFile) += "__\n#define __scaly__";
    (*mainHeaderFile) += *program->name;
    (*mainHeaderFile) += "__\n\n#include \"Scaly.h\"\n";
    size_t noOfCompilationUnits = program->compilationUnits->length();
    for (size_t i = 0; i < noOfCompilationUnits; i++) {
        _Region _region; _Page* _p = _region.get();
        (*mainHeaderFile) +=  "#include \"";
        (*mainHeaderFile) += *new(mainHeaderFile->getPage()) _VarString(*Path::getFileNameWithoutExtension(_p, *(*(*program->compilationUnits)[i])->fileName)) + ".h\"\n";
    }
    (*mainHeaderFile) += "\nusing namespace scaly;\nnamespace ";
    (*mainHeaderFile) += *program->name;
    (*mainHeaderFile) += " {\nint _main(_Vector<_LetString>* arguments);\n}\n\n#endif // __scaly__scalyc__\n";
}

void CppVisitor::buildProjectFileString(_VarString* projectFile, Program* program) {
    (*projectFile) += "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
    (*projectFile) += "<CodeLite_Project Name=\"";
    (*projectFile) +=  *program->name;
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
    (*projectFile) +=  *program->name;
    (*projectFile) +=  " -d output";
    for (size_t i = 0; i < noOfCompilationUnits; i++) {
        _Region _region; _Page* _p = _region.get();
        (*projectFile) +=  " ../"; (*projectFile) +=  *program->name; (*projectFile) += "/";
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

bool CppVisitor::_isCppVisitor() { return true; }

Inherits::Inherits(_LetString* className) {
    name = &_LetString::create(getPage(), *className);
    inheritors = new(getPage()) _Array<_LetString>();
}

}
