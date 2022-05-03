namespace scaly::compiler {
using namespace scaly::containers;

struct Parser : Object {
    Lexer lexer;
    String file_name;
    HashSet<String> keywords;

    Parser(String& deck)
      : lexer(*new(alignof(Lexer), Page::get(this)) Lexer(deck)),
        file_name(*String::from_c_string(Page::get(this), ""))
        {
            auto r = Region::create_from_page(Page::get(this));
            Array<String>& array = *Array<String>::create(r.page);
            array.add(*String::from_c_string(r.page, "as"));
            array.add(*String::from_c_string(r.page, "break"));
            array.add(*String::from_c_string(r.page, "catch"));
            array.add(*String::from_c_string(r.page, "case"));
            array.add(*String::from_c_string(r.page, "continue"));
            array.add(*String::from_c_string(r.page, "define"));
            array.add(*String::from_c_string(r.page, "default"));
            array.add(*String::from_c_string(r.page, "delegate"));
            array.add(*String::from_c_string(r.page, "drop"));
            array.add(*String::from_c_string(r.page, "else"));
            array.add(*String::from_c_string(r.page, "extends"));
            array.add(*String::from_c_string(r.page, "extern"));
            array.add(*String::from_c_string(r.page, "for"));
            array.add(*String::from_c_string(r.page, "function"));
            array.add(*String::from_c_string(r.page, "if"));
            array.add(*String::from_c_string(r.page, "is"));
            array.add(*String::from_c_string(r.page, "implement"));
            array.add(*String::from_c_string(r.page, "in"));
            array.add(*String::from_c_string(r.page, "instruction"));
            array.add(*String::from_c_string(r.page, "intrinsic"));
            array.add(*String::from_c_string(r.page, "label"));
            array.add(*String::from_c_string(r.page, "lambda"));
            array.add(*String::from_c_string(r.page, "let"));
            array.add(*String::from_c_string(r.page, "loop"));
            array.add(*String::from_c_string(r.page, "macro"));
            array.add(*String::from_c_string(r.page, "match"));
            array.add(*String::from_c_string(r.page, "module"));
            array.add(*String::from_c_string(r.page, "mutable"));
            array.add(*String::from_c_string(r.page, "operator"));
            array.add(*String::from_c_string(r.page, "procedure"));
            array.add(*String::from_c_string(r.page, "private"));
            array.add(*String::from_c_string(r.page, "return"));
            array.add(*String::from_c_string(r.page, "returns"));
            array.add(*String::from_c_string(r.page, "repeat"));
            array.add(*String::from_c_string(r.page, "set"));
            array.add(*String::from_c_string(r.page, "sizeof"));
            array.add(*String::from_c_string(r.page, "throw"));
            array.add(*String::from_c_string(r.page, "throws"));
            array.add(*String::from_c_string(r.page, "trait"));
            array.add(*String::from_c_string(r.page, "union"));
            array.add(*String::from_c_string(r.page, "use"));
            array.add(*String::from_c_string(r.page, "var"));
            array.add(*String::from_c_string(r.page, "while"));
            Vector<String>& vector = *Vector<String>::from_array(r.page, array);
            keywords = *HashSet<String>::from_vector(Page::get(this), vector);

        }
 };

}