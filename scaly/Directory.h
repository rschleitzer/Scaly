#ifndef __Scaly__Directory__
#define __Scaly__Directory__
namespace scaly {
    
enum _DirectoryErrorCode {
    _DirectoryErrorCode_unknownError = 1,
    _DirectoryErrorCode_noSuchDirectory,
};

class DirectoryError : public Object {
public:
    DirectoryError(_DirectoryErrorCode fileErrorCode)
    : errorCode(fileErrorCode) {}
    
    long getErrorCode();
    void* getErrorInfo();

private:
    _DirectoryErrorCode errorCode;

};

class Directory {
public:
    static bool exists(String* path);
    static DirectoryError* create(_Page *_ep, String* path);
};

}

#endif // __Scaly__Directory__
