CXX=g++ -Wall
INC=-I ../zad2/include -I ../zad2/src -I ../standard/include -I ../zad1/include
LIB=-L ./lib
LIBS=-lstd_lib -ldiff_library
DEFINES=-DRESULT_FILE=\"raport2.txt\"

all: diff_app_dynamic diff_app_shared diff_app_static
	sh -c 'cd bin && export LD_LIBRARY_PATH=../lib && exec ./diff_appst test'
	sh -c 'cd bin && export LD_LIBRARY_PATH=../lib && exec ./diff_appsh test'
	sh -c 'cd bin && export LD_LIBRARY_PATH=../lib && exec ./diff_appdy test'

standard_shared: stringsh.o
	$(CXX) -shared -o ./lib/libstd_libsh.so stringsh.o -lc
stringsh.o:
	$(CXX) $(INC) -c -fPIC ../standard/src/string/string.cpp -o stringsh.o

standard_static: stringst.o
	ar rcs ./lib/libstd_libst.a stringst.o
stringst.o:
	$(CXX) $(INC) -c ../standard/src/string/string.cpp -o stringst.o

diff_library_shared: standard_shared diff_functionssh.o
	$(CXX) $(LIB) -lstd_libsh -shared -o ./lib/libdiff_librarysh.so dfsh.o -lc
diff_functionssh.o:
	$(CXX) $(INC) -c -fPIC ../zad1/src/diff/diff_functions.cpp -o dfsh.o

diff_library_static: standard_static diff_functionsst.o
	ar rcs ./lib/libdiff_libraryst.a dfst.o
diff_functionsst.o:
	$(CXX) $(INC) -c ../zad1/src/diff/diff_functions.cpp -o dfst.o


diff_app_dynamic: standard_shared diff_library_shared maindy.o time.o
	$(CXX) $(LIB) -Wall maindy.o time.o -o ./bin/diff_appdy -lstd_libsh -ldl
diff_app_shared: standard_shared diff_library_shared mainsh.o time.o
	$(CXX) $(LIB) -Wall mainsh.o time.o -o ./bin/diff_appsh -lstd_libsh -ldiff_librarysh
diff_app_static: standard_static diff_library_static mainst.o time.o
	$(CXX) $(LIB) -Wall mainst.o time.o -o ./bin/diff_appst -lstd_libst -ldiff_libraryst

maindy.o: 
	$(CXX) $(INC) -Wall -c ../zad2/src/main.cpp -o maindy.o -DRESULT_FILE=\"raport3a.txt\" -DDYNAMIC_LOAD_FUNC=1 -DTEST_NAME=\"zadanie3a3\" -DLIBRARY_NAME=\"libdiff_librarysh.so\"

mainsh.o: 
	$(CXX) $(INC) -Wall -c ../zad2/src/main.cpp -o mainsh.o -DRESULT_FILE=\"raport3a.txt\" -DTEST_NAME=\"zadanie3a2\"

mainst.o: 
	$(CXX) $(INC) -Wall -c ../zad2/src/main.cpp -o mainst.o -DRESULT_FILE=\"raport3a.txt\" -DTEST_NAME=\"zadanie3a1\"

time.o: 
	$(CXX) $(INC) -Wall -c ../zad2/src/time.cpp -o time.o

clean:
	rm mainst.o
	rm mainsh.o
	rm maindy.o
	rm time.o
	rm dfst.o
	rm dfsh.o
	rm stringst.o
	rm stringsh.o
	rm ./bin/diff_appdy
	rm ./bin/diff_appst
	rm ./bin/diff_appsh
	rm ./lib/libstd_libst.a
	rm ./lib/libstd_libsh.so
	rm ./lib/libdiff_libraryst.a
	rm ./lib/libdiff_librarysh.so
