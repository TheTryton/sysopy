CXX=g++
INC=-I ./include
CXXCOMPILEFLAGS=-c -Wall -O0 $(INC)

all: std_lib

std_lib: string.o
	ar rcs ./lib/libstd_lib.a string.o

string.o:
	$(CXX) $(CXXCOMPILEFLAGS)  src/string/string.cpp

clean:
	rm string.o
	rm ./lib/libstd_lib.a
