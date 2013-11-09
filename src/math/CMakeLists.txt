cmake_minimum_required (VERSION 2.8)

project (math)

set(LIBRARY_OUTPUT_PATH "${CMAKE_SOURCE_DIR}/builds/lib/")

file(
    GLOB_RECURSE
    source_files
    *.cpp
)

file(
    GLOB_RECURSE
    header_files
    *.h
)

include_directories(../common/)

add_library(
    math
    STATIC
    ${source_files}
    ${header_files}
)

target_link_libraries(
    math
)
