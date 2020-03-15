C=gcc -Wall

all: zad1 tests

tests: zad1
	cd bin && make

zad1: main.o
	$(C) main.o -O3 -o ./bin/program

main.o: 
	$(C) -c ./src/main.c -o main.o

clean:
	rm main.o
	rm ./bin/program
