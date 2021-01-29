mod compiler;
mod repl;

use compiler::Compiler;
use repl::Repl;
use std::env;

fn main() {
    let args: Vec<String> = env::args().collect();
    if args.len() > 1 {
        let mut compiler = Compiler::new();
        match compiler.load_standard_library() {
            Ok(()) => println!("{}", compiler.compile(args[1].clone())),
            Err(error) => println!("{}", error),
        }
        ;
    } else {
        let repl_result = Repl::new();
        match repl_result {
            Ok(mut repl) => repl.run(),
            Err(error) => println!("{}", error),
        }
    }
}
