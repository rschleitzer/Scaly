# Kette 15: jsnums Schreibpuffer -- `out: pointer[char]` mit der Kapazitaet des Aufrufers als `Slice[char]`
J = 'packages/tscaly/0.1.0/tscaly/jsnum.scaly'
C = 'packages/tscaly/0.1.0/tscaly/checker.scaly'
L = 'packages/tscaly/0.1.0/tscaly/LitCheck.scaly'
SPEC = dict(
    root='packages/tscaly/0.1.0',
    params=[
        (J, 'ftoa_fmt_e', 967, 'out', None),
        (J, 'ftoa_fmt_f', 1043, 'out', None),
        (J, 'ftoa_shortest', 1105, 'out', None),
        (J, 'jsnum_put', 1141, 'out', None),
        (J, 'jsnum_format_uint', 1644, 'out', None),
        (J, 'jsnum_to_string', 1674, 'out', None),
        (J, 'jsnum_parse_pseudo_big_int', 1763, 'out', None),
        (C, 'pseudo_big_int_digits', 34467, 'out', None),
        (C, 'get_property_name_from_type', 12652, 'buf', None),
        (C, 'get_property_name_from_index', 12715, 'buf', None),
        (C, 'get_destructuring_property_name', 14231, 'buf', None),
        (C, 'get_property_name_for_known_symbol_name', 45302, 'buf', None),
        (L, 'write_bigint', 123, 'dec', None),
    ],
    fields=[],
    accessors=[
        ('AstNode.identifier_text_of', 'AstNode.identifier_text_length_of', None),
        ('AstNode.literal_text_of', 'AstNode.literal_text_length_of', None),
        ('AstNode.property_name_text_of', 'AstNode.property_name_text_length_of', None),
        ('Symbol.name_of', 'Symbol.name_len_of', None),
        ('SymbolTable.entry_name_at', 'SymbolTable.entry_name_len_at', None),
        ('Binder.classifiable_name_at', 'Binder.classifiable_name_len_at', None),
        ('Checker.member_name_at', 'Checker.member_name_len_at', None),
        ('Checker.literal_text_of', 'Checker.literal_text_length_of', None),
        ('Checker.union_key_property', 'Checker.union_key_property_len', None),
        ('Scanner.token_value', 'Scanner.token_value_length', None),
        ('Checker.copy_name_to_page', None, None),
    ],
    slice_fields=['text', 'token_value', 'file_name', 'key_property', 'local_jsx_namespace', 'local_jsx_fragment_namespace'],
    method_concepts=['Scanner', 'Checker'],
    ambiguous_fields=['name', 'parameter_name'],
    records={}, retlen=[], retlen_int=[], outpair=[],
)
