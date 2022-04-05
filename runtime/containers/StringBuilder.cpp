using namespace scaly::memory;

struct StringBuilder : Object {
    Array<char> buffer;

    static StringBuilder* create(Page* _rp) {
        return new(alignof(StringBuilder), _rp) StringBuilder {
            .buffer = scaly::containers::Array<char>{
                .length = 0,
                .vector = nullptr,
            },
        };
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
