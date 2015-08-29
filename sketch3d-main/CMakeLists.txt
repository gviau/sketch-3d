cmake_minimum_required(VERSION 2.8)

project(sketch-3d)

set(LIBRARY_OUTPUT_PATH "${CMAKE_SOURCE_DIR}/lib/")

# Math files
set (MATH_SOURCE_FILES
	src/math/Complex.cpp
	src/math/Matrix3x3.cpp
	src/math/Matrix4x4.cpp
	src/math/Plane.cpp
	src/math/Quaternion.cpp
	src/math/Ray.cpp
	src/math/Sphere.cpp
	src/math/Vector2.cpp
	src/math/Vector3.cpp
	src/math/Vector4.cpp
)

set (MATH_HEADER_FILES
	include/math/Complex.h
	include/math/Constants.h
	include/math/Matrix3x3.h
	include/math/Matrix4x4.h
	include/math/Plane.h
	include/math/Quaternion.h
	include/math/Ray.h
	include/math/Sphere.h
	include/math/Vector2.h
	include/math/Vector3.h
	include/math/Vector4.h
)
source_group("Source Files\\math" FILES ${MATH_SOURCE_FILES})
source_group("Header Files\\math" FILES ${MATH_HEADER_FILES})

# Render files
set(RENDER_SOURCE_FILES
	src/render/Buffer.cpp
	src/render/HardwareResource.cpp
	src/render/HardwareResourceCreator.cpp
	src/render/RenderContext.cpp
	src/render/RenderDevice.cpp
	src/render/Renderer_Common.cpp
	src/render/Resource.cpp
	src/render/SamplerState.cpp
	src/render/RenderTarget.cpp
	src/render/Shader.cpp
	src/render/Texture.cpp
	src/render/TextureMap.cpp
	src/render/VertexFormat.cpp
)

set(RENDER_HEADER_FILES
	include/render/Buffer.h
	include/render/ConstantBuffers.h
	include/render/HardwareResource.h
	include/render/HardwareResourceCreator.h
	include/render/RenderContext.h
	include/render/RenderDevice.h
	include/render/RenderDevice_Common.h
	include/render/Renderer_Common.h
	include/render/RenderState.h
	include/render/RenderTarget.h
	include/render/Resource.h
	include/render/SamplerState.h
	include/render/Shader.h
	include/render/Texture.h
	include/render/TextureMap.h
	include/render/VertexFormat.h
)
source_group("Source Files\\render" FILES ${RENDER_SOURCE_FILES})
source_group("Header Files\\render" FILES ${RENDER_HEADER_FILES})

set(RENDER_OPENGL_SOURCE_FILES
	src/render/OpenGL/BufferOpenGL.cpp
	src/render/OpenGL/ErrorCheckingOpenGL.cpp
	src/render/OpenGL/glew.cpp
	src/render/OpenGL/HardwareResourceCreatorOpenGL.cpp
	src/render/OpenGL/RenderDeviceOpenGL.cpp
	src/render/OpenGL/RenderTargetOpenGL.cpp
	src/render/OpenGL/SamplerStateOpenGL.cpp
	src/render/OpenGL/ShaderOpenGL.cpp
	src/render/OpenGL/TextureOpenGL.cpp
)

set(RENDER_OPENGL_HEADER_FILES
	include/render/OpenGL/BufferOpenGL.h
	include/render/OpenGL/ErrorCheckingOpenGL.h
	include/render/OpenGL/HardwareResourceCreatorOpenGL.h
	include/render/OpenGL/RenderContextOpenGL.h
	include/render/OpenGL/RenderDeviceOpenGL.h
	include/render/OpenGL/RenderTargetOpenGL.h
	include/render/OpenGL/SamplerStateOpenGL.h
	include/render/OpenGL/ShaderOpenGL.h
	include/render/OpenGL/TextureOpenGL.h
)

if (WIN32)
	set(RENDER_OPENGL_SOURCE_FILES
		${RENDER_OPENGL_SOURCE_FILES}
		src/render/OpenGL/Win32/RenderContextOpenGLWin32.cpp
	)
	
	set(RENDER_OPENGL_HEADER_FILES
		${RENDER_OPENGL_HEADER_FILES}
		include/render/OpenGL/Win32/RenderContextOpenGLWin32.h
	)
	
	set(RENDER_DIRECT3D11_SOURCE_FILES
		src/render/Direct3D11/BufferDirect3D11.cpp
		src/render/Direct3D11/HardwareResourceCreatorDirect3D11.cpp
		src/render/Direct3D11/RenderContextDirect3D11.cpp
		src/render/Direct3D11/RenderDeviceDirect3D11.cpp
		src/render/Direct3D11/RenderTargetDirect3D11.cpp
		src/render/Direct3D11/SamplerStateDirect3D11.cpp
		src/render/Direct3D11/ShaderDirect3D11.cpp
		src/render/Direct3D11/TextureDirect3D11.cpp
	)
	
	set(RENDER_DIRECT3D11_HEADER_FILES
		include/render/Direct3D11/BufferDirect3D11.h
		include/render/Direct3D11/HardwareResourceCreatorDirect3D11.h
		include/render/Direct3D11/RenderContextDirect3D11.h
		include/render/Direct3D11/RenderDeviceDirect3D11.h
		include/render/Direct3D11/RenderTargetDirect3D11.h
		include/render/Direct3D11/SamplerStateDirect3D11.h
		include/render/Direct3D11/ShaderDirect3D11.h
		include/render/Direct3D11/TextureDirect3D11.h
	)
	
	source_group("Source Files\\render\\Direct3D11" FILES ${RENDER_DIRECT3D11_SOURCE_FILES})
	source_group("Header Files\\render\\Direct3D11" FILES ${RENDER_DIRECT3D11_HEADER_FILES})
elseif (UNIX)
    set(RENDER_OPENGL_SOURCE_FILES
        ${RENDER_OPENGL_SOURCE_FILES}
		src/render/OpenGL/Unix/RenderContextOpenGLUnix.cpp
    )

    set(RENDER_OPENGL_HEADER_FILES
        ${RENDER_OPENGL_HEADER_FILES}
		include/render/OpenGL/Unix/RenderContextOpenGLUnix.h
    )
endif ()
source_group("Source Files\\render\\OpenGL" FILES ${RENDER_OPENGL_SOURCE_FILES})
source_group("Header Files\\render\\OpenGL" FILES ${RENDER_OPENGL_HEADER_FILES})

# System files
set (SYSTEM_SOURCE_FILES
	 src/system/Logger.cpp
	 src/system/Platform.cpp
	 src/system/Utils.cpp
	 src/system/Window.cpp
     src/system/WindowEvent.cpp
     src/system/WindowImplementation.cpp
)

set (SYSTEM_HEADER_FILES
	 include/system/Common.h
	 include/system/Logger.h
	 include/system/Platform.h
	 include/system/Utils.h
	 include/system/Window.h
     include/system/WindowEvent.h
	 include/system/WindowHandle.h
	 include/system/WindowImplementation.h
)
source_group("Source Files\\system" FILES ${SYSTEM_SOURCE_FILES})
source_group("Header Files\\system" FILES ${SYSTEM_HEADER_FILES})

if (WIN32)
	set (SYSTEM_PLATFORM_SOURCE_FILES
		 src/system/Win32/WindowImplementationWin32.cpp
	)
	
	set (SYSTEM_PLATFORM_HEADER_FILES
		 include/system/Win32/WindowImplementationWin32.h
	)
	
	source_group("Source Files\\system\\Win32" FILES ${SYSTEM_PLATFORM_SOURCE_FILES})
	source_group("Header Files\\system\\Win32" FILES ${SYSTEM_PLATFORM_HEADER_FILES})
elseif (UNIX)
    set(SYSTEM_PLATFORM_SOURCE_FILES
        src/system/Unix/WindowImplementationUnix.cpp
    )

    set(SYSTEM_PLATFORM_HEADER_FILES
        include/system/Unix/WindowImplementationUnix.h
    )

    source_group("Source Files\\system\\Unix" FILES ${SYSTEM_PLATFORM_SOURCE_FILES})
    source_group("Header Files\\system\\Unix" FILES ${SYSTEM_PLATFORM_HEADER_FILES})
endif()

add_definitions(-DSKETCH_3D_NONCLIENT_BUILD)

include_directories(include)
include_directories (${ASSIMP_INCLUDE_DIR})
include_directories (${FreeImage_INCLUDE_DIRS})
include_directories (${FREETYPE_INCLUDE_DIRS})

if (WIN32)
	include_directories (${DirectX_INCLUDE_DIR})
endif ()

add_definitions(-DGLEW_BUILD)

if (NOT SKETCH_3D_BUILD_STATIC)
	set (LIBRARY_TYPE SHARED)
else ()
	set (LIBRARY_TYPE STATIC)
endif ()

add_library(
    sketch-3d
	
    ${LIBRARY_TYPE}

    ${MATH_SOURCE_FILES}
	
	${RENDER_SOURCE_FILES}
	${RENDER_OPENGL_SOURCE_FILES}
	${RENDER_DIRECT3D11_SOURCE_FILES}
	
	${SYSTEM_SOURCE_FILES}
	${SYSTEM_PLATFORM_SOURCE_FILES}
	
	
    ${MATH_HEADER_FILES}
	
	${RENDER_HEADER_FILES}
	${RENDER_OPENGL_HEADER_FILES}
	${RENDER_DIRECT3D9_HEADER_FILES}
	${RENDER_DIRECT3D11_HEADER_FILES}
	
	${SYSTEM_HEADER_FILES}
	${SYSTEM_PLATFORM_HEADER_FILES}
)

target_link_libraries(
    sketch-3d

	${ASSIMP_LIBRARY}
	${DirectX_LIBRARY}
	${DirectX_D3DX9_LIBRARY}
	${FreeImage_LIBRARIES}
	${FREETYPE_LIBRARIES}
	${OIS_LIBRARY}
    ${OPENGL_LIBRARIES}
    ${X11_LIBRARIES}
)

# Add a bunch of post build events to copy the required files in the bin folder
if (NOT SKETCH_3D_BUILD_STATIC)
	add_custom_command(TARGET sketch-3d POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_if_different
		"${CMAKE_SOURCE_DIR}/lib/$<CONFIGURATION>/sketch-3d.dll"
		"${CMAKE_SOURCE_DIR}/bin/$<CONFIGURATION>/"
	)
endif ()

if (WIN32)
	add_custom_command(TARGET sketch-3d POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_if_different
		"${ASSIMP_INCLUDE_DIR}/../bin/x86/Assimp32.dll"
		"${CMAKE_SOURCE_DIR}/bin/$<CONFIGURATION>/"
	)
	
	add_custom_command(TARGET sketch-3d POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_if_different
		"${FreeImage_INCLUDE_DIRS}/../FreeImage.dll"
		"${CMAKE_SOURCE_DIR}/bin/$<CONFIGURATION>/"
	)
endif ()