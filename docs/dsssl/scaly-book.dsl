<!DOCTYPE style-sheet PUBLIC "-//James Clark//DTD DSSSL Style Sheet//EN">
<style-sheet>
<style-specification>
<style-specification-body>

;; The specification as a book on the web, set by the Scaly DSSSL engine:
;;
;;   dazzle -t sgml -d dsssl/scaly-book.dsl scaly-spec.xml   (docs/html.sh)
;;
;; writes one page per chapter and appendix, named by its id plus .htm (the addresses the
;; DocBook stylesheets gave them, which the tour and the site link to),
;; index.html, the title page with the contents, and searchindex.js, the text
;; of every section for the search. Every page has the contents in a sidebar,
;; a bar on top (contents, theme, search, the tour, the PDF) and the chapters
;; before and after it. The look is book.css, the behaviour book.js -- the
;; theme, the sidebar, the search, copying a listing and colouring Scaly in it
;; with the words of words.js, which ./mkp generates from scaly.sgm, and
;; running it in the page where runnable.js (docs/runnable.py) says it runs --,
;; theme.js sets the theme before the page is drawn. docs/html.sh puts them
;; beside the pages.

(declare-flow-object-class element
  "UNREGISTERED::James Clark//Flow Object Class::element")
(declare-flow-object-class empty-element
  "UNREGISTERED::James Clark//Flow Object Class::empty-element")
(declare-flow-object-class document-type
  "UNREGISTERED::James Clark//Flow Object Class::document-type")
(declare-flow-object-class entity
  "UNREGISTERED::James Clark//Flow Object Class::entity")
(declare-flow-object-class formatting-instruction
  "UNREGISTERED::James Clark//Flow Object Class::formatting-instruction")

(define %book-title% "The Scaly Programming Language")
(define %fonts%
  "https://fonts.googleapis.com/css2?family=DM+Sans:ital,opsz,wght@0,9..40,300;0,9..40,400;0,9..40,500;0,9..40,600;1,9..40,400&#38;family=JetBrains+Mono:wght@400;500&#38;display=swap")

;; ---------------------------------------------------------------- helpers

(define (node-list->items nl)
  (if (node-list-empty? nl)
      '()
      (cons (node-list-first nl) (node-list->items (node-list-rest nl)))))

(define (sosofo-map f items)
  (let loop ((rest items) (out (empty-sosofo)))
    (if (null? rest)
        out
        (loop (cdr rest) (sosofo-append out (f (car rest)))))))

(define (part? nd)
  (or (equal? (gi nd) "chapter") (equal? (gi nd) "appendix")))

(define (parts-of book)
  (node-list->items (node-list-filter part? (children book))))

(define (sections-of nd)
  (node-list->items (select-elements (children nd) "section")))

(define (part-of nd)
  (if (part? nd) nd (part-of (parent nd))))

(define (title-node nd)
  (select-elements (children nd) "title"))

(define (title-text nd)
  (let ((t (title-node nd)))
    (if (node-list-empty? t) (data nd) (data t))))

;; a chapter's number, an appendix's letter
(define (part-number part)
  (if (equal? (gi part) "chapter")
      (number->string (child-number part))
      (list-ref '("A" "B" "C" "D" "E" "F" "G" "H") (- (child-number part) 1))))

(define (part-label part)
  (if (equal? (gi part) "chapter")
      (string-append "Chapter " (part-number part))
      (string-append "Appendix " (part-number part))))

;; "3.2.1": the part's number and the section's place in each section above it
(define (section-number nd)
  (if (part? nd)
      (part-number nd)
      (string-append (section-number (parent nd)) "." (number->string (child-number nd)))))

(define (file-of part) (string-append (id part) ".htm"))

;; an element's anchor: its id, else its number (a few sections have none)
(define (anchor nd)
  (or (id nd) (string-append "s" (section-number nd))))

;; where a link from `from' to `target' goes: the target's page, its anchor
;; unless it is the page itself, no file where it is the same page
(define (href-to target from)
  (let ((part (part-of target)))
    (cond ((node-list=? target part) (file-of part))
          ((node-list=? part (part-of from)) (string-append "#" (anchor target)))
          (else (string-append (file-of part) "#" (anchor target))))))

;; ------------------------------------------------------------ the pages

(define (head-of title)
  (sosofo-append
    (make empty-element gi: "meta" attributes: '(("charset" "utf-8")))
    (make empty-element gi: "meta"
          attributes: '(("name" "viewport") ("content" "width=device-width, initial-scale=1")))
    (make element gi: "title" (literal title))
    (make empty-element gi: "meta" attributes: '(("name" "color-scheme") ("content" "light dark")))
    (make empty-element gi: "link" attributes: '(("rel" "preconnect") ("href" "https://fonts.googleapis.com")))
    (make empty-element gi: "link"
          attributes: '(("rel" "preconnect") ("href" "https://fonts.gstatic.com") ("crossorigin" "")))
    (make empty-element gi: "link" attributes: (list (list "rel" "stylesheet") (list "href" %fonts%)))
    (make empty-element gi: "link" attributes: '(("rel" "stylesheet") ("href" "book.css")))
    (make element gi: "script" attributes: '(("src" "theme.js")) (empty-sosofo))
    (make element gi: "script" attributes: '(("src" "words.js") ("defer" "defer")) (empty-sosofo))
    (make element gi: "script" attributes: '(("src" "searchindex.js") ("defer" "defer")) (empty-sosofo))
    (make element gi: "script" attributes: '(("src" "runnable.js") ("defer" "defer")) (empty-sosofo))
    (make element gi: "script" attributes: '(("src" "runnable.js") ("defer" "defer")) (empty-sosofo))
    (make element gi: "script" attributes: '(("src" "book.js") ("defer" "defer")) (empty-sosofo))))

(define (icon path)
  (make element gi: "svg"
        attributes: '(("viewBox" "0 0 24 24") ("width" "20") ("height" "20") ("aria-hidden" "true"))
    (make element gi: "path" attributes: (list (list "d" path)) (empty-sosofo))))

(define %icon-menu% "M3 6h18M3 12h18M3 18h18")
(define %icon-theme% "M12 3a9 9 0 1 0 9 9 7 7 0 0 1-9-9z")
(define %icon-search% "M10.5 3a7.5 7.5 0 1 0 4.6 13.4l4.7 4.7 1.4-1.4-4.7-4.7A7.5 7.5 0 0 0 10.5 3zm0 2a5.5 5.5 0 1 1 0 11 5.5 5.5 0 0 1 0-11z")

(define (button id label path)
  (make element gi: "button"
        attributes: (list (list "id" id) (list "class" "icon") (list "type" "button")
                          (list "aria-label" label) (list "title" label))
    (icon path)))

(define (topbar)
  (make element gi: "header" attributes: '(("class" "topbar"))
    (make element gi: "div" attributes: '(("class" "topbar-tools"))
      (button "toggle-sidebar" "Contents" %icon-menu%)
      (button "toggle-theme" "Theme" %icon-theme%)
      (button "toggle-search" "Search (S)" %icon-search%))
    (make element gi: "a" attributes: '(("class" "topbar-title") ("href" "index.html"))
      (literal %book-title%))
    (make element gi: "nav" attributes: '(("class" "topbar-links") ("aria-label" "Elsewhere"))
      (make element gi: "a" attributes: '(("href" "../play/tour.html")) (literal "Tour"))
      (make element gi: "a" attributes: '(("href" "scaly-spec.pdf")) (literal "PDF"))
      (make element gi: "a" attributes: '(("href" "https://github.com/rschleitzer/Scaly")) (literal "GitHub"))
      (make element gi: "a" attributes: '(("href" "../")) (literal "scaly.io")))))

(define (search-panel)
  (make element gi: "div" attributes: '(("class" "search") ("id" "search") ("hidden" "hidden"))
    (make empty-element gi: "input"
          attributes: '(("type" "search") ("id" "search-input") ("placeholder" "Search the book")
                        ("aria-label" "Search the book") ("autocomplete" "off") ("spellcheck" "false")))
    (make element gi: "p" attributes: '(("class" "search-summary") ("id" "search-summary")) (empty-sosofo))
    (make element gi: "ol" attributes: '(("class" "search-results") ("id" "search-results")) (empty-sosofo))))

;; the contents: every part, its sections below it (book.js opens the
;; current part's and lets the reader open the others)
(define (toc-section sec)
  (make element gi: "li"
    (make element gi: "a" attributes: (list (list "href" (string-append (file-of (part-of sec)) "#" (anchor sec))))
      (make element gi: "span" attributes: '(("class" "num")) (literal (section-number sec)))
      (literal (title-text sec)))))

(define (toc-part part current)
  (let ((here (and current (node-list=? part current)))
        (secs (sections-of part)))
    (make element gi: "li"
          attributes: (list (list "class" (if here "part current open" "part")))
      (make element gi: "div" attributes: '(("class" "part-row"))
        (make element gi: "a"
              attributes: (if here
                              (list (list "href" (file-of part)) (list "aria-current" "page"))
                              (list (list "href" (file-of part))))
          (make element gi: "span" attributes: '(("class" "num")) (literal (string-append (part-number part) ".")))
          (literal (title-text part)))
        (if (null? secs)
            (empty-sosofo)
            (make element gi: "button"
                  attributes: '(("class" "fold") ("type" "button") ("aria-label" "Show the sections"))
              (empty-sosofo))))
      (if (null? secs)
          (empty-sosofo)
          (make element gi: "ol" attributes: '(("class" "sections"))
            (sosofo-map toc-section secs))))))

(define (sidebar parts current)
  (make element gi: "nav" attributes: '(("class" "sidebar") ("id" "sidebar") ("aria-label" "Contents"))
    (make element gi: "a" attributes: '(("class" "sidebar-title") ("href" "index.html"))
      (literal %book-title%))
    (make element gi: "ol" attributes: '(("class" "toc"))
      (sosofo-map (lambda (p) (toc-part p current)) parts))))

(define (pager-link part rel)
  (if part
      (make element gi: "a"
            attributes: (list (list "class" rel) (list "rel" rel) (list "href" (file-of part)))
        (make element gi: "span" attributes: '(("class" "pager-label"))
          (literal (if (equal? rel "prev") "Previous" "Next")))
        (make element gi: "span" attributes: '(("class" "pager-title"))
          (literal (title-text part))))
      (make element gi: "span" attributes: (list (list "class" rel)) (empty-sosofo))))

(define (page file title parts current prev next body)
  (make entity system-id: file
    (make document-type name: "html")
    (make element gi: "html" attributes: '(("lang" "en"))
      (make element gi: "head" (head-of title))
      (make element gi: "body"
        (sidebar parts current)
        (make element gi: "div" attributes: '(("class" "scrim") ("id" "scrim")) (empty-sosofo))
        (make element gi: "div" attributes: '(("class" "page"))
          (topbar)
          (search-panel)
          (make element gi: "main" attributes: '(("class" "content") ("id" "content"))
            (make element gi: "article" body)
            (make element gi: "nav" attributes: '(("class" "pager") ("aria-label" "Chapters"))
              (pager-link prev "prev")
              (pager-link next "next"))))))))

(define (part-page part parts prev next)
  (page (file-of part)
        (string-append (title-text part) " - " %book-title%)
        parts part prev next
        (sosofo-append
          (make element gi: "p" attributes: '(("class" "part-label")) (literal (part-label part)))
          (make element gi: "h1" attributes: (list (list "id" (id part)))
            (process-node-list (children (title-node part))))
          (process-node-list
            (node-list-filter (lambda (n) (not (equal? (gi n) "title"))) (children part))))))

(define (pages parts)
  (let loop ((rest parts) (prev #f) (out (empty-sosofo)))
    (if (null? rest)
        out
        (loop (cdr rest) (car rest)
              (sosofo-append out
                (part-page (car rest) parts prev (if (null? (cdr rest)) #f (car (cdr rest)))))))))

(define (index-page book parts)
  (page "index.html" %book-title% parts #f #f (car parts)
    (sosofo-append
      (make element gi: "div" attributes: '(("class" "title-page"))
        (make element gi: "h1" (literal %book-title%))
        (make element gi: "p" attributes: '(("class" "lead"))
          (literal "The reference of the language: what a program says and what it means, from a first
program to the grammar."))
        (make element gi: "p" attributes: '(("class" "actions"))
          (make element gi: "a" attributes: (list (list "class" "button primary") (list "href" (file-of (car parts))))
            (literal "Start reading"))
          (make element gi: "a" attributes: '(("class" "button") ("href" "../play/tour.html"))
            (literal "Take the tour in the browser"))
          (make element gi: "a" attributes: '(("class" "button") ("href" "scaly-spec.pdf"))
            (literal "The PDF"))))
      (make element gi: "h2" attributes: '(("id" "contents")) (literal "Contents"))
      (make element gi: "ol" attributes: '(("class" "contents"))
        (sosofo-map
          (lambda (part)
            (make element gi: "li"
              (make element gi: "a" attributes: (list (list "href" (file-of part)))
                (make element gi: "span" attributes: '(("class" "num")) (literal (part-number part)))
                (literal (title-text part)))
              (let ((secs (sections-of part)))
                (if (null? secs)
                    (empty-sosofo)
                    (make element gi: "ol" (sosofo-map toc-section secs))))))
          parts)))))

;; ------------------------------------------------------------ the search
;; searchindex.js: one entry per part and section -- page, anchor, number,
;; title and the text of the element without its sections -- as a JavaScript
;; array literal

(define (js-chars chars space out)
  (cond ((null? chars) (reverse out))
        ((or (char=? (car chars) #\space)
             (char=? (car chars) #\line-feed) (char=? (car chars) #\carriage-return))
         (js-chars (cdr chars) #t out))
        (else
         (let ((out (if (and space (not (null? out))) (cons #\space out) out))
               (c (car chars)))
           (js-chars (cdr chars) #f
                     (cond ((char=? c #\") (cons #\" (cons #\\ out)))
                           ((char=? c #\\) (cons #\\ (cons #\\ out)))
                           (else (cons c out))))))))

(define (js-string s)
  (string-append "\"" (list->string (js-chars (string->list s) #f '())) "\""))

;; the text of an element without its title and its sections, a space
;; between its children (a paragraph does not run into the next)
(define (own-text nd)
  (apply string-append
         (map (lambda (n) (string-append (data n) " "))
              (node-list->items
                (node-list-filter
                  (lambda (n) (not (or (equal? (gi n) "section") (equal? (gi n) "title"))))
                  (children nd))))))

(define (search-entry nd)
  (let ((part (part-of nd)))
    (string-append
      "[" (js-string (file-of part))
      "," (js-string (if (node-list=? nd part) "" (anchor nd)))
      "," (js-string (section-number nd))
      "," (js-string (title-text nd))
      "," (js-string (own-text nd))
      "],
")))

(define (search-entries nd)
  (apply string-append
         (cons (search-entry nd)
               (map search-entries (sections-of nd)))))

(define (search-index parts)
  (make entity system-id: "searchindex.js"
    (make formatting-instruction
      data: (string-append
              "// searchindex.js - written by docs/dsssl/scaly-book.dsl: page, anchor, number,
// title and text of every part and section of the book
window.SEARCH_INDEX = [
"
              (apply string-append (map search-entries parts))
              "];
"))))

;; ------------------------------------------------------------ the rules

(root (process-children))

(element book
  (let ((parts (parts-of (current-node))))
    (sosofo-append
      (index-page (current-node) parts)
      (pages parts)
      (search-index parts))))

(element title (empty-sosofo))

(define (heading-level nd)
  (if (part? (parent nd)) 2 (+ 1 (heading-level (parent nd)))))

(element section
  (let ((level (min 6 (heading-level (current-node)))))
    (make element gi: "section" attributes: (list (list "id" (anchor (current-node))))
      (make element gi: (string-append "h" (number->string level))
        (make element gi: "a" attributes: (list (list "class" "header") (list "href" (string-append "#" (anchor (current-node)))))
          (process-node-list (children (title-node (current-node))))))
      (process-children))))

(element para (make element gi: "p" (process-children)))

(element programlisting
  (make element gi: "pre"
        attributes: (list (list "class" (if (equal? (attribute-string "role") "fragment")
                                            "listing fragment"
                                            "listing")))
    (make element gi: "code" (process-children))))

(element screen
  (make element gi: "pre" attributes: '(("class" "screen"))
    (make element gi: "code" (process-children))))

(define (inline-code class)
  (make element gi: "code" attributes: (list (list "class" class)) (process-children)))

(element literal (make element gi: "code" (process-children)))
(element code (make element gi: "code" (process-children)))
(element filename (inline-code "filename"))
(element command (inline-code "command"))
(element userinput (inline-code "userinput"))
(element envar (inline-code "envar"))
(element option (inline-code "option"))
(element computeroutput (inline-code "computeroutput"))
(element errortext (inline-code "errortext"))
(element emphasis
  (if (equal? (attribute-string "role") "bold")
      (make element gi: "strong" (process-children))
      (make element gi: "em" (process-children))))
(element quote (make element gi: "q" (process-children)))
(element keycap (make element gi: "kbd" (process-children)))
(element guilabel (make element gi: "span" attributes: '(("class" "gui")) (process-children)))
(element guimenu (make element gi: "span" attributes: '(("class" "gui")) (process-children)))
(element guimenuitem (make element gi: "span" attributes: '(("class" "gui")) (process-children)))
(element guibutton (make element gi: "span" attributes: '(("class" "gui")) (process-children)))
(element application (make element gi: "span" attributes: '(("class" "application")) (process-children)))

(element itemizedlist (make element gi: "ul" (process-children)))
(element orderedlist (make element gi: "ol" (process-children)))
(element listitem
  (if (equal? (gi (parent (current-node))) "varlistentry")
      (make element gi: "dd" (process-children))
      (make element gi: "li" (process-children))))
(element variablelist (make element gi: "dl" (process-children)))
(element varlistentry (process-children))
(element term (make element gi: "dt" (process-children)))

(element informaltable
  (make element gi: "div" attributes: '(("class" "table-wrap"))
    (make element gi: "table" (process-children))))
(element tgroup (process-children))
(element colspec (empty-sosofo))
(element thead (make element gi: "thead" (process-children)))
(element tbody (make element gi: "tbody" (process-children)))
(element row (make element gi: "tr" (process-children)))
(element entry
  (if (equal? (gi (parent (parent (current-node)))) "thead")
      (make element gi: "th" (process-children))
      (make element gi: "td" (process-children))))

(element note
  (make element gi: "aside" attributes: '(("class" "note"))
    (make element gi: "p" attributes: '(("class" "note-title")) (literal "Note"))
    (process-children)))

(element example
  (make element gi: "figure" attributes: (if (id) (list (list "class" "example") (list "id" (id))) '(("class" "example")))
    (make element gi: "figcaption" (process-node-list (children (title-node (current-node)))))
    (process-children)))

(element productionset
  (make element gi: "div" attributes: '(("class" "grammar")) (process-children)))
(element production
  (make element gi: "div" attributes: (list (list "class" "production") (list "id" (id)))
    (process-children)))
(element lhs
  (sosofo-append
    (make element gi: "a" attributes: (list (list "class" "lhs") (list "href" (string-append "#" (id (parent (current-node))))))
      (process-children))
    (make element gi: "span" attributes: '(("class" "defines")) (literal "::="))))
(element rhs (make element gi: "span" attributes: '(("class" "rhs")) (process-children)))
(element nonterminal
  (let ((target (element-with-id (attribute-string "def"))))
    (if (node-list-empty? target)
        (make element gi: "span" attributes: '(("class" "nonterminal")) (process-children))
        (make element gi: "a" attributes: (list (list "class" "nonterminal") (list "href" (href-to target (current-node))))
          (process-children)))))

(element xref
  (let ((target (element-with-id (attribute-string "linkend"))))
    (make element gi: "a" attributes: (list (list "href" (href-to target (current-node))))
      (literal (title-text target)))))

</style-specification-body>
</style-specification>
</style-sheet>
