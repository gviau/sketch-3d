# - Try to find OIS
# Once done, this will define
#
#  OIS_FOUND - system has OIS
#  OIS_INCLUDE_DIR - the OIS include directories 
#  OIS_LIBRARY - link these to use OIS in Release
#  OIS_LIBRARY_DEBUG - link these to use OIS in Debug
#  OIS_BINARY_REL / OIS_BINARY_DBG - DLL names (windows only)
set(_ois_INCLUDE_SEARCH_DIRS_SYSTEM
    C:/ois/includes
    C:/ois
    "$ENV{ProgramFiles}/ois/includes"
    "$ENV{ProgramFiles}/ois"
    /sw/local/include
  )

set(_ois_LIB_SEARCH_DIRS_SYSTEM
	C:/ois/lib/
    "$ENV{ProgramFiles}/ois/lib"
    /sw/local/lib
  )

FIND_PATH(OIS_INCLUDE_DIR OIS.h
    $ENV{OISDIR}/include
    $ENV{OISDIR}
    $ENV{OISDIR}/..
    /usr/local/include/ois
    /usr/local/include
    /usr/include/ois
    /usr/include
    /sw/include/ois # Fink
    /sw/include
    /opt/local/include/ois # DarwinPorts
    /opt/local/include
    /opt/csw/include/ois # Blastwave
    /opt/csw/include
    /opt/include/ois
    /opt/include
    ${_ois_INCLUDE_SEARCH_DIRS_SYSTEM}
    )

FIND_LIBRARY(OIS_LIBRARY
    NAMES OIS_static
    PATHS
    $ENV{OISDIR}/lib
    /usr/local/lib
    /usr/lib
    /sw/lib
    /opt/local/lib
    /opt/csw/lib
    /opt/lib
    ${_ois_LIB_SEARCH_DIRS_SYSTEM}
    )

FIND_LIBRARY(OIS_LIBRARY_DEBUG
    NAMES OIS_static_d
    PATHS
    $ENV{OISDIR}/lib
    /usr/local/lib
    /usr/lib
    /sw/lib
    /opt/local/lib
    /opt/csw/lib
    /opt/lib
    ${_ois_LIB_SEARCH_DIRS_SYSTEM}
    )

SET(OIS_FOUND FALSE)
IF(OIS_INCLUDE_DIR AND OIS_LIBRARY)
    SET(OIS_FOUND TRUE)
    SET(OIS_LIBRARY ${OIS_LIBRARY})
ENDIF(OIS_INCLUDE_DIR AND OIS_LIBRARY)

if(OIS_FIND_REQUIRED AND NOT (OIS_LIBRARY AND OIS_LIBRARY_DEBUG AND OIS_INCLUDE_DIR))
    message(FATAL_ERROR "Could not find ois")
ENDIF(OIS_FIND_REQUIRED AND NOT (OIS_LIBRARY AND OIS_LIBRARY_DEBUG AND OIS_INCLUDE_DIR))

mark_as_advanced(OIS_LIBRARY OIS_LIBRARY_DEBUG OIS_INCLUDE_DIR)