#include "Containers.cpp"

namespace scaly {
namespace containers {

using namespace scaly::memory;

    StringBuilder::StringBuilder(Array<char> _buffer)
      : buffer(_buffer) {}

    StringBuilder::StringBuilder() : StringBuilder(Array<char>()) {}

    StringBuilder::StringBuilder(char character) : StringBuilder() {
        this->append(character);
    }

    StringBuilder::StringBuilder(const String& string) : StringBuilder() {
        this->append(string);
    }

    StringBuilder::StringBuilder(const char* c_string) : StringBuilder() {
        this->append(c_string);
    }

    void StringBuilder::append(char character) {
        this->buffer.add(character);
    }

    void StringBuilder::append(const String& string) {
        this->buffer.add(Vector<char>(string.get_buffer(), string.get_length()));
    }

    void StringBuilder::append(const char* c_string) {
        auto length = strlen(c_string);
        if (length == 0)
            return;

        this->buffer.add(Vector<char>((char*)c_string, strlen(c_string)));
    }

    size_t StringBuilder::get_length() {
        return this->buffer.get_length();
    }

    String StringBuilder::to_string(Page*_rp) {
        return String(_rp, this->buffer.get_buffer(), this->buffer.get_length());
    }

}
}
