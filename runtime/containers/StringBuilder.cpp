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

    void append_character(char character) {
        this->buffer.add(character);
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
