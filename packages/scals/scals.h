#ifndef _scals_h
#define _scals_h
#include "../scaly/scaly.h"
using namespace scaly;
using namespace scaly::memory;

size_t get_message_length(char** buf, char* end);
String* get_message(Page* rp, int fd);
void process_message(String message);
#endif