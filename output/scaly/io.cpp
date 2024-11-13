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
                        default:
                            exit(-2);

                        }
                    };
                    if (text.equals(String(r.get_page(), "baz")) == false) 
                        exit(-3);
                };
                break;
            }
        default:
            exit(-4);

        }
    };
}

void io::test_path() {
    auto r = Region();
    const auto path = String(r.get_page(), "../foo.scaly");
    auto directory_name = Path::get_directory_name(r.get_page(), path);
    if (directory_name.equals(String(r.get_page(), "..")) == false) 
        exit(-4);
    auto file_name = Path::get_file_name(r.get_page(), String(r.get_page(), path));
    if (file_name.equals(String(r.get_page(), "foo.scaly")) == false) 
        exit(-5);
    auto joined_path = Path::join(r.get_page(), directory_name, file_name);
    if (joined_path.equals(path) == false) 
        exit(-6);
    directory_name = Path::get_directory_name(r.get_page(), file_name);
    if (directory_name.equals(String(r.get_page(), "")) == false) 
        exit(-7);
}

void io::test_directory() {
    auto r = Region();
    const auto foo = String(r.get_page(), "foo");
    const auto _dir_exists_result = Directory::exists(r.get_page(), foo);
    auto dir_exists = _dir_exists_result._Ok;
    if (_dir_exists_result._tag == Success::Error) {
        const auto _dir_exists_Error = _dir_exists_result._Error;
        switch (_dir_exists_Error._tag) {
        default:
            exit(-9);

        }
    };
    if (dir_exists) 
        exit(-10);
    {
        const auto _void_result = Directory::create(r.get_page(), foo);
        if (_void_result._tag == Success::Error) {
            const auto _void_Error = _void_result._Error;
            switch (_void_Error._tag) {
            default:
                exit(-11);

            }
        }}
        ;
    const auto _dir_exists_now_result = Directory::exists(r.get_page(), foo);
    auto dir_exists_now = _dir_exists_now_result._Ok;
    if (_dir_exists_now_result._tag == Success::Error) {
        const auto _dir_exists_now_Error = _dir_exists_now_result._Error;
        switch (_dir_exists_now_Error._tag) {
        default:
            exit(-12);

        }
    };
    if (dir_exists_now == false) 
        exit(-13);
    {
        const auto _void_result = Directory::remove(r.get_page(), foo);
        if (_void_result._tag == Success::Error) {
            const auto _void_Error = _void_result._Error;
            switch (_void_Error._tag) {
            default:
                exit(-14);

            }
        }}
        ;
    const auto _dir_exists_still_result = Directory::exists(r.get_page(), foo);
    auto dir_exists_still = _dir_exists_still_result._Ok;
    if (_dir_exists_still_result._tag == Success::Error) {
        const auto _dir_exists_still_Error = _dir_exists_still_result._Error;
        switch (_dir_exists_still_Error._tag) {
        default:
            exit(-15);

        }
    };
    if (dir_exists_still) 
        exit(-16);
}

void io::test() {
    test_directory();
    test_path();
    test_file();
}

}