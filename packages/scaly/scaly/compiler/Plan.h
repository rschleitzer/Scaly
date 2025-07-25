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

struct FMul : Object {
    String l;
    String r;
    String result;

    FMul(String l, String r, String result);
};

struct Ret : Object {
    String v;

    Ret(String v);
};

struct RetVoid : Object {

};

struct Instruction : Object {
    Instruction(FMul);
    Instruction(Ret);
    Instruction(RetVoid);
    enum {
        FMul,
        Ret,
        RetVoid,
    } _tag;
    union {
        struct FMul _FMul;
        struct Ret _Ret;
        struct RetVoid _RetVoid;
    };
};
}
#endif