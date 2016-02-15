#include "Scaly.h"
namespace scaly {

_LetString& _LetString::create(_Page* page) {
    return createUninitialized(page, 0);
}

_LetString& _LetString::create(_Page* page, const char* theString) {
    size_t length = strlen(theString);
    _LetString& string = createUninitialized(page, length);
    char* pString = string.getNativeString();
    strcpy(pString, theString);
    return string;
}

_LetString& _LetString::create(_Page* page, const _LetString& theString) {
    _LetString& string = createUninitialized(page, theString.length);
    char* pString = string.getNativeString();
    strcpy(pString, theString.getNativeString());
    return string;
}

_LetString& _LetString::create(_Page* page, _VarString& theString) {
    size_t length = theString.getLength();
    _LetString& string = createUninitialized(page, length);
    char* pString = string.getNativeString();
    strcpy(pString, theString.getNativeString());
    return string;
}

_LetString& _LetString::createUninitialized(_Page* page, size_t length)
{
    _LetString& string = *(_LetString*)page->allocateObject(sizeof(_LetString) + length + 1);
    string.length = length;
    return string;
}

_LetString& _LetString::createFromChar(_Page* page, char c) {
    _LetString& string = createUninitialized(page, 1);
    char* pString = string.getNativeString();
    *pString = c;
    pString++;
    *pString = 0;
    return string;
}

char* _LetString::getNativeString() const {
    const _LetString* ret = this;
    ret++;
    return (char*)ret;
}

size_t _LetString::getLength() {
    return length;
}

bool _LetString::operator == (const char* theString){
    return strcmp(getNativeString(), theString) == 0;
}

bool _LetString::operator != (const char* theString){
    return strcmp(getNativeString(), theString) != 0;
}

bool _LetString::operator == (const _LetString& theString){
    return strcmp(getNativeString(), theString.getNativeString()) == 0;
}

bool _LetString::operator != (const _LetString& theString){
    return strcmp(getNativeString(), theString.getNativeString()) != 0;
}

char _LetString::operator [](size_t i) {
    if (i < length)
        return getNativeString()[i];

    return -1;
}

_Array<_LetString>& _LetString::Split(_Page* _rp, char c) {
    _Array<_LetString>* ret = new(_rp) _Array<_LetString>();
    _VarString* part = 0;
    for (size_t _i = 0; _i < length; _i++) {
        char currentChar = (*this)[_i];
        if (currentChar == c) {
            if (part) {
                ret->push(&create(_rp, *part));
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
        ret->push(&create(_rp, *part));
    
    return *ret;
}

}

