#ifndef _CompilerError_h
#define _CompilerError_h
#include "../../scaly.h"
using namespace scaly;
using namespace scaly::memory;

struct MultipleMainFunctions : Object {

};

struct MainIsNotAFunction : Object {

};

struct ProgramModuleIsNotANameSpace : Object {

};

struct CompilerError : Object {
    CompilerError(TranspilerError);
    CompilerError(ModelError);
    CompilerError(MultipleMainFunctions);
    CompilerError(MainIsNotAFunction);
    CompilerError(ProgramModuleIsNotANameSpace);
    enum {
        Transpiler,
        Model,
        MultipleMainFunctions,
        MainIsNotAFunction,
        ProgramModuleIsNotANameSpace,
    } _tag;
    union {
        struct TranspilerError _Transpiler;
        struct ModelError _Model;
        struct MultipleMainFunctions _MultipleMainFunctions;
        struct MainIsNotAFunction _MainIsNotAFunction;
        struct ProgramModuleIsNotANameSpace _ProgramModuleIsNotANameSpace;
    };
    String to_string(Page* rp);
};
#endif