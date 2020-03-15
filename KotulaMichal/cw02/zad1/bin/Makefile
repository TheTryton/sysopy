C=gcc -Wall

all: tests

tests: generate copy_sys copy_lib sort_sys sort_lib clean

clean:
	rm dane1a
	rm dane4a
	rm dane512a
	rm dane1024a
	rm dane4096a
	rm dane8192a
	rm dane1b
	rm dane4b
	rm dane512b
	rm dane1024b
	rm dane4096b
	rm dane8192b

generate:
	./program generate dane1a 1000 1 lib
	./program generate dane4a 1000 4 lib
	./program generate dane512a 1000 512 lib
	./program generate dane1024a 1000 1024 lib
	./program generate dane4096a 1000 4096 lib
	./program generate dane8192a 1000 8192 lib
	./program generate dane1b 4000 1 lib
	./program generate dane4b 4000 4 lib
	./program generate dane512b 4000 512 lib
	./program generate dane1024b 4000 1024 lib
	./program generate dane4096b 4000 4096 lib
	./program generate dane8192b 4000 8192 lib

copy_sys: copy_sys_a copy_sys_b

copy_sys_a:
	echo "copy 1000 1 sys:" >> wyniki.txt
	{ time -p ./program copy dane1a dane1a_c 1000 1 sys; } 2>> wyniki.txt
	chmod 777 dane1a_c
	rm dane1a_c
	echo "copy 1000 4 sys:" >> wyniki.txt
	{ time -p ./program copy dane4a dane4a_c 1000 4 sys; } 2>> wyniki.txt
	chmod 777 dane4a_c
	rm dane4a_c
	echo "copy 1000 512 sys:" >> wyniki.txt
	{ time -p ./program copy dane512a dane512a_c 1000 512 sys; } 2>> wyniki.txt
	chmod 777 dane512a_c
	rm dane512a_c
	echo "copy 1000 1024 sys:" >> wyniki.txt
	{ time -p ./program copy dane1024a dane1024a_c 1000 1024 sys; } 2>> wyniki.txt
	chmod 777 dane1024a_c
	rm dane1024a_c
	echo "copy 1000 4096 sys:" >> wyniki.txt
	{ time -p ./program copy dane4096a dane4096a_c 1000 4096 sys; } 2>> wyniki.txt
	chmod 777 dane4096a_c
	rm dane4096a_c
	echo "copy 1000 8192 sys:" >> wyniki.txt
	{ time -p ./program copy dane4096a dane4096a_c 1000 8192 sys; } 2>> wyniki.txt
	chmod 777 dane4096a_c
	rm dane4096a_c	

copy_sys_b:
	echo "copy 4000 1 sys:" >> wyniki.txt
	{ time -p ./program copy dane1b dane1b_c 4000 1 sys; } 2>> wyniki.txt
	chmod 777 dane1b_c
	rm dane1b_c
	echo "copy 4000 4 sys:" >> wyniki.txt
	{ time -p ./program copy dane4b dane4b_c 4000 4 sys; } 2>> wyniki.txt
	chmod 777 dane4b_c
	rm dane4b_c
	echo "copy 4000 512 sys:" >> wyniki.txt
	{ time -p ./program copy dane512b dane512b_c 4000 512 sys; } 2>> wyniki.txt
	chmod 777 dane512b_c
	rm dane512b_c
	echo "copy 4000 1024 sys:" >> wyniki.txt
	{ time -p ./program copy dane1024b dane1024b_c 4000 1024 sys; } 2>> wyniki.txt
	chmod 777 dane1024b_c
	rm dane1024b_c
	echo "copy 4000 4096 sys:" >> wyniki.txt
	{ time -p ./program copy dane4096b dane4096b_c 4000 4096 sys; } 2>> wyniki.txt
	chmod 777 dane4096b_c
	rm dane4096b_c
	echo "copy 4000 8192 sys:" >> wyniki.txt
	{ time -p ./program copy dane4096b dane4096b_c 4000 8192 sys; } 2>> wyniki.txt
	chmod 777 dane4096b_c
	rm dane4096b_c

copy_lib: copy_lib_a copy_lib_b

copy_lib_a:
	echo "copy 1000 1 lib:" >> wyniki.txt
	{ time -p ./program copy dane1a dane1a_c 1000 1 lib; } 2>> wyniki.txt
	chmod 777 dane1a_c
	rm dane1a_c
	echo "copy 1000 4 lib:" >> wyniki.txt
	{ time -p ./program copy dane4a dane4a_c 1000 4 lib; } 2>> wyniki.txt
	chmod 777 dane4a_c
	rm dane4a_c
	echo "copy 1000 512 lib:" >> wyniki.txt
	{ time -p ./program copy dane512a dane512a_c 1000 512 lib; } 2>> wyniki.txt
	chmod 777 dane512a_c
	rm dane512a_c
	echo "copy 1000 1024 lib:" >> wyniki.txt
	{ time -p ./program copy dane1024a dane1024a_c 1000 1024 lib; } 2>> wyniki.txt
	chmod 777 dane1024a_c
	rm dane1024a_c
	echo "copy 1000 4096 lib:" >> wyniki.txt
	{ time -p ./program copy dane4096a dane4096a_c 1000 4096 lib; } 2>> wyniki.txt
	chmod 777 dane4096a_c
	rm dane4096a_c
	echo "copy 1000 8192 lib:" >> wyniki.txt
	{ time -p ./program copy dane4096a dane4096a_c 1000 8192 lib; } 2>> wyniki.txt
	chmod 777 dane4096a_c
	rm dane4096a_c	

copy_lib_b:
	echo "copy 4000 1 lib:" >> wyniki.txt
	{ time -p ./program copy dane1b dane1b_c 4000 1 lib; } 2>> wyniki.txt
	chmod 777 dane1b_c
	rm dane1b_c
	echo "copy 4000 4 lib:" >> wyniki.txt
	{ time -p ./program copy dane4b dane4b_c 4000 4 lib; } 2>> wyniki.txt
	chmod 777 dane4b_c
	rm dane4b_c
	echo "copy 4000 512 lib:" >> wyniki.txt
	{ time -p ./program copy dane512b dane512b_c 4000 512 lib; } 2>> wyniki.txt
	chmod 777 dane512b_c
	rm dane512b_c
	echo "copy 4000 1024 lib:" >> wyniki.txt
	{ time -p ./program copy dane1024b dane1024b_c 4000 1024 lib; } 2>> wyniki.txt
	chmod 777 dane1024b_c
	rm dane1024b_c
	echo "copy 4000 4096 lib:" >> wyniki.txt
	{ time -p ./program copy dane4096b dane4096b_c 4000 4096 lib; } 2>> wyniki.txt
	chmod 777 dane4096b_c
	rm dane4096b_c
	echo "copy 4000 8192 lib:" >> wyniki.txt
	{ time -p ./program copy dane4096b dane4096b_c 4000 8192 lib; } 2>> wyniki.txt
	chmod 777 dane4096b_c
	rm dane4096b_c

sort_sys: sort_sys_a sort_sys_b

sort_sys_a:
	echo "sort 1000 1 sys:" >> wyniki.txt
	./program copy dane1a dane_s 1000 1 lib
	{ time -p ./program sort dane_s 1000 1 sys; } 2>> wyniki.txt
	rm dane_s
	echo "sort 1000 4 sys:" >> wyniki.txt
	./program copy dane4a dane_s 1000 4 lib
	{ time -p ./program sort dane_s 1000 4 sys; } 2>> wyniki.txt
	rm dane_s
	echo "sort 1000 512 sys:" >> wyniki.txt
	./program copy dane512a dane_s 1000 512 lib
	{ time -p ./program sort dane_s 1000 512 sys; } 2>> wyniki.txt
	rm dane_s
	echo "sort 1000 1024 sys:" >> wyniki.txt
	./program copy dane1024a dane_s 1000 1024 lib
	{ time -p ./program sort dane_s 1000 1024 sys; } 2>> wyniki.txt
	rm dane_s
	echo "sort 1000 4096 sys:" >> wyniki.txt
	./program copy dane4096a dane_s 1000 4096 lib
	{ time -p ./program sort dane_s 1000 4096 sys; } 2>> wyniki.txt
	rm dane_s
	echo "sort 1000 8192 sys:" >> wyniki.txt
	./program copy dane8192a dane_s 1000 8192 lib
	{ time -p ./program sort dane_s 1000 8192 sys; } 2>> wyniki.txt
	rm dane_s

sort_sys_b:
	echo "sort 4000 1 sys:" >> wyniki.txt
	./program copy dane1b dane_s 4000 1 lib
	{ time -p ./program sort dane_s 4000 1 sys; } 2>> wyniki.txt
	rm dane_s
	echo "sort 4000 4 sys:" >> wyniki.txt
	./program copy dane4b dane_s 4000 4 lib
	{ time -p ./program sort dane_s 4000 4 sys; } 2>> wyniki.txt
	rm dane_s
	echo "sort 4000 512 sys:" >> wyniki.txt
	./program copy dane512b dane_s 4000 512 lib
	{ time -p ./program sort dane_s 4000 512 sys; } 2>> wyniki.txt
	rm dane_s
	echo "sort 4000 1024 sys:" >> wyniki.txt
	./program copy dane1024b dane_s 4000 1024 lib
	{ time -p ./program sort dane_s 4000 1024 sys; } 2>> wyniki.txt
	rm dane_s
	echo "sort 4000 4096 sys:" >> wyniki.txt
	./program copy dane4096b dane_s 4000 4096 lib
	{ time -p ./program sort dane_s 4000 4096 sys; } 2>> wyniki.txt
	rm dane_s
	echo "sort 4000 8192 sys:" >> wyniki.txt
	./program copy dane8192b dane_s 4000 8192 lib
	{ time -p ./program sort dane_s 4000 8192 sys; } 2>> wyniki.txt
	rm dane_s

sort_lib: sort_lib_a sort_lib_b

sort_lib_a:
	echo "sort 1000 1 lib:" >> wyniki.txt
	./program copy dane1a dane_s 1000 1 lib
	{ time -p ./program sort dane_s 1000 1 lib; } 2>> wyniki.txt
	rm dane_s
	echo "sort 1000 4 lib:" >> wyniki.txt
	./program copy dane4a dane_s 1000 4 lib
	{ time -p ./program sort dane_s 1000 4 lib; } 2>> wyniki.txt
	rm dane_s
	echo "sort 1000 512 lib:" >> wyniki.txt
	./program copy dane512a dane_s 1000 512 lib
	{ time -p ./program sort dane_s 1000 512 lib; } 2>> wyniki.txt
	rm dane_s
	echo "sort 1000 1024 lib:" >> wyniki.txt
	./program copy dane1024a dane_s 1000 1024 lib
	{ time -p ./program sort dane_s 1000 1024 lib; } 2>> wyniki.txt
	rm dane_s
	echo "sort 1000 4096 lib:" >> wyniki.txt
	./program copy dane4096a dane_s 1000 4096 lib
	{ time -p ./program sort dane_s 1000 4096 lib; } 2>> wyniki.txt
	rm dane_s
	echo "sort 1000 8192 lib:" >> wyniki.txt
	./program copy dane8192a dane_s 1000 8192 lib
	{ time -p ./program sort dane_s 1000 8192 lib; } 2>> wyniki.txt
	rm dane_s

sort_lib_b:
	echo "sort 4000 1 lib:" >> wyniki.txt
	./program copy dane1b dane_s 4000 1 lib
	{ time -p ./program sort dane_s 4000 1 lib; } 2>> wyniki.txt
	rm dane_s
	echo "sort 4000 4 lib:" >> wyniki.txt
	./program copy dane4b dane_s 4000 4 lib
	{ time -p ./program sort dane_s 4000 4 lib; } 2>> wyniki.txt
	rm dane_s
	echo "sort 4000 512 lib:" >> wyniki.txt
	./program copy dane512b dane_s 4000 512 lib
	{ time -p ./program sort dane_s 4000 512 lib; } 2>> wyniki.txt
	rm dane_s
	echo "sort 4000 1024 lib:" >> wyniki.txt
	./program copy dane1024b dane_s 4000 1024 lib
	{ time -p ./program sort dane_s 4000 1024 lib; } 2>> wyniki.txt
	rm dane_s
	echo "sort 4000 4096 lib:" >> wyniki.txt
	./program copy dane4096b dane_s 4000 4096 lib
	{ time -p ./program sort dane_s 4000 4096 lib; } 2>> wyniki.txt
	rm dane_s
	echo "sort 4000 8192 lib:" >> wyniki.txt
	./program copy dane8192b dane_s 4000 8192 lib
	{ time -p ./program sort dane_s 4000 8192 lib; } 2>> wyniki.txt
	rm dane_s

