#ifndef _compiler_h
#define _compiler_h
#include "../scaly.h"
using namespace scaly;
using namespace scaly::memory;

using namespace scaly::containers;

using namespace scaly::io;
namespace compiler {
#include "compiler/Errors.h"
#include "compiler/Lexer.h"
#include "compiler/Parser.h"
#include "compiler/Model.h"
#include "compiler/Modeler.h"
#include "compiler/ModelError.h"
#include "compiler/TranspilerError.h"
#include "compiler/Transpiler.h"
#include "compiler/Generator.h"
#include "compiler/CompilerError.h"
#include "compiler/Compiler.h"

    void test_lexer();
    void test_parser();
    void test_compiler();
    void test_compile_scalyc();
    void test();
}
#endif