#ifndef __scalyc__CppOutput__
#define __scalyc__CppOutput__
#include "scalyc.h"
using namespace scaly;
namespace scalyc {

class CppModule : public Object {
public:
    string* name;
    string* header;
    string* source;
    CppModule();
    CppModule(string* name, string* header, string* source);

};

class CppProgram : public Object {
public:
    CppProgram();
    CppProgram(string* name, string* project, string* main, _Array<CppModule>* modules);
    string* name;
    string* project;
    string* main;
    _Array<CppModule>* modules;

};

}
#endif // __scalyc__CppOutput__
