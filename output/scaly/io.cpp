#include "../scaly.h"
namespace scaly {

void io::test_file() {
    auto r = Region();
    const auto file_not_found_result = File::read_to_string(r.get_page(), r.get_page(), String(r.get_page(), "foo"));
    const auto main_text_result = File::read_to_string(r.get_page(), r.get_page(), String(r.get_page(), "bar"));
    const auto main_text = main_text_result.Ok;
}

void io::test() {
    test_file();
}

}