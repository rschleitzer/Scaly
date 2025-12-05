#ifndef _Lsp_h
#define _Lsp_h
#include "../scals.h"
using namespace scaly;
using namespace scaly::memory;

using namespace scaly::json;

struct RequestMessage : Object {
    String jsonrpc;
    size_t id;
    String method;

    RequestMessage(String jsonrpc, size_t id, String method);
};

RequestMessage de_serialize_requestMessage(Page* rp, String json);
#endif