cmake_minimum_required (VERSION 2.8)

project (render)

set (LIBRARY_OUTPUT_PATH "${CMAKE_SOURCE_DIR}/lib/")
set(SRC_PATH "${CMAKE_SOURCE_DIR}/src/render")
set(HEADER_PATH "${CMAKE_SOURCE_DIR}/include/render")

set(SRC
	${SRC_PATH}/CompositeNode.cpp
	${SRC_PATH}/Mesh.cpp
	${SRC_PATH}/Node.cpp
	${SRC_PATH}/Renderer.cpp
    ${SRC_PATH}/RenderSystem.cpp
	${SRC_PATH}/SceneTree.cpp
)

set(HEADER
	${HEADER_PATH}/CompositeNode.h
	${HEADER_PATH}/Mesh.h
	${HEADER_PATH}/Node.h
	${HEADER_PATH}/RenderContext.h
	${HEADER_PATH}/Renderer.h
	${HEADER_PATH}/RenderSystem.h
	${HEADER_PATH}/ResourceManager.h
	${HEADER_PATH}/SceneTree.h
)
source_group("Source Files" FILES ${SRC})
source_group("Header Files" FILES ${HEADER})

if (WIN32)
	set(OPENGL_SRC,
		${SRC_PATH}/OpenGL/RenderSystemOpenGL.cpp
		${SRC_PATH}/OpenGL/Win32/RenderContextOpenGLWin32.cpp
	)
	
	set(OPENGL_HEADER,
		${HEADER_PATH}/OpenGL/RenderContextOpenGL.h
		${HEADER_PATH}/OpenGL/RenderSystemOpenGL.h
		${HEADER_PATH}/OpenGL/Win32/RenderContextOpenGLWin32.h
	)
	
	source_group("Source Files" FILES ${OPENGL_SRC})
	source_group("Header Files" FILES ${OPENGL_HEADER})
elseif (UNIX)
    set(OPENGL_SRC
        ${SRC_PATH}/OpenGL/RenderSystemOpenGL.cpp
        ${SRC_PATH}/OpenGL/Unix/RenderContextOpenGLUnix.cpp
    )

    set(OPENGL_HEADER
        ${HEADER_PATH}/OpenGL/RenderContextOpenGL.h
        ${HEADER_PATH}/OpenGL/RenderSystemOpenGL.h
        ${HEADER_PATH}/OpenGL/Unix/RenderContextOpenGLUnix.h
    )

endif ()

include_directories (../../include)

add_library(
    render
    STATIC
    ${SRC} ${OPENGL_SRC}
    ${HEADER} ${OPENGL_HEADER}
)

target_link_libraries(
    render
    math
    ${OPENGL_LIBRARIES}
)
