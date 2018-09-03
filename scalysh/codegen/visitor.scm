(define (visitor) ($
"
    public class SyntaxVisitor
    {"
    (apply-to-selected-children "syntax" (lambda (syntax) (if (abstract? syntax) "" ($
        (if (has-syntax-children? syntax)
            ($ "
        public virtual bool open"(id syntax)"("(id syntax)"Syntax the"(id syntax)")
        {
            return true;
        }

        public virtual void close"(id syntax)"("(id syntax)"Syntax the"(id syntax)")
        {
        }
"            )
            ($ "
        public virtual void visit"(id syntax)"("(id syntax)"Syntax the"(id syntax)")
        {
        }
"            )
        )
    ))))
"}
"
))
