#include <cstdio>

namespace scaly {
namespace io {

FileError* print(Page* _ep, const char* message) {
    if (fputs(message, stdout) == EOF) {
        switch (errno) {
            case ENOENT: return new(alignof(FileError), _ep) FileError(NoSuchFileOrDirectoryError(String(_ep, "<stdout>")));
            default: return new(alignof(FileError), _ep) FileError(UnknownFileError(String(_ep, "<stdout>"))); 
        }
    }
    return nullptr;
}

FileError* print(Page* _ep, String message) {
    Region _r;
    return print(_ep, message.to_c_string(_r.page));
}

}
}