(define (visitor-rs) ($
"
pub trait SyntaxVisitor {"
    (apply-to-selected-children "syntax" (lambda (syntax) (if (abstract? syntax) "" ($
        (if (has-syntax-children? syntax)
            ($ "
    fn open"(id syntax)"(&""mut self, "(string-firstchar-downcase (id syntax))"Syntax: Ref<"(id syntax)"Syntax>) -> bool;
    fn close"(id syntax)"(&""mut self, "(string-firstchar-downcase (id syntax))"Syntax: Ref<"(id syntax)"Syntax>);
"            )
            ($ "
    fn visit"(id syntax)"(&""mut self, "(string-firstchar-downcase (id syntax))"Syntax: Ref<"(id syntax)"Syntax>);
"            )
        )
    ))))
"}
"
))

(define (visitor) ($
"
class Visitor {"
    (apply-to-selected-children "syntax" (lambda (syntax) (if (abstract? syntax) "" ($
        (if (has-syntax-children? syntax)
            ($ "
    function open"(id syntax)"("(string-firstchar-downcase (id syntax))"Syntax: "(id syntax)"Syntax): bool {
        true
    }

    function close"(id syntax)"("(string-firstchar-downcase (id syntax))"Syntax: "(id syntax)"Syntax) {
    }
"            )
            ($ "
    function visit"(id syntax)"("(string-firstchar-downcase (id syntax))"Syntax: "(id syntax)"Syntax) {
    }
"            )
        )
    ))))
"}
"
))
