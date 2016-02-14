#include "Scaly.h"
namespace scaly {

String* Path::getFileNameWithoutExtension(_Page* _rp, String& path) {
    String* fileName = getFileName(_rp, path);
    if (!fileName)
        return 0;
    
    _Array<String>& fileNameComponents = fileName->Split(_rp, '.');
    size_t fileNameComponentsLength = fileNameComponents.length();
    if (fileNameComponentsLength == 1)
        return new(_rp) String(path);
        
    String* ret = new(_rp) String();
    for (size_t _i = 0; _i < fileNameComponentsLength - 1; _i++)
        *ret += **fileNameComponents[_i];
        
    return ret;
}

String* Path::getFileName(_Page* _rp, String& path) {
    if (!path.getLength())
        return 0;
    if (path[path.getLength() - 1] == '/')
        return 0;
    _Array<String>& pathComponents = path.Split(_rp, '/');
    return *pathComponents[pathComponents.length() - 1];
}

}
