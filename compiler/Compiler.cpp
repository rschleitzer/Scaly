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
    auto main_functions = module.concept.functions[string_name];
    if (main_functions->length != 1)
        return -2;

    auto main = main_functions[0];
    
    return 0;
}

}
}
#endif // __Scaly_Compiler__
