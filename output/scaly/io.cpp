#include "../scaly.h"
namespace scaly {

void io::test_file() {
    auto r = Region();
    const auto _text_result = File::read_to_string(r.get_page(), r.get_page(), String(r.get_page(), "bar"));
    auto text = _text_result._Ok;
    if (_text_result._tag == Success::Error) {
        const auto _text_Error = _text_result._Error;
        switch (_text_Error._tag) {
            default: {
                exit(-2);
                break;
            }
        }
    }
    if (text.equals(String(r.get_page(), "baz")) == false) 
        exit(-3);
}

void io::test() {
    test_file();
}

}