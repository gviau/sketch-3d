cmake_minimum_required (VERSION 2.8)

project(Sample_TessendorfOcean)

set(EXECUTABLE_OUTPUT_PATH "${CMAKE_SOURCE_DIR}/bin/")

if (MSVC)
	configure_file(
		${SKETCH3D_TEMPLATES_PATH}/VisualStudioUserFile.vcxproj.user.in
		${CMAKE_CURRENT_BINARY_DIR}/Sample_TessendorfOcean.vcxproj.user
		@ONLY
	)
	configure_file(
		${SKETCH3D_TEMPLATES_PATH}/VisualStudioUserFile.vcproj.user.in
		${CMAKE_CURRENT_BINARY_DIR}/Sample_TessendorfOcean.vcproj.user
		@ONLY
	)
endif ()

set(HEADER_FILES include/Ocean.h)
set(SOURCE_FILES
	src/Ocean.cpp
	src/Main.cpp
)
include_directories(include)

include_directories(../../include)

link_directories(${CMAKE_SOURCE_DIR}/lib)

set(LIBRARIES
	${OPENGL_LIBRARIES}
	${ASSIMP_LIBRARY}
	${FreeImage_LIBRARIES}
)

if (UNIX)
    set (LIBRARIES
         ${LIBRARIES}
         ${CMAKE_THREAD_LIBS_INIT}
    )
endif ()

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
	Sample_TessendorfOcean
	${WIN32_EXECUTABLE}
	${SOURCE_FILES}
	${HEADER_FILES}
)

set (LINK_LIBRARIES
	math
	render
	system
	${LIBRARIES}
)

if (${OIS_FOUND})
	include_directories(${OIS_INCLUDE_DIR})
	add_definitions(-DOIS_AVAILABLE)
	
	set (LINK_LIBRARIES
		${LINK_LIBRARIES}
		${OIS_LIBRARY_DEBUG}
	)
endif ()

target_link_libraries(
	Sample_TessendorfOcean
	${LINK_LIBRARIES}
)
