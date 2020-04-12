use std::io;
use std::io::Write;

fn main() {
    print!("scalyc>");
    io::stdout().flush().unwrap();
    let mut card = String::new();
    io::stdin().read_line(&mut card).expect("failed to read line");
    println!("The card is {}", card);
}
