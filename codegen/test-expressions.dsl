<!DOCTYPE STYLE-SHEET PUBLIC "-//James Clark//DTD DSSSL Style Sheet//EN" [
<!ENTITY testjs    SYSTEM "testjs.scm">
<!ENTITY testdoc   SYSTEM "testdoc.scm">
<!ENTITY helpers   SYSTEM "helpers.scm">
<!ENTITY fodeclare SYSTEM "fodeclare.scm">
]>

<STYLE-SHEET>
<STYLE-SPECIFICATION>

&fodeclare;
&helpers;
&testjs;
&testdoc;

<![CDATA[
(element suite
    (sosofo-append
        (file "packages/scaly-lang/tests/generated-expressions.test.ts"
            (generate-testjs)
        )
        (file "docs/scaly/generated-expressions.xml"
            (generate-testdoc)
        )
    )
)
]]>

</STYLE-SPECIFICATION>
</STYLE-SHEET>
