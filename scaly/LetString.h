#ifndef __Scaly__LetString__
#define __Scaly__LetString__
namespace scaly {

class _VarString;

class _LetString : public Object {
public:
    static _LetString& create(_Page* page);
    static _LetString& create(_Page* page, const char* theString);
    static _LetString& create(_Page* page, const _LetString& theString);
    static _LetString& create(_Page* page, _VarString& theString);
    static _LetString& createFromChar(_Page* page, char c);
    static _LetString& createUninitialized(_Page* page, size_t length);
    char* getNativeString() const;
    size_t getLength();
    char operator [](size_t i);
    bool operator == (const char* theString);
    bool operator != (const char* theString);
    bool operator == (const _LetString& theString);
    bool operator != (const _LetString& theString);
    _Array<_LetString>& Split(_Page* _rp, char c);

private:
    // Disable default and copy constuctors
    _LetString();
    _LetString(const _LetString&);
    size_t length;
};

}
#endif
