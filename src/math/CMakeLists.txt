cmake_minimum_required (VERSION 2.8)

project (math)

set(LIBRARY_OUTPUT_PATH "${CMAKE_SOURCE_DIR}/lib/")

file(
    GLOB_RECURSE
    source_files
    *.cpp
)

file(
    GLOB_RECURSE
    header_files
    ../../include/math/*.h
)

include_directories(../../include)

if (NOT SKETCH_3D_BUILD_STATIC)
	set (LIBRARY_TYPE SHARED)
else ()
	set (LIBRARY_TYPE STATIC)
endif ()

add_library(
    math
    ${LIBRARY_TYPE}
    ${source_files}
    ${header_files}
)

target_link_libraries(
    math
	system
)
