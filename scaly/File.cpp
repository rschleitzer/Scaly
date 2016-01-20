#include "Scaly.h"
namespace scaly {

long FileError::getErrorCode() {
    return errorCode;
}

void* FileError::getErrorInfo() {
    return 0;
}

_Result<String, FileError> File::readToString(_Page* _rp, _Page* _ep, const String& path) {
    FILE* file = fopen(path.getNativeString(), "rb");
    if (!file) {
        _FileErrorCode fileErrorCode = unknownError;
        switch (errno) {
            case ENOENT: fileErrorCode = noSuchFileOrDirectory; break;
        }
        return _Result<String, FileError>(new(_ep) FileError(fileErrorCode));
    }
    
    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    rewind(file);
    String* ret = (String*) new(_rp) String((size_t)size);
    char* buffer = ret->getNativeString();
    fread (buffer, 1, size, file);
    fclose (file);
    
    return ret;
}

}
