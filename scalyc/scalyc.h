#ifndef __scaly__scalyc__
#define __scaly__scalyc__

#include "Scaly.h"
#include "Lexer.h"
#include "Visitor.h"
#include "Syntax.h"
#include "CppVisitor.h"
#include "HeaderVisitor.h"
#include "SourceVisitor.h"
#include "CompilerError.h"
#include "Parser.h"
#include "OptionsError.h"
#include "Options.h"
#include "Compiler.h"
#include "scalyc.h"

using namespace scaly;
namespace scalyc {
FileError* _main(_Page* page, _Array<string>* arguments);
}

#endif // __scaly__scalyc__
