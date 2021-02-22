CFLAGS=-g `llvm-config --cflags`
LD=clang++
LDFLAGS=`llvm-config --cxxflags --ldflags --libs core executionengine mcjit interpreter analysis native bitwriter --system-libs`

all: shortest

shortest.o: shortest.scaly
	dotnet ../scalyc/bin/Debug/net5.0/linux-x64/scalyc.dll -o shortest shortest.scaly

shortest: shortest.o
	$(LD) $< $(LDFLAGS) -o $@

clean:
	-rm -f shortest.o shortest