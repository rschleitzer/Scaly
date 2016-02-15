#include "Scaly.h"
namespace scaly {

_LetString* Path::getFileNameWithoutExtension(_Page* _rp, _LetString& path) {
    _LetString* fileName = getFileName(_rp, path);
    if (!fileName)
        return 0;
    
    _Array<_LetString>& fileNameComponents = fileName->Split(_rp, '.');
    size_t fileNameComponentsLength = fileNameComponents.length();
    if (fileNameComponentsLength == 1)
        return &_LetString::create(_rp, path);
        
    _VarString* ret = new(_rp) _VarString();
    for (size_t _i = 0; _i < fileNameComponentsLength - 1; _i++)
        *ret += **fileNameComponents[_i];
        
    return &_LetString::create(_rp, *ret);
}

_LetString* Path::getFileName(_Page* _rp, _LetString& path) {
    if (!path.getLength())
        return 0;
    if (path[path.getLength() - 1] == '/')
        return 0;
    _Array<_LetString>& pathComponents = path.Split(_rp, '/');
    return *pathComponents[pathComponents.length() - 1];
}

}
