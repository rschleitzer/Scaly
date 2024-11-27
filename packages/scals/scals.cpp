#include "scals.h"
using namespace scaly::json;


void process_message(String message) {
    auto r = Region();
    auto p = r.get_page();
    Parser& parser = *new (alignof(Parser), r.get_page()) Parser(message);
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
                                            if (s.value.equals("id")) {
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
                                                                                method = sm.value;
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
                };
                break;
            }
            default:
                exit(4);
        }
    };
}

String* get_message(Page* rp, int fd) {
    auto r = Region();
    const size_t buffer_size = 0x800;
    char* buffer = (char*)(*r.get_page()).allocate(buffer_size, 1);
    auto bytes_read = read(fd, buffer, buffer_size);
    if (bytes_read == -1) 
        exit(-1);
    if (bytes_read == 0) 
        return nullptr;
    auto end = buffer+bytes_read;
    write(2, buffer, bytes_read);
    auto index = buffer;
    size_t length = 0;
    while (index<end) {
        if (*index>='0'&&*index<='9') 
            break;
        index = index+1;
    };
    const auto last = index;
    while (index<end) {
        if (*index<'0'||*index>'9') 
            break;
        index = index+1;
    };
    auto current = index-1;
    auto multiplier = 1;
    while (current>=last) {
        length = length+(*current-'0')*multiplier;
        multiplier = multiplier*10;
        current = current-1;
    };
    while (index<end) {
        if (*index != '\r'&&*index>'\n') 
            break;
        index = index+1;
    };
    if (length == 0) 
        return nullptr;
    StringBuilder& builder = *new (alignof(StringBuilder), r.get_page()) StringBuilder(length);
    while (length>0) {
        const auto remainder = end-index;
        builder.append(index, remainder);
        length = length-remainder;
        if (length<0) 
            exit(-1);
        if (length == 0) 
            break;
        auto bytes_to_read = length;
        if (bytes_to_read>buffer_size) 
            bytes_to_read = buffer_size;
        bytes_read = read(fd, buffer, bytes_to_read);
        if (bytes_read == -1) 
            exit(-2);
        if (bytes_read == 0) 
            return nullptr;
        index = buffer;
        end = buffer+bytes_read;
        write(2, index, bytes_read);
    };
    return new (alignof(String), rp) String(rp, builder.to_string(r.get_page()));
}
