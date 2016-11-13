#ifndef __Scaly__Path__
#define __Scaly__Path__
namespace scaly {
    
class Path {
public:
    static string* getFileNameWithoutExtension(_Page* _rp, string* path);
    static string* getFileName(_Page* _rp, string* path);
};

}

#endif // __Scaly__Path__
