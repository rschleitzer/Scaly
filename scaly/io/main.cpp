#include "IO.cpp"

using namespace scaly::io;

void test_file() {
    {
        Region _r_1;
        auto file_not_found_result = File::read_to_string(_r_1.get_page(), _r_1.get_page(), String(_r_1.get_page(), "foo"));
        if (file_not_found_result._tag != Result<String, FileError>::Error)
            exit (-1);
    }
    
    {
        Region _r_1;
        auto main_text_result = File::read_to_string(_r_1.get_page(), _r_1.get_page(), String(_r_1.get_page(), "bar"));
        if (main_text_result._tag != Result<String, FileError>::Ok)
            exit (-2);
        auto main_text = main_text_result._Ok;
        if (!main_text.equals(String(_r_1.get_page(), "baz")))
            exit (-3);
    }
}

void test_path() {
    Region _r;

    auto path = String(_r.get_page(), "../scaly.scaly");
    auto directory_name = Path::get_directory_name(_r.get_page(), path);
    if (!directory_name.equals(String(_r.get_page(), "..")))
        exit(-4);

    auto file_name = Path::get_file_name(_r.get_page(), String(_r.get_page(), path));
    if (!file_name.equals(String(_r.get_page(), "scaly.scaly")))
        exit(-5);

    auto joined_path = Path::join(_r.get_page(), directory_name, file_name);
    if (!joined_path.equals(path))
        exit(-6);

    directory_name = Path::get_directory_name(_r.get_page(), file_name);
    if (!directory_name.equals(String(_r.get_page(), "")))
        exit(-7);
}

int main(int argc, char** argv)
{
    test_path();
    test_file();
}
