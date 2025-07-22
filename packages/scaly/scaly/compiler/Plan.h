#ifndef _Plan_h
#define _Plan_h
#include "../../scaly.h"
using namespace scaly;
using namespace scaly::memory;
namespace Plan {

struct Type : Object {
    String name;
    Vector<String> fields;

    Type(String name, Vector<String> fields);
};

struct Function : Object {
    String name;
    Vector<String> input;
    String* output;

    Function(String name, Vector<String> input, String* output);
};

struct Module : Object {
    String path;
    String name;
    HashMap<String, Function> functions;

    Module(String path, String name, HashMap<String, Function> functions);
};

}
#endif