cmake_minimum_required(VERSION 3.8)
project(common C)
project(matrix C)
project(child C)
project(generator C)
project(tester C)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

#find_c_and_cpp_files("${CMAKE_CURRENT_SOURCE_DIR}/src" zad2_sources)

add_library(common STATIC src/common.c)
target_include_directories(common PUBLIC include)
target_link_libraries(common PUBLIC m)

add_executable(matrix src/main.c)
target_include_directories(matrix PUBLIC include)
target_link_libraries(matrix PUBLIC common)

add_executable(child src/child.c)
target_include_directories(child PUBLIC include)
target_link_libraries(child PUBLIC common)

add_executable(generator src/generator.c)
target_include_directories(generator PUBLIC include)
target_link_libraries(generator PUBLIC common)

add_executable(tester src/tester.c)
target_include_directories(tester PUBLIC include)
target_link_libraries(tester PUBLIC common)
