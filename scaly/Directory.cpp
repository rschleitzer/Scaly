#include "Scaly.h"
namespace scaly {

long DirectoryError::getErrorCode() {
    return errorCode;
}

void* DirectoryError::getErrorInfo() {
    return 0;
}

bool Directory::exists(const String& path) {
    return false;
}

DirectoryError* Directory::create(_Page* _ep, const String& path) {
    FILE* file = fopen(path.getNativeString(), "rb");
    if (!file) {
        _DirectoryErrorCode fileErrorCode = unknownError2;
        switch (errno) {
            case ENOENT: fileErrorCode = noSuchDirectoryOrDirectory; break;
        }
        return new(_ep) DirectoryError(fileErrorCode);
    }
        
    return 0;
}

}
