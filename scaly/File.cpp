#include "Scaly.h"
#include <cstdio>
namespace scaly {

long FileError::_getErrorCode() {
    return errorCode;
}

void* FileError::_getErrorInfo() {
    return 0;
}

_Result<string, FileError> File::readToString(_Page* _rp, _Page* _ep, string* path) {
    FILE* file = fopen(path->getNativeString(), "rb");
    if (!file) {
        _FileErrorCode fileErrorCode = _FileErrorCode_unknownError;
        switch (errno) {
            case ENOENT: fileErrorCode = _FileErrorCode_noSuchFileOrDirectory; break;
        }
        return _Result<string, FileError>(new(_ep) FileError(fileErrorCode));
    }
    
    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    rewind(file);
    string* ret = new(_rp) string((size_t)size);
    char* buffer = ret->getNativeString();
    fread (buffer, 1, size, file);
    fclose (file);
    
    return ret;
}

FileError* File::writeFromString(_Page *_ep, VarString* path, VarString* contents) {
    FILE* file = fopen(path->getNativeString(), "wb");
    if (!file) {
        _FileErrorCode fileErrorCode = _FileErrorCode_unknownError;
        switch (errno) {
            case ENOENT: fileErrorCode = _FileErrorCode_noSuchFileOrDirectory; break;
        }
        return new(_ep) FileError(fileErrorCode);
    }

    fwrite(contents->getNativeString(), 1, contents->getLength(), file);
    fclose (file);
    return 0;
}

}
