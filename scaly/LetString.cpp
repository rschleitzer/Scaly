#include "Scaly.h"
namespace scaly {

string::string()
: buffer(0), length(0) {
}

string::string(const char c) {
    length = 1;
    buffer = (char*)getPage()->allocateObject(length + 1);
    buffer[0] = c;
    buffer[1] = 0;
}

string::string(const char* theString) {
    length = strlen(theString);
    buffer = (char*)getPage()->allocateObject(length + 1);
    copyNativeString(theString, length);
}

string::string(string* theString) {
    length = theString->getLength();
    buffer = (char*)getPage()->allocateObject(length + 1);
    copyNativeString(theString->getNativeString(), length);
}

string::string(VarString* theString) {
    length = theString->getLength();
    buffer = (char*)getPage()->allocateObject(length + 1);
    copyNativeString(theString->getNativeString(), length);
}

string::string(size_t theLength) {
    length = theLength;
    buffer = (char*)getPage()->allocateObject(length + 1);
    buffer[length] = 0;
}

char* string::getNativeString() const {
    return buffer;
}

size_t string::getLength() {
    return length;
}

bool string::equals(const char* theString){
    return strcmp(getNativeString(), theString) == 0;
}

bool string::notEquals(const char* theString){
    return strcmp(getNativeString(), theString) != 0;
}

bool string::equals(string* theString){
    return strcmp(getNativeString(), theString->getNativeString()) == 0;
}

bool string::notEquals(string* theString){
    return strcmp(getNativeString(), theString->getNativeString()) != 0;
}

bool string::equals(VarString* theString){
    return strcmp(getNativeString(), theString->getNativeString()) == 0;
}

bool string::notEquals(VarString* theString){
    return strcmp(getNativeString(), theString->getNativeString()) != 0;
}

char string::charAt(size_t i) {
    if (i < length)
        return getNativeString()[i];

    return -1;
}

_Array<string>& string::Split(_Page* _rp, char c) {
    _Array<string>* ret = new(_rp) _Array<string>();
    VarString* part = 0;
    for (size_t _i = 0; _i < length; _i++) {
        char currentChar = this->charAt(_i);
        if (currentChar == c) {
            if (part) {
                ret->push(new(_rp) string(part));
                part = 0;
            }
        }
        else {
            if (!part)
                part = new(_rp) VarString();
            part->append(this->charAt(_i));
        }
    }

    if (part)
        ret->push(new(_rp) string(part));

    return *ret;
}

void string::copyNativeString(const char* theString, size_t length) {
    if (length == 0)
        buffer[0] = 0;
    else
        strcpy(buffer, theString);
}

}

