<!DOCTYPE STYLE-SHEET PUBLIC "-//James Clark//DTD DSSSL Style Sheet//EN" [
<!ENTITY helpers SYSTEM "helpers.scm">
]>

<STYLE-SHEET>
<STYLE-SPECIFICATION>

&helpers;

;; Test extraction stylesheet
;; Transforms test.dtd documents into JSON test cases

(root
  (make sequence
    (literal "[")
    (process-children)
    (literal "]")))

(element category
  (make sequence
    (process-children)))

(element tests
  (make sequence
    (process-children)))

(element test
  (make sequence
    (if (not (absolute-first-sibling?))
        (literal ",\n")
        (empty-sosofo))
    (literal "{")
    (literal "\"input\":")
    (process-matching-children 'input)
    (literal ",\"expect\":")
    (process-matching-children 'expect)
    (literal "}")))

(element input
  (make sequence
    (literal (string-append "\"" (escape-json (data (current-node))) "\""))))

(element expect
  (make sequence
    (literal (string-append "\"" (escape-json (data (current-node))) "\""))))

(element description
  (empty-sosofo))

;; Helper to escape JSON strings
(define (escape-json str)
  (let loop ((chars (string->list str)) (result '()))
    (if (null? chars)
        (list->string (reverse result))
        (let ((c (car chars)))
          (cond
            ((char=? c #\\) (loop (cdr chars) (cons #\\ (cons #\\ result))))
            ((char=? c #\") (loop (cdr chars) (cons #\" (cons #\\ result))))
            ((char=? c #\newline) (loop (cdr chars) (cons #\n (cons #\\ result))))
            ((char=? c #\return) (loop (cdr chars) (cons #\r (cons #\\ result))))
            ((char=? c #\tab) (loop (cdr chars) (cons #\t (cons #\\ result))))
            (else (loop (cdr chars) (cons c result))))))))

</STYLE-SPECIFICATION>
</STYLE-SHEET>
