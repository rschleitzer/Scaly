#include "../../scaly.h"
namespace scaly {
namespace io {
using namespace scaly::containers;


UnknownFileError::UnknownFileError(String file_name) : file_name(file_name) {}

NoSuchFileOrDirectoryError::NoSuchFileOrDirectoryError(String file_name) : file_name(file_name) {}
FileError::FileError(UnknownFileError _Unknown) : _tag(Unknown), _Unknown(_Unknown) {}

FileError::FileError(NoSuchFileOrDirectoryError _NoSuchFileOrDirectory) : _tag(NoSuchFileOrDirectory), _NoSuchFileOrDirectory(_NoSuchFileOrDirectory) {}


}
}