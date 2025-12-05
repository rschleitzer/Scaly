#ifndef _Path_h
#define _Path_h
#include "../../scaly.h"
using namespace scaly;
using namespace scaly::memory;

struct Path : Object {

    static String get_directory_name(Page* rp, String path);
    static String get_file_name(Page* rp, String path);
    static String join(Page* rp, String path1, String path2);
};

#endif