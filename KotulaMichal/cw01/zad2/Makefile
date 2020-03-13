CXX=g++
INC=-I ./src -I ../standard/include -I ../zad1/include
LIB=-L ../standard/lib -L ../zad1/lib
LIBS=-lstd_lib -ldiff_libraryst

all: diff_app
	sh -c 'cd bin && exec ./diff_app test'

diff_app: main.o time.o
	$(CXX) -Wall main.o time.o -o ./bin/diff_app $(LIB) $(LIBS)
	
main.o: 
	$(CXX) -Wall -c $(INC) ./src/main.cpp -o main.o -DRESULT_FILE=\"raport2.txt\" -DTEST_NAME=\"zadanie2\"

time.o: 
	$(CXX) -Wall -c $(INC) ./src/time.cpp -o time.o

clean:
	rm main.o
	rm time.o
	rm ./bin/diff_app
