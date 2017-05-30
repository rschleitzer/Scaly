#include "scaly.h"

scaly_string* scaly_Path_getFileNameWithoutExtension(scaly_Page* _rp, scaly_string* path) {
    scaly_string* fileName = scaly_Path_getFileName(_rp, path);
    if (!fileName)
        return 0;
    
    scaly_Array* fileNameComponents = scaly_string_split(_rp, fileName, '.');
    size_t fileNameComponentsLength = fileNameComponents->_size;
    if (fileNameComponentsLength == 1)
        return scaly_string_fromString(_rp, path);
        
    scaly_VarString* ret = scaly_VarString_new(_rp);
    for (size_t _i = 0; _i < fileNameComponentsLength - 1; _i++)
        scaly_VarString_appendString(ret, (scaly_string*)(*scaly_Array_elementAt(fileNameComponents, _i)));
        
    return scaly_string_fromVarString(_rp, ret);
}

scaly_string* scaly_Path_getFileName(scaly_Page* _rp, scaly_string* path) {
    if (!scaly_string_getLength(path))
        return 0;
    if (scaly_string_charAt(path, scaly_string_getLength(path) - 1) == '/')
        return 0;
    scaly_Array* pathComponents = scaly_string_split(_rp, path, '/');
    return (scaly_string*)*scaly_Array_elementAt(pathComponents, pathComponents->_size - 1);
}
