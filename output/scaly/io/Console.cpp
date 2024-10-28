#include "../../scaly.h"
namespace scaly {
namespace io {using namespace scaly::containers;


void Console::print(Page* ep, const_char* message) {
    fputs(message, stdout);
}

void Console::print(Page* ep, String message) {
    auto r = Region();
    print(r.get_page(), message.to_c_string(r.get_page()));
}

}
}