#include "scals.h"

size_t get_message_length(char** buf, char* end) {
    size_t ret = 0;
    while (*buf<end) {
        if (**buf>='0'&&**buf<='9') 
            break;
        *buf = *buf+1;
    };
    const auto last = *buf;
    while (*buf<end) {
        if (**buf<'0'||**buf>'9') 
            break;
        *buf = *buf+1;
    };
    auto current = *buf-1;
    auto multiplier = 1;
    while (current>=last) {
        ret = ret+(*current-'0')*multiplier;
        multiplier = multiplier*10;
        current = current-1;
    };
    return ret;
}

String* get_message(Page* rp, int fd) {
    auto r = Region();
    StringBuilder& builder = *new (alignof(StringBuilder), r.get_page()) StringBuilder();
    const size_t buffer_size = 0x800;
    char* buf = (char*)(*r.get_page()).allocate(buffer_size, 1);
    const auto bytes_read = read(fd, buf, buffer_size);
    const auto end = buf+bytes_read;
    write(2, buf, bytes_read);
    auto message_length = get_message_length(&buf, end);
    return new (alignof(String), rp) String();
}

void process_message(String message) {
}
