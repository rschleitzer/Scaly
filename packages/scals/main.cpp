#include "main.h"

int main(int argc, char** argv) {
    auto r = Region();
    auto fd = 0;
    if (argc == 2) {
    const auto arg = *(argv+1);
    auto argument = String(r.get_page(), arg);
    fd = open(argument.to_c_string(r.get_page()), 0);
};
    while (auto message = get_message(r.get_page(), fd)) process_message(*message);
    return 0;
}
