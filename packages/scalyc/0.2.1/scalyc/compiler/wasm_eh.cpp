// What the compiler needs from LLVM that its C API does not say: the
// exception model of a target machine, for WebAssembly's catch points; and,
// built for WebAssembly itself, lld's wasm linker and the host (at the end).
//
// The emitter writes a `setjmp` at every catch point (a `try`, a panic arm);
// on WebAssembly LLVM lowers it to wasm exception handling when
// `-wasm-enable-sjlj` is given (Parts.enable_wasm_sjlj) and the target
// machine's exception model is Wasm. LLVM 21's WebAssembly target machine
// derives that model from the option, but only AFTER it built its MCAsmInfo
// from the options it was handed -- the C API hands `None` -- and the code
// generator asks the MCAsmInfo: every invoke became a plain call and no catch
// point ever caught. LLVM main (the coming 23) builds the MCAsmInfo after the
// check, 22.x does not; with the first release that does, this file goes.
//
// C++ because the model is a field of `TargetMachine::Options` and of the
// MCAsmInfo, reached through inline members only: no symbol of LLVM's C++
// library is linked, so it links wherever the C API does (LLVM-C.dll
// included), and needs LLVM's C++ headers to compile.
//
// No reference to LLVM's ABI-check symbol either (llvm/Config/abi-breaking.h):
// it lives in the C++ library, which the C API's DLL does not export.
#define LLVM_DISABLE_ABI_BREAKING_CHECKS_ENFORCING 1
#include "llvm-c/TargetMachine.h"
#include "llvm/MC/MCAsmInfo.h"
#include "llvm/Target/TargetMachine.h"

extern "C" void scalyc_target_machine_wasm_eh(LLVMTargetMachineRef ref)
{
    llvm::TargetMachine *tm = reinterpret_cast<llvm::TargetMachine *>(ref);
    tm->Options.ExceptionModel = llvm::ExceptionHandling::Wasm;
    const_cast<llvm::MCAsmInfo *>(tm->getMCAsmInfo())->setExceptionsType(llvm::ExceptionHandling::Wasm);
}

// The compiler in a page (the REPL in the browser): where it is built for
// WebAssembly there is no JIT, so an entry is linked by lld's wasm linker --
// a library of the program there -- and run by the host, which starts the
// module as an instance of its own. Natively the three answer that none of
// it is here; they reference nothing of LLVM's C++ library, so the file
// still links wherever the C API does.
#if defined(__wasi__)
#include "lld/Common/Driver.h"
LLD_HAS_DRIVER(wasm)

extern "C" int scalyc_has_jit(void) { return 0; }

// wasm-ld with argv (argv[0] names the flavour): its exit code
extern "C" int scalyc_wasm_link(int argc, const char **argv)
{
    lld::Result r = lld::lldMain(llvm::ArrayRef<const char *>(argv, argc), llvm::outs(), llvm::errs(),
                                 {{lld::Wasm, &lld::wasm::link}});
    llvm::outs().flush();
    llvm::errs().flush();
    return r.retCode;
}

// the host's: start the module at path with argv, answer its exit code
__attribute__((import_module("scaly"), import_name("run")))
extern "C" int scalyc_host_run(const char *path, int argc, const char **argv);

extern "C" int scalyc_wasm_run(const char *path, int argc, const char **argv)
{
    return scalyc_host_run(path, argc, argv);
}
#else
extern "C" int scalyc_has_jit(void) { return 1; }
extern "C" int scalyc_wasm_link(int argc, const char **argv) { (void)argc; (void)argv; return -1; }
extern "C" int scalyc_wasm_run(const char *path, int argc, const char **argv) { (void)path; (void)argc; (void)argv; return -1; }
#endif
