#include "Containers.cpp"
using namespace scaly::containers;

void test_vector(Page* _rp)
{
    auto r = Region::create_from_page(_rp);
    {
        auto r_1 = Region::create(r);
        Vector<int>& vector = *Vector<int>::create(r_1.page, 2);
        *vector[0] = 1;
        vector.set(1, 2);
        if (vector.get(0) != 1)
            exit(-1);
        if (*vector[1] != 2)
            exit(-2);
    }
    {
        auto r_1 = Region::create(r);
        const char* string_array[3] = {"using", "namespace", "typedef"};
        Vector<const char*>& vector = *Vector<const char*>::from_raw_array(r_1.page, (const char**)string_array, 3);
        if (vector.get(0) != string_array[0])
            exit(-3);
        if (*vector[1] != string_array[1])
            exit(-4);
        if (vector.get(2) != string_array[2])
            exit(-5);
    }
}

void test_array(Page* _rp)
{
    auto r = Region::create_from_page(_rp);
    Array<int>& array = *Array<int>::create(r.page);
    // A gigabyte worth of ints
    for (int i = 1; i <= 1024 * 1024 * 256; i++)
        array.add(i);
    
    int* buffer = array.get_buffer();
    for (int i = 1; i <= 1024 * 1024 * 256; i++)
        if (buffer[i - 1] != i)
            exit(-6);
}

void test_string(Page* _rp) {
    auto r = Region::create_from_page(_rp);
    auto string = String::from_c_string(r.page, "Hello world!");
    auto length = string.get_length();
    if (length != 12)
        exit(-7);
    auto long_string = String::from_c_string(r.page, "1234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890");
    if (long_string.get_length() != 130)
        exit(-8);
    auto c_string = long_string.to_c_string(r.page);
    auto string_c = String::from_c_string(r.page, c_string);

    auto semi = String::from_c_string(r.page, ";");
    auto dot = String::from_c_string(r.page, ".");
    if (semi.equals(dot))
        exit(-9);
    auto semi2 = String::from_c_string(r.page, ";");
    if (!semi.equals(semi2))
        exit(-10);
}

void test_string_builder(Page* _rp) {
    auto r = Region::create_from_page(_rp);
    StringBuilder& string_builder = *StringBuilder::create(r.page);
    auto length = string_builder.get_length();
    if (length != 0)
        exit(-11);
    string_builder.append_character('a');
    string_builder.append_character('b');
    string_builder.append_character('c');
    if (!string_builder.to_string(r.page).equals(String::from_c_string(r.page, "abc")))
        exit(-12);
    if (string_builder.get_length() != 3)
        exit(-13);

    StringBuilder& string_builder2 = *StringBuilder::from_character(r.page, 'd');
    string_builder2.append_character('e');
    string_builder2.append_character('f');
    string_builder2.append_character('g');
    if (!string_builder2.to_string(r.page).equals(String::from_c_string(r.page, "defg")))
        exit(-14);
    if (string_builder2.get_length() != 4)
        exit(-15);
}

void test_list(Page* _rp) {
    auto r = Region::create_from_page(_rp);
    List<int>& list = *List<int>::create(r.page);

    int huge_number = 1024 * 1024 * 63;
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

int main(int argc, char** argv)
{
    auto heap = Heap::create();
    auto root_stack_bucket = StackBucket::create(&heap);
    auto root_page = Page::get(root_stack_bucket);

    // test_vector(root_page);
    // test_array(root_page);
    // test_string(root_page);
    // test_string_builder(root_page);
    test_list(root_page);
}
