cmake_minimum_required(VERSION 3.8)
project(zad1server)
project(zad1client)
project(sockets)

set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(common)

find_c_and_cpp_files("${CMAKE_CURRENT_SOURCE_DIR}/include/sockets" sockets_includes)
find_c_and_cpp_files("${CMAKE_CURRENT_SOURCE_DIR}/src/sockets" sockets_sources)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

add_library(sockets ${sockets_sources} ${sockets_includes})
add_executable(zad1server src/server.cpp)
add_executable(zad1client src/client.cpp)

target_link_libraries(sockets PUBLIC pthread)
target_link_libraries(zad1server PUBLIC sockets)
target_link_libraries(zad1client PUBLIC sockets)

target_include_directories(sockets PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_include_directories(zad1server PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_include_directories(zad1client PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
set_target_properties(sockets PROPERTIES CXX_STANDARD 17)
set_target_properties(zad1server PROPERTIES CXX_STANDARD 17)
set_target_properties(zad1client PROPERTIES CXX_STANDARD 17)
