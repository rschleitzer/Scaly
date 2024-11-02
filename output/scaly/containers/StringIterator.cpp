#include "../../scaly.h"
namespace scaly {
namespace containers {

StringIterator::StringIterator(char* current, char* last) : current(current), last(last) {}

StringIterator::StringIterator(String string) {
    if (string.get_length()>0) {
        const auto buffer = string.get_buffer();
        current = buffer;
        last = buffer+string.get_length();
    };
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