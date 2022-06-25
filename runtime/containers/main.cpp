#include "Containers.cpp"
using namespace scaly::containers;

void test_vector(Region& _pr)
{
    auto _r = Region::create(_pr);
    {
        auto r_1 = Region::create(_r);
        Vector<int>& vector = *new(alignof(Vector<int>), r_1.page) Vector<int>(r_1.page, 2);
        *vector[0] = 1;
        vector.set(1, 2);
        if (*(vector.get(0)) != 1)
            exit(-1);
        if (*vector[1] != 2)
            exit(-2);
    }
    {
        auto r_1 = Region::create(_r);
        const char* string_array[3] = {"using", "namespace", "typedef"};
        Vector<const char*> vector = Vector<const char*>(r_1.page, (const char**)string_array, 3);
        if (*(vector.get(0)) != string_array[0])
            exit(-3);
        if (*vector[1] != string_array[1])
            exit(-4);
        if (*(vector.get(2)) != string_array[2])
            exit(-5);
    }
}

void test_array(Region& _pr)
{
    auto _r = Region::create(_pr);
    Array<int>& array = *new(alignof(Array<int>), _r.page) Array<int>();
    // A quarter gigabyte worth of ints
    int huge_number = 1024 * 1024 * 64;
    for (int i = 1; i <= huge_number; i++)
        array.add(i);
    
    int* buffer = array.get_buffer();
    for (int i = 1; i <= huge_number; i++)
        if (buffer[i - 1] != i)
            exit(-6);
}

void test_string(Region& _pr) {
    auto _r = Region::create(_pr);
    auto string = String(_r.page, "Hello world!");
    auto length = string.get_length();
    if (length != 12)
        exit(-7);
    auto long_string = String(_r.page, "1234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890");
    if (long_string.get_length() != 130)
        exit(-8);
    auto c_string = long_string.to_c_string(_r.page);
    auto string_c = String(_r.page, c_string);

    auto semi = String(_r.page, ";");
    auto dot = String(_r.page, ".");
    if (semi.equals(dot))
        exit(-9);
    auto semi2 = String(_r.page, ";");
    if (!semi.equals(semi2))
        exit(-10);
}

void test_string_builder(Region& _pr) {
    auto _r = Region::create(_pr);
    StringBuilder& string_builder = *new(alignof(StringBuilder), _r.page) StringBuilder();
    auto length = string_builder.get_length();
    if (length != 0)
        exit(-11);
    string_builder.append_character('a');
    string_builder.append_character('b');
    string_builder.append_character('c');
    if (!string_builder.to_string(_r.page).equals(String(_r.page, "abc")))
        exit(-12);
    if (string_builder.get_length() != 3)
        exit(-13);

    StringBuilder& string_builder2 = *new(alignof(StringBuilder), _r.page) StringBuilder('d');
    string_builder2.append_character('e');
    string_builder2.append_character('f');
    string_builder2.append_character('g');
    if (!string_builder2.to_string(_r.page).equals(String(_r.page, "defg")))
        exit(-14);
    if (string_builder2.get_length() != 4)
        exit(-15);
}

void test_list(Region& _pr) {
    auto _r = Region::create(_pr);
    List<int>& list = *List<int>::create(_r.page);

    int huge_number = 1024 * 1024 * 62;
    for (int i = 1; i <= huge_number ; i++)
        list.add(_r.page, i);

    auto iterator = list.get_iterator();
    for (int i = huge_number; i >= 1; i--)
    {
        auto p = iterator.next();
        if (p == nullptr)
            exit(-16);

        if (*p != i)
            exit(-17);
    }
}

void test_hash_set(Region& _pr) {
    auto _r = Region::create(_pr);

    Array<String>& array = *new(alignof(Array<String>), _r.page) Array<String>();
    array.add(String(_r.page, "using"));
    array.add(String(_r.page, "namespace"));
    array.add(String(_r.page, "typedef"));
    Vector<String> vector = Vector<String>(_r.page, array);
    HashSetBuilder<String>& keywords_builder = *new(alignof(HashSetBuilder<String>), _r.page) HashSetBuilder<String>(_r.page, vector);
    if (!keywords_builder.contains(String(_r.page, "using")))
        exit (-18);
    if (!keywords_builder.contains(String(_r.page, "namespace")))
        exit (-18);
    if (!keywords_builder.contains(String(_r.page, "typedef")))
        exit (-18);
    if (keywords_builder.contains(String(_r.page, "nix")))
        exit (-18);
    HashSet<String>& keywords = *new(alignof(HashSet<String>), _r.page)  HashSet<String>(_r, _r.page, keywords_builder);
    if (!keywords.contains(String(_r.page, "using")))
        exit (-19);
    if (!keywords.contains(String(_r.page, "namespace")))
        exit (-19);
    if (!keywords.contains(String(_r.page, "typedef")))
        exit (-19);
    if (keywords.contains(String(_r.page, "nix")))
        exit (-19);
}

void test_hash_map(Region& _pr) {
    auto _r = Region::create(_pr);
    {
        auto _r_1 = Region::create(_r);
        Array<KeyValuePair<String, int>>& array = *new(alignof(Array<KeyValuePair<String, int>>), _r_1.page) Array<KeyValuePair<String, int>>();
        array.add(KeyValuePair<String, int> { .key = String(_r_1.page, "using"), .value = 1 });
        array.add(KeyValuePair<String, int> { .key = String(_r_1.page, "namespace"), .value = 2 });
        array.add(KeyValuePair<String, int> { .key = String(_r_1.page, "typedef"), .value = 3 });
        Vector<KeyValuePair<String, int>> vector = Vector<KeyValuePair<String, int>>(_r_1.page, array);
        HashMapBuilder<String, int>& keywords_builder = *new(alignof(HashMapBuilder<String, int>), _r_1.page) HashMapBuilder<String, int>(_r_1.page, vector);
        if (!keywords_builder.contains(String(_r_1.page, "using")))
            exit (-18);
        if (!keywords_builder.contains(String(_r_1.page, "namespace")))
            exit (-19);
        if (!keywords_builder.contains(String(_r_1.page, "typedef")))
            exit (-20);
        if (keywords_builder.contains(String(_r_1.page, "nix")))
            exit (-21);
        if (*keywords_builder[String(_r_1.page, "using")] != 1)
            exit (-22);
        if (*keywords_builder[String(_r_1.page, "namespace")] != 2)
            exit (-23);
        if (*keywords_builder[String(_r_1.page, "typedef")] != 3)
            exit (-24);
        if (keywords_builder[String(_r_1.page, "nix")] != nullptr)
            exit (-25);
        HashMap<String, int>& keywords = *new(alignof(HashMap<String, int>), _r_1.page) HashMap<String, int>(_r_1, _r_1.page, keywords_builder);
        if (!keywords.contains(String(_r_1.page, "using")))
            exit (-18);
        if (!keywords.contains(String(_r_1.page, "namespace")))
            exit (-19);
        if (!keywords.contains(String(_r_1.page, "typedef")))
            exit (-20);
        if (keywords.contains(String(_r_1.page, "nix")))
            exit (-21);
        if (*keywords[String(_r_1.page, "using")] != 1)
            exit (-22);
        if (*keywords[String(_r_1.page, "namespace")] != 2)
            exit (-23);
        if (*keywords[String(_r_1.page, "typedef")] != 3)
            exit (-24);
        if (keywords[String(_r_1.page, "nix")] != nullptr)
            exit (-25);
    }
    {
        auto _r_1 = Region::create(_r);
        HashMapBuilder<String, size_t>& map = *new(alignof(HashMapBuilder<String, size_t>), _r_1.page) HashMapBuilder<String, size_t>();
        HashSetBuilder<String>& set = *new(alignof(HashSetBuilder<String>), _r_1.page) HashSetBuilder<String>();
        for (char i = 'A'; i <= 'Z'; i++)
        {
            for (char j = 'a'; j <= 'z'; j++)
            {
                for (char k = '0'; k <= '9'; k++)
                {
                    for (char l = '!'; l <= '/'; l++)
                    {
                        Region _r_2 = Region::create(_r_1);
                        StringBuilder& sb = *new(alignof(StringBuilder), _r_2.page) StringBuilder(i);
                        sb.append_character(j);
                        sb.append_character(k);
                        sb.append_character(l);
                        map.add(sb.to_string(_r_1.page), (size_t)i * j * k * l);
                        set.add(sb.to_string(_r_1.page));
                    }
                }
            }
        }
        for (char i = 'A'; i <= 'Z'; i++)
        {
            for (char j = 'a'; j <= 'z'; j++)
            {
                for (char k = '0'; k <= '9'; k++)
                {
                    for (char l = '!'; l <= '/'; l++)
                    {
                        Region _r_2 = Region::create(_r_1);
                        StringBuilder& sb = *new(alignof(StringBuilder), _r_2.page) StringBuilder(i);
                        sb.append_character(j);
                        sb.append_character(k);
                        sb.append_character(l);
                        String theString = sb.to_string(_r_1.page);
                        if (!set.contains(theString))
                             exit(-26);
                        if (!map.contains(theString))
                            exit(-27);
                        if (*map[theString] != (size_t)i * j * k * l)
                            exit(-28);
                    }
                }
            }
        }
    }
}

void test_multi_map(Region& _pr) {
    auto _r = Region::create(_pr);
    {
        auto _r_1 = Region::create(_r);
        MultiMapBuilder<String, size_t>& map_builder = *new(alignof(MultiMapBuilder<String, size_t>), _r_1.page) MultiMapBuilder<String, size_t>();
        for (char i = 'A'; i <= 'Z'; i++)
        {
            for (char j = 'a'; j <= 'z'; j++)
            {
                for (char k = '0'; k <= '9'; k++)
                {
                    for (char l = '!'; l <= '/'; l++)
                    {
                        Region _r_2 = Region::create(_r_1);
                        StringBuilder& sb = *new(alignof(StringBuilder), _r_2.page) StringBuilder(i);
                        sb.append_character(j);
                        sb.append_character(k);
                        sb.append_character(l);
                        map_builder.add(sb.to_string(_r_1.page), (size_t)i * j * k * l);
                        map_builder.add(sb.to_string(_r_1.page), (size_t)i * j * k * l + 1);
                        map_builder.add(sb.to_string(_r_1.page), (size_t)i * j * k * l + 2);
                    }
                }
            }
        }
        MultiMap<String, size_t>& map = *new(alignof(MultiMap<String, size_t>), _r_1.page) MultiMap<String, size_t>(_r_1, _r_1.page, map_builder);
        for (char i = 'A'; i <= 'Z'; i++)
        {
            for (char j = 'a'; j <= 'z'; j++)
            {
                for (char k = '0'; k <= '9'; k++)
                {
                    for (char l = '!'; l <= '/'; l++)
                    {
                        Region _r_2 = Region::create(_r_1);
                        StringBuilder& sb = *new(alignof(StringBuilder), _r_2.page) StringBuilder(i);
                        sb.append_character(j);
                        sb.append_character(k);
                        sb.append_character(l);
                        String theString = (sb.to_string(_r_1.page));
                        if (!map_builder.contains(theString))
                            exit(-27);
                        if (!map.contains(theString))
                            exit(-27);
                        if (*map_builder[theString]->get(0) != (size_t)i * j * k * l)
                            exit(-28);
                        if (*map[theString]->get(0) != (size_t)i * j * k * l)
                            exit(-28);
                        if (*map_builder[theString]->get(1) != (size_t)i * j * k * l + 1)
                            exit(-28);
                        if (*map[theString]->get(1) != (size_t)i * j * k * l + 1)
                            exit(-28);
                        if (*map_builder[theString]->get(2) != (size_t)i * j * k * l + 2)
                            exit(-28);
                        if (*map[theString]->get(2) != (size_t)i * j * k * l + 2)
                            exit(-28);
                    }
                }
            }
        }
    }
}

int main(int argc, char** argv)
{
    auto heap = Heap::create();
    auto root_stack_bucket = StackBucket::create(&heap);
    auto root_page = Page::get(root_stack_bucket);
    auto region = Region(root_page);

    test_vector(region);
    test_array(region);
    test_string(region);
    test_string_builder(region);
    test_list(region);
    test_hash_set(region);
    test_hash_map(region);
    test_multi_map(region);
}
