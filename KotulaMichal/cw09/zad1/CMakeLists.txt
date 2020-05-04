cmake_minimum_required(VERSION 3.8)
project(zad1 C)

set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(common)

find_c_and_cpp_files("${CMAKE_CURRENT_SOURCE_DIR}/include/common" common_includes)
find_c_and_cpp_files("${CMAKE_CURRENT_SOURCE_DIR}/src/common" common_sources)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

add_executable(zad1 src/main.c ${common_sources} ${common_includes})

target_link_libraries(zad1 PUBLIC pthread)

target_include_directories(zad1 PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
