#ifndef _io_h
#define _io_h
#include "../scaly.h"
using namespace scaly;
using namespace scaly::memory;
namespace io {
#include "io/Console.h"
#include "io/FileError.h"
#include "io/Directory.h"
#include "io/File.h"
#include "io/Path.h"

void test_file();
void test_path();
void test_directory();
void test();
}
#endif