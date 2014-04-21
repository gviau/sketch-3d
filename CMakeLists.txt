cmake_minimum_required (VERSION 2.8)

set (
    CMAKE_MODULE_PATH
    ${CMAKE_MODULE_PATH}
    "${CMAKE_SOURCE_DIR}/CMakeModules/"
)

set (Boost_USE_STATIC_LIBS OFF)
set (Boost_USE_SHARED_LIBS ON)
set (Boost_USE_MULTITHREADED ON)
set (Boost_USE_STATIC_RUNTIME OFF)

find_package(
    Boost
    COMPONENTS
    unit_test_framework REQUIRED
)

message(STATUS "Boost found? ${Boost_FOUND}")
if (Boost_FOUND)
    message (STATUS "Boost include: ${Boost_INCLUDE_DIRS}")
    message (STATUS "Boost libs: ${Boost_LIBRARIES}")
    link_directories (${Boost_LIBRARY_DIRS})
else (Boost_FOUND)
    set (MISSING_MODULES ${MISSING_MODULES} "Boost")
endif (Boost_FOUND)

find_package(OpenGL)

message(STATUS "OpenGL found? ${OPENGL_FOUND}")
if (OPENGL_FOUND)
    message (STATUS "OpenGL include: ${OPENGL_INCLUDE_DIR}")
    message (STATUS "OpenGL libs: ${OPENGL_LIBRARIES}")
else (OPENGL_FOUND)
    set (MISSING_MODULES ${MISSING_MODULES} "OpenGL")
endif (OPENGL_FOUND)

find_package(SFML 2 COMPONENTS window REQUIRED)

message(STATUS "SFML found? ${SFML_FOUND}")
if (SFML_FOUND)
    message (STATUS "SFML include: ${SFML_INCLUDE_DIR}")
    message (STATUS "SFML libs: ${SFML_LIBRARIES}")
else (SFML_FOUND)
    set (MISSING_MODULES ${MISSING_MODULES} "SFML")
endif (SFML_FOUND)

if (MISSING_MODULES)
    message(FATAL_ERROR "Could generate all makefiles: missing modules: ${MISSING_MODULES}")
endif (MISSING_MODULES)

if (WIN32)
else (WIN32)
    add_definitions(-Wall -W -ansi)
    add_definitions(-Wno-long-long -Wno-comments)
endif (WIN32)

add_subdirectory (src)

enable_testing()
include(CTest)
add_subdirectory (tests)
