#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

namespace scaly {
namespace io {

struct Directory {
    static Result<bool, FileError> exists(Page *_ep, const String& path);
    static FileError* create(Page *_ep, const String& path);
    static FileError* remove(Page *_ep, const String& path);
};

}
}