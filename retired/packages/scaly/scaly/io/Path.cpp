#include "../../scaly.h"
namespace scaly {
namespace io {

String Path::get_directory_name(Page* rp, String path) {
    auto r = Region();
    const auto path_as_c_string = path.to_c_string(r.get_page());
    const auto directory_name_as_c_string = dirname((char*)path_as_c_string);
    if (strcmp(directory_name_as_c_string, ".") == 0) 
        return String(rp, "");
    const auto ret = String(rp, directory_name_as_c_string);
    return ret;
}

String Path::get_file_name(Page* rp, String path) {
    auto r = Region();
    const auto path_as_c_string = path.to_c_string(r.get_page());
    const auto file_name_as_c_string = basename((char*)path_as_c_string);
    const auto ret = String(rp, file_name_as_c_string);
    return ret;
}

String Path::join(Page* rp, String path1, String path2) {
    auto r = Region();
    if (path1.get_length() == 0) 
        return String(rp, path2);
    auto path = new (alignof(StringBuilder), r.get_page()) StringBuilder();
    (*path).append(path1);
    if (path2.get_length() == 0) 
        return (*path).to_string(rp);
    (*path).append('/');
    (*path).append(path2);
    return (*path).to_string(rp);
}

}
}