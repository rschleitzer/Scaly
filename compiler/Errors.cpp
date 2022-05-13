namespace scaly {
namespace compiler {

struct ParserError : Object {
    ParserError(String file_name, size_t position) : file_name(file_name), position(position) {}
    String file_name;
    size_t position;
};

}
}
