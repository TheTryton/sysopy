CXX=g++
INC=-I ../zad2/include -I ../zad2/src -I ../standard/include -I ../zad1/include
LIB=-L ./lib

all: diff_app_staticO0 diff_app_staticO3 diff_app_staticOs diff_app_sharedO0 diff_app_sharedO3 diff_app_sharedOs diff_app_dynamicO0 diff_app_dynamicO3 diff_app_dynamicOs
	sh -c 'cd bin && export LD_LIBRARY_PATH=../lib && exec ./diff_appstO0 test'
	sh -c 'cd bin && export LD_LIBRARY_PATH=../lib && exec ./diff_appstO3 test'
	sh -c 'cd bin && export LD_LIBRARY_PATH=../lib && exec ./diff_appstOs test'
	sh -c 'cd bin && export LD_LIBRARY_PATH=../lib && exec ./diff_appshO0 test'
	sh -c 'cd bin && export LD_LIBRARY_PATH=../lib && exec ./diff_appshO3 test'
	sh -c 'cd bin && export LD_LIBRARY_PATH=../lib && exec ./diff_appshOs test'
	sh -c 'cd bin && export LD_LIBRARY_PATH=../lib && exec ./diff_appdyO0 test'
	sh -c 'cd bin && export LD_LIBRARY_PATH=../lib && exec ./diff_appdyO3 test'
	sh -c 'cd bin && export LD_LIBRARY_PATH=../lib && exec ./diff_appdyOs test'

# DIFF APP STATIC

diff_app_staticO0: standard_staticO0 diff_library_staticO0 mainstO0.o timeO0.o
	$(CXX) $(LIB) -Wall mainstO0.o timeO0.o -o ./bin/diff_appstO0 -lstd_libstO0 -ldiff_librarystO0 -O0
mainstO0.o: 
	$(CXX) $(INC) -Wall -c ../zad2/src/main.cpp -o mainstO0.o -DRESULT_FILE=\"raport3b.txt\" -DTEST_NAME=\"zadanie3b11\" -O0
timeO0.o: 
	$(CXX) $(INC) -Wall -c ../zad2/src/time.cpp -o timeO0.o -O0

diff_app_staticO3: standard_staticO3 diff_library_staticO3 mainstO3.o timeO3.o
	$(CXX) $(LIB) -Wall mainstO3.o timeO3.o -o ./bin/diff_appstO3 -lstd_libstO3 -ldiff_librarystO3 -O3
mainstO3.o: 
	$(CXX) $(INC) -Wall -c ../zad2/src/main.cpp -o mainstO3.o -DRESULT_FILE=\"raport3b.txt\" -DTEST_NAME=\"zadanie3b12\" -O3
timeO3.o: 
	$(CXX) $(INC) -Wall -c ../zad2/src/time.cpp -o timeO3.o -O3

diff_app_staticOs: standard_staticOs diff_library_staticOs mainstOs.o timeOs.o
	$(CXX) $(LIB) -Wall mainstOs.o timeOs.o -o ./bin/diff_appstOs -lstd_libstOs -ldiff_librarystOs -Os
mainstOs.o: 
	$(CXX) $(INC) -Wall -c ../zad2/src/main.cpp -o mainstOs.o -DRESULT_FILE=\"raport3b.txt\" -DTEST_NAME=\"zadanie3b13\" -Os
timeOs.o: 
	$(CXX) $(INC) -Wall -c ../zad2/src/time.cpp -o timeOs.o -Os

# DIFF APP SHARED

diff_app_sharedO0: standard_sharedO0 diff_library_sharedO0 mainshO0.o timeO0.o
	$(CXX) $(LIB) -Wall mainshO0.o timeO0.o -o ./bin/diff_appshO0 -lstd_libshO0 -ldiff_libraryshO0 -O0
mainshO0.o: 
	$(CXX) $(INC) -Wall -c ../zad2/src/main.cpp -o mainshO0.o -DRESULT_FILE=\"raport3b.txt\" -DTEST_NAME=\"zadanie3b21\" -O0

diff_app_sharedO3: standard_sharedO3 diff_library_sharedO3 mainshO3.o timeO3.o
	$(CXX) $(LIB) -Wall mainshO3.o timeO3.o -o ./bin/diff_appshO3 -lstd_libshO3 -ldiff_libraryshO3 -O3
mainshO3.o: 
	$(CXX) $(INC) -Wall -c ../zad2/src/main.cpp -o mainshO3.o -DRESULT_FILE=\"raport3b.txt\" -DTEST_NAME=\"zadanie3b22\" -O3

diff_app_sharedOs: standard_sharedOs diff_library_sharedOs mainshOs.o timeOs.o
	$(CXX) $(LIB) -Wall mainshOs.o timeOs.o -o ./bin/diff_appshOs -lstd_libshOs -ldiff_libraryshOs -Os
mainshOs.o: 
	$(CXX) $(INC) -Wall -c ../zad2/src/main.cpp -o mainshOs.o -DRESULT_FILE=\"raport3b.txt\" -DTEST_NAME=\"zadanie3b23\" -Os

# DIFF APP DYNAMIC

diff_app_dynamicO0: standard_sharedO0 diff_library_sharedO0 maindyO0.o timeO0.o
	$(CXX) $(LIB) -Wall maindyO0.o timeO0.o -o ./bin/diff_appdyO0 -lstd_libshO0 -ldl -O0
maindyO0.o: 
	$(CXX) $(INC) -Wall -c ../zad2/src/main.cpp -o maindyO0.o -DRESULT_FILE=\"raport3b.txt\" -DDYNAMIC_LOAD_FUNC=1 -DTEST_NAME=\"zadanie3b31\" -DLIBRARY_NAME=\"libdiff_libraryshO0.so\" -O0

diff_app_dynamicO3: standard_sharedO3 diff_library_sharedO3 maindyO3.o timeO3.o
	$(CXX) $(LIB) -Wall maindyO3.o timeO3.o -o ./bin/diff_appdyO3 -lstd_libshO3 -ldl -O3
maindyO3.o: 
	$(CXX) $(INC) -Wall -c ../zad2/src/main.cpp -o maindyO3.o -DRESULT_FILE=\"raport3b.txt\" -DDYNAMIC_LOAD_FUNC=1 -DTEST_NAME=\"zadanie3b32\" -DLIBRARY_NAME=\"libdiff_libraryshO3.so\" -O3

diff_app_dynamicOs: standard_sharedOs diff_library_sharedOs maindyOs.o timeOs.o
	$(CXX) $(LIB) -Wall maindyOs.o timeOs.o -o ./bin/diff_appdyOs -lstd_libshOs -ldl -Os
maindyOs.o: 
	$(CXX) $(INC) -Wall -c ../zad2/src/main.cpp -o maindyOs.o -DRESULT_FILE=\"raport3b.txt\" -DDYNAMIC_LOAD_FUNC=1 -DTEST_NAME=\"zadanie3b32\" -DLIBRARY_NAME=\"libdiff_libraryshOs.so\" -Os

# STANDARD STATIC

standard_staticO0: stringstO0.o
	ar rcs ./lib/libstd_libstO0.a stringstO0.o
stringstO0.o:
	$(CXX) $(INC) -c ../standard/src/string/string.cpp -o stringstO0.o -O3

standard_staticO3: stringstO3.o
	ar rcs ./lib/libstd_libstO3.a stringstO3.o
stringstO3.o:
	$(CXX) $(INC) -c ../standard/src/string/string.cpp -o stringstO3.o -O3

standard_staticOs: stringstOs.o
	ar rcs ./lib/libstd_libstOs.a stringstOs.o
stringstOs.o:
	$(CXX) $(INC) -c ../standard/src/string/string.cpp -o stringstOs.o -Os

# STANDARD SHARED

standard_sharedO0: stringshO0.o
	$(CXX) -shared -o ./lib/libstd_libshO0.so stringshO0.o -lc -O0
stringshO0.o:
	$(CXX) $(INC) -c -fPIC ../standard/src/string/string.cpp -o stringshO0.o -O0

standard_sharedO3: stringshO3.o
	$(CXX) -shared -o ./lib/libstd_libshO3.so stringshO3.o -lc -O3
stringshO3.o:
	$(CXX) $(INC) -c -fPIC ../standard/src/string/string.cpp -o stringshO3.o -O3

standard_sharedOs: stringshOs.o
	$(CXX) -shared -o ./lib/libstd_libshOs.so stringshOs.o -lc -Os
stringshOs.o:
	$(CXX) $(INC) -c -fPIC ../standard/src/string/string.cpp -o stringshOs.o -Os

# DIFF LIBRARY STATIC

diff_library_staticO0: standard_staticO0 diff_functionsstO0.o
	ar rcs ./lib/libdiff_librarystO0.a dfstO0.o
diff_functionsstO0.o:
	$(CXX) $(INC) -c ../zad1/src/diff/diff_functions.cpp -o dfstO0.o -O0

diff_library_staticO3: standard_staticO3 diff_functionsstO3.o
	ar rcs ./lib/libdiff_librarystO3.a dfstO3.o
diff_functionsstO3.o:
	$(CXX) $(INC) -c ../zad1/src/diff/diff_functions.cpp -o dfstO3.o -O3

diff_library_staticOs: standard_staticOs diff_functionsstOs.o
	ar rcs ./lib/libdiff_librarystOs.a dfstOs.o
diff_functionsstOs.o:
	$(CXX) $(INC) -c ../zad1/src/diff/diff_functions.cpp -o dfstOs.o -Os

# DIFF LIBRARY SHARED

diff_library_sharedO0: standard_sharedO0 diff_functionsshO0.o
	$(CXX) $(LIB) -lstd_libshO0 -shared -o ./lib/libdiff_libraryshO0.so dfshO0.o -lc -O0
diff_functionsshO0.o:
	$(CXX) $(INC) -c -fPIC ../zad1/src/diff/diff_functions.cpp -o dfshO0.o -O0

diff_library_sharedO3: standard_sharedO3 diff_functionsshO3.o
	$(CXX) $(LIB) -lstd_libshO3 -shared -o ./lib/libdiff_libraryshO3.so dfshO3.o -lc -O3
diff_functionsshO3.o:
	$(CXX) $(INC) -c -fPIC ../zad1/src/diff/diff_functions.cpp -o dfshO3.o -O3

diff_library_sharedOs: standard_sharedOs diff_functionsshOs.o
	$(CXX) $(LIB) -lstd_libshOs -shared -o ./lib/libdiff_libraryshOs.so dfshOs.o -lc -Os
diff_functionsshOs.o:
	$(CXX) $(INC) -c -fPIC ../zad1/src/diff/diff_functions.cpp -o dfshOs.o -Os

clean:
	rm mainstO0.o
	rm mainstO3.o
	rm mainstOs.o
	rm mainshO0.o
	rm mainshO3.o
	rm mainshOs.o
	rm maindyO0.o
	rm maindyO3.o
	rm maindyOs.o
	rm timeO0.o
	rm timeO3.o
	rm timeOs.o
	rm dfstO0.o
	rm dfstO3.o
	rm dfstOs.o
	rm dfshO0.o
	rm dfshO3.o
	rm dfshOs.o
	rm stringstO0.o
	rm stringstO3.o
	rm stringstOs.o
	rm stringshO0.o
	rm stringshO3.o
	rm stringshOs.o
	rm ./bin/diff_appstO0
	rm ./bin/diff_appstO3
	rm ./bin/diff_appstOs
	rm ./bin/diff_appdyO0
	rm ./bin/diff_appdyO3
	rm ./bin/diff_appdyOs
	rm ./bin/diff_appshO0
	rm ./bin/diff_appshO3
	rm ./bin/diff_appshOs
	rm ./lib/libstd_libstO0.a
	rm ./lib/libstd_libstO3.a
	rm ./lib/libstd_libstOs.a
	rm ./lib/libstd_libshO0.so
	rm ./lib/libstd_libshO3.so
	rm ./lib/libstd_libshOs.so
	rm ./lib/libdiff_librarystO0.a
	rm ./lib/libdiff_librarystO3.a
	rm ./lib/libdiff_librarystOs.a
	rm ./lib/libdiff_libraryshO0.so
	rm ./lib/libdiff_libraryshO3.so
	rm ./lib/libdiff_libraryshOs.so
