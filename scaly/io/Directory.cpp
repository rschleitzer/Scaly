#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

namespace scaly {
namespace io {

struct Directory {
    static Result<bool, FileError> exists(Page *_ep, const String& path) {
        Region _r;

        struct stat s;
        int err = stat(path.to_c_string(_r.get_page()), &s);
        if(err == -1) {
            if(errno == ENOENT)
                return Result<bool, FileError> { ._tag =  Result<bool, FileError>::Ok, ._Ok = false };
            else
                return Result<bool, FileError> { ._tag =  Result<bool, FileError>::Error, ._Error = FileError(UnknownFileError(String(_ep, path))) };
        } else {
            if(S_ISDIR(s.st_mode)) {
                return Result<bool, FileError> { ._tag =  Result<bool, FileError>::Ok, ._Ok = true };
            } else {
                return Result<bool, FileError> { ._tag =  Result<bool, FileError>::Ok, ._Ok = false };
            }
        }
    }
};

}
}