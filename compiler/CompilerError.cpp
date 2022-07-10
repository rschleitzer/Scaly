#ifndef __Scaly_CompilerError__
#define __Scaly_CompilerError__

namespace scaly {
namespace compiler {

using namespace scaly::compiler::model;

struct MultipleMainFunctions {};
struct MainIsNotAFunction {};
struct ProgramModuleIsNotANameSpace {};

struct CompilerError : Object {
    CompilerError(ModelError modelError) : _tag(Model), _Model(modelError) {}
    CompilerError(MultipleMainFunctions multipleMainFunctions) : _tag(MultipleMainFunctions), _MultipleMainFunctions(multipleMainFunctions) {}
    CompilerError(MainIsNotAFunction mainIsNotAFunction) : _tag(MainIsNotAFunction), _MainIsNotAFunction(mainIsNotAFunction) {}
    CompilerError(ProgramModuleIsNotANameSpace programModuleIsNotANameSpace) : _tag(ProgramModuleIsNotANameSpace), _ProgramModuleIsNotANameSpace(programModuleIsNotANameSpace) {}
    enum {
        Model,
        MultipleMainFunctions,
        MainIsNotAFunction,
        ProgramModuleIsNotANameSpace,
    } _tag;
    union {
        ModelError _Model;
        struct MultipleMainFunctions _MultipleMainFunctions;
        struct MainIsNotAFunction _MainIsNotAFunction;
        struct ProgramModuleIsNotANameSpace _ProgramModuleIsNotANameSpace;
    };

    String generate_compiler_message(Region& _pr, Page* _rp) {
        Region _r(_pr);
        StringBuilder& message_builder = *new(alignof(StringBuilder), _r.page) StringBuilder();
        switch (_tag) {
            case Model:
                message_builder.append_string(String(_r.page, "A problem with the model has occurred."));
            break;
            case MultipleMainFunctions:
                message_builder.append_string(String(_r.page, "Multiple main functions have been defined. There can only be one main function."));
            break;
            case MainIsNotAFunction:
                message_builder.append_string(String(_r.page, "The item called main is not a function."));
            break;
            case ProgramModuleIsNotANameSpace:
                message_builder.append_string(String(_r.page, "The root concept of a program is not a name space."));
            break;
        }
        return message_builder.to_string(_rp);     
    }
};

}
}
#endif // __Scaly_CompilerError__
