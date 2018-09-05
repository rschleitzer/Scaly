(element grammar
    (sosofo-append
        (file ($ "Parser.cs")
            ($ 
                (parser-cs)
                (visitor-cs)
                (syntax-cs))
        )
        (file ($ "Parser.scaly")
            ($ 
                (parser)
                (visitor)
                (syntax))
        )
    )
)
