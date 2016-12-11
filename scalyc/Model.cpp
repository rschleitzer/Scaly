#include "scalyc.h"
using namespace scaly;
namespace scalyc {

Model::Model(string* name) {
    this->name = name;
    units = new(_getPage()) _Array<Unit>();
}

Unit::Unit(Model* model) {
    this->model = model;
    definitions = new(_getPage()) _Array<Definition>();
}

bool Unit::_isTopLevel() { return (false); }

TopLevel::TopLevel(Model* model) {
    this->model = model;
    definitions = new() _Array<Definition>();
    actions = new() _Array<Action>();
}

bool TopLevel::_isTopLevel() { return (true); }

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
    if (constantDeclaration->parent->_isClassDeclaration()) {
    }
    return true;
}

void ModelVisitor::closeConstantDeclaration(ConstantDeclaration* constantDeclaration) {
}

bool ModelVisitor::_isModelVisitor() { return (true); }


}
