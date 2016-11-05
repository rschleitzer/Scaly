#include "Scaly.h"
namespace scaly {

String::String()
: string(0), length(0) {
}

String::String(const char c) {
    length = 1;
    string = (char*)getPage()->allocateObject(length + 1);
    string[0] = c;
    string[1] = 0;
}

String::String(const char* theString) {
    length = strlen(theString);
    string = (char*)getPage()->allocateObject(length + 1);
    strcpy(string, theString);
}

String::String(String* theString) {
    length = theString->getLength();
    string = (char*)getPage()->allocateObject(length + 1);
    strcpy(string, theString->getNativeString());
}

String::String(VarString* theString) {
    length = theString->getLength();
    string = (char*)getPage()->allocateObject(length + 1);
    strcpy(string, theString->getNativeString());
}

String::String(size_t theLength) {
    length = theLength;
    string = (char*)getPage()->allocateObject(length + 1);
}

char* String::getNativeString() const {
    return string;
}

size_t String::getLength() {
    return length;
}

bool String::equals(const char* theString){
    return strcmp(getNativeString(), theString) == 0;
}

bool String::notEquals(const char* theString){
    return strcmp(getNativeString(), theString) != 0;
}

bool String::equals(String* theString){
    return strcmp(getNativeString(), theString->getNativeString()) == 0;
}

bool String::notEquals(String* theString){
    return strcmp(getNativeString(), theString->getNativeString()) != 0;
}

bool String::equals(VarString* theString){
    return strcmp(getNativeString(), theString->getNativeString()) == 0;
}

bool String::notEquals(VarString* theString){
    return strcmp(getNativeString(), theString->getNativeString()) != 0;
}

char String::charAt(size_t i) {
    if (i < length)
        return getNativeString()[i];

    return -1;
}

_Array<String>& String::Split(_Page* _rp, char c) {
    _Array<String>* ret = new(_rp) _Array<String>();
    VarString* part = 0;
    for (size_t _i = 0; _i < length; _i++) {
        char currentChar = this->charAt(_i);
        if (currentChar == c) {
            if (part) {
                ret->push(new(_rp) String(part));
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
        ret->push(new(_rp) String(part));

    return *ret;
}

}

