#ifndef _Plan_h
#define _Plan_h
#include "../../scaly.h"
using namespace scaly;
using namespace scaly::memory;
namespace Plan {

struct Source : Object {
    String path;
    String name;

    Source(String path, String name);
};

struct Compilation : Object {
    Source* source;
    HashMap<String, Type> types;
    HashMap<String, Function> functions;

    Compilation(Source* source, HashMap<String, Type> types, HashMap<String, Function> functions);
};

struct Type : Object {
    String name;
    Vector<String> fields;

    Type(String name, Vector<String> fields);
};

struct Function : Object {
    Source* source;
    String name;
    Vector<Argument> input;
    String output;
    Vector<Block> blocks;

    Function(Source* source, String name, Vector<Argument> input, String output, Vector<Block> blocks);
};

struct Argument : Object {
    String name;
    String type;

    Argument(String name, String type);
};

struct Block : Object {
    String name;
    Vector<Instruction> instructions;

    Block(String name, Vector<Instruction> instructions);
};

struct Instruction : Object {
    String* result;
    String name;
    Vector<String> args;

    Instruction(String* result, String name, Vector<String> args);
};

}
#endif