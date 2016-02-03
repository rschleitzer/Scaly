#ifndef __Scaly__Directory__
#define __Scaly__Directory__
namespace scaly {
    
enum _DirectoryErrorCode {
    _DirectoryError_unknownError = 1,
    _DirectoryError_noSuchDirectory,
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
    static bool exists(const String& path);
    static DirectoryError* create(_Page *_ep, const String& path);
};

}

#endif // __Scaly__Directory__
