cmake_minimum_required(VERSION 3.8)
project(zad2 C)
project(zad2child C)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

add_executable(zad2 src/main.c)
target_include_directories(zad2 PUBLIC include)

add_executable(zad2child src/child.c)
target_include_directories(zad2child PUBLIC include)
