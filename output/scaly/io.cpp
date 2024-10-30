#include "../scaly.h"
namespace scaly {

void io::test_file() {
    auto r = Region();
    {
        const auto _file_not_found_result = File::read_to_string(r.get_page(), r.get_page(), String(r.get_page(), "foo"));
        switch (_file_not_found_result._tag) {
            case Result<String, FileError>::Error: {
                const auto _file_not_found_Error = _file_not_found_result._Error;
                switch (_file_not_found_Error._tag) {
                    default:
                        {
                            const auto _text_result = File::read_to_string(r.get_page(), r.get_page(), String(r.get_page(), "bar"));
                            switch (_text_result._tag) {
                                case Result<String, FileError>::Error: {
                                    const auto _text_result_Error = _file_not_found_result._Error;
                                    switch (_text_result_Error._tag) {
                                        default:
                                            exit(-2);
                                            break;
                                    }
                                    break;
                                }
                                case Result<String, FileError>::Ok: {
                                    auto text = _text_result._Ok;
                                    if (!text.equals(String(r.get_page(), "baz")))
                                        exit (-3);
                                    break;
                                }
                            }
                        }
                        break;
                }
                break;
            }
            case Result<String, FileError>::Ok: {
                const auto file_not_found = _file_not_found_result._Ok;
                exit(-1);
                break;
            }
        }
    }
}

void io::test() {
    test_file();
}

}