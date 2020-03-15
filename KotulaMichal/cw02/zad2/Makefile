C=gcc -Wall

all: zad2a zad2b

zad2a: maina.o
	$(C) maina.o -O3 -o ./bin/programa

maina.o: 
	$(C) -c ./src/main.c -o maina.o -DUSE_B_TRAVERSE=0

zad2b: mainb.o
	$(C) mainb.o -O3 -o ./bin/programb

mainb.o: 
	$(C) -c ./src/main.c -o mainb.o -DUSE_B_TRAVERSE=1

clean:
	rm maina.o
	rm mainb.o
	rm ./bin/programa
	rm ./bin/programb
