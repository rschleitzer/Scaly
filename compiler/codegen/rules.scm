(element grammar
    (file ($ "parser.rs")
        ($
"pub mod lexer;
use lexer::Lexer;
use lexer::Literal;
use lexer::Position;
use std::collections::HashSet;
"           (syntax)
            (parser)
        )
    )
)
