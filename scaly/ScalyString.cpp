#include "Scaly.h"
namespace scaly {

String::String(const char* theString) {
    length = strlen(theString);
    string = (char*)getPage()->allocateObject(length + 1);
    strcpy(string, theString);
}

String::String(const String& theString)
: length(theString.length) {
    string = (char*)getPage()->allocateObject(length + 1);
    strcpy(string, theString.string);
}

String::String(size_t theLength) {
    length = theLength;
    string = (char*)getPage()->allocateObject(length + 1);
}

String::String(char c) {
    length = 1;
    string = (char*)getPage()->allocateObject(length + 1);
    string[0] = c;
    string[1] = 0;
}

char* String::getNativeString() const {
    return string;
}

size_t String::getLength() {
    return length;
}

bool String::operator == (const char* theString){
    return strcmp(string, theString) == 0;
}

char String::operator [](size_t i) {
    if (i < length)
        return string[i];

    return -1;
}

String& String::operator += (char c) {
    if (!string) {
        // Allocate for the char itself and the trailing 0
        allocate(2);
    }
    else {
        if (!extend(1))
            reallocate(length + 1);
    }

    *(string + length - 1) = c;
    *(string + length) = 0;

    return *this;
}

String& String::operator += (const char* theString) {
    size_t stringLength = strlen(theString);
    if (!string) {
        // Allocate for the char itself and the trailing 0
        allocate(stringLength + 1);
    }
    else {
        if (!extend(stringLength))
            reallocate(length + stringLength);
    }

    //*(string + length - 1) = c;
    strcpy(string + length - stringLength, theString);
    *(string + length) = 0;

    return *this;
}

String& String::operator += (const String& theString) {
    (*this) += theString.getNativeString();
    
    return *this;
}

bool String::extend(size_t size) {
    _Page& page = *_Page::getPage(string);
    if (page.extend(string + length + 1, size)) {
        length += size;
        return true;
    }
    else {
        return false;
    }
}

void String::reallocate(size_t newLength) {
    char* oldString = string;
    size_t oldLength = length;
    length = newLength;
    allocate(length + 1);
    memcpy(string, oldString, length + 1);
    
    // Reclaim the page if it was oversized, i.e., exclusively allocated
    if (oldLength > _Page::pageSize)
        _Page::reclaimArray(oldString);
}

void String::allocate(size_t size) {
    string = (char*) getPage()->allocateObject(size);
}




/*
String::String(Array<unsigned char>& utf8String)
{
    size_t len = utf8String.Count();
    Page* page = GetPage();
    _String = new (page) Array<char32_t>(len);

    // Surely the following hack has to be replaced by the appropriate standard library function.
    size_t stringPos = 0;
    for (size_t i = 0; i < len; i++)
    {
        unsigned char c = *utf8String[i];
        if (c < 128)
        {
            *(*_String)[stringPos++] = (char32_t)c;
            continue;
        }

        if ((c & 0b11100000) == 0b11000000)
        {
            char32_t uc = (c & 0b11111) << 6;
            i++; if ((i == len) || (*utf8String[i] & 0b11000000) != 0b10000000) break; uc += (*utf8String[i] & 0b111111);
            *(*_String)[stringPos++] = uc;
            continue;
        }

        if ((c & 0b11110000) == 0b11100000)
        {
            char32_t uc = (c & 0b1111) << 12;
            i++; if ((i == len) || (*utf8String[i] & 0b11000000) != 0b10000000) break; uc += (*utf8String[i] & 0b111111) << 6;
            i++; if ((i == len) || (*utf8String[i] & 0b11000000) != 0b10000000) break; uc += (*utf8String[i] & 0b111111);
            *(*_String)[stringPos++] = uc;
            continue;
        }

        if ((c & 0b11111000) == 0b11110000)
        {
            char32_t uc = (c & 0b111) << 18;
            i++; if ((i == len) || (*utf8String[i] & 0b11000000) != 0b10000000) break; uc += (*utf8String[i] & 0b111111) << 12;
            i++; if ((i == len) || (*utf8String[i] & 0b11000000) != 0b10000000) break; uc += (*utf8String[i] & 0b111111) << 6;
            i++; if ((i == len) || (*utf8String[i] & 0b11000000) != 0b10000000) break; uc += (*utf8String[i] & 0b111111);
            *(*_String)[stringPos++] = uc;
            continue;
        }

        if ((c & 0b11111100) == 0b11111000)
        {
            char32_t uc = (c & 0b11) << 24;
            i++; if ((i == len) || (*utf8String[i] & 0b11000000) != 0b10000000) break; uc += (*utf8String[i] & 0b111111) << 18;
            i++; if ((i == len) || (*utf8String[i] & 0b11000000) != 0b10000000) break; uc += (*utf8String[i] & 0b111111) << 12;
            i++; if ((i == len) || (*utf8String[i] & 0b11000000) != 0b10000000) break; uc += (*utf8String[i] & 0b111111) << 6;
            i++; if ((i == len) || (*utf8String[i] & 0b11000000) != 0b10000000) break; uc += (*utf8String[i] & 0b111111);
            *(*_String)[stringPos++] = uc;
            continue;
        }

        if ((c & 0b11111110) == 0b11111100)
        {
            char32_t uc = (c & 0b1) << 30;
            i++; if ((i == len) || (*utf8String[i] & 0b11000000) != 0b10000000) break; uc += (*utf8String[i] & 0b111111) << 24;
            i++; if ((i == len) || (*utf8String[i] & 0b11000000) != 0b10000000) break; uc += (*utf8String[i] & 0b111111) << 18;
            i++; if ((i == len) || (*utf8String[i] & 0b11000000) != 0b10000000) break; uc += (*utf8String[i] & 0b111111) << 12;
            i++; if ((i == len) || (*utf8String[i] & 0b11000000) != 0b10000000) break; uc += (*utf8String[i] & 0b111111) << 6;
            i++; if ((i == len) || (*utf8String[i] & 0b11000000) != 0b10000000) break; uc += (*utf8String[i] & 0b111111);
            *(*_String)[stringPos++] = uc;
            continue;
        }

        *(*_String)[stringPos++] = (char32_t) c;
    }
}
*/

}

