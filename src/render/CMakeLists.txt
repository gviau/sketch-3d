cmake_minimum_required (VERSION 2.8)

project (render)

set (LIBRARY_OUTPUT_PATH "${CMAKE_SOURCE_DIR}/lib/")
set(SRC_PATH "${CMAKE_SOURCE_DIR}/src/render")
set(HEADER_PATH "${CMAKE_SOURCE_DIR}/include/render")

set(SRC
	${SRC_PATH}/AnimationState.cpp
	${SRC_PATH}/BufferObject.cpp
	${SRC_PATH}/BufferObjectManager.cpp
	${SRC_PATH}/Material.cpp
	${SRC_PATH}/Mesh.cpp
	${SRC_PATH}/ModelManager.cpp
	${SRC_PATH}/Node.cpp
	${SRC_PATH}/Renderer.cpp
	${SRC_PATH}/RenderQueue.cpp
	${SRC_PATH}/RenderQueueItem.cpp
    ${SRC_PATH}/RenderSystem.cpp
	${SRC_PATH}/RenderTexture.cpp
	${SRC_PATH}/SceneTree.cpp
	${SRC_PATH}/Shader.cpp
	${SRC_PATH}/Skeleton.cpp
	${SRC_PATH}/SkinnedMesh.cpp
	${SRC_PATH}/Texture.cpp
	${SRC_PATH}/Texture2D.cpp
	${SRC_PATH}/Texture3D.cpp
	${SRC_PATH}/TextureManager.cpp
)

set(HEADER
	${HEADER_PATH}/AnimationState.h
	${HEADER_PATH}/BufferObject.h
	${HEADER_PATH}/BufferObjectManager.h
	${HEADER_PATH}/Material.h
	${HEADER_PATH}/Mesh.h
	${HEADER_PATH}/ModelManager.h
	${HEADER_PATH}/Node.h
	${HEADER_PATH}/RenderContext.h
	${HEADER_PATH}/Renderer.h
	${HEADER_PATH}/RenderQueue.h
	${HEADER_PATH}/RenderQueueItem.h
	${HEADER_PATH}/RenderSystem.h
	${HEADER_PATH}/RenderTexture.h
	${HEADER_PATH}/SceneTree.h
	${HEADER_PATH}/Shader.h
	${HEADER_PATH}/Skeleton.h
	${HEADER_PATH}/SkinnedMesh.h
	${HEADER_PATH}/Texture.h
	${HEADER_PATH}/Texture2D.h
	${HEADER_PATH}/Texture3D.h
	${HEADER_PATH}/TextureManager.h
)
source_group("Source Files" FILES ${SRC})
source_group("Header Files" FILES ${HEADER})

set(OPENGL_SRC
	${SRC_PATH}/OpenGL/BufferObjectManagerOpenGL.cpp
	${SRC_PATH}/OpenGL/BufferObjectOpenGL.cpp
	${SRC_PATH}/OpenGL/RenderSystemOpenGL.cpp
	${SRC_PATH}/OpenGL/RenderTextureOpenGL.cpp
	${SRC_PATH}/OpenGL/ShaderOpenGL.cpp
	${SRC_PATH}/OpenGL/Texture2DOpenGL.cpp
	${SRC_PATH}/OpenGL/Texture3DOpenGL.cpp
	${SRC_PATH}/OpenGL/glew.c
)

set(OPENGL_HEADER
	${HEADER_PATH}/OpenGL/BufferObjectManagerOpenGL.h
	${HEADER_PATH}/OpenGL/BufferObjectOpenGL.h
	${HEADER_PATH}/OpenGL/RenderContextOpenGL.h
	${HEADER_PATH}/OpenGL/RenderSystemOpenGL.h
	${HEADER_PATH}/OpenGL/RenderTextureOpenGL.h
	${HEADER_PATH}/OpenGL/ShaderOpenGL.h
	${HEADER_PATH}/OpenGL/Texture2DOpenGL.h
	${HEADER_PATH}/OpenGL/Texture3DOpenGL.h
)

if (WIN32)
	set(OPENGL_SRC
		${OPENGL_SRC}
		${SRC_PATH}/OpenGL/Win32/RenderContextOpenGLWin32.cpp
	)
	
	set(OPENGL_HEADER
		${OPENGL_HEADER}
		${HEADER_PATH}/OpenGL/Win32/RenderContextOpenGLWin32.h
	)
	
	set(DIRECT3D9_SRC
		${SRC_PATH}/Direct3D9/BufferObjectDirect3D9.cpp
		${SRC_PATH}/Direct3D9/BufferObjectManagerDirect3D9.cpp
		${SRC_PATH}/Direct3D9/RenderContextDirect3D9.cpp
		${SRC_PATH}/Direct3D9/RenderSystemDirect3D9.cpp
		${SRC_PATH}/Direct3D9/RenderTextureDirect3D9.cpp
		${SRC_PATH}/Direct3D9/ShaderDirect3D9.cpp
		${SRC_PATH}/Direct3D9/Texture2DDirect3D9.cpp
		${SRC_PATH}/Direct3D9/Texture3DDirect3D9.cpp
	)
	
	set(DIRECT3D9_HEADER
		${HEADER_PATH}/Direct3D9/BufferObjectDirect3D9.h
		${HEADER_PATH}/Direct3D9/BufferObjectManagerDirect3D9.h
		${HEADER_PATH}/Direct3D9/RenderContextDirect3D9.h
		${HEADER_PATH}/Direct3D9/RenderSystemDirect3D9.h
		${HEADER_PATH}/Direct3D9/RenderTextureDirect3D9.h
		${HEADER_PATH}/Direct3D9/ShaderDirect3D9.h
		${HEADER_PATH}/Direct3D9/Texture2DDirect3D9.h
		${HEADER_PATH}/Direct3D9/Texture3DDirect3D9.h
	)
	
	source_group("Source Files\\Direct3D9" FILES ${DIRECT3D9_SRC})
	source_group("Header Files\\Direct3D9" FILES ${DIRECT3D9_HEADER})
elseif (UNIX)
    set(OPENGL_SRC
        ${OPENGL_SRC}
        ${SRC_PATH}/OpenGL/Unix/RenderContextOpenGLUnix.cpp
    )

    set(OPENGL_HEADER
        ${OPENGL_HEADER}
        ${HEADER_PATH}/OpenGL/Unix/RenderContextOpenGLUnix.h
    )
endif ()

source_group("Source Files\\OpenGL" FILES ${OPENGL_SRC})
source_group("Header Files\\OpenGL" FILES ${OPENGL_HEADER})

include_directories (../../include)
include_directories (${ASSIMP_INCLUDE_DIR})
include_directories (${FreeImage_INCLUDE_DIRS})

if (WIN32)
	include_directories (${DirectX_INCLUDE_DIR})
endif ()

add_definitions(-DGLEW_BUILD)

add_library(
    render
    STATIC
    ${SRC} ${OPENGL_SRC} ${DIRECT3D9_SRC}
    ${HEADER} ${OPENGL_HEADER} ${DIRECT3D9_HEADER}
)

target_link_libraries(
    render
    math
    ${OPENGL_LIBRARIES}
)
