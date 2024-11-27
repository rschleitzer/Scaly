#include "../scals.h"
namespace scals {

RequestMessage::RequestMessage(String jsonrpc, size_t id, String method) : jsonrpc(jsonrpc), id(id), method(method) {}

RequestMessage de_serialize_requestMessage(Page* rp, String json) {
}

}