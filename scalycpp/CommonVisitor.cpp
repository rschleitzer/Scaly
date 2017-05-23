#include "scalycpp.h"
using namespace scaly;
namespace scalycpp {

bool CommonVisitor::isTopLevelFile(Module* module) {
    Statement* statement = nullptr;
    size_t _module_length = module->statements->length();
    for (size_t _i = 0; _i < _module_length; _i++) {
        statement = *(*module->statements)[_i];
        if (statement->_isExpression())
            return true;
;
    }
    return false;
}

string* CommonVisitor::getFileName(_Page* _rp, Module* module) {
    if (module->statements == nullptr)
        return nullptr;
    Statement* statement = nullptr;
    size_t _module_length = module->statements->length();
    for (size_t _i = 0; _i < _module_length; _i++) {
        statement = *(*module->statements)[_i];
        {
            if (statement->_isClassDeclaration()) {
                ClassDeclaration* classDeclaration = (ClassDeclaration*)statement;
                if (classDeclaration->body != nullptr)
                    return new(_rp) string(classDeclaration->name);
            }
            if (statement->_isEnumDeclaration()) {
                EnumDeclaration* enumDeclaration = (EnumDeclaration*)statement;
                return new(_rp) string(enumDeclaration->name);
            }
        }
    }
    if (module->parent->_isProgram()) {
        Program* program = (Program*)module->parent;
        return new(_rp) string(program->name);
    }
    return nullptr;
}

bool CommonVisitor::_isCommonVisitor() { return (true); }

bool CommonVisitor::_isCppVisitor() { return (false); }
bool CommonVisitor::_isHeaderVisitor() { return (false); }
bool CommonVisitor::_isSourceVisitor() { return (false); }
bool CommonVisitor::_isModelVisitor() { return (false); }


}
