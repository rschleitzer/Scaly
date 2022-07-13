#include "IO.cpp"

using namespace scaly::io;

void test_file() {
    {
        Region _r_1;
        auto file_not_found_result = File::read_to_string(_r_1.page, _r_1.page, String(_r_1.page, "foo"));
        if (file_not_found_result._tag != Result<String, FileError>::Error)
            exit (-1);
    }
    
    {
        Region _r_1;
        auto main_text_result = File::read_to_string(_r_1.page, _r_1.page, String(_r_1.page, "bar"));
        if (main_text_result._tag != Result<String, FileError>::Ok)
            exit (-2);
        auto main_text = main_text_result._Ok;
        if (!main_text.equals(String(_r_1.page, "baz")))
            exit (-3);
    }
}

int main(int argc, char** argv)
{
    test_file();
}
