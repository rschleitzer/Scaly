namespace scaly {
namespace containers {

using namespace scaly::memory;

struct StringBuilder : Object {
    Array<char> buffer;

    StringBuilder(Array<char> _buffer)
      : buffer(_buffer) {}

    StringBuilder() : StringBuilder(Array<char>()) {}

    StringBuilder(char character) : StringBuilder() {
        this->append_character(character);
    }

    StringBuilder(const String& string) : StringBuilder() {
        this->append_string(string);
    }

    void append_character(char character) {
        this->buffer.add(character);
    }

    void append_string(const String& string) {
        this->buffer.add(Vector<char>(string.get_buffer(), string.get_length()));
    }

    size_t get_length() {
        return this->buffer.get_length();
    }

    String to_string(Page*_rp) {
        return String(_rp, this->buffer.get_buffer(), this->buffer.get_length());
    }
};

}
}
