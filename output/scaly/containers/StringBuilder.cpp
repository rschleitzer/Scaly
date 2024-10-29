#include "../../scaly.h"
namespace scaly {
namespace containers {

StringBuilder::StringBuilder(Array<char> buffer) : buffer(buffer) {}

StringBuilder::StringBuilder() : StringBuilder(Array<char>()){
}

StringBuilder::StringBuilder(char character) : StringBuilder(){
    append(character);
}

StringBuilder::StringBuilder(String string) : StringBuilder(){
    append(string);
}

StringBuilder::StringBuilder(const_char* c_string) : StringBuilder(){
    append(c_string);
}

void StringBuilder::append(char character) {
    buffer.add(character);
}

void StringBuilder::append(String string) {
    buffer.add(Vector<char>(string.get_buffer(), string.get_length()));
}

void StringBuilder::append(const_char* c_string) {
    const auto length = strlen(c_string);
    if (length == 0) 
        return;
    buffer.add(Vector<char>((char*)c_string, length));
}

size_t StringBuilder::get_length() {
    return buffer.get_length();
}

String StringBuilder::to_string(Page* rp) {
    return String(rp, buffer.get_buffer(), buffer.get_length());
}

}
}