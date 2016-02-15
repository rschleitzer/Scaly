#include "Scaly.h"
#include <sys/stat.h>
namespace scaly {

long DirectoryError::getErrorCode() {
    return errorCode;
}

void* DirectoryError::getErrorInfo() {
    return 0;
}

bool Directory::exists(const _LetString& path) {
    struct stat sb;

    if (stat(path.getNativeString(), &sb) == 0 && S_ISDIR(sb.st_mode))
        return true;
    else
        return false;
}

DirectoryError* Directory::create(_Page* _ep, const _LetString& path) {
    if (mkdir(path.getNativeString(), 0777) == -1) {
        _DirectoryErrorCode fileErrorCode = _DirectoryError_unknownError;
        switch (errno) {
            case ENOENT: fileErrorCode = _DirectoryError_noSuchDirectory; break;
        }
        return new(_ep) DirectoryError(fileErrorCode);
    }
        
    return 0;
}

}
