#ifndef _StringBuilder_h
#define _StringBuilder_h
#include "../../scaly.h"
using namespace scaly;
using namespace scaly::memory;

struct StringBuilder : Object {
    Array<char> buffer;
    StringBuilder (Array<char> buffer);
    StringBuilder();
    StringBuilder(char character);
    StringBuilder(String string);
    StringBuilder(const_char* c_string);
    String to_string(Page* rp);
    size_t get_length();
    void append(char character);
    void append(String string);
    void append(const_char* c_string);
};

#endif