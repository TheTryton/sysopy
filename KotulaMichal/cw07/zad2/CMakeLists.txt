cmake_minimum_required(VERSION 3.8)
project(zad2receiver C)
project(zad2packer C)
project(zad2sender C)
project(zad2manager C)

set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(common)

find_c_and_cpp_files("${CMAKE_CURRENT_SOURCE_DIR}/include/common" common_includes)
find_c_and_cpp_files("${CMAKE_CURRENT_SOURCE_DIR}/src/common" common_sources)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

add_executable(zad2receiver src/receiver.c ${common_sources} ${common_includes})
add_executable(zad2packer src/packer.c ${common_sources} ${common_includes})
add_executable(zad2sender src/sender.c ${common_sources} ${common_includes})
add_executable(zad2manager src/manager.c ${common_sources} ${common_includes})

target_link_libraries(zad2receiver rt pthread)
target_link_libraries(zad2packer rt pthread)
target_link_libraries(zad2sender rt pthread)
target_link_libraries(zad2manager rt pthread)

target_include_directories(zad2receiver PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_include_directories(zad2packer PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_include_directories(zad2sender PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_include_directories(zad2manager PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
