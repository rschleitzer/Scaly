"""lldb data formatters for Scaly's stdlib types.

Load it from an lldb session, a ~/.lldbinit, or a VS Code launch config:

    command script import <repo>/tools/lldb/scaly.py

What it is for: DWARF describes Scaly's containers HONESTLY, and honest is
unreadable. A `String` is a single pointer at a varint-length-prefixed buffer, so
without a formatter the debugger can only show the address (and, if it guesses
the bytes are a C string, the LENGTH PREFIX in front of the text). A `Vector` is
a length and a base pointer, so its elements are not children of anything. That
is exactly what a formatter is for, and it is the last mile of the -g work:
`tests/debuginfo/run.sh` gates the DWARF, this file makes it legible.

Deliberately NOT here: the tagged unions. Their tag carries a DWARF
ENUMERATION built over every variant, so lldb prints `tag = Chosen` by itself —
a formatter would only re-implement what the type already says. See
di_union_type in Emitter.scaly for why the mapping cannot live on the payload
union instead.

Layouts (packages/scaly/0.1.0/scaly/containers/) — every field is read BY NAME,
never by offset, so a layout change surfaces as a missing field rather than as
silently wrong values:
    String    (data: pointer[u8])                    varint length, then bytes
    Vector[T] (length: size_t, data: pointer[T])
    Array[T]  (length: size_t, vector: pointer[Vector[T]])
    List[T]   (head: pointer[Node[T]])               Node (element: T, next: ptr)
"""

import lldb             # lldb injects this into __main__ but NOT into imported
                        # modules — without it every formatter raises NameError
                        # at its first SBError() and lldb falls back to the raw
                        # struct, which looks like the formatter simply not
                        # matching the type.

MAX_ELEMENTS = 256      # cap on synthesized children, so a corrupt length or a
                        # cyclic list cannot hang the debugger
MAX_STRING = 4096


def _err(valobj):
    """A value we cannot read at all — distinguish it from an empty one."""
    return not valobj.IsValid() or valobj.GetError().Fail()


def _child(valobj, name):
    """Read a REAL struct member.

    ★GetNonSyntheticValue() is load-bearing: inside a synthetic-children
    provider (and in a summary registered on a type that has one) the value
    already carries the synthetic filter, so GetChildMemberWithName resolves
    against the SYNTHESIZED children instead of the struct's own members. The
    failure is silent and looks like a DWARF defect — measured 2026-08-11, a
    Vector[int] whose `data` member is `int *` in the DWARF yielded a 1-byte
    element type, so the children came out as bytes ('\\a', 0, 0) instead of
    7, 8, 9."""
    base = valobj.GetNonSyntheticValue()
    if base is None or not base.IsValid():
        base = valobj
    c = base.GetChildMemberWithName(name)
    return c if (c is not None and c.IsValid()) else None


# ---------------------------------------------------------------- String

def _read_varint(process, addr):
    """LEB128 as String.get_length writes it: 7 bits per byte, 0x80 continues.
    Returns (value, bytes_consumed) or (None, 0). PACKED_SIZE is 9, so a run
    longer than that is corruption, not a big number."""
    value = 0
    shift = 0
    for consumed in range(1, 10):
        err = lldb.SBError()
        byte = process.ReadUnsignedFromMemory(addr + consumed - 1, 1, err)
        if err.Fail():
            return None, 0
        value |= (byte & 0x7F) << shift
        if (byte & 0x80) == 0:
            return value, consumed
        shift += 7
    return None, 0


def string_summary(valobj, internal_dict):
    data = _child(valobj, "data")
    if data is None:
        return "<no data field>"
    addr = data.GetValueAsUnsigned(0)
    if addr == 0:
        return '""'                      # an empty String allocates nothing
    process = valobj.GetProcess()
    if not process.IsValid():
        return "<no process>"
    length, consumed = _read_varint(process, addr)
    if length is None:
        return "<unreadable length prefix @ 0x%x>" % addr
    if length > MAX_STRING:
        return '<length %d exceeds %d> @ 0x%x' % (length, MAX_STRING, addr)
    err = lldb.SBError()
    raw = process.ReadMemory(addr + consumed, length, err)
    if err.Fail():
        return "<unreadable %d bytes @ 0x%x>" % (length, addr + consumed)
    return '"%s"' % raw.decode("utf-8", "replace")


# ------------------------------------------------- Vector / Array / List

class _Elements:
    """Shared synthetic-children base: subclasses supply a count and a base
    pointer, and the element type comes from the pointer's POINTEE — so this
    works for any instantiation without knowing T."""

    def __init__(self, valobj, internal_dict):
        self.valobj = valobj
        self.count = 0
        self.base = 0
        self.elem_type = None
        self.elem_size = 0

    def num_children(self):
        return min(self.count, MAX_ELEMENTS)

    def has_children(self):
        return self.count > 0

    def get_child_index(self, name):
        try:
            return int(name.lstrip("[").rstrip("]"))
        except ValueError:
            return -1

    def get_child_at_index(self, index):
        if index < 0 or index >= self.num_children() or self.elem_type is None:
            return None
        return self.valobj.CreateValueFromAddress(
            "[%d]" % index, self.base + index * self.elem_size, self.elem_type)

    def _from_length_and_data(self, length_owner, data_owner):
        length = _child(length_owner, "length") if length_owner else None
        data = _child(data_owner, "data") if data_owner else None
        if length is None or data is None:
            return
        self.count = length.GetValueAsUnsigned(0)
        self.base = data.GetValueAsUnsigned(0)
        if self.base == 0:
            self.count = 0
            return
        self.elem_type = data.GetType().GetPointeeType()
        self.elem_size = self.elem_type.GetByteSize()
        if self.elem_size == 0:
            self.count = 0


class VectorChildren(_Elements):
    def update(self):
        self.count = 0
        self._from_length_and_data(self.valobj, self.valobj)
        return False


class ArrayChildren(_Elements):
    def update(self):
        # Array holds its own length and a POINTER to the Vector that owns the
        # buffer, so the length comes from Array and the data from the Vector.
        self.count = 0
        vec = _child(self.valobj, "vector")
        if vec is None or vec.GetValueAsUnsigned(0) == 0:
            return False
        self._from_length_and_data(self.valobj, vec.Dereference())
        return False


class ListChildren:
    """List is a chain, not a buffer: walk head -> next. The cap doubles as the
    cycle guard — a corrupted `next` must not hang the debugger."""

    def __init__(self, valobj, internal_dict):
        self.valobj = valobj
        self.nodes = []

    def update(self):
        self.nodes = []
        node = _child(self.valobj, "head")
        seen = set()
        while node is not None and node.GetValueAsUnsigned(0) != 0:
            addr = node.GetValueAsUnsigned(0)
            if addr in seen or len(self.nodes) >= MAX_ELEMENTS:
                break
            seen.add(addr)
            target = node.Dereference()
            element = _child(target, "element")
            if element is None:
                break
            self.nodes.append(element)
            node = _child(target, "next")
        return False

    def num_children(self):
        return len(self.nodes)

    def has_children(self):
        return len(self.nodes) > 0

    def get_child_index(self, name):
        try:
            return int(name.lstrip("[").rstrip("]"))
        except ValueError:
            return -1

    def get_child_at_index(self, index):
        if index < 0 or index >= len(self.nodes):
            return None
        return self.nodes[index].Clone("[%d]" % index)


def _count_summary(valobj, internal_dict):
    length = _child(valobj, "length")
    if length is not None:
        return "%d element(s)" % length.GetValueAsUnsigned(0)
    return ""


def __lldb_init_module(debugger, internal_dict):
    # A generic INSTANTIATION carries its mangled name (di_display_name explains
    # why: lldb uniques types by name, so every Vector[T] sharing the name
    # "Vector" collapsed into one type and read elements at the wrong stride).
    # `_Z6VectorI...E` is Itanium: `_Z`, then the LENGTH of the concept name,
    # then the name, then `I` args `E`. The length prefix is what makes these
    # regexes exact — `_Z6VectorI` can never match VectorIterator, which is
    # `_Z14VectorIteratorI` and has different fields.
    #
    # The bare alternative stays in each pattern for non-generic spellings and
    # for the day InstantiationInfo.type_args is populated and the names become
    # readable (`Vector[int]`); then these can lose the mangled arm.
    add = debugger.HandleCommand
    add('type summary add -F scaly.string_summary -x "^String$"')
    add('type synthetic add -l scaly.VectorChildren -x "^(Vector(\\[.*\\])?|_Z6VectorI.*E)$"')
    add('type synthetic add -l scaly.ArrayChildren  -x "^(Array(\\[.*\\])?|_Z5ArrayI.*E)$"')
    add('type synthetic add -l scaly.ListChildren   -x "^(List(\\[.*\\])?|_Z4ListI.*E)$"')
    add('type summary add -F scaly._count_summary -e -x "^(Vector(\\[.*\\])?|_Z6VectorI.*E)$"')
    add('type summary add -F scaly._count_summary -e -x "^(Array(\\[.*\\])?|_Z5ArrayI.*E)$"')
    print("scaly: formatters loaded (String, Vector, Array, List)")
