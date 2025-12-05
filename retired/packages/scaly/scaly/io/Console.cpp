#include "../../scaly.h"
namespace scaly {
namespace io {
using namespace scaly::containers;

namespace Console {


void print(const_char* message) {
    fputs(message, stdout);
}

void print(String message) {
    auto r = Region();
    print(message.to_c_string(r.get_page()));
}

}

}
}