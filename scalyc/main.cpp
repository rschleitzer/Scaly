#include "llvm/Support/CommandLine.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;

// Command line options
static cl::OptionCategory ScalyCategory("Scaly Compiler Options");

static cl::list<std::string> InputFiles(
    cl::Positional,
    cl::desc("<input files>"),
    cl::cat(ScalyCategory));

static cl::opt<std::string> OutputFile(
    "o",
    cl::desc("Output file"),
    cl::value_desc("file"),
    cl::cat(ScalyCategory));

static cl::opt<bool> CompileOnly(
    "c",
    cl::desc("Compile only (no link)"),
    cl::cat(ScalyCategory));

static cl::opt<bool> EmitLLVM(
    "S",
    cl::desc("Emit LLVM IR"),
    cl::cat(ScalyCategory));

static cl::opt<bool> RunTests(
    "test",
    cl::desc("Run test suite"),
    cl::cat(ScalyCategory));

static cl::opt<std::string> TestName(
    "test-name",
    cl::desc("Run specific test"),
    cl::value_desc("name"),
    cl::cat(ScalyCategory));

static cl::opt<bool> Verbose(
    "v",
    cl::desc("Verbose output"),
    cl::cat(ScalyCategory));

int main(int argc, char **argv) {
    cl::HideUnrelatedOptions(ScalyCategory);
    cl::ParseCommandLineOptions(argc, argv, "Scaly Compiler\n");

    if (RunTests) {
        outs() << "Running tests...\n";
        // TODO: Run compiled-in tests via JIT
        return 0;
    }

    if (InputFiles.empty()) {
        errs() << "scalyc: error: no input files\n";
        return 1;
    }

    for (const auto &File : InputFiles) {
        if (Verbose) {
            outs() << "Processing: " << File << "\n";
        }
        // TODO: Compile file
    }

    return 0;
}
