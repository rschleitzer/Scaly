#include "../../scaly.h"
namespace scaly {
namespace containers {

String::String(char* data) : data(data) {}

String::String() : data(nullptr) {}

String::String(Page* rp, size_t length) {
    char length_array[PACKED_SIZE];
    auto rest = length;
    size_t counter = 0;
    while (rest>=0x80) {
        *(length_array+counter) = (char)rest|0x80;
        rest = rest>>7;
        counter = counter+1;
    };
    *(length_array+counter) = (char)rest;
    const auto overall_length = counter+1+length;
    data = (char*)(*rp).allocate(overall_length, 1);
    memcpy(data, length_array, counter+1);
}

String::String(Page* rp, const_char* other, size_t length) {
    if (length == 0) {
        data = nullptr;
        return;
    };
    char length_array[PACKED_SIZE];
    auto rest = length;
    size_t counter = 0;
    while (rest>=0x80) {
        *(length_array+counter) = (char)rest|0x80;
        rest = rest>>7;
        counter = counter+1;
    };
    *(length_array+counter) = (char)rest;
    const auto overall_length = counter+1+length;
    data = (char*)(*rp).allocate(overall_length, 1);
    memcpy(data, length_array, counter+1);
    memcpy((void*)(data+counter+1), other, length);
}

String::String(Page* rp, Vector<char> other) : String(rp, other.data, other.length){
}

String::String(Page* rp, const_char* c_string) : String(rp, c_string, strlen(c_string)){
}

String::String(Page* rp, String other) {
    if (other.data == nullptr) 
        return;
    size_t length = 0;
    auto bit_count = 0;
    auto index = 0;
    while (true) {
        if (bit_count == PACKED_SIZE*7) 
            exit(12);
        const auto byte = *(other.data+index);
        length = length|((size_t)(byte&0x7f))<<bit_count;
        if ((byte&0x80) == 0) 
            break;
        bit_count = bit_count+7;
        index = index+1;
    };
    auto overall_length = index+1+length;
    data = (char*)(*rp).allocate(overall_length, 1);
    memcpy(data, other.data, overall_length);
}

String::String(Page* rp, char character) {
    data = (char*)(*rp).allocate(2, 1);
    *data = 1;
    *(data+1) = character;
}

char* String::get_buffer() {
    if (data == nullptr) 
        return nullptr;
    size_t length = 0;
    auto bit_count = 0;
    size_t index = 0;
    while (true) {
        if (bit_count == PACKED_SIZE*7) 
            exit(11);
        char byte = *(data+index);
        length = length|((size_t)(byte&0x7f))<<bit_count;
        if ((byte&0x80)==0) 
            break;
        bit_count = bit_count+7;
        index = index+1;
    };
    return data+index+1;
}

const_char* String::to_c_string(Page* rp) {
    size_t length = 0;
    auto bit_count = 0;
    size_t index = 0;
    while (true) {
        if (bit_count == PACKED_SIZE*7) 
            exit(11);
        char byte = *(data+index);
        length = length|((size_t)(byte&0x7f))<<bit_count;
        if ((byte&0x80)==0) 
            break;
        bit_count = bit_count+7;
        index = index+1;
    };
    auto dest = (*rp).allocate(length+1, 1);
    memcpy(dest, data+index+1, length);
    *((char*)dest+index+length) = 0;
    return (const_char*)dest;
}

size_t String::get_length() {
    if (data == nullptr) 
        return 0;
    size_t result = 0;
    auto bit_count = 0;
    size_t index = 0;
    while (true) {
        if (bit_count == PACKED_SIZE*7) 
            exit(11);
        char byte = *(data+index);
        result = result|((size_t)(byte&0x7f))<<bit_count;
        if ((byte&0x80)==0) 
            break;
        bit_count = bit_count+7;
        index = index+1;
    };
    return result;
}

bool String::equals(String other) {
    size_t length = 0;
    size_t index = 0;
    if (data != nullptr) {
        auto bit_count = 0;
        while (true) {
            if (bit_count == PACKED_SIZE*7) 
                exit(11);
            char byte = *(data+index);
            length = length|((size_t)(byte&0x7f))<<bit_count;
            if ((byte&0x80)==0) 
                break;
            bit_count = bit_count+7;
            index = index+1;
        };
    };
    if (length != other.get_length()) 
        return false;
    if (data == nullptr) 
        return true;
    return memcmp(data+index+1, other.data+index+1, length) == 0;
}

bool String::equals(const_char* other) {
    size_t length = 0;
    size_t index = 0;
    if (data != nullptr) {
        auto bit_count = 0;
        while (true) {
            if (bit_count == PACKED_SIZE*7) 
                exit(13);
            char byte = *(data+index);
            length = length|((size_t)(byte&0x7f))<<bit_count;
            if ((byte&0x80)==0) 
                break;
            bit_count = bit_count+7;
            index = index+1;
        };
    };
    if (length != strlen(other)) 
        return false;
    if (data == nullptr) 
        return true;
    return memcmp(data+index+1, other, length) == 0;
}

bool String::equals(Vector<char> other) {
    size_t length = 0;
    auto bit_count = 0;
    size_t index = 0;
    while (true) {
        if (bit_count == PACKED_SIZE*7) 
            exit(11);
        char byte = *(data+index);
        length = length|((size_t)(byte&0x7f))<<bit_count;
        if ((byte&0x80)==0) 
            break;
        bit_count = bit_count+7;
        index = index+1;
    };
    if (length != other.length) 
        return false;
    return memcmp(data+index+1, other.data, length) == 0;
}

size_t String::hash() {
    size_t length = 0;
    auto bit_count = 0;
    size_t index = 0;
    while (true) {
        if (bit_count == PACKED_SIZE*7) 
            exit(11);
        char byte = *(data+index);
        length = length|((size_t)(byte&0x7f))<<bit_count;
        if ((byte&0x80)==0) 
            break;
        bit_count = bit_count+7;
        index = index+1;
    };
    return hashing::hash(data+index+1, length);
}

StringIterator String::get_iterator() {
    return StringIterator(*this);
}

}
}