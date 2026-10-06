<!DOCTYPE style-sheet PUBLIC "-//James Clark//DTD DSSSL Style Sheet//EN" [
<!ENTITY docbook.dsl SYSTEM "../dsssl-stylesheets/html/docbook.dsl" CDATA DSSSL>
]>
<style-sheet>
<style-specification use="docbook">
<style-specification-body>

;; The specification's sources name their elements with xml:id (DocBook 5);
;; the stylesheets ask for an attribute called "id" and number what has none
;; (AEN2550), and such a number moves whenever a section is added before it.
;; (id) answers the element's ID attribute under whatever name the DTD
;; declares it, so an anchor is the name its source gives it and a link to
;; it stays good.
(define (element-id #!optional (nd (current-node)))
  (let ((elem (if (equal? (gi nd) (normalize "title"))
                  (parent nd)
                  nd)))
    (if (id elem)
        (id elem)
        (generate-anchor elem))))

</style-specification-body>
</style-specification>
<external-specification id="docbook" document="docbook.dsl">
</style-sheet>
