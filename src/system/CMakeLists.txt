cmake_minimum_required (VERSION 2.8)

project (system)

set(LIBRARY_OUTPUT_PATH "${CMAKE_SOURCE_DIR}/lib/")

file(
    GLOB_RECURSE
    source_files
    *.cpp
)

file(
    GLOB_RECURSE
    header_files
    ../../include/system/*.h
)

include_directories(../../include/)

add_library(
    system
    STATIC
    ${source_files}
    ${header_files}
)

target_link_libraries(
    system
)
