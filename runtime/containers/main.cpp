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

int main(int argc, char** argv)
{
    auto heap = Heap::create();
    auto root_stack_bucket = StackBucket::create(&heap);
    auto root_page = Page::get(root_stack_bucket);

    test_vector(root_page);
    test_array(root_page);
}
