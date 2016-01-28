#ifndef __scaly__scalyc__
#define __scaly__scalyc__

#include "Scaly.h"
#include "Lexer.h"
#include "Visitor.h"
#include "Syntax.h"
#include "MyVisitor.h"
#include "CppVisitor.h"
#include "ParserError.h"
#include "Parser.h"
#include "CompilerError.h"
#include "Compiler.h"

using namespace scaly;

namespace scalyc
{

int _main(Array<String>& arguments);
int compileString(String& text);

}

#endif // __scaly__scalyc__
