(define (visitor concrete) ($
    (if concrete "" ($
        (apply-to-selected-children "syntax" (lambda (syntax-node) ($
"class "(id syntax-node)"
"
        )))
"
"   ))

"class "(if concrete "MyVisitor extends " "")"SyntaxVisitor {"
    (apply-to-selected-children "syntax" (lambda (syntax) (if (abstract? syntax) "" ($
        (if (has-syntax-children? syntax)
            ($ "
    function open"(id syntax)"("(string-firstchar-downcase (id syntax))": "(id syntax)"): bool"
                (if concrete
                    " {
        true
    }
"
                    ""
                )

"
    function close"(id syntax)"("(string-firstchar-downcase (id syntax))": "(id syntax)")"
                (if concrete
                    " {
    }
"
                    ""
                )
            )
            ($ "
    function visit"(id syntax)"("(string-firstchar-downcase (id syntax))": "(id syntax)")"
                (if concrete " {
    }
"                  ""
                )
            )
        )
    ))))
"}"
))
