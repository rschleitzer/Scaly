#include "scals.h"
using namespace scaly::json;


void process_message(String message) {
    auto r = Region();
    const auto requestMessage = scals::de_serialize_requestMessage(r.get_page(), message);
}

String* get_message(Page* rp, int fd) {
    auto r = Region();
    const size_t buffer_size = 0x800;
    char* buffer = (char*)(*r.get_page()).allocate(buffer_size, 1);
    auto bytes_read = read(fd, buffer, buffer_size);
    if (bytes_read == -1) 
        exit(-1);
    if (bytes_read == 0) 
        return nullptr;
    auto end = buffer+bytes_read;
    write(2, buffer, bytes_read);
    auto index = buffer;
    size_t length = 0;
    while (index<end) {
        if (*index>='0'&&*index<='9') 
            break;
        index = index+1;
    };
    const auto last = index;
    while (index<end) {
        if (*index<'0'||*index>'9') 
            break;
        index = index+1;
    };
    auto current = index-1;
    auto multiplier = 1;
    while (current>=last) {
        length = length+(*current-'0')*multiplier;
        multiplier = multiplier*10;
        current = current-1;
    };
    while (index<end) {
        if (*index != '\r'&&*index>'\n') 
            break;
        index = index+1;
    };
    if (length == 0) 
        return nullptr;
    StringBuilder& builder = *new (alignof(StringBuilder), r.get_page()) StringBuilder(length);
    while (length>0) {
        const auto remainder = end-index;
        builder.append(index, remainder);
        length = length-remainder;
        if (length<0) 
            exit(-1);
        if (length == 0) 
            break;
        auto bytes_to_read = length;
        if (bytes_to_read>buffer_size) 
            bytes_to_read = buffer_size;
        bytes_read = read(fd, buffer, bytes_to_read);
        if (bytes_read == -1) 
            exit(-2);
        if (bytes_read == 0) 
            return nullptr;
        index = buffer;
        end = buffer+bytes_read;
        write(2, index, bytes_read);
    };
    return new (alignof(String), rp) String(rp, builder.to_string(r.get_page()));
}
