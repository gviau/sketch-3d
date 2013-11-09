cmake_minimum_required (VERSION 2.8)
project (tests)

set (EXECUTABLE_OUTPUT_PATH "${CMAKE_SOURCE_DIR}/builds/bin/")

file(
    GLOB_RECURSE
    math_files
    math/*.cpp
)

include_directories(
    math
    ${Boost_INCLUDE_DIRS}
)

include_directories(../src/math/)
include_directories(../src/common/)

link_directories(${Boost_LIBRARY_DIRS})
link_directories(${CMAKE_SOURCE_DIR}/builds/lib/)

add_executable(
    tests
    main.cpp
    ${math_files}
)

SOURCE_GROUP ("Misc" FILES ./main.cpp)
SOURCE_GROUP ("Math" REGULAR_EXPRESSION math/.*\\.cpp)

target_link_libraries(
    tests
    math
    ${Boost_LIBRARIES}
)

get_target_property(tests_EXE tests_all LOCATION)

add_custom_target(
    RUN_TESTS
    ${tests_EXE} --report_level=no
    DEPENDS tests_all
)
