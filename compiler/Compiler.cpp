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
#include "TranspilerError.cpp"
#include "Transpiler.cpp"
#include "CompilerError.cpp"

namespace scaly {
namespace compiler {

using namespace scaly::compiler::model;
using namespace scaly::compiler::transpiler;

CompilerError* compile(Page* _ep, String file_name, String program_name) {
    Region _r;
    auto program_result = build_program(_r.get_page(), _ep, file_name, program_name);
    if (program_result._tag == Result<Program, ModelError>::Error)
        return new(alignof(CompilerError), _ep) CompilerError(program_result._Error);
    
    auto program = program_result._Ok;

    auto transpiler = *new(alignof(Transpiler), _r.get_page()) Transpiler();
    auto transpiler_result = transpiler.program(_r.get_page(), program);
    if (transpiler_result != nullptr)
        return new(alignof(CompilerError), _ep) CompilerError(transpiler_result);
    
    return nullptr;
}

}
}
#endif // __Scaly_Compiler__
