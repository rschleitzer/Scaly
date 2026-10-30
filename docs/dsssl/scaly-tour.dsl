<!DOCTYPE style-sheet PUBLIC "-//James Clark//DTD DSSSL Style Sheet//EN">
<style-sheet>
<style-specification>
<style-specification-body>

;; The tour's steps (web/repl/tour.html), set by the Scaly DSSSL engine from
;; the tutorial's "A Tour of Scaly" section (tutorial.xml, tut-tour) -- the
;; same text as the specification's, not a second one to keep in step:
;;
;;   dazzle -t sgml -d dsssl/scaly-tour.dsl scaly-spec.xml
;;
;; (web/repl/build.sh, and docs/deploy.sh, which publishes the steps anew
;; when they changed).
;; writes tour-steps.htm: one section element of class "step" per program of
;; the section, with its title, its prose -- every paragraph and fragment
;; since the program before, and after the last program of a subsection what
;; follows it --, and the program in a pre element of class "program". A listing marked
;; role="fragment" (part of a program, two files, a package line) is prose.
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

(define (make-step sec step)
  (make element gi: "section"
        attributes: (list (list "class" "step") (list "data-section" (id sec)))
    (make element gi: "h2" (literal (section-title sec)))
    (make element gi: "div" attributes: '(("class" "prose"))
      (let loop ((nodes (car step)) (out (empty-sosofo)))
        (if (null? nodes)
            out
            (loop (cdr nodes) (sosofo-append out (process-node-list (car nodes)))))))
    (if (cdr step)
        (make element gi: "pre" attributes: '(("class" "program"))
          (process-node-list (children (cdr step))))
        (empty-sosofo))))

(define (steps-of sec)
  (let loop ((steps (group (node-list->items
                            (node-list-filter content? (children sec)))))
             (out (empty-sosofo)))
    (if (null? steps)
        out
        (loop (cdr steps) (sosofo-append out (make-step sec (car steps)))))))

(root
  (make entity system-id: "tour-steps.htm"
    (make element gi: "div" attributes: '(("class" "tour"))
      (let ((tour (element-with-id "tut-tour")))
        (sosofo-append
          (steps-of tour)
          (process-node-list (select-elements (children tour) "section")))))))

(element section (steps-of (current-node)))
(element para (make element gi: "p" (process-children)))
(element programlisting
  (make element gi: "pre" attributes: '(("class" "fragment")) (process-children)))
(element code (make element gi: "code" (process-children)))
(element filename (make element gi: "code" (process-children)))
(element command (make element gi: "code" (process-children)))
(element userinput (make element gi: "code" (process-children)))
(element envar (make element gi: "code" (process-children)))
(element computeroutput (make element gi: "code" (process-children)))
(element emphasis (make element gi: "em" (process-children)))

;; a link into the specification: its chapter's page on scaly.io, the target
;; by its id, named by its title
(define (chapter-of nd)
  (if (or (equal? (gi nd) "chapter") (equal? (gi nd) "appendix"))
      nd
      (chapter-of (parent nd))))

(element xref
  (let* ((target (element-with-id (attribute-string "linkend")))
         (chapter (chapter-of target))
         (href (if (node-list=? target chapter)
                   (string-append "https://scaly.io/lang/" (id chapter) ".htm")
                   (string-append "https://scaly.io/lang/" (id chapter) ".htm#" (id target)))))
    (make element gi: "a" attributes: (list (list "href" href))
      (literal (section-title target)))))

</style-specification-body>
</style-specification>
</style-sheet>
