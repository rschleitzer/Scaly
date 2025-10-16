(element grammar
    (file ($ (let ((grammar-name (name (current-node)))) ($ (if grammar-name grammar-name "") "Parser.scaly")))
        ($
            (generate-syntax)
            (parser)
        )
    )
)
