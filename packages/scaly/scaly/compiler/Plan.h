#ifndef _Plan_h
#define _Plan_h
#include "../../scaly.h"
using namespace scaly;
using namespace scaly::memory;
namespace Plan {

struct Type : Object {
    String name;

    Type(String name);
};

struct Function : Object {
    String name;
    Vector<Type> input;
    Type output;

    Function(String name, Vector<Type> input, Type output);
};

struct Module : Object {
    String path;
    String name;

    Module(String path, String name);
};

}
#endif