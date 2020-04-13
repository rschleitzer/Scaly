mod parser;
use parser::Parser;

pub struct Compiler {}

impl Compiler {
    pub fn new() -> Compiler {
        Compiler {}
    }
    pub fn evaluate(&mut self, deck: &str) -> String {
        let mut parser = Parser::new(deck);
        let _statement = parser.parse_statement();
        String::from(deck)
    }
}
