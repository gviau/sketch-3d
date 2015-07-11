include (CMakeParseArguments)

# Add a new sample to the build
# ex: sketch3d_add_sample(Sample_SponzaDemo
function (sketch3d_add_sample target)
	project(${target})
	
    # parse the arguments
    cmake_parse_arguments(THIS "" "" "SOURCES;HEADERS" ${ARGN})

	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY  "${CMAKE_SOURCE_DIR}/bin/")

	if (MSVC)
		configure_file(
			${SKETCH3D_TEMPLATES_PATH}/VisualStudioUserFile.vcxproj.user.in
			${CMAKE_CURRENT_BINARY_DIR}/${target}.vcxproj.user
			@ONLY
		)
		configure_file(
			${SKETCH3D_TEMPLATES_PATH}/VisualStudioUserFile.vcproj.user.in
			${CMAKE_CURRENT_BINARY_DIR}/${target}.vcproj.user
			@ONLY
		)
	endif ()
	
	include_directories(include)

	include_directories(../../sketch3d-main/include)

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
			 ${DirectX_D3D11_LIBRARIES}
		)
	else (WIN32)
		set (WIN32_EXEUTABLE "")
	endif (WIN32)

	add_executable(
		${target}
		${WIN32_EXECUTABLE}
		${THIS_SOURCES}
		${THIS_HEADERS}
	)

	set (LINK_LIBRARIES
		sketch-3d
		${LIBRARIES}
	)

	if (${OIS_FOUND})
		include_directories(${OIS_INCLUDE_DIR})
		add_definitions(-DOIS_AVAILABLE)
		
		set (LINK_LIBRARIES
			${LINK_LIBRARIES}
			debug ${OIS_LIBRARY_DEBUG} optimized ${OIS_LIBRARY}
		)
	endif ()

	target_link_libraries(
		${target}
		${LINK_LIBRARIES}
	)
endfunction ()