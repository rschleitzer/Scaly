#include "../scals.h"
namespace scals {
using namespace scaly::json;


RequestMessage::RequestMessage(String jsonrpc, size_t id, String method) : jsonrpc(jsonrpc), id(id), method(method) {}

RequestMessage de_serialize_requestMessage(Page* rp, String json) {
    auto r = Region();
    auto p = r.get_page();
    Parser& parser = *new (alignof(Parser), r.get_page()) Parser(json);
    const auto _value_syntax_result = parser.parse_value(p, p);
    auto value_syntax = _value_syntax_result._Ok;
    if (_value_syntax_result._tag == Success::Error) {
        const auto _value_syntax_Error = _value_syntax_result._Error;
        switch (_value_syntax_Error._tag) {
            case ParserError::Invalid: {
                const auto i = _value_syntax_Error._Invalid;
                exit(1);
                break;
            }
            case ParserError::Different: {
                const auto d = _value_syntax_Error._Different;
                exit(2);
                break;
            }
        }
    }if (parser.is_at_end() == false) 
        exit(3);
    {
        auto _result = value_syntax;
        switch (_result._tag)
        {
            case ValueSyntax::Object:
            {
                auto o = _result._Object;
                {
                    auto jsonrpc = String();
                    size_t id = 0;
                    auto method = String();
                    
                    auto _property_iterator = (*o.properties).get_iterator();
                    while (auto _property = _property_iterator.next()) {
                        auto property = *_property;{
                            {
                                auto _result = property.name.literal;
                                switch (_result._tag)
                                {
                                    case Literal::String:
                                    {
                                        auto s = _result._String;
                                        {
                                            if (s.value.equals("jsonrpc")) {
                                                {
                                                    auto _result = property.value;
                                                    switch (_result._tag)
                                                    {
                                                        case ValueSyntax::Literal:
                                                        {
                                                            auto s = _result._Literal;
                                                            {
                                                                {
                                                                    auto _result = s.literal;
                                                                    switch (_result._tag)
                                                                    {
                                                                        case Literal::String:
                                                                        {
                                                                            auto sm = _result._String;
                                                                            {
                                                                                jsonrpc = String(rp, sm.value);
                                                                            };
                                                                            break;
                                                                        }
                                                                        default:
                                                                            exit(8);
                                                                    }
                                                                };
                                                            };
                                                            break;
                                                        }
                                                        default:
                                                            exit(9);
                                                    }
                                                };
                                            }
                                            else if (s.value.equals("id")) {
                                                {
                                                    auto _result = property.value;
                                                    switch (_result._tag)
                                                    {
                                                        case ValueSyntax::Literal:
                                                        {
                                                            auto l = _result._Literal;
                                                            {
                                                                {
                                                                    auto _result = l.literal;
                                                                    switch (_result._tag)
                                                                    {
                                                                        case Literal::Integer:
                                                                        {
                                                                            auto i = _result._Integer;
                                                                            {
                                                                                const auto value = (size_t)strtol(i.value.to_c_string(r.get_page()), nullptr, 10);
                                                                                if (errno == ERANGE) 
                                                                                    exit(9);
                                                                                id = value;
                                                                            };
                                                                            break;
                                                                        }
                                                                        default:
                                                                            exit(8);
                                                                    }
                                                                };
                                                            };
                                                            break;
                                                        }
                                                        default:
                                                            exit(7);
                                                    }
                                                };
                                            }
                                            else if (s.value.equals("method")) {
                                                {
                                                    auto _result = property.value;
                                                    switch (_result._tag)
                                                    {
                                                        case ValueSyntax::Literal:
                                                        {
                                                            auto s = _result._Literal;
                                                            {
                                                                {
                                                                    auto _result = s.literal;
                                                                    switch (_result._tag)
                                                                    {
                                                                        case Literal::String:
                                                                        {
                                                                            auto sm = _result._String;
                                                                            {
                                                                                method = String(rp, sm.value);
                                                                            };
                                                                            break;
                                                                        }
                                                                        default:
                                                                            exit(8);
                                                                    }
                                                                };
                                                            };
                                                            break;
                                                        }
                                                        default:
                                                            exit(9);
                                                    }
                                                };
                                            };
                                        };
                                        break;
                                    }
                                    default:
                                        exit(6);
                                }
                            };
                        }
                    };
                    return RequestMessage(jsonrpc, id, method);
                };
                break;
            }
            default:
                exit(4);
        }
    };
}

}