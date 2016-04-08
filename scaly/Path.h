#ifndef __Scaly__Path__
#define __Scaly__Path__
namespace scaly {
    
class Path {
public:
    static String* getFileNameWithoutExtension(_Page* _rp, String& path);
    static String* getFileName(_Page* _rp, String& path);
};

}

#endif // __Scaly__Path__
