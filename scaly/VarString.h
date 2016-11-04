#ifndef __Scaly__VarString__
#define __Scaly__VarString__
namespace scaly {

class VarString : public Object {
public:
    VarString();
    VarString(const char* theString);
    VarString(VarString* theString);
    VarString(String* theString);
    VarString(size_t theLength);
    VarString(size_t theLength, size_t theCapacity);
    VarString(char c);
    char* getNativeString() const;
    size_t getLength();
    char operator [](size_t i);
    void append(char c);
    void append(const char* theString);
    void append(VarString* theString);
    void append(String* theString);
    bool operator == (const char* theString);
    bool equals(const char* theString);
    bool equals(String* theString);
    bool operator != (const char* theString);
    bool operator == (const VarString& theString);
    bool operator != (const VarString& theString);
    _Array<VarString>& Split(_Page* _rp, char c);

private:
    void reallocate(size_t newLength);
    bool extend(size_t size);
    void allocate(size_t size);

    char* string;
    size_t length;
    size_t capacity;
};

}
#endif
