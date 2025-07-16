#include "main.h"

int main(int argc, char** argv) {
    auto r = Region();
    {
    auto _result = scaly::compiler::compiler::transpile(r.get_page(), String(r.get_page(), *(argv+1)), String(r.get_page(), *(argv+2)));
    switch (_result._tag)
    {
        case Success::Error:
        {
            auto error = _result._Error;
            {
                auto error_message = error.to_string(r.get_page());
                Console::print(error_message);
                exit(-2);
            };
            break;
        }
        default:
            {
        };
    }
};
    return 0;
}
