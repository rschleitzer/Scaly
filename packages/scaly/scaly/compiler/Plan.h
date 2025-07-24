#ifndef _Plan_h
#define _Plan_h
#include "../../scaly.h"
using namespace scaly;
using namespace scaly::memory;
namespace Plan {

struct Module : Object {
    String path;
    String name;
    Vector<Type> types;
    HashMap<String, Type> type_map;
    Vector<Function> functions;
    HashMap<String, Function> function_map;

    Module(String path, String name, Vector<Type> types, HashMap<String, Type> type_map, Vector<Function> functions, HashMap<String, Function> function_map);
};

struct Type : Object {
    String name;
    Vector<String> fields;

    Type(String name, Vector<String> fields);
};

struct Instruction : Object {
    String name;
    String type;
    Vector<String> input;

    Instruction(String name, String type, Vector<String> input);
};

struct Block : Object {
    Vector<Instruction> instructions;

    Block(Vector<Instruction> instructions);
};

struct Function : Object {
    String name;
    Vector<String> input;
    String output;
    Vector<Block> blocks;

    Function(String name, Vector<String> input, String output, Vector<Block> blocks);
};

}
#endif