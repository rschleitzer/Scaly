#include "Containers.cpp"

namespace scaly {
namespace containers {

using namespace scaly::memory;

    StringBuilder::StringBuilder(Array<char> _buffer)
      : buffer(_buffer) {}

    StringBuilder::StringBuilder() : StringBuilder(Array<char>()) {}

    StringBuilder::StringBuilder(char character) : StringBuilder() {
        this->append_character(character);
    }

    StringBuilder::StringBuilder(const String& string) : StringBuilder() {
        this->append_string(string);
    }

    void StringBuilder::append_character(char character) {
        this->buffer.add(character);
    }

    void StringBuilder::append_string(const String& string) {
        this->buffer.add(Vector<char>(string.get_buffer(), string.get_length()));
    }

    size_t StringBuilder::get_length() {
        return this->buffer.get_length();
    }

    String StringBuilder::to_string(Page*_rp) {
        return String(_rp, this->buffer.get_buffer(), this->buffer.get_length());
    }

}
}
