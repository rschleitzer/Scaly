(element language
    (make sequence

        (create-file ($ "Syntax.scaly" generated-extension)
            (output (syntax)))

        (create-file ($ "Visitor.scaly" generated-extension)
            (output (visitor #f)))

        (create-file ($ "MyVisitor.scaly" generated-extension)
            (output (visitor #t)))

        (create-file ($ "Parser.scaly" generated-extension)
            (output (parser)))
))
