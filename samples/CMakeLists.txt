cmake_minimum_required (VERSION 2.8)

include (${PROJECT_SOURCE_DIR}/CMakeModules/Macros.cmake)

add_subdirectory(Bokeh)
add_subdirectory(BRDF)
add_subdirectory(BurningPaper)
add_subdirectory(CelShading)
add_subdirectory(DeferredShading)
add_subdirectory(FrustumCullingInstancing)
add_subdirectory(FrustumCullingStaticBatching)
add_subdirectory(InteractiveWater)
add_subdirectory(NormalMapping)
add_subdirectory(ParallaxMapping)
add_subdirectory(Shadow)
add_subdirectory(SkeletalAnimation)
add_subdirectory(SponzaDemo)
add_subdirectory(SSAO)
add_subdirectory(TessendorfOcean)

if (WIN32)
	execute_process(COMMAND ${CMAKE_COMMAND} -E copy_directory
		"${CMAKE_CURRENT_SOURCE_DIR}/Media/"
		"${CMAKE_SOURCE_DIR}/bin/Debug/Media"
	)

	execute_process(COMMAND ${CMAKE_COMMAND} -E copy_directory
		"${CMAKE_CURRENT_SOURCE_DIR}/Shaders/"
		"${CMAKE_SOURCE_DIR}/bin/Debug/Shaders"
	)
	
	execute_process(COMMAND ${CMAKE_COMMAND} -E copy_directory
		"${CMAKE_CURRENT_SOURCE_DIR}/Media/"
		"${CMAKE_SOURCE_DIR}/bin/Release/Media"
	)

	execute_process(COMMAND ${CMAKE_COMMAND} -E copy_directory
		"${CMAKE_CURRENT_SOURCE_DIR}/Shaders/"
		"${CMAKE_SOURCE_DIR}/bin/Release/Shaders"
	)
else ()
	execute_process(COMMAND ${CMAKE_COMMAND} -E copy_directory
		"${CMAKE_CURRENT_SOURCE_DIR}/Media/"
		"${CMAKE_SOURCE_DIR}/bin/Media"
	)

	execute_process(COMMAND ${CMAKE_COMMAND} -E copy_directory
		"${CMAKE_CURRENT_SOURCE_DIR}/Shaders/"
		"${CMAKE_SOURCE_DIR}/bin/Shaders"
	)
endif ()