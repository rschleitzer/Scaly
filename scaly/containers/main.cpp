#include "Containers.cpp"
using namespace scaly::containers;

void test_vector()
{
    {
        Region _r_1;
        Vector<int>& vector = *new(alignof(Vector<int>), _r_1.get_page()) Vector<int>(_r_1.get_page(), 2);
        *vector[0] = 1;
        vector.set(1, 2);
        if (*(vector.get(0)) != 1)
            exit(-1);
        if (*vector[1] != 2)
            exit(-2);
    }
    {
        Region _r_1;
        const char* string_array[3] = {"using", "namespace", "typedef"};
        Vector<const char*> vector = Vector<const char*>(_r_1.get_page(), (const char**)string_array, 3);
        if (*(vector.get(0)) != string_array[0])
            exit(-3);
        if (*vector[1] != string_array[1])
            exit(-4);
        if (*(vector.get(2)) != string_array[2])
            exit(-5);
    }
}

void test_array()
{
    Region _r;
    Array<int>& array = *new(alignof(Array<int>), _r.get_page()) Array<int>();
    // A quarter gigabyte worth of ints
    int huge_number = 1024 * 1024 * 64;
    for (int i = 1; i <= huge_number; i++)
        array.add(i);
    
    int* buffer = array.get_buffer();
    for (int i = 1; i <= huge_number; i++)
        if (buffer[i - 1] != i)
            exit(-6);
}

void test_string() {
    Region _r;
    auto string = String(_r.get_page(), "Hello world!");
    auto length = string.get_length();
    if (length != 12)
        exit(-7);
    auto long_string = String(_r.get_page(), "1234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890");
    if (long_string.get_length() != 130)
        exit(-8);
    auto c_string = long_string.to_c_string(_r.get_page());
    auto string_c = String(_r.get_page(), c_string);

    auto semi = String(_r.get_page(), ";");
    auto dot = String(_r.get_page(), ".");
    if (semi.equals(dot))
        exit(-9);
    auto semi2 = String(_r.get_page(), ";");
    if (!semi.equals(semi2))
        exit(-10);
}

void test_string_builder() {
    Region _r;
    StringBuilder& string_builder = *new(alignof(StringBuilder), _r.get_page()) StringBuilder();
    auto length = string_builder.get_length();
    if (length != 0)
        exit(-11);
    string_builder.append('a');
    string_builder.append('b');
    string_builder.append('c');
    if (!string_builder.to_string(_r.get_page()).equals("abc"))
        exit(-12);
    if (string_builder.get_length() != 3)
        exit(-13);

    StringBuilder& string_builder2 = *new(alignof(StringBuilder), _r.get_page()) StringBuilder('d');
    string_builder2.append('e');
    string_builder2.append('f');
    string_builder2.append('g');
    if (!string_builder2.to_string(_r.get_page()).equals("defg"))
        exit(-14);
    if (string_builder2.get_length() != 4)
        exit(-15);
    
    StringBuilder& string_builder3 = *new(alignof(StringBuilder), _r.get_page()) StringBuilder("hijk");    
    string_builder3.append("lmno");
    if (!string_builder3.to_string(_r.get_page()).equals("hijklmno"))
        exit(-16);
}

void test_list() {
    Region _r;
    List<int>& list = *new(alignof(List<int>), _r.get_page()) List<int>();

    int huge_number = 1024 * 1024 * 62;
    for (int i = 1; i <= huge_number ; i++)
        list.add(i);

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

void test_hash_set() {
    Region _r;

    Array<String>& array = *new(alignof(Array<String>), _r.get_page()) Array<String>();
    array.add(String(_r.get_page(), "using"));
    array.add(String(_r.get_page(), "namespace"));
    array.add(String(_r.get_page(), "typedef"));
    Vector<String> vector = Vector<String>(_r.get_page(), array);
    HashSetBuilder<String>& keywords_builder = *new(alignof(HashSetBuilder<String>), _r.get_page()) HashSetBuilder<String>(_r.get_page(), vector);
    if (!keywords_builder.contains(String(_r.get_page(), "using")))
        exit (-18);
    if (!keywords_builder.contains(String(_r.get_page(), "namespace")))
        exit (-18);
    if (!keywords_builder.contains(String(_r.get_page(), "typedef")))
        exit (-18);
    if (keywords_builder.contains(String(_r.get_page(), "nix")))
        exit (-18);
    HashSet<String>& keywords = *new(alignof(HashSet<String>), _r.get_page())  HashSet<String>(_r.get_page(), keywords_builder);
    if (!keywords.contains(String(_r.get_page(), "using")))
        exit (-19);
    if (!keywords.contains(String(_r.get_page(), "namespace")))
        exit (-19);
    if (!keywords.contains(String(_r.get_page(), "typedef")))
        exit (-19);
    if (keywords.contains(String(_r.get_page(), "nix")))
        exit (-19);
}

void test_hash_map() {
    {
        Region _r_1;
        Array<KeyValuePair<String, int>>& array = *new(alignof(Array<KeyValuePair<String, int>>), _r_1.get_page()) Array<KeyValuePair<String, int>>();
        array.add(KeyValuePair<String, int> { .key = String(_r_1.get_page(), "using"), .value = 1 });
        array.add(KeyValuePair<String, int> { .key = String(_r_1.get_page(), "namespace"), .value = 2 });
        array.add(KeyValuePair<String, int> { .key = String(_r_1.get_page(), "typedef"), .value = 3 });
        Vector<KeyValuePair<String, int>> vector = Vector<KeyValuePair<String, int>>(_r_1.get_page(), array);
        HashMapBuilder<String, int>& keywords_builder = *new(alignof(HashMapBuilder<String, int>), _r_1.get_page()) HashMapBuilder<String, int>(_r_1.get_page(), vector);
        if (!keywords_builder.contains(String(_r_1.get_page(), "using")))
            exit (-18);
        if (!keywords_builder.contains(String(_r_1.get_page(), "namespace")))
            exit (-19);
        if (!keywords_builder.contains(String(_r_1.get_page(), "typedef")))
            exit (-20);
        if (keywords_builder.contains(String(_r_1.get_page(), "nix")))
            exit (-21);
        if (*keywords_builder[String(_r_1.get_page(), "using")] != 1)
            exit (-22);
        if (*keywords_builder[String(_r_1.get_page(), "namespace")] != 2)
            exit (-23);
        if (*keywords_builder[String(_r_1.get_page(), "typedef")] != 3)
            exit (-24);
        if (keywords_builder[String(_r_1.get_page(), "nix")] != nullptr)
            exit (-25);
        HashMap<String, int>& keywords = *new(alignof(HashMap<String, int>), _r_1.get_page()) HashMap<String, int>(_r_1.get_page(), keywords_builder);
        if (!keywords.contains(String(_r_1.get_page(), "using")))
            exit (-18);
        if (!keywords.contains(String(_r_1.get_page(), "namespace")))
            exit (-19);
        if (!keywords.contains(String(_r_1.get_page(), "typedef")))
            exit (-20);
        if (keywords.contains(String(_r_1.get_page(), "nix")))
            exit (-21);
        if (*keywords[String(_r_1.get_page(), "using")] != 1)
            exit (-22);
        if (*keywords[String(_r_1.get_page(), "namespace")] != 2)
            exit (-23);
        if (*keywords[String(_r_1.get_page(), "typedef")] != 3)
            exit (-24);
        if (keywords[String(_r_1.get_page(), "nix")] != nullptr)
            exit (-25);
    }
    {
        Region _r_1;
        HashMapBuilder<String, size_t>& map = *new(alignof(HashMapBuilder<String, size_t>), _r_1.get_page()) HashMapBuilder<String, size_t>();
        HashSetBuilder<String>& set = *new(alignof(HashSetBuilder<String>), _r_1.get_page()) HashSetBuilder<String>();
        for (char i = 'A'; i <= 'Z'; i++)
        {
            for (char j = 'a'; j <= 'z'; j++)
            {
                for (char k = '0'; k <= '9'; k++)
                {
                    for (char l = '!'; l <= '/'; l++)
                    {
                        Region _r_2;
                        StringBuilder& sb = *new(alignof(StringBuilder), _r_2.get_page()) StringBuilder(i);
                        sb.append(j);
                        sb.append(k);
                        sb.append(l);
                        map.add(sb.to_string(_r_1.get_page()), (size_t)i * j * k * l);
                        set.add(sb.to_string(_r_1.get_page()));
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
                        Region _r_2;
                        StringBuilder& sb = *new(alignof(StringBuilder), _r_2.get_page()) StringBuilder(i);
                        sb.append(j);
                        sb.append(k);
                        sb.append(l);
                        String theString = sb.to_string(_r_1.get_page());
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

void test_multi_map() {
    {
        Region _r_1;
        MultiMapBuilder<String, size_t>& map_builder = *new(alignof(MultiMapBuilder<String, size_t>), _r_1.get_page()) MultiMapBuilder<String, size_t>();
        for (char i = 'A'; i <= 'Z'; i++)
        {
            for (char j = 'a'; j <= 'z'; j++)
            {
                for (char k = '0'; k <= '9'; k++)
                {
                    for (char l = '!'; l <= '/'; l++)
                    {
                        Region _r_2;
                        StringBuilder& sb = *new(alignof(StringBuilder), _r_2.get_page()) StringBuilder(i);
                        sb.append(j);
                        sb.append(k);
                        sb.append(l);
                        map_builder.add(sb.to_string(_r_1.get_page()), (size_t)i * j * k * l);
                        map_builder.add(sb.to_string(_r_1.get_page()), (size_t)i * j * k * l + 1);
                        map_builder.add(sb.to_string(_r_1.get_page()), (size_t)i * j * k * l + 2);
                    }
                }
            }
        }
        MultiMap<String, size_t>& map = *new(alignof(MultiMap<String, size_t>), _r_1.get_page()) MultiMap<String, size_t>(_r_1.get_page(), map_builder);
        for (char i = 'A'; i <= 'Z'; i++)
        {
            for (char j = 'a'; j <= 'z'; j++)
            {
                for (char k = '0'; k <= '9'; k++)
                {
                    for (char l = '!'; l <= '/'; l++)
                    {
                        Region _r_2;
                        StringBuilder& sb = *new(alignof(StringBuilder), _r_2.get_page()) StringBuilder(i);
                        sb.append(j);
                        sb.append(k);
                        sb.append(l);
                        String theString = (sb.to_string(_r_2.get_page()));
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
    test_vector();
    test_array();
    test_string();
    test_string_builder();
    test_list();
    test_hash_set();
    test_hash_map();
    test_multi_map();
}
