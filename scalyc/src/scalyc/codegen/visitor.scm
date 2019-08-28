(define (visitor-rs) ($
"
pub trait SyntaxVisitor {"
    (apply-to-selected-children "syntax" (lambda (syntax) (if (abstract? syntax) "" ($
        (if (has-syntax-children? syntax)
            ($ "
    fn open_"(downcase-string (id syntax))"(&""mut self, "(downcase-string (id syntax))"_syntax: Ref<"(id syntax)"Syntax>) -> bool;
    fn close_"(downcase-string (id syntax))"(&""mut self, "(downcase-string (id syntax))"_syntax: Ref<"(id syntax)"Syntax>);
"            )
            ($ "
    fn visit_"(downcase-string (id syntax))"(&""mut self, "(downcase-string (id syntax))"_syntax: Ref<"(id syntax)"Syntax>);
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
    function open_"(downcase-string (id syntax))"("(downcase-string (id syntax))"Syntax: "(id syntax)"Syntax): bool {
        true
    }

    function close_"(downcase-string (id syntax))"("(downcase-string (id syntax))"Syntax: "(id syntax)"Syntax) {
    }
"            )
            ($ "
    function visit_"(downcase-string (id syntax))"("(downcase-string (id syntax))"Syntax: "(id syntax)"Syntax) {
    }
"            )
        )
    ))))
"}
"
))
