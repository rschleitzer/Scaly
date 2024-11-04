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

    void test_lexer();
    void test_parser();
    void test();
}
#endif