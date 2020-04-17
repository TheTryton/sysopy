cmake_minimum_required(VERSION 3.8)
project(zad1server CXX)
project(zad1client CXX)
project(common CXX)

set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(common)

find_c_and_cpp_files("${CMAKE_CURRENT_SOURCE_DIR}/include/common" common_includes)
find_c_and_cpp_files("${CMAKE_CURRENT_SOURCE_DIR}/src/common" common_sources)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

add_library(common ${common_sources} ${common_includes})
target_include_directories(common PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

add_executable(zad1client src/client.cpp)
add_executable(zad1server src/server.cpp)

target_link_libraries(zad1client PUBLIC common)
target_link_libraries(zad1server PUBLIC common)

target_include_directories(zad1client PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_include_directories(zad1server PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

set_target_properties(common PROPERTIES CXX_STANDARD 17)
set_target_properties(zad1client PROPERTIES CXX_STANDARD 17)
set_target_properties(zad1server PROPERTIES CXX_STANDARD 17)
