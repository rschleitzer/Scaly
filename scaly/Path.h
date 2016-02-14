#ifndef __Scaly__Path__
#define __Scaly__Path__
namespace scaly {
    
class Path {
public:
    static _VarString* getFileNameWithoutExtension(_Page* _rp, _VarString& path);
    static _VarString* getFileName(_Page* _rp, _VarString& path);
};

}

#endif // __Scaly__Path__
