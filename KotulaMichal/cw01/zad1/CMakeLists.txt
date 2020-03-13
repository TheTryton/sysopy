cmake_minimum_required(VERSION 3.8)
project(diff_library_static CXX)
project(diff_library_shared CXX)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(common)

set(std_lib "${CMAKE_CURRENT_SOURCE_DIR}/../standard/lib/libstd_lib.a")
set(std_lib_include "${CMAKE_CURRENT_SOURCE_DIR}/../standard/include")

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_C_FLAGS "-Wall")

find_c_and_cpp_files("${CMAKE_CURRENT_SOURCE_DIR}/include" diff_library_includes)
find_c_and_cpp_files("${CMAKE_CURRENT_SOURCE_DIR}/src" diff_library_sources)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${diff_library_includes} ${diff_library_sources})

add_library(diff_library_static STATIC ${diff_library_includes} ${diff_library_sources})
add_library(diff_library_shared SHARED ${diff_library_includes} ${diff_library_sources})

target_link_libraries(diff_library_static PUBLIC ${std_lib})
target_link_libraries(diff_library_shared PUBLIC ${std_lib})
target_include_directories(diff_library_static PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include ${std_lib_include})
target_include_directories(diff_library_shared PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include ${std_lib_include})

set_target_properties(diff_library_static PROPERTIES OUTPUT_NAME diff_library)
set_target_properties(diff_library_shared PROPERTIES OUTPUT_NAME diff_library)