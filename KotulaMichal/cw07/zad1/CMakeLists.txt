cmake_minimum_required(VERSION 3.8)
project(zad1receiver C)
project(zad1packer C)
project(zad1sender C)
project(zad1manager C)

set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(common)

find_c_and_cpp_files("${CMAKE_CURRENT_SOURCE_DIR}/include/common" common_includes)
find_c_and_cpp_files("${CMAKE_CURRENT_SOURCE_DIR}/src/common" common_sources)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

add_executable(zad1receiver src/receiver.c ${common_sources} ${common_includes})
add_executable(zad1packer src/packer.c ${common_sources} ${common_includes})
add_executable(zad1sender src/sender.c ${common_sources} ${common_includes})
add_executable(zad1manager src/manager.c ${common_sources} ${common_includes})

target_include_directories(zad1receiver PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_include_directories(zad1packer PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_include_directories(zad1sender PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_include_directories(zad1manager PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
