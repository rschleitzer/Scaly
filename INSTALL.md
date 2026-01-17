# Installing Scaly

## Quick Start

```bash
git clone https://github.com/user/Scaly.git
cd Scaly
./install.sh
```

## Prerequisites

### macOS (Homebrew)

```bash
brew install llvm cmake
```

### Ubuntu 24.04 LTS

```bash
sudo apt install llvm-18-dev clang cmake
```

### Ubuntu 22.04 LTS

```bash
# Add LLVM repository
wget https://apt.llvm.org/llvm.sh
chmod +x llvm.sh
sudo ./llvm.sh 18

# Install dependencies
sudo apt install llvm-18-dev clang-18 cmake
```

## Manual Installation

If you prefer to build manually:

```bash
cd scalyc
mkdir -p build && cd build
cmake ..
make

# Run tests
./scalyc --test

# Install (optional)
sudo install -m 755 scalyc /usr/local/bin/
sudo mkdir -p /usr/local/share/scaly/packages
sudo cp -r ../../packages/scaly /usr/local/share/scaly/packages/
```

## Hello World

After installation, try compiling the hello world example:

```bash
# Using the installed compiler
scalyc hello.scaly -o hello
./hello
```

Or create your own:

```bash
cat > myprogram.scaly << 'EOF'
use scaly.io.Console

function main() returns int
{
    Console.println("Hello from Scaly!")
    0
}
EOF

scalyc myprogram.scaly -o myprogram
./myprogram
```

## Compiler Options

```
scalyc [options] <input file>

Options:
  -o <file>       Output executable file
  -c              Compile only (generate object file)
  -S              Emit LLVM IR
  -v              Verbose output
  --test          Run test suite
```
