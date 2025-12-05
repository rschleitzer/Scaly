#include "../../scaly.h"
namespace scaly {
namespace io {
using namespace scaly::containers;


UnknownFileError::UnknownFileError(String file_name) : file_name(file_name) {}

NoSuchFileOrDirectoryError::NoSuchFileOrDirectoryError(String file_name) : file_name(file_name) {}

String FileError::to_string(Page* rp) {
    auto r = Region();
    StringBuilder& message_builder = *new (alignof(StringBuilder), r.get_page()) StringBuilder();
    {
        auto _result = *this;
        switch (_result._tag)
        {
            case FileError::Unknown:
            {
                auto unknown = _result._Unknown;
                {
                    message_builder.append("An unknown file error occurred with the file ");
                    message_builder.append(unknown.file_name);
                    message_builder.append(".");
                };
                break;
            }
            case FileError::NoSuchFileOrDirectory:
            {
                auto no_such = _result._NoSuchFileOrDirectory;
                {
                    message_builder.append("A file or directory with the name ");
                    message_builder.append(no_such.file_name);
                    message_builder.append(" does not exist.\n");
                };
                break;
            }
        }
    };
    return message_builder.to_string(rp);
}
FileError::FileError(struct UnknownFileError _Unknown) : _tag(Unknown), _Unknown(_Unknown) {}

FileError::FileError(struct NoSuchFileOrDirectoryError _NoSuchFileOrDirectory) : _tag(NoSuchFileOrDirectory), _NoSuchFileOrDirectory(_NoSuchFileOrDirectory) {}


}
}