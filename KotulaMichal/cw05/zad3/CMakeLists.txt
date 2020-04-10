cmake_minimum_required(VERSION 3.8)
project(zad3producer C)
project(zad3consumer C)
project(zad3 C)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

add_executable(zad3producer src/producer.c)
add_executable(zad3consumer src/consumer.c)
add_executable(zad3 src/main.c)
target_include_directories(zad3producer PUBLIC include)
target_include_directories(zad3consumer PUBLIC include)
target_include_directories(zad3 PUBLIC include)
