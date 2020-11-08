mod compiler;
mod repl;

use repl::Repl;
use compiler::Compiler;
use std::env;

fn main() {
    let args: Vec<String> = env::args().collect();
    if args.len() > 1
    {
        let mut compiler = Compiler::new();
        compiler.load_standard_library();
    }
    else
    {
        let mut repl = Repl::new();
        repl.run();
    }
}
