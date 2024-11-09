#include "../../scaly.h"
namespace scaly {
namespace compiler {

String CompilerError::to_string(Page* rp) {
    auto r = Region();
    StringBuilder& message_builder = *new (alignof(StringBuilder), r.get_page()) StringBuilder();
    {
        auto _result = *this;
        switch (_result._tag)
        {
            case CompilerError::Transpiler:
            {
                auto t = _result._Transpiler;
                message_builder.append(t.to_string(rp));
                break;
            }
            case CompilerError::Model:
            {
                auto m = _result._Model;
                message_builder.append(m.to_string(rp));
                break;
            }
            case CompilerError::MultipleMainFunctions:
            {
                auto mm = _result._MultipleMainFunctions;
                message_builder.append("Multiple main functions have been defined. There can only be one main function.");
                break;
            }
            case CompilerError::MainIsNotAFunction:
            {
                auto mf = _result._MainIsNotAFunction;
                message_builder.append("The item called main is not a function.");
                break;
            }
            case CompilerError::ProgramModuleIsNotANameSpace:
            {
                auto pm = _result._ProgramModuleIsNotANameSpace;
                message_builder.append("The root concept of a program is not a name space.");
                break;
            }
        }
    };
    return message_builder.to_string(rp);
}
CompilerError::CompilerError(struct TranspilerError _Transpiler) : _tag(Transpiler), _Transpiler(_Transpiler) {}

CompilerError::CompilerError(struct ModelError _Model) : _tag(Model), _Model(_Model) {}

CompilerError::CompilerError(struct MultipleMainFunctions _MultipleMainFunctions) : _tag(MultipleMainFunctions), _MultipleMainFunctions(_MultipleMainFunctions) {}

CompilerError::CompilerError(struct MainIsNotAFunction _MainIsNotAFunction) : _tag(MainIsNotAFunction), _MainIsNotAFunction(_MainIsNotAFunction) {}

CompilerError::CompilerError(struct ProgramModuleIsNotANameSpace _ProgramModuleIsNotANameSpace) : _tag(ProgramModuleIsNotANameSpace), _ProgramModuleIsNotANameSpace(_ProgramModuleIsNotANameSpace) {}


}
}