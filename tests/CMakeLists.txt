cmake_minimum_required (VERSION 2.8)
project (tests)

set (EXECUTABLE_OUTPUT_PATH "${CMAKE_SOURCE_DIR}/bin/")

file(
    GLOB_RECURSE
    math_files
    math/*.cpp
)

file(
    GLOB_RECURSE
    render_files
    render/*.cpp
)

include_directories(
    math
    render
    ${Boost_INCLUDE_DIRS}
)

include_directories(../src/math/)
include_directories(../src/common/)
include_directories(../src/render/)

link_directories(${Boost_LIBRARY_DIRS})
link_directories(${CMAKE_SOURCE_DIR}/lib/)

add_executable(
    tests
    Main.cpp
    ${math_files}
    ${render_files}
)

SOURCE_GROUP ("Misc" FILES ./main.cpp)
SOURCE_GROUP ("Math" REGULAR_EXPRESSION math/.*\\.cpp)
SOURCE_GROUP ("Render" REGULAR_EXPRESSION render/.*\\.cpp)

target_link_libraries(
    tests
    math
    render
    ${Boost_LIBRARIES}
)

add_test(sketch-3d_test ${CMAKE_SOURCE_DIR}/bin/tests)
