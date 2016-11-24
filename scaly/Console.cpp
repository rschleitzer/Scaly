#include "Scaly.h"
#include <cstdio>
namespace scaly {

FileError* print(_Page* _ep, const char* message) {
    if (fputs(message, stdout) == EOF) {
        _FileErrorCode fileErrorCode = _FileErrorCode_unknownError;
        switch (errno) {
            case ENOENT: fileErrorCode = _FileErrorCode_noSuchFileOrDirectory; break;
        }
        return new(_ep) FileError(fileErrorCode);
    }
    return nullptr;
}

FileError* print(_Page* _ep, string* message) {
    return print(_ep, message->getNativeString());
}

}
