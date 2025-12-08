(element grammar
    (sosofo-append
        (file "packages/scaly-lang/src/syntax.ts"
            (generate-syntax)
        )
        (file "packages/scaly-lang/src/parser.ts"
            (parser)
        )
        (file "grammar.xml"
            (generate-grammar-doc)
        )
    )
)
