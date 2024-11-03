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
struct FileError;
struct Directory;
struct File;
struct Path;
}
namespace compiler {
struct DifferentSyntax;
struct InvalidSyntax;
struct ParserError;
struct EmptyToken;
struct InvalidToken;
struct ColonToken;
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
struct LiteralToken;
struct Token;
struct Lexer;
struct IdentifierLiteral;
struct StringLiteral;
struct CharacterLiteral;
struct FragmentLiteral;
struct IntegerLiteral;
struct BooleanLiteral;
struct FloatingPointLiteral;
struct HexLiteral;
struct Literal;
struct ThrownSyntax;
struct ReferenceSyntax;
struct LocalSyntax;
struct CallSyntax;
struct LifetimeSyntax;
struct ExtensionSyntax;
struct NameSyntax;
struct TypeSyntax;
struct SizeOfSyntax;
struct SetSyntax;
struct OperationSyntax;
struct ActionSyntax;
struct RepeatSyntax;
struct LabelSyntax;
struct ForSyntax;
struct LambdaSyntax;
struct ThrowSyntax;
struct ReturnSyntax;
struct LoopSyntax;
struct BreakSyntax;
struct ContinueSyntax;
struct TypeAnnotationSyntax;
struct PropertySyntax;
struct FieldSyntax;
struct PartSyntax;
struct StructureSyntax;
struct ArraySyntax;
struct BindingSpecSyntax;
struct BindingAnnotationSyntax;
struct BindingSyntax;
struct MutableSyntax;
struct VarSyntax;
struct LetSyntax;
struct WhenSyntax;
struct ConditionSyntax;
struct TrySyntax;
struct ChooseSyntax;
struct WhileSyntax;
struct CommandSyntax;
struct StatementSyntax;
struct DefaultSyntax;
struct CaseSyntax;
struct BranchSyntax;
struct MatchSyntax;
struct ElseSyntax;
struct IfSyntax;
struct BlockSyntax;
struct ElementSyntax;
struct VectorSyntax;
struct ValueSyntax;
struct ComponentSyntax;
struct ObjectSyntax;
struct LiteralSyntax;
struct ExpressionSyntax;
struct MemberAccessSyntax;
struct OperandSyntax;
struct InitializerSyntax;
struct PackageSyntax;
struct ModuleSyntax;
struct ModelSyntax;
struct AttributeSyntax;
struct MacroSyntax;
struct ExtendSyntax;
struct ExtendsSyntax;
struct IntrinsicSyntax;
struct InstructionSyntax;
struct ExternSyntax;
struct ImplementationSyntax;
struct RoutineSyntax;
struct NamedSyntax;
struct TargetSyntax;
struct OperatorSyntax;
struct ProcedureSyntax;
struct FunctionSyntax;
struct DeInitSyntax;
struct InitSyntax;
struct MethodSyntax;
struct TraitSyntax;
struct ImplementSyntax;
struct UseSyntax;
struct ThrowsSyntax;
struct ReturnsSyntax;
struct ItemSyntax;
struct ParametersSyntax;
struct ParameterSetSyntax;
struct OptionalSyntax;
struct GenericArgumentSyntax;
struct GenericArgumentsSyntax;
struct DelegateSyntax;
struct ConstantSyntax;
struct VariantSyntax;
struct UnionSyntax;
struct NamespaceSyntax;
struct BodySyntax;
struct ClassSyntax;
struct ConceptSyntax;
struct GenericParameterSyntax;
struct GenericParametersSyntax;
struct DefinitionSyntax;
struct ConstituentSyntax;
struct MemberSyntax;
struct ExportSyntax;
struct PrivateSyntax;
struct SymbolSyntax;
struct DeclarationSyntax;
struct FileSyntax;
struct ProgramSyntax;
struct Parser;
}
}
