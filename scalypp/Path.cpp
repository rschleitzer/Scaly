#include "Scaly.h"
namespace scaly {

string* Path::getFileNameWithoutExtension(_Page* _rp, string* path) {
    string* fileName = getFileName(_rp, path);
    if (!fileName)
        return 0;
    
    _Array<string>& fileNameComponents = fileName->Split(_rp, '.');
    size_t fileNameComponentsLength = fileNameComponents.length();
    if (fileNameComponentsLength == 1)
        return new(_rp) string(path);
        
    VarString* ret = new(_rp) VarString();
    for (size_t _i = 0; _i < fileNameComponentsLength - 1; _i++)
        ret->append(*fileNameComponents[_i]);
        
    return new(_rp) string(ret);
}

string* Path::getFileName(_Page* _rp, string* path) {
    if (!path->getLength())
        return 0;
    if (path->charAt(path->getLength() - 1) == '/')
        return 0;
    _Array<string>& pathComponents = path->Split(_rp, '/');
    return *pathComponents[pathComponents.length() - 1];
}

}
