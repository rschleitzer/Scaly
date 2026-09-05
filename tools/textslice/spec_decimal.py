# Kette 17: die Dezimal-Schreiber -- `write_decimal(out, v)` / `render_decimal(out, value)` als `Slice[char]`
P = 'packages/tscaly/0.1.0/tscaly/parser.scaly'
C = 'packages/tscaly/0.1.0/tscaly/checker.scaly'
SPEC = dict(
    root='packages/tscaly/0.1.0',
    views=[
        (P, 'let out host.allocate(len + 24, 1) as pointer[char]', 'let out Slice[char]((len + 24) as size_t, host.allocate(len + 24, 1) as pointer[char])'),
    ],
    params=[
        (P, 'write_decimal', 18008, 'out', None),
        (C, 'render_decimal', 99923, 'out', None),
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
    slice_fields=['text', 'token_value', 'file_name', 'key_property', 'local_jsx_namespace', 'local_jsx_fragment_namespace', 'buffer', 'value_buffer'],
    this_slice_fields=['buffer'],
    method_concepts=['Scanner', 'Checker'],
    ambiguous_fields=['name', 'parameter_name'],
    records={}, retlen=[], retlen_int=[], outpair=[],
)
