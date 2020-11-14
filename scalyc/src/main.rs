mod compiler;
mod repl;

use compiler::Compiler;
use repl::Repl;
use std::env;

fn main() {
    let args: Vec<String> = env::args().collect();
    if args.len() > 1 {
        let mut compiler = Compiler::new();
        compiler.load_standard_library();
        println!("{}", compiler.compile(&args[1]));
    } else {
        let mut repl = Repl::new();
        repl.run();
    }
}
