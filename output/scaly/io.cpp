#include "../scaly.h"
namespace scaly {

void io::test_file() {
    auto r = Region();
    const auto _file_not_found_result = File::read_to_string(r.get_page(), r.get_page(), String(r.get_page(), "foo"));
    auto file_not_found = _file_not_found_result._Ok;
    if (_file_not_found_result._tag == Success::Error) {
        const auto _file_not_found_Error = _file_not_found_result._Error;
        switch (_file_not_found_Error._tag) {
            case FileError::NoSuchFileOrDirectory: {
                const auto error = _file_not_found_Error._NoSuchFileOrDirectory;
                {
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
                break;
            }
            default: {
                exit(-4);
                break;
            }
        }
    }
    ;
}

void io::test() {
    test_file();
}

}