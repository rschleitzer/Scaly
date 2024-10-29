#include "../../scaly.h"
namespace scaly {
namespace io {

Result<String, FileError> File::read_to_string(Page* rp, Page* ep, String path) {
    auto r = Region();
    auto file = fopen(path.to_c_string(r.get_page()), "rb");
    if (file == nullptr) {
        switch (errno) 
        default: return Result<String, FileError>(FileError(UnknownFileError(String(ep, path))));
    };
    fseek(file, 0, SEEK_END);
    const auto size = ftell(file);
    rewind(file);
    auto ret = String(rp, (size_t)size);
    const auto buffer = ret.get_buffer();
    fread(buffer, 1, size, file);
    fclose(file);
    return Result<String, FileError>(ret);
}

Result<Void, FileError> File::write_from_string(Page* ep, String path, String contents) {
    auto r = Region();
    auto file = fopen(path.to_c_string(r.get_page()), "wb");
    if (file == nullptr) {
        switch (errno) 
        default: return Result<Void, FileError>(FileError(UnknownFileError(String(ep, path))));
    };
    fwrite(contents.to_c_string(r.get_page()), 1, contents.get_length(), file);
    fclose(file);
    return Result<Void, FileError>(Void());
}

}
}