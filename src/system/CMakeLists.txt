cmake_minimum_required (VERSION 2.8)

project (system)

set(LIBRARY_OUTPUT_PATH "${CMAKE_SOURCE_DIR}/lib/")
set(SRC_PATH "${CMAKE_SOURCE_DIR}/src/system")
set(HEADER_PATH "${CMAKE_SOURCE_DIR}/include/system")

set (SRC
	 ${SRC_PATH}/Logger.cpp
	 ${SRC_PATH}/Platform.cpp
	 ${SRC_PATH}/Window.cpp
)

set (HEADER
	 ${HEADER_PATH}/Common.h
	 ${HEADER_PATH}/Logger.h
	 ${HEADER_PATH}/Platform.h
	 ${HEADER_PATH}/Window.h
	 ${HEADER_PATH}/WindowHandle.h
	 ${HEADER_PATH}/WindowImplementation.h
)
source_group("Source Files" FILES ${SRC})
source_group("Header Files" FILES ${HEADER})

if (WIN32)
	set (PLATFORM_SRC
		 ${SRC_PATH}/Win32/WindowImplementationWin32.cpp
	)
	
	set (PLATFORM_HEADER
		 ${HEADER_PATH}/Win32/WindowImplementationWin32.h
	)
	
	source_group("Source Files" FILES ${PLATFORM_SRC})
	source_group("Header Files" FILES ${PLATFORM_HEADER})
endif()

include_directories(../../include/)

add_library(
    system
    STATIC
    ${SRC} ${PLATFORM_SRC}
    ${HEADER} ${PLATFORM_HEADER}
)

target_link_libraries(
    system
)
