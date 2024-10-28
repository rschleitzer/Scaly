#ifndef _containers_h
#define _containers_h
#include "../scaly.h"
using namespace scaly;
using namespace scaly::memory;
namespace containers {
#include "containers/Node.h"
#include "containers/ListIterator.h"
#include "containers/List.h"
#include "containers/Vector.h"
#include "containers/Array.h"
#include "containers/BuilderListIterator.h"
#include "containers/BuilderList.h"
#include "containers/hashing.h"
#include "containers/HashSetBuilder.h"
#include "containers/HashSet.h"
#include "containers/HashMapBuilder.h"
#include "containers/HashMap.h"
#include "containers/MultiMapBuilder.h"
#include "containers/MultiMap.h"
#include "containers/String.h"
#include "containers/StringIterator.h"
#include "containers/StringBuilder.h"

    void test_hash_set();
    void test();
    void test_multi_map();
    void test_vector();
    void test_array();
    void test_list();
    void test_string_builder();
    void test_string();
    void test_hash_map();
}
#endif