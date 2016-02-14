#ifndef __Scaly__LetString__
#define __Scaly__LetString__
namespace scaly {

class _VarString;

class _LetString : public Object {
public:
    _LetString();
    _LetString(const char* theString);
    _LetString(const _LetString& theString);
    _LetString(_VarString& theString);
    _LetString(size_t theLength);
    _LetString(size_t theLength, size_t theCapacity);
    _LetString(char c);
    char* getNativeString() const;
    size_t getLength();
    char operator [](size_t i);
    bool operator == (const char* theString);
    bool operator != (const char* theString);
    bool operator == (const _LetString& theString);
    bool operator != (const _LetString& theString);
    _Array<_LetString>& Split(_Page* _rp, char c);

protected:
    void allocate(size_t size);

    char* string;
    size_t length;
};

}
#endif
