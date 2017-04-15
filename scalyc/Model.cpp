#include "scalyc.h"
using namespace scaly;
namespace scalyc {

Model::Model(string* name) {
    this->name = name;
    main = nullptr;
    units = new(_getPage()->allocateExclusivePage()) _Array<Unit>();
}

Unit::Unit() {
    definitions = new(_getPage()->allocateExclusivePage()) _Array<Definition>();
}

Unit::Unit(Model* model, string* name) {
    this->model = model;
    this->name = name;
    definitions = new(_getPage()->allocateExclusivePage()) _Array<Definition>();
}

bool Action::_isScope() { return (false); }

Scope::Scope(Scope* parent) {
    this->parent = parent;
    bindings = new(_getPage()->allocateExclusivePage()) _Array<Binding>();
    definitions = new(_getPage()->allocateExclusivePage()) _Array<Definition>();
    actions = new(_getPage()->allocateExclusivePage()) _Array<Action>();
}

bool Scope::_isScope() { return (true); }

ModelVisitor::ModelVisitor() {
    model = nullptr;
    unit = nullptr;
}

bool ModelVisitor::openProgram(Program* program) {
    if (model != nullptr)
        model->_getPage()->clear();
    model = new(model == nullptr ? _getPage()->allocateExclusivePage() : model->_getPage()) Model(program->name);
    return true;
}

bool ModelVisitor::openModule(Module* module) {
    if (isTopLevelFile(module)) {
        if (model != nullptr)
            model->_getPage()->clear();
        model->main = new(model == nullptr ? _getPage()->allocateExclusivePage() : model->_getPage()) Scope(nullptr);
    }
    else {
        if (unit != nullptr)
            unit->_getPage()->clear();
        unit = new(unit == nullptr ? _getPage()->allocateExclusivePage() : unit->_getPage()) Unit(model, getFileName(unit == nullptr ? _getPage()->allocateExclusivePage() : unit->_getPage(), module));
    }
    return true;
}

void ModelVisitor::closeModule(Module* module) {
    if (isTopLevelFile(module))
        model->units->push(unit);
}

bool ModelVisitor::openConstantDeclaration(ConstantDeclaration* constantDeclaration) {
    if (constantDeclaration->parent->_isClassDeclaration()) {
    }
    else {
    }
    return true;
}

void ModelVisitor::closeConstantDeclaration(ConstantDeclaration* constantDeclaration) {
}

bool ModelVisitor::_isModelVisitor() { return (true); }


}
