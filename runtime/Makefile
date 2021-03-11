PROGRAM=strlen

CFLAGS=-g `llvm-config --cflags`
LD=clang++
LDFLAGS=`llvm-config --cxxflags --ldflags --libs core executionengine mcjit interpreter analysis native bitwriter --system-libs`

all: $(PROGRAM)

$(PROGRAM).o: ../scalyc/tests/$(PROGRAM).scaly
	dotnet ../scalyc/bin/Debug/net5.0/osx-x64/scalyc.dll -o $(PROGRAM) ../scalyc/tests/$(PROGRAM).scaly

$(PROGRAM): $(PROGRAM).o
	$(LD) $< $(LDFLAGS) -o $@

clean:
	-rm -f $(PROGRAM).o $(PROGRAM)