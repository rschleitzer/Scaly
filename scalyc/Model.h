#ifndef __scalyc__Model__
#define __scalyc__Model__
#include "scalyc.h"
using namespace scaly;
namespace scalyc {

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

class Scope : public Action {
public:
    Scope* parent;
    _Array<Definition>* definitions;
    _Array<Action>* actions;
    Scope(Scope* parent);

    virtual bool _isScope();
};

class ModelVisitor : public CommonVisitor {
public:
    Model* model;
    Unit* unit;
    virtual bool openProgram(Program* program);
    virtual bool openCompilationUnit(CompilationUnit* compilationUnit);
    virtual void closeCompilationUnit(CompilationUnit* compilationUnit);
    virtual bool openConstantDeclaration(ConstantDeclaration* constantDeclaration);
    virtual void closeConstantDeclaration(ConstantDeclaration* constantDeclaration);

    virtual bool _isModelVisitor();
};

}
#endif // __scalyc__Model__
