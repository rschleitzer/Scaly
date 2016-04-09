#include "Scaly.h"
namespace scaly {

String* Path::getFileNameWithoutExtension(_Page* _rp, String& path) {
    String* fileName = getFileName(_rp, path);
    if (!fileName)
        return 0;
    
    _Array<String>& fileNameComponents = fileName->Split(_rp, '.');
    size_t fileNameComponentsLength = fileNameComponents.length();
    if (fileNameComponentsLength == 1)
        return &String::create(_rp, &path);
        
    VarString* ret = new(_rp) VarString();
    for (size_t _i = 0; _i < fileNameComponentsLength - 1; _i++)
        ret->append(*fileNameComponents[_i]);
        
    return &String::create(_rp, ret);
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
