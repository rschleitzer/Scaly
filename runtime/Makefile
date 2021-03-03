CFLAGS=-g `llvm-config --cflags`
LD=clang++
LDFLAGS=`llvm-config --cxxflags --ldflags --libs core executionengine mcjit interpreter analysis native bitwriter --system-libs`

all: argc

argc.o: ../scalyc/tests/argc.scaly
	dotnet ../scalyc/bin/Debug/net5.0/linux-x64/scalyc.dll -o argc ../scalyc/tests/argc.scaly

shortest: argc.o
	$(LD) $< $(LDFLAGS) -o $@

clean:
	-rm -f argc.o argc