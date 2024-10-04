namespace scaly {
namespace containers {

using namespace scaly::memory;

struct StringBuilder : Object {
    Array<char> buffer;

    StringBuilder(Array<char> _buffer);
    StringBuilder();
    StringBuilder(char character);
    StringBuilder(const String& string);
    StringBuilder(const char* c_string);
    void append(char character);
    void append(const String& string);
    void append(const char* c_string);
    size_t get_length();
    String to_string(Page*_rp);
};

}
}
