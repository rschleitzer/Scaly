#include "Scaly.h"
#include <sys/stat.h>
namespace scaly {

long DirectoryError::_getErrorCode() {
    return errorCode;
}

void* DirectoryError::_getErrorInfo() {
    return 0;
}

bool Directory::exists(string* path) {
    struct stat sb;

    if (stat(path->getNativeString(), &sb) == 0 && S_ISDIR(sb.st_mode))
        return true;
    else
        return false;
}

DirectoryError* Directory::create(_Page* _ep, string* path) {
    if (mkdir(path->getNativeString(), 0777) == -1) {
        _DirectoryErrorCode fileErrorCode = _DirectoryErrorCode_unknownError;
        switch (errno) {
            case ENOENT: fileErrorCode = _DirectoryErrorCode_noSuchDirectory; break;
        }
        return new(_ep) DirectoryError(fileErrorCode);
    }
        
    return 0;
}

}
