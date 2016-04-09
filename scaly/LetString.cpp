#include "Scaly.h"
namespace scaly {

String& String::create(_Page* page) {
    return createUninitialized(page, 0);
}

String& String::create(_Page* page, char c) {
    String& string = createUninitialized(page, 1);
    char* pString = string.getNativeString();
    pString[0] = c;
    pString[1] = 0;
    return string;
}

String& String::create(_Page* page, const char* theString) {
    size_t length = strlen(theString);
    String& string = createUninitialized(page, length);
    char* pString = string.getNativeString();
    strcpy(pString, theString);
    return string;
}

String& String::create(_Page* page, String* theString) {
    String& string = createUninitialized(page, theString->length);
    char* pString = string.getNativeString();
    strcpy(pString, theString->getNativeString());
    return string;
}

String& String::create(_Page* page, VarString* theString) {
    size_t length = theString->getLength();
    String& string = createUninitialized(page, length);
    char* pString = string.getNativeString();
    strcpy(pString, theString->getNativeString());
    return string;
}

String& String::createUninitialized(_Page* page, size_t length)
{
    String& string = *(String*)page->allocateObject(sizeof(String) + length + 1);
    string.length = length;
    return string;
}

String& String::createFromChar(_Page* page, char c) {
    String& string = createUninitialized(page, 1);
    char* pString = string.getNativeString();
    *pString = c;
    pString++;
    *pString = 0;
    return string;
}

char* String::getNativeString() const {
    const String* ret = this;
    ret++;
    return (char*)ret;
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

char String::operator [](size_t i) {
    if (i < length)
        return getNativeString()[i];

    return -1;
}

_Array<String>& String::Split(_Page* _rp, char c) {
    _Array<String>* ret = new(_rp) _Array<String>();
    VarString* part = 0;
    for (size_t _i = 0; _i < length; _i++) {
        char currentChar = (*this)[_i];
        if (currentChar == c) {
            if (part) {
                ret->push(&create(_rp, part));
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
        ret->push(&create(_rp, part));

    return *ret;
}

}

