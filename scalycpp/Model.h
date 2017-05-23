#ifndef __scalycpp__Model__
#define __scalycpp__Model__
#include "scalycpp.h"
using namespace scaly;
namespace scalycpp {

class Scope;

class Unit;

class Model : public Object {
public:
    string* name;
    Scope* main;
    _Array<Unit>* units;
    Model(string* name);

};

class Definition : public Object {
public:

};

class Unit : public Object {
public:
    Model* model;
    string* name;
    _Array<Definition>* definitions;
    Unit();
    Unit(Model* model, string* name);

};

class Action : public Object {
public:

    virtual bool _isScope();
};

class Binding : public Object {
public:
    Scope* scope;
    string* identifier;
    bool isMutable;
    Action* initializer;

};

class Scope : public Action {
public:
    Scope* parent;
    _Array<Definition>* definitions;
    _Array<Binding>* bindings;
    _Array<Action>* actions;
    Scope(Scope* parent);

    virtual bool _isScope();
};

class ModelVisitor : public CommonVisitor {
public:
    Model* model;
    Unit* unit;
    ModelVisitor();
    virtual bool openProgram(Program* program);
    virtual bool openModule(Module* module);
    virtual void closeModule(Module* module);
    virtual bool openConstantDeclaration(ConstantDeclaration* constantDeclaration);
    virtual void closeConstantDeclaration(ConstantDeclaration* constantDeclaration);

    virtual bool _isModelVisitor();
};

}
#endif // __scalycpp__Model__
