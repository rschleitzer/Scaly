namespace scaly {
namespace containers {

using namespace scaly::memory;

struct StringBuilder : Object {
    Array<char> buffer;

    StringBuilder(Array<char> _buffer);
    StringBuilder();
    StringBuilder(char character);
    StringBuilder(const String& string);
    void append_character(char character);
    void append_string(const String& string);
    size_t get_length();
    String to_string(Page*_rp);
};

}
}
