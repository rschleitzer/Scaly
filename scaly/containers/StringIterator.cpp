#include "Containers.cpp"

namespace scaly {
namespace containers {

StringIterator::StringIterator(String string) {
    auto buffer = string.get_buffer();
    this->current = buffer;
    this->last = buffer + string.get_length();
}

char* StringIterator::next() {
    if (this->current == last) {
        return nullptr;
    } else {
        auto ret = this->current;
        this->current++;
        return ret;
    }
}

}
}
