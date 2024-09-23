namespace scaly {
namespace containers {

using namespace scaly::memory;

const size_t PACKED_SIZE = sizeof(size_t) * 8 / 7;

struct String : Object {
    char* data;

    String(Page* _rp, size_t length);
    String(Page* _rp, const char* other, size_t length);
    String(Page* _rp, Vector<char> other);
    String(Page* _rp, const char* c_string);
    String(Page* _rp, const String& other);
    String(Page* _rp, char character);
    const char* to_c_string(Page* _rp) const;
    char* get_buffer() const;
    size_t get_length() const;
    bool equals(String other) const;
    bool equals(Vector<char> other) const;
    size_t hash() const;
};

}
}
