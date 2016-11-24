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
    
    long _getErrorCode();
    void* _getErrorInfo();

private:
    _DirectoryErrorCode errorCode;

};

class Directory {
public:
    static bool exists(string* path);
    static DirectoryError* create(_Page *_ep, string* path);
};

}

#endif // __Scaly__Directory__
