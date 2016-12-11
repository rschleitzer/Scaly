#include "scalyc.h"
using namespace scaly;
namespace scalyc {

Model::Model(string* name) {
    this->name = name;
    units = new(_getPage()) _Array<Unit>();
}

bool Unit::_isTopLevelCode() { return (false); }
bool Unit::_isCode() { return (false); }

bool Item::_isDefinition() { return (false); }

bool Definition::_isDefinition() { return (true); }

TopLevelCode::TopLevelCode(Model* model) {
    this->model = model;
    items = new(_getPage()) _Array<Item>();
}

bool TopLevelCode::_isTopLevelCode() { return (true); }

Code::Code(Model* model) {
    this->model = model;
    definitions = new(_getPage()) _Array<Definition>();
}

bool Code::_isCode() { return (true); }

bool ModelVisitor::openProgram(Program* program) {
    if (model != nullptr)
        model->_getPage()->clear();
    model = new(model == nullptr ? _getPage()->allocateExclusivePage() : model->_getPage()) Model(program->name);
    return true;
}

bool ModelVisitor::openCompilationUnit(CompilationUnit* compilationUnit) {
    if (isTopLevelFile(compilationUnit)) {
        if (unit != nullptr)
            unit->_getPage()->clear();
        unit = new(unit == nullptr ? _getPage()->allocateExclusivePage() : unit->_getPage()) TopLevelCode(model);
    }
    else {
        if (unit != nullptr)
            unit->_getPage()->clear();
        unit = new(unit == nullptr ? _getPage()->allocateExclusivePage() : unit->_getPage()) Code(model);
    }
    return true;
}

void ModelVisitor::closeCompilationUnit(CompilationUnit* compilationUnit) {
    model->units->push(unit);
}

bool ModelVisitor::openConstantDeclaration(ConstantDeclaration* constantDeclaration) {
    return true;
}

void ModelVisitor::closeConstantDeclaration(ConstantDeclaration* constantDeclaration) {
}

bool ModelVisitor::_isModelVisitor() { return (true); }


}
