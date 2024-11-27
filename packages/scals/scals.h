#ifndef _scals_h
#define _scals_h
#include "../scaly/scaly.h"
using namespace scaly;
using namespace scaly::memory;

using namespace scaly::json;
namespace scals {
#include "scals/Lsp.h"

}
void process_message(String message);
String* get_message(Page* rp, int fd);
#endif