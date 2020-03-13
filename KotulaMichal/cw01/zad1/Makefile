CXX=g++
INC=-I ./include -I ../standard/include
LIB=-L ../standard/lib
CXXCOMPILEFLAGS=-c -Wall -O0 $(INC) $(LIB) -lstd_lib
CXXCOMPILEFLAGSSTATIC=$(CXXCOMPILEFLAGS)
CXXCOMPILEFLAGSDYNAMIC=-fPIC $(CXXCOMPILEFLAGS)

all: diff_library_static diff_library_shared

diff_library_static: diff_functions_static.o
	ar rcs ./lib/libdiff_libraryst.a dfstatic.o
	
diff_library_shared: diff_functions_shared.o
	$(CXX) -shared -o ./lib/libdiff_librarysh.so dfshared.o -lc

diff_functions_static.o:
	$(CXX) $(CXXCOMPILEFLAGSSTATIC) -o dfstatic.o  ./src/diff/diff_functions.cpp

diff_functions_shared.o:
	$(CXX) $(CXXCOMPILEFLAGSDYNAMIC) -o dfshared.o ./src/diff/diff_functions.cpp

clean:
	rm dfstatic.o
	rm dfshared.o
	rm ./lib/libdiff_libraryst.a
	rm ./lib/libdiff_librarysh.so

