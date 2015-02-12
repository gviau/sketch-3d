cmake_minimum_required (VERSION 2.8)

project(Sample_BurningPaper)

set(EXECUTABLE_OUTPUT_PATH "${CMAKE_SOURCE_DIR}/bin/")

if (MSVC)
	configure_file(
		${SKETCH3D_TEMPLATES_PATH}/VisualStudioUserFile.vcxproj.user.in
		${CMAKE_CURRENT_BINARY_DIR}/Sample_BurningPaper.vcxproj.user
		@ONLY
	)
	configure_file(
		${SKETCH3D_TEMPLATES_PATH}/VisualStudioUserFile.vcproj.user.in
		${CMAKE_CURRENT_BINARY_DIR}/Sample_BurningPaper.vcproj.user
		@ONLY
	)
endif ()

set(SOURCE_FILES
	src/Main.cpp
)
include_directories(include)

include_directories(../../include)

link_directories(${CMAKE_SOURCE_DIR}/lib)

set(LIBRARIES
	${OPENGL_LIBRARIES}
	${ASSIMP_LIBRARY}
	${FreeImage_LIBRARIES}
	${FREETYPE_LIBRARIES}
)

if (WIN32)
	set (WIN32_EXECUTABLE "WIN32")
	set (LIBRARIES
		 ${LIBRARIES}
		 ${DirectX_LIBRARIES}
	)
else (WIN32)
	set (WIN32_EXEUTABLE "")
endif (WIN32)

add_executable(
	Sample_BurningPaper
	${WIN32_EXECUTABLE}
	${SOURCE_FILES}
	${HEADER_FILES}
)

target_link_libraries(
	Sample_BurningPaper
	math
	render
	system
	${LIBRARIES}
)