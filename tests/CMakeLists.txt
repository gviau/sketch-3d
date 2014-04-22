cmake_minimum_required (VERSION 2.8)
project (tests)

set (EXECUTABLE_OUTPUT_PATH "${CMAKE_SOURCE_DIR}/bin/")

if (CMAKE_BUILD_TOOL MATCHES "(msdev|devenv|nmake)")
	# Use the highest warning level for visual studio
	set (CMAKE_CXX_WARNING_LEVEL 4)
	if (CMAKE_CXX_FLAGS MATCHES "/W[0-4]")
		string (REGEX REPLACE "/W[0-4]" "/W4"
				CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
	else ()
		set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
	endif ()
endif ()

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

include_directories(../include)

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
    system
    ${Boost_LIBRARIES}
)

if (WIN32)
	set_target_properties(tests PROPERTIES COMPILE_FLAGS
						  "-DBOOST_ALL_NO_LIB -DBOOST_TEST_DYN_LINK
						  ${Boost_UNIT_TEST_FRAMEWORK_DYN_LINK}
						  ${Boost_LIB_DIAGNOSTIC_DEFINITIONS}")
endif()

add_test(sketch-3d_test ${CMAKE_SOURCE_DIR}/bin/tests)
