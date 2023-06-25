#include "../compiler/Compiler.cpp"

using namespace scaly::containers;

int scalyc(Vector<String>& arguments) {
    return 0;
}

int main(int argc, char** argv) {
    Region _r;
    List<String> list;
    int i = 0;
    while (i < argc) {
        char* raw_argument = argv[i];
        String argument(_r.get_page(), raw_argument);
        list.add(_r.get_page(), argument);
        i++;
    }
    Vector<String> arguments(_r.get_page(), list);

    return scalyc(arguments);
}
