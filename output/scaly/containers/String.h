#ifndef _String_h
#define _String_h
#include "../../scaly.h"
using namespace scaly;
using namespace scaly::memory;
const int PACKED_SIZE = 9;

struct String : Object {
    char* data;
    String (char* data);
    String(Page* rp, size_t length);
    String(Page* rp, const_char* other, size_t length);
    String(Page* rp, Vector<char> other);
    String(Page* rp, const_char* c_string);
    String(Page* rp, String other);
    String(Page* rp, char character);
    char* get_buffer();
    const_char* to_c_string(Page* rp);
    size_t get_length();
    bool equals(String other);
    bool equals(const_char* other);
    bool equals(Vector<char> other);
    size_t hash();
};

#endif