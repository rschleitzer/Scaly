#include "scalyc.h"
using namespace scaly;
namespace scalyc {

HeaderVisitor::HeaderVisitor(string* outputDirectory) {
    directory = outputDirectory;
    headerFile = nullptr;
    mainHeaderFile = nullptr;
    inherits = new(_getPage()->allocateExclusivePage()) _Array<Inherits>();
    classes = new(_getPage()->allocateExclusivePage()) _Array<string>();
}

bool HeaderVisitor::openProgram(Program* program) {
    _Region _region; _Page* _p = _region.get();
    string* programDirectory = new(_p) string(directory);
    if (programDirectory == nullptr || programDirectory->equals("")) {
        programDirectory = new(_p) string(".");
    }
    if (!Directory::exists(programDirectory)) {
        auto _Directory_error = Directory::create(_p, programDirectory);
        if (_Directory_error) { switch (_Directory_error->_getErrorCode()) {
            default: {
            return false;
            }
        } }
    }
    {
        _Region _region; _Page* _p = _region.get();
        VarString* outputFilePath = new(_p) VarString(programDirectory);
        outputFilePath->append("/");
        outputFilePath->append(program->name);
        {
            buildMainHeaderFileString(program);
            {
                _Region _region; _Page* _p = _region.get();
                VarString* headerFilePath = new(_p) VarString(outputFilePath);
                headerFilePath->append(".h");
                auto _File_error = File::writeFromString(_p, headerFilePath, mainHeaderFile);
                if (_File_error) { switch (_File_error->_getErrorCode()) {
                    default: {
                    return false;
                    }
                } }
            }
        }
        collectInheritances(program);
    }
    return true;
}

bool HeaderVisitor::openCompilationUnit(CompilationUnit* compilationUnit) {
    _Region _region; _Page* _p = _region.get();
    if (!(compilationUnit->parent->_isProgram()))
        return false;
    string* programName = ((Program*)compilationUnit->parent)->name;
    string* fileName = getFileName(_p, compilationUnit);
    if (fileName != nullptr) {
        _Region _region; _Page* _p = _region.get();
        string* name = Path::getFileNameWithoutExtension(_p, fileName);
        if (!fileName->equals(programName)) {
            if (headerFile != nullptr)
                headerFile->_getPage()->clear();
            headerFile = new(headerFile == nullptr ? _getPage()->allocateExclusivePage() : headerFile->_getPage()) VarString();
            headerFile->append("#ifndef __");
            headerFile->append(programName);
            headerFile->append("__");
            headerFile->append(name);
            headerFile->append("__\n");
            headerFile->append("#define __");
            headerFile->append(programName);
            headerFile->append("__");
            headerFile->append(name);
            headerFile->append("__\n#include \"");
            headerFile->append(programName);
            headerFile->append(".h\"\nusing namespace scaly;\nnamespace ");
            headerFile->append(programName);
            headerFile->append(" {");
        }
    }
    return true;
}

void HeaderVisitor::closeCompilationUnit(CompilationUnit* compilationUnit) {
    _Region _region; _Page* _p = _region.get();
    if (!(compilationUnit->parent)->_isProgram())
        return;
    string* programName = ((Program*)compilationUnit->parent)->name;
    VarString* outputFilePath = new(_p) VarString(directory);
    outputFilePath->append('/');
    string* fileName = getFileName(_p, compilationUnit);
    if (fileName != nullptr) {
        _Region _region; _Page* _p = _region.get();
        string* name = Path::getFileNameWithoutExtension(_p, fileName);
        outputFilePath->append(name);
        if (!fileName->equals(programName)) {
            _Region _region; _Page* _p = _region.get();
            headerFile->append("\n\n}\n#endif // __");
            headerFile->append(programName);
            headerFile->append("__");
            headerFile->append(name);
            headerFile->append("__\n");
            VarString* headerFilePath = new(_p) VarString(outputFilePath);
            headerFilePath->append(".h");
            auto _File_error = File::writeFromString(_p, headerFilePath, headerFile);
            if (_File_error) { switch (_File_error->_getErrorCode()) {
                default: {
                return;
                }
            } }
        }
    }
}

bool HeaderVisitor::openConstantDeclaration(ConstantDeclaration* constantDeclaration) {
    if (constantDeclaration->parent->parent->parent == nullptr)
        return false;
    if (constantDeclaration->parent->parent->parent->_isClassDeclaration())
        return true;
    return false;
}

bool HeaderVisitor::openMutableDeclaration(MutableDeclaration* mutableDeclaration) {
    if (mutableDeclaration->parent->parent->parent == nullptr)
        return false;
    if (mutableDeclaration->parent->parent->parent->_isClassDeclaration())
        return true;
    return false;
}

void HeaderVisitor::closeFunctionDeclaration(FunctionDeclaration* functionDeclaration) {
    if (functionDeclaration->body == nullptr)
        headerFile->append(" = 0");
}

bool HeaderVisitor::openEnumDeclaration(EnumDeclaration* enumDeclaration) {
    string* enumDeclarationName = enumDeclaration->name;
    headerFile->append("\n\nclass ");
    headerFile->append(enumDeclarationName);
    headerFile->append(";\n");
    return true;
}

void HeaderVisitor::closeEnumDeclaration(EnumDeclaration* enumDeclaration) {
    string* enumDeclarationName = enumDeclaration->name;
    if (enumDeclaration->members != nullptr) {
        headerFile->append("enum _");
        headerFile->append(enumDeclarationName);
        headerFile->append("Code {\n");
        int i = 0;
        EnumMember* member = nullptr;
        size_t _enumDeclaration_length = enumDeclaration->members->length();
        for (size_t _i = 0; _i < _enumDeclaration_length; _i++) {
            member = *(*enumDeclaration->members)[_i];
            {
                headerFile->append("    _");
                headerFile->append(enumDeclarationName);
                headerFile->append("Code_");
                headerFile->append(member->enumCase->name);
                if (i == 0)
                    headerFile->append(" = 1");
                headerFile->append(",\n");
                i++;
            }
        }
        headerFile->append("};\n\n");
    }
    headerFile->append("class ");
    headerFile->append(enumDeclarationName);
    headerFile->append(" : public Object {\npublic:\n    ");
    headerFile->append(enumDeclarationName);
    headerFile->append("(_");
    headerFile->append(enumDeclarationName);
    headerFile->append("Code errorCode)\n    : errorCode(errorCode), errorInfo(0) {}\n\n");
    if (enumDeclaration->members != nullptr) {
        EnumMember* member = nullptr;
        size_t _enumDeclaration_length = enumDeclaration->members->length();
        for (size_t _i = 0; _i < _enumDeclaration_length; _i++) {
            member = *(*enumDeclaration->members)[_i];
            {
                if (member->parameterClause) {
                    headerFile->append("    ");
                    headerFile->append(enumDeclarationName);
                    headerFile->append("(_");
                    headerFile->append(enumDeclarationName);
                    headerFile->append("_");
                    headerFile->append(member->enumCase->name);
                    headerFile->append("* ");
                    headerFile->append(member->enumCase->name);
                    headerFile->append(")\n    : errorCode(_");
                    headerFile->append(enumDeclarationName);
                    headerFile->append("Code_");
                    headerFile->append(member->enumCase->name);
                    headerFile->append("), errorInfo(");
                    headerFile->append(member->enumCase->name);
                    headerFile->append(") {}\n\n");
                }
            }
        }
    }
    headerFile->append("    long _getErrorCode();\n    void* _getErrorInfo();\n\n");
    if (enumDeclaration->members != nullptr) {
        EnumMember* member = nullptr;
        size_t _enumDeclaration_length = enumDeclaration->members->length();
        for (size_t _i = 0; _i < _enumDeclaration_length; _i++) {
            member = *(*enumDeclaration->members)[_i];
            {
                if (member->parameterClause) {
                    headerFile->append("    _");
                    headerFile->append(enumDeclarationName);
                    headerFile->append("_");
                    headerFile->append(member->enumCase->name);
                    headerFile->append("* get_");
                    headerFile->append(member->enumCase->name);
                    headerFile->append("();\n");
                }
            }
        }
    }
    headerFile->append("\nprivate:\n    _");
    headerFile->append(enumDeclarationName);
    headerFile->append("Code errorCode;\n    void* errorInfo;\n};");
}

bool HeaderVisitor::openClassDeclaration(ClassDeclaration* classDeclaration) {
    headerFile->append("\n\nclass ");
    headerFile->append(classDeclaration->name);
    if (classDeclaration->body == nullptr) {
        headerFile->append(";");
        return false;
    }
    headerFile->append(" : public ");
    if (classDeclaration->typeInheritanceClause != nullptr) {
        int i = 0;
        Inheritance* inheritance = nullptr;
        size_t _classDeclaration_length = classDeclaration->typeInheritanceClause->inheritances->length();
        for (size_t _i = 0; _i < _classDeclaration_length; _i++) {
            inheritance = *(*classDeclaration->typeInheritanceClause->inheritances)[_i];
            {
                if (i > 0)
                    headerFile->append(", ");
                headerFile->append(inheritance->type->name);
                i++;
            }
        }
    }
    else {
        headerFile->append("Object");
    }
    headerFile->append(" {\n");
    headerFile->append("public:");
    return true;
}

void HeaderVisitor::closeClassDeclaration(ClassDeclaration* classDeclaration) {
    headerFile->append("\n");
    if (classDeclaration->typeInheritanceClause != nullptr) {
        headerFile->append("\n    virtual bool _is");
        headerFile->append(classDeclaration->name);
        headerFile->append("();");
    }
    {
        _Region _region; _Page* _p = _region.get();
        _Array<string>* derivedClasses = new(_p) _Array<string>();
        collectDerivedClasses(derivedClasses, classDeclaration->name);
        string* derivedClass = nullptr;
        size_t _derivedClasses_length = derivedClasses->length();
        for (size_t _i = 0; _i < _derivedClasses_length; _i++) {
            derivedClass = *(*derivedClasses)[_i];
            {
                headerFile->append("\n    virtual bool _is");
                headerFile->append(derivedClass);
                headerFile->append("();");
            }
        }
    }
    headerFile->append("\n};");
}

bool HeaderVisitor::openConstructorDeclaration(ConstructorDeclaration* constructorDeclaration) {
    if (!constructorDeclaration->parent->parent->parent->_isClassDeclaration())
        return false;
    string* classDeclarationName = ((ClassDeclaration*)constructorDeclaration->parent->parent->parent)->name;
    headerFile->append(classDeclarationName);
    headerFile->append("(");
    return true;
}

bool HeaderVisitor::openCodeBlock(CodeBlock* codeBlock) {
    return false;
}

bool HeaderVisitor::openIdentifierInitializer(IdentifierInitializer* identifierInitializer) {
    if (identifierInitializer->parent->_isAdditionalInitializer())
        headerFile->append("; ");
    return true;
}

bool HeaderVisitor::openFunctionSignature(FunctionSignature* functionSignature) {
    FunctionDeclaration* functionDeclaration = (FunctionDeclaration*)functionSignature->parent;
    if (functionDeclaration->modifiers != nullptr) {
        Modifier* modifier = nullptr;
        size_t _functionDeclaration_length = functionDeclaration->modifiers->length();
        for (size_t _i = 0; _i < _functionDeclaration_length; _i++) {
            modifier = *(*functionDeclaration->modifiers)[_i];
            {
                if (modifier->_isStaticWord())
                    headerFile->append("static ");
                else
                    headerFile->append("virtual ");
            }
        }
    }
    else {
        headerFile->append("virtual ");
    }
    if (functionSignature->result == nullptr) {
        if (functionSignature->throwsClause == nullptr) {
            headerFile->append("void");
        }
        else {
            appendCppType(headerFile, functionSignature->throwsClause->throwsType);
        }
    }
    else {
        if (functionSignature->throwsClause != nullptr) {
            headerFile->append("_Result<");
            if (hasArrayPostfix(functionSignature->result->resultType)) {
                headerFile->append("_Array<");
                appendCppTypeName(headerFile, functionSignature->result->resultType);
                headerFile->append(">");
            }
            else {
                appendCppTypeName(headerFile, functionSignature->result->resultType);
            }
            headerFile->append(", ");
            appendCppTypeName(headerFile, functionSignature->throwsClause->throwsType);
            headerFile->append(">");
        }
        else {
            if (hasArrayPostfix(functionSignature->result->resultType)) {
                headerFile->append("_Array<");
                appendCppTypeName(headerFile, functionSignature->result->resultType);
                headerFile->append(">*");
            }
            else {
                appendCppTypeName(headerFile, functionSignature->result->resultType);
                if (isClass(functionSignature->result->resultType->name)) {
                    headerFile->append("*");
                }
            }
        }
    }
    headerFile->append(" ");
    headerFile->append(((FunctionDeclaration*)functionSignature->parent)->name);
    headerFile->append("(");
    if (functionSignature->result != nullptr) {
        if (isClass(functionSignature->result->resultType->name)) {
            LifeTime* lifeTime = functionSignature->result->resultType->lifeTime;
            if ((lifeTime == nullptr) || !(lifeTime->_isReference())) {
                headerFile->append("_Page* _rp");
                if ((functionSignature->parameterClause->parameters) || (functionSignature->throwsClause)) {
                    headerFile->append(", ");
                }
            }
        }
    }
    if (functionSignature->throwsClause != nullptr) {
        headerFile->append("_Page* _ep");
        if (functionSignature->parameterClause->parameters) {
            headerFile->append(", ");
        }
    }
    return true;
}

void HeaderVisitor::closeParameterClause(ParameterClause* parameterClause) {
    headerFile->append(")");
}

bool HeaderVisitor::openConstParameter(ConstParameter* constParameter) {
    writeParameter(constParameter->name, constParameter->parameterType);
    return false;
}

void HeaderVisitor::writeParameter(string* name, Type* parameterType) {
    if (parameterType->parent->_isParameter()) {
        Parameter* parameter = (Parameter*)parameterType->parent;
        if (parameter->parent->_isParameterClause()) {
            _Array<Parameter>* parameters = ((ParameterClause*)parameter->parent)->parameters;
            if (parameter != *(*parameters)[0])
                headerFile->append(", ");
            parameterType->accept(this);
            headerFile->append(" ");
            headerFile->append(name);
        }
    }
}

bool HeaderVisitor::isClass(string* name) {
    if (name->equals("string") || name->equals("VarString") || name->equals("File") || name->equals("Directory") || name->equals("Path") || name->equals("DirectoryError") || name->equals("FileError") || name->equals("ParserError") || name->equals("CppError") || name->equals("CompilerError"))
        return true;
    string* className = nullptr;
    size_t _classes_length = classes->length();
    for (size_t _i = 0; _i < _classes_length; _i++) {
        className = *(*classes)[_i];
        {
            if (className->equals(name)) {
                return true;
            }
        }
    }
    return false;
}

void HeaderVisitor::closeConstParameter(ConstParameter* constParameter) {
    headerFile->append(constParameter->name);
}

bool HeaderVisitor::openVarParameter(VarParameter* varParameter) {
    writeParameter(varParameter->name, varParameter->parameterType);
    return false;
}

void HeaderVisitor::closeVarParameter(VarParameter* varParameter) {
    headerFile->append(varParameter->name);
}

bool HeaderVisitor::openEnumMember(EnumMember* enumMember) {
    if (!enumMember->parent->_isEnumDeclaration())
        return false;
    if (enumMember->parameterClause) {
        headerFile->append("\nclass _");
        headerFile->append(((EnumDeclaration*)enumMember->parent)->name);
        headerFile->append("_");
        headerFile->append(enumMember->enumCase->name);
        headerFile->append(" : public Object {\npublic:\n    _");
        headerFile->append(((EnumDeclaration*)enumMember->parent)->name);
        headerFile->append("_");
        headerFile->append(enumMember->enumCase->name);
        headerFile->append("(");
    }
    return true;
}

void HeaderVisitor::closeEnumMember(EnumMember* enumMember) {
    if (enumMember->parameterClause != nullptr) {
        headerFile->append(";\n\n");
        if (enumMember->parameterClause->parameters != nullptr) {
            size_t pos = 0;
            Parameter* parameter = nullptr;
            size_t _enumMember_length = enumMember->parameterClause->parameters->length();
            for (size_t _i = 0; _i < _enumMember_length; _i++) {
                parameter = *(*enumMember->parameterClause->parameters)[_i];
                {
                    if (parameter->_isConstParameter()) {
                        headerFile->append("    ");
                        appendCppType(headerFile, ((ConstParameter*)parameter)->parameterType);
                        headerFile->append(" ");
                        headerFile->append(((ConstParameter*)parameter)->name);
                        headerFile->append(";\n");
                    }
                    pos++;
                }
            }
        }
        headerFile->append("};\n");
    }
}

bool HeaderVisitor::openClassMember(ClassMember* classMember) {
    headerFile->append("\n    ");
    return true;
}

void HeaderVisitor::closeClassMember(ClassMember* classMember) {
    headerFile->append(";");
}

bool HeaderVisitor::openIdentifierPattern(IdentifierPattern* identifierPattern) {
    if (identifierPattern->annotationForType != nullptr) {
        identifierPattern->annotationForType->accept(this);
        headerFile->append(" ");
    }
    headerFile->append(identifierPattern->identifier);
    return false;
}

bool HeaderVisitor::openType(Type* type) {
    if ((type->parent->_isFunctionResult()) || (type->parent->_isInheritance()) || (type->parent->_isThrowsClause()))
        return false;
    if (hasArrayPostfix(type)) {
        headerFile->append("_Array<");
    }
    appendCppTypeName(headerFile, type);
    return true;
}

void HeaderVisitor::closeType(Type* type) {
    if (hasArrayPostfix(type))
        headerFile->append(">*");
    if (isClass(type->name) && !hasArrayPostfix(type) && !type->parent->_isConstructorCall()) {
        headerFile->append("*");
    }
}

void HeaderVisitor::buildMainHeaderFileString(Program* program) {
    if (mainHeaderFile != nullptr)
        mainHeaderFile->_getPage()->clear();
    mainHeaderFile = new(mainHeaderFile == nullptr ? _getPage()->allocateExclusivePage() : mainHeaderFile->_getPage()) VarString();
    mainHeaderFile->append("#ifndef __scaly__");
    mainHeaderFile->append(program->name);
    mainHeaderFile->append("__\n#define __scaly__");
    mainHeaderFile->append(program->name);
    mainHeaderFile->append("__\n\n#include \"Scaly.h\"\n");
    if (program->compilationUnits != nullptr) {
        CompilationUnit* compilationUnit = nullptr;
        size_t _program_length = program->compilationUnits->length();
        for (size_t _i = 0; _i < _program_length; _i++) {
            compilationUnit = *(*program->compilationUnits)[_i];
            {
                _Region _region; _Page* _p = _region.get();
                string* fileName = getFileName(_p, compilationUnit);
                if (fileName != nullptr) {
                    _Region _region; _Page* _p = _region.get();
                    string* name = Path::getFileNameWithoutExtension(_p, fileName);
                    mainHeaderFile->append("#include \"");
                    mainHeaderFile->append(name);
                    mainHeaderFile->append(".h\"\n");
                }
            }
        }
    }
    mainHeaderFile->append("\nusing namespace scaly;\nnamespace ");
    mainHeaderFile->append(program->name);
    mainHeaderFile->append(" {\nFileError* _main(_Page* page, _Array<string>* arguments);\n}\n\n#endif // __scaly__");
    mainHeaderFile->append(program->name);
    mainHeaderFile->append("__\n");
}

void HeaderVisitor::collectInheritances(Program* program) {
    CompilationUnit* compilationUnit = nullptr;
    size_t _program_length = program->compilationUnits->length();
    for (size_t _i = 0; _i < _program_length; _i++) {
        compilationUnit = *(*program->compilationUnits)[_i];
        collectInheritancesInCompilationUnit(compilationUnit);
    }
}

bool HeaderVisitor::_isHeaderVisitor() { return (true); }


}
