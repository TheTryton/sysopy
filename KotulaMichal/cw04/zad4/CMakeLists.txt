cmake_minimum_required(VERSION 3.8)
project(sender C)
project(catcher C)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

add_executable(sender src/sender.c)
target_include_directories(sender PUBLIC include)

add_executable(catcher src/catcher.c)
target_include_directories(catcher PUBLIC include)
