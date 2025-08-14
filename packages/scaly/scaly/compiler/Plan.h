#ifndef _Plan_h
#define _Plan_h
#include "../../scaly.h"
using namespace scaly;
using namespace scaly::memory;
namespace Plan {

struct Module : Object {
    String path;
    String name;
    HashMap<String, Type> types;
    HashMap<String, Function> functions;

    Module(String path, String name, HashMap<String, Type> types, HashMap<String, Function> functions);
};

struct Type : Object {
    String name;
    Vector<String> fields;

    Type(String name, Vector<String> fields);
};

struct Function : Object {
    String name;
    Vector<Argument> input;
    String output;
    Vector<Block> blocks;

    Function(String name, Vector<Argument> input, String output, Vector<Block> blocks);
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