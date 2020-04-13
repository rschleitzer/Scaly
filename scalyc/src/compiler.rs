pub struct Compiler {}

impl Compiler {
    pub fn new() -> Compiler {
        Compiler {}
    }
    pub fn evaluate(&mut self, deck: &mut String) -> String {
        deck.clone()
    }
}
