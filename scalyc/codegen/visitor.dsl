(define (visitor) ($
"class Visitor {"
    (apply-to-selected-children "syntax" (lambda (syntax) (if (abstract? syntax) "" ($
        (if (has-syntax-children? syntax)
            ($ "
    function open"(id syntax)"("(string-firstchar-downcase (id syntax))": "(id syntax)"): bool {
        true
    }

    function close"(id syntax)"("(string-firstchar-downcase (id syntax))": "(id syntax)") {
    }
"
            )
            ($ "
    function visit"(id syntax)"("(string-firstchar-downcase (id syntax))": "(id syntax)") {
    }
"            )
        )
    ))))
"}

"
))
