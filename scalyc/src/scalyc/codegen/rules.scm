(element grammar
    (sosofo-append
        (file ($ "parser.rs")
            ($ 
                (parser-rs)
                (visitor-rs)
                (syntax-rs))
        )
        (file ($ "parser.scaly")
            ($ 
                (parser)
                (visitor)
                (syntax))
        )
    )
)
