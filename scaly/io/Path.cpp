#include "../containers/Containers.cpp"

#include <libgen.h>
#include <filesystem>
#include "Path.h"

namespace scaly {
namespace io {

using namespace scaly::containers;

String Path::get_directory_name(Page* _rp, const String& path) {
    Region _r;
    auto path_as_c_string = path.to_c_string(_r.get_page());
    auto directory_name_as_c_string = dirname((char*)path_as_c_string);
    if (strcmp(directory_name_as_c_string, ".") == 0)
        return String(_rp, "");

    String ret = String(_rp, directory_name_as_c_string);
    return ret;
}

String Path::get_file_name(Page* _rp, const String& path) {
    Region _r;
    auto path_as_c_string = path.to_c_string(_r.get_page());
    auto file_name_as_c_string = basename((char*)path_as_c_string);
    String ret = String(_rp, file_name_as_c_string);
    return ret;
}

String Path::get_file_name_without_extension(Page* _rp, const String& path) {
    Region _r;
    auto path_as_c_string = path.to_c_string(_r.get_page());
    std::filesystem::path p = path_as_c_string;
    auto stem = p.stem();
    auto file_name_without_extension_as_c_string = stem.c_str();
    String ret = String(_rp, file_name_without_extension_as_c_string);
    return ret;
}

String Path::join(Page* _rp, const String& path1, const String& path2) {
    Region _r;
    if (path1.get_length() == 0)
        return String(_rp, path2);
    
    StringBuilder& path = *new(alignof(StringBuilder), _r.get_page()) StringBuilder();
    path.append_string(path1);
    if (path2.get_length() == 0)
        return path.to_string(_rp);

    path.append_character('/');
    path.append_string(path2);
    return path.to_string(_rp);
}

}
}