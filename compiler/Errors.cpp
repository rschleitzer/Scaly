namespace scaly::compiler {

struct ParserError : Object {
    String file_name;
    size_t position;
};

}