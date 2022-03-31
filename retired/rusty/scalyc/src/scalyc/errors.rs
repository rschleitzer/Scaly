use scaly::containers::String;

pub struct ParserError {
    pub file_name: String,
    pub line: usize,
    pub column: usize,
}
