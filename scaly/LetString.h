#ifndef __Scaly__LetString__
#define __Scaly__LetString__
namespace scaly {

class VarString;

class string : public Object {
public:
    string();
    string(const char c);
    string(const char* theString);
    string(string* theString);
    string(VarString* theString);
    string(size_t theLength);
    char* getNativeString() const;
    size_t getLength();
    char charAt(size_t i);
    bool equals(const char* theString);
    bool notEquals(const char* theString);
    bool equals(string* theString);
    bool notEquals(string* theString);
    bool equals(VarString* theString);
    bool notEquals(VarString* theString);
    _Array<string>& Split(_Page* _rp, char c);

private:
    // Disable copy constuctor
    string(const string&);

    char* buffer;
    size_t length;
};

}
#endif
