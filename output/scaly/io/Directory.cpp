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

Result<Void, FileError> Directory::create(Page* ep, String path) {
    auto r = Region();
    const auto err = mkdir(path.to_c_string(r.get_page()), 755);
    if (err == -1) {
        if (errno == ENOENT) {
            return Result<Void, FileError>(FileError(NoSuchFileOrDirectoryError(String(ep, path))));
        }
        else {
            return Result<Void, FileError>(FileError(UnknownFileError(String(ep, path))));
        };
    };
    return Result<Void, FileError>(Void());
}

Result<bool, FileError> Directory::exists(Page* ep, String path) {
    auto r = Region();
    struct_stat* s = (struct_stat*)(*r.get_page()).allocate_raw(alignof(struct_stat), sizeof(struct_stat));
    const auto err = stat(path.to_c_string(r.get_page()), s);
    if (err == -1) {
        if (errno == ENOENT) {
            return Result<bool, FileError>(false);
        }
        else {
            return Result<bool, FileError>(FileError(UnknownFileError(String(ep, path))));
        };
    }
    else {
        if (S_ISDIR((*s).st_mode)) {
            return Result<bool, FileError>(true);
        }
        else {
            return Result<bool, FileError>(false);
        };
    };
}

}
}