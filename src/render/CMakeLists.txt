cmake_minimum_required (VERSION 2.8)

project (render)

set (LIBRARY_OUTPUT_PATH "${CMAKE_SOURCE_DIR}/lib/")

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

include_directories (../common/)
include_directories (../math/)

add_library(
    render
    STATIC
    ${source_files}
    ${header_files}
)

target_link_libraries(
    render
    math
)
