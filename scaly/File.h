#ifndef __Scaly__File__
#define __Scaly__File__
namespace scaly {
    
enum _FileErrorCode {
    _FileError_unknownError = 1,
    _FileError_noSuchFileOrDirectory,
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
    static FileError* writeFromString(_Page *_ep, VarString& path, VarString& contents);
};

}

#endif // __Scaly__File__
