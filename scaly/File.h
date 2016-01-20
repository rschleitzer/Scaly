#ifndef __Scaly__File__
#define __Scaly__File__
#include <cstdio>
namespace scaly {
    
enum _FileErrorCode {
    unknownError = 1,
    noSuchFileOrDirectory,
};

class FileError : public Object {
public:
    FileError(_FileErrorCode fileErrorCode)
    : errorCode(fileErrorCode) {}
    
    long getErrorCode();
    void* getErrorInfo();

private:
    _FileErrorCode errorCode;

};

class File {
public:
    static _Result<String, FileError> readToString(_Page* _rp, _Page *_ep, const String& path);
};

}

#endif // __Scaly__File__
