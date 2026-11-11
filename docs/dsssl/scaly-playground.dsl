<!DOCTYPE style-sheet PUBLIC "-//James Clark//DTD DSSSL Style Sheet//EN">
<style-sheet>
<style-specification>
<style-specification-body>

;; The Scaly Playground's own chapters (web/repl/playground.html), set by the
;; Scaly DSSSL engine from scaly-playground.xml -- a document of its own, not
;; part of the specification; the playground's first chapter is the book's
;; tour (scaly-tour.dsl), taken as it is:
;;
;;   dazzle -t sgml -d dsssl/scaly-playground.dsl scaly-playground.xml
;;
;; (web/repl/build.sh). Each chapter's sections become steps as the tour's
;; do, each step marked with its chapter's title (data-chapter) and id
;; (data-chapter-id). The same chapters in German (scaly-playground.de.xml,
;; xml:lang="de", the book's tour translated as their first chapter) write
;; playground-steps.de.htm.
;; writes playground-steps.htm, as scaly-tour.dsl writes tour-steps.htm: one section element of class "step" per program of
;; the section, with its title, its prose -- every paragraph and fragment
;; since the program before, and after the last program of a subsection what
;; follows it --, and the program in a pre element of class "program". A listing marked
;; role="fragment" (part of a program, two files, a package line) is prose;
;; one marked role="gpu" is a program for the graphics card too (class
;; "program gpu": the page runs it with --gpu and lets the reader choose).
;; A paragraph marked role="spec-only" -- the pointer to this very page -- is
;; left out. A subsection without a program is one step of prose alone.
;; tests/tour/run.sh runs every program.

(declare-flow-object-class element
  "UNREGISTERED::James Clark//Flow Object Class::element")
(declare-flow-object-class entity
  "UNREGISTERED::James Clark//Flow Object Class::entity")

(define (node-list->items nl)
  (if (node-list-empty? nl)
      '()
      (cons (node-list-first nl) (node-list->items (node-list-rest nl)))))

(define (program? nd)
  (and (equal? (gi nd) "programlisting")
       (not (equal? (attribute-string "role" nd) "fragment"))))

(define (content? nd)
  (or (and (equal? (gi nd) "para")
           (not (equal? (attribute-string "role" nd) "spec-only")))
      (equal? (gi nd) "programlisting")))

;; the section's paragraphs and listings, grouped: a list of steps, each
;; (prose . program) with program #f for a step of prose alone
(define (group items)
  (let loop ((rest items) (prose '()) (steps '()))
    (cond ((null? rest)
           (cond ((null? prose) (reverse steps))
                 ((null? steps) (list (cons (reverse prose) #f)))
                 (else
                  (let ((last (car steps)))
                    (reverse (cons (cons (append (car last) (reverse prose)) (cdr last))
                                   (cdr steps)))))))
          ((program? (car rest))
           (loop (cdr rest) '() (cons (cons (reverse prose) (car rest)) steps)))
          (else (loop (cdr rest) (cons (car rest) prose) steps)))))

(define (section-title sec)
  (data (select-elements (children sec) "title")))

(define (chapter-of nd)
  (if (equal? (gi nd) "chapter") nd (chapter-of (parent nd))))

(define (chapter-title nd)
  (data (select-elements (children (chapter-of nd)) "title")))

(define (make-step sec step)
  (make element gi: "section"
        attributes: (list (list "class" "step") (list "data-section" (id sec))
                          (list "data-chapter" (chapter-title sec))
                          (list "data-chapter-id" (id (chapter-of sec))))
    (make element gi: "h2" (literal (section-title sec)))
    (make element gi: "div" attributes: '(("class" "prose"))
      (let loop ((nodes (car step)) (out (empty-sosofo)))
        (if (null? nodes)
            out
            (loop (cdr nodes) (sosofo-append out (process-node-list (car nodes)))))))
    (if (cdr step)
        (make element gi: "pre"
              attributes: (list (list "class"
                                      (if (equal? (attribute-string "role" (cdr step)) "gpu")
                                          "program gpu"
                                          "program")))
          (process-node-list (children (cdr step))))
        (empty-sosofo))))

(define (steps-of sec)
  (let loop ((steps (group (node-list->items
                            (node-list-filter content? (children sec)))))
             (out (empty-sosofo)))
    (if (null? steps)
        out
        (loop (cdr steps) (sosofo-append out (make-step sec (car steps)))))))

;; the book in English writes playground-steps.htm, one in another language
;; (xml:lang="de") playground-steps.LANG.htm
(element book
  (let ((lang (attribute-string "xml:lang")))
    (make entity system-id: (if (and lang (not (equal? lang "en")))
                                (string-append "playground-steps." lang ".htm")
                                "playground-steps.htm")
      (make element gi: "div" attributes: '(("class" "tour"))
        (process-node-list (select-elements (children (current-node)) "chapter"))))))
(element chapter (process-node-list (select-elements (children (current-node)) "section")))
(element section (steps-of (current-node)))
(element para (make element gi: "p" (process-children)))
(element programlisting
  (make element gi: "pre" attributes: '(("class" "fragment")) (process-children)))
(element code (make element gi: "code" (process-children)))
(element filename (make element gi: "code" (process-children)))
(element command (make element gi: "code" (process-children)))
(element userinput (make element gi: "code" (process-children)))
(element envar (make element gi: "code" (process-children)))
(element emphasis (make element gi: "em" (process-children)))
(element link
  (make element gi: "a" attributes: (list (list "href" (attribute-string "xlink:href")))
    (process-children)))

</style-specification-body>
</style-specification>
</style-sheet>
