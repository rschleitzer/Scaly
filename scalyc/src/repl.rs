use crate::compiler::Compiler;
use std::io;
use std::io::Write;

pub struct Repl {
    file: String,
    compiler: Compiler,
}

impl Repl {
    pub fn new() -> Repl {
        let mut compiler = Compiler::new();
        compiler.load_standard_library();
        Repl {
            file: String::new(),
            compiler: compiler,
        }
    }

    pub fn run(&mut self) {
        loop {
            let mut deck = String::new();
            'deck: loop {
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
                    deck.push_str(card.as_str());
                    let result = self.compiler.evaluate_deck(&mut deck);
                    match result {
                        Ok(output) => {
                            if output.len() > 0 {
                                println!("{}", output);
                                break;
                            } else {
                                self.file.push_str(deck.as_ref());
                                deck = String::new();
                                println!("Declaration added.");
                            }
                        }
                        Err(error) => {
                            if error.len() != 0 {
                                print!("{}", error);
                                break 'deck;
                            }
                        }
                    }
                    card = String::new();
                    io::stdin()
                        .read_line(&mut card)
                        .expect("failed to read line");
                }
            }
        }
    }
}
