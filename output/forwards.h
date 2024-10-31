typedef const char const_char;
typedef struct stat struct_stat;
typedef const void const_void;
namespace scaly {
namespace memory {
struct Object;
struct PageNode;
struct PageList;
struct PageListIterator;
struct Page;
struct Region;
}
namespace containers {
template<class T>
struct Node;
template<class T>
struct ListIterator;
template<class T>
struct List;
template<class T>
struct VectorIterator;
template<class T>
struct Vector;
template<class T>
struct Array;
template<class T>
struct BuilderListIterator;
template<class T>
struct BuilderList;
namespace hashing {
}
template<class T>
struct Slot;
template<class T>
struct HashSetBuilder;
template<class T>
struct HashSet;
template<class K, class V>
struct KeyValuePair;
template<class K, class V>
struct HashMapBuilder;
template<class K, class V>
struct HashMapIterator;
template<class K, class V>
struct HashMap;
template<class K, class V>
struct MultiMapBuilder;
template<class K, class V>
struct MultiMapIterator;
template<class K, class V>
struct MultiMap;
struct String;
struct StringIterator;
struct StringBuilder;
}
namespace io {
namespace Console {
}
struct UnknownFileError;
struct NoSuchFileOrDirectoryError;
struct Directory;
struct File;
struct Path;
}
namespace compiler {
struct EmptyToken;
struct InvalidToken;
struct IdentifierToken;
struct AttributeToken;
struct PunctuationToken;
struct StringToken;
struct CharacterToken;
struct FragmentToken;
struct IntegerToken;
struct BooleanToken;
struct FloatingPointToken;
struct HexToken;
}
}
