cmake_minimum_required(VERSION 3.8)
project(zad1 C)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(common)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

set(CMAKE_C_FLAGS "-Wall")

find_c_and_cpp_files("${CMAKE_CURRENT_SOURCE_DIR}/src" zad1_sources)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${zad1_sources})

add_executable(zad1 ${zad1_sources})
