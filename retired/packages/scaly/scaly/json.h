#ifndef _json_h
#define _json_h
#include "../scaly.h"
using namespace scaly;
using namespace scaly::memory;

using namespace scaly::containers;

using namespace scaly::io;
namespace json {
#include "json/JsonErrors.h"
#include "json/JsonLexer.h"
#include "json/JsonParser.h"

void test_parser();
void test();
}
#endif