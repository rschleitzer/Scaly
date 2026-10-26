/* stubs_wasm.c -- what scalyc calls that a wasm32-wasip1 build has not:
 * the initializers of the backends other than WebAssembly (libLLVM here is
 * built with the WebAssembly target alone), dynamic loading and system(),
 * which WASI has neither of. Each answers what a missing facility answers. */
#include <stddef.h>

void LLVMInitializeAArch64AsmPrinter(void) {}
void LLVMInitializeAArch64Target(void) {}
void LLVMInitializeAArch64TargetInfo(void) {}
void LLVMInitializeAArch64TargetMC(void) {}
void LLVMInitializeX86AsmPrinter(void) {}
void LLVMInitializeX86Target(void) {}
void LLVMInitializeX86TargetInfo(void) {}
void LLVMInitializeX86TargetMC(void) {}

void *dlopen(const char *file, int mode) { (void)file; (void)mode; return NULL; }
void *dlsym(void *handle, const char *name) { (void)handle; (void)name; return NULL; }
int dlclose(void *handle) { (void)handle; return 0; }
char *dlerror(void) { return (char *)"no dynamic loading on WASI"; }

int system(const char *command) { (void)command; return -1; }
