#include "Scaly.h"
#include <cstdio>
namespace scaly {

long FileError::getErrorCode() {
    return errorCode;
}

void* FileError::getErrorInfo() {
    return 0;
}

_Result<_LetString, FileError> File::readToString(_Page* _rp, _Page* _ep, const _LetString& path) {
    FILE* file = fopen(path.getNativeString(), "rb");
    if (!file) {
        _FileErrorCode fileErrorCode = _FileError_unknownError;
        switch (errno) {
            case ENOENT: fileErrorCode = _FileError_noSuchFileOrDirectory; break;
        }
        return _Result<_LetString, FileError>(new(_ep) FileError(fileErrorCode));
    }
    
    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    rewind(file);
    _LetString* ret = &_LetString::createUninitialized(_rp, (size_t)size);
    char* buffer = ret->getNativeString();
    fread (buffer, 1, size, file);
    fclose (file);
    
    return ret;
}

FileError* File::writeFromString(_Page *_ep, _VarString& path, _VarString& contents) {
    FILE* file = fopen(path.getNativeString(), "wb");
    if (!file) {
        _FileErrorCode fileErrorCode = _FileError_unknownError;
        switch (errno) {
            case ENOENT: fileErrorCode = _FileError_noSuchFileOrDirectory; break;
        }
        return new(_ep) FileError(fileErrorCode);
    }

    fwrite(contents.getNativeString(), 1, contents.getLength(), file);
    fclose (file);
    return 0;
}

}
