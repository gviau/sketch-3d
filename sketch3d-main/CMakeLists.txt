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
	src/render/AnimationState.cpp
	src/render/BufferObject.cpp
	src/render/BufferObjectManager.cpp
	src/render/Material.cpp
	src/render/Mesh.cpp
	src/render/ModelManager.cpp
	src/render/Node.cpp
	src/render/RenderContext.cpp
	src/render/Renderer.cpp
	src/render/Renderer_Common.cpp
	src/render/RenderQueue.cpp
	src/render/RenderQueueItem.cpp
	src/render/RenderStateCache.cpp
    src/render/RenderSystem.cpp
	src/render/RenderTexture.cpp
	src/render/SceneTree.cpp
	src/render/Shader.cpp
	src/render/Skeleton.cpp
	src/render/SkinnedMesh.cpp
	src/render/Text.cpp
	src/render/Texture.cpp
	src/render/Texture2D.cpp
	src/render/Texture3D.cpp
	src/render/TextureManager.cpp
)

set(RENDER_HEADER_FILES
	include/render/AnimationState.h
	include/render/BufferObject.h
	include/render/BufferObjectManager.h
	include/render/Material.h
	include/render/Mesh.h
	include/render/ModelManager.h
	include/render/Node.h
	include/render/RenderContext.h
	include/render/Renderer.h
	include/render/Renderer_Common.h
	include/render/RenderQueue.h
	include/render/RenderQueueItem.h
	include/render/RenderState.h
	include/render/RenderStateCache.h
	include/render/RenderSystem.h
	include/render/RenderTexture.h
	include/render/SceneTree.h
	include/render/Shader.h
	include/render/Skeleton.h
	include/render/SkinnedMesh.h
	include/render/Text.h
	include/render/Texture.h
	include/render/Texture2D.h
	include/render/Texture3D.h
	include/render/TextureManager.h
)
source_group("Source Files\\render" FILES ${RENDER_SOURCE_FILES})
source_group("Header Files\\render" FILES ${RENDER_HEADER_FILES})

set(RENDER_OPENGL_SOURCE_FILES
	src/render/OpenGL/BufferObjectManagerOpenGL.cpp
	src/render/OpenGL/BufferObjectOpenGL.cpp
	src/render/OpenGL/RenderStateCacheOpenGL.cpp
	src/render/OpenGL/RenderSystemOpenGL.cpp
	src/render/OpenGL/RenderTextureOpenGL.cpp
	src/render/OpenGL/ShaderOpenGL.cpp
	src/render/OpenGL/Texture2DOpenGL.cpp
	src/render/OpenGL/Texture3DOpenGL.cpp
	src/render/OpenGL/glew.c
)

set(RENDER_OPENGL_HEADER_FILES
	include/render/OpenGL/BufferObjectManagerOpenGL.h
	include/render/OpenGL/BufferObjectOpenGL.h
	include/render/OpenGL/RenderContextOpenGL.h
	include/render/OpenGL/RenderStateCacheOpenGL.h
	include/render/OpenGL/RenderSystemOpenGL.h
	include/render/OpenGL/RenderTextureOpenGL.h
	include/render/OpenGL/ShaderOpenGL.h
	include/render/OpenGL/Texture2DOpenGL.h
	include/render/OpenGL/Texture3DOpenGL.h
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
	
	set(RENDER_DIRECT3D9_SOURCE_FILES
		src/render/Direct3D9/BufferObjectDirect3D9.cpp
		src/render/Direct3D9/BufferObjectManagerDirect3D9.cpp
		src/render/Direct3D9/RenderContextDirect3D9.cpp
		src/render/Direct3D9/RenderStateCacheDirect3D9.cpp
		src/render/Direct3D9/RenderSystemDirect3D9.cpp
		src/render/Direct3D9/RenderTextureDirect3D9.cpp
		src/render/Direct3D9/ShaderDirect3D9.cpp
		src/render/Direct3D9/Texture2DDirect3D9.cpp
		src/render/Direct3D9/Texture3DDirect3D9.cpp
	)
	
	set(RENDER_DIRECT3D9_HEADER_FILES
		include/render/Direct3D9/BufferObjectDirect3D9.h
		include/render/Direct3D9/BufferObjectManagerDirect3D9.h
		include/render/Direct3D9/RenderContextDirect3D9.h
		include/render/Direct3D9/RenderStateCacheDirect3D9.h
		include/render/Direct3D9/RenderSystemDirect3D9.h
		include/render/Direct3D9/RenderTextureDirect3D9.h
		include/render/Direct3D9/ShaderDirect3D9.h
		include/render/Direct3D9/Texture2DDirect3D9.h
		include/render/Direct3D9/Texture3DDirect3D9.h
	)
	
	source_group("Source Files\\render\\Direct3D9" FILES ${RENDER_DIRECT3D9_SOURCE_FILES})
	source_group("Header Files\\render\\Direct3D9" FILES ${RENDER_DIRECT3D9_HEADER_FILES})
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
	${RENDER_DIRECT3D9_SOURCE_FILES}
	
	${SYSTEM_SOURCE_FILES}
	${SYSTEM_PLATFORM_SOURCE_FILES}
	
    ${MATH_HEADER_FILES}
	
	${RENDER_HEADER_FILES}
	${RENDER_OPENGL_HEADER_FILES}
	${RENDER_DIRECT3D9_HEADER_FILES}
	
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