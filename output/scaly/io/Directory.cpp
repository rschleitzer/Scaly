#include "../../scaly.h"
namespace scaly {
namespace io {

Result<Void, FileError> Directory::remove(Page* ep, String path) {
    auto r = Region();
    const auto err = rmdir(path.to_c_string(r.get_page()));
    if (err == -1)
        return Result<Void, FileError>(FileError(UnknownFileError(String(ep, path))));
    return Result<Void, FileError>(Void());
}

}
}