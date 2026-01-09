<![CDATA[
;; Rules for generating Scaly parser from scaly.sgm

(element grammar
    (file "parser.scaly"
        ($
            (generate-syntax-scaly)
            (generate-parser-scaly)
        )
    )
)
]]>
