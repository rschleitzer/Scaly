#ifndef __scaly__scalycpp__
#define __scaly__scalycpp__

#include "Scaly.h"
#include "Lexer.h"
#include "OptionsError.h"
#include "Parser.h"
#include "CommonVisitor.h"
#include "CppVisitor.h"
#include "Model.h"
#include "Options.h"
#include "Compiler.h"
#include "scalycpp.h"

using namespace scaly;
namespace scalycpp {
FileError* _main(_Page* page, _Array<string>* arguments);
}

#endif // __scaly__scalycpp__
