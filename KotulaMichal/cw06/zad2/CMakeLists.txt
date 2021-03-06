cmake_minimum_required(VERSION 3.8)
project(zad2server C)
project(zad2client C)
set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(common)

find_c_and_cpp_files("${CMAKE_CURRENT_SOURCE_DIR}/include/common" common_includes)
find_c_and_cpp_files("${CMAKE_CURRENT_SOURCE_DIR}/src/common" common_sources)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

add_executable(zad2client src/client.c ${common_includes})
add_executable(zad2server src/server.c ${common_includes})

target_link_libraries(zad2client PUBLIC rt)
target_link_libraries(zad2server PUBLIC rt)

target_include_directories(zad2client PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_include_directories(zad2server PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
