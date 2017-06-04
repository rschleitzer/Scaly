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

scaly_string* scaly_string_fromVarString(scaly_Page* _page, scaly_VarString* theString) {
    scaly_string* this = scaly_string_allocate(_page);
    *this = (scaly_string){ scaly_Page_allocateObject(_page, theString->length + 1), theString->length };

    scaly_string_copyNativeString(this, theString->buffer, theString->length);

    return this;
}

scaly_string* scaly_string_newWithLength(scaly_Page* _page, size_t theLength) {
    scaly_string* this = scaly_string_allocate(_page);
    *this = (scaly_string){ scaly_Page_allocateObject(_page, theLength + 1), theLength };

    this->buffer[0] = 0;
    
    return this;
}

char* scaly_string_getNativeString(scaly_string* this) {
    return this->buffer;
}

size_t scaly_string_getLength(scaly_string* this) {
    return this->length;
}

int scaly_string_equalsRawString(scaly_string* this, const char* theString) {
    return strcmp(scaly_string_getNativeString(this), theString) == 0;
}

int scaly_string_notEqualsRawString(scaly_string* this, const char* theString) {
    return strcmp(scaly_string_getNativeString(this), theString) != 0;
}

int scaly_string_equals(scaly_string* this, scaly_string* theString) {
    return strcmp(scaly_string_getNativeString(this), scaly_string_getNativeString(theString)) == 0;
}

int scaly_string_notEquals(scaly_string* this, scaly_string* theString) {
    return strcmp(scaly_string_getNativeString(this), scaly_string_getNativeString(theString)) != 0;
}

int scaly_string_equalsVarString(scaly_string* this, scaly_VarString* theString) {
    return strcmp(scaly_string_getNativeString(this), scaly_VarString_getNativeString(theString)) == 0;
}

int scaly_string_notEqualsVarString(scaly_string* this, scaly_VarString* theString) {
    return strcmp(scaly_string_getNativeString(this), scaly_VarString_getNativeString(theString)) != 0;
}

char scaly_string_charAt(scaly_string* this, size_t i) {
    if (i < this->length)
        return scaly_string_getNativeString(this)[i];

    return -1;
}

scaly_Array* scaly_string_split(scaly_Page* _rp, scaly_string* this, char c) {
    scaly_Array* ret = scaly_Array_new(_rp);
    scaly_VarString* part = 0;
    for (size_t _i = 0; _i < this->length; _i++) {
        char currentChar = scaly_string_charAt(this, _i);
        if (currentChar == c) {
            if (part) {
                scaly_Array_push(ret, scaly_string_fromVarString(_rp, part));
                part = 0;
            }
        }
        else {
            if (!part)
                part = scaly_VarString_new(_rp);
            scaly_VarString_appendChar(part, scaly_string_charAt(this, _i));
        }
    }

    if (part)
        scaly_Array_push(ret, scaly_string_fromVarString(_rp, part));

    return ret;
}

void scaly_string_copyNativeString(scaly_string* this, const char* theString, size_t length) {
    if (this->length == 0)
        this->buffer[0] = 0;
    else
        strcpy(this->buffer, theString);
}
