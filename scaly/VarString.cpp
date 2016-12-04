#include "Scaly.h"
namespace scaly {

VarString::VarString()
: buffer(0), length(0), capacity(0) {
}

VarString::VarString(const char* theString) {
    length = strlen(theString);
    capacity = length;
    buffer = (char*)_getPage()->allocateObject(length + 1);
    strcpy(buffer, theString);
}

VarString::VarString(VarString* theString)
: length(theString->length), capacity(length) {
    buffer = (char*)_getPage()->allocateObject(length + 1);
    strcpy(buffer, theString->buffer);
}

VarString::VarString(string* theString)
: length(theString->getLength()), capacity(length) {
    buffer = (char*)_getPage()->allocateObject(length + 1);
    strcpy(buffer, theString->getNativeString());
}

VarString::VarString(size_t theLength)
: length(theLength), capacity(length) {
    buffer = (char*)_getPage()->allocateObject(length + 1);
}

VarString::VarString(size_t theLength, size_t theCapacity)
: length(theLength), capacity(theCapacity) {
    buffer = (char*)_getPage()->allocateObject(capacity + 1);
}

VarString::VarString(char c)
: length(1), capacity(length) {
    buffer = (char*)_getPage()->allocateObject(length + 1);
    buffer[0] = c;
    buffer[1] = 0;
}

char* VarString::getNativeString() const {
    return buffer;
}

size_t VarString::getLength() {
    return length;
}

bool VarString::operator == (const char* theString){
    return equals(theString);
}

bool VarString::equals (const char* theString){
    return strcmp(buffer, theString) == 0;
}

bool VarString::equals (string* theString){
    return strcmp(buffer, theString->getNativeString()) == 0;
}

bool VarString::operator != (const char* theString){
    return strcmp(buffer, theString) != 0;
}

bool VarString::operator == (const VarString& theString){
    return strcmp(buffer, theString.getNativeString()) == 0;
}

bool VarString::operator != (const VarString& theString){
    return strcmp(buffer, theString.getNativeString()) != 0;
}

char VarString::operator [](size_t i) {
    if (i < length)
        return buffer[i];

    return -1;
}

void VarString::append (char c) {
    if (!buffer) {
        // Allocate for the char itself and the trailing 0
        allocate(2);
        length = 1;
        capacity = 1;
    }
    else {
        if (!extend(1))
            reallocate(length + 1);
    }

    *(buffer + length - 1) = c;
    *(buffer + length) = 0;
}

void VarString::append(const char* theString) {
    size_t stringLength = strlen(theString);
    if (!buffer) {
        // Allocate for the char itself and the trailing 0
        allocate(stringLength + 1);
        length = stringLength;
        capacity = length;
    }
    else {
        if (!extend(stringLength))
            reallocate(length + stringLength);
    }

    strcpy(buffer + length - stringLength, theString);
    *(buffer + length) = 0;

}

void VarString::append(VarString* theString) {
    append(theString->buffer);
}

void VarString::append(string* theString) {
    append(theString->getNativeString());
}

bool VarString::extend(size_t size) {
    _Page& page = *_Page::getPage(buffer);
    if (length + size < capacity) {
        length += size;
        return true;
    }
    if (page.extend(buffer + length + 1, size + 1)) {
        length += size;
        capacity += size;
        return true;
    }
    else {
        return false;
    }
}

void VarString::reallocate(size_t newLength) {
    char* oldString = buffer;
    size_t oldLength = length;
    length = newLength;
    capacity = newLength * 2;
    allocate(capacity + 1);
    memcpy(buffer, oldString, oldLength + 1);

    // Reclaim the page if it was oversized, i.e., exclusively allocated
    if (((Object*)oldString)->_getPage() == ((_Page*)oldString))
        _getPage()->reclaimArray(oldString);
}

void VarString::allocate(size_t size) {
    buffer = (char*) _getPage()->allocateObject(size);
}

_Array<VarString>& VarString::Split(_Page* _rp, char c) {
    _Array<VarString>* ret = new(_rp) _Array<VarString>();
    VarString* part = 0;
    for (size_t _i = 0; _i < length; _i++) {
        char currentChar = (*this)[_i];
        if (currentChar == c) {
            if (part) {
                ret->push(part);
                part = 0;
            }
        }
        else {
            if (!part)
                part = new(_rp) VarString();
            part->append((*this)[_i]);
        }
    }

    if (part)
        ret->push(part);

    return *ret;
}

}

