CC=gcc
CCFLAGS=-std=c11 -pedantic -Wall -Wextra
CCOPT=-O2
CXX=g++
CXXFLAGS=-std=c++14 -pedantic -Wall -Wextra
CXXOPT=-O2 -fno-rtti

LDFLAGS=-static -luuid -lole32 -Wl,--add-stdcall-alias
DLLFLAGS=-shared

CppCom.Factorial.dll: $(patsubst %.cpp, %.o, $(wildcard *.cpp)) IFactorial_i.o
	$(CXX) $(CXXFLAGS) $(DLLFLAGS) -o $@ $^ $(LDFLAGS)

clean:
	-rm *.o CppCom.Factorial.dll

# but, you say, this has 'extern "C"' protection, why compile as C?
# because the treacherous gangster thugs of Microsoft have foolishly
# forgottent that a const variable at file scope in C++ without explicit extern
# linkage has implicit internal linkage, and hence can get optimized
# out by the compiler; additionally extern "C" blocks are rendered useless
# by this as name mangling will still be applied to the
# presumed-internal-linkage-only symbol names.
#
# in C, const variables at file scope have external linkage
IFactorial_i.o: IFactorial_i.c
	$(CC) $(CCFLAGS) $(CCOPT) -o $@ -c $<

%.o: %.cpp IFactorial.h IFactorial_i.o
	$(CXX) $(CXXFLAGS) $(CXXOPT) -o $@ -c $<

IFactorial.h: IFactorial.idl
	midl IFactorial.idl

IFactorial.tlb: IFactorial.h

IFactorial_i.c: IFactorial.h

register: CppCom.Factorial.dll
	regsvr32 CppCom.Factorial.dll

unregister: CppCom.Factorial.dll
	regsvr32 /u CppCom.Factorial.dll
