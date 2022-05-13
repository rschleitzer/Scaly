namespace scaly {
namespace containers {

using namespace scaly::memory;

const size_t PACKED_SIZE = sizeof(size_t) * 8 / 7;

struct String : Object {
    char* data;

    String(char* data)
    : data(data) {}

    static String* create(Page* _rp, const char* data, size_t length) {
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
        auto pointer = _rp->allocate_raw(overall_length, 1);
        memcpy(pointer, length_array, counter + 1);
        memcpy((char*)pointer + counter + 1, data, length);

        return new (alignof(String), _rp) String((char*)pointer);
    }

    static String* from_c_string(Page* _rp, const char* c_string) {
        auto length = strlen(c_string);
        return String::create(_rp, c_string, length);
    }

    String* copy(Page* _rp) {
        size_t length = 0;
        auto bit_count = 0;
        auto index = 0;
        while (true) {
            if (bit_count == PACKED_SIZE * 7) {
                // Bad string length
                exit(12);
            }

            char byte = *(this->data + index);
            length |= ((size_t)(byte & 0x7F)) << bit_count;
            if ((byte & 0x80) == 0)
                break;

            bit_count += 7;
            index += 1;
        }
        auto overall_length = index + 1 + length;
        auto pointer = _rp->allocate_raw(overall_length, 1);
        memcpy(pointer, this->data, overall_length);
        return new (alignof(String), _rp) String((char*)pointer);
    }

    static String* from_character(Page* _rp, char character) {
        return String::create(_rp, (const char*)&character, 1);
    }

    const char* to_c_string(Page* _rp) {
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

        auto dest = _rp->allocate_raw(length + 1, 1);
        memcpy(dest, this->data + index + 1, length);
        *((char*)dest + index + 1 + length) = 0;

        return (const char*)dest;
    }

    const char* get_buffer() {
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

    String* append_character(Page* _rp, char c) {
        auto s = String::create(_rp, this->data, this->get_length() + 1);
        auto data = s->data;
        auto char_pointer = data + this->get_length() - 1;
        *char_pointer = c;
        return s;
    }

    size_t get_length() {
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

    bool equals(String& other) {
        size_t length = 0;
        auto bit_count = 0;
        auto index = 0;
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

        if (length != other.get_length()) {
            return false;
        }

        return memcmp(this->data + index + 1, other.data + index + 1, length) == 0;
    }

    size_t hash() {
        // std::string s(this->data, this->get_length());
        // return std::hash<std::string>{}(s);
        size_t length = 0;
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

};

struct StringIterator {
    char* current;
    char* last;

    static StringIterator create(String string) {
        auto buffer = string.get_buffer();
        return StringIterator {
            .current = (char*)buffer,
            .last = (char*)buffer + string.get_length() };
    }

    char* next() {
        if (this->current == last) {
            return nullptr;
        } else {
            auto ret = this->current;
            this->current++;
            return ret;
        }
    }
};

}
}
