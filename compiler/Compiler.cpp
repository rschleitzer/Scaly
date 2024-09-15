#ifndef __Scaly_Compiler__
#define __Scaly_Compiler__

#include "../scaly/containers/Containers.cpp"
#include "../scaly/io/IO.cpp"

#include "Configuration.cpp"
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

CompilerError* compile(Page* _ep, const String& file_name) {
    Region _r;
    auto file_result = scaly::compiler::model::build_program_from_file(_r.get_page(), _ep, file_name);
    if (file_result._tag == Result<Program, ModelError>::Error)
        return new(alignof(CompilerError), _ep) CompilerError(file_result._Error);
    
    return nullptr;
}

CompilerError* compile_and_run_program(Page* _ep, const String& program_text) {
    Region _r;
    auto program_result = scaly::compiler::model::build_program_from_string(_r.get_page(), _ep, program_text);
    if (program_result._tag == Result<Program, ModelError>::Error)
        return new(alignof(CompilerError), _ep) CompilerError(program_result._Error);
    
    auto program = program_result._Ok;
    String string_name(_r.get_page(), "main");
    switch (program.concept.body._tag) {
        case Body::Namespace: {
            auto name_space = program.concept.body._Namespace;
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
