(define (visitor-rs) ($
"
    public class SyntaxVisitor
    {"
    (apply-to-selected-children "syntax" (lambda (syntax) (if (abstract? syntax) "" ($
        (if (has-syntax-children? syntax)
            ($ "
        public virtual bool open"(id syntax)"("(id syntax)"Syntax "(string-firstchar-downcase (id syntax))"Syntax)
        {
            return true;
        }

        public virtual void close"(id syntax)"("(id syntax)"Syntax "(string-firstchar-downcase (id syntax))"Syntax)
        {
        }
"            )
            ($ "
        public virtual void visit"(id syntax)"("(id syntax)"Syntax "(string-firstchar-downcase (id syntax))"Syntax)
        {
        }
"            )
        )
    ))))
"    }
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
