#include "../scaly.h"
namespace scaly {

void containers::test_hash_set() {
    auto r = Region();
    auto array = new (alignof(Array<String>), r.get_page()) Array<String>();
    (*array).add(String(r.get_page(), "using"));
    (*array).add(String(r.get_page(), "namespace"));
    (*array).add(String(r.get_page(), "typedef"));
    const auto vector = Vector<String>(r.get_page(), *array);
    auto keywords_builder = new (alignof(HashSetBuilder<String>), r.get_page()) HashSetBuilder<String>(vector);
    if ((*keywords_builder).contains(String(r.get_page(), "using")) == false) 
        exit(-18);
    if ((*keywords_builder).contains(String(r.get_page(), "namespace")) == false) 
        exit(-18);
    if ((*keywords_builder).contains(String(r.get_page(), "typedef")) == false) 
        exit(-18);
    if ((*keywords_builder).contains(String(r.get_page(), "nix"))) 
        exit(-18);
    auto keywords = new (alignof(HashSet<String>), r.get_page()) HashSet<String>(r.get_page(), *keywords_builder);
    if ((*keywords).contains(String(r.get_page(), "using")) == false) 
        exit(-19);
    if ((*keywords).contains(String(r.get_page(), "namespace")) == false) 
        exit(-19);
    if ((*keywords).contains(String(r.get_page(), "typedef")) == false) 
        exit(-19);
    if ((*keywords).contains(String(r.get_page(), "nix"))) 
        exit(-19);
}

void containers::test() {
    test_vector();
    test_array();
    test_string();
    test_string_builder();
    test_list();
    test_hash_set();
    test_hash_map();
    test_multi_map();
}

void containers::test_multi_map() {
    {
        auto r = Region();
        auto p = r.get_page();
        auto map_builder = new (alignof(MultiMapBuilder<String, size_t>), r.get_page()) MultiMapBuilder<String, size_t>();
        {
            char i = 'A';
            while (i<='Z') {
                char j = 'a';
                while (j<='z') {
                    char k = '0';
                    while (k<='9') {
                        char l = '!';
                        while (l<='/') {
                            auto r_2 = Region();
                            auto p_2 = r.get_page();
                            auto sb = new (alignof(StringBuilder), p_2) StringBuilder(i);
                            (*sb).append(j);
                            (*sb).append(k);
                            (*sb).append(l);
                            (*map_builder).add((*sb).to_string(p), (size_t)i*j*k*l);
                            (*map_builder).add((*sb).to_string(p), (size_t)i*j*k*l+1);
                            (*map_builder).add((*sb).to_string(p), (size_t)i*j*k*l+2);
                            l = l+1;
                        };
                        k = k+1;
                    };
                    j = j+1;
                };
                i = i+1;
            };
        };
        auto map = new (alignof(MultiMap<String, size_t>), r.get_page()) MultiMap<String, size_t>(p, *map_builder);
        {
            char i = 'A';
            while (i<='Z') {
                char j = 'a';
                while (j<='z') {
                    char k = '0';
                    while (k<='9') {
                        char l = '!';
                        while (l<='/') {
                            auto r_2 = Region();
                            auto p_2 = r.get_page();
                            auto sb = new (alignof(StringBuilder), p_2) StringBuilder(i);
                            (*sb).append(j);
                            (*sb).append(k);
                            (*sb).append(l);
                            const auto theString = (*sb).to_string(p_2);
                            if ((*map_builder).contains(theString) == false) 
                                exit(-27);
                            if ((*map).contains(theString) == false) 
                                exit(-27);
                            if (*(*(*map_builder)[theString]).get(0) != (size_t)i*j*k*l) 
                                exit(-28);
                            if (*(*(*map)[theString]).get(0) != (size_t)i*j*k*l) 
                                exit(-28);
                            if (*(*(*map_builder)[theString]).get(1) != (size_t)i*j*k*l+1) 
                                exit(-28);
                            if (*(*(*map)[theString]).get(1) != (size_t)i*j*k*l+1) 
                                exit(-28);
                            if (*(*(*map_builder)[theString]).get(2) != (size_t)i*j*k*l+2) 
                                exit(-28);
                            if (*(*(*map)[theString]).get(2) != (size_t)i*j*k*l+2) 
                                exit(-28);
                            l = l+1;
                        };
                        k = k+1;
                    };
                    j = j+1;
                };
                i = i+1;
            };
        };
    };
}

void containers::test_vector() {
    auto r = Region();
    auto vector = new (alignof(Vector<int>), r.get_page()) Vector<int>(r.get_page(), 2);
    *(*vector).get(0) = 1;
    (*vector).put(1, 2);
    if (*((*vector).get(0)) != 1) 
        exit(-1);
    if (*(*vector)[1] != 2) 
        exit(-2);
}

void containers::test_array() {
    auto r = Region();
    auto array = new (alignof(Array<int>), r.get_page()) Array<int>();
    const int huge_number = 1024*1024*64;
    {
        int i = 1;
        while (i<=huge_number) {
            (*array).add(i);
            i = i+1;
        };
    };
    const int* buffer = (*array).get_buffer();
    {
        int i = 1;
        while (i<=huge_number) {
            if (*(buffer+i-1) != i) 
                exit(-6);
            i = i+1;
        };
    };
}

void containers::test_list() {
    auto r = Region();
    auto rp = r.get_page();
    auto list = new (alignof(List<int>), r.get_page()) List<int>();
    const int huge_number = 1024*1024*62;
    {
        int i = 1;
        while (i<=huge_number) {
            (*list).add(i);
            i = i+1;
        };
    };
    auto iterator = (*list).get_iterator();
    {
        int i = huge_number;
        while (i>=1) {
            const auto p = iterator.next();
            if (p == nullptr) 
                exit(-16);
            if (*p != i) 
                exit(-17);
            i = i-1;
        };
    };
}

void containers::test_string_builder() {
    auto r = Region();
    auto string_builder = new (alignof(StringBuilder), r.get_page()) StringBuilder();
    const auto length = (*string_builder).get_length();
    if (length != 0) 
        exit(-11);
    (*string_builder).append('a');
    (*string_builder).append('b');
    (*string_builder).append('c');
    if ((*string_builder).to_string(r.get_page()).equals("abc") == false) 
        exit(-12);
    if ((*string_builder).get_length() != 3) 
        exit(-13);
    auto string_builder2 = new (alignof(StringBuilder), r.get_page()) StringBuilder('d');
    (*string_builder2).append('e');
    (*string_builder2).append('f');
    (*string_builder2).append('g');
    if ((*string_builder2).to_string(r.get_page()).equals("defg") == false) 
        exit(-14);
    if ((*string_builder2).get_length() != 4) 
        exit(-15);
    auto string_builder3 = new (alignof(StringBuilder), r.get_page()) StringBuilder("hijk");
    (*string_builder3).append("lmno");
    if ((*string_builder3).to_string(r.get_page()).equals("hijklmno") == false) 
        exit(-16);
}

void containers::test_string() {
    auto r = Region();
    auto string = String(r.get_page(), "Hello world!");
    const auto length = string.get_length();
    if (length != 12) 
        exit(-7);
    auto long_string = String(r.get_page(), "1234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890");
    if (long_string.get_length() != 130) 
        exit(-8);
    auto c_string = long_string.to_c_string(r.get_page());
    auto string_c = String(r.get_page(), c_string);
    auto semi = String(r.get_page(), ";");
    auto dot = String(r.get_page(), ".");
    if (semi.equals(dot)) 
        exit(-9);
    auto semi2 = String(r.get_page(), ";");
    if (semi.equals(semi2) != true) 
        exit(-10);
}

void containers::test_hash_map() {
    {
        auto r = Region();
        auto p = r.get_page();
        auto array = new (alignof(Array<KeyValuePair<String, int>>), r.get_page()) Array<KeyValuePair<String, int>>();
        (*array).add(KeyValuePair<String, int>(String(p, "using"), 1));
        (*array).add(KeyValuePair<String, int>(String(p, "namespace"), 2));
        (*array).add(KeyValuePair<String, int>(String(p, "typedef"), 3));
        const auto vector = Vector<KeyValuePair<String, int>>(p, *array);
        auto keywords_builder = new (alignof(HashMapBuilder<String, int>), r.get_page()) HashMapBuilder<String, int>(p, vector);
        if ((*keywords_builder).contains(String(p, "using")) == false) 
            exit(-18);
        if ((*keywords_builder).contains(String(p, "namespace")) == false) 
            exit(-19);
        if ((*keywords_builder).contains(String(p, "typedef")) == false) 
            exit(-20);
        if ((*keywords_builder).contains(String(p, "nix"))) 
            exit(-21);
        if (*(*keywords_builder)[String(p, "using")] != 1) 
            exit(-22);
        if (*(*keywords_builder)[String(p, "namespace")] != 2) 
            exit(-23);
        if (*(*keywords_builder)[String(p, "typedef")] != 3) 
            exit(-24);
        if ((*keywords_builder)[String(p, "nix")] != nullptr) 
            exit(-25);
        auto keywords = new (alignof(HashMap<String, int>), r.get_page()) HashMap<String, int>(p, *keywords_builder);
        if ((*keywords).contains(String(p, "using")) == false) 
            exit(-18);
        if ((*keywords).contains(String(p, "namespace")) == false) 
            exit(-19);
        if ((*keywords).contains(String(p, "typedef")) == false) 
            exit(-20);
        if ((*keywords).contains(String(p, "nix"))) 
            exit(-21);
        if (*(*keywords)[String(p, "using")] != 1) 
            exit(-22);
        if (*(*keywords)[String(p, "namespace")] != 2) 
            exit(-23);
        if (*(*keywords)[String(p, "typedef")] != 3) 
            exit(-24);
        if ((*keywords)[String(p, "nix")] != nullptr) 
            exit(-25);
    };
    {
        auto r = Region();
        auto p = r.get_page();
        auto map = new (alignof(HashMapBuilder<String, size_t>), r.get_page()) HashMapBuilder<String, size_t>();
        auto hash_set = new (alignof(HashSetBuilder<String>), r.get_page()) HashSetBuilder<String>();
        {
            char i = 'A';
            while (i<='Z') {
                char j = 'a';
                while (j<='z') {
                    char k = '0';
                    while (k<='9') {
                        char l = '!';
                        while (l<='/') {
                            auto r_2 = Region();
                            auto p_2 = r.get_page();
                            auto sb = new (alignof(StringBuilder), p_2) StringBuilder(i);
                            (*sb).append(j);
                            (*sb).append(k);
                            (*sb).append(l);
                            (*map).add((*sb).to_string(p), (size_t)i*j*k*l);
                            (*hash_set).add((*sb).to_string(p));
                            l = l+1;
                        };
                        k = k+1;
                    };
                    j = j+1;
                };
                i = i+1;
            };
        };
        {
            char i = 'A';
            while (i<='Z') {
                char j = 'a';
                while (j<='z') {
                    char k = '0';
                    while (k<='9') {
                        char l = '!';
                        while (l<='/') {
                            auto r_2 = Region();
                            auto p_2 = r.get_page();
                            auto sb = new (alignof(StringBuilder), p_2) StringBuilder(i);
                            (*sb).append(j);
                            (*sb).append(k);
                            (*sb).append(l);
                            const auto theString = (*sb).to_string(p_2);
                            if ((*hash_set).contains(theString) == false) 
                                exit(-26);
                            if ((*map).contains(theString) == false) 
                                exit(-27);
                            if (*(*map)[theString] != (size_t)i*j*k*l) 
                                exit(-28);
                            l = l+1;
                        };
                        k = k+1;
                    };
                    j = j+1;
                };
                i = i+1;
            };
        };
    };
}

}