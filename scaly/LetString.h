#ifndef __Scaly__LetString__
#define __Scaly__LetString__
namespace scaly {

class VarString;

class String : public Object {
public:
    static String& create(_Page* page);
    static String& create(_Page* page, char c);
    static String& create(_Page* page, const char* theString);
    static String& create(_Page* page, const String& theString);
    static String& create(_Page* page, VarString& theString);
    static String& createFromChar(_Page* page, char c);
    static String& createUninitialized(_Page* page, size_t length);
    char* getNativeString() const;
    size_t getLength();
    char operator [](size_t i);
    bool operator == (const char* theString);
    bool operator != (const char* theString);
    bool operator == (const String& theString);
    bool operator != (const String& theString);
    bool operator == (const VarString& theString);
    bool operator != (const VarString& theString);
    _Array<String>& Split(_Page* _rp, char c);

private:
    // Disable default and copy constuctors
    String();
    String(const String&);
    size_t length;
};

}
#endif
