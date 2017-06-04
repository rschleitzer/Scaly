#include "scaly.h"

scaly_VarString* scaly_VarString_allocate(scaly_Page* _page) {
    return (scaly_VarString*) scaly_Page_allocateObject(_page, sizeof(scaly_VarString));
}

scaly_VarString* scaly_VarString_new(scaly_Page* _page) {
    scaly_VarString* this = scaly_VarString_allocate(_page);
    *this = (scaly_VarString){ 0, 0, 0 };
    return this;
}

scaly_VarString* scaly_VarString_fromChar(scaly_Page* _page, const char c) {
    scaly_VarString* this = scaly_VarString_allocate(_page);
    *this = (scaly_VarString){ scaly_Page_allocateObject(_page, 2), 1, 1 };

    this->buffer[0] = c;
    this->buffer[1] = 0;

    return this;
}

scaly_VarString* scaly_VarString_fromRawString(scaly_Page* _page, const char* theString) {
    size_t length = strlen(theString);

    scaly_VarString* this = scaly_VarString_allocate(_page);
    *this = (scaly_VarString){ scaly_Page_allocateObject(_page, length + 1), length, length };

    scaly_VarString_copyNativeString(this, theString, length);
    
    return this;
}

scaly_VarString* scaly_VarString_fromString(scaly_Page* _page, scaly_string* theString) {
    scaly_VarString* this = scaly_VarString_allocate(_page);
    *this = (scaly_VarString){ scaly_Page_allocateObject(_page, theString->length + 1), theString->length, theString->length };

    scaly_VarString_copyNativeString(this, theString->buffer, theString->length);

    return this;
}

scaly_VarString* scaly_VarString_newWithLength(scaly_Page* _page, size_t theLength) {
    scaly_VarString* this = scaly_VarString_allocate(_page);
    *this = (scaly_VarString){ scaly_Page_allocateObject(_page, theLength + 1), theLength, theLength };

    this->buffer[0] = 0;
    
    return this;
}

char* scaly_VarString_getNativeString(scaly_VarString* this) {
    return this->buffer;
}

/*
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

void scaly_VarString_copyNativeString(scaly_VarString* this, const char* theString, size_t length) {
    if (this->length == 0)
        this->buffer[0] = 0;
    else
        strcpy(this->buffer, theString);
}

int scaly_VarString_extend(scaly_VarString* this, size_t size) {
    scaly_Page* page = scaly_Page_getPage(this->buffer);
    if (this->length + size < this->capacity) {
        this->length += size;
        return 1;
    }
    if (scaly_Page_extend(page, this->buffer + this->length + 1, size + 1)) {
        this->length += size;
        this->capacity += size;
        return 1;
    }
    else {
        return 0;
    }
}

void scaly_VarString_reallocate(scaly_VarString* this, size_t newLength) {
    char* oldString = this->buffer;
    size_t oldLength = this->length;
    this->length = newLength;
    this->capacity = newLength * 2;
    scaly_VarString_allocateBuffer(this, this->capacity + 1);
    memcpy(this->buffer, oldString, oldLength + 1);

    // Reclaim the page if it was oversized, i.e., exclusively allocated
    if (scaly_Page_isOversized(scaly_Page_getPage(oldString)))
        scaly_Page_reclaimArray(scaly_Page_getPage(this), oldString);
}

void scaly_VarString_appendChar(scaly_VarString* this, char c) {
    if (!this->buffer) {
        // Allocate for the char itself and the trailing 0
        scaly_VarString_allocateBuffer(this, 2);
        this->length = 1;
        this->capacity = 1;
    }
    else {
        if (!scaly_VarString_extend(this, 1))
            scaly_VarString_reallocate(this, this->length + 1);
    }

    *(this->buffer + this->length - 1) = c;
    *(this->buffer + this->length) = 0;
}

void scaly_VarString_appendRawString(scaly_VarString* this, const char* theString) {
    size_t stringLength = strlen(theString);
    if (!this->buffer) {
        // Allocate for the char itself and the trailing 0
        scaly_VarString_allocateBuffer(this, stringLength + 1);
        this->length = stringLength;
        this->capacity = this->length;
    }
    else {
        if (!scaly_VarString_extend(this, stringLength))
            scaly_VarString_reallocate(this, this->length + stringLength);
    }

    strcpy(this->buffer + this->length - stringLength, theString);
    *(this->buffer + this->length) = 0;

}

void scaly_VarString_appendVarString(scaly_VarString* this, scaly_VarString* theString) {
    scaly_VarString_appendRawString(this, theString->buffer);
}

void scaly_VarString_appendString(scaly_VarString* this, scaly_string* theString) {
    scaly_VarString_appendRawString(this, scaly_string_getNativeString(theString));
}

void scaly_VarString_allocateBuffer(scaly_VarString* this, size_t size) {
    this->buffer = (char*) scaly_Page_allocateObject(scaly_Page_getPage(this), size);
}

