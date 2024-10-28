#include "../../scaly.h"
namespace scaly {
namespace containers {

StringIterator::StringIterator(String string) {
    const auto buffer = string.get_buffer();
    current = buffer;
    last = buffer+string.get_length();
}

char* StringIterator::next() {
    if (current == last) {
        return nullptr;
    }
    else {
        const auto ret = current;
        current = current+1;
        return ret;
    };
}

}
}