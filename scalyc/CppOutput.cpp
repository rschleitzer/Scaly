#include "scalyc.h"
using namespace scaly;
namespace scalyc {

CppModule::CppModule() {
    this->name = nullptr;
    this->header = nullptr;
    this->source = nullptr;
}

CppModule::CppModule(string* name, string* header, string* source) {
    this->name = name;
    this->header = header;
    this->source = source;
}

CppProgram::CppProgram() {
    name = nullptr;
    project = nullptr;
    main = nullptr;
    modules = nullptr;
}

CppProgram::CppProgram(string* name, string* project, string* main, _Array<CppModule>* modules) {
    this->name = name;
    this->project = project;
    this->main = main;
    this->modules = modules;
}


}
