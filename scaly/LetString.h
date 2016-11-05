#ifndef __Scaly__LetString__
#define __Scaly__LetString__
namespace scaly {

class VarString;

class String : public Object {
public:
    String();
    String(const char c);
    String(const char* theString);
    String(String* theString);
    String(VarString* theString);
    String(size_t theLength);
    char* getNativeString() const;
    size_t getLength();
    char charAt(size_t i);
    bool equals(const char* theString);
    bool notEquals(const char* theString);
    bool equals(String* theString);
    bool notEquals(String* theString);
    bool equals(VarString* theString);
    bool notEquals(VarString* theString);
    _Array<String>& Split(_Page* _rp, char c);

private:
    // Disable copy constuctor
    String(const String&);

    char* string;
    size_t length;
};

}
#endif
