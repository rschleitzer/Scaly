# Kette 16: die Bereichspuffer -- Puffer mit expliziten Grenzen (start/stop, from/limit) als `Slice[char]`
A = 'packages/tscaly/0.1.0/tscaly/ArithCheck.scaly'
L = 'packages/tscaly/0.1.0/tscaly/LitCheck.scaly'
Sc = 'packages/tscaly/0.1.0/tscaly/scanner.scaly'
T = 'packages/tscaly/0.1.0/tscaly/tspath.scaly'
SPEC = dict(
    root='packages/tscaly/0.1.0',
    views=[
        (A, 'let d text.get_buffer() as pointer[char]', 'let d Slice[char](text.get_length(), text.get_buffer() as pointer[char])'),
        (L, 'let d text.get_buffer() as pointer[char]', 'let d Slice[char](text.get_length(), text.get_buffer() as pointer[char])'),
        (L, 'let raw scratch.get_buffer()', 'let raw Slice[char]((n + 8) as size_t, scratch.get_buffer() as pointer[char])'),
        (T, 'let buf host.allocate((fd.length + sd.length + 2) as size_t, 1) as pointer[char]', 'let buf Slice[char]((fd.length + sd.length + 2) as size_t, host.allocate((fd.length + sd.length + 2) as size_t, 1) as pointer[char])'),
        (T, 'var second_buf host.allocate((sd.length + 1) as size_t, 1) as pointer[char]', 'var second_buf Slice[char]((sd.length + 1) as size_t, host.allocate((sd.length + 1) as size_t, 1) as pointer[char])'),
        (T, 'let buf host.allocate((fd.length + 1) as size_t, 1) as pointer[char]', 'let buf Slice[char]((fd.length + 1) as size_t, host.allocate((fd.length + 1) as size_t, 1) as pointer[char])'),
    ],
    params=[
        (A, 'read_hex', 49, 'd', None),
        (L, 'unhex', 173, 's', None),
        (L, 'unhex', 173, 'out', None),
        (Sc, 'jsdoc_tag_at', 3030, 'buf', None),
        (Sc, 'jsdoc_tag_at', 3030, 'tag', 'taglen'),
        (T, 'copy_normalized', 565, 'dst', None),
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
    method_concepts=['Scanner', 'Checker'],
    this_slice_fields=['buffer'],
    ambiguous_fields=['name', 'parameter_name'],
    records={}, retlen=[], retlen_int=[], outpair=[],
)
