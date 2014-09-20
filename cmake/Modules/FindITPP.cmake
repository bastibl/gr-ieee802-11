# - Find ITPP
# Find the native ITPP includes and library
# This module defines
#  ITPP_INCLUDE_DIR, where to find itpp/itbase.h, etc.
#  ITPP_LIBRARIES, the libraries needed to use ITPP.
#  ITPP_FOUND, If false, do not try to use ITPP.
# also defined, but not for general use are
#  ITPP_LIBRARY, where to find the ITPP library.

include(FindPkgConfig)
pkg_check_modules(PC_ITPP itpp)

find_path(ITPP_INCLUDE_DIR
  NAMES itpp/itbase.h
  HINTS ${PC_ITPP_INCLUDEDIR}/include
  ${ITPP_DIR}/include
  /usr/pkgs64/include
  /usr/include
  /usr/local/include
)
find_library(ITPP_LIBRARY_NORMAL
  NAMES itpp
  HINTS ${PC_ITPP_LIBDIR}
  PATHS ${ITPP_DIR}/libs
  "${ITPP_DIR}\\win32\\lib"
  /usr/pkgs64/lib
  /usr/lib64
  /usr/lib
  /usr/local/lib
)
find_library(ITPP_LIBRARY_DEBUG
  NAMES itpp_debug
  HINTS ${PC_ITPP_LIBDIR}
  PATHS ${ITPP_DIR}/libs
  "${ITPP_DIR}\\win32\\lib"
  /usr/pkgs64/lib
  /usr/lib64
  /usr/lib
  /usr/local/lib
)

if (ITPP_LIBRARY_NORMAL OR ITPP_LIBRARY_DEBUG)
  if (CMAKE_BUILD_TYPE STREQUAL "Debug" AND ITPP_LIBRARY_DEBUG)
    SET (ITPP_LIBRARY ${ITPP_LIBRARY_DEBUG})
  else (CMAKE_BUILD_TYPE STREQUAL "Debug" AND ITPP_LIBRARY_DEBUG)
    SET (ITPP_LIBRARY ${ITPP_LIBRARY_NORMAL})
  endif (CMAKE_BUILD_TYPE STREQUAL "Debug" AND ITPP_LIBRARY_DEBUG)
endif (ITPP_LIBRARY_NORMAL OR ITPP_LIBRARY_DEBUG)

if (ITPP_LIBRARY AND ITPP_INCLUDE_DIR)
  SET(ITPP_LIBRARIES ${ITPP_LIBRARY})
  SET(ITPP_FOUND "YES")
else (ITPP_LIBRARY AND ITPP_INCLUDE_DIR)
  SET(ITPP_FOUND "NO")
endif (ITPP_LIBRARY AND ITPP_INCLUDE_DIR)

if (ITPP_FOUND)
  if (NOT ITPP_FIND_QUIETLY)
    MESSAGE(STATUS "Found ITPP: ${ITPP_LIBRARIES}")
  endif (NOT ITPP_FIND_QUIETLY)
else (ITPP_FOUND)
  if (ITPP_FIND_REQUIRED)
    MESSAGE(FATAL_ERROR "Could not find ITPP library")
  endif (ITPP_FIND_REQUIRED)
endif (ITPP_FOUND)

mark_as_advanced(
  ITPP_LIBRARY
  ITPP_INCLUDE_DIR
)
