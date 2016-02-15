#include "Scaly.h"
namespace scaly {

_LetString::_LetString()
: string(0), length(0) {
}

_LetString::_LetString(const char* theString) {
    length = strlen(theString);
    string = (char*)getPage()->allocateObject(length + 1);
    strcpy(string, theString);
}

_LetString::_LetString(const _LetString& theString)
: length(theString.length) {
    string = (char*)getPage()->allocateObject(length + 1);
    strcpy(string, theString.string);
}

_LetString::_LetString(_VarString& theString)
: length(theString.getLength()) {
    string = (char*)getPage()->allocateObject(length + 1);
    strcpy(string, theString.getNativeString());
}

_LetString::_LetString(size_t theLength)
: length(theLength) {
    string = (char*)getPage()->allocateObject(length + 1);
}

_LetString::_LetString(size_t theLength, size_t theCapacity)
: length(theLength) {
    string = (char*)getPage()->allocateObject(length + 1);
}

_LetString::_LetString(char c)
: length(1) {
    string = (char*)getPage()->allocateObject(length + 1);
    string[0] = c;
    string[1] = 0;
}

char* _LetString::getNativeString() const {
    return string;
}

size_t _LetString::getLength() {
    return length;
}

bool _LetString::operator == (const char* theString){
    return strcmp(string, theString) == 0;
}

bool _LetString::operator != (const char* theString){
    return strcmp(string, theString) != 0;
}

bool _LetString::operator == (const _LetString& theString){
    return strcmp(string, theString.getNativeString()) == 0;
}

bool _LetString::operator != (const _LetString& theString){
    return strcmp(string, theString.getNativeString()) != 0;
}

char _LetString::operator [](size_t i) {
    if (i < length)
        return string[i];

    return -1;
}

void _LetString::allocate(size_t size) {
    string = (char*) getPage()->allocateObject(size);
}

_Array<_LetString>& _LetString::Split(_Page* _rp, char c) {
    _Array<_LetString>* ret = new(_rp) _Array<_LetString>();
    _VarString* part = 0;
    for (size_t _i = 0; _i < length; _i++) {
        char currentChar = (*this)[_i];
        if (currentChar == c) {
            if (part) {
                ret->push(new(_rp) _LetString(*part));
                part = 0;
            }
        }
        else {
            if (!part)
                part = new(_rp) _VarString();
            *part += (*this)[_i];
        }
    }

    if (part)
        ret->push(new(_rp) _LetString(*part));
    
    return *ret;
}

}

