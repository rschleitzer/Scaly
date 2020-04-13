mod compiler;
mod repl;

use repl::Repl;

fn main() {
    let mut repl = Repl::new();
    repl.run();
}
