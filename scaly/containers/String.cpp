#include "Containers.cpp"

namespace scaly {
namespace containers {

using namespace scaly::memory;

String::String() {
    data = nullptr;
}

String::String(Page* _rp, size_t length) {
    char length_array[PACKED_SIZE];
    auto rest = length;

    size_t counter = 0;
    while (rest >= 0x80) {
        length_array[counter] = (char)rest | 0x80;
        rest >>= 7;
        counter += 1;
    }

    length_array[counter] = (char)rest;
    auto overall_length = counter + 1 + length;
    data = (char*)_rp->allocate_raw(overall_length, 1);
    memcpy(data, length_array, counter + 1);
}

String::String(Page* _rp, const char* other, size_t length) {
    if (length == 0) {
        this->data = nullptr;
        return;
    }

    char length_array[PACKED_SIZE];
    auto rest = length;

    size_t counter = 0;
    while (rest >= 0x80) {
        length_array[counter] = (char)rest | 0x80;
        rest >>= 7;
        counter += 1;
    }

    length_array[counter] = (char)rest;
    auto overall_length = counter + 1 + length;
    this->data = (char*)_rp->allocate_raw(overall_length, 1);
    memcpy(this->data, length_array, counter + 1);
    memcpy((void*)(this->data + counter + 1), other, length);
}

String::String(Page* _rp, Vector<char> other) : String(_rp, other.data, other.length) {}

String::String(Page* _rp, const char* c_string) : String(_rp, c_string, strlen(c_string)) {}

String::String(Page* _rp, const String& other) {
    if (other.data == nullptr)
        return;

    size_t length = 0;
    auto bit_count = 0;
    auto index = 0;
    while (true) {
        if (bit_count == PACKED_SIZE * 7) {
            // Bad string length
            exit(12);
        }

        char byte = *(other.data + index);
        length |= ((size_t)(byte & 0x7F)) << bit_count;
        if ((byte & 0x80) == 0)
            break;

        bit_count += 7;
        index += 1;
    }
    auto overall_length = index + 1 + length;
    this->data = (char*)_rp->allocate_raw(overall_length, 1);
    memcpy(this->data, other.data, overall_length);
}

String::String(Page* _rp, char character) {
    this->data = (char*)_rp->allocate_raw(2, 1);
    this->data[0] = 1;
    this->data[1] = character;
}

const char* String::to_c_string(Page* _rp) const {
    size_t length = 0;
    size_t index = 0;
    if (this->data != nullptr) {
        auto bit_count = 0;
        while (true) {
            if (bit_count == PACKED_SIZE * 7)
                // Bad string length
                exit(11);

            char byte = *(this->data + index);
            length |= ((size_t)(byte & 0x7F)) << bit_count;
            if ((byte & 0x80) == 0)
                break;
            bit_count += 7;
            index += 1;
        }
    }

    auto dest = _rp->allocate_raw(length + 1, 1);
    if (this->data != nullptr) {
        memcpy(dest, this->data + index + 1, length);
    }
    *((char*)dest + index + length) = 0;

    return (const char*)dest;
}

char* String::get_buffer() const {
    if (this->data == nullptr)
        return nullptr;

    size_t length = 0;
    auto bit_count = 0;
    size_t index = 0;
    while (true) {
        if (bit_count == PACKED_SIZE * 7)
            // Bad string length
            exit(11);

        char byte = *(this->data + index);
        length |= ((size_t)(byte & 0x7F)) << bit_count;
        if ((byte & 0x80) == 0)
            break;
        bit_count += 7;
        index += 1;
    }

    return this->data + index + 1;
}

size_t String::get_length() const {
    if (this->data == nullptr)
        return 0;

    size_t result = 0;
    auto bit_count = 0;
    auto index = 0;
    while (true) {
        if (bit_count == PACKED_SIZE * 7) {
            // Bad string length
            exit(12);
        }

        char byte = *(this->data + index);
        result |= ((size_t)(byte & 0x7F)) << bit_count;
        if ((byte & 0x80) == 0)
            break;

        bit_count += 7;
        index += 1;
    }
    return result;
}

bool String::equals(String other) const {
    size_t length = 0;
    auto index = 0;
    if (this->data != nullptr) {
        auto bit_count = 0;
        while (true) {
            if (bit_count == PACKED_SIZE * 7) {
                // Bad string length
                exit(13);
            }

            char byte = *(this->data + index);
            length |= ((size_t)(byte & 0x7F)) << bit_count;
            if ((byte & 0x80) == 0) {
                break;
            }
            bit_count += 7;
            index += 1;
        }
    }

    if (length != other.get_length()) {
        return false;
    }

    if (this->data == nullptr)
        return true;

    return memcmp(this->data + index + 1, other.data + index + 1, length) == 0;
}

bool String::equals(Vector<char> other) const {
    size_t length = 0;
    auto index = 0;
    if (this->data != nullptr) {
        auto bit_count = 0;
        while (true) {
            if (bit_count == PACKED_SIZE * 7) {
                // Bad string length
                exit(13);
            }

            char byte = *(this->data + index);
            length |= ((size_t)(byte & 0x7F)) << bit_count;
            if ((byte & 0x80) == 0) {
                break;
            }
            bit_count += 7;
            index += 1;
        }
    }

    if (length != other.length) {
        return false;
    }

    if (this->data == nullptr)
        return true;

    return memcmp(this->data + index + 1, other.data, length) == 0;
}

size_t String::hash() const {
    size_t length = 0;
    if (this->data != nullptr) {
        auto bit_count = 0;
        auto index = 0;
        while (true) {
            if (bit_count == PACKED_SIZE * 7) {
                // Bad string length
                exit(14);
            }

            char byte = *(this->data + index);
            length |= ((size_t)(byte & 0x7F)) << bit_count;
            if ((byte & 0x80) == 0) {
                break;
            }
            bit_count += 7;
            index += 1;
        }
        return scaly::containers::hash(this->data + index + 1, length);
    }
    return 0;
}

}
}
