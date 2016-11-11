#ifndef __scalyc__CppOutput__
#define __scalyc__CppOutput__
#include "scalyc.h"
using namespace scaly;
namespace scalyc {

class CppModule : public Object {
public:
    String* name;
    String* header;
    String* source;
    CppModule();
    CppModule(String* name, String* header, String* source);

};

class CppProgram : public Object {
public:
    CppProgram();
    CppProgram(String* name, String* project, String* main, _Vector<CppModule>* modules);
    String* name;
    String* project;
    String* main;
    _Vector<CppModule>* modules;

};

}
#endif // __scalyc__CppOutput__
