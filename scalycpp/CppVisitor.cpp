#include "scalycpp.h"
using namespace scaly;
namespace scalycpp {

bool CppVisitor::hasArrayPostfix(Type* type) {
    if (type->postfixes == nullptr)
        return false;
    _Array<TypePostfix>* postfixes = type->postfixes;
    TypePostfix* typePostfix = *(*postfixes)[0];
    if (typePostfix->_isIndexedType())
        return true;
    return false;
}

void CppVisitor::appendCppType(VarString* s, Type* type) {
    if (hasArrayPostfix(type)) {
        s->append("_Array<");
        appendCppTypeName(s, type);
        s->append(">*");
    }
    else {
        appendCppTypeName(s, type);
        if (isClass(type->name)) {
            s->append("*");
        }
    }
}

void CppVisitor::appendCppTypeName(VarString* s, Type* type) {
    string* typeName = type->name;
    if (typeName->equals("number")) {
        s->append("size_t");
        return;
    }
    else {
        if (typeName->equals("char")) {
            s->append("char");
            return;
        }
    }
    s->append(typeName);
}

void CppVisitor::collectDerivedClasses(_Array<string>* derivedClasses, string* className) {
    Inherits* inherit = nullptr;
    size_t _inherits_length = inherits->length();
    for (size_t _i = 0; _i < _inherits_length; _i++) {
        inherit = *(*inherits)[_i];
        {
            if (inherit->name->equals(className))
                appendDerivedClasses(derivedClasses, inherit->inheritors);
        }
    }
}

void CppVisitor::appendDerivedClasses(_Array<string>* derivedClasses, _Array<string>* inheritors) {
    string* inheritor = nullptr;
    size_t _inheritors_length = inheritors->length();
    for (size_t _i = 0; _i < _inheritors_length; _i++) {
        inheritor = *(*inheritors)[_i];
        {
            derivedClasses->push(inheritor);
            collectDerivedClasses(derivedClasses, inheritor);
        }
    }
}

void CppVisitor::collectInheritancesInModule(Module* module) {
    if (module->statements != nullptr) {
        Statement* statement = nullptr;
        size_t _module_length = module->statements->length();
        for (size_t _i = 0; _i < _module_length; _i++) {
            statement = *(*module->statements)[_i];
            {
                if (statement->_isClassDeclaration()) {
                    ClassDeclaration* classDeclaration = (ClassDeclaration*)statement;
                    classes->push(classDeclaration->name);
                    if (classDeclaration->typeInheritanceClause != nullptr) {
                        TypeInheritanceClause* inheritanceClause = classDeclaration->typeInheritanceClause;
                        Inheritance* inheritance = nullptr;
                        size_t _inheritanceClause_length = inheritanceClause->inheritances->length();
                        for (size_t _i = 0; _i < _inheritanceClause_length; _i++) {
                            inheritance = *(*inheritanceClause->inheritances)[_i];
                            {
                                registerInheritance(classDeclaration->name, inheritance->type->name);
                            }
                        }
                    }
                }
            }
        }
    }
}

void CppVisitor::registerInheritance(string* className, string* baseName) {
    Inherits* inherit = nullptr;
    Inherits* inh = nullptr;
    size_t _inherits_length = inherits->length();
    for (size_t _i = 0; _i < _inherits_length; _i++) {
        inh = *(*inherits)[_i];
        {
            if (inh->name->equals(baseName)) {
                inherit = inh;
            }
        }
    }
    if (inherit == nullptr) {
        Inherits* newInherit = new(inherits->_getPage()) Inherits(baseName);
        inherit = newInherit;
        inherits->push(inherit);
    }
    inherit->inheritors->push(className);
}

bool CppVisitor::_isCppVisitor() { return (true); }

bool CppVisitor::_isHeaderVisitor() { return (false); }
bool CppVisitor::_isSourceVisitor() { return (false); }

Inherits::Inherits(string* className) {
    name = new(_getPage()) string(className);
    inheritors = new(_getPage()->allocateExclusivePage()) _Array<string>();
}

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

bool HeaderVisitor::openModule(Module* module) {
    _Region _region; _Page* _p = _region.get();
    if (!(module->parent->_isProgram()))
        return false;
    string* programName = ((Program*)module->parent)->name;
    string* fileName = getFileName(_p, module);
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

void HeaderVisitor::closeModule(Module* module) {
    _Region _region; _Page* _p = _region.get();
    if (!(module->parent)->_isProgram())
        return;
    string* programName = ((Program*)module->parent)->name;
    VarString* outputFilePath = new(_p) VarString(directory);
    outputFilePath->append('/');
    string* fileName = getFileName(_p, module);
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
    if (program->modules != nullptr) {
        Module* module = nullptr;
        size_t _program_length = program->modules->length();
        for (size_t _i = 0; _i < _program_length; _i++) {
            module = *(*program->modules)[_i];
            {
                _Region _region; _Page* _p = _region.get();
                string* fileName = getFileName(_p, module);
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
    Module* module = nullptr;
    size_t _program_length = program->modules->length();
    for (size_t _i = 0; _i < _program_length; _i++) {
        module = *(*program->modules)[_i];
        collectInheritancesInModule(module);
    }
}

bool HeaderVisitor::_isHeaderVisitor() { return (true); }

SourceVisitor::SourceVisitor(string* outputDirectory) {
    directory = outputDirectory;
    sourceFile = nullptr;
    inherits = new(_getPage()->allocateExclusivePage()) _Array<Inherits>();
    classes = new(_getPage()->allocateExclusivePage()) _Array<string>();
}

bool SourceVisitor::openProgram(Program* program) {
    _Region _region; _Page* _p = _region.get();
    VarString* projectFilePath = new(_p) VarString(directory);
    if (projectFilePath == nullptr || projectFilePath->equals(""))
        projectFilePath = new(_p) VarString(".");
    projectFilePath->append("/");
    projectFilePath->append(program->name);
    projectFilePath->append(".project");
    VarString* projectFile = buildProjectFileString(_p, program);
    auto _File_error = File::writeFromString(_p, projectFilePath, projectFile);
    if (_File_error) { switch (_File_error->_getErrorCode()) {
        default: {
        return false;
        }
    } }
    collectInheritances(program);
    return true;
}

bool SourceVisitor::openModule(Module* module) {
    if (!(module->parent->_isProgram()))
        return false;
    string* programName = ((Program*)module->parent)->name;
    if (sourceFile != nullptr)
        sourceFile->_getPage()->clear();
    sourceFile = new(sourceFile == nullptr ? _getPage()->allocateExclusivePage() : sourceFile->_getPage()) VarString(0, 4096);
    sourceFile->append("#include \"");
    sourceFile->append(programName);
    sourceFile->append(".h\"\nusing namespace scaly;\n");
    if (module->statements != nullptr && isTopLevelFile(module)) {
        sourceFile->append("namespace scaly {\n\n");
        sourceFile->append("extern __thread _Page* __CurrentPage;\n");
        sourceFile->append("extern __thread _Task* __CurrentTask;\n\n");
        sourceFile->append("}\n\n");
        sourceFile->append("int main(int argc, char** argv) {\n");
        sourceFile->append("    // Allocate the root page for the main thread\n");
        sourceFile->append("    posix_memalign((void**)&__CurrentPage, _pageSize, _pageSize * _maxStackPages);\n");
        sourceFile->append("    if (!__CurrentPage)\n");
        sourceFile->append("        return -1;\n");
        sourceFile->append("    __CurrentPage->reset();\n");
        sourceFile->append("    _Task* task = new(__CurrentPage) _Task();\n");
        sourceFile->append("    __CurrentTask = task;\n\n");
        sourceFile->append("    // Collect the arguments into a string Vector\n");
        sourceFile->append("    _Array<string>* arguments = new(__CurrentPage) _Array<string>(argc - 1);\n");
        sourceFile->append("    for (int i = 1; i < argc; i++)\n");
        sourceFile->append("        arguments->push(new(__CurrentPage) string(argv[i]));\n\n");
        sourceFile->append("    // Call Scaly's top-level code\n");
        sourceFile->append("    auto _File_error = ");
        sourceFile->append(programName);
        sourceFile->append("::_main(__CurrentPage, arguments);\n    int ret = 0;\n\n    // Convert Scaly's error enum back to OS errno values\n");
        sourceFile->append("    if (_File_error) {\n        switch(_File_error->_getErrorCode()) {\n");
        sourceFile->append("            case _FileErrorCode_noSuchFileOrDirectory:\n                ret = ENOENT;\n");
        sourceFile->append("                break;\n            default:\n                ret = -1;\n                break;\n        }\n    }\n\n");
        sourceFile->append("    // Only for monitoring, debugging and stuff\n");
        sourceFile->append("    __CurrentTask->dispose();\n\n");
        sourceFile->append("    // Give back the return code of the top-level code\n");
        sourceFile->append("    return ret;\n");
        sourceFile->append("}\n\n");
    }
    sourceFile->append("namespace ");
    sourceFile->append(programName);
    sourceFile->append(" {\n\n");
    if ((module->statements != nullptr) && isTopLevelFile(module))
        sourceFile->append("FileError* _main(_Page* _ep,  _Array<string>* arguments) {\n    _Region _rp; _Page* _p = _rp.get();\n\n");
    return true;
}

void SourceVisitor::closeModule(Module* module) {
    _Region _region; _Page* _p = _region.get();
    if (!(module->parent)->_isProgram())
        return;
    VarString* outputFilePath = new(_p) VarString(directory);
    outputFilePath->append('/');
    string* fileName = getFileName(_p, module);
    if (fileName != nullptr) {
        _Region _region; _Page* _p = _region.get();
        string* name = Path::getFileNameWithoutExtension(_p, fileName);
        outputFilePath->append(name);
    }
    if (module->statements != nullptr && isTopLevelFile(module)) {
        size_t length = module->statements->length();
        if (length > 0) {
            Statement* statement = *(*module->statements)[length - 1];
            if (statement->_isSimpleExpression()) {
                SimpleExpression* simpleExpression = (SimpleExpression*)statement;
                PrimaryExpression* primaryExpression = simpleExpression->prefixExpression->expression->primaryExpression;
                if ((!(primaryExpression->_isReturnExpression())) && (!(primaryExpression->_isLiteralExpression())))
                    sourceFile->append("\n    return nullptr;\n");
            }
        }
        sourceFile->append("\n}\n");
    }
    sourceFile->append("\n}\n");
    VarString* sourceFilePath = new(_p) VarString(outputFilePath);
    sourceFilePath->append(".cpp");
    auto _File_error = File::writeFromString(_p, sourceFilePath, sourceFile);
    if (_File_error) { switch (_File_error->_getErrorCode()) {
        default: {
        return;
        }
    } }
}

bool SourceVisitor::openConstantDeclaration(ConstantDeclaration* constantDeclaration) {
    if (constantDeclaration->parent->parent->parent == nullptr)
        return true;
    if (constantDeclaration->parent->parent->parent->_isClassDeclaration())
        return false;
    return true;
}

bool SourceVisitor::openMutableDeclaration(MutableDeclaration* mutableDeclaration) {
    if (mutableDeclaration->parent->parent->parent->_isClassDeclaration())
        return false;
    return true;
}

bool SourceVisitor::openFunctionDeclaration(FunctionDeclaration* functionDeclaration) {
    if (functionDeclaration->body == nullptr)
        return false;
    return true;
}

bool SourceVisitor::openEnumDeclaration(EnumDeclaration* enumDeclaration) {
    string* enumDeclarationName = enumDeclaration->name;
    sourceFile->append("long ");
    sourceFile->append(enumDeclarationName);
    sourceFile->append("::_getErrorCode() {\n    return (long)errorCode;\n}\n\nvoid* ");
    sourceFile->append(enumDeclarationName);
    sourceFile->append("::_getErrorInfo() {\n    return errorInfo;\n}\n\n");
    return true;
}

void SourceVisitor::closeClassDeclaration(ClassDeclaration* classDeclaration) {
    _Region _region; _Page* _p = _region.get();
    if (classDeclaration->body == nullptr)
        return;
    if (classDeclaration->typeInheritanceClause != nullptr) {
        sourceFile->append("bool ");
        sourceFile->append(classDeclaration->name);
        sourceFile->append("::_is");
        sourceFile->append(classDeclaration->name);
        sourceFile->append("() { return (true); }\n\n");
    }
    _Array<string>* derivedClasses = new(_p) _Array<string>();
    collectDerivedClasses(derivedClasses, classDeclaration->name);
    string* derivedClass = nullptr;
    size_t _derivedClasses_length = derivedClasses->length();
    for (size_t _i = 0; _i < _derivedClasses_length; _i++) {
        derivedClass = *(*derivedClasses)[_i];
        {
            sourceFile->append("bool ");
            sourceFile->append(classDeclaration->name);
            sourceFile->append("::_is");
            sourceFile->append(derivedClass);
            sourceFile->append("() { return (false); }\n");
        }
    }
    if (derivedClasses->length() > 0)
        sourceFile->append("\n");
}

bool SourceVisitor::openConstructorDeclaration(ConstructorDeclaration* constructorDeclaration) {
    if (!constructorDeclaration->parent->parent->parent->_isClassDeclaration())
        return false;
    string* classDeclarationName = ((ClassDeclaration*)constructorDeclaration->parent->parent->parent)->name;
    sourceFile->append(classDeclarationName);
    sourceFile->append("::");
    sourceFile->append(classDeclarationName);
    sourceFile->append("(");
    return true;
}

void SourceVisitor::closeConstructorDeclaration(ConstructorDeclaration* constructorDeclaration) {
    sourceFile->append("\n");
}

bool SourceVisitor::openCodeBlock(CodeBlock* codeBlock) {
    if (codeBlock->parent->_isCodeBlock())
        indent(level(codeBlock) - 1);
    if (codeBlock->parent->_isCaseContent())
        indent(level(codeBlock) - 1);
    sourceFile->append("{\n");
    if (localAllocations(codeBlock)) {
        indent(level(codeBlock));
        sourceFile->append("_Region _region; _Page* _p = _region.get();\n");
    }
    return true;
}

bool SourceVisitor::localAllocations(CodeBlock* codeBlock) {
    if (codeBlock->statements != nullptr) {
        Statement* statement = nullptr;
        size_t _codeBlock_length = codeBlock->statements->length();
        for (size_t _i = 0; _i < _codeBlock_length; _i++) {
            statement = *(*codeBlock->statements)[_i];
            {
                BindingInitializer* bindingInitializer = nullptr;
                if (statement->_isConstantDeclaration())
                    bindingInitializer = ((ConstantDeclaration*)statement)->initializer;
                if (statement->_isMutableDeclaration())
                    bindingInitializer = ((MutableDeclaration*)statement)->initializer;
                if (isRootBinding(bindingInitializer))
                    return true;
            }
        }
    }
    return false;
}

FunctionCall* SourceVisitor::getFunctionCall(IdentifierInitializer* identifierInitializer) {
    if (identifierInitializer->initializer != nullptr) {
        Expression* expression = identifierInitializer->initializer->expression;
        if (expression->_isSimpleExpression()) {
            PostfixExpression* postfixExpression = ((SimpleExpression*)expression)->prefixExpression->expression;
            if (postfixExpression->primaryExpression->_isIdentifierExpression()) {
                if (postfixExpression->postfixes != nullptr) {
                    Postfix* postfix = nullptr;
                    size_t _postfixExpression_length = postfixExpression->postfixes->length();
                    for (size_t _i = 0; _i < _postfixExpression_length; _i++) {
                        postfix = *(*postfixExpression->postfixes)[_i];
                        {
                            if (postfix->_isFunctionCall()) {
                                return (FunctionCall*)postfix;
                            }
                        }
                    }
                }
            }
        }
    }
    return nullptr;
}

bool SourceVisitor::isCatchingFunctionCall(IdentifierInitializer* identifierInitializer) {
    if (catchesError(getFunctionCall(identifierInitializer)))
        return true;
    return false;
}

void SourceVisitor::closeCodeBlock(CodeBlock* codeBlock) {
    if (codeBlock->parent->_isFunctionDeclaration()) {
        FunctionDeclaration* functionDeclaration = (FunctionDeclaration*)codeBlock->parent;
        if ((functionDeclaration->signature->throwsClause != nullptr) && (functionDeclaration->signature->result == nullptr)) {
            indent(level(codeBlock));
            sourceFile->append("return nullptr;\n");
        }
    }
    indent(level(codeBlock) - 1);
    sourceFile->append("}\n");
    if (codeBlock->parent->_isFunctionDeclaration())
        sourceFile->append("\n");
}

bool SourceVisitor::openSimpleExpression(SimpleExpression* simpleExpression) {
    Statement* statement = (Statement*)simpleExpression;
    if (statement->parent->_isCodeBlock() || statement->parent->_isCaseContent() || statement->parent->_isModule())
        indent(level(simpleExpression));
    if (statement->parent->_isFunctionDeclaration()) {
        _Region _region; _Page* _p = _region.get();
        string* returnType = getReturnType(_p, statement);
        if (returnType != nullptr)
            prependReturn(simpleExpression);
    }
    if (statement->parent->_isCodeBlock()) {
        CodeBlock* block = (CodeBlock*)statement->parent;
        if (block->parent->_isFunctionDeclaration()) {
            if (*(*block->statements)[block->statements->length() - 1] == statement) {
                _Region _region; _Page* _p = _region.get();
                string* returnType = getReturnType(_p, statement);
                if (returnType != nullptr)
                    prependReturn(simpleExpression);
            }
        }
    }
    if (simpleExpression->binaryExpressions != nullptr) {
        BinaryExpression* binaryExpression = nullptr;
        size_t _simpleExpression_length = simpleExpression->binaryExpressions->length();
        for (size_t _i = 0; _i < _simpleExpression_length; _i++) {
            binaryExpression = *(*simpleExpression->binaryExpressions)[_i];
            {
                if (binaryExpression->_isTypeCast()) {
                    sourceFile->append("(");
                    sourceFile->append(((TypeCast*)binaryExpression)->objectType->name);
                    sourceFile->append("*)");
                    simpleExpression->prefixExpression->accept(this);
                    return false;
                }
            }
        }
    }
    if (simpleExpression->prefixExpression->expression->primaryExpression->_isIdentifierExpression()) {
        _Array<BinaryExpression>* binaryExpressions = simpleExpression->binaryExpressions;
        if (binaryExpressions != nullptr) {
            if (binaryExpressions->length() == 1) {
                BinaryExpression* binaryExpression = *(*binaryExpressions)[0];
                if (binaryExpression->_isAssignment()) {
                    Assignment* assignment = (Assignment*)binaryExpression;
                    if (assignment->parent->_isSimpleExpression()) {
                        SimpleExpression* simpleExpression = (SimpleExpression*)(assignment->parent);
                        PostfixExpression* leftSide = simpleExpression->prefixExpression->expression;
                        if (leftSide->primaryExpression->_isIdentifierExpression()) {
                            IdentifierExpression* memberExpression = (IdentifierExpression*)(leftSide->primaryExpression);
                            string* memberName = memberExpression->name;
                            ClassDeclaration* classDeclaration = getClassDeclaration(assignment);
                            if (classDeclaration != nullptr) {
                                if (isVariableObjectField(memberName, classDeclaration)) {
                                    if ((memberName != nullptr) && (!inConstructor(assignment))) {
                                        sourceFile->append("if (");
                                        sourceFile->append(memberName);
                                        sourceFile->append(" != nullptr)\n");
                                        this->indent(level(simpleExpression));
                                        sourceFile->append("    ");
                                        sourceFile->append(memberName);
                                        sourceFile->append("->_getPage()->clear();\n");
                                        this->indent(level(simpleExpression));
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        if (simpleExpression->parent->_isCodeBlock() || simpleExpression->parent->_isCaseContent() || simpleExpression->parent->_isModule()) {
            if (simpleExpression->prefixExpression->expression->postfixes != nullptr) {
                Postfix* postfix = nullptr;
                size_t _simpleExpression_length = simpleExpression->prefixExpression->expression->postfixes->length();
                for (size_t _i = 0; _i < _simpleExpression_length; _i++) {
                    postfix = *(*simpleExpression->prefixExpression->expression->postfixes)[_i];
                    {
                        if (postfix->_isFunctionCall()) {
                            if (((FunctionCall*)postfix)->catchClauses != nullptr) {
                                IdentifierExpression* identifierExpression = (IdentifierExpression*)simpleExpression->prefixExpression->expression->primaryExpression;
                                sourceFile->append("auto _");
                                sourceFile->append(identifierExpression->name);
                                sourceFile->append("_error = ");
                            }
                        }
                    }
                }
            }
        }
    }
    if (simpleExpression->parent->_isModule()) {
        if (simpleExpression->prefixExpression->expression->postfixes != nullptr) {
            Postfix* postfix = nullptr;
            size_t _simpleExpression_length = simpleExpression->prefixExpression->expression->postfixes->length();
            for (size_t _i = 0; _i < _simpleExpression_length; _i++) {
                postfix = *(*simpleExpression->prefixExpression->expression->postfixes)[_i];
                {
                    if (postfix->_isFunctionCall()) {
                        FunctionCall* functionCall = (FunctionCall*)postfix;
                        if (functionCall->catchClauses == nullptr) {
                            if (functionCall->parent->_isPostfixExpression()) {
                                PostfixExpression* postfixExpression = (PostfixExpression*)(functionCall->parent);
                                if (postfixExpression->primaryExpression->_isIdentifierExpression()) {
                                    IdentifierExpression* identifierExpression = (IdentifierExpression*)(postfixExpression->primaryExpression);
                                    if (identifierExpression->name->equals("print")) {
                                        sourceFile->append("{\n    auto _File_error = ");
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    return true;
}

void SourceVisitor::prependReturn(SimpleExpression* simpleExpression) {
    PrimaryExpression* expression = simpleExpression->prefixExpression->expression->primaryExpression;
    if ((!expression->_isReturnExpression()) && (!expression->_isThrowExpression()) && (!expression->_isWhileExpression()) && (!expression->_isDoExpression()))
        sourceFile->append("return ");
}

void SourceVisitor::closeSimpleExpression(SimpleExpression* simpleExpression) {
    PrimaryExpression* primaryExpression = simpleExpression->prefixExpression->expression->primaryExpression;
    if (primaryExpression->_isIfExpression() || primaryExpression->_isSwitchExpression() || primaryExpression->_isForExpression() || primaryExpression->_isWhileExpression())
        return;
    if (primaryExpression->_isIdentifierExpression()) {
        PostfixExpression* postfixExpression = simpleExpression->prefixExpression->expression;
        if (postfixExpression->postfixes != nullptr) {
            Postfix* postfix = nullptr;
            size_t _postfixExpression_length = postfixExpression->postfixes->length();
            for (size_t _i = 0; _i < _postfixExpression_length; _i++) {
                postfix = *(*postfixExpression->postfixes)[_i];
                {
                    if (postfix->_isFunctionCall()) {
                        if (catchesError((FunctionCall*)postfix))
                            return;
                    }
                }
            }
        }
    }
    if (simpleExpression->parent->_isCodeBlock() || simpleExpression->parent->_isCaseContent() || simpleExpression->parent->_isModule())
        sourceFile->append(";\n");
    if (simpleExpression->parent->_isModule()) {
        _Array<Postfix>* postfixes = simpleExpression->prefixExpression->expression->postfixes;
        if (postfixes != nullptr) {
            Postfix* postfix = nullptr;
            size_t _postfixes_length = postfixes->length();
            for (size_t _i = 0; _i < _postfixes_length; _i++) {
                postfix = *(*postfixes)[_i];
                {
                    if (postfix->_isFunctionCall()) {
                        FunctionCall* functionCall = (FunctionCall*)postfix;
                        if (functionCall->catchClauses == nullptr) {
                            if (functionCall->parent->_isPostfixExpression()) {
                                PostfixExpression* postfixExpression = (PostfixExpression*)(functionCall->parent);
                                if (postfixExpression->primaryExpression->_isIdentifierExpression()) {
                                    IdentifierExpression* identifierExpression = (IdentifierExpression*)(postfixExpression->primaryExpression);
                                    if (identifierExpression->name->equals("print")) {
                                        sourceFile->append("    if (_File_error)\n        return _File_error;\n}\n");
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}

bool SourceVisitor::openInitializer(Initializer* initializer) {
    sourceFile->append(" = ");
    return true;
}

bool SourceVisitor::openBindingInitializer(BindingInitializer* bindingInitializer) {
    if (bindingInitializer->parent->parent->_isCodeBlock() || bindingInitializer->parent->parent->_isCaseContent() || bindingInitializer->parent->parent->_isModule())
        indent(level(bindingInitializer));
    return true;
}

void SourceVisitor::closeBindingInitializer(BindingInitializer* bindingInitializer) {
    if (isCatchingFunctionCall(bindingInitializer->initializer))
        return;
    if (bindingInitializer->parent->parent->_isCodeBlock() || bindingInitializer->parent->parent->_isCaseContent() || bindingInitializer->parent->parent->_isModule())
        sourceFile->append(";\n");
}

bool SourceVisitor::openFunctionSignature(FunctionSignature* functionSignature) {
    string* functionName = ((FunctionDeclaration*)functionSignature->parent)->name;
    if (functionSignature->result == nullptr) {
        if (functionSignature->throwsClause == nullptr)
            sourceFile->append("void");
        else
            appendCppType(sourceFile, functionSignature->throwsClause->throwsType);
    }
    else {
        if (functionSignature->throwsClause != nullptr) {
            sourceFile->append("_Result<");
            if (hasArrayPostfix(functionSignature->result->resultType)) {
                sourceFile->append("_Array<");
                appendCppTypeName(sourceFile, functionSignature->result->resultType);
                sourceFile->append(">");
            }
            else {
                appendCppTypeName(sourceFile, functionSignature->result->resultType);
            }
            sourceFile->append(", ");
            appendCppTypeName(sourceFile, functionSignature->throwsClause->throwsType);
            sourceFile->append(">");
        }
        else {
            if (hasArrayPostfix(functionSignature->result->resultType)) {
                sourceFile->append("_Array<");
                appendCppTypeName(sourceFile, functionSignature->result->resultType);
                sourceFile->append(">*");
            }
            else {
                appendCppTypeName(sourceFile, functionSignature->result->resultType);
                if (isClass(functionSignature->result->resultType->name)) {
                    sourceFile->append("*");
                }
            }
        }
    }
    sourceFile->append(" ");
    if (functionSignature->parent->parent->parent->parent->_isClassDeclaration()) {
        sourceFile->append(((ClassDeclaration*)functionSignature->parent->parent->parent->parent)->name);
        sourceFile->append("::");
    }
    sourceFile->append(functionName);
    sourceFile->append("(");
    if (functionSignature->result != nullptr) {
        if (isClass(functionSignature->result->resultType->name)) {
            LifeTime* lifeTime = functionSignature->result->resultType->lifeTime;
            if ((lifeTime == nullptr) || !(lifeTime->_isReference())) {
                sourceFile->append("_Page* _rp");
                if ((functionSignature->parameterClause->parameters) || (functionSignature->throwsClause)) {
                    sourceFile->append(", ");
                }
            }
        }
    }
    if (functionSignature->throwsClause != nullptr) {
        sourceFile->append("_Page* _ep");
        if (functionSignature->parameterClause->parameters) {
            sourceFile->append(", ");
        }
    }
    return true;
}

bool SourceVisitor::openFunctionResult(FunctionResult* functionResult) {
    return false;
}

void SourceVisitor::closeParameterClause(ParameterClause* parameterClause) {
    sourceFile->append(") ");
}

bool SourceVisitor::openConstParameter(ConstParameter* constParameter) {
    writeParameter(constParameter->name, constParameter->parameterType);
    return false;
}

void SourceVisitor::writeParameter(string* name, Type* parameterType) {
    if (parameterType->parent->_isParameter()) {
        Parameter* parameter = (Parameter*)parameterType->parent;
        if (parameter->parent->_isParameterClause()) {
            _Array<Parameter>* parameters = ((ParameterClause*)parameter->parent)->parameters;
            if (parameter != *(*parameters)[0])
                sourceFile->append(", ");
            parameterType->accept(this);
            sourceFile->append(" ");
            sourceFile->append(name);
        }
    }
}

bool SourceVisitor::isClass(string* name) {
    if (name->equals("string") || name->equals("VarString") || name->equals("Number") || name->equals("File") || name->equals("Directory") || name->equals("Path") || name->equals("DirectoryError") || name->equals("FileError") || name->equals("ParserError") || name->equals("CppError") || name->equals("CompilerError")) {
        return true;
    }
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

bool SourceVisitor::openVarParameter(VarParameter* varParameter) {
    writeParameter(varParameter->name, varParameter->parameterType);
    return false;
}

bool SourceVisitor::openThrowsClause(ThrowsClause* throwsClause) {
    return false;
}

bool SourceVisitor::openEnumMember(EnumMember* enumMember) {
    if (!enumMember->parent->_isEnumDeclaration())
        return false;
    string* enumDeclarationName = ((EnumDeclaration*)enumMember->parent)->name;
    if (enumMember->parameterClause) {
        sourceFile->append("_");
        sourceFile->append(enumDeclarationName);
        sourceFile->append("_");
        sourceFile->append(enumMember->enumCase->name);
        sourceFile->append("::_");
        sourceFile->append(enumDeclarationName);
        sourceFile->append("_");
        sourceFile->append(enumMember->enumCase->name);
        sourceFile->append("(");
    }
    return true;
}

void SourceVisitor::closeEnumMember(EnumMember* enumMember) {
    if (enumMember->parameterClause != nullptr) {
        sourceFile->append("\n");
        if (enumMember->parameterClause->parameters != nullptr) {
            sourceFile->append(": ");
            size_t pos = 0;
            Parameter* parameter = nullptr;
            size_t _enumMember_length = enumMember->parameterClause->parameters->length();
            for (size_t _i = 0; _i < _enumMember_length; _i++) {
                parameter = *(*enumMember->parameterClause->parameters)[_i];
                {
                    if (parameter->_isConstParameter()) {
                        if (pos != 0)
                            sourceFile->append(", ");
                        sourceFile->append(((ConstParameter*)parameter)->name);
                        sourceFile->append("(");
                        sourceFile->append(((ConstParameter*)parameter)->name);
                        sourceFile->append(")");
                    }
                    pos++;
                }
            }
        }
        sourceFile->append(" { }\n\n_");
        string* enumDeclarationName = ((EnumDeclaration*)(enumMember->parent))->name;
        sourceFile->append(enumDeclarationName);
        sourceFile->append("_");
        sourceFile->append(enumMember->enumCase->name);
        sourceFile->append("* ");
        sourceFile->append(enumDeclarationName);
        sourceFile->append("::get_");
        sourceFile->append(enumMember->enumCase->name);
        sourceFile->append("() {\n    return (_");
        sourceFile->append(enumDeclarationName);
        sourceFile->append("_");
        sourceFile->append(enumMember->enumCase->name);
        sourceFile->append("*)errorInfo;\n}\n\n");
    }
}

size_t SourceVisitor::level(SyntaxNode* syntaxNode) {
    size_t level = 0;
    SyntaxNode* node = syntaxNode;
    while (node != nullptr) {
        if (node->_isCodeBlock() || node->_isModule() || node->_isForExpression() || node->_isCatchClause() || node->_isCurliedSwitchBody() || node->_isCaseContent())
            level++;
        if (node->_isClassDeclaration())
            level--;
        node = node->parent;
    }
    return level;
}

void SourceVisitor::indent(size_t level) {
    size_t i = 0;
    while (i < level) {
        sourceFile->append("    ");
        i++;
    }
}

bool SourceVisitor::openPrefixExpression(PrefixExpression* prefixExpression) {
    if (prefixExpression->prefixOperator != nullptr)
        sourceFile->append(prefixExpression->prefixOperator);
    return true;
}

bool SourceVisitor::openPostfixExpression(PostfixExpression* postfixExpression) {
    if (postfixExpression->postfixes != nullptr) {
        Postfix* postfix = nullptr;
        size_t _postfixExpression_length = postfixExpression->postfixes->length();
        for (size_t _i = 0; _i < _postfixExpression_length; _i++) {
            postfix = *(*postfixExpression->postfixes)[_i];
            {
                if (postfix->_isSubscript())
                    sourceFile->append("*(*");
            }
        }
    }
    return true;
}

bool SourceVisitor::openBinaryOperation(BinaryOperation* binaryOperation) {
    sourceFile->append(" ");
    sourceFile->append(binaryOperation->binaryOperator);
    sourceFile->append(" ");
    return true;
}

bool SourceVisitor::openAssignment(Assignment* assignment) {
    sourceFile->append(" = ");
    return true;
}

bool SourceVisitor::inConstructor(SyntaxNode* node) {
    if (node->_isConstructorDeclaration())
        return true;
    if (node->parent == nullptr)
        return false;
    return inConstructor(node->parent);
}

bool SourceVisitor::inThrow(SyntaxNode* node) {
    if (node->_isThrowExpression())
        return true;
    if (node->parent == nullptr)
        return false;
    return inThrow(node->parent);
}

ClassDeclaration* SourceVisitor::getClassDeclaration(SyntaxNode* node) {
    if (node->_isClassDeclaration())
        return (ClassDeclaration*)node;
    if (node->parent != nullptr) {
        ClassDeclaration* classDeclaration = getClassDeclaration(node->parent);
        return classDeclaration;
    }
    return nullptr;
}

bool SourceVisitor::isObjectField(string* memberName, ClassDeclaration* classDeclaration) {
    if (classDeclaration->body == nullptr)
        return false;
    if (classDeclaration->body->members == nullptr)
        return false;
    ClassMember* member = nullptr;
    size_t _classDeclaration_length = classDeclaration->body->members->length();
    for (size_t _i = 0; _i < _classDeclaration_length; _i++) {
        member = *(*classDeclaration->body->members)[_i];
        {
            BindingInitializer* bindingInitializer = nullptr;
            if (member->declaration->_isConstantDeclaration())
                bindingInitializer = ((ConstantDeclaration*)member->declaration)->initializer;
            if (member->declaration->_isMutableDeclaration())
                bindingInitializer = ((MutableDeclaration*)member->declaration)->initializer;
            if (bindingInitializer == nullptr)
                continue;
            IdentifierPattern* identifierPattern = bindingInitializer->initializer->pattern;
            if (identifierPattern->identifier->equals(memberName)) {
                if (identifierPattern->annotationForType != nullptr) {
                    if (isClass(identifierPattern->annotationForType->annotationForType->name))
                        return true;
                    else
                        return false;
                }
            }
        }
    }
    if (classDeclaration->typeInheritanceClause != nullptr) {
        _Array<Inheritance>* inheritances = classDeclaration->typeInheritanceClause->inheritances;
        if (inheritances != nullptr) {
            Inheritance* inheritance = nullptr;
            size_t _inheritances_length = inheritances->length();
            for (size_t _i = 0; _i < _inheritances_length; _i++) {
                inheritance = *(*inheritances)[_i];
                {
                    _Region _region; _Page* _p = _region.get();
                    _Array<ClassDeclaration>* baseClassDeclarations = findClassDeclarations(_p, classDeclaration, inheritance->type->name);
                    ClassDeclaration* baseClassDeclaration = nullptr;
                    size_t _baseClassDeclarations_length = baseClassDeclarations->length();
                    for (size_t _i = 0; _i < _baseClassDeclarations_length; _i++) {
                        baseClassDeclaration = *(*baseClassDeclarations)[_i];
                        {
                            if (isObjectField(memberName, baseClassDeclaration))
                                return true;
                        }
                    }
                }
            }
        }
    }
    return false;
}

bool SourceVisitor::isVariableObjectField(string* memberName, ClassDeclaration* classDeclaration) {
    if (classDeclaration->body == nullptr)
        return false;
    if (classDeclaration->body->members == nullptr)
        return false;
    ClassMember* member = nullptr;
    size_t _classDeclaration_length = classDeclaration->body->members->length();
    for (size_t _i = 0; _i < _classDeclaration_length; _i++) {
        member = *(*classDeclaration->body->members)[_i];
        {
            BindingInitializer* bindingInitializer = nullptr;
            if (member->declaration->_isMutableDeclaration())
                bindingInitializer = ((MutableDeclaration*)member->declaration)->initializer;
            if (bindingInitializer == nullptr)
                continue;
            IdentifierPattern* identifierPattern = bindingInitializer->initializer->pattern;
            if (identifierPattern->identifier->equals(memberName)) {
                if (identifierPattern->annotationForType != nullptr) {
                    if (isClass(identifierPattern->annotationForType->annotationForType->name))
                        return true;
                    else
                        return false;
                }
            }
        }
    }
    if (classDeclaration->typeInheritanceClause != nullptr) {
        _Array<Inheritance>* inheritances = classDeclaration->typeInheritanceClause->inheritances;
        if (inheritances != nullptr) {
            Inheritance* inheritance = nullptr;
            size_t _inheritances_length = inheritances->length();
            for (size_t _i = 0; _i < _inheritances_length; _i++) {
                inheritance = *(*inheritances)[_i];
                {
                    _Region _region; _Page* _p = _region.get();
                    _Array<ClassDeclaration>* baseClassDeclarations = findClassDeclarations(_p, classDeclaration, inheritance->type->name);
                    ClassDeclaration* baseClassDeclaration = nullptr;
                    size_t _baseClassDeclarations_length = baseClassDeclarations->length();
                    for (size_t _i = 0; _i < _baseClassDeclarations_length; _i++) {
                        baseClassDeclaration = *(*baseClassDeclarations)[_i];
                        {
                            if (isVariableObjectField(memberName, baseClassDeclaration))
                                return true;
                        }
                    }
                }
            }
        }
    }
    return false;
}

_Array<ClassDeclaration>* SourceVisitor::findClassDeclarations(_Page* _rp, SyntaxNode* node, string* name) {
    Program* program = nullptr;
    while (node != nullptr) {
        if (node->_isProgram()) {
            program = (Program*)node;
            break;
        }
        node = node->parent;
    }
    _Array<ClassDeclaration>* classDeclarations = new(_rp) _Array<ClassDeclaration>();
    if (program->modules != nullptr) {
        Module* module = nullptr;
        size_t _program_length = program->modules->length();
        for (size_t _i = 0; _i < _program_length; _i++) {
            module = *(*program->modules)[_i];
            {
                if (module->statements != nullptr) {
                    Statement* statement = nullptr;
                    size_t _module_length = module->statements->length();
                    for (size_t _i = 0; _i < _module_length; _i++) {
                        statement = *(*module->statements)[_i];
                        {
                            if (statement->_isClassDeclaration()) {
                                ClassDeclaration* classDeclaration = (ClassDeclaration*)statement;
                                if (classDeclaration->name->equals(name))
                                    classDeclarations->push(classDeclaration);
                            }
                        }
                    }
                }
            }
        }
    }
    return classDeclarations;
}

bool SourceVisitor::openTypeQuery(TypeQuery* typeQuery) {
    sourceFile->append("->_is");
    return true;
}

bool SourceVisitor::openCatchClause(CatchClause* catchClause) {
    if (!catchClause->parent->_isFunctionCall())
        return false;
    FunctionCall* functionCall = (FunctionCall*)(catchClause->parent);
    {
        BindingInitializer* bindingInitializer = getBindingInitializer(functionCall);
        if (bindingInitializer != nullptr) {
            IdentifierPattern* identifierPattern = bindingInitializer->initializer->pattern;
            if (*(*functionCall->catchClauses)[0] == catchClause) {
                sourceFile->append(";\n");
                indent(level(catchClause) - 1);
                identifierPattern->annotationForType->accept(this);
                sourceFile->append(" ");
                sourceFile->append(identifierPattern->identifier);
                sourceFile->append(" = nullptr;\n");
                indent(level(catchClause) - 1);
                sourceFile->append("if (_");
                sourceFile->append(identifierPattern->identifier);
                sourceFile->append("_result.succeeded()) {\n");
                indent(level(catchClause) - 1);
                sourceFile->append("    ");
                sourceFile->append(identifierPattern->identifier);
                sourceFile->append(" = _");
                sourceFile->append(identifierPattern->identifier);
                sourceFile->append("_result.getResult();\n");
                indent(level(catchClause) - 1);
                sourceFile->append("}\n");
            }
            indent(level(catchClause) - 1);
            sourceFile->append("else");
            string* errorType = getErrorType(catchClause);
            if (catchClause->catchPattern->_isIdentifierCatchPattern()) {
                IdentifierCatchPattern* identifierCatchPattern = (IdentifierCatchPattern*)(catchClause->catchPattern);
                sourceFile->append(" if (_");
                sourceFile->append(identifierPattern->identifier);
                sourceFile->append("_result._getErrorCode() == _");
                sourceFile->append(identifierCatchPattern->name);
                sourceFile->append("Code_");
                if (errorType != nullptr)
                    sourceFile->append(errorType);
                sourceFile->append(")");
            }
            sourceFile->append(" {\n");
            if (catchClause->bindingPattern != nullptr) {
                TuplePattern* bindingPattern = catchClause->bindingPattern;
                if (bindingPattern->elements != nullptr) {
                    if (catchClause->catchPattern->_isWildCardCatchPattern()) {
                        if (bindingPattern->elements->length() > 0) {
                            TuplePatternElement* element = *(*bindingPattern->elements)[0];
                            if (element->pattern->_isIdentifierPattern()) {
                                IdentifierPattern* pattern = (IdentifierPattern*)(element->pattern);
                                indent(level(catchClause));
                                sourceFile->append("auto ");
                                sourceFile->append(pattern->identifier);
                                sourceFile->append(" = _");
                                sourceFile->append(identifierPattern->identifier);
                                sourceFile->append("_result.getError();\n");
                            }
                        }
                    }
                    if (catchClause->catchPattern->_isIdentifierCatchPattern()) {
                        TuplePatternElement* element = nullptr;
                        size_t _bindingPattern_length = bindingPattern->elements->length();
                        for (size_t _i = 0; _i < _bindingPattern_length; _i++) {
                            element = *(*bindingPattern->elements)[_i];
                            {
                                indent(level(catchClause));
                                element->accept(this);
                                sourceFile->append(" = _");
                                sourceFile->append(identifierPattern->identifier);
                                sourceFile->append("_result.getError()->get_");
                                if (errorType != nullptr)
                                    sourceFile->append(errorType);
                                sourceFile->append("()->");
                                if (element->pattern->_isIdentifierPattern()) {
                                    sourceFile->append(((IdentifierPattern*)element->pattern)->identifier);
                                }
                                sourceFile->append(";");
                                sourceFile->append("\n");
                            }
                        }
                    }
                }
            }
            indent(level(catchClause));
            if (catchClause->expression->_isSimpleExpression()) {
                SimpleExpression* simpleExpression = (SimpleExpression*)(catchClause->expression);
                PrimaryExpression* primaryExpression = simpleExpression->prefixExpression->expression->primaryExpression;
                if ((!primaryExpression->_isReturnExpression()) && (!primaryExpression->_isBreakExpression()) && (!primaryExpression->_isThrowExpression())) {
                    sourceFile->append(identifierPattern->identifier);
                    sourceFile->append(" = ");
                }
            }
            catchClause->expression->accept(this);
            if (catchClause->expression->_isSimpleExpression())
                sourceFile->append(";\n");
            indent(level(catchClause) - 1);
            sourceFile->append("}\n");
        }
    }
    {
        IdentifierExpression* identifierExpression = getIdentifierExpression((PostfixExpression*)(functionCall->parent));
        if (identifierExpression != nullptr) {
            if (*(*functionCall->catchClauses)[0] == catchClause) {
                sourceFile->append(";\n");
                indent(level(catchClause) - 1);
                sourceFile->append("if (_");
                sourceFile->append(identifierExpression->name);
                sourceFile->append("_error) { switch (_");
                sourceFile->append(identifierExpression->name);
                sourceFile->append("_error->_getErrorCode()) {\n");
            }
            indent(level(catchClause));
            if (catchClause->catchPattern->_isIdentifierCatchPattern()) {
                sourceFile->append("case _");
                IdentifierCatchPattern* identifierCatchPattern = (IdentifierCatchPattern*)(catchClause->catchPattern);
                sourceFile->append(identifierCatchPattern->name);
                sourceFile->append("Code_");
                if (identifierCatchPattern->member != nullptr) {
                    sourceFile->append(identifierCatchPattern->member->member);
                    sourceFile->append(": {\n");
                    indent(level(catchClause) - 1);
                    sourceFile->append("    _");
                    sourceFile->append(identifierCatchPattern->name);
                    sourceFile->append("_");
                    sourceFile->append(identifierCatchPattern->member->member);
                    sourceFile->append("* ");
                    if (catchClause->bindingPattern != nullptr) {
                        TuplePattern* bindingPattern = catchClause->bindingPattern;
                        if (bindingPattern->elements != nullptr) {
                            if (bindingPattern->elements->length() > 0) {
                                TuplePatternElement* element = *(*bindingPattern->elements)[0];
                                if (element->pattern->_isIdentifierPattern()) {
                                    IdentifierPattern* pattern = (IdentifierPattern*)(element->pattern);
                                    sourceFile->append(pattern->identifier);
                                    sourceFile->append(" = _");
                                    sourceFile->append(identifierExpression->name);
                                    sourceFile->append("_error->get_");
                                    sourceFile->append(identifierCatchPattern->member->member);
                                    sourceFile->append("();");
                                }
                            }
                        }
                    }
                    sourceFile->append("\n");
                }
            }
            if (catchClause->catchPattern->_isWildCardCatchPattern()) {
                sourceFile->append("default: {\n");
            }
            indent(level(catchClause));
            catchClause->expression->accept(this);
            {
                bool insertBreak = true;
                if (catchClause->expression->_isSimpleExpression()) {
                    SimpleExpression* simpleExpression = (SimpleExpression*)catchClause->expression;
                    if (simpleExpression->prefixExpression->expression->primaryExpression->_isReturnExpression()) {
                        sourceFile->append(";\n");
                        insertBreak = false;
                    }
                }
                if (insertBreak) {
                    indent(level(catchClause));
                    sourceFile->append("break;\n");
                }
            }
            indent(level(catchClause));
            sourceFile->append("}\n");
            if (*(*functionCall->catchClauses)[functionCall->catchClauses->length() - 1] == catchClause) {
                indent(level(catchClause) - 1);
                sourceFile->append("} }\n");
            }
        }
    }
    return false;
}

IdentifierExpression* SourceVisitor::getIdentifierExpression(PostfixExpression* postfixExpression) {
    if (postfixExpression->parent->parent->parent->_isCodeBlock() || postfixExpression->parent->parent->parent->_isModule()) {
        if (postfixExpression->parent->_isPrefixExpression()) {
            PrefixExpression* prefixExpression = (PrefixExpression*)(postfixExpression->parent);
            if (prefixExpression->expression->primaryExpression->_isIdentifierExpression()) {
                return (IdentifierExpression*)(prefixExpression->expression->primaryExpression);
            }
        }
    }
    return nullptr;
}

string* SourceVisitor::getErrorType(CatchClause* catchClause) {
    if (catchClause->catchPattern->_isWildCardCatchPattern()) {
        if (catchClause->bindingPattern != nullptr) {
            if (catchClause->bindingPattern->elements != nullptr) {
                if (catchClause->bindingPattern->elements->length() == 1) {
                    TuplePatternElement* element = *(*catchClause->bindingPattern->elements)[0];
                    if (element->pattern->_isIdentifierPattern()) {
                        IdentifierPattern* pattern = (IdentifierPattern*)(element->pattern);
                        if (pattern->annotationForType != nullptr) {
                            return pattern->annotationForType->annotationForType->name;
                        }
                    }
                }
            }
        }
    }
    if (catchClause->catchPattern->_isIdentifierCatchPattern()) {
        IdentifierCatchPattern* identifierCatchPattern = (IdentifierCatchPattern*)catchClause->catchPattern;
        if (identifierCatchPattern->member == nullptr)
            return identifierCatchPattern->name;
        else
            return identifierCatchPattern->member->member;
    }
    return nullptr;
}

void SourceVisitor::visitOperatorPostfix(OperatorPostfix* operatorPostfix) {
    sourceFile->append(operatorPostfix->postfixOperator);
}

void SourceVisitor::visitMemberExpression(MemberExpression* memberExpression) {
    if (memberExpression->parent->_isPostfixExpression()) {
        PostfixExpression* postfixExpression = (PostfixExpression*)(memberExpression->parent);
        if (postfixExpression->primaryExpression->_isIdentifierExpression()) {
            IdentifierExpression* identifierExpression = (IdentifierExpression*)(postfixExpression->primaryExpression);
            if (postfixExpression->postfixes->length() > 1) {
                if (((Postfix*)*(*postfixExpression->postfixes)[0]) && ((*(*postfixExpression->postfixes)[1])->_isFunctionCall())) {
                    if (isClass(identifierExpression->name)) {
                        sourceFile->append("::");
                        sourceFile->append(memberExpression->member);
                        return;
                    }
                }
            }
        }
    }
    sourceFile->append("->");
    sourceFile->append(memberExpression->member);
}

bool SourceVisitor::openSubscript(Subscript* subscript) {
    sourceFile->append(")[");
    return true;
}

void SourceVisitor::closeSubscript(Subscript* subscript) {
    sourceFile->append("]");
}

void SourceVisitor::closeExpressionElement(ExpressionElement* expressionElement) {
    if (!isLastExpressionElement(expressionElement))
        sourceFile->append(", ");
}

bool SourceVisitor::isLastExpressionElement(ExpressionElement* expressionElement) {
    if (expressionElement->parent->_isParenthesizedExpression()) {
        ParenthesizedExpression* parenthesizedExpression = (ParenthesizedExpression*)(expressionElement->parent);
        _Array<ExpressionElement>* expressionElements = parenthesizedExpression->expressionElements;
        size_t length = expressionElements->length() - 1;
        size_t i = 0;
        ExpressionElement* element = nullptr;
        size_t _expressionElements_length = expressionElements->length();
        for (size_t _i = 0; _i < _expressionElements_length; _i++) {
            element = *(*expressionElements)[_i];
            {
                if ((element == expressionElement) && (i == length))
                    return true;
                i++;
            }
        }
    }
    if (expressionElement->parent->_isSubscript()) {
        Subscript* subscript = (Subscript*)(expressionElement->parent);
        _Array<ExpressionElement>* expressions = subscript->expressions;
        size_t length = expressions->length() - 1;
        size_t i = 0;
        ExpressionElement* element = nullptr;
        size_t _expressions_length = expressions->length();
        for (size_t _i = 0; _i < _expressions_length; _i++) {
            element = *(*expressions)[_i];
            {
                if ((element == expressionElement) && (i == length))
                    return true;
                i++;
            }
        }
    }
    return false;
}

void SourceVisitor::visitIdentifierExpression(IdentifierExpression* identifierExpression) {
    sourceFile->append(identifierExpression->name);
}

bool SourceVisitor::inAssignment(SyntaxNode* syntaxNode) {
    if (syntaxNode->_isAssignment())
        return true;
    SyntaxNode* parentNode = syntaxNode->parent;
    if (parentNode == nullptr)
        return false;
    return inAssignment(parentNode);
}

Assignment* SourceVisitor::getAssignment(SyntaxNode* syntaxNode) {
    if (syntaxNode->_isAssignment())
        return (Assignment*)syntaxNode;
    SyntaxNode* parentNode = syntaxNode->parent;
    if (parentNode == nullptr)
        return nullptr;
    return getAssignment(parentNode);
}

Initializer* SourceVisitor::getInitializer(SyntaxNode* syntaxNode) {
    if (syntaxNode->_isInitializer())
        return (Initializer*)syntaxNode;
    SyntaxNode* parentNode = syntaxNode->parent;
    if (parentNode == nullptr)
        return nullptr;
    return getInitializer(parentNode);
}

void SourceVisitor::visitLiteralExpression(LiteralExpression* literalExpression) {
    Literal* literal = literalExpression->literal;
    if (literal->_isNumericLiteral()) {
        NumericLiteral* numericLiteral = (NumericLiteral*)literal;
        sourceFile->append(numericLiteral->value);
    }
    else {
        if (literal->_isStringLiteral()) {
            StringLiteral* stringLiteral = (StringLiteral*)literal;
            sourceFile->append("\"");
            sourceFile->append(stringLiteral->value);
            sourceFile->append("\"");
        }
        else {
            if (literal->_isCharacterLiteral()) {
                CharacterLiteral* characterLiteral = (CharacterLiteral*)literal;
                sourceFile->append("\'");
                if (characterLiteral->value->getLength() > 0) {
                    if ((characterLiteral->value->charAt(0) == '\"') || (characterLiteral->value->charAt(0) == '\'')) {
                        sourceFile->append("\\");
                        sourceFile->append(characterLiteral->value);
                    }
                    else {
                        switch (characterLiteral->value->charAt(0)) {
                            case '\r': {
                                sourceFile->append("\\r");
                                break;
                            }

                            case '\n': {
                                sourceFile->append("\\n");
                                break;
                            }

                            case '\t': {
                                sourceFile->append("\\t");
                                break;
                            }

                            case '\0': {
                                sourceFile->append("\\0");
                                break;
                            }

                            case '\\': {
                                sourceFile->append("\\\\");
                                break;
                            }

                            default: {
                                sourceFile->append(characterLiteral->value);
                            }
                        }
                    }
                }
                sourceFile->append("\'");
            }
        }
    }
}

bool SourceVisitor::openIfExpression(IfExpression* ifExpression) {
    sourceFile->append("if (");
    ifExpression->condition->accept(this);
    sourceFile->append(")");
    if (ifExpression->consequent->_isSimpleExpression()) {
        sourceFile->append("\n    ");
        indent(level(ifExpression));
        ifExpression->consequent->accept(this);
        sourceFile->append(";\n");
    }
    else {
        sourceFile->append(" ");
        ifExpression->consequent->accept(this);
    }
    if (ifExpression->elseClause != nullptr) {
        if (ifExpression->elseClause->alternative->_isSimpleExpression()) {
            ifExpression->elseClause->accept(this);
            sourceFile->append(";\n");
        }
        else {
            ifExpression->elseClause->accept(this);
        }
    }
    return false;
}

bool SourceVisitor::openSwitchExpression(SwitchExpression* switchExpression) {
    sourceFile->append("switch (");
    return true;
}

void SourceVisitor::closeSwitchExpression(SwitchExpression* switchExpression) {
}

bool SourceVisitor::openForExpression(ForExpression* forExpression) {
    Pattern* pattern = forExpression->pattern;
    pattern->accept(this);
    sourceFile->append(" = nullptr;\n");
    indent(level(forExpression) - 1);
    sourceFile->append("size_t _");
    Expression* expression = forExpression->expression;
    if (expression->_isSimpleExpression()) {
        SimpleExpression* simpleExpression = (SimpleExpression*)expression;
        if (simpleExpression->prefixExpression->expression->primaryExpression->_isIdentifierExpression()) {
            IdentifierExpression* identifierExpression = (IdentifierExpression*)(simpleExpression->prefixExpression->expression->primaryExpression);
            string* collectionName = identifierExpression->name;
            sourceFile->append(collectionName);
            sourceFile->append("_length = ");
            forExpression->expression->accept(this);
            sourceFile->append("->length();\n");
            indent(level(forExpression) - 1);
            sourceFile->append("for (size_t _i = 0; _i < _");
            sourceFile->append(collectionName);
            sourceFile->append("_length; _i++) {\n");
            indent(level(forExpression));
            sourceFile->append(forExpression->pattern->identifier);
            sourceFile->append(" = *(*");
            forExpression->expression->accept(this);
            sourceFile->append(")[_i];\n");
            indent(level(forExpression));
            forExpression->code->accept(this);
            if (forExpression->code->_isSimpleExpression())
                sourceFile->append(";\n");
            indent(level(forExpression) - 1);
            sourceFile->append("}\n");
        }
    }
    return false;
}

bool SourceVisitor::openWhileExpression(WhileExpression* whileExpression) {
    sourceFile->append("while (");
    whileExpression->condition->accept(this);
    sourceFile->append(")");
    if (whileExpression->code->_isSimpleExpression()) {
        sourceFile->append("\n");
        indent(level(whileExpression) + 1);
        whileExpression->code->accept(this);
        sourceFile->append(";\n");
    }
    else {
        sourceFile->append(" ");
        whileExpression->code->accept(this);
    }
    return false;
}

bool SourceVisitor::openDoExpression(DoExpression* doExpression) {
    sourceFile->append("do");
    if (doExpression->code->_isSimpleExpression()) {
        sourceFile->append("\n");
        indent(level(doExpression) + 1);
        doExpression->code->accept(this);
        sourceFile->append(";");
    }
    else {
        sourceFile->append(" ");
        doExpression->code->accept(this);
    }
    indent(level(doExpression));
    sourceFile->append("while (");
    doExpression->condition->accept(this);
    sourceFile->append(")");
    return false;
}

bool SourceVisitor::openParenthesizedExpression(ParenthesizedExpression* parenthesizedExpression) {
    if (!(parenthesizedExpression->parent)->_isReturnExpression())
        sourceFile->append("(");
    if (parenthesizedExpression->parent->_isFunctionCall()) {
        _Region _region; _Page* _p = _region.get();
        FunctionCall* functionCall = (FunctionCall*)(parenthesizedExpression->parent);
        bool parameterInserted = false;
        string* pageName = getPage(_p, functionCall);
        if (pageName != nullptr) {
            sourceFile->append(pageName);
            parameterInserted = true;
        }
        if (catchesError(functionCall)) {
            if (!inThrow(functionCall)) {
                _Region _region; _Page* _p = _region.get();
                if (parameterInserted)
                    sourceFile->append(", ");
                string* thrownType = getThrownType(_p, functionCall);
                if (thrownType == nullptr)
                    sourceFile->append("_p");
                else
                    sourceFile->append("_ep");
                parameterInserted = true;
            }
        }
        else {
            if (functionCall->parent->_isPostfixExpression()) {
                PostfixExpression* postfixExpression = (PostfixExpression*)(functionCall->parent);
                if (postfixExpression->primaryExpression->_isIdentifierExpression()) {
                    IdentifierExpression* identifierExpression = (IdentifierExpression*)(postfixExpression->primaryExpression);
                    if (identifierExpression->name->equals("print")) {
                        sourceFile->append("_ep");
                        parameterInserted = true;
                    }
                }
            }
        }
        if (parameterInserted && (parenthesizedExpression->expressionElements != nullptr))
            sourceFile->append(", ");
    }
    return true;
}

bool SourceVisitor::assignedToRootObject(FunctionCall* functionCall) {
    BindingInitializer* bindingInitializer = getBindingInitializer(functionCall);
    if (bindingInitializer == nullptr)
        return false;
    return isRootBinding(bindingInitializer);
}

string* SourceVisitor::getLocalPage(FunctionCall* functionCall) {
    BindingInitializer* bindingInitializer = getBindingInitializer(functionCall);
    if (bindingInitializer == nullptr)
        return nullptr;
    Type* type = bindingInitializer->initializer->pattern->annotationForType->annotationForType;
    if (type->lifeTime == nullptr)
        return nullptr;
    if (type->lifeTime->_isLocal()) {
        Local* local = (Local*)type->lifeTime;
        return local->location;
    }
    return nullptr;
}

bool SourceVisitor::isRootBinding(BindingInitializer* bindingInitializer) {
    if (bindingInitializer == nullptr)
        return false;
    Type* type = bindingInitializer->initializer->pattern->annotationForType->annotationForType;
    if (type->lifeTime == nullptr)
        return false;
    if (type->lifeTime->_isRoot())
        return true;
    return false;
}

bool SourceVisitor::assignedToReturnedObject(FunctionCall* functionCall) {
    BindingInitializer* bindingInitializer = getBindingInitializer(functionCall);
    if (bindingInitializer == nullptr)
        return false;
    if (bindingInitializer->parent->_isConstantDeclaration()) {
        if (boundToObject(bindingInitializer))
            return true;
    }
    return false;
}

bool SourceVisitor::boundToObject(BindingInitializer* bindingInitializer) {
    if (bindingInitializer == nullptr)
        return false;
    Type* type = bindingInitializer->initializer->pattern->annotationForType->annotationForType;
    if (type->lifeTime != nullptr)
        return false;
    if (isClass(type->name) && (getFunctionCall(bindingInitializer->initializer) != nullptr))
        return true;
    return false;
}

BindingInitializer* SourceVisitor::getBindingInitializer(FunctionCall* functionCall) {
    if (functionCall->parent->_isPostfixExpression()) {
        PostfixExpression* postfixExpression = (PostfixExpression*)(functionCall->parent);
        if (postfixExpression->parent->parent->parent->_isInitializer()) {
            Initializer* initializer = (Initializer*)(postfixExpression->parent->parent->parent);
            if (initializer->parent->_isIdentifierInitializer()) {
                IdentifierInitializer* identifierInitializer = (IdentifierInitializer*)(initializer->parent);
                if (identifierInitializer->parent->_isBindingInitializer()) {
                    return (BindingInitializer*)(identifierInitializer->parent);
                }
            }
        }
    }
    return nullptr;
}

bool SourceVisitor::catchesError(FunctionCall* functionCall) {
    if (functionCall == nullptr)
        return false;
    if (functionCall->catchClauses != nullptr)
        return true;
    return false;
}

void SourceVisitor::closeParenthesizedExpression(ParenthesizedExpression* parenthesizedExpression) {
    if (!(parenthesizedExpression->parent)->_isReturnExpression())
        sourceFile->append(")");
}

bool SourceVisitor::openReturnExpression(ReturnExpression* returnExpression) {
    sourceFile->append("return");
    if (returnExpression->expression != nullptr) {
        _Region _region; _Page* _p = _region.get();
        sourceFile->append(" ");
        string* returnType = getReturnType(_p, returnExpression);
        string* thrownType = getThrownType(_p, returnExpression);
        if (thrownType != nullptr) {
            sourceFile->append("_Result<");
            if (returnType != nullptr)
                sourceFile->append(returnType);
            sourceFile->append(", ");
            sourceFile->append(thrownType);
            sourceFile->append(">(");
        }
        returnExpression->expression->accept(this);
        if (returnsArray(returnExpression)) {
            sourceFile->append(" ? new(_rp) ");
            sourceFile->append(returnType);
            sourceFile->append("(");
            returnExpression->expression->accept(this);
            sourceFile->append(") : nullptr");
        }
        if (thrownType != nullptr)
            sourceFile->append(")");
    }
    else {
        if (inTopLevelCode(returnExpression))
            sourceFile->append(" nullptr");
    }
    return false;
}

bool SourceVisitor::inTopLevelCode(SyntaxNode* syntaxNode) {
    if (syntaxNode == nullptr)
        return false;
    if (syntaxNode->_isFunctionDeclaration())
        return false;
    if (syntaxNode->_isModule())
        return true;
    if (syntaxNode->parent == nullptr)
        return false;
    return inTopLevelCode(syntaxNode->parent);
}

void SourceVisitor::closeReturnExpression(ReturnExpression* returnExpression) {
    _Region _region; _Page* _p = _region.get();
    string* returnType = getReturnType(_p, returnExpression);
    if (returnType != nullptr) {
        sourceFile->append(")");
    }
}

bool SourceVisitor::openThrowExpression(ThrowExpression* throwExpression) {
    _Region _region; _Page* _p = _region.get();
    string* thrownType = getThrownType(_p, throwExpression);
    if (thrownType == nullptr)
        return false;
    sourceFile->append("return ");
    string* returnType = getReturnType(_p, throwExpression);
    if (returnType != nullptr) {
        sourceFile->append("_Result<");
        sourceFile->append(returnType);
        sourceFile->append(", ");
        sourceFile->append(thrownType);
        sourceFile->append(">(");
    }
    {
        bool buildError = true;
        if (throwExpression->arguments == nullptr) {
            if (throwExpression->error->_isIdentifierExpression()) {
                IdentifierExpression* errorExpression = (IdentifierExpression*)(throwExpression->error);
                string* errorName = errorExpression->name;
                CatchClause* catchClause = getCatchClause(throwExpression);
                if (catchClause != nullptr) {
                    if (catchClause->catchPattern != nullptr) {
                        if (catchClause->catchPattern->_isWildCardCatchPattern()) {
                            if (catchClause->bindingPattern != nullptr) {
                                TuplePattern* bindingPattern;
                                bindingPattern = catchClause->bindingPattern;
                                if (bindingPattern->elements != nullptr) {
                                    if (bindingPattern->elements->length() > 0) {
                                        TuplePatternElement* element = *(*bindingPattern->elements)[0];
                                        if (element->pattern->_isIdentifierPattern()) {
                                            IdentifierPattern* pattern = (IdentifierPattern*)(element->pattern);
                                            if (pattern->identifier->equals(errorName))
                                                buildError = false;
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        if (buildError) {
            sourceFile->append("new(_ep) ");
            sourceFile->append(thrownType);
            sourceFile->append("(");
            if (throwExpression->arguments != nullptr)
                sourceFile->append("new(_ep) ");
            sourceFile->append("_");
            sourceFile->append(thrownType);
            if (throwExpression->arguments == nullptr)
                sourceFile->append("Code");
            sourceFile->append("_");
        }
        if (throwExpression->error != nullptr)
            throwExpression->error->accept(this);
        if (throwExpression->arguments != nullptr)
            throwExpression->arguments->accept(this);
        if (buildError)
            sourceFile->append(")");
    }
    if (returnType != nullptr)
        sourceFile->append(")");
    return false;
}

bool SourceVisitor::inWildcardCatchClause(ThrowExpression* throwExpression) {
    CatchClause* catchClause = getCatchClause(throwExpression);
    if ((catchClause != nullptr) && (catchClause->catchPattern->_isWildCardCatchPattern()))
        return true;
    return false;
}

CatchClause* SourceVisitor::getCatchClause(SyntaxNode* syntaxNode) {
    if (syntaxNode->_isCatchClause())
        return (CatchClause*)syntaxNode;
    if (syntaxNode->parent == nullptr)
        return nullptr;
    return getCatchClause(syntaxNode->parent);
}

string* SourceVisitor::getReturnType(_Page* _rp, SyntaxNode* syntaxNode) {
    FunctionDeclaration* functionDeclaration = getFunctionDeclaration(syntaxNode);
    if (functionDeclaration != nullptr) {
        FunctionResult* functionResult = functionDeclaration->signature->result;
        if (functionResult != nullptr) {
            VarString* ret = new(_rp) VarString();
            if (hasArrayPostfix(functionResult->resultType)) {
                Type* type = functionResult->resultType;
                ret->append("_Array<");
                appendCppTypeName(ret, type);
                ret->append(">");
                return new(_rp) string(ret);
            }
            else {
                appendCppTypeName(ret, functionResult->resultType);
                return new(_rp) string(ret);
            }
        }
    }
    return nullptr;
}

string* SourceVisitor::getThrownType(_Page* _rp, SyntaxNode* syntaxNode) {
    FunctionDeclaration* functionDeclaration = getFunctionDeclaration(syntaxNode);
    if (functionDeclaration != nullptr) {
        ThrowsClause* throwsClause = functionDeclaration->signature->throwsClause;
        if (throwsClause != nullptr) {
            VarString* ret = new(_rp) VarString();
            if (hasArrayPostfix(throwsClause->throwsType)) {
                Type* type = throwsClause->throwsType;
                ret->append("_Array<");
                appendCppTypeName(ret, type);
                ret->append(">");
                return new(_rp) string(ret);
            }
            else {
                appendCppTypeName(ret, throwsClause->throwsType);
                return new(_rp) string(ret);
            }
        }
    }
    return nullptr;
}

bool SourceVisitor::returnsArray(SyntaxNode* syntaxNode) {
    FunctionDeclaration* functionDeclaration = getFunctionDeclaration(syntaxNode);
    if (functionDeclaration != nullptr) {
        FunctionResult* functionResult = functionDeclaration->signature->result;
        if (functionResult != nullptr) {
            if (hasArrayPostfix(functionResult->resultType))
                return true;
        }
    }
    return false;
}

FunctionDeclaration* SourceVisitor::getFunctionDeclaration(SyntaxNode* syntaxNode) {
    if (syntaxNode->_isFunctionDeclaration())
        return (FunctionDeclaration*)syntaxNode;
    if (syntaxNode->parent == nullptr)
        return nullptr;
    return getFunctionDeclaration(syntaxNode->parent);
}

bool SourceVisitor::openBreakExpression(BreakExpression* breakExpression) {
    sourceFile->append("break");
    return true;
}

string* SourceVisitor::getPageOfVariable(_Page* _rp, string* name, CodeBlock* codeBlock) {
    Statement* statement = nullptr;
    size_t _codeBlock_length = codeBlock->statements->length();
    for (size_t _i = 0; _i < _codeBlock_length; _i++) {
        statement = *(*codeBlock->statements)[_i];
        {
            BindingInitializer* bindingInitializer = nullptr;
            if (statement->_isConstantDeclaration()) {
                ConstantDeclaration* constantDeclaration = (ConstantDeclaration*)statement;
                bindingInitializer = constantDeclaration->initializer;
            }
            if (statement->_isMutableDeclaration()) {
                MutableDeclaration* mutableDeclaration = (MutableDeclaration*)statement;
                bindingInitializer = mutableDeclaration->initializer;
            }
            if (bindingInitializer == nullptr)
                continue;
            IdentifierPattern* identifierPattern = bindingInitializer->initializer->pattern;
            if (identifierPattern->identifier->equals(name)) {
                if (identifierPattern->annotationForType != nullptr) {
                    if (identifierPattern->annotationForType->annotationForType->lifeTime == nullptr) {
                        return new(_rp) string("_rp");
                    }
                    else {
                        LifeTime* lifeTime = identifierPattern->annotationForType->annotationForType->lifeTime;
                        if (lifeTime->_isRoot())
                            return new(_rp) string("_p");
                        if (lifeTime->_isLocal()) {
                            Local* local = (Local*)lifeTime;
                            VarString* ret = new(_rp) VarString(local->location);
                            ret->append("->_getPage()");
                            return new(_rp) string(ret);
                        }
                    }
                }
            }
        }
    }
    return nullptr;
}

string* SourceVisitor::getPage(_Page* _rp, SyntaxNode* node) {
    while (node != nullptr) {
        if (node->_isReturnExpression())
            return new(_rp) string("_rp");
        if (node->_isThrowExpression())
            return new(_rp) string("_ep");
        if (node->_isCatchClause())
            return nullptr;
        if (node->_isIdentifierInitializer()) {
            IdentifierInitializer* identifierInitializer = (IdentifierInitializer*)node;
            IdentifierPattern* identifierPattern = identifierInitializer->pattern;
            if (identifierPattern->annotationForType != nullptr) {
                if (!isClass(identifierPattern->annotationForType->annotationForType->name))
                    return nullptr;
                if (identifierPattern->annotationForType->annotationForType->lifeTime == nullptr) {
                    return new(_rp) string("_rp");
                }
                else {
                    LifeTime* lifeTime = identifierPattern->annotationForType->annotationForType->lifeTime;
                    if (lifeTime->_isRoot())
                        return new(_rp) string("_p");
                    if (lifeTime->_isLocal()) {
                        Local* local = (Local*)lifeTime;
                        VarString* ret = new(_rp) VarString(local->location);
                        ret->append("->_getPage()");
                        return new(_rp) string(ret);
                    }
                    if (lifeTime->_isReference())
                        return nullptr;
                }
            }
        }
        if (node->_isAssignment()) {
            Assignment* assignment = (Assignment*)node;
            SimpleExpression* simpleExpression = (SimpleExpression*)(assignment->parent);
            PostfixExpression* leftSide = simpleExpression->prefixExpression->expression;
            if (leftSide->primaryExpression->_isIdentifierExpression()) {
                IdentifierExpression* identifierExpression = (IdentifierExpression*)(leftSide->primaryExpression);
                string* name = identifierExpression->name;
                SyntaxNode* node = assignment;
                while (node != nullptr) {
                    if (node->_isCodeBlock()) {
                        CodeBlock* codeBlock = (CodeBlock*)node;
                        string* page = getPageOfVariable(_rp, name, codeBlock);
                        if (page != nullptr)
                            return page;
                    }
                    node = node->parent;
                }
                ClassDeclaration* classDeclaration = getClassDeclaration(assignment);
                if (classDeclaration != nullptr) {
                    VarString* page = new(_rp) VarString("");
                    if (isObjectField(name, classDeclaration)) {
                        if (isVariableObjectField(name, classDeclaration)) {
                            if (!inConstructor(assignment)) {
                                page->append(name);
                                page->append(" == nullptr ? _getPage()->allocateExclusivePage() : ");
                                page->append(name);
                                page->append("->");
                            }
                            page->append("_getPage()");
                            if (inConstructor(assignment)) {
                                page->append("->allocateExclusivePage()");
                            }
                        }
                        else {
                            page->append("_getPage()");
                        }
                        return new(_rp) string(page);
                    }
                }
            }
        }
        node = node->parent;
    }
    return nullptr;
}

bool SourceVisitor::openConstructorCall(ConstructorCall* constructorCall) {
    _Region _region; _Page* _p = _region.get();
    sourceFile->append("new(");
    string* page = getPage(_p, constructorCall->parent);
    if (page != nullptr)
        sourceFile->append(page);
    sourceFile->append(") ");
    return true;
}

void SourceVisitor::visitThisExpression(ThisExpression* thisExpression) {
    sourceFile->append("this");
}

void SourceVisitor::visitNullExpression(NullExpression* nullExpression) {
    sourceFile->append("nullptr");
}

bool SourceVisitor::openElseClause(ElseClause* elseClause) {
    indent(level(elseClause));
    sourceFile->append("else");
    if (elseClause->alternative->_isSimpleExpression()) {
        sourceFile->append("\n    ");
        indent(level(elseClause));
        elseClause->alternative->accept(this);
    }
    else {
        sourceFile->append(" ");
        elseClause->alternative->accept(this);
    }
    return false;
}

bool SourceVisitor::openCurliedSwitchBody(CurliedSwitchBody* curliedSwitchBody) {
    sourceFile->append(") {\n");
    return true;
}

void SourceVisitor::closeCurliedSwitchBody(CurliedSwitchBody* curliedSwitchBody) {
    indent(level(curliedSwitchBody) - 1);
    sourceFile->append("}\n");
}

bool SourceVisitor::openSwitchCase(SwitchCase* switchCase) {
    indent(level(switchCase));
    return true;
}

bool SourceVisitor::openItemCaseLabel(ItemCaseLabel* itemCaseLabel) {
    sourceFile->append("case ");
    return true;
}

void SourceVisitor::visitDefaultCaseLabel(DefaultCaseLabel* defaultCaseLabel) {
    sourceFile->append("default: ");
}

bool SourceVisitor::openCaseItem(CaseItem* caseItem) {
    sourceFile->append("case ");
    return true;
}

void SourceVisitor::closeCaseItem(CaseItem* caseItem) {
    sourceFile->append(": ");
}

bool SourceVisitor::openIdentifierPattern(IdentifierPattern* identifierPattern) {
    if (identifierPattern->parent->_isIdentifierInitializer()) {
        IdentifierInitializer* identifierInitializer = (IdentifierInitializer*)(identifierPattern->parent);
        if (isCatchingIdentifierInitializer(identifierInitializer)) {
            sourceFile->append("auto _");
            sourceFile->append(identifierPattern->identifier);
            sourceFile->append("_result");
            return false;
        }
    }
    if (identifierPattern->annotationForType != nullptr) {
        identifierPattern->annotationForType->accept(this);
        sourceFile->append(" ");
    }
    sourceFile->append(identifierPattern->identifier);
    return false;
}

bool SourceVisitor::isCatchingIdentifierInitializer(IdentifierInitializer* identifierInitializer) {
    if (identifierInitializer->initializer != nullptr) {
        if (identifierInitializer->initializer->expression->_isSimpleExpression()) {
            SimpleExpression* simpleExpression = (SimpleExpression*)(identifierInitializer->initializer->expression);
            PostfixExpression* postfixExpression = simpleExpression->prefixExpression->expression;
            if (postfixExpression->postfixes != nullptr) {
                FunctionCall* functionCall = nullptr;
                if (postfixExpression->postfixes->length() > 0) {
                    Postfix* postfix = *(*postfixExpression->postfixes)[0];
                    if (postfix->_isFunctionCall()) {
                        functionCall = (FunctionCall*)postfix;
                    }
                    else {
                        if (postfix->_isMemberExpression()) {
                            if (postfixExpression->postfixes->length() > 1) {
                                Postfix* postfix = *(*postfixExpression->postfixes)[1];
                                if (postfix->_isFunctionCall()) {
                                    functionCall = (FunctionCall*)postfix;
                                }
                            }
                        }
                    }
                }
                if (functionCall != nullptr) {
                    if (functionCall->catchClauses != nullptr)
                        return true;
                }
            }
        }
    }
    return false;
}

void SourceVisitor::closeExpressionPattern(ExpressionPattern* expressionPattern) {
    if (expressionPattern->parent->_isItemCaseLabel())
        sourceFile->append(": ");
}

bool SourceVisitor::openCaseContent(CaseContent* caseContent) {
    sourceFile->append("{\n");
    return true;
}

void SourceVisitor::closeCaseContent(CaseContent* caseContent) {
    bool additionalLineFeed = true;
    if (caseContent->parent->_isSwitchCase()) {
        if (!((SwitchCase*)caseContent->parent)->label->_isDefaultCaseLabel()) {
            indent(level(caseContent));
            sourceFile->append("break;\n");
        }
        else
            additionalLineFeed = false;
    }
    indent(level(caseContent) - 1);
    sourceFile->append("}\n");
    if (additionalLineFeed)
        sourceFile->append("\n");
}

bool SourceVisitor::openType(Type* type) {
    if (hasArrayPostfix(type)) {
        sourceFile->append("_Array<");
    }
    appendCppTypeName(sourceFile, type);
    return true;
}

void SourceVisitor::closeType(Type* type) {
    if (hasArrayPostfix(type)) {
        sourceFile->append(">");
        if (!type->parent->_isConstructorCall())
            sourceFile->append("*");
    }
    if (isClass(type->name) && !hasArrayPostfix(type) && !inTypeQuery(type) && !type->parent->_isConstructorCall())
        sourceFile->append("*");
    if (inTypeQuery(type))
        sourceFile->append("()");
}

bool SourceVisitor::inTypeQuery(Type* type) {
    if (type->parent->_isTypeQuery())
        return true;
    return false;
}

void SourceVisitor::visitPointer(Pointer* pointer) {
    sourceFile->append("*");
}

bool SourceVisitor::openInheritance(Inheritance* inheritance) {
    return false;
}

VarString* SourceVisitor::buildProjectFileString(_Page* _rp, Program* program) {
    VarString* projectFile = new(_rp) VarString();
    projectFile->append("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
    projectFile->append("<CodeLite_Project Name=\"");
    projectFile->append(program->name);
    projectFile->append("\" InternalType=\"Console\">\n  <Plugins>\n    <Plugin Name=\"qmake\">\n");
    projectFile->append("      <![CDATA[00020001N0005Debug0000000000000001N0007Release000000000000]]>\n    </Plugin>\n");
    projectFile->append("    <Plugin Name=\"CMakePlugin\">\n      <![CDATA[[{\n");
    projectFile->append("  \"name\": \"Debug\",\n  \"enabled\": false,\n  \"buildDirectory\": \"build\",\n");
    projectFile->append("  \"sourceDirectory\": \"$(ProjectPath)\",\n  \"generator\": \"\",\n  \"buildType\": \"\",\n");
    projectFile->append("  \"arguments\": [],\n  \"parentProject\": \"\"\n");
    projectFile->append(" }, {\n");
    projectFile->append("  \"name\": \"Release\",\n");
    projectFile->append("  \"enabled\": false,\n");
    projectFile->append("  \"buildDirectory\": \"build\",\n");
    projectFile->append("  \"sourceDirectory\": \"$(ProjectPath)\",\n");
    projectFile->append("  \"generator\": \"\",\n");
    projectFile->append("  \"buildType\": \"\",\n");
    projectFile->append("  \"arguments\": [],\n");
    projectFile->append("  \"parentProject\": \"\"\n");
    projectFile->append(" }]]]>\n    </Plugin>\n  </Plugins>\n");
    projectFile->append("  <Description/>\n  <Dependencies/>\n");
    projectFile->append("  <VirtualDirectory Name=\"src\">\n");
    {
        Module* module = nullptr;
        size_t _program_length = program->modules->length();
        for (size_t _i = 0; _i < _program_length; _i++) {
            module = *(*program->modules)[_i];
            {
                _Region _region; _Page* _p = _region.get();
                projectFile->append("    <File Name=\"");
                string* fileName = getFileName(_p, module);
                if (fileName != nullptr) {
                    _Region _region; _Page* _p = _region.get();
                    string* name = Path::getFileNameWithoutExtension(_p, fileName);
                    projectFile->append(name);
                    projectFile->append(".cpp\"/>\n");
                    projectFile->append("    <File Name=\"");
                    projectFile->append(name);
                    projectFile->append(".scaly\"/>\n");
                }
            }
        }
    }
    projectFile->append("  </VirtualDirectory>\n  <VirtualDirectory Name=\"include\">\n");
    {
        Module* module = nullptr;
        size_t _program_length = program->modules->length();
        for (size_t _i = 0; _i < _program_length; _i++) {
            module = *(*program->modules)[_i];
            {
                _Region _region; _Page* _p = _region.get();
                string* fileName = getFileName(_p, module);
                if (fileName != nullptr) {
                    _Region _region; _Page* _p = _region.get();
                    string* name = Path::getFileNameWithoutExtension(_p, fileName);
                    projectFile->append("    <File Name=\"");
                    projectFile->append(name);
                    projectFile->append(".h\"/>\n");
                }
            }
        }
    }
    projectFile->append("  </VirtualDirectory>\n  <Dependencies Name=\"Debug\"/>\n  <Dependencies Name=\"Release\"/>\n");
    projectFile->append("  <Settings Type=\"Executable\">\n    <GlobalSettings>\n");
    projectFile->append("      <Compiler Options=\"\" C_Options=\"\" Assembler=\"\">\n");
    projectFile->append("        <IncludePath Value=\".\"/>\n      </Compiler>\n      <Linker Options=\"\">\n");
    projectFile->append("        <LibraryPath Value=\".\"/>\n      </Linker>\n      <ResourceCompiler Options=\"\"/>\n");
    projectFile->append("    </GlobalSettings>\n");
    projectFile->append("    <Configuration Name=\"Debug\" CompilerType=\"GCC\" DebuggerType=\"GNU gdb debugger\"");
    projectFile->append(" Type=\"Executable\" BuildCmpWithGlobalSettings=\"append\" BuildLnkWithGlobalSettings=\"append\"");
    projectFile->append(" BuildResWithGlobalSettings=\"append\">\n");
    projectFile->append("      <Compiler Options=\"-g;-O0;-std=c++11;-Wall\" C_Options=\"-g;-O0;-Wall\" Assembler=\"\"");
    projectFile->append(" Required=\"yes\" PreCompiledHeader=\"\" PCHInCommandLine=\"no\" PCHFlags=\"\" PCHFlagsPolicy=\"0\">\n");
    projectFile->append("        <IncludePath Value=\".\"/>\n        <IncludePath Value=\"../scalypp\"/>\n      </Compiler>\n");
    projectFile->append("      <Linker Options=\"\" Required=\"yes\">\n        <LibraryPath Value=\"../Debug\"/>\n");
    projectFile->append("        <Library Value=\"libscalypp\"/>\n      </Linker>\n      <ResourceCompiler Options=\"\" Required=\"no\"/>\n");
    projectFile->append("      <General OutputFile=\"$(IntermediateDirectory)/$(ProjectName)\" IntermediateDirectory=\"../Debug\" ");
    projectFile->append("Command=\"./$(ProjectName)\" CommandArguments=\"-o ");
    projectFile->append(program->name);
    projectFile->append(" -d ../");
    projectFile->append(program->name);
    {
        Module* module = nullptr;
        size_t _program_length = program->modules->length();
        for (size_t _i = 0; _i < _program_length; _i++) {
            module = *(*program->modules)[_i];
            {
                _Region _region; _Page* _p = _region.get();
                string* fileName = getFileName(_p, module);
                if (fileName != nullptr) {
                    _Region _region; _Page* _p = _region.get();
                    string* name = Path::getFileNameWithoutExtension(_p, fileName);
                    projectFile->append(" ../");
                    projectFile->append(program->name);
                    projectFile->append("/");
                    projectFile->append(name);
                    projectFile->append(".scaly");
                }
            }
        }
    }
    projectFile->append("\" UseSeparateDebugArgs=\"no\" DebugArguments=\"\" WorkingDirectory=\"$(IntermediateDirectory)\"");
    projectFile->append(" PauseExecWhenProcTerminates=\"yes\" IsGUIProgram=\"no\" IsEnabled=\"yes\"/>\n");
    projectFile->append("      <Environment EnvVarSetName=\"&lt;Use Defaults&gt;\" DbgSetName=\"&lt;Use Defaults&gt;\">\n");
    projectFile->append("        <![CDATA[]]>\n      </Environment>\n");
    projectFile->append("      <Debugger IsRemote=\"no\" RemoteHostName=\"\" RemoteHostPort=\"\" DebuggerPath=\"\" IsExtended=\"yes\">\n");
    projectFile->append("        <DebuggerSearchPaths/>\n        <PostConnectCommands/>\n        <StartupCommands/>\n      </Debugger>\n");
    projectFile->append("      <PreBuild/>\n      <PostBuild/>\n      <CustomBuild Enabled=\"no\">\n        <RebuildCommand/>\n");
    projectFile->append("        <CleanCommand/>\n        <BuildCommand/>\n        <PreprocessFileCommand/>\n        <SingleFileCommand/>\n");
    projectFile->append("        <MakefileGenerationCommand/>\n        <ThirdPartyToolName>None</ThirdPartyToolName>\n");
    projectFile->append("        <WorkingDirectory/>\n      </CustomBuild>\n      <AdditionalRules>\n");
    projectFile->append("        <CustomPostBuild/>\n        <CustomPreBuild/>\n      </AdditionalRules>\n");
    projectFile->append("      <Completion EnableCpp11=\"no\" EnableCpp14=\"no\">\n        <ClangCmpFlagsC/>\n        <ClangCmpFlags/>\n");
    projectFile->append("        <ClangPP/>\n        <SearchPaths/>\n      </Completion>\n");
    projectFile->append("    </Configuration>\n");
    projectFile->append("    <Configuration Name=\"Release\" CompilerType=\"GCC\" DebuggerType=\"GNU gdb debugger\" Type=\"Executable\"");
    projectFile->append(" BuildCmpWithGlobalSettings=\"append\" BuildLnkWithGlobalSettings=\"append\" BuildResWithGlobalSettings=\"append\">\n");
    projectFile->append("      <Compiler Options=\"-O2;-Wall\" C_Options=\"-O2;-Wall\" Assembler=\"\" Required=\"yes\" PreCompiledHeader=\"\"");
    projectFile->append(" PCHInCommandLine=\"no\" PCHFlags=\"\" PCHFlagsPolicy=\"0\">\n");
    projectFile->append("        <IncludePath Value=\".\"/>\n        <Preprocessor Value=\"NDEBUG\"/>\n");
    projectFile->append("      </Compiler>\n      <Linker Options=\"\" Required=\"yes\">\n");
    projectFile->append("        <LibraryPath Value=\"../Release\"/>\n        <Library Value=\"libscaly\"/>\n      </Linker>\n");
    projectFile->append("      <ResourceCompiler Options=\"\" Required=\"no\"/>\n");
    projectFile->append("      <General OutputFile=\"$(IntermediateDirectory)/$(ProjectName)\" IntermediateDirectory=\"../Release\"");
    projectFile->append(" Command=\"./$(ProjectName)\" CommandArguments=\"\" UseSeparateDebugArgs=\"no\" DebugArguments=\"\"");
    projectFile->append(" WorkingDirectory=\"$(IntermediateDirectory)\" PauseExecWhenProcTerminates=\"yes\" IsGUIProgram=\"no\" IsEnabled=\"yes\"/>\n");
    projectFile->append("      <Environment EnvVarSetName=\"&lt;Use Defaults&gt;\" DbgSetName=\"&lt;Use Defaults&gt;\">\n");
    projectFile->append("        <![CDATA[]]>\n      </Environment>\n");
    projectFile->append("      <Debugger IsRemote=\"no\" RemoteHostName=\"\" RemoteHostPort=\"\" DebuggerPath=\"\" IsExtended=\"yes\">\n");
    projectFile->append("        <DebuggerSearchPaths/>\n        <PostConnectCommands/>\n        <StartupCommands/>\n");
    projectFile->append("      </Debugger>\n      <PreBuild/>\n      <PostBuild/>\n      <CustomBuild Enabled=\"no\">\n");
    projectFile->append("        <RebuildCommand/>\n        <CleanCommand/>\n        <BuildCommand/>\n        <PreprocessFileCommand/>\n");
    projectFile->append("        <SingleFileCommand/>\n        <MakefileGenerationCommand/>\n");
    projectFile->append("        <ThirdPartyToolName>None</ThirdPartyToolName>\n        <WorkingDirectory/>\n");
    projectFile->append("      </CustomBuild>\n      <AdditionalRules>\n        <CustomPostBuild/>\n");
    projectFile->append("        <CustomPreBuild/>\n      </AdditionalRules>\n      <Completion EnableCpp11=\"no\" EnableCpp14=\"no\">\n");
    projectFile->append("        <ClangCmpFlagsC/>\n        <ClangCmpFlags/>\n        <ClangPP/>\n");
    projectFile->append("        <SearchPaths/>\n      </Completion>\n    </Configuration>\n  </Settings>\n");
    projectFile->append("  <Dependencies Name=\"Debug\"/>\n  <Dependencies Name=\"Release\"/>\n</CodeLite_Project>\n");
    return projectFile;
}

void SourceVisitor::collectInheritances(Program* program) {
    Module* module = nullptr;
    size_t _program_length = program->modules->length();
    for (size_t _i = 0; _i < _program_length; _i++) {
        module = *(*program->modules)[_i];
        collectInheritancesInModule(module);
    }
}

bool SourceVisitor::_isSourceVisitor() { return (true); }


}
