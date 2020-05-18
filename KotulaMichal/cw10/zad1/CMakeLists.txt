cmake_minimum_required(VERSION 3.8)
project(zad1server C)
project(zad1client C)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

add_executable(zad1server src/server.c include/messages.h include/common.h)
add_executable(zad1client src/client.c include/messages.h include/common.h)

target_link_libraries(zad1server PUBLIC pthread)
target_link_libraries(zad1client PUBLIC pthread)

target_include_directories(zad1server PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_include_directories(zad1client PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
