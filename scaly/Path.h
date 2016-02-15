#ifndef __Scaly__Path__
#define __Scaly__Path__
namespace scaly {
    
class Path {
public:
    static _LetString* getFileNameWithoutExtension(_Page* _rp, _LetString& path);
    static _LetString* getFileName(_Page* _rp, _LetString& path);
};

}

#endif // __Scaly__Path__
