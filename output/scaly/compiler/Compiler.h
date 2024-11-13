#ifndef _Compiler_h
#define _Compiler_h
#include "../../scaly.h"
using namespace scaly;
using namespace scaly::memory;

using namespace scaly::containers;

using namespace scaly::io;

using namespace scaly::compiler::model;

using namespace scaly::compiler::transpiler;
namespace compiler {

    Result<Void, CompilerError> compile(Page* ep, String file_name, String program_name);
}
#endif