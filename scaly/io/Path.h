#include <libgen.h>
#include <filesystem>

namespace scaly {
namespace io {

using namespace scaly::containers;

struct Path {
    static String get_directory_name(Page* _rp, const String& path);
    static String get_file_name(Page* _rp, const String& path);
    static String get_file_name_without_extension(Page* _rp, const String& path);
    static String join(Page* _rp, const String& path1, const String& path2);
};


}
}