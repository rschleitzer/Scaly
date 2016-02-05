#ifndef __Scaly__SwipeString__
#define __Scaly__SwipeString__
namespace scaly {

class String : public Object {
public:
    String(const char* theString);
    String(const String& theString);
    String(size_t theLength);
    String(char c);
    char* getNativeString() const;
    size_t getLength();
    char operator [](size_t i);
    String& operator += (char c);
    String& operator += (const char* theString);
    String& operator += (const String& theString);
    bool operator == (const char* theString);

private:
    void reallocate(size_t newLength);
    bool extend(size_t size);
    void allocate(size_t size);

    char* string;
    size_t length;
};

}
#endif
