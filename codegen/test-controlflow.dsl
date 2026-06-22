<!DOCTYPE STYLE-SHEET PUBLIC "-//James Clark//DTD DSSSL Style Sheet//EN" [
<!ENTITY testcpp   SYSTEM "testcpp.scm">
<!ENTITY testscaly SYSTEM "testscaly.scm">
<!ENTITY testdoc   SYSTEM "testdoc.scm">
<!ENTITY helpers   SYSTEM "helpers.scm">
<!ENTITY fodeclare SYSTEM "fodeclare.scm">
]>

<STYLE-SHEET>
<STYLE-SPECIFICATION>

&fodeclare;
&helpers;
&testcpp;
&testscaly;
&testdoc;

<![CDATA[
(element suite
    (sosofo-append
        ; C++ ControlFlowTests.{h,cpp} retired from --test (s198): the literate
        ; suite now runs through the self-hosted in-process JIT via
        ; tests/selfhosted/run.sh (generate-selfhosted-tests below).
        (file "docs/scaly/generated-controlflow.xml"
            (generate-testdoc)
        )
        (generate-selfhosted-tests "controlflow")
    )
)
]]>

</STYLE-SPECIFICATION>
</STYLE-SHEET>
