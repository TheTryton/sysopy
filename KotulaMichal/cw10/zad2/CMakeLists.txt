cmake_minimum_required(VERSION 3.8)
project(zad2server C)
project(zad2client C)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

add_executable(zad2server src/server.c include/messages.h include/common.h)
add_executable(zad2client src/client.c include/messages.h include/common.h)

target_link_libraries(zad2server PUBLIC pthread)
target_link_libraries(zad2client PUBLIC pthread)

target_include_directories(zad2server PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_include_directories(zad2client PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
