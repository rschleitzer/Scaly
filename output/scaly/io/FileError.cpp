#include "../../scaly.h"
namespace scaly {
namespace io {using namespace scaly::containers;


UnknownFileError::UnknownFileError(String file_name) : file_name(file_name) {}

NoSuchFileOrDirectoryError::NoSuchFileOrDirectoryError(String file_name) : file_name(file_name) {}
FileError::FileError(UnknownFileError _UnknownFileError) : _tag(Unknown) { _Unknown = _UnknownFileError; }

FileError::FileError(NoSuchFileOrDirectoryError _NoSuchFileOrDirectoryError) : _tag(NoSuchFileOrDirectory) { _NoSuchFileOrDirectory = _NoSuchFileOrDirectoryError; }


}
}