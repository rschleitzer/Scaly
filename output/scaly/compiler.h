#ifndef _compiler_h
#define _compiler_h
#include "../scaly.h"
using namespace scaly;
using namespace scaly::memory;
namespace compiler {
#include "compiler/Errors.h"
#include "compiler/Lexer.h"
#include "compiler/Parser.h"

    void test_lexer();
    void test();
}
#endif