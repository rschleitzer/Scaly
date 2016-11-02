#include "Scaly.h"
namespace scaly {

VarString::VarString()
: string(0), length(0), capacity(0) {
}

VarString::VarString(const char* theString) {
    length = strlen(theString);
    capacity = length;
    string = (char*)getPage()->allocateObject(length + 1);
    strcpy(string, theString);
}

VarString::VarString(VarString* theString)
: length(theString->length), capacity(length) {
    string = (char*)getPage()->allocateObject(length + 1);
    strcpy(string, theString->string);
}

VarString::VarString(String* theString)
: length(theString->getLength()), capacity(length) {
    string = (char*)getPage()->allocateObject(length + 1);
    strcpy(string, theString->getNativeString());
}

VarString::VarString(size_t theLength)
: length(theLength), capacity(length) {
    string = (char*)getPage()->allocateObject(length + 1);
}

VarString::VarString(size_t theLength, size_t theCapacity)
: length(theLength), capacity(theCapacity) {
    string = (char*)getPage()->allocateObject(capacity + 1);
}

VarString::VarString(char c)
: length(1), capacity(length) {
    string = (char*)getPage()->allocateObject(length + 1);
    string[0] = c;
    string[1] = 0;
}

char* VarString::getNativeString() const {
    return string;
}

size_t VarString::getLength() {
    return length;
}

bool VarString::operator == (const char* theString){
    return equals(theString);
}

bool VarString::equals (const char* theString){
    return strcmp(string, theString) == 0;
}
bool VarString::operator != (const char* theString){
    return strcmp(string, theString) != 0;
}

bool VarString::operator == (const VarString& theString){
    return strcmp(string, theString.getNativeString()) == 0;
}

bool VarString::operator != (const VarString& theString){
    return strcmp(string, theString.getNativeString()) != 0;
}

char VarString::operator [](size_t i) {
    if (i < length)
        return string[i];

    return -1;
}

void VarString::append (char c) {
    if (!string) {
        // Allocate for the char itself and the trailing 0
        allocate(2);
        length = 1;
        capacity = 1;
    }
    else {
        if (!extend(1))
            reallocate(length + 1);
    }

    *(string + length - 1) = c;
    *(string + length) = 0;
}

void VarString::append(const char* theString) {
    size_t stringLength = strlen(theString);
    if (!string) {
        // Allocate for the char itself and the trailing 0
        allocate(stringLength + 1);
        length = stringLength;
        capacity = length;
    }
    else {
        if (!extend(stringLength))
            reallocate(length + stringLength);
    }

    strcpy(string + length - stringLength, theString);
    *(string + length) = 0;

}

void VarString::append(VarString* theString) {
    append(theString->string);
}

void VarString::append(String* theString) {
    append(theString->getNativeString());
}

bool VarString::extend(size_t size) {
    _Page& page = *_Page::getPage(string);
    if (length + size <= capacity) {
        length += size;
        return true;
    }
    if (page.extend(string + length + 1, size)) {
        length += size;
        capacity += size;
        return true;
    }
    else {
        return false;
    }
}

void VarString::reallocate(size_t newLength) {
    char* oldString = string;
    length = newLength;
    capacity = newLength * 2;
    allocate(capacity + 1);
    memcpy(string, oldString, length + 1);

    // Reclaim the page if it was oversized, i.e., exclusively allocated
    if (((Object*)oldString)->getPage() == ((_Page*)oldString))
        getPage()->reclaimArray(oldString);
}

void VarString::allocate(size_t size) {
    string = (char*) getPage()->allocateObject(size);
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

