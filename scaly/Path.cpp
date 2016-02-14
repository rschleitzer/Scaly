#include "Scaly.h"
namespace scaly {

_VarString* Path::getFileNameWithoutExtension(_Page* _rp, _VarString& path) {
    _VarString* fileName = getFileName(_rp, path);
    if (!fileName)
        return 0;
    
    _Array<_VarString>& fileNameComponents = fileName->Split(_rp, '.');
    size_t fileNameComponentsLength = fileNameComponents.length();
    if (fileNameComponentsLength == 1)
        return new(_rp) _VarString(path);
        
    _VarString* ret = new(_rp) _VarString();
    for (size_t _i = 0; _i < fileNameComponentsLength - 1; _i++)
        *ret += **fileNameComponents[_i];
        
    return ret;
}

_VarString* Path::getFileName(_Page* _rp, _VarString& path) {
    if (!path.getLength())
        return 0;
    if (path[path.getLength() - 1] == '/')
        return 0;
    _Array<_VarString>& pathComponents = path.Split(_rp, '/');
    return *pathComponents[pathComponents.length() - 1];
}

}
