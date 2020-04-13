use crate::compiler::Compiler;
use std::io;
use std::io::Write;

pub struct Repl {
    deck: String,
    compiler: Compiler,
}

impl Repl {
    pub fn new() -> Repl {
        Repl {
            deck: String::new(),
            compiler: Compiler::new(),
        }
    }

    pub fn run(&mut self) {
        loop {
            print!("scalyc>");
            io::stdout().flush().unwrap();
            let mut card = String::new();
            io::stdin()
                .read_line(&mut card)
                .expect("failed to read line");
            let ch = card.chars().next().unwrap();
            if ch == '%' {
                break;
            }
            loop {
                self.deck.push_str(card.as_str());
                let result = self.compiler.evaluate(&mut self.deck);
                if result.len() > 0 {
                    print!("{}", result);
                    self.deck = String::new();
                    break;
                }
                io::stdin()
                    .read_line(&mut card)
                    .expect("failed to read line");
            }
        }
    }
}
