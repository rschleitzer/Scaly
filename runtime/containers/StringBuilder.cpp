namespace scaly {
namespace containers {

using namespace scaly::memory;

struct StringBuilder : Object {
    Array<char> buffer;

    StringBuilder(Array<char> _buffer)
    :   buffer(_buffer) {}

    static StringBuilder* create(Page* _rp) {
        return new(alignof(StringBuilder), _rp) StringBuilder(Array<char>());
    }

    static StringBuilder* from_character(Page* _rp, char character) {
        auto string_builder = StringBuilder::create(_rp);
        string_builder->append_character(character);
        return string_builder;
    }

    void append_character(char character) {
        this->buffer.add(character);
    }

    size_t get_length() {
        return this->buffer.get_length();
    }

    String* to_string(Page*_rp) {
        return String::create(_rp, this->buffer.get_buffer(), this->buffer.get_length());
    }
};

}
}
