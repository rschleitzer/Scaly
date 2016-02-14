#ifndef __Scaly__VarString__
#define __Scaly__VarString__
namespace scaly {

class _VarString : public _LetString {
public:
    _VarString();
    _VarString(const char* theString);
    _VarString(const _VarString& theString);
    _VarString(size_t theLength);
    _VarString(size_t theLength, size_t theCapacity);
    _VarString(char c);
    _VarString& operator += (char c);
    _VarString& operator += (const char* theString);
    _VarString& operator + (const char* theString);
    _VarString& operator += (const _VarString& theString);
    _VarString& operator + (const _VarString& thestring);
    _Array<_VarString>& Split(_Page* _rp, char c);

private:
    void reallocate(size_t newLength);
    bool extend(size_t size);

    size_t capacity;
};

}
#endif
