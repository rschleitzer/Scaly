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

int compile_and_run_program(Region& _pr, const String& program, Vector<String>& arguments) {
    Region _r(_pr);
    auto module = scaly::compiler::model::build_program_module(_r, _r.page, _r.page, program);
    return 0;
}

}
}
#endif // __Scaly_Compiler__
