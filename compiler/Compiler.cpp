#ifndef __Scaly_Compiler__
#define __Scaly_Compiler__

#include "../runtime/containers/Containers.cpp"

#include "Lexer.cpp"
#include "Errors.cpp"
#include "Parser.cpp"
#include "Modeler.cpp"
#include "Generator.cpp"


namespace scaly {
namespace compiler {

int compile_and_run_program(Region& _pr, String& program, Vector<String>& arguments) {
    Region _r = Region::create(_pr);
    return 0;
}

}
}
#endif // __Scaly_Compiler__
