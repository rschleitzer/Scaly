#include "../../scaly.h"
namespace scaly {
namespace io {
using namespace scaly::containers;


void Console::print(const_char* message) {
    fputs(message, stdout);
}

void Console::print(String message) {
    auto r = Region();
    print(message.to_c_string(r.get_page()));
}

}
}