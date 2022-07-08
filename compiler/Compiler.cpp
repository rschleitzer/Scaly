#ifndef __Scaly_Compiler__
#define __Scaly_Compiler__

#include "../runtime/containers/Containers.cpp"
#include "../runtime/io/IO.cpp"

#include "Lexer.cpp"
#include "Errors.cpp"
#include "Parser.cpp"
#include "Model.cpp"
#include "Modeler.cpp"
#include "Generator.cpp"


namespace scaly {
namespace compiler {

using namespace scaly::compiler::model;

int compile_and_run_program(Region& _pr, const String& program, Vector<String>& arguments) {
    Region _r(_pr);
    auto module_result = scaly::compiler::model::build_program_module(_r, _r.page, _r.page, program);
    if (module_result._tag == Result<Module, ModelError>::Error)
        return -1;
    
    auto module = module_result._Ok;
    String string_name(_r.page, "main");
    switch (module.concept.body._tag) {
        case Body::NameSpace: {
            auto name_space = module.concept.body._NameSpace;
            auto main_functions_symbol = name_space.code.symbols[string_name];
            switch (main_functions_symbol->_tag) {
                case Nameable::Functions: {
                    auto main_functions = main_functions_symbol->_Functions;
                    if (main_functions.length != 1)
                        return -3;

                    auto main = main_functions[0];
                    
                    return 0;                }
                default:
                    return -2;
            }
        }
        break;

        default:
            return -4;
    }
}

}
}
#endif // __Scaly_Compiler__
