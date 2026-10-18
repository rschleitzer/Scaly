<!DOCTYPE STYLE-SHEET PUBLIC "-//James Clark//DTD DSSSL Style Sheet//EN" [
<!ENTITY testhelpers SYSTEM "testhelpers.scm">
<!ENTITY testscaly SYSTEM "testscaly.scm">
<!ENTITY testdoc   SYSTEM "testdoc.scm">
<!ENTITY helpers   SYSTEM "helpers.scm">
<!ENTITY fodeclare SYSTEM "fodeclare.scm">
]>

<STYLE-SHEET>
<STYLE-SPECIFICATION>

&fodeclare;
&helpers;
&testhelpers;
&testscaly;
&testdoc;

<![CDATA[
(element suite
    (sosofo-append
        ; the literate suite runs through the self-hosted in-process JIT
        ; (tests/selfhosted/run.sh, generate-selfhosted-tests below)
        (file "docs/scaly/generated-controlflow.xml"
            (generate-testdoc)
        )
        (generate-selfhosted-tests "controlflow")
    )
)
]]>

</STYLE-SPECIFICATION>
</STYLE-SHEET>
