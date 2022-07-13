#ifndef __Scaly_Compiler__
#define __Scaly_Compiler__

#include "../runtime/containers/Containers.cpp"
#include "../runtime/io/IO.cpp"

#include "Lexer.cpp"
#include "Errors.cpp"
#include "Parser.cpp"
#include "Model.cpp"
#include "ModelError.cpp"
#include "Modeler.cpp"
#include "Generator.cpp"
#include "CompilerError.cpp"

namespace scaly {
namespace compiler {

using namespace scaly::compiler::model;

CompilerError* compile_and_run_program(Page* _ep, const String& program, Vector<String>& arguments) {
    Region _r;
    auto module_result = scaly::compiler::model::build_program_module(_r.page, _ep, program);
    if (module_result._tag == Result<Module, ModelError>::Error)
        return new(alignof(CompilerError), _ep) CompilerError(module_result._Error);
    
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
                        return new(alignof(CompilerError), _ep) CompilerError(MultipleMainFunctions());

                    auto main = main_functions[0];
                    
                    return nullptr;
                }
                default:
                    return new(alignof(CompilerError), _ep) CompilerError(MainIsNotAFunction());
            }
        }
        break;

        default:
            return new(alignof(CompilerError), _ep) CompilerError(ProgramModuleIsNotANameSpace());
    }
}

}
}
#endif // __Scaly_Compiler__
