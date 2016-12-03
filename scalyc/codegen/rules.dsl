(element language
    (make sequence

        (create-file ($ "MyVisitor.scaly" generated-extension)
            (output (visitor #t)))

        (create-file ($ "Parser.scaly" generated-extension)
            (output ($ (visitor #f)(syntax)(parser))))
))
