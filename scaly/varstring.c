#include "scaly.h"

scaly_string* scaly_string_allocate(scaly_Page* _page) {
    return (scaly_string*) scaly_Page_allocateObject(_page, sizeof(scaly_string));
}

scaly_string* scaly_string_new(scaly_Page* _page) {
    scaly_string* this = scaly_string_allocate(_page);
    *this = (scaly_string){ 0, 0 };
    return this;
}

scaly_string* scaly_string_fromChar(scaly_Page* _page, const char c) {
    scaly_string* this = scaly_string_allocate(_page);
    *this = (scaly_string){ scaly_Page_allocateObject(_page, 2), 1 };

    this->buffer[0] = c;
    this->buffer[1] = 0;

    return this;
}

scaly_string* scaly_string_fromRawString(scaly_Page* _page, const char* theString) {
    size_t length = strlen(theString);

    scaly_string* this = scaly_string_allocate(_page);
    *this = (scaly_string){ scaly_Page_allocateObject(_page, length + 1), length };

    scaly_string_copyNativeString(this, theString, length);
    
    return this;
}

scaly_string* scaly_string_fromString(scaly_Page* _page, scaly_string* theString) {
    scaly_string* this = scaly_string_allocate(_page);
    *this = (scaly_string){ scaly_Page_allocateObject(_page, theString->length + 1), theString->length };

    scaly_string_copyNativeString(this, theString->buffer, theString->length);

    return this;
}
/*
scaly_string* scaly_string_fromVarString(scaly_Page* _page, scaly_VarString* theString) {
    scaly_string* this = scaly_string_allocate(_page);
    *this = (scaly_string){ scaly_Page_allocateObject(_page, theString->length + 1), theString->length };

    scaly_string_copyNativeString(this, theString->buffer, theString->length);

    return this;
}

string::string(size_t theLength) {
    length = theLength;
    buffer = (char*)_getPage()->allocateObject(length + 1);
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
*/

void scaly_string_copyNativeString(scaly_string* this, const char* theString, size_t length) {
    if (this->length == 0)
        this->buffer[0] = 0;
    else
        strcpy(this->buffer, theString);
}
