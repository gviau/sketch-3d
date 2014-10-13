cmake_minimum_required (VERSION 2.8)

project(Sample_Shadow)

set(EXECUTABLE_OUTPUT_PATH "${CMAKE_SOURCE_DIR}/bin/")

add_custom_command(
	TARGET
	Sample_Shadow
	POST_BUILD WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/Debug/"
)

set(HEADER_FILES include/Shadow.h)
set(SOURCE_FILES src/Shadow.cpp)
include_directories(include)

include_directories(../../include)

link_directories(${CMAKE_SOURCE_DIR}/lib)

if (WIN32)
	set (WIN32_EXECUTABLE "WIN32")
else (WIN32)
	set (WIN32_EXEUTABLE "")
endif (WIN32)

add_executable(
	Sample_Shadow
	${WIN32_EXECUTABLE}
	${SOURCE_FILES}
	${HEADER_FILES}
)

target_link_libraries(
	Sample_Shadow
	math
	render
	system
	${OPENGL_LIBRARIES}
	${ASSIMP_LIBRARY}
	${FreeImage_LIBRARIES}
)