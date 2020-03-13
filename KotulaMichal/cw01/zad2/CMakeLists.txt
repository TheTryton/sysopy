cmake_minimum_required(VERSION 3.8)
project(diff_app CXX)

set(diff_library_static "${CMAKE_CURRENT_SOURCE_DIR}/../zad1/lib/libdiff_library.a")
set(diff_library_static_include_dirs "${CMAKE_CURRENT_SOURCE_DIR}/../zad1/include")
set(std_lib "${CMAKE_CURRENT_SOURCE_DIR}/../standard/lib/libstd_lib.a")
set(std_lib_include_dirs "${CMAKE_CURRENT_SOURCE_DIR}/../standard/include")

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(common)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_C_FLAGS "-Wall")

find_c_and_cpp_files("${CMAKE_CURRENT_SOURCE_DIR}/src" diff_app_sources)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${diff_app_sources})

add_executable(diff_app ${diff_app_sources})

target_link_libraries(diff_app PUBLIC ${diff_library_static} ${std_lib})
target_include_directories(diff_app PUBLIC ${diff_library_static_include_dirs} ${std_lib_include_dirs} ${CMAKE_CURRENT_SOURCE_DIR}/src)