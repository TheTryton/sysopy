cmake_minimum_required(VERSION 3.8)
project(std_lib CXX)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(common)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_C_FLAGS "-Wall -fPIC")

find_c_and_cpp_files("${CMAKE_CURRENT_SOURCE_DIR}/include" std_includes)
find_c_and_cpp_files("${CMAKE_CURRENT_SOURCE_DIR}/src" std_sources)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${std_includes} ${std_sources})

add_library(std_lib STATIC ${std_includes} ${std_sources})

target_include_directories(std_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

set_target_properties(std_lib PROPERTIES OUTPUT_NAME std_lib)
