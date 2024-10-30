#include "../scaly.h"
namespace scaly {

void io::test_file() {
    auto r = Region();
    const auto _file_not_found_result = File::read_to_string(r.get_page(), r.get_page(), String(r.get_page(), "foo"));
    if (_file_not_found_result._tag == Success::Error) {
        const auto _file_not_found_Error = _file_not_found_result._Error;
        switch (_file_not_found_Error._tag) {
            case FileError::NoSuchFileOrDirectory: {
                const auto error = _file_not_found_Error._NoSuchFileOrDirectory;
                {
                    const auto _text_result = File::read_to_string(r.get_page(), r.get_page(), String(r.get_page(), "bar"));
                    if (_text_result._tag == Success::Error) {
                        const auto _text_Error = _text_result._Error;
                        switch (_text_Error._tag) {
                            default: {
                                exit(-2);
                                break;
                            }
                        }
                    }
                    else
                    {
                        auto text = _text_result._Ok;
                        if (!text.equals(String(r.get_page(), "baz")))
                            exit (-3);
                    }
                }
                break;
            }
            default: {
                exit (-4);
                break;
            }
        }
    }
    else
    {
        const auto file_not_found = _file_not_found_result._Ok;
        exit(-1);
    }
}

void io::test() {
    test_file();
}

}