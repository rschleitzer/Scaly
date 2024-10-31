#include "../../scaly.h"
namespace scaly {
namespace compiler {
using namespace scaly::containers;


IdentifierToken::IdentifierToken(Vector<char> name) : name(name) {}

AttributeToken::AttributeToken(Vector<char> name) : name(name) {}

PunctuationToken::PunctuationToken(char sign) : sign(sign) {}

StringToken::StringToken(Vector<char> value) : value(value) {}

CharacterToken::CharacterToken(Vector<char> value) : value(value) {}

FragmentToken::FragmentToken(Vector<char> value) : value(value) {}

IntegerToken::IntegerToken(Vector<char> value) : value(value) {}

BooleanToken::BooleanToken(bool value) : value(value) {}

FloatingPointToken::FloatingPointToken(Vector<char> value) : value(value) {}

HexToken::HexToken(Vector<char> value) : value(value) {}
LiteralToken::LiteralToken(StringToken _StringToken) : _tag(String) { _String = _StringToken; }

LiteralToken::LiteralToken(CharacterToken _CharacterToken) : _tag(Character) { _Character = _CharacterToken; }

LiteralToken::LiteralToken(FragmentToken _FragmentToken) : _tag(Fragment) { _Fragment = _FragmentToken; }

LiteralToken::LiteralToken(IntegerToken _IntegerToken) : _tag(Integer) { _Integer = _IntegerToken; }

LiteralToken::LiteralToken(BooleanToken _BooleanToken) : _tag(Boolean) { _Boolean = _BooleanToken; }

LiteralToken::LiteralToken(FloatingPointToken _FloatingPointToken) : _tag(FloatingPoint) { _FloatingPoint = _FloatingPointToken; }

LiteralToken::LiteralToken(HexToken _HexToken) : _tag(Hex) { _Hex = _HexToken; }

Token::Token(EmptyToken _EmptyToken) : _tag(Empty) { _Empty = _EmptyToken; }

Token::Token(InvalidToken _InvalidToken) : _tag(Invalid) { _Invalid = _InvalidToken; }

Token::Token(IdentifierToken _IdentifierToken) : _tag(Identifier) { _Identifier = _IdentifierToken; }

Token::Token(AttributeToken _AttributeToken) : _tag(Attribute) { _Attribute = _AttributeToken; }

Token::Token(PunctuationToken _PunctuationToken) : _tag(Punctuation) { _Punctuation = _PunctuationToken; }

Token::Token(LiteralToken _LiteralToken) : _tag(Literal) { _Literal = _LiteralToken; }

Token::Token(ColonToken _ColonToken) : _tag(Colon) { _Colon = _ColonToken; }


Lexer::Lexer(Token token, char* character, StringIterator chars, size_t previous_position, size_t position) : token(token), character(character), chars(chars), previous_position(previous_position), position(position) {}

}
}